#include "bds_gps.h"
#include "TinyModbusRTU.h"
#include <rtdef.h>
#include <rtdevice.h>
#include <string.h>


int bds_gps_read_line(uint8_t *buf)
{
    rt_device_t serial;                /* 串口设备句柄 */
    uint32_t len,t;

    /* 查找串口设备 */
    serial = rt_device_find(BSP_USING_BDS_GPS_UART_NAME);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", BSP_USING_BDS_GPS_UART_NAME);
        return RT_ERROR;
    }

    /* 以中断接收及DMA发送模式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);

    len = 0;
    t = 0;
    buf[0] = 0;
    do
    {
        if (rt_device_read(serial, 0, &buf[len], 1) == 0)
		{
			t += 1;
		}
		else
		{
			// rt_kprintf("%c len:%d\n", buf[len], len);
			if (buf[len] == '$' || len > 0)
				len += 1;
		}
        rt_thread_mdelay(1);
    } while(buf[len-1] != '*' && t < 50);
    rt_device_close(serial);

    if (len > 0)
        return len - 1;
    return 0;
}

int bds_gps_get_gga(struct bds_gps_dev *data)
{
    uint8_t i;
    uint32_t len;

    for ( i = 0;i < 40; i++)
    {
        len = bds_gps_read_line(&data->gga[0]);

        if (len >= 71)
        {
            if (data->gga[0] == '$' && \
                data->gga[1] == 'G' && \
                data->gga[2] == 'N' && \
                data->gga[3] == 'G' && \
                data->gga[4] == 'G' && \
                data->gga[5] == 'A' \
            )
            {
                data->data_num = len;
                data->devErrCode = 0;
                // rt_kprintf("gga len:%d\n", data->data_num);
                return RT_EOK;
            }
        }
        else
        {
            data->data_num = 0;
            rt_thread_mdelay(1);
        }
    }
    data->devErrCode = 1;
    return RT_ERROR;
}

struct bds_gps_dev bds_gps_dev_data;
void gga(void)
{
    uint8_t i;

    rt_kprintf("BDS/GPS GGA:");
    if (bds_gps_get_gga(&bds_gps_dev_data) == RT_EOK)
    {
        for ( i = 0; i < bds_gps_dev_data.data_num; i++)
        {
            rt_kprintf("%c", bds_gps_dev_data.gga[i]);
        }
        rt_kprintf("\n");
    }
    else
    {
        rt_kprintf("failed!\n");
    }
}
MSH_CMD_EXPORT(gga, bds gga);
