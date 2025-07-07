#include "pa_lux_pm2510_dev.h"
#include "TinyModbusRTU.h"
#include <rtdef.h>

#define swap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

int read_pa_lux_pm2510_dev(uint8_t addr, struct pa_lux_pm2510_dev *data)
{
    rt_err_t status;
    uint8_t i;

    for ( i = 0; i < 3; i++)
    {
        status = modbus_rtu_read_reg(addr, PA_LUX_PM2510_GET_PM25, &data->PM2_5, 2000);
        data->PM2_5 = swap_16(data->PM2_5);
        data->devErrCode = (uint8_t)status;
        if (status == RT_EOK)
        {
        //    rt_kprintf("PM2_5 ok\n");
            break;
        }
        else{data->devErrCode = 1;}
    }

    for ( i = 0; i < 3; i++)
    {
        status = modbus_rtu_read_reg(addr, PA_LUX_PM2510_GET_PM10, &data->PM10, 2000);
        data->PM10 = swap_16(data->PM10);
        data->devErrCode = (uint8_t)status;
        if (status == RT_EOK)
        {
        //    rt_kprintf("PM10 ok\n");
            break;
        }
        else{data->devErrCode = 1;}
    }

    for ( i = 0; i < 3; i++)
    {
        status = modbus_rtu_read_reg(addr, PA_LUX_PM2510_GET_PA, &data->KPA, 2000);
        data->KPA = swap_16(data->KPA);
        data->devErrCode = (uint8_t)status;
        if (status == RT_EOK)
        {
        //    rt_kprintf("KPA ok\n");
            break;
        }
        else{data->devErrCode = 1;}
    }

    for ( i = 0; i < 3; i++)
    {
        status = modbus_rtu_read_reg(addr, PA_LUX_PM2510_GET_LUXH, &data->Lux_H, 2000);
        data->Lux_H = swap_16(data->Lux_H);
        data->devErrCode = (uint8_t)status;
        if (status == RT_EOK)
        {
        //    rt_kprintf("Lux_H ok\n");
            break;
        }
        else{data->devErrCode = 1;}
    }

    for ( i = 0; i < 3; i++)
    {
        status = modbus_rtu_read_reg(addr, PA_LUX_PM2510_GET_LUXL, &data->Lux_L, 2000);
        data->Lux_L = swap_16(data->Lux_L);
        data->devErrCode = (uint8_t)status;
        if (status == RT_EOK)
        {
        //    rt_kprintf("Lux_L ok\n");
            break;
        }
        else{data->devErrCode = 1;}
    }
    return status;
}

// void pa(void)
// {
//     struct pa_lux_pm2510_dev data;
//     int status;

//     status = read_pa_lux_pm2510_dev(PA_LUX_PM2510_DEV0_ADDR, &data);

//     rt_kprintf("------------\nstatus = 0x%X %d\n", status, status);
//     rt_kprintf("PM2_5 = 0x%X %d\n", data.PM2_5, data.PM2_5);
//     rt_kprintf("PM10 = 0x%X %d\n", data.PM10, data.PM10);
//     rt_kprintf("KPA = 0x%X %d\n", data.KPA, data.KPA);
//     rt_kprintf("Lux_H = 0x%X %d\n", data.Lux_H, data.Lux_H);
//     rt_kprintf("Lux_L = 0x%X %d\n", data.Lux_L, data.Lux_L);
// }
// MSH_CMD_EXPORT(pa, baiyexiang);
