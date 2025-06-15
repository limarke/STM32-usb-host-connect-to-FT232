#ifndef __CRC_XMOD_H_
#define __CRC_XMOD_H_

#include "main.h"

uint8_t check_sum(uint8_t *ptr,uint16_t len);
uint16_t do_crc_table(uint8_t *ptr,uint16_t len);
uint16_t usMBCRC16(uint8_t * pucFrame,uint16_t usLen);
#endif /* BSP_CRC_H_ */
