#ifndef _MY_WDT_H
#define _MY_WDT_H

#include <rtdevice.h>

#define WDT_DEVICE_NAME    "wdt"    /* ���Ź��豸���� */

extern rt_device_t my_wdg_dev;         /* ���Ź��豸��� */

int wdt_sample(void);

#endif

