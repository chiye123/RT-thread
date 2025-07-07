#include "npk_ph_dev.h"
#include "TinyModbusRTU.h"
#include <rtdef.h>

#define swap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

int read_npk_ph_dev(uint8_t addr, struct npk_ph_dev *data)
{
    rt_err_t status;
    uint8_t i;

    data->data_num = 8;
    for ( i = 0; i < 3; i++)
    {
        /* 含氮量 */
        status = modbus_rtu_read_reg(addr, NPK_PH_GET_N, &data->n, 2000);
        data->n = swap_16(data->n);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 含磷量 */
        status = modbus_rtu_read_reg(addr, NPK_PH_GET_P, &data->p, 2000);
        data->p = swap_16(data->p);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 含钾量 */
        status = modbus_rtu_read_reg(addr, NPK_PH_GET_K, &data->k, 2000);
        data->k = swap_16(data->k);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* PH值 */
        status = modbus_rtu_read_reg(addr, NPK_PH_GET_PH, &data->ph, 2000);
        data->ph = swap_16(data->ph);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 含氮校准值 */
        status = modbus_rtu_read_reg(addr, NPK_PH_ADJ_N, &data->n_calibration, 2000);
        data->n_calibration = swap_16(data->n_calibration);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 含磷校准值 */
        status = modbus_rtu_read_reg(addr, NPK_PH_ADJ_P, &data->p_calibration, 2000);
        data->p_calibration = swap_16(data->p_calibration);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 含钾校准值 */
        status = modbus_rtu_read_reg(addr, NPK_PH_ADJ_K, &data->k_calibration, 2000);
        data->k_calibration = swap_16(data->k_calibration);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* PH校准值 */
        status = modbus_rtu_read_reg(addr, NPK_PH_ADJ_PH, &data->ph_calibration, 2000);
        data->ph_calibration = swap_16(data->ph_calibration);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }          
    }
    return status;
	}
