#ifndef _WIND_SPEED_DEV_H
#define _WIND_SPEED_DEV_H

#include <stdint.h>

struct wind_speed_dev {
    uint8_t devErrCode;
    uint8_t data_num;
    uint16_t wind_speed;
    uint16_t reserved[4];
};

/* 风速变送器 */
#define WIND_SPEED_DEV0_ADDR             0x01U

#define WIND_SPEED_GET_DATA             0x0000U

int read_wind_speed_dev(uint8_t addr, struct wind_speed_dev *data);

#endif
