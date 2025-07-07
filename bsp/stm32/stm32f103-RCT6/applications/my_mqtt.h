#ifndef _MY_MQTT_H_
#define _MY_MQTT_H_

#include <rtthread.h>
#include <stdint.h>
#include <paho_mqtt.h>

// #define MQTT_IP             "tcp://39.104.238.231:1883"
// #define MQTT_UserName       "mcu_test"
// #define MQTT_PassWord       "123456"
// #define MQTT_TOPIC          "nongye/develop/iot/cmd"
// #define MQTT_PUBLIC_TOPIC   "nongye/develop/iot/bus485_sensor"

// #define UPDATE_FILE_NAME_LEN 50

/* 下行数据包结构 */
struct my_mqtt_pkg
{
    char dev_id[13];        /* 设备识别码 */
    uint8_t dev_num;        /* 设备编号 */
    uint8_t data_buf[50];     /* */
    uint16_t data_len;      /* 数据长度 */
    uint16_t cmd;           /* 命令 */
    uint16_t crc;
};

/* 设备运行参数 */
struct dev_run_argc
{
    uint16_t cpu_tmp;                   /* CPU温度 */
    uint16_t mqtt_upload_mode;          /* 数据上报模式：0，周期上报 1，变动上报 */
    uint16_t mqtt_upload_interval;      /* IoT设备上报传感器数据的时间间隔，单位秒 */
    uint16_t cpu_fan_tmp;               /* CPU风扇启动温度 */
    uint16_t video_limit_pwr;           /* 摄像头断电阈值 */
    uint16_t video_pwr_status;          /* 摄像头电源状态：0，未接通 1，已接通 */

    uint16_t video_up_pwr_time;         /* 摄像头上电时间：高八位，时  低八位，分 */
    uint16_t video_down_pwr_time;       /* 摄像头断电时间：高八位，时  低八位，分 */
    uint16_t cache_rainfall_val;        /* 本地缓存的雨量计值 */
};

/* defined the SYS_LED pin: PA1 */
#define SYS_LED_PIN     GET_PIN(A, 1)
#define SYS_LED_ON      rt_pin_write(SYS_LED_PIN, PIN_HIGH);
#define SYS_LED_OFF     rt_pin_write(SYS_LED_PIN, PIN_LOW);

/* defined the SENSOR_PWR pin: PB1 */
#define SENSOR_PWR_PIN     GET_PIN(B, 1)
#define SENSOR_PWR_ON      rt_pin_write(SENSOR_PWR_PIN, PIN_HIGH);
#define SENSOR_PWR_OFF     rt_pin_write(SENSOR_PWR_PIN, PIN_LOW);

/* defined the VIDEO_PWR pin: PB12 */
#define VIDEO_PWR_PIN     GET_PIN(B, 12)
#define VIDEO_PWR_ON      rt_pin_write(VIDEO_PWR_PIN, PIN_HIGH);dev_argc.video_pwr_status=1;
#define VIDEO_PWR_OFF     rt_pin_write(VIDEO_PWR_PIN, PIN_LOW);dev_argc.video_pwr_status=0;

/* defined the Air724 Mode POWER_IN pin: PB9 */
#define POWER_IN_PIN    GET_PIN(B, 9)
#define POWER_IN_ON     rt_pin_write(VIDEO_PWR_PIN, PIN_HIGH);
#define POWER_IN_OFF    rt_pin_write(VIDEO_PWR_PIN, PIN_LOW);
/* defined the Air724 Mode POWER_STATUS pin: PB8 */
#define POWER_STATUS_PIN    GET_PIN(B, 8)

extern uint8_t response_dev_argc_flag;
extern volatile uint8_t update_firmwware_flag;
extern char uri_buf[];

/* 外设供电状态：0，未供电 1，已供电 */
extern uint8_t sensor_pwr_status;
extern struct dev_run_argc dev_argc;

extern struct MQTTClient client;


rt_err_t upload_dev_argc(void);
rt_err_t upload_485_sensor_data(uint8_t deviceType, uint8_t deviceNum, uint8_t err_code, uint8_t *data, uint16_t data_len);

int app_my_mqtt_init(void);
int create_update_pack(uint8_t *buf, uint8_t deviceType, uint8_t deviceNum, uint8_t err_code, uint8_t *data, uint16_t data_len);
int my_mqtt_public(MQTTClient *_client, enum QoS qos, const char *topic, const uint8_t *payload, uint16_t payloadlen);

#endif
