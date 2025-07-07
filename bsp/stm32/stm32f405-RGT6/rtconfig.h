#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 2
#define IDLE_THREAD_STACK_SIZE 1024
#define RT_DEBUG

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart4"
#define RT_VER_NUM 0x30105
#define ARCH_ARM
#define RT_USING_CPU_FFS
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M3

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 10

/* C++ features */


/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 3
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 768
#define FINSH_CMD_SIZE 40
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_USING_MSH_ONLY
#define FINSH_ARG_MAX 5

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 2
#define DFS_FILESYSTEM_TYPES_MAX 2
#define DFS_FD_MAX 8
#define RT_USING_DFS_DEVFS

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 1536
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23
#define RT_USING_SERIAL
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 1024
#define RT_USING_PIN
#define RT_USING_RTC
#define RTC_SYNC_USING_NTP
#define RTC_NTP_FIRST_SYNC_DELAY 15
#define RTC_NTP_SYNC_PERIOD 3600
#define RT_USING_SPI
#define RT_USING_QSPI
#define RT_USING_SFUD
#define RT_SFUD_USING_SFDP
#define RT_SFUD_USING_FLASH_INFO_TABLE
#define RT_SFUD_SPI_MAX_HZ 18000000
#define RT_USING_WDT
#define RT_USING_HWCRYPTO
#define RT_HWCRYPTO_DEFAULT_NAME "hwcryto"
#define RT_HWCRYPTO_IV_MAX_SIZE 16
#define RT_HWCRYPTO_KEYBIT_MAX_SIZE 256
#define RT_HWCRYPTO_USING_CRC

/* Using USB */


/* POSIX layer and C standard library */

#define RT_USING_LIBC
#define RT_USING_POSIX

/* Network */

/* Socket abstraction layer */

#define RT_USING_SAL

/* protocol stack implement */

#define SAL_USING_AT
#define SAL_USING_POSIX

/* Network interface device */

#define RT_USING_NETDEV
#define NETDEV_USING_IFCONFIG
#define NETDEV_USING_PING
#define NETDEV_USING_AUTO_DEFAULT
#define NETDEV_IPV4 1
#define NETDEV_IPV6 0

/* light weight TCP/IP stack */


/* AT commands */

#define RT_USING_AT
#define AT_USING_CLIENT
#define AT_CLIENT_NUM_MAX 1
#define AT_USING_SOCKET
#define AT_USING_CLI
#define AT_CMD_MAX_LEN 2048
#define AT_SW_VERSION_NUM 0x10301

/* VBUS(Virtual Software BUS) */


/* Utilities */


/* RT-Thread online packages */

/* IoT - internet of things */

#define PKG_USING_PAHOMQTT
#define PAHOMQTT_PIPE_MODE
#define RT_PKG_MQTT_THREAD_STACK_SIZE 2048
#define PKG_PAHOMQTT_SUBSCRIBE_HANDLERS 1
#define PKG_USING_PAHOMQTT_V110
#define PKG_USING_WEBCLIENT
#define WEBCLIENT_NOT_USE_TLS
#define PKG_USING_WEBCLIENT_V220
#define PKG_WEBCLIENT_VER_NUM 0x20200

/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */

#define PKG_USING_NETUTILS
#define PKG_NETUTILS_NTP
#define NTP_USING_AUTO_SYNC
#define NTP_AUTO_SYNC_FIRST_DELAY 15
#define NTP_AUTO_SYNC_PERIOD 3600
#define NETUTILS_NTP_HOSTNAME "ntp.ntsc.ac.cn"
#define NETUTILS_NTP_HOSTNAME2 "ntp.aliyun.com"
#define NETUTILS_NTP_HOSTNAME3 "time.windows.com"
#define PKG_USING_NETUTILS_V131
#define PKG_NETUTILS_VER_NUM 0x10301
#define PKG_USING_AT_DEVICE
#define AT_DEVICE_USING_AIR720
#define AT_DEVICE_AIR720_SAMPLE
#define AIR720_SAMPLE_POWER_PIN 25
#define AIR720_SAMPLE_STATUS_PIN 24
#define AIR720_SAMPLE_CLIENT_NAME "uart2"
#define AIR720_SAMPLE_RECV_BUFF_LEN 4096
#define PKG_USING_AT_DEVICE_V204
#define PKG_AT_DEVICE_VER_NUM 0x20004

/* IoT Cloud */


/* security packages */


/* language packages */

/* JSON: JavaScript Object Notation, a lightweight data-interchange format */


/* XML: Extensible Markup Language */


/* multimedia packages */

/* LVGL: powerful and easy-to-use embedded GUI library */


/* u8g2: a monochrome graphic library */


/* PainterEngine: A cross-platform graphics application framework written in C language */


/* tools packages */


/* system packages */

/* enhanced kernel services */


/* POSIX extension functions */


/* acceleration: Assembly language or algorithmic acceleration packages */


/* CMSIS: ARM Cortex-M Microcontroller Software Interface Standard */


/* Micrium: Micrium software products porting for RT-Thread */

#define PKG_USING_FAL
#define FAL_DEBUG_CONFIG
#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG
#define FAL_USING_SFUD_PORT
#define FAL_USING_NOR_FLASH_DEV_NAME "W25Q16"
#define PKG_USING_FAL_V10000
#define PKG_FAL_VER_NUM 0x10000

/* peripheral libraries and drivers */


/* AI packages */


/* miscellaneous packages */

/* project laboratory */

/* samples: kernel and components samples */


/* entertainment: terminal games and other interesting software packages */

#define SOC_FAMILY_STM32
#define SOC_SERIES_STM32F1

/* Hardware Drivers Config */

#define SOC_STM32F103RC

/* Onboard Peripheral Drivers */

#define BSP_USING_USB_TO_USART
#define BSP_USING_RS485
#define BSP_USING_RS485_UART_NAME "uart1"
#define BSP_USING_RS485_UART_BAUD 9600
#define BSP_USING_RS485_UART_BUFFER 64
#define USING_TINY_MODBUS_RTU
#define USING_TINY_MODBUS_RTU_CMD
#define BSP_USING_SPI_FLASH
#define BSP_SPI_FLASH_DEVICE_NAME "spi10"
#define BSP_SFUD_DEVICE_NAME "W25Q16"
#define BSP_SPI_FLASH_MOUNT "/"

/* On-chip Peripheral Drivers */

#define BSP_USING_GPIO
#define BSP_USING_UART
#define BSP_USING_UART1
#define BSP_UART1_TX_USING_DMA
#define BSP_USING_UART2
#define BSP_UART2_TX_USING_DMA
#define BSP_USING_UART3
#define BSP_USING_UART4
#define BSP_UART4_TX_USING_DMA
#define BSP_USING_SPI
#define BSP_USING_SPI1
#define BSP_SPI1_TX_USING_DMA
#define BSP_USING_ON_CHIP_FLASH
#define BSP_USING_ONCHIP_RTC
#define BSP_RTC_USING_LSE
#define BSP_USING_WDT
#define BSP_USING_CRC
#define BSP_USING_SOFTWARE_CRC16

/* Board extended module Drivers */

#define BSP_USING_BDS_GPS_UART_NAME "uart3"
#define BSP_USING_BDS_GPS_UART_BAUD 9600
#define BSP_USING_BDS_GPS_UART_BUFFER_SIZE 100

/* Device Runner Parameter */

#define BSP_USE_MQTT_IP "tcp://39.104.238.231:1883"
#define BSP_USE_MQTT_USERNAME ""
#define BSP_USE_MQTT_PASSWORD ""
#define BSP_USE_MQTT_TOPIC "nongye/iot/cmd"
#define BSP_USE_MQTT_PUBLIC_TOPIC "nongye/iot/bus485_sensor"
#define BSP_USE_MQTT_DEBUG
#define BSP_DEV_RUN_USE_SECTION_NAME "easyflash"
#define BSP_DEV_RUN_USE_SECTION_OFFSET 0
#define BSP_GET_UPDATE_URI_HEADER "http://39.104.238.231:8002/firmware/"
#define BSP_FIRMWARE_HTTP_OTA_BUFF_LEN 1024
#define BSP_FIRMWARE_HTTP_OTA_GET_HEADER_BUFSZ 512
#define BSP_FIRMWARE_UPDATE_FILE_NAME_LEN 50
#define BSP_SAVE_FIRMWARE_SECTOR "download"
#define BSP_FIRMWARE_USE_SECTION_OFFSET 0
#define BSP_USE_CHECK_FIRMWARE_CMD
#define BSP_DEV_RUN_USE_DARGC_CMD

#endif
