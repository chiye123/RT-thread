#ifndef _WIND_DIR_DEV_H
#define _WIND_DIR_DEV_H

#include <stdint.h>

struct wind_dir_dev {
    uint8_t devErrCode;
    uint8_t data_num;
    uint16_t wind_dir;
    uint16_t reserved[4];
};

/* 风向变送器 */
#define WIND_DIR_DEV0_ADDR               0x06U

#define WIND_DIR_GET_DATA             0x0001U

int read_wind_dir_dev(uint8_t addr, struct wind_dir_dev *data);

#endif
