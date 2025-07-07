#include "my_mqtt.h"
#include "crc16.h"
#include "dev_addr.h"
#include "TinyModbusRTU.h"
#include <stdlib.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>
#include <hw_crc.h>
#include <hwcrypto.h>
#include <at_device.h>
#include <netdev.h>
#include "my_wdt.h"
#include <string.h>
#include "my_http.h"
#include "my_flash.h"

#include <time.h>

volatile uint8_t wdt_allow_refush = 1;

void my_mqtt_thread_entry(void *parameter);
char device_id[12] = {'?','?','?','?','?','?','?','?','?','?','?','?'};

uint8_t sensor_pwr_status;
struct MQTTClient client;
const char *mqtt_client_id = &device_id[0];

uint8_t response_dev_argc_flag;
volatile uint8_t update_firmwware_flag;

#define swap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

union pkg_data_format {uint16_t data16;uint32_t data32;};

struct dev_run_argc dev_argc = {
    .mqtt_upload_mode = 0,
    .video_limit_pwr = 80,
    .cpu_tmp = 157,
    .cpu_fan_tmp = 40,
    .video_pwr_status = 0,
    .video_up_pwr_time = 0x0830,
    .video_down_pwr_time = 0x1819,
    .cache_rainfall_val = 0x0000,
    .mqtt_upload_interval = 3000
};


#ifdef BSP_DEV_RUN_USE_DARGC_CMD
void dargc(void)
{
    rt_kprintf("cpu_tmp = 0x%X %d\n", dev_argc.cpu_tmp, dev_argc.cpu_tmp);
    rt_kprintf("mqtt_upload_mode = 0x%X %d\n", dev_argc.mqtt_upload_mode, dev_argc.mqtt_upload_mode);
    rt_kprintf("mqtt_upload_interval = 0x%X %d\n", dev_argc.mqtt_upload_interval, dev_argc.mqtt_upload_interval);
    rt_kprintf("cpu_fan_tmp = 0x%X %d\n", dev_argc.cpu_fan_tmp, dev_argc.cpu_fan_tmp);
    rt_kprintf("video_limit_pwr = 0x%X %d\n", dev_argc.video_limit_pwr, dev_argc.video_limit_pwr);
    rt_kprintf("video_pwr_status = 0x%X %d\n", dev_argc.video_pwr_status, dev_argc.video_pwr_status);
    rt_kprintf("video_up_pwr_time = 0x%X %d\n", dev_argc.video_up_pwr_time, dev_argc.video_up_pwr_time);
    rt_kprintf("video_down_pwr_time = 0x%X %d\n", dev_argc.video_down_pwr_time, dev_argc.video_down_pwr_time);
    rt_kprintf("cache_rainfall_val = 0x%X %d\n", dev_argc.cache_rainfall_val, dev_argc.cache_rainfall_val);
}
MSH_CMD_EXPORT(dargc, print dev run argc);
#endif

/**
 * 获取Air724UG 4G模组MAC地址
 * 
 */
void get_mac(void)
{
    struct netdev *cur_netdev_list = netdev_list;
    struct at_device *device = RT_NULL;
    at_response_t resp = RT_NULL;
    uint8_t recv_mac[6] = {0};

    if (cur_netdev_list == RT_NULL)
    {
        goto __GMAC_ERROR_EXIT;
    }

    device = at_device_get_by_name(AT_DEVICE_NAMETYPE_NETDEV, netdev_default->name);
    if (device == RT_NULL)
    {
        goto __GMAC_ERROR_EXIT;
    }

    resp = at_create_resp(64, 0, 300);
    if (resp == RT_NULL)
    {
        goto __GMAC_ERROR_EXIT;
    }

    if (at_obj_exec_cmd(device->client, resp, "AT+MIFIMAC=R") < 0)
    {
        at_delete_resp(resp);
        goto __GMAC_ERROR_EXIT;
    }

    if (at_resp_parse_line_args_by_kw(resp, "MAC:", "%*[^:]%*s%x:%x:%x:%x:%x:%x",
                                        &recv_mac[0], &recv_mac[1], &recv_mac[2], &recv_mac[3], 
                                        &recv_mac[4], &recv_mac[5]) > 0)
    {
        sprintf(device_id, "%02X%02X%02X%02X%02X%02X", recv_mac[0], recv_mac[1], recv_mac[2], recv_mac[3], 
                                        recv_mac[4], recv_mac[5]);
    }

    if (resp)
    {
        at_delete_resp(resp);
        return;
    }

    __GMAC_ERROR_EXIT:
        memset(device_id, '?', sizeof(device_id)-1);
        device_id[sizeof(device_id)-1] = 0;
}

/**
 * 创建一个上传数据包
 * 
 * @param buf 存储报文序列
 * @param deviceType 设备类型
 * @param deviceNum 设备编号
 * @param err_code 错误码
 * @param data 数据
 * @param data_len 数据个数
 * 
 * @return packet size if success, else 0
 */
int create_update_pack(uint8_t *buf, uint8_t deviceType, uint8_t deviceNum, uint8_t err_code, uint8_t *data, uint16_t data_len)
{
    uint16_t i,j;
    struct rt_hwcrypto_ctx *ctx = NULL;

    for (i = 0; i < 12; i++)
        buf[i] = mqtt_client_id[i];

    buf[i++] = deviceType;
    buf[i++] = deviceNum;

    buf[i++] = data_len >> 8;
    buf[i++] = data_len & 0x00FF;
    buf[i++] = err_code;
    if (err_code == 0 && deviceType != 9)
        data_len *= 2;
    // rt_kprintf("err_code:%d data_len:%d\n", err_code, data_len);
    for (j = 0; j < data_len; j++)
        buf[i++] = data[j];

    /* 生成校验 */
    ctx = rt_hwcrypto_crc_create(rt_hwcrypto_dev_default(), HWCRYPTO_CRC_CRC16);
    if (ctx == RT_NULL)
        return 0;
    rt_hwcrypto_crc_cfg(ctx, (struct hwcrypto_crc_cfg *)&crc16_cfg);
    j = rt_hwcrypto_crc_update(ctx, &buf[12], 1+1+2+1+data_len);
    rt_hwcrypto_crc_destroy(ctx);
    if (j != 0)
    {
        buf[i++] = j >> 8;
        buf[i++] = j & 0x00FF;
        return i;
    }

    return 0;
}

/**
 * 发布一条消息
 * 
 * @param _client
 * @param qos QOS(only 0/1/2)
 * @param payload 
 * @param payloadlen payload length
 * 
 * @return the error code, 0 on subscribe successfully.
 */
int my_mqtt_public(MQTTClient *_client, enum QoS qos, const char *topic, const uint8_t *payload, uint16_t payloadlen)
{
    MQTTMessage message;

    message.qos = qos;
    message.retained = 0;
    message.payload = (void *)payload;
    message.payloadlen = payloadlen;

    return MQTTPublish(_client, topic, &message);
}

/**
 * 根据收到的下行指令执行对应的动作
 * 
 * @param p_pkg mqtt下行报文
 */
char uri_buf[sizeof(update_uri_header)+sizeof(device_id)+BSP_FIRMWARE_UPDATE_FILE_NAME_LEN];
static void do_mqtt_cmd(struct my_mqtt_pkg *p_pkg)
{
    union pkg_data_format tmp;
    char *buf = uri_buf;
    uint16_t addr;
    rt_err_t status;

    response_dev_argc_flag = 0;

    #ifdef BSP_USE_MQTT_DEBUG
    rt_kprintf("命令:%02X\n功能:", p_pkg->cmd);
    #endif

    if (p_pkg->cmd == 0x1)
    {
        if (p_pkg->data_len < BSP_FIRMWARE_UPDATE_FILE_NAME_LEN)
        {
            p_pkg->data_buf[p_pkg->data_len] = 0;
            sprintf(buf, "%s%s/", update_uri_header, device_id);
            memcpy(&buf[sizeof(update_uri_header)+sizeof(device_id)], &p_pkg->data_buf[0], p_pkg->data_len);
            buf[sizeof(update_uri_header)+sizeof(device_id)+p_pkg->data_len] = '\0';
            memcpy(uri_buf, buf, sizeof(update_uri_header)+sizeof(device_id)+p_pkg->data_len+1);
            
            #ifdef BSP_USE_MQTT_DEBUG
            rt_kprintf("更新IoT设备的固件 \n参数:%s\r\n", (char *)&uri_buf[0]);
            #endif

            update_firmwware_flag = 1;
        }
        #ifdef BSP_USE_MQTT_DEBUG
        else
        {
            rt_kprintf("文件名长度超出缓冲区大小!\r\n");
        }
        #endif
    }
    else if (p_pkg->cmd == 0x2)
    {
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("系统控制\n参数:%X ", p_pkg->data_buf[0]);
        #endif

        if (p_pkg->data_buf[0] == 1)
        {
            VIDEO_PWR_OFF
            // POWER_IN_OFF
            rt_thread_mdelay(1600);
            // POWER_IN_ON
            SENSOR_PWR_OFF
            upload_dev_argc();
            write_to_flash(
                BSP_DEV_RUN_USE_SECTION_NAME, 
                BSP_DEV_RUN_USE_SECTION_OFFSET, 
                (uint8_t *)&dev_argc, 
                sizeof(dev_argc));

            #ifdef BSP_USE_MQTT_DEBUG
            rt_kprintf("(关机)\n");
            #endif
        }
        else if (p_pkg->data_buf[0] == 2) {
            VIDEO_PWR_OFF
            rt_thread_mdelay(1000);
            SENSOR_PWR_OFF
            upload_dev_argc();
            POWER_IN_OFF
            paho_mqtt_stop(&client);
            rt_thread_mdelay(1600);
            POWER_IN_ON
            write_to_flash(
                BSP_DEV_RUN_USE_SECTION_NAME, 
                BSP_DEV_RUN_USE_SECTION_OFFSET, 
                (uint8_t *)&dev_argc, 
                sizeof(dev_argc));

            #ifdef BSP_USE_MQTT_DEBUG
            rt_kprintf("(重启)\n");
            #endif

            wdt_allow_refush = 0;
            rt_hw_cpu_reset();
        }
    }
    else if (p_pkg->cmd == 0x3)
    {
        VIDEO_PWR_OFF
        rt_thread_mdelay(1000);
        SENSOR_PWR_OFF
        upload_dev_argc();
        POWER_IN_OFF
        rt_thread_mdelay(1600);
        POWER_IN_ON
        write_to_flash(
            BSP_DEV_RUN_USE_SECTION_NAME, 
            BSP_DEV_RUN_USE_SECTION_OFFSET, 
            (uint8_t *)&dev_argc, 
            sizeof(dev_argc));

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("重启IoT服务程序\n");
        #endif

        wdt_allow_refush = 0;
        rt_hw_cpu_reset();
    }
    else if (p_pkg->cmd == 0x4)
    {
        response_dev_argc_flag = 1;

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置数据上报模式\n参数:%X ", p_pkg->data_buf[0]);
        #endif

        if (p_pkg->data_buf[0] == 0 || p_pkg->data_buf[0] == 1) {
            dev_argc.mqtt_upload_mode = p_pkg->data_buf[0];
            write_to_flash(
                BSP_DEV_RUN_USE_SECTION_NAME, 
                BSP_DEV_RUN_USE_SECTION_OFFSET, 
                (uint8_t *)&dev_argc, 
                sizeof(dev_argc));

            #ifdef BSP_USE_MQTT_DEBUG
            if (dev_argc.mqtt_upload_mode == 0)
                rt_kprintf("(周期上报)\n");
            else
                rt_kprintf("(变动上报)\n");
            #endif
        }
    }
    else if (p_pkg->cmd == 0x5)
    {
        response_dev_argc_flag = 1;
        p_pkg->data_buf[4] = p_pkg->data_buf[0];
        p_pkg->data_buf[0] = p_pkg->data_buf[3];
        p_pkg->data_buf[3] = p_pkg->data_buf[4];
        p_pkg->data_buf[4] = p_pkg->data_buf[1];
        p_pkg->data_buf[1] = p_pkg->data_buf[2];
        p_pkg->data_buf[2] = p_pkg->data_buf[4];
        tmp.data16 = *(uint16_t *)&p_pkg->data_buf[0];
        dev_argc.mqtt_upload_interval = tmp.data16;
        write_to_flash(
            BSP_DEV_RUN_USE_SECTION_NAME, 
            BSP_DEV_RUN_USE_SECTION_OFFSET, 
            (uint8_t *)&dev_argc, 
            sizeof(dev_argc));
        
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置IoT设备上报传感器数据的时间间隔 \n参数:%d\n", dev_argc.mqtt_upload_interval);
        #endif
    }
    else if (p_pkg->cmd == 0x20)
    {
        response_dev_argc_flag = 1;
        
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("清除雨量计计数值 \n");
        #endif
        
        modbus_rtu_write_reg(RAINFALL_DEV0_ADDR, RAINFALL_CLEAN_DATA, 0);
    }
    else if (p_pkg->cmd == 0x21)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(TMP_HUM_DEV0_ADDR, TMP_HUM_ADJ_TMP, tmp.data16);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置温湿度传感器的温度补偿值 \n参数:%d\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x22)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(TMP_HUM_DEV0_ADDR, TMP_HUM_ADJ_HUM, tmp.data16);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置温湿度传感器的湿度补偿值 \n参数:%d\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x23)
    {
        if (p_pkg->dev_num == 0)
            addr = SOLI_TMP_HUM_EC_DEV0_ADDR;
        else if (p_pkg->dev_num == 1)
            addr = SOLI_TMP_HUM_EC_DEV1_ADDR;
        else if (p_pkg->dev_num == 2)
            addr = SOLI_TMP_HUM_EC_DEV2_ADDR;
        else if (p_pkg->dev_num == 3)
            addr = SOLI_TMP_HUM_EC_DEV3_ADDR;
        else if (p_pkg->dev_num == 4)
            addr = SOLI_TMP_HUM_EC_DEV4_ADDR;
        else if (p_pkg->dev_num == 5)
            addr = SOLI_TMP_HUM_EC_DEV5_ADDR;
        else
            return;
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(addr, SOLI_TMP_HUM_EC_ECTMP, tmp.data16);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置土壤传感器电导温度系数 \r\n参数:%d\r\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x24)
    {
        if (p_pkg->dev_num == 0)
            addr = SOLI_TMP_HUM_EC_DEV0_ADDR;
        else if (p_pkg->dev_num == 1)
            addr = SOLI_TMP_HUM_EC_DEV1_ADDR;
        else if (p_pkg->dev_num == 2)
            addr = SOLI_TMP_HUM_EC_DEV2_ADDR;
        else if (p_pkg->dev_num == 3)
            addr = SOLI_TMP_HUM_EC_DEV3_ADDR;
        else if (p_pkg->dev_num == 4)
            addr = SOLI_TMP_HUM_EC_DEV4_ADDR;
        else if (p_pkg->dev_num == 5)
            addr = SOLI_TMP_HUM_EC_DEV5_ADDR;
        else
            return;
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(addr, SOLI_TMP_HUM_EC_SALT_COEFFICIENT, tmp.data16);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置土壤传感器盐度系数 \r\n参数:%d\r\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x25)
    {
        if (p_pkg->dev_num == 0)
            addr = SOLI_TMP_HUM_EC_DEV0_ADDR;
        else if (p_pkg->dev_num == 1)
            addr = SOLI_TMP_HUM_EC_DEV1_ADDR;
        else if (p_pkg->dev_num == 2)
            addr = SOLI_TMP_HUM_EC_DEV2_ADDR;
        else if (p_pkg->dev_num == 3)
            addr = SOLI_TMP_HUM_EC_DEV3_ADDR;
        else if (p_pkg->dev_num == 4)
            addr = SOLI_TMP_HUM_EC_DEV4_ADDR;
        else if (p_pkg->dev_num == 5)
            addr = SOLI_TMP_HUM_EC_DEV5_ADDR;
        else
            return;
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(addr, SOLI_TMP_HUM_EC_TDS_COEFFICIENT, tmp.data16);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置土壤传感器TDS系数 \r\n参数:%d\r\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x26)
    {
        if (p_pkg->dev_num == 0)
            addr = SOLI_TMP_HUM_EC_DEV0_ADDR;
        else if (p_pkg->dev_num == 1)
            addr = SOLI_TMP_HUM_EC_DEV1_ADDR;
        else if (p_pkg->dev_num == 2)
            addr = SOLI_TMP_HUM_EC_DEV2_ADDR;
        else if (p_pkg->dev_num == 3)
            addr = SOLI_TMP_HUM_EC_DEV3_ADDR;
        else if (p_pkg->dev_num == 4)
            addr = SOLI_TMP_HUM_EC_DEV4_ADDR;
        else if (p_pkg->dev_num == 5)
            addr = SOLI_TMP_HUM_EC_DEV5_ADDR;
        else
            return;
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(addr, SOLI_TMP_HUM_EC_TMP_ADJ, tmp.data16);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置土壤传感器温度校准值 \r\n参数:%d\r\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x27)
    {
        if (p_pkg->dev_num == 0)
            addr = SOLI_TMP_HUM_EC_DEV0_ADDR;
        else if (p_pkg->dev_num == 1)
            addr = SOLI_TMP_HUM_EC_DEV1_ADDR;
        else if (p_pkg->dev_num == 2)
            addr = SOLI_TMP_HUM_EC_DEV2_ADDR;
        else if (p_pkg->dev_num == 3)
            addr = SOLI_TMP_HUM_EC_DEV3_ADDR;
        else if (p_pkg->dev_num == 4)
            addr = SOLI_TMP_HUM_EC_DEV4_ADDR;
        else if (p_pkg->dev_num == 5)
            addr = SOLI_TMP_HUM_EC_DEV5_ADDR;
        else
            return;
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(addr, SOLI_TMP_HUM_EC_WATER_ADJ, tmp.data16);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置土壤传感器含水率校准值 \r\n参数:%d\r\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x28)
    {
        if (p_pkg->dev_num == 0)
            addr = SOLI_TMP_HUM_EC_DEV0_ADDR;
        else if (p_pkg->dev_num == 1)
            addr = SOLI_TMP_HUM_EC_DEV1_ADDR;
        else if (p_pkg->dev_num == 2)
            addr = SOLI_TMP_HUM_EC_DEV2_ADDR;
        else if (p_pkg->dev_num == 3)
            addr = SOLI_TMP_HUM_EC_DEV3_ADDR;
        else if (p_pkg->dev_num == 4)
            addr = SOLI_TMP_HUM_EC_DEV4_ADDR;
        else if (p_pkg->dev_num == 5)
            addr = SOLI_TMP_HUM_EC_DEV5_ADDR;
        else
            return;
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(addr, SOLI_TMP_HUM_EC_EC_ADJ, tmp.data16);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置土壤传感器电导率校准值 \r\n参数:%d\r\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x29)
    {
        response_dev_argc_flag = 1;
        dev_argc.video_pwr_status = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("打开/关闭摄像头电源 \n参数:%X ", dev_argc.video_pwr_status);
        #endif

        if (dev_argc.video_pwr_status == 1) {
            VIDEO_PWR_ON

            #ifdef BSP_USE_MQTT_DEBUG
            rt_kprintf("(打开)\n");
            #endif
        }
        else {
            VIDEO_PWR_OFF

            #ifdef BSP_USE_MQTT_DEBUG
            rt_kprintf("(关闭)\n");
            #endif
        }
    }
    else if (p_pkg->cmd == 0x30)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_BAT_CAPACITY, tmp.data16);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("修改太阳能控制器-蓄电池容量 \n参数:%d\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x31)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_BAT_UNDER_VOL_POINT, tmp.data16);
        
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("修改太阳能控制器-欠压点 \n参数:%d\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x32)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_BAT_UNDER_VOL_RECV_POINT, tmp.data16);
        
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("修改太阳能控制器-欠压恢复点 \n参数:%d\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x33)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_BAT_FLOAT_CHARGE_POINT, tmp.data16);
        
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("修改太阳能控制器-浮充点 \n参数:%d\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x34)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_OVERLOAD_RECV_POINT, tmp.data16);
        
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("修改太阳能控制器-过压恢复点 \n参数:%d\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x35)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_BAT_OVERVOL_POINT, tmp.data16);
        
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("修改太阳能控制器-过压点 \n参数:%d\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x36)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_OVERLOAD_RECV_POINT, tmp.data16);
        
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("修改太阳能控制器-过载恢复点 \n参数:%d\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x37)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_OVERLOAD_POINT, tmp.data16);
        
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("修改太阳能控制器-过载点 \n参数:%d\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x38)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_TMP_COMPENSATION, tmp.data16);
        
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("修改太阳能控制器- 温度补偿 \n参数:%d\n", tmp.data16);
        #endif
    }
    else if (p_pkg->cmd == 0x41)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);
        
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("太阳能控制器-切换充电开关状态 \n参数:%X ", tmp.data16);
        #endif

        if (tmp.data16 == 1) {
            modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_BAT_CHARG_SWITCH, 1);
            
            #ifdef BSP_USE_MQTT_DEBUG
            rt_kprintf("(打开)\n");
            #endif
        }
        else if (tmp.data16 == 0) {
            modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_BAT_CHARG_SWITCH, 0);
            
            #ifdef BSP_USE_MQTT_DEBUG
            rt_kprintf("(关闭)\n");
            #endif
        }
    }
    else if (p_pkg->cmd == 0x42)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("太阳能控制器-切换MPPT开关状态 \n参数:%X ", tmp.data16);
        #endif

        if (tmp.data16 == 1) {
            modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_MPPT_SWITCH, 1);
            
            #ifdef BSP_USE_MQTT_DEBUG
            rt_kprintf("(打开)\n");
            #endif
        }
        else if (tmp.data16 == 0) {
            modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_MPPT_SWITCH, 0);
            
            #ifdef BSP_USE_MQTT_DEBUG
            rt_kprintf("(关闭)\n");
            #endif
        }
    }
    else if (p_pkg->cmd == 0x43)
    {
        response_dev_argc_flag = 1;
        tmp.data16 = swap_16(*(uint16_t *)&p_pkg->data_buf[0]);

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("太阳能控制器-切换负载开关状态 \n参数:%X ", tmp.data16);
        #endif

        if (tmp.data16 == 1) {
            modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_LOAD_SWITCH, 1);
            #ifdef BSP_USE_MQTT_DEBUG
            rt_kprintf("(打开)\n");
            #endif
        }
        else if (tmp.data16 == 0) {
            modbus_rtu_write_reg(SOLAR_CRTL_DEV0_ADDR, SOLAR_CRTL_LOAD_SWITCH, 0);
            
            #ifdef BSP_USE_MQTT_DEBUG
            rt_kprintf("(关闭)\n");
            #endif
        }
    }
    else if (p_pkg->cmd == 0x44)
    {
        response_dev_argc_flag = 1;
        dev_argc.cpu_fan_tmp = p_pkg->data_buf[0] * 10;
        write_to_flash(BSP_DEV_RUN_USE_SECTION_NAME, 
            BSP_DEV_RUN_USE_SECTION_OFFSET, 
            (uint8_t *)&dev_argc, 
            sizeof(dev_argc));

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置CPU散热温度 \n参数:%d\n", dev_argc.cpu_fan_tmp);
        #endif
    }
    else if (p_pkg->cmd == 0x45)
    {
        response_dev_argc_flag = 1;
        dev_argc.video_limit_pwr = p_pkg->data_buf[0];
        write_to_flash(
            BSP_DEV_RUN_USE_SECTION_NAME, 
            BSP_DEV_RUN_USE_SECTION_OFFSET, 
            (uint8_t *)&dev_argc, 
            sizeof(dev_argc));

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置摄像头断电阈值 \n参数:%d\n", dev_argc.video_limit_pwr);
        #endif
    }
    else if (p_pkg->cmd == 0x46)
    {
        response_dev_argc_flag = 1;
        p_pkg->data_buf[p_pkg->data_len] = 0;

        p_pkg->data_buf[2] = '\0';
        dev_argc.video_up_pwr_time = atoi((char *)&p_pkg->data_buf[0]);
        dev_argc.video_up_pwr_time <<= 8;
        dev_argc.video_up_pwr_time |= atoi((char *)&p_pkg->data_buf[3]);
        write_to_flash(
            BSP_DEV_RUN_USE_SECTION_NAME, 
            BSP_DEV_RUN_USE_SECTION_OFFSET, 
            (uint8_t *)&dev_argc, 
            sizeof(dev_argc));

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置给摄像头供电的时间 \n参数:%s 0x%X\n", (uint8_t *)&p_pkg->data_buf[0], dev_argc.video_up_pwr_time);
        #endif
    }
    else if (p_pkg->cmd == 0x47)
    {
        response_dev_argc_flag = 1;
        p_pkg->data_buf[p_pkg->data_len] = 0;

        p_pkg->data_buf[2] = '\0';
        dev_argc.video_down_pwr_time = atoi((char *)&p_pkg->data_buf[0]);
        dev_argc.video_down_pwr_time <<= 8;
        dev_argc.video_down_pwr_time |= atoi((char *)&p_pkg->data_buf[3]);
        write_to_flash(
            BSP_DEV_RUN_USE_SECTION_NAME, 
            BSP_DEV_RUN_USE_SECTION_OFFSET, 
            (uint8_t *)&dev_argc, 
            sizeof(dev_argc));

        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("设置给摄像头断电的时间 \n参数:%s 0x%X\n", (uint8_t *)&p_pkg->data_buf[0], dev_argc.video_down_pwr_time);
        #endif
    }
    else if (p_pkg->cmd == 0x48)
    {
        #ifdef BSP_USE_MQTT_DEBUG
        rt_kprintf("操作设备运行日志\n参数:%X ", p_pkg->data_buf[0]);
        if (p_pkg->data_buf[0] == 0) {
            rt_kprintf("(上传日志))\n");
        }
        else if (p_pkg->data_buf[0] == 1) {
            rt_kprintf("(清零日志)\n");
        }
        #endif   
    }
    #ifdef BSP_USE_MQTT_DEBUG
    else
    {
        rt_kprintf("\b\b\b\b未知的命令\r\n");
    }
    #endif
}

/* 收到订阅的主题的消息时的回调函数 */
static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    uint32_t i;
    rt_uint32_t crc;
    struct rt_hwcrypto_ctx *ctx = NULL;
    struct my_mqtt_pkg recv_pkg;

    /* 检查报文长度，忽略非法报文 */
    if (msg_data->message->payloadlen > 18)
    {
        memcpy((uint8_t *)&recv_pkg.data_buf[0],
                (uint8_t *)msg_data->message->payload,
                ((msg_data->message->payloadlen <= sizeof(recv_pkg.data_buf))
                ? msg_data->message->payloadlen
                : sizeof(recv_pkg.data_buf)));
        memcpy((char *)&recv_pkg.dev_id[0], (char *)&recv_pkg.data_buf[0], 12);
        recv_pkg.dev_id[12] = 0;
        if (!strcmp(&recv_pkg.dev_id[0], mqtt_client_id))
        {
            recv_pkg.dev_num = (*(((uint8_t *)msg_data->message->payload)+12));
            recv_pkg.data_len = swap_16((*(uint16_t *)(((uint8_t *)msg_data->message->payload)+13)));
            recv_pkg.cmd = (*(((uint8_t *)msg_data->message->payload)+15));
            recv_pkg.crc = swap_16(*(uint16_t *)(((uint8_t *)msg_data->message->payload)+15+recv_pkg.data_len+1));

            /* 下发的报文数据段长度大于缓冲区大小，丢弃此类数据 */
            if (recv_pkg.data_len > sizeof(recv_pkg.data_buf)) {
                #ifdef BSP_USE_MQTT_DEBUG
                rt_kprintf("mqtt recv data > data buffer(%d) size\n", sizeof(recv_pkg.data_buf));
                #endif
                return;
            }

            /* 校验报文 */
            ctx = rt_hwcrypto_crc_create(rt_hwcrypto_dev_default(), HWCRYPTO_CRC_CRC16);
            if (ctx == RT_NULL)
            {
                // rt_kprintf("ctx is NULL\r\n");
                return;
            }
            rt_hwcrypto_crc_cfg(ctx, (struct hwcrypto_crc_cfg *)&crc16_cfg);
            i = msg_data->message->payloadlen - 12 - 2;
            crc = rt_hwcrypto_crc_update(ctx,
                                        &recv_pkg.data_buf[12],
                                        i);
            rt_hwcrypto_crc_destroy(ctx);

            if (crc != 0 && crc == recv_pkg.crc)
            {
                memcpy((uint8_t *)&recv_pkg.data_buf[0],
                        (uint8_t *)&recv_pkg.data_buf[16],
                        (recv_pkg.data_len < sizeof(recv_pkg.data_buf)) ? (recv_pkg.data_len): (sizeof(recv_pkg.data_buf)));

                // rt_kprintf("设备识别码:%s\r\n", recv_pkg.dev_id);
                // rt_kprintf("设备编号:%d\r\n", recv_pkg.dev_num, recv_pkg.dev_num);
                // rt_kprintf("数据长度:%d\r\n", recv_pkg.data_len, recv_pkg.data_len);
                // rt_kprintf("命令:%XH\r\n", recv_pkg.cmd, recv_pkg.cmd);
                // rt_kprintf("CRC:%04XH %d\r\n", recv_pkg.crc, recv_pkg.crc);

                do_mqtt_cmd(&recv_pkg);
            }
            #ifdef BSP_USE_MQTT_DEBUG
            else
            {
                rt_kprintf("calc crc(%04X) or pkg crc(%04X) failed!\n", crc, recv_pkg.crc);
            }
            #endif
        }
    }
    #ifdef BSP_USE_MQTT_DEBUG
    rt_kprintf("\n-------------------\n");
    #endif
}

/* 默认的订阅回调函数，如果有订阅的 Topic 没有设置回调函数，则使用该默认回调函数 */
static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    #ifdef BSP_USE_MQTT_DEBUG
    uint32_t i;
    uint8_t *p = NULL;
    time_t curTime;
    struct tm _time;
    curTime = time(NULL);
            localtime_r(&curTime, &_time);
            rt_kprintf("%04d-%02d-%02d %02d:%02d:%02d mqtt_default_callback\n",
                        _time.tm_year + 1900,_time.tm_mon + 1,_time.tm_mday,
                        _time.tm_hour,_time.tm_min,_time.tm_sec);
    // *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    
    rt_kprintf("payloadlen:%d\n", msg_data->message->payloadlen);
    p = (uint8_t *)msg_data->message->payload;
    for (i = 0; i < msg_data->message->payloadlen; i++)
    {
        rt_kprintf("%X ", p[i]);
    }
    p = NULL;
    rt_kprintf("\n");
    #endif

    // *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    // rt_kprintf("0Receive topic: %.*s, message data:\r\n", msg_data->topicName->lenstring.len, msg_data->topicName->lenstring.data);
    // rt_kprintf("%.*s\r\n", msg_data->message->payloadlen, (char *)msg_data->message->payload);
}

/* 连接成功回调函数 */
static void mqtt_connect_callback(MQTTClient *c)
{
    rt_kprintf("mqtt connect success\n");
}

/* 上线回调函数 */
static void mqtt_online_callback(MQTTClient *c)
{
    rt_kprintf("mqtt online\n");
}

/* 下线回调函数 */
static void mqtt_offline_callback(MQTTClient *c)
{
    rt_kprintf("mqtt offline\n");

    // paho_mqtt_stop(&client);
    // my_mqtt_thread_entry(NULL);
}

/**
 * 初始化mqtt，配置连接参数
 * 
 * @param parameter
 */
void my_mqtt_thread_entry(void *parameter)
{
    client.isconnected = 0;
    client.uri = BSP_USE_MQTT_IP;

    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    memcpy(&client.condata, &condata, sizeof(condata));
    // client.condata.clientID.cstring = MQTT_CLIENTID;
    client.condata.clientID.cstring = device_id;
    client.condata.keepAliveInterval = 30;
    client.condata.cleansession = 1;
    // client.reconnect_interval = 10;
    // client.condata.username.cstring = (char*)BSP_USE_MQTT_USERNAME;
    // client.condata.password.cstring = (char*)BSP_USE_MQTT_PASSWORD;

    /* 为mqtt申请内存 */
    client.buf_size = client.readbuf_size = 256;
    client.buf = rt_calloc(1, client.buf_size);
    client.readbuf = rt_calloc(1, client.readbuf_size);
    if (!(client.buf && client.readbuf))
    {
        rt_kprintf("no memory for MQTT client buffer!\n");
        return;
    }

    /* 设置回调函数  */
    client.connect_callback = mqtt_connect_callback;
    client.online_callback = mqtt_online_callback;
    client.offline_callback = mqtt_offline_callback;

    /* 订阅命令主题，并设置其回调函数 */
    client.messageHandlers[0].topicFilter = rt_strdup(BSP_USE_MQTT_TOPIC);
    client.messageHandlers[0].callback = mqtt_sub_callback;
    client.messageHandlers[0].qos = QOS1;

    /* 设置默认的回调函数 */
    client.defaultMessageHandler = mqtt_sub_default_callback;

    response_dev_argc_flag = 0;
    update_firmwware_flag = 0;
    paho_mqtt_start(&client);
}

/**
 * 检查mqtt连接状态
 * 
 */
void check_mqtt_inline(void *parameter)
{
  uint32_t wdt_loss_cnt = 1 * 60;
  
    while (1)
    {
        if ((wdt_loss_cnt > 0) && (wdt_allow_refush == 1))
            rt_device_control(my_wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
//        else
//            rt_kprintf("wdt_allow_refush = %d\n", wdt_allow_refush);

        if (client.isconnected)
        {
            SYS_LED_ON
            rt_thread_mdelay(500);
            SYS_LED_OFF
            rt_thread_mdelay(500);

            wdt_loss_cnt = 1 * 60;
        }
        else
        {
            SYS_LED_OFF
            if (wdt_loss_cnt > 0)
            {
                wdt_loss_cnt -= 1;
            }
            else
            {
                wdt_loss_cnt = 0;
                rt_kprintf("watchdog reset!\n");
            }
            rt_thread_mdelay(1000);
        }
    }
}
// static struct rt_thread mqtt_thread;
// static rt_uint8_t mqtt_stack[1024];
int app_my_mqtt_init(void)
{
    rt_thread_t tid;    //线程句柄

    get_mac();
    // rt_kprintf("DeviceID:%s\n", device_id);

   rt_kprintf("load device run argc");
   if (read_from_flash(
        BSP_DEV_RUN_USE_SECTION_NAME, 
        BSP_DEV_RUN_USE_SECTION_OFFSET, 
        (uint8_t *)&dev_argc, 
        sizeof(dev_argc)) == 0)
   {
       rt_kprintf("\t\t\t[ok]\n");
   }
   else
   {
       rt_kprintf("\t\t\t[fail]\nuse default argc ...\n");
       write_to_flash(
           BSP_DEV_RUN_USE_SECTION_NAME, 
           BSP_DEV_RUN_USE_SECTION_OFFSET, 
           (uint8_t *)&dev_argc, 
           sizeof(dev_argc));
   }
   dev_argc.cpu_tmp = 461;
    VIDEO_PWR_OFF
    #ifdef BSP_DEV_RUN_USE_DARGC_CMD
    dargc();
    #endif
	
    /* 创建MQTT线程 */
    rt_kprintf("my_mqtt init ");
    tid = rt_thread_create("my_mqtt",
                            my_mqtt_thread_entry,
                            RT_NULL,
                            512,
                            11, 15);
    if(tid != RT_NULL)
    {
        rt_thread_startup(tid);
        rt_kprintf("sucess\n");
    }
    else
    {
        rt_kprintf("failed! %d\n", tid);
    }

    /* 创建c_inline线程 */
    tid = rt_thread_create("sys_led",
                            check_mqtt_inline,
                            RT_NULL,
                            256,
                            11, 15);
    if(tid != RT_NULL)
    {
        rt_thread_startup(tid);
//        rt_kprintf("sys_led init sucess\n");
    }
//    else
//    {
//        rt_kprintf("sys_led failed! %d\n", tid);
//    }

    return RT_EOK;
}
