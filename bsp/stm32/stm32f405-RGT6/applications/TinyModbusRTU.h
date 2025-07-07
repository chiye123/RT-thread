#ifndef _TINY_MODBUS_RTU_H
#define _TINY_MODBUS_RTU_H

#include <rtthread.h>

rt_err_t set_uart(const char *name, rt_uint32_t baud, rt_uint32_t bufsz);
rt_err_t modbus_rtu_write_reg(uint8_t addr, uint16_t reg, uint16_t data);
rt_err_t modbus_rtu_read_reg(uint8_t addr, uint16_t reg, uint16_t *data, uint16_t timeout);
rt_err_t modbus_rtu_read_more_times(uint8_t addr, uint16_t reg, uint16_t *data, uint16_t timeout, uint8_t times);
void my_rs485_test(void);

#endif
