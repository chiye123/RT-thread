#include "my_flash.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_spi.h"
#include "spi_flash_sfud.h"
#include <fal.h>

/**
 * 创建存储设备
 */
static int rt_hw_spi_flash_with_sfud_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    rt_pin_mode(FLASH_WP_PIN, PIN_MODE_OUTPUT);
    //    FLASH_WP_DISABLE
    FLASH_WP_ENABLE

    rt_hw_spi_device_attach("spi1", "spi10", FLASH_CS_PIN_GROUP, FLASH_CS_PIN);

    if (rt_sfud_flash_probe(BSP_SFUD_DEVICE_NAME, BSP_SPI_FLASH_DEVICE_NAME))
    {
        return RT_EOK;
    };
    return -RT_ERROR;
}
/* 导出到自动初始化 */
INIT_COMPONENT_EXPORT(rt_hw_spi_flash_with_sfud_init);

/**
 * 保存设备运行参数到Flash中
 * 
 * @param file 文件名
 * 
 * @return 0 on successful, -1 on failed.
 */
int write_to_flash(const char *partition_name, uint32_t addr, uint8_t *buf, uint32_t len)
{
    int ret;
    const struct fal_partition *partition = partition = fal_partition_find(partition_name);
    if (partition == RT_NULL)
    {
        LOG_E("flash not find\n");
        return -1;
    }

    ret =  fal_partition_erase(partition, addr, len);
    if (ret < 0)
    {
        LOG_E("Partition (%s) erase failed!", partition->name);
        return -1;
    }

    ret =  fal_partition_write(partition, addr, buf, len);
    if (ret < 0)
    {
        LOG_E("Partition (%s) write failed!", partition->name);
        return -1;
    }

    return 0;
}

/**
 * 从Flash中读出参数到结构体dev_argc
 * 
 * @param file 文件名
 * 
 * @return 0 on successful, -1 on failed.
 */
int read_from_flash(const char *partition_name, uint32_t addr, uint8_t *buf, uint32_t len)
{
    int ret;
    const struct fal_partition *partition = partition = fal_partition_find(partition_name);
    if (partition == RT_NULL)
    {
        LOG_E("flash not find\n");
        return -1;
    }

    ret = fal_partition_read(partition, addr, buf, len);
    if (ret < 0)
    {
        LOG_E("Partition (%s) read failed!", partition->name);
        return -1;
    }

   return 0;
}

// /* 挂载文件系统 */
// int dfs_mount_init(void)
// {
//     /* 挂载文件系统 */
//     // rt_kprintf("dfs_mount");
//     if(dfs_mount(BSP_SFUD_DEVICE_NAME, "/", "elm", 0, 0) == 0)
//     {
//         // rt_kprintf("\t\t\t\t[ok]\n");
//         return RT_EOK;
//     }
//     else {
//         // rt_kprintf("\t\t\t\t[fail]\n");
//         return -RT_ERROR;
//     }
// }
// /* 系统启动时自动执行 */
// INIT_ENV_EXPORT(dfs_mount_init);   // 初始化执行顺序在注册flash之后
