#ifndef __MY_HTTP_H
#define __MY_HTTP_H

#include <stdint.h>
#include <rtthread.h>

extern char update_uri_header[sizeof(BSP_GET_UPDATE_URI_HEADER)];


int http_ota_fw_download(const char* uri);
int check_firmware(void);

#endif
