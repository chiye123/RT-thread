#ifndef _SOLI_TMP_HUM_EC_DEV_H
#define _SOLI_TMP_HUM_EC_DEV_H

#include <stdint.h>

struct soli_tmp_hum_ec_dev {
    uint8_t devErrCode;
    uint8_t data_num;
    uint16_t water;                 /* 含水率 */
    uint16_t tmp;                   /* 温度 */
    uint16_t ec;                    /* 电导率 */
    uint16_t salt;                  /* 盐度 */
    uint16_t tds;                   /* 总容积固体TDS */
    uint16_t ecTmp_cofficiente;     /* 电导温度系数 */
    uint16_t salt_cofficiente;      /* 盐度系数 */
    uint16_t tds_cofficiente;       /* TDS系数 */
    uint16_t tmp_calibration;       /* 温度校准值 */
    uint16_t water_calibration;     /* 含水率校准值 */
    uint16_t ec_calibration;        /* 电导率校准值 */
};

/* 土壤温湿度电导率 */
#define SOLI_TMP_HUM_EC_DEV0_ADDR       0x07U
#define SOLI_TMP_HUM_EC_DEV1_ADDR       0x08U
#define SOLI_TMP_HUM_EC_DEV2_ADDR       0x09U
#define SOLI_TMP_HUM_EC_DEV3_ADDR       0x10U
#define SOLI_TMP_HUM_EC_DEV4_ADDR       0x11U
#define SOLI_TMP_HUM_EC_DEV5_ADDR       0x12U


#define SOLI_TMP_HUM_EC_WATER                   0x0000U
#define SOLI_TMP_HUM_EC_TMP                     0x0001U
#define SOLI_TMP_HUM_EC_EC                      0x0002U
#define SOLI_TMP_HUM_EC_SALT                    0x0003U
#define SOLI_TMP_HUM_EC_TDS                     0x0004U
#define SOLI_TMP_HUM_EC_ECTMP                   0x0022U
#define SOLI_TMP_HUM_EC_SALT_COEFFICIENT        0x0023U
#define SOLI_TMP_HUM_EC_TDS_COEFFICIENT         0x0024U
#define SOLI_TMP_HUM_EC_TMP_ADJ                 0x0050U
#define SOLI_TMP_HUM_EC_WATER_ADJ               0x0051U
#define SOLI_TMP_HUM_EC_EC_ADJ                  0x0052U


int read_soli_tmp_hum_ec_dev(uint8_t addr, struct soli_tmp_hum_ec_dev *data);

#endif

