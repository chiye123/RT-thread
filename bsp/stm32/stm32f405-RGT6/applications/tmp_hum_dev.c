#include "tmp_hum_dev.h"
#include "TinyModbusRTU.h"

#define swap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

int read_tmp_hum_dev(uint8_t addr, struct tmp_hum_dev *data)
{
    rt_err_t status;
    uint8_t i;

    data->data_num = 4;
    for ( i = 0; i < 3; i++)
    {
        status = modbus_rtu_read_reg(addr, TMP_HUM_GET_HUM, &data->airHum, 2000);
        data->airHum = swap_16(data->airHum);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        status = modbus_rtu_read_reg(addr, TMP_HUM_GET_TMP, &data->airTmp, 2000);
        data->airTmp = swap_16(data->airTmp);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        status = modbus_rtu_read_reg(addr, TMP_HUM_ADJ_TMP, &data->tmp_calibration, 2000);
        data->tmp_calibration = swap_16(data->tmp_calibration);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        status = modbus_rtu_read_reg(addr, TMP_HUM_ADJ_HUM, &data->hum_calibration, 2000);
        data->hum_calibration = swap_16(data->hum_calibration);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
        }
        else
        {
            return status;
        }

        _EXIT_FOR:
            data->devErrCode = 0;
    }
    return status;





    // status = modbus_rtu_read_reg(addr, TMP_HUM_GET_HUM, &data->airHum, 2000);
    // data->airHum = swap_16(data->airHum);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;
    // status = modbus_rtu_read_reg(addr, TMP_HUM_GET_TMP, &data->airTmp, 2000);
    // data->airTmp = swap_16(data->airTmp);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;
    // status = modbus_rtu_read_reg(addr, TMP_HUM_ADJ_TMP, &data->tmp_calibration, 2000);
    // data->tmp_calibration = swap_16(data->tmp_calibration);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;
    // status = modbus_rtu_read_reg(addr, TMP_HUM_ADJ_HUM, &data->hum_calibration, 2000);
    // data->hum_calibration = swap_16(data->hum_calibration);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // data->data_num = 4;
    // data->devErrCode = 0;
    // return RT_EOK;
}
