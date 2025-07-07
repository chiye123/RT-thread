#ifndef _BDS_GPS_H
#define _BDS_GPS_H

#include <rtthread.h>

struct bds_gps_dev {
    uint8_t devErrCode;
    uint8_t data_num;
    uint8_t gga[BSP_USING_BDS_GPS_UART_BUFFER_SIZE];
};

// int bds_gps_read_line(uint8_t *buf);
int bds_gps_get_gga(struct bds_gps_dev *data);

#endif
