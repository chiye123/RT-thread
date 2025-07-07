#include "my_header.h"


const struct __my_header app_header __attribute__((section(".ARM.__at_0x8010200"))) = {
    .version = APP_HEADER_VERSION,
    .time = APP_HEADER_TIME,
    .start_addr = APP_HEADER_ADDR,
    .size = APP_HEADER_SIZE,
    .checksum = 0
};

