#ifndef __USBH_FT232_H
#define __USBH_FT232_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usbh_core.h"

/* FT232设备VID和PID定义 */
#define FT232_VID 0x0403
#define FT232_PID 0x6001

/* FT232控制命令定义 */
#define FTDI_SIO_RESET          0x00    /* Reset the port */
#define FTDI_SIO_MODEM_CTRL     0x01    /* Set the modem control register */
#define FTDI_SIO_SET_FLOW_CTRL  0x02    /* Set flow control register */
#define FTDI_SIO_SET_BAUD_RATE  0x03    /* Set baud rate */
#define FTDI_SIO_SET_DATA       0x04    /* Set the data characteristics of the port */

/* FT232数据位定义 */
#define FTDI_SIO_SET_DATA_BITS(n) (n & 0x0f)
#define FTDI_SIO_SET_DATA_STOP_BITS_1   (0x0 << 11)
#define FTDI_SIO_SET_DATA_STOP_BITS_15  (0x1 << 11)
#define FTDI_SIO_SET_DATA_STOP_BITS_2   (0x2 << 11)
#define FTDI_SIO_SET_DATA_PARITY_NONE   (0x0 << 8)
#define FTDI_SIO_SET_DATA_PARITY_ODD    (0x1 << 8)
#define FTDI_SIO_SET_DATA_PARITY_EVEN   (0x2 << 8)
#define FTDI_SIO_SET_DATA_PARITY_MARK   (0x3 << 8)
#define FTDI_SIO_SET_DATA_PARITY_SPACE  (0x4 << 8)

/* FT232串口配置结构体 */
typedef struct {
    uint32_t baudrate;
    uint8_t  databits;
    uint8_t  stopbits;
    uint8_t  parity;
} FT232_ConfigTypeDef;

extern USBH_ClassTypeDef FT232_Class;
#define USBH_FT232_CLASS &FT232_Class

/* 函数声明 */
USBH_StatusTypeDef USBH_FT232_SetConfig(USBH_HandleTypeDef *phost, FT232_ConfigTypeDef *config);
USBH_StatusTypeDef USBH_FT232_Transmit(USBH_HandleTypeDef *phost, uint8_t *data, uint16_t length);
USBH_StatusTypeDef USBH_FT232_Receive(USBH_HandleTypeDef *phost, uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* __USBH_FT232_H */ 