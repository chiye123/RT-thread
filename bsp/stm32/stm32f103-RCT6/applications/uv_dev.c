#include "uv_dev.h"
#include "TinyModbusRTU.h"

#define swap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

int read_uv_dev(uint8_t addr, struct uv_dev *data)
{
    rt_err_t status;

    data->data_num = 2;
    data->devErrCode = 1;
    status = modbus_rtu_read_more_times(addr, UV_GET_INTENSITY_DATA, &data->uvIntensity, 2000, 3);
    if (status == RT_EOK)
    {
        status = modbus_rtu_read_more_times(addr, UV_GET_INDEX_DATA, &data->uvIndex, 2000, 3);
        data->devErrCode =(uint8_t)status;
    }
    return status;

    // uint8_t i;

    // data->data_num = 2;
    // for ( i = 0; i < 3; i++)
    // {
    //     status = modbus_rtu_read_reg(addr, UV_GET_INTENSITY_DATA, &data->uvIntensity, 2000);
    //     data->uvIntensity = swap_16(data->uvIntensity);
    //     data->devErrCode = (uint8_t)status;
    //     if (status != RT_EOK)
    //     {
    //         data->devErrCode = 1;
    //         goto _EXIT_FOR;
    //     }

    //     status = modbus_rtu_read_reg(addr, UV_GET_INDEX_DATA, &data->uvIndex, 2000);
    //     data->uvIndex = swap_16(data->uvIndex);
    //     data->devErrCode = (uint8_t)status;
    //     if (status != RT_EOK)
    //     {
    //         data->devErrCode = 1;
    //     }
    //     else
    //     {
    //         return status;
    //     }

    //     _EXIT_FOR:
    //         data->devErrCode = 0;
    // }
    // return status;
}
