#ifndef _RAINFALL_DEV_H
#define _RAINFALL_DEV_H

#include <stdint.h>

struct rainfall_dev {
    uint8_t devErrCode;
    uint8_t data_num;
    uint16_t rainfall;
    uint16_t reserved[4];
};

/* 雨量计 */
#define RAINFALL_DEV0_ADDR               0x04U

#define RAINFALL_GET_DATA               0x0000U
#define RAINFALL_CLEAN_DATA             0x0000U

int read_rainfall_dev(uint8_t addr, struct rainfall_dev *data);
#define clear_rainfall_dev(addr) modbus_rtu_write_reg(addr, RAINFALL_CLEAN_DATA, 0)

#endif
