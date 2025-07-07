
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <arpa/inet.h>         /* 包含 ip_addr_t 等地址相关的头文件 */
#include <netdev.h>            /* 包含全部的 netdev 相关操作接口函数 */
#include <ntp.h>
#include "my_mqtt.h"
#include "my_flash.h"
#include "dev_addr.h"
#include "TinyModbusRTU.h"
#include "my_server.h"
#include "bds_gps.h"
#include "my_wdt.h"
#include <fal.h>


/* 检查网络连通性 */
// int net_check(void)
// {
//     struct netdev *dev = RT_NULL;
//     while (1)
//     {
//         dev = netdev_get_first_by_flags(NETDEV_FLAG_INTERNET_UP);
//         if (dev == RT_NULL)
//         {
//             rt_kprintf("wait netdev internet up...\r\n");
//             rt_thread_mdelay(500);
//         }
//         else
//         {
//             break;
//         }
//     }

//     return 0;
// }

/* 检查系统电源 */
void check_power_sys(void)
{
    rt_err_t status;
    uint16_t data;

    /* 检查太阳能控制器故障代码 */
    do
    {
        status = modbus_rtu_read_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_ERROR_CODE, &data, 2000);
        if (status == RT_EOK)
        {
            /* 控制器充电电路故障、未连接蓄电池、负载短路、负载过压、温度故障 */
            if (data == 2 || data == 4 || data == 8 || data == 32 || data == 64)
            {
                /* 切断外设供电 */
                SENSOR_PWR_OFF
                sensor_pwr_status = 0;
            }
        }
        else
        {
            rt_thread_mdelay(1000);
        }
    } while (status != RT_EOK);

    /* 检查蓄电池故障代码 */
    do
    {
        status = modbus_rtu_read_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_BAT_STATUS, &data, 2000);
        if (status == RT_EOK)
        {
            /* 欠压 */
            if (data == 1)
            {
                /* 切断外设供电 */
                SENSOR_PWR_OFF
                sensor_pwr_status = 0;

                /* 打开MPPT、蓄电池充电开关 */
                modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_MPPT_SWITCH, 1);
                modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_BAT_CHARG_SWITCH, 1);
            }
            else
            {
                /* 允许向外设供电 */
                SENSOR_PWR_ON
                sensor_pwr_status = 1;

                /* 等待供电稳定 */
                rt_thread_mdelay(3000);
            }
        }
        else
        {
            rt_thread_mdelay(1000);
        }
    } while (status != RT_EOK);
}

// void st(void)
// {
//     start_my_server();
// }
// MSH_CMD_EXPORT(st, start server);

int main(void)
{
//    int status;

//    rt_kprintf("POWER_IN=%d POWER_STATUS=%d\n", POWER_IN_PIN, POWER_STATUS_PIN);

    fal_init();
    
    set_uart(BSP_USING_RS485_UART_NAME,BSP_USING_RS485_UART_BAUD, BSP_USING_RS485_UART_BUFFER);
    set_uart(BSP_USING_BDS_GPS_UART_NAME, BSP_USING_BDS_GPS_UART_BAUD, BSP_USING_BDS_GPS_UART_BUFFER_SIZE);

    /* set SYS_LED pin mode to output */
    rt_pin_mode(SYS_LED_PIN, PIN_MODE_OUTPUT);
    SYS_LED_OFF

    /* set SENSOR_PWR pin mode to output */
    rt_pin_mode(SENSOR_PWR_PIN, PIN_MODE_OUTPUT);
    SENSOR_PWR_OFF

    /* set VIDEO_PWR pin mode to output */
    rt_pin_mode(VIDEO_PWR_PIN, PIN_MODE_OUTPUT);
    VIDEO_PWR_OFF


//    check_power_sys();

    /* 检查并等待网络联通 */
    
   SYS_LED_ON
   // net_check();
   SENSOR_PWR_ON
   SYS_LED_OFF
   /* 同步网络时间 */
   SYS_LED_ON
   ntp_sync_to_rtc(NULL);
   SYS_LED_OFF
   /* 与mqtt服务器建立连接 */
   SYS_LED_ON
   app_my_mqtt_init();
   SYS_LED_OFF

   start_my_server();
   wdt_sample();
}
