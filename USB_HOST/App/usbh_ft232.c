#include "usbh_ft232.h"
#include "usbh_core.h"

typedef enum {
    FT232_INIT = 0,
    FT232_RESET,
    FT232_SET_BAUDRATE,
    FT232_SET_DATA,
    FT232_IDLE,
    FT232_SYNC_DISCARD,
    FT232_GET_DATA,
} FT232_StateTypeDef;

typedef struct {
    uint8_t  InPipe;
    uint8_t  OutPipe;
    uint8_t  InEp;
    uint8_t  OutEp;
    uint16_t InEpSize;
    uint16_t OutEpSize;
    FT232_StateTypeDef state;
    FT232_ConfigTypeDef config;
} FT232_HandleTypeDef;

static USBH_StatusTypeDef FT232_Init(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef FT232_DeInit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef FT232_Process(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef FT232_SOFProcess(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef FT232_Requests(USBH_HandleTypeDef *phost);

/* 内部函数声明 */
static USBH_StatusTypeDef FT232_Reset(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef FT232_SetBaudRate(USBH_HandleTypeDef *phost, uint32_t baudrate);
static USBH_StatusTypeDef FT232_SetDataCharacteristics(USBH_HandleTypeDef *phost, uint8_t databits, uint8_t stopbits, uint8_t parity);
static uint32_t FT232_CalcDivisor(uint32_t baudrate);

USBH_ClassTypeDef FT232_Class = {
    "FT232",
    0xFF, // Vendor Specific
    FT232_Init,
    FT232_DeInit,
    FT232_Requests,
    FT232_Process,
    FT232_SOFProcess,
    NULL
};

static USBH_StatusTypeDef FT232_Init(USBH_HandleTypeDef *phost)
{
    USBH_StatusTypeDef status = USBH_OK;
    FT232_HandleTypeDef *ft232 = (FT232_HandleTypeDef *)phost->pActiveClass->pData;
    
    USBH_UsrLog("FT232_Init: VID=0x%04X, PID=0x%04X", 
                phost->device.DevDesc.idVendor, phost->device.DevDesc.idProduct);
    
    /* 检查是否是FT232设备 */
    if (phost->device.DevDesc.idVendor != FT232_VID || 
        phost->device.DevDesc.idProduct != FT232_PID)
    {
        USBH_UsrLog("FT232_Init: Not FT232 device, returning FAIL");
        return USBH_FAIL;
    }
    
    USBH_UsrLog("FT232_Init: FT232 device detected, initializing...");
    
    if(ft232 == NULL)
    {
        ft232 = (FT232_HandleTypeDef *)USBH_malloc(sizeof(FT232_HandleTypeDef));
        if(ft232 == NULL)
        {
            USBH_UsrLog("FT232_Init: Memory allocation failed");
            return USBH_FAIL;
        }
        phost->pActiveClass->pData = ft232;
        ft232->state = FT232_INIT;
        
        /* 设置默认串口配置：9600, 7N1, 偶校验 */
        ft232->config.baudrate = 9600;
        ft232->config.databits = 7;
        ft232->config.stopbits = 1;
        ft232->config.parity = 2; // 偶校验
    }
    
    // 查找FT232的接口
    uint8_t interface = USBH_FindInterface(phost, 0xFF, 0xFF, 0xFF);
    if(interface == 0xFF)
    {
        USBH_UsrLog("FT232_Init: Interface not found");
        return USBH_FAIL;
    }
    
    USBH_UsrLog("FT232_Init: Found interface %d", interface);
    
    // 选择接口
    status = USBH_SelectInterface(phost, interface);
    if(status != USBH_OK)
    {
        USBH_UsrLog("FT232_Init: Interface selection failed");
        return status;
    }
    
    // 获取端点信息
    USBH_InterfaceDescTypeDef *itf = &phost->device.CfgDesc.Itf_Desc[interface];
    for(uint8_t i = 0; i < itf->bNumEndpoints; i++)
    {
        if(itf->Ep_Desc[i].bEndpointAddress & 0x80)
        {
            ft232->InEp = itf->Ep_Desc[i].bEndpointAddress;
            ft232->InEpSize = itf->Ep_Desc[i].wMaxPacketSize;
            USBH_UsrLog("FT232_Init: IN EP=0x%02X, Size=%d", ft232->InEp, ft232->InEpSize);
        }
        else
        {
            ft232->OutEp = itf->Ep_Desc[i].bEndpointAddress;
            ft232->OutEpSize = itf->Ep_Desc[i].wMaxPacketSize;
            USBH_UsrLog("FT232_Init: OUT EP=0x%02X, Size=%d", ft232->OutEp, ft232->OutEpSize);
        }
    }
    
    // 打开管道
    ft232->InPipe = USBH_AllocPipe(phost, ft232->InEp);
    ft232->OutPipe = USBH_AllocPipe(phost, ft232->OutEp);
    
    // 配置管道
    USBH_OpenPipe(phost, ft232->InPipe, ft232->InEp, phost->device.address, 
                 phost->device.speed, USB_EP_TYPE_BULK, ft232->InEpSize);
    USBH_OpenPipe(phost, ft232->OutPipe, ft232->OutEp, phost->device.address, 
                 phost->device.speed, USB_EP_TYPE_BULK, ft232->OutEpSize);
    
    USBH_UsrLog("FT232_Init: Initialization completed successfully");
    return USBH_OK;
}

static USBH_StatusTypeDef FT232_DeInit(USBH_HandleTypeDef *phost)
{
    FT232_HandleTypeDef *ft232 = (FT232_HandleTypeDef *)phost->pActiveClass->pData;
    
    USBH_UsrLog("FT232_DeInit: Deinitializing...");
    
    if(ft232 != NULL)
    {
        if(ft232->InPipe)
        {
            USBH_ClosePipe(phost, ft232->InPipe);
            USBH_FreePipe(phost, ft232->InPipe);
        }
        
        if(ft232->OutPipe)
        {
            USBH_ClosePipe(phost, ft232->OutPipe);
            USBH_FreePipe(phost, ft232->OutPipe);
        }
        
        USBH_free(ft232);
        phost->pActiveClass->pData = NULL;
    }
    
    return USBH_OK;
}

static USBH_StatusTypeDef FT232_Process(USBH_HandleTypeDef *phost)
{
    USBH_StatusTypeDef status = USBH_OK;
    FT232_HandleTypeDef *ft232 = (FT232_HandleTypeDef *)phost->pActiveClass->pData;
    
    switch(ft232->state)
    {
        case FT232_INIT:
            USBH_UsrLog("FT232_Process: Starting configuration sequence");
            ft232->state = FT232_RESET;
            status = USBH_BUSY;
            break;
            
        case FT232_RESET:
            status = FT232_Reset(phost);
            if(status == USBH_OK)
            {
                USBH_UsrLog("FT232_Process: Reset completed, setting baud rate");
                ft232->state = FT232_SET_BAUDRATE;
            }
            break;
            
        case FT232_SET_BAUDRATE:
            status = FT232_SetBaudRate(phost, ft232->config.baudrate);
            if(status == USBH_OK)
            {
                USBH_UsrLog("FT232_Process: Baud rate set, configuring data format");
                ft232->state = FT232_SET_DATA;
            }
            break;
            
        case FT232_SET_DATA:
            status = FT232_SetDataCharacteristics(phost, ft232->config.databits, 
                                                 ft232->config.stopbits, ft232->config.parity);
            if(status == USBH_OK)
            {
                USBH_UsrLog("FT232_Process: Configuration completed successfully");
                ft232->state = FT232_IDLE;
                phost->pUser(phost, HOST_USER_CLASS_ACTIVE);
            }
            break;
            
        case FT232_IDLE:
            status = USBH_OK;
            break;
            
        default:
            status = USBH_OK;
            break;
    }
    
    return status;
}

static USBH_StatusTypeDef FT232_SOFProcess(USBH_HandleTypeDef *phost)
{
    return USBH_OK;
}

static USBH_StatusTypeDef FT232_Requests(USBH_HandleTypeDef *phost)
{
    USBH_UsrLog("FT232_Requests: Called, returning OK");
    return USBH_OK;
}

/* FT232复位函数 */
static USBH_StatusTypeDef FT232_Reset(USBH_HandleTypeDef *phost)
{
    USBH_StatusTypeDef status = USBH_BUSY;
    
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
    phost->Control.setup.b.bRequest = FTDI_SIO_RESET;
    phost->Control.setup.b.wValue.w = 0;
    phost->Control.setup.b.wIndex.w = 0;
    phost->Control.setup.b.wLength.w = 0;
    
    status = USBH_CtlReq(phost, NULL, 0);
    
    if(status == USBH_OK)
    {
        USBH_UsrLog("FT232_Reset: Reset command sent successfully");
        USBH_Delay(50); // 等待复位完成
    }
    
    return status;
}

/* FT232设置波特率函数 */
static USBH_StatusTypeDef FT232_SetBaudRate(USBH_HandleTypeDef *phost, uint32_t baudrate)
{
    USBH_StatusTypeDef status = USBH_BUSY;
    uint32_t divisor = FT232_CalcDivisor(baudrate);
    
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
    phost->Control.setup.b.bRequest = FTDI_SIO_SET_BAUD_RATE;
    phost->Control.setup.b.wValue.w = (uint16_t)(divisor & 0xFFFF);
    phost->Control.setup.b.wIndex.w = (uint16_t)((divisor >> 16) & 0xFFFF);
    phost->Control.setup.b.wLength.w = 0;
    
    status = USBH_CtlReq(phost, NULL, 0);
    
    if(status == USBH_OK)
    {
        USBH_UsrLog("FT232_SetBaudRate: Baud rate %lu set successfully", baudrate);
    }
    
    return status;
}

/* FT232设置数据特性函数 */
static USBH_StatusTypeDef FT232_SetDataCharacteristics(USBH_HandleTypeDef *phost, uint8_t databits, uint8_t stopbits, uint8_t parity)
{
    USBH_StatusTypeDef status = USBH_BUSY;
    uint16_t value = 0;
    
    /* 设置数据位 */
    value |= FTDI_SIO_SET_DATA_BITS(databits);
    
    /* 设置停止位 */
    if(stopbits == 1)
        value |= FTDI_SIO_SET_DATA_STOP_BITS_1;
    else if(stopbits == 2)
        value |= FTDI_SIO_SET_DATA_STOP_BITS_2;
    
    /* 设置校验位 */
    switch(parity)
    {
        case 0: value |= FTDI_SIO_SET_DATA_PARITY_NONE; break;
        case 1: value |= FTDI_SIO_SET_DATA_PARITY_ODD; break;
        case 2: value |= FTDI_SIO_SET_DATA_PARITY_EVEN; break;
        default: value |= FTDI_SIO_SET_DATA_PARITY_NONE; break;
    }
    
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
    phost->Control.setup.b.bRequest = FTDI_SIO_SET_DATA;
    phost->Control.setup.b.wValue.w = value;
    phost->Control.setup.b.wIndex.w = 0;
    phost->Control.setup.b.wLength.w = 0;
    
    status = USBH_CtlReq(phost, NULL, 0);
    
    if(status == USBH_OK)
    {
        USBH_UsrLog("FT232_SetDataCharacteristics: %d%c%d configured successfully", 
                   databits, (parity == 0) ? 'N' : (parity == 1) ? 'O' : 'E', stopbits);
    }
    
    return status;
}

/* 计算FT232波特率分频器 */
static uint32_t FT232_CalcDivisor(uint32_t baudrate)
{
    uint32_t divisor;
    
    if(baudrate == 0)
        return 0;
    
    /* FT232R的基础时钟是3MHz */
    divisor = 3000000 / baudrate;
    
    /* 确保分频器在有效范围内 */
    if(divisor < 1) divisor = 1;
    if(divisor > 0x3FFF) divisor = 0x3FFF;
    
    return divisor;
}

/* 用户API函数 */
USBH_StatusTypeDef USBH_FT232_SetConfig(USBH_HandleTypeDef *phost, FT232_ConfigTypeDef *config)
{
    FT232_HandleTypeDef *ft232 = (FT232_HandleTypeDef *)phost->pActiveClass->pData;
    
    if(ft232 == NULL || config == NULL)
        return USBH_FAIL;
    
    /* 保存配置 */
    ft232->config = *config;
    
    /* 如果设备已经初始化，重新配置 */
    if(ft232->state == FT232_IDLE)
    {
        ft232->state = FT232_RESET;
    }
    
    return USBH_OK;
}

USBH_StatusTypeDef USBH_FT232_Transmit(USBH_HandleTypeDef *phost, uint8_t *data, uint16_t length)
{
    FT232_HandleTypeDef *ft232 = (FT232_HandleTypeDef *)phost->pActiveClass->pData;
    
    if(ft232 == NULL || data == NULL || length == 0)
        return USBH_FAIL;
    
    if(ft232->state != FT232_IDLE)
        return USBH_BUSY;
    
    return USBH_BulkSendData(phost, data, length, ft232->OutPipe, 1);
}

USBH_StatusTypeDef USBH_FT232_Receive(USBH_HandleTypeDef *phost, uint8_t *data, uint16_t length)
{
    FT232_HandleTypeDef *ft232 = (FT232_HandleTypeDef *)phost->pActiveClass->pData;
    
    if(ft232 == NULL || data == NULL || length == 0)
        return USBH_FAIL;
    
    if(ft232->state != FT232_IDLE)
        return USBH_BUSY;
    
    return USBH_BulkReceiveData(phost, data, length, ft232->InPipe);
} 