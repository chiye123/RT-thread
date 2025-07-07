#ifndef _TMP_HUM_DEV_H
#define _TMP_HUM_DEV_H

#include <stdint.h>

struct tmp_hum_dev {
    uint8_t devErrCode;
    uint8_t data_num;
    uint16_t airTmp;
    uint16_t airHum;
    uint16_t tmp_calibration;
    uint16_t hum_calibration;
    uint16_t reserved[1];
};

/* 温湿度变送器 */
#define TMP_HUM_DEV0_ADDR                0x03U

#define TMP_HUM_GET_HUM             0x0000U
#define TMP_HUM_GET_TMP             0x0001U
#define TMP_HUM_ADJ_TMP             0x0050U
#define TMP_HUM_ADJ_HUM             0x0051U

int read_tmp_hum_dev(uint8_t addr, struct tmp_hum_dev *data);

#endif
