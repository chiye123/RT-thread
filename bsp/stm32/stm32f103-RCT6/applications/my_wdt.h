#ifndef _MY_WDT_H
#define _MY_WDT_H

#include <rtdevice.h>

#define WDT_DEVICE_NAME    "wdt"    /* 看门狗设备名称 */

extern rt_device_t my_wdg_dev;         /* 看门狗设备句柄 */

int wdt_sample(void);

#endif

