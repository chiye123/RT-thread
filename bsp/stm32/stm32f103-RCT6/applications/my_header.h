#ifndef __MY_HEADER_H
#define __MY_HEADER_H

#include <stdint.h>

// <<< Use Configuration Wizard in Context Menu >>>
// <o> APP版本 <0x00000000-0xFFFFFFFF>
#define APP_HEADER_VERSION 0x55555555

// <o> APP时间 <0x00000000-0xFFFFFFFF>
#define APP_HEADER_TIME 0x00000000

// <o> APP入口地址 <0x00000000-0xFFFFFFFF>
#define APP_HEADER_ADDR 0x08010000

// <o> 用户区大小 <0x00000000-0xFFFFFFFF>
#define APP_HEADER_SIZE 0xBEADBEAD

// <<< end of configuration section >>>

struct __my_header
{
    uint32_t version;
    uint32_t checksum;
    uint32_t time;
    uint32_t start_addr;
    uint32_t size;
};



#endif
