/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-03-22     Murphy       the first version
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <finsh.h>

#include "my_http.h"
#include "webclient.h"
#include <fal.h>
#include "my_header.h"
#include "crc16.h"
#include "my_flash.h"

#define DBG_ENABLE
#define DBG_SECTION_NAME          "http_ota"
#ifdef OTA_DOWNLOADER_DEBUG
#define DBG_LEVEL                 DBG_LOG
#else
#define DBG_LEVEL                 DBG_INFO
#endif
#define DBG_COLOR
#include <rtdbg.h>

const struct fal_partition * partition = RT_NULL;
static int begin_offset = 0;
static int file_size = 0;

char update_uri_header[sizeof(BSP_GET_UPDATE_URI_HEADER)] = BSP_GET_UPDATE_URI_HEADER;


// int check_firmware(uint32_t firmware_size)
int check_firmware()
{
    uint32_t i,crc,_crc,len,firmware_size;
    struct __my_header *app_header = NULL;
    
    uint8_t *buf = (uint8_t *)rt_malloc(1024);
    if (buf == NULL)
    {
        return -1;
    }

    read_from_flash(
        BSP_SAVE_FIRMWARE_SECTOR, 
        BSP_FIRMWARE_USE_SECTION_OFFSET, 
        &buf[0], 
        1024);
    app_header = (struct __my_header *)&buf[512];
    _crc = app_header->checksum;

    // rt_kprintf("version:0x%08X\n", app_header->version);
    // rt_kprintf("checksum:0x%08X\n", app_header->checksum);
    // rt_kprintf("time:0x%08X\n", app_header->time);
    // rt_kprintf("start_addr:0x%08X\n", app_header->start_addr);
    // rt_kprintf("size:%d\n", app_header->size);

    /* 检查固件版本 */
    if (app_header->version < APP_HEADER_VERSION)
    {
        rt_kprintf("firmware version(%X < %X) too low!\n", app_header->version, APP_HEADER_VERSION);
        goto __CHECK_ERR_EXIT;
    }

    // /* 检查固件大小 */
    // if (firmware_size != app_header->size)
    // {
    //     rt_kprintf("firmware size != download size\n");
    //     goto __CHECK_ERR_EXIT;
    // }
    firmware_size = app_header->size;

    /* 检查固件SP、PC */
    len = app_header->start_addr;
    app_header = (struct __my_header *)&buf[0];
    /* SP */
    if ((app_header->version < 0x20000000) || (app_header->version > 0x20000000 + 0xC000))
    {
        goto __CHECK_FIRM_ERROR;
    }
    /* PC */
    if ((app_header->checksum < APP_HEADER_ADDR) || (app_header->checksum > APP_HEADER_ADDR + 0x30000))
    {
        __CHECK_FIRM_ERROR:
        rt_kprintf("firmware error!\n");
        goto __CHECK_ERR_EXIT;
    }

    /* 校验报文 */
    buf[0] = 0;
    buf[1] = 0;
    len = 0;
    for ( i = 0; i < firmware_size; i+=512)
    {
        len = 2;
        if (firmware_size - i < 512)
            len += (firmware_size - i);
        else
            len += 512;
        
        read_from_flash(
            BSP_SAVE_FIRMWARE_SECTOR, 
            BSP_FIRMWARE_USE_SECTION_OFFSET + i, 
            &buf[2], 
            len);

        if (i == 512)
        {
            app_header = (struct __my_header *)&buf[2];
            app_header->checksum = 0;
            app_header = 0;
        }

        crc = calc_crc16((uint8_t *)&buf[0], len);
        buf[0] = crc & 0xFF;
        buf[1] = crc >> 8;
    }
    // rt_kprintf("crc:%08X %d\n", crc, i);
    if (crc != _crc)
    {
        rt_kprintf("firmware check error!\n");
        goto __CHECK_ERR_EXIT;
    }
    return RT_EOK;

    __CHECK_ERR_EXIT:
    if (buf != NULL)
    {
        rt_free(buf);
    }

    return -1;
}

#ifdef BSP_USE_CHECK_FIRMWARE_CMD
void check_fir(void)
{
    uint8_t *buf = (uint8_t *)rt_malloc(512);
    if (buf == NULL)
    {
        return;
    }

    read_from_flash(
        BSP_SAVE_FIRMWARE_SECTOR, 
        BSP_FIRMWARE_USE_SECTION_OFFSET + 512, 
        &buf[0], 
        512);

    if (buf != NULL)
    {
        rt_free(buf);
    }

    rt_kprintf("check firmware ...\t\t");
    if (check_firmware() == RT_EOK)
        rt_kprintf("[ok]\n");
    else
        rt_kprintf("[faild]\n");
}
MSH_CMD_EXPORT(check_fir, check firmrare);
#endif

static void print_progress(size_t cur_size, size_t total_size)
{
    static unsigned char progress_sign[100 + 1];
    uint8_t i, per = cur_size * 100 / total_size;

    if (per > 100)
    {
        per = 100;
    }

    for (i = 0; i < 100; i++)
    {
        if (i < per)
        {
            progress_sign[i] = '=';
        }
        else if (per == i)
        {
            progress_sign[i] = '>';
        }
        else
        {
            progress_sign[i] = ' ';
        }
    }

    progress_sign[sizeof(progress_sign) - 1] = '\0';

    rt_kprintf("Download: [%s] %d%%\033[1A\n", progress_sign, per);
}

static int http_ota_shard_download_handle(char *buffer, int length)
{
    int ret = RT_EOK;
    /* Write the data to the corresponding partition address */
    if (fal_partition_write(partition, begin_offset, (uint8_t *)&buffer[0], length) < 0)
    {
        LOG_E("Firmware download failed! Partition (%s) write data error!", partition->name);
        ret = -RT_ERROR;
        goto __exit;
    }
    begin_offset += length;

    print_progress(begin_offset, file_size);

__exit:
    rt_free(buffer);
    return ret;
}

int http_ota_fw_download(const char* uri)
{
    int ret = RT_EOK;
    struct webclient_session* session = RT_NULL;

    /* create webclient session and set header response size */
    session = webclient_session_create(BSP_FIRMWARE_HTTP_OTA_GET_HEADER_BUFSZ);
    if (!session)
    {
        rt_kprintf("open uri failed.\n");
        ret = -RT_ERROR;
        goto __exit;
    }

    /* get the real data length */
    webclient_shard_head_function(session, uri, &file_size);

    if (file_size == 0)
    {
        rt_kprintf("Request file size is 0!\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    else if (file_size < 0)
    {
        rt_kprintf("webclient GET request type is chunked.\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    rt_kprintf("OTA file size is (%d)\n", file_size);

    /* Get download partition information and erase download partition data */
    if ((partition = fal_partition_find(BSP_SAVE_FIRMWARE_SECTOR)) == RT_NULL)
    {
        LOG_E("Firmware download failed! Partition (%s) find error!", "download");
        ret = -RT_ERROR;
        goto __exit;
    }

    LOG_I("Start erase flash (%s) partition!", partition->name);

    if (fal_partition_erase(partition, 0, file_size) < 0)
    {
        LOG_E("Firmware download failed! Partition (%s) erase error!", partition->name);
        ret = -RT_ERROR;
        goto __exit;
    }
    LOG_I("Erase flash (%s) partition success!", partition->name);

    /* register the handle function, you can handle data in the function */
    webclient_register_shard_position_function(session, http_ota_shard_download_handle);

    /* the "memory size" that you can provide in the project and uri */
    ret = webclient_shard_position_function(session, uri, begin_offset, file_size, BSP_FIRMWARE_HTTP_OTA_BUFF_LEN);

    /* clear the handle function */
    webclient_register_shard_position_function(session, RT_NULL);

    if (ret == RT_EOK)
    {
        if (session != RT_NULL)
        {
            webclient_close(session);
            session = RT_NULL;
        }

        // rt_kprintf("Download firmware to flash success.\n");
        // rt_kprintf("Checksum firmware ...");
        // LOG_I("System now will restart...");

        rt_thread_delay(rt_tick_from_millisecond(5));

        // if (check_firmware(file_size) != RT_EOK)
        // {
        //     rt_kprintf("\t[faild]\n");
        // }
        // else
        // {
        //     rt_kprintf("\t[ok]\n");
        // }

        /* Reset the device, Start new firmware */
    //    extern void rt_hw_cpu_reset(void);
    //    rt_hw_cpu_reset();
    }
    else
    {
        rt_kprintf("Download firmware failed!\n");
    }

__exit:
    if (session != RT_NULL)
        webclient_close(session);
    begin_offset = 0;

    return ret;
}


