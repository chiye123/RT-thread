#ifndef _SOLAR_CTRL_DEV_H
#define _SOLAR_CTRL_DEV_H

#include <stdint.h>

struct solar_ctrl_dev {
    uint8_t devErrCode;
    uint8_t data_num;
    uint16_t solarctrl_errCode;
    uint16_t batter_status;
    uint16_t batter_percent;
    uint16_t batter_voltage;
    uint16_t batter_in_current;
    uint16_t solar_voltage;
    uint16_t solar_current;
    uint16_t load_current;
    uint16_t mos_tmp;
    uint16_t batter_tmp;
    uint16_t day_pwr_kwh;
    uint16_t day_pwr_wh;
    uint16_t day_use_kwh;
    uint16_t day_use_wh;
    uint16_t all_pwr_kwh;
    uint16_t all_pwr_wh;
    uint16_t all_use_kwh;
    uint16_t all_use_wh;
    uint16_t batter_capacity;
    uint16_t batter_rated_voltage;
    uint16_t batter_under_vol_point;
    uint16_t batter_under_vol_recv_point;
    uint16_t batter_floating_charge_point;
    uint16_t batter_overvol_recv_point;
    uint16_t batter_over_vol_point;
    uint16_t batter_overload_recv_point;
    uint16_t batter_overload_point;
    uint16_t batter_tmp_compensation;
    uint16_t batter_charging_switch;
    uint16_t mppt_switch;
    uint16_t load1_switch;
};

/* 太阳能控制器 */
#define SOLAR_CRTL_DEV0_ADDR             0x02U

#define SOLAR_CRTL_ERROR_CODE                   0x0003U         // 获取故障代码
#define SOLAR_CRTL_BAT_STATUS                   0x0004U         // 获取电池状态
#define SOLAR_CRTL_BAT_PERCENT                  0x0005U         // 获取电池电量（百分比）
#define SOLAR_CRTL_BAT_VOLTAGE                  0x0007U         // 获取电池电压
#define SOLAR_CRTL_BAT_IN_CURRENT               0x0008U         // 获取电池充电电流
#define SOLAR_CRTL_SOLAR_VOLTAGE                0x0009U         // 获取光伏板电压
#define SOLAR_CRTL_SOLAR_OUT_CURRENT            0x000AU         // 获取光伏板电流
#define SOLAR_CRTL_LOAD_CURRENT                 0x000EU         // 获取负载电流
#define SOLAR_CRTL_MOS_TMP                      0x000FU         // 获取MOS管温度
#define SOLAR_CRTL_BATTER_TMP                   0x0010U         // 获取电池温度
#define SOLAR_CRTL_DAY_PWR_KWH                  0x0011U         // 获取当日光伏发电千瓦时
#define SOLAR_CRTL_DAY_PWR_WH                   0x0012U         // 获取当日光伏发电量瓦时
#define SOLAR_CRTL_DAY_USE_KWH                  0x0015U         // 获取当日负载用电千瓦时
#define SOLAR_CRTL_DAY_USE_WH                   0x0016U         // 获取当日负载用电量瓦时
#define SOLAR_CRTL_ALL_PWR_KWH                  0x0017U         // 获取累计光伏发电千瓦时
#define SOLAR_CRTL_ALL_PWR_WH                   0x0018U         // 获取累计光伏发电量瓦时
#define SOLAR_CRTL_ALL_USE_KWH                  0x001BU         // 获取累计负载用电千瓦时

#define SOLAR_CRTL_ALL_USE_WH                   0x001CU         // 获取累计负载用电量瓦时
#define SOLAR_CRTL_BAT_CAPACITY                 0x1001U         // 获取蓄电池容量
#define SOLAR_CRTL_BAT_RATED_VOLTAGE            0x1002U         // 获取电池额定电压
#define SOLAR_CRTL_BAT_UNDER_VOL_POINT          0x1003U         // 获取欠压点
#define SOLAR_CRTL_BAT_UNDER_VOL_RECV_POINT     0x1004U         // 获取欠压恢复点
#define SOLAR_CRTL_BAT_FLOAT_CHARGE_POINT       0x1005U         // 获取浮充点
#define SOLAR_CRTL_BAT_OVERVOL_RECV_POINT       0x1006U         // 获取过压恢复点
#define SOLAR_CRTL_BAT_OVERVOL_POINT            0x1007U         // 获取过压点
#define SOLAR_CRTL_OVERLOAD_RECV_POINT          0x1008U         // 获取过载恢复点
#define SOLAR_CRTL_OVERLOAD_POINT               0x1009U         // 获取过载点
#define SOLAR_CRTL_TMP_COMPENSATION             0x100BU         // 获取温度补偿值
#define SOLAR_CRTL_BAT_CHARG_SWITCH             0x2003U         // 获取充电开关
#define SOLAR_CRTL_MPPT_SWITCH                  0x2004U         // 获取MPPT开关
#define SOLAR_CRTL_LOAD_SWITCH                  0x4002U         // 获取负载开关

int read_solar_ctrl_dev(uint8_t addr, struct solar_ctrl_dev *data);

#endif 
