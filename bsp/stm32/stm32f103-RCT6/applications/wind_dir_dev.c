#include "wind_dir_dev.h"
#include "TinyModbusRTU.h"

#define swap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

int read_wind_dir_dev(uint8_t addr, struct wind_dir_dev *data)
{
    rt_err_t status;

    data->data_num = 1;
    data->devErrCode = 1;
    status = modbus_rtu_read_more_times(addr, WIND_DIR_GET_DATA, &data->wind_dir, 2000, 3);
    if (status == RT_EOK)
    {
        data->devErrCode = 0;
    }
    
    return status;

    
    // uint8_t i;

    // data->data_num = 1;
    // for ( i = 0; i < 3; i++)
    // {
    //     data->devErrCode = 0;
    //     status = modbus_rtu_read_reg(addr, WIND_DIR_GET_DATA, &data->wind_dir, 2000);
    //     data->wind_dir = swap_16(data->wind_dir);
    //     data->devErrCode = (uint8_t)status;
    //     if (status == RT_EOK)
    //     {
    //         return status;
    //     }
    //     else
    //     {
    //         data->devErrCode = 1;
    //     }
    // }
    // return status;
}
