#include "my_server.h"
#include "my_mqtt.h"
#include <rtdevice.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dev_addr.h"
#include "TinyModbusRTU.h"
#include "bds_gps.h"
#include "my_http.h"
#include <fal.h>


#define swap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

const uint8_t rs485_no_answer[] = "no answer";

//volatile uint32_t _boot_update __attribute__((at(0x20000000), zero_init));

static struct wind_speed_dev wind_speed_dev_data;
static struct solar_ctrl_dev solar_ctrl_dev_data;
static struct tmp_hum_dev tmp_hum_dev_data;
static struct rainfall_dev rainfall_dev_data;
static struct uv_dev uv_dev_data;
static struct wind_dir_dev wind_dir_dev_data;
static struct pa_lux_pm2510_dev pa_lux_pm2510_dev_data;
static struct soli_tmp_hum_ec_dev soli_tmp_hum_ec_dev_0_data;
static struct soli_tmp_hum_ec_dev soli_tmp_hum_ec_dev_1_data;
static struct soli_tmp_hum_ec_dev soli_tmp_hum_ec_dev_2_data;
static struct soli_tmp_hum_ec_dev soli_tmp_hum_ec_dev_3_data;
static struct soli_tmp_hum_ec_dev soli_tmp_hum_ec_dev_4_data;
static struct soli_tmp_hum_ec_dev soli_tmp_hum_ec_dev_5_data;
static struct bds_gps_dev bds_gps_dev_data;
static struct npk_ph_dev npk_ph_dev_data;

static void read_all_sensor(void)
{
    /* 风速变送器 */
    if (read_wind_speed_dev(WIND_SPEED_DEV0_ADDR, &wind_speed_dev_data) != RT_EOK)
    {
        rt_kprintf("wind_speed_dev_data error\n");
        memcpy(&wind_speed_dev_data.wind_speed, &rs485_no_answer[0], 9);
        wind_speed_dev_data.data_num = 9;
        wind_speed_dev_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("wind_speed_dev_data ok\n");
    }
    /* 风向变送器 */
    if (read_wind_dir_dev(WIND_DIR_DEV0_ADDR, &wind_dir_dev_data) != RT_EOK)
    {
        rt_kprintf("wind_dir_dev_data error\n");
        memcpy(&wind_dir_dev_data.wind_dir, &rs485_no_answer[0], 9);
        wind_dir_dev_data.data_num = 9;
        wind_dir_dev_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("wind_dir_dev_data ok\n");
    }
    /* 太阳能控制器 */
    if (read_solar_ctrl_dev(SOLAR_CRTL_DEV0_ADDR, &solar_ctrl_dev_data) != RT_EOK)
    {
        rt_kprintf("solar_ctrl_dev_data error\n");
        memcpy(&solar_ctrl_dev_data.solarctrl_errCode, &rs485_no_answer[0], 9);
        solar_ctrl_dev_data.data_num = 9;
        solar_ctrl_dev_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("solar_ctrl_dev_data ok\n");
    }
    /* 温湿度变送器 */
    if (read_tmp_hum_dev(TMP_HUM_DEV0_ADDR, &tmp_hum_dev_data) != RT_EOK)
    {
        rt_kprintf("tmp_hum_dev_data error\n");
        memcpy(&tmp_hum_dev_data.airTmp, &rs485_no_answer[0], 9);
        tmp_hum_dev_data.data_num = 9;
        tmp_hum_dev_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("tmp_hum_dev_data ok\n");
    }
		/* 氮磷钾PH变送器 */
		if ( read_npk_ph_dev(NPK_PH_DEV0_ADDR, &npk_ph_dev_data) != RT_EOK)
		{
				rt_kprintf("npk_ph_dev_data error\n");
				memcpy( &npk_ph_dev_data.n, &rs485_no_answer[0], 9);
				npk_ph_dev_data.data_num = 9;
				npk_ph_dev_data.devErrCode = 1;
		}
		else
		{
				rt_kprintf("npk_ph_dev_data ok\n");
		}
    /* 雨量计 */
    if (read_rainfall_dev(RAINFALL_DEV0_ADDR, &rainfall_dev_data) != RT_EOK)
    {
        rt_kprintf("rainfall_dev_data error\n");
        memcpy(&rainfall_dev_data.rainfall, &rs485_no_answer[0], 9);
        rainfall_dev_data.data_num = 9;
        rainfall_dev_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("rainfall_dev_data ok\n");
    }
    /* 紫外线变送器 */
    if (read_uv_dev(UV_DEV0_ADDR, &uv_dev_data) != RT_EOK)
    {
        rt_kprintf("uv_dev_data error\n");
        memcpy(&uv_dev_data.uvIntensity, &rs485_no_answer[0], 9);
        uv_dev_data.data_num = 9;
        uv_dev_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("uv_dev_data ok\n");
    }
    /* 大气压_光照强度_PM2.5_PM10 */
    if (read_pa_lux_pm2510_dev(PA_LUX_PM2510_DEV0_ADDR, &pa_lux_pm2510_dev_data) != RT_EOK)
    {
        rt_kprintf("pa_lux_pm2510_dev_data error\n");
        memcpy(&pa_lux_pm2510_dev_data.PM2_5, &rs485_no_answer[0], 9);
        pa_lux_pm2510_dev_data.data_num = 9;
        pa_lux_pm2510_dev_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("pa_lux_pm2510_dev_data ok\n");
    }
    /* 土壤温湿度电导率 */
    if (read_soli_tmp_hum_ec_dev(SOLI_TMP_HUM_EC_DEV0_ADDR, &soli_tmp_hum_ec_dev_0_data) != RT_EOK)
    {
        rt_kprintf("soli_tmp_hum_ec_dev_0_data error\n");
        memcpy(&soli_tmp_hum_ec_dev_0_data.water, &rs485_no_answer[0], 9);
        soli_tmp_hum_ec_dev_0_data.data_num = 9;
        soli_tmp_hum_ec_dev_0_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("soli_tmp_hum_ec_dev_0_data ok\n");
    }
    if (read_soli_tmp_hum_ec_dev(SOLI_TMP_HUM_EC_DEV1_ADDR, &soli_tmp_hum_ec_dev_1_data) != RT_EOK)
    {
        rt_kprintf("soli_tmp_hum_ec_dev_1_data error\n");
        memcpy(&soli_tmp_hum_ec_dev_1_data.water, &rs485_no_answer[0], 9);
        soli_tmp_hum_ec_dev_1_data.data_num = 9;
        soli_tmp_hum_ec_dev_1_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("soli_tmp_hum_ec_dev_1_data ok\n");
    }
    if (read_soli_tmp_hum_ec_dev(SOLI_TMP_HUM_EC_DEV2_ADDR, &soli_tmp_hum_ec_dev_2_data) != RT_EOK)
    {
        rt_kprintf("soli_tmp_hum_ec_dev_2_data error\n");
        memcpy(&soli_tmp_hum_ec_dev_2_data.water, &rs485_no_answer[0], 9);
        soli_tmp_hum_ec_dev_2_data.data_num = 9;
        soli_tmp_hum_ec_dev_2_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("soli_tmp_hum_ec_dev_2_data ok\n");
    }
    if (read_soli_tmp_hum_ec_dev(SOLI_TMP_HUM_EC_DEV3_ADDR, &soli_tmp_hum_ec_dev_3_data) != RT_EOK)
    {
        rt_kprintf("soli_tmp_hum_ec_dev_3_data error\n");
        memcpy(&soli_tmp_hum_ec_dev_3_data.water, &rs485_no_answer[0], 9);
        soli_tmp_hum_ec_dev_3_data.data_num = 9;
        soli_tmp_hum_ec_dev_3_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("soli_tmp_hum_ec_dev_3_data ok\n");
    }
    if (read_soli_tmp_hum_ec_dev(SOLI_TMP_HUM_EC_DEV4_ADDR, &soli_tmp_hum_ec_dev_4_data) != RT_EOK)
    {
        rt_kprintf("soli_tmp_hum_ec_dev_4_data error\n");
        memcpy(&soli_tmp_hum_ec_dev_4_data.water, &rs485_no_answer[0], 9);
        soli_tmp_hum_ec_dev_4_data.data_num = 9;
        soli_tmp_hum_ec_dev_4_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("soli_tmp_hum_ec_dev_4_data ok\n");
    }
    if (read_soli_tmp_hum_ec_dev(SOLI_TMP_HUM_EC_DEV5_ADDR, &soli_tmp_hum_ec_dev_5_data) != RT_EOK)
    {
        rt_kprintf("soli_tmp_hum_ec_dev_5_data error\n");
        memcpy(&soli_tmp_hum_ec_dev_5_data.water, &rs485_no_answer[0], 9);
        soli_tmp_hum_ec_dev_5_data.data_num = 9;
        soli_tmp_hum_ec_dev_5_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("soli_tmp_hum_ec_dev_5_data ok\n");
    }
    /* BDS & GPS */
    if (bds_gps_get_gga(&bds_gps_dev_data) != RT_EOK)
    {
        rt_kprintf("bds_gps_dev_data error\n");
        memcpy(&bds_gps_dev_data.gga, &rs485_no_answer[0], 9);
        bds_gps_dev_data.data_num = 9;
        bds_gps_dev_data.devErrCode = 1;
    }
    else
    {
        rt_kprintf("bds_gps_dev_data ok\n");
    }
}

/**
 * 上传系统参数
 * 
 * @return the error code, 0 on subscribe successfully.
 */
rt_err_t upload_dev_argc(void)
{
    uint32_t i,len;
    uint8_t buf[32];
    uint16_t _buf[6];

    memcpy(&_buf[0], &dev_argc.cpu_tmp, 12);
    _buf[0] = swap_16(_buf[0]);
    _buf[1] = swap_16(_buf[1]);
    _buf[2] = swap_16(_buf[2]);
    _buf[3] = swap_16(_buf[3]);
    _buf[4] = swap_16(_buf[4]);
    _buf[5] = swap_16(_buf[5]);
    len = create_update_pack(&buf[0], 0xFF, 1, 0, (uint8_t *)&_buf[0], 6);
    // if (len)
    // {
    //     p = &buf[0];
    //     for (i = 0; i < len; i++)
    //     {
    //         rt_kprintf("%02X ", p[i]);
    //     }
    //     p = NULL;
    //     rt_kprintf("\n");
    // }
    if (len)
    {
        rt_kprintf("mqtt dev_argc\t\t\t\t");
        i = my_mqtt_public(&client, QOS0, BSP_USE_MQTT_PUBLIC_TOPIC, &buf[0], len);
        if (i != RT_EOK)
            rt_kprintf("[error]\n");
        else
            rt_kprintf("[ok]\n");
        return i;
    }
    return RT_ERROR;
}

/**
 * 上传485传感器数据
 * 
 * @param deviceType 设备类型
 * @param deviceNum 设备编号
 * @param err_code 错误码
 * @param data 数据
 * @param data_len 数据个数
 * 
 * @return the error code, 0 on subscribe successfully.
 */
rt_err_t upload_485_sensor_data(uint8_t deviceType, uint8_t deviceNum, uint8_t err_code, uint8_t *data, uint16_t data_len)
{
    // uint32_t i;
    uint32_t len;
    uint8_t buf[128];

    len = create_update_pack(&buf[0], deviceType, deviceNum, err_code, data, data_len);
    // if (len)
    // {
    //     p = &buf[0];
    //     for (i = 0; i < len; i++)
    //     {
    //         rt_kprintf("%02X ", p[i]);
    //     }
    //     p = NULL;
    //     rt_kprintf("\n");
    // }
    if (len)
        return my_mqtt_public(&client, QOS0, BSP_USE_MQTT_PUBLIC_TOPIC, &buf[0], len);
    return RT_ERROR;
}

/**
 * 周期上报
 * 
 * @param parameter
 */
void periodic_reporting(void)
{
    read_all_sensor();

    /* 风速变送器 */
    rt_kprintf("mqtt wind_speed_dev_data\t\t");
    if (upload_485_sensor_data(
                                1, 
                                1, 
                                (1?wind_speed_dev_data.devErrCode:0), 
                                (uint8_t *)&wind_speed_dev_data.wind_speed, 
                                wind_speed_dev_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    /* 太阳能控制器 */
    rt_kprintf("mqtt solar_ctrl_dev_data\t\t");
    if (upload_485_sensor_data(
                                2, 
                                1, 
                                (1?solar_ctrl_dev_data.devErrCode:0), 
                                (uint8_t *)&solar_ctrl_dev_data.solarctrl_errCode, 
                                solar_ctrl_dev_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    /* 温湿度变送器 */
    rt_kprintf("mqtt tmp_hum_dev_data\t\t\t");
    if (upload_485_sensor_data(
                                3, 
                                1, 
                                (1?tmp_hum_dev_data.devErrCode:0), 
                                (uint8_t *)&tmp_hum_dev_data.airTmp, 
                                tmp_hum_dev_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }
		/* 氮磷钾PH变送器 */
		rt_kprintf("mqtt npk_ph_dev_data\t\t\t");
		if (upload_485_sensor_data(
                                4, 
                                1, 
                                (1?npk_ph_dev_data.devErrCode:0), 
                                (uint8_t *)&npk_ph_dev_data.n, 
                                npk_ph_dev_data.data_num) == RT_EOK)
		
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }
		
    /* 雨量计 */
    rt_kprintf("mqtt rainfall_dev_data\t\t\t");
    if (upload_485_sensor_data(
                                5, 
                                1, 
                                (1?rainfall_dev_data.devErrCode:0), 
                                (uint8_t *)&rainfall_dev_data.rainfall, 
                                rainfall_dev_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    /* 紫外线变送器 */
    rt_kprintf("mqtt uv_dev_data\t\t\t");
    if (upload_485_sensor_data(
                                6, 
                                1, 
                                (1?uv_dev_data.devErrCode:0), 
                                (uint8_t *)&uv_dev_data.uvIntensity, 
                                uv_dev_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    /* 风向变送器 */
    rt_kprintf("mqtt wind_dir_dev_data\t\t\t");
    if (upload_485_sensor_data(
                                7, 
                                1, 
                                (1?wind_dir_dev_data.devErrCode:0), 
                                (uint8_t *)&wind_dir_dev_data.wind_dir, 
                                wind_dir_dev_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    /* 土壤温湿度电导率 */
    rt_kprintf("mqtt soli_tmp_hum_ec_dev_0_data\t\t");
    if (upload_485_sensor_data(
                                8, 
                                0, 
                                (1?soli_tmp_hum_ec_dev_0_data.devErrCode:0), 
                                (uint8_t *)&soli_tmp_hum_ec_dev_0_data.water, 
                                soli_tmp_hum_ec_dev_0_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    rt_kprintf("mqtt soli_tmp_hum_ec_dev_1_data\t\t");
    if (upload_485_sensor_data(
                                8, 
                                1, 
                                (1?soli_tmp_hum_ec_dev_1_data.devErrCode:0), 
                                (uint8_t *)&soli_tmp_hum_ec_dev_1_data.water, 
                                soli_tmp_hum_ec_dev_1_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    rt_kprintf("mqtt soli_tmp_hum_ec_dev_2_data\t\t");
    if (upload_485_sensor_data(
                                8, 
                                2, 
                                (1?soli_tmp_hum_ec_dev_2_data.devErrCode:0), 
                                (uint8_t *)&soli_tmp_hum_ec_dev_2_data.water, 
                                soli_tmp_hum_ec_dev_2_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    rt_kprintf("mqtt soli_tmp_hum_ec_dev_3_data\t\t");
    if (upload_485_sensor_data(
                                8, 
                                3, 
                                (1?soli_tmp_hum_ec_dev_3_data.devErrCode:0), 
                                (uint8_t *)&soli_tmp_hum_ec_dev_3_data.water, 
                                soli_tmp_hum_ec_dev_3_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    rt_kprintf("mqtt soli_tmp_hum_ec_dev_4_data\t\t");
    if (upload_485_sensor_data(
                                8, 
                                4, 
                                (1?soli_tmp_hum_ec_dev_4_data.devErrCode:0), 
                                (uint8_t *)&soli_tmp_hum_ec_dev_4_data.water, 
                                soli_tmp_hum_ec_dev_4_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    rt_kprintf("mqtt soli_tmp_hum_ec_dev_5_data\t\t");
    if (upload_485_sensor_data(
                                8, 
                                5, 
                                (1?soli_tmp_hum_ec_dev_5_data.devErrCode:0), 
                                (uint8_t *)&soli_tmp_hum_ec_dev_5_data.water, 
                                soli_tmp_hum_ec_dev_5_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    /* 大气压_光照强度_PM2.5_PM10 */
    rt_kprintf("mqtt pa_lux_pm2510_dev_data\t\t");
    if (upload_485_sensor_data(
                                9, 
                                1, 
                                (1?pa_lux_pm2510_dev_data.devErrCode:0), 
                                (uint8_t *)&pa_lux_pm2510_dev_data.PM2_5, 
                                pa_lux_pm2510_dev_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    /* BDS & GPS */
    rt_kprintf("mqtt bds_gps_dev_data\t\t\t");
    if (upload_485_sensor_data(
                                10, 
                                1, 
                                (1?bds_gps_dev_data.devErrCode:0), 
                                (uint8_t *)&bds_gps_dev_data.gga, 
                                bds_gps_dev_data.data_num) == RT_EOK)
    {
            rt_kprintf("[ok]\n");
    }
    else
    {
        rt_kprintf("[error]\n");
    }

    /* 上传系统参数 */
    upload_dev_argc();

}

/**
 * 变动上报
 * 
 * @param parameter
 */
void change_eporting(void)
{
    
}

/**
 * 数据上报模式
 * 
 * @param parameter
 */
static void upload_data(void *parameter)
{
    static time_t curTime;
    struct tm _time;

    while (1)
    {
        /* 周期上报模式 */
        if (dev_argc.mqtt_upload_mode == 0)
        {
            if (time(NULL) >= (curTime + dev_argc.mqtt_upload_interval))
            {
                
                curTime = time(NULL);
                localtime_r(&curTime, &_time);
                rt_kprintf("----------\nstart upload %04d-%02d-%02d %02d:%02d:%02d\n",
                        _time.tm_year + 1900,_time.tm_mon + 1,_time.tm_mday,
                        _time.tm_hour,_time.tm_min,_time.tm_sec);
                
                periodic_reporting();
                response_dev_argc_flag = 0;
            }

//            rt_thread_mdelay(1000);
        }
        /* 变动上报模式 */
        else if (dev_argc.mqtt_upload_mode == 1)
        {
            change_eporting();
        }
        
        if (response_dev_argc_flag == 1)
        {
            rt_kprintf("response upload data ...\n");
            periodic_reporting();
            response_dev_argc_flag = 0;
        }

        /* 下载并校验固件 */
        if (update_firmwware_flag == 1)
        {
            rt_kprintf("Download firmware to flash...\n");
            if (http_ota_fw_download(uri_buf) == RT_EOK)
            {
                rt_kprintf("Checksum firmware ...");
                if (check_firmware() != RT_EOK)
                {
                    rt_kprintf("[faild]\n");
                }
                else
                {
                    rt_kprintf("[ok]\n");
//                    _boot_update = 0x5555AAAA;
                  
                    const struct fal_partition *partition = partition = fal_partition_find("app");
                    if (partition == RT_NULL)
                    {
                        rt_kprintf("flash not find\n");
                        return;
                    }
                    int ret = fal_partition_erase(partition, 0, 1024);
                    if (ret < 0)
                    {
                        rt_kprintf("Partition (%s) erase failed!", partition->name);
                    }
                    rt_kprintf("will reset cpu ...\r\n");
                    rt_hw_cpu_reset();
                }
            }
            else
            {
                rt_kprintf("[faild]\n");
//                _boot_update = 0xBEADBEAD;
            }
            update_firmwware_flag = 0;
        }
    }
}

int start_my_server(void)
{
    rt_thread_t tid;    //线程句柄

//    _boot_update = 0xBEADBEAD;
  
    tid = rt_thread_create("up_server",
                            upload_data,
                            RT_NULL,
                            2048,
                            12, 10);
    if(tid != RT_NULL)
    {
        rt_thread_startup(tid);
        rt_kprintf("upload_server init ok\n");
    }
    else
    {
        rt_kprintf("upload_server init failed! %d\n", tid);
        return RT_ERROR;
    }

    return RT_EOK;
}
