#ifndef _PA_LUX_PM2510_DEV_H
#define _PA_LUX_PM2510_DEV_H

#include <stdint.h>

struct pa_lux_pm2510_dev {
    uint8_t devErrCode;
    uint8_t data_num;
    uint16_t PM2_5;
    uint16_t PM10;
    uint16_t KPA;
    uint16_t Lux_H;
    uint16_t Lux_L;
};

/* 大气压_光照强度_PM2.5_PM10 */
#define PA_LUX_PM2510_DEV0_ADDR          0x0AU

#define PA_LUX_PM2510_GET_PM25          503
#define PA_LUX_PM2510_GET_PM10          504
#define PA_LUX_PM2510_GET_PA            505
#define PA_LUX_PM2510_GET_LUXH          506
#define PA_LUX_PM2510_GET_LUXL          507

int read_pa_lux_pm2510_dev(uint8_t addr, struct pa_lux_pm2510_dev *data);

#endif
