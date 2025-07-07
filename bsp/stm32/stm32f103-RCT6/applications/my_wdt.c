#include "my_wdt.h"
#include <rtthread.h>


rt_device_t my_wdg_dev;

//static void idle_hook(void)
//{
//    /* 在空闲线程的回调函数里喂狗 */
//    rt_device_control(my_wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
//    // rt_kprintf("feed dog!\n");//正式使用时去掉
//}

int wdt_sample(void)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t timeout = 15;        /* 溢出时间，单位：秒 */
    char device_name[RT_NAME_MAX] = WDT_DEVICE_NAME;

    /* 根据设备名称查找看门狗设备，获取设备句柄 */
    my_wdg_dev = rt_device_find(device_name);
    if (!my_wdg_dev)
    {
        rt_kprintf("find %s failed!\n", device_name);
        return RT_ERROR;
    }
    /* 初始化设备 */
    ret = rt_device_init(my_wdg_dev);
    if (ret != RT_EOK)
    {
        rt_kprintf("initialize %s failed!\n", device_name);
        return RT_ERROR;
    }
    /* 设置看门狗溢出时间 */
    ret = rt_device_control(my_wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    if (ret != RT_EOK)
    {
        rt_kprintf("set %s timeout failed!\n", device_name);
        return RT_ERROR;
    }
    /* 启动看门狗 */
    ret = rt_device_control(my_wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
    if (ret != RT_EOK)
    {
        rt_kprintf("start %s failed!\n", device_name);
        return -RT_ERROR;
    }
    /* 设置空闲线程回调函数 */
//    rt_thread_idle_sethook(idle_hook);

    return ret;
}
