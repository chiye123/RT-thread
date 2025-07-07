/*
 * 这是一个精简版的Modbus-RTU协议，目前功能号只支持03和06
 * 
 * Change Logs:
 * Date           Author       Notes
 * 2022-03-07     吴学儒       The first version
 */

#include "TinyModbusRTU.h"
#include "crc16.h"
#include <rtdevice.h>
#include <rtdef.h>
#include <hw_crc.h>
#include <hwcrypto.h>

#include <stdlib.h>


#define swap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

#ifdef USING_TINY_MODBUS_RTU
/**
 * 创建一个Modbus-RTU协议读/写单个寄存器的报文
 * 
 * @param addr 设备地址
 * @param fcode 功能码，0x03或0x06
 * @param reg 寄存器
 * @param data 数据
 * @param buf 缓存报文的缓冲区
 * 
 * @return RT_EOK if success, else RT_ERROR
 */
rt_err_t create_modbus_rtu_pkg(uint8_t addr, uint8_t fcode, uint16_t reg, uint16_t data, uint8_t *buf)
{
    struct rt_hwcrypto_ctx *ctx = NULL;
    uint16_t crc;

    reg = swap_16(reg);
    buf[0] = addr;
    buf[1] = fcode;
    buf[2] = reg & 0xFF;
    buf[3] = reg >> 8;
    buf[4] = data >> 8;
    buf[5] = data & 0xFF;

    ctx = rt_hwcrypto_crc_create(rt_hwcrypto_dev_default(), HWCRYPTO_CRC_CRC16);
    if (ctx == RT_NULL)
    {
        // rt_kprintf("ctx is NULL\r\n");
        return RT_ERROR;
    }
    rt_hwcrypto_crc_cfg(ctx, (struct hwcrypto_crc_cfg *)&crc16_cfg);
    crc = rt_hwcrypto_crc_update(ctx,
                                &buf[0],
                                6);
    rt_hwcrypto_crc_destroy(ctx);

    buf[6] = (crc & 0xFF);
    buf[7] = (crc >> 8);

//    if (addr == 10)
//    {
//        rt_kprintf("pkg:");
//        for ( crc = 0; crc < 8; crc++)
//        {
//            rt_kprintf("%02X ", buf[crc]);
//        }
//        rt_kprintf("\n");
//    }
    
    return RT_EOK;
}

/**
 * 设置UART设备通信参数
 * 
 * @param name 串口设备名
 * @param baud 波特率
 * @param bufsz 数据缓冲区大小
 * 
 * @return RT_EOK if success, else RT_ERROR
 */
rt_err_t set_uart(const char *name, rt_uint32_t baud, rt_uint32_t bufsz)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* 初始化配置参数 */
    static rt_device_t serial;                /* 串口设备句柄 */
    
    /* 查找串口设备 */
    serial = rt_device_find(name);
    if (!serial)
    {
        // rt_kprintf("find %s failed!\n", name);
        return RT_ERROR;
    }

    /* 修改串口配置参数 */
    config.baud_rate = baud;
    config.data_bits = DATA_BITS_8;           //数据位 8
    config.stop_bits = STOP_BITS_1;           //停止位 1
    config.bufsz     = bufsz;
    config.parity    = PARITY_NONE;           //无奇偶校验位

    /* 控制串口设备。通过控制接口传入命令控制字，与控制参数 */
    return rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
}

/**
 * 使用Modbus-RTU协议写一个寄存器
 * 
 * @param addr 设备地址
 * @param reg 寄存器
 * @param data 要写入的数据
 * 
 * @return RT_EOK if success, else RT_ERROR
 */
rt_err_t modbus_rtu_write_reg(uint8_t addr, uint16_t reg, uint16_t data)
{
    rt_device_t serial;                /* 串口设备句柄 */
    uint8_t buf[8];

    if (create_modbus_rtu_pkg(addr, 0x06, reg, data, &buf[0]) != RT_EOK)
        return RT_ERROR;

    /* 查找串口设备 */
    serial = rt_device_find(BSP_USING_RS485_UART_NAME);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", BSP_USING_RS485_UART_NAME);
        return RT_ERROR;
    }

    /* 以中断接收及DMA发送模式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);

    /* 发送 */
    rt_device_write(serial, 0, buf, 8);

    rt_device_close(serial);
    return RT_EOK;
}

/**
 * 使用Modbus-RTU协议读一个寄存器
 * 
 * @param addr 设备地址
 * @param reg 寄存器
 * @param data 读出的数据
 * @param timeout 读超时，单位毫秒
 * 
 * @return RT_EOK if success, else RT_ERROR or RT_ETIMEOUT
 */
rt_err_t modbus_rtu_read_reg(uint8_t addr, uint16_t reg, uint16_t *data, uint16_t timeout)
{
    rt_device_t serial;                /* 串口设备句柄 */
    struct rt_hwcrypto_ctx *ctx = NULL;
    uint8_t buf[8];
    uint8_t len;
    uint16_t crc,t;

    // uint8_t i;
    
    if (create_modbus_rtu_pkg(addr, 0x03, reg, 0x01, &buf[0]) != RT_EOK)
        return RT_ERROR;

    /* 查找串口设备 */
    serial = rt_device_find(BSP_USING_RS485_UART_NAME);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", BSP_USING_RS485_UART_NAME);
        return RT_ERROR;
    }

    /* 以中断接收及DMA发送模式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);

    /* 发送 */
    rt_device_write(serial, 0, buf, 8);

    /* 接收 */
    len = 0;
    t = 0;
    do
    {
        rt_thread_mdelay(1);
        len += rt_device_read(serial, 0, &buf[len], 8);
        t += 1;
    } while(len < 7 && t <= timeout);
    rt_device_close(serial);

    if (t >= timeout)
        return RT_ETIMEOUT;
        
    if (len != 7)
        return RT_ERROR;
    
    ctx = rt_hwcrypto_crc_create(rt_hwcrypto_dev_default(), HWCRYPTO_CRC_CRC16);
    if (ctx == RT_NULL)
    {
        // rt_kprintf("ctx is NULL\r\n");
        return RT_ERROR;
    }
    rt_hwcrypto_crc_cfg(ctx, (struct hwcrypto_crc_cfg *)&crc16_cfg);
    crc = rt_hwcrypto_crc_update(ctx,
                                &buf[0],
                                5);
    rt_hwcrypto_crc_destroy(ctx);

    t= buf[5] | (((uint16_t)buf[6]) << 8);

//    rt_kprintf("%04X %04X\n", crc, t);
//    for (uint8_t i = 0; i < len; i++ )
//        rt_kprintf("%02X ", buf[i]);
//    rt_kprintf("\n");

    if (crc == t)
    {
        *data = buf[4] | (((uint16_t)buf[3]) << 8);
        return RT_EOK;
    }
    
    return RT_ERROR;
}

rt_err_t modbus_rtu_read_more_times(uint8_t addr, uint16_t reg, uint16_t *data, uint16_t timeout, uint8_t times)
{
    rt_err_t status;
    uint8_t i;

    for ( i = 0; i < times; i++)
    {
        status = modbus_rtu_read_reg(addr, reg, data, timeout);
        *data = swap_16(*data);
        if (status == RT_EOK)
            break;
    }
    return status;
}

#ifdef USING_TINY_MODBUS_RTU_CMD
static void TinyModbusRTU(uint8_t argc, char **argv)
{
    rt_err_t status;
    char *p_str = NULL;
    uint8_t addr;
    uint16_t reg,data,res;

    if (argc < 3)
    {
        goto __EXIT;
    }

    p_str = argv[2];
    while (*p_str)
    {
        if (*p_str >= '0' && *p_str <= '9')
        {
            p_str += 1;
        }
        else
        {
            rt_kprintf("addr not number!\n");
            goto __EXIT;
        }
    }

    p_str = argv[3];
    while (*p_str)
    {
        if (*p_str >= '0' && *p_str <= '9')
        {
            p_str += 1;
        }
        else
        {
            rt_kprintf("reg not number!\n");
            goto __EXIT;
        }
    }

    p_str = argv[4];
    while (*p_str)
    {
        if (*p_str >= '0' && *p_str <= '9')
        {
            p_str += 1;
        }
        else
        {
            rt_kprintf("data not number!\n");
            goto __EXIT;
        }
    }
    p_str = NULL;

    addr = atoi(argv[2]);
    reg = atoi(argv[3]);
    data = atoi(argv[4]);
    // rt_kprintf("addr=%X reg=%X data=%X\n", addr, reg, data);

    if (!rt_strcmp(argv[1], "w"))
    {
        status = modbus_rtu_write_reg(addr, reg, data);
        if (status == RT_EOK)
        {
            rt_kprintf("write sucess\n");
        }
        else
        {
            rt_kprintf("write failed!\n");
        }
        return;
    }
    else if (!rt_strcmp(argv[1], "r"))
    {
        status = modbus_rtu_read_reg(addr, reg, &res, data);
        if (status == RT_ETIMEOUT)
        {
            rt_kprintf("read timeout!\n");
        }
        else if (status == RT_ERROR)
        {
            rt_kprintf("read failed!\n");
        }
        else if (status == RT_EOK)
        {
            rt_kprintf("read value:0x%04X %d\n", res, res);
        }
        return;
    }

    __EXIT:
        rt_kprintf("Usage:'\n");
        rt_kprintf("TinyModbusRTU w addr reg data\n");
        rt_kprintf("TinyModbusRTU r addr reg timeout\n\n");
}
MSH_CMD_EXPORT(TinyModbusRTU, Tiny Modbus-RTU);
#endif

#endif
