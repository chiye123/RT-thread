#include "solar_ctrl_dev.h"
#include "TinyModbusRTU.h"
#include <rtdef.h>

#define swap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

int read_solar_ctrl_dev(uint8_t addr, struct solar_ctrl_dev *data)
{
    rt_err_t status;
    uint8_t i;

    data->data_num = 31;
    for ( i = 0; i < 3; i++)
    {
        /* 获取故障代码 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_ERROR_CODE, &data->solarctrl_errCode, 2000);
        data->solarctrl_errCode = swap_16(data->solarctrl_errCode);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("solarctrl_errCode %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取电池状态 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_STATUS, &data->batter_status, 2000);
        data->batter_status = swap_16(data->batter_status);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_status %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取电池电量（百分比） */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_PERCENT, &data->batter_percent, 2000);
        data->batter_percent = swap_16(data->batter_percent);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_percent %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取电池电压 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_VOLTAGE, &data->batter_voltage, 2000);
        data->batter_voltage = swap_16(data->batter_voltage);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_voltage %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取电池充电电流 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_IN_CURRENT, &data->batter_in_current, 2000);
        data->batter_in_current = swap_16(data->batter_in_current);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_in_current %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取光伏板电压 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_SOLAR_VOLTAGE, &data->solar_voltage, 2000);
        data->solar_voltage = swap_16(data->solar_voltage);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("solar_voltage %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取光伏板电流 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_SOLAR_OUT_CURRENT, &data->solar_current, 2000);
        data->solar_current = swap_16(data->solar_current);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("solar_current %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取负载电流 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_LOAD_CURRENT, &data->load_current, 2000);
        data->load_current = swap_16(data->load_current);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("load_current %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取MOS管温度 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_MOS_TMP, &data->mos_tmp, 2000);
        data->mos_tmp = swap_16(data->mos_tmp);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("mos_tmp %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取电池温度 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BATTER_TMP, &data->batter_tmp, 2000);
        data->batter_tmp = swap_16(data->batter_tmp);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_tmp %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取当日光伏发电千瓦时 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_PWR_KWH, &data->day_pwr_kwh, 2000);
        data->day_pwr_kwh = swap_16(data->day_pwr_kwh);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("day_pwr_kwh %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取当日光伏发电量瓦时 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_PWR_WH, &data->day_pwr_wh, 2000);
        data->day_pwr_wh = swap_16(data->day_pwr_wh);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("day_pwr_wh %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取当日负载用电千瓦时 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_USE_KWH, &data->day_use_kwh, 2000);
        data->day_use_kwh = swap_16(data->day_use_kwh);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("day_use_kwh %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取当日负载用电瓦时 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_USE_WH, &data->day_use_wh, 2000);
        data->day_use_wh = swap_16(data->day_use_wh);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("day_use_wh %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取累计光伏发电千瓦时 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_ALL_PWR_KWH, &data->all_pwr_kwh, 2000);
        data->all_pwr_kwh = swap_16(data->all_pwr_kwh);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("all_pwr_kwh %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }
        
        /* 获取累计光伏发电瓦时 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_ALL_PWR_WH, &data->all_pwr_wh, 2000);
        data->all_pwr_wh = swap_16(data->all_pwr_wh);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("all_pwr_wh %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取当日负载用电量瓦时 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_USE_WH, &data->all_use_wh, 2000);
        data->all_use_wh = swap_16(data->all_use_wh);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("all_use_wh %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取累计负载用电千瓦时 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_USE_WH, &data->all_use_kwh, 2000);
        data->all_use_kwh = swap_16(data->all_use_kwh);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("all_use_kwh %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取蓄电池容量 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_CAPACITY, &data->batter_capacity, 2000);
        data->batter_capacity = swap_16(data->batter_capacity);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_capacity %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取电池额定电压 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_RATED_VOLTAGE, &data->batter_rated_voltage, 2000);
        data->batter_rated_voltage = swap_16(data->batter_rated_voltage);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_rated_voltage %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取欠压点 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_UNDER_VOL_POINT, &data->batter_under_vol_point, 2000);
        data->batter_under_vol_point = swap_16(data->batter_under_vol_point);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_under_vol_point %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取欠压恢复点 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_UNDER_VOL_RECV_POINT, &data->batter_under_vol_recv_point, 2000);
        data->batter_under_vol_recv_point = swap_16(data->batter_under_vol_recv_point);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_under_vol_recv_point %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取浮充点 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_FLOAT_CHARGE_POINT, &data->batter_floating_charge_point, 2000);
        data->batter_floating_charge_point = swap_16(data->batter_floating_charge_point);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_floating_charge_point %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取过压恢复点 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_OVERVOL_RECV_POINT, &data->batter_overvol_recv_point, 2000);
        data->batter_overvol_recv_point = swap_16(data->batter_overvol_recv_point);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_overvol_recv_point %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取过压点 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_OVERVOL_POINT, &data->batter_over_vol_point, 2000);
        data->batter_over_vol_point = swap_16(data->batter_over_vol_point);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_over_vol_point %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取过载恢复点 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_OVERLOAD_RECV_POINT, &data->batter_overload_recv_point, 2000);
        data->batter_overload_recv_point = swap_16(data->batter_overload_recv_point);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_overload_recv_point %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取过载点 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_OVERLOAD_POINT, &data->batter_overload_point, 2000);
        data->batter_overload_point = swap_16(data->batter_overload_point);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_overload_point %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取温度补偿值 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_TMP_COMPENSATION, &data->batter_tmp_compensation, 2000);
        data->batter_tmp_compensation = swap_16(data->batter_tmp_compensation);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_tmp_compensation %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取充电开关 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_CHARG_SWITCH, &data->batter_charging_switch, 2000);
        data->batter_charging_switch = swap_16(data->batter_charging_switch);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("batter_charging_switch %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取MPPT开关 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_MPPT_SWITCH, &data->mppt_switch, 2000);
        data->mppt_switch = swap_16(data->mppt_switch);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("mppt_switch %d\n", status);
            data->devErrCode = 1;
            goto _EXIT_FOR;
        }

        /* 获取负载开关 */
        status = modbus_rtu_read_reg(addr, SOLAR_CRTL_LOAD_SWITCH, &data->load1_switch, 2000);
        data->load1_switch = swap_16(data->load1_switch);
        data->devErrCode = (uint8_t)status;
        if (status != RT_EOK)
        {
            // rt_kprintf("load1_switch %d\n", status);
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


    // /* 获取故障代码 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_ERROR_CODE, &data->solarctrl_errCode, 2000);
    // data->solarctrl_errCode = swap_16(data->solarctrl_errCode);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取电池状态 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_STATUS, &data->batter_status, 2000);
    // data->batter_status = swap_16(data->batter_status);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取电池电量（百分比） */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_PERCENT, &data->batter_percent, 2000);
    // data->batter_percent = swap_16(data->batter_percent);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取电池电压 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_VOLTAGE, &data->batter_voltage, 2000);
    // data->batter_voltage = swap_16(data->batter_voltage);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取电池充电电流 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_IN_CURRENT, &data->batter_in_current, 2000);
    // data->batter_in_current = swap_16(data->batter_in_current);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取光伏板电压 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_SOLAR_VOLTAGE, &data->solar_voltage, 2000);
    // data->solar_voltage = swap_16(data->solar_voltage);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取光伏板电流 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_SOLAR_OUT_CURRENT, &data->solar_current, 2000);
    // data->solar_current = swap_16(data->solar_current);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取负载电流 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_LOAD_CURRENT, &data->load_current, 2000);
    // data->load_current = swap_16(data->load_current);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取MOS管温度 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_MOS_TMP, &data->mos_tmp, 2000);
    // data->mos_tmp = swap_16(data->mos_tmp);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取电池温度 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BATTER_TMP, &data->batter_tmp, 2000);
    // data->batter_tmp = swap_16(data->batter_tmp);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取当日光伏发电千瓦时 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_PWR_KWH, &data->day_pwr_kwh, 2000);
    // data->day_pwr_kwh = swap_16(data->day_pwr_kwh);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取当日光伏发电量瓦时 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_PWR_WH, &data->day_pwr_wh, 2000);
    // data->day_pwr_wh = swap_16(data->day_pwr_wh);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取当日负载用电千瓦时 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_USE_KWH, &data->day_use_kwh, 2000);
    // data->day_use_kwh = swap_16(data->day_use_kwh);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取当日负载用电瓦时 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_USE_WH, &data->day_use_wh, 2000);
    // data->day_use_wh = swap_16(data->day_use_wh);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取累计光伏发电千瓦时 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_ALL_PWR_KWH, &data->all_pwr_kwh, 2000);
    // data->all_pwr_kwh = swap_16(data->all_pwr_kwh);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;
    
    // /* 获取累计光伏发电瓦时 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_ALL_PWR_WH, &data->all_pwr_wh, 2000);
    // data->all_pwr_wh = swap_16(data->all_pwr_wh);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取当日负载用电量瓦时 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_USE_WH, &data->all_use_wh, 2000);
    // data->all_use_wh = swap_16(data->all_use_wh);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取累计负载用电千瓦时 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_DAY_USE_WH, &data->all_use_kwh, 2000);
    // data->all_use_kwh = swap_16(data->all_use_kwh);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取蓄电池容量 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_CAPACITY, &data->batter_capacity, 2000);
    // data->batter_capacity = swap_16(data->batter_capacity);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取电池额定电压 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_RATED_VOLTAGE, &data->batter_rated_voltage, 2000);
    // data->batter_rated_voltage = swap_16(data->batter_rated_voltage);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取欠压点 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_UNDER_VOL_POINT, &data->batter_under_vol_point, 2000);
    // data->batter_under_vol_point = swap_16(data->batter_under_vol_point);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取欠压恢复点 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_UNDER_VOL_RECV_POINT, &data->batter_under_vol_recv_point, 2000);
    // data->batter_under_vol_recv_point = swap_16(data->batter_under_vol_recv_point);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取浮充点 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_FLOAT_CHARGE_POINT, &data->batter_floating_charge_point, 2000);
    // data->batter_floating_charge_point = swap_16(data->batter_floating_charge_point);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取过压恢复点 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_OVERVOL_RECV_POINT, &data->batter_overvol_recv_point, 2000);
    // data->batter_overvol_recv_point = swap_16(data->batter_overvol_recv_point);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取过压点 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_OVERVOL_POINT, &data->batter_over_vol_point, 2000);
    // data->batter_over_vol_point = swap_16(data->batter_over_vol_point);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取过载恢复点 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_OVERLOAD_RECV_POINT, &data->batter_overload_recv_point, 2000);
    // data->batter_overload_recv_point = swap_16(data->batter_overload_recv_point);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取过载点 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_OVERLOAD_POINT, &data->batter_overload_point, 2000);
    // data->batter_overload_point = swap_16(data->batter_overload_point);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取温度补偿值 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_TMP_COMPENSATION, &data->batter_tmp_compensation, 2000);
    // data->batter_tmp_compensation = swap_16(data->batter_tmp_compensation);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取充电开关 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_BAT_CHARG_SWITCH, &data->batter_charging_switch, 2000);
    // data->batter_charging_switch = swap_16(data->batter_charging_switch);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取MPPT开关 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_MPPT_SWITCH, &data->mppt_switch, 2000);
    // data->mppt_switch = swap_16(data->mppt_switch);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // /* 获取负载开关 */
    // status = modbus_rtu_read_reg(addr, SOLAR_CRTL_LOAD_SWITCH, &data->load1_switch, 2000);
    // data->load1_switch = swap_16(data->load1_switch);
    // data->devErrCode = (uint8_t)status;
    // if (status != RT_EOK)
    //     return status;

    // data->data_num = 31;
    // data->devErrCode = 0;
    // return RT_EOK;
}
