#ifndef _CRC16_H
#define _CRC16_H

#include <stdint.h>
#include <hw_crc.h>

extern const struct hwcrypto_crc_cfg crc16_cfg;

uint16_t calc_crc16(uint8_t *puchMsg, uint32_t usDataLen);

#endif

