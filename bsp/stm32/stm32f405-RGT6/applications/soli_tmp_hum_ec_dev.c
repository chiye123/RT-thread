#include "soli_tmp_hum_ec_dev.h"
#include "TinyModbusRTU.h"
#include <rtdef.h>

#define swap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

int read_soli_tmp_hum_ec_dev(uint8_t addr, struct soli_tmp_hum_ec_dev *data)
{
    rt_err_t status;
    uint8_t i;

    data->data_num = 11;
    for ( i = 0; i < 3; i++)
    {
        /* 含水率 */
        status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_WATER, &data->water, 2000);
        data->water = swap_16(data->water);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 温度 */
        status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_TMP, &data->tmp, 2000);
        data->tmp = swap_16(data->tmp);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 电导率 */
        status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_EC, &data->ec, 2000);
        data->ec = swap_16(data->ec);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 盐度 */
        status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_SALT, &data->salt, 2000);
        data->salt = swap_16(data->salt);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 总容积固体TDS */
        status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_TDS, &data->tds, 2000);
        data->tds = swap_16(data->tds);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 电导温度系数 */
        status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_ECTMP, &data->ecTmp_cofficiente, 2000);
        data->ecTmp_cofficiente = swap_16(data->ecTmp_cofficiente);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 盐度系数 */
        status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_SALT_COEFFICIENT, &data->salt_cofficiente, 2000);
        data->salt_cofficiente = swap_16(data->salt_cofficiente);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* TDS系数 */
        status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_TDS_COEFFICIENT, &data->tds_cofficiente, 2000);
        data->tds_cofficiente = swap_16(data->tds_cofficiente);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 温度校准值 */
        status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_TMP_ADJ, &data->tmp_calibration, 2000);
        data->tmp_calibration = swap_16(data->tmp_calibration);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 含水率校准值 */
        status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_WATER_ADJ, &data->water_calibration, 2000);
        data->water_calibration = swap_16(data->water_calibration);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
            continue;
        }

        /* 电导率校准值 */
        status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_EC_ADJ, &data->ec_calibration, 2000);
        data->ec_calibration = swap_16(data->ec_calibration);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            data->devErrCode = 1;
        }
        else
        {
            data->devErrCode = 0;
            return status;
        }
    }
    return status;



    // /* 含水率 */
    // status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_WATER, &data->water, 2000);
    // data->water = swap_16(data->water);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 温度 */
    // status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_TMP, &data->tmp, 2000);
    // data->tmp = swap_16(data->tmp);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 电导率 */
    // status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_EC, &data->ec, 2000);
    // data->ec = swap_16(data->ec);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 盐度 */
    // status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_SALT, &data->salt, 2000);
    // data->salt = swap_16(data->salt);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;
        
    // /* 总容积固体TDS */
    // status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_TDS, &data->tds, 2000);
    // data->tds = swap_16(data->tds);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 电导温度系数 */
    // status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_ECTMP, &data->ecTmp_cofficiente, 2000);
    // data->ecTmp_cofficiente = swap_16(data->ecTmp_cofficiente);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;
    
    // /* 盐度系数 */
    // status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_SALT_COEFFICIENT, &data->salt_cofficiente, 2000);
    // data->salt_cofficiente = swap_16(data->salt_cofficiente);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* TDS系数 */
    // status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_TDS_COEFFICIENT, &data->tds_cofficiente, 2000);
    // data->tds_cofficiente = swap_16(data->tds_cofficiente);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 温度校准值 */
    // status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_TMP_ADJ, &data->tmp_calibration, 2000);
    // data->tmp_calibration = swap_16(data->tmp_calibration);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 含水率校准值 */
    // status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_WATER_ADJ, &data->water_calibration, 2000);
    // data->water_calibration = swap_16(data->water_calibration);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 电导率校准值 */
    // status = modbus_rtu_read_reg(addr, SOLI_TMP_HUM_EC_EC_ADJ, &data->ec_calibration, 2000);
    // data->ec_calibration = swap_16(data->ec_calibration);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // data->data_num = 11;
    // data->devErrCode = 0;
    // return RT_EOK;
}
