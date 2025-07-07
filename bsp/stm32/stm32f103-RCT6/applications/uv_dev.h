#ifndef _UV_DEV_H
#define _UV_DEV_H

#include <stdint.h>

struct uv_dev {
    uint8_t devErrCode;
    uint8_t data_num;
    uint16_t uvIntensity;
    uint16_t uvIndex;
    uint16_t reserved[3];
};

/* 紫外线变送器 */
#define UV_DEV0_ADDR                     0x05U

#define UV_GET_INTENSITY_DATA           0x0000U
#define UV_GET_INDEX_DATA               0x0001U

int read_uv_dev(uint8_t addr, struct uv_dev *data);

#endif
