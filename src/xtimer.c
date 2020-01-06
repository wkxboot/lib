/**
******************************************************************************                                                                                                                                                       
*                                                                            
*  This program is free software; you can redistribute it and/or modify      
*  it under the terms of the GNU General Public License version 3 as         
*  published by the Free Software Foundation.                                
*                                                                            
*  @file       xtimer.c
*  @brief      tiny timer
*  @author     wkxboot
*  @version    v1.0.0
*  @date       2019/7/3
*  @copyright  <h4><left>&copy; copyright(c) 2019 wkxboot 1131204425@qq.com</center></h4>  
*                                                                            
*                                                                            
*****************************************************************************/
#include "cmsis_os.h"
#include "xtimer.h"
#include "debug_assert.h"


/**
* @brief 定时器初始化
* @details
* @param timer 定时器指针
* @param timeout 定时器超时时间
* @return 初始化结果
* @retval 0 成功
* @retval -1 失败
* @attention
* @note
*/
int xtimer_init(xtimer_t *timer,uint32_t timeout)
{
    if (timer == NULL) {
        return -1;
    }
    timer->timeout = timeout;
    timer->start = osKernelSysTick();

    return 0;
}

/**
* @brief 定时器是否超时
* @details
* @param timer 定时器指针
* @return 0：未超时 1：超时
* @note
*/
uint8_t xtimer_is_timeout(xtimer_t *timer)
{
    uint32_t timeout_time;
    uint32_t current_time = osKernelSysTick();
    DEBUG_ASSERT(timer);

    timeout_time = timer->start + timer->timeout;

    return  time_before_eq(timeout_time,current_time) ? 1 : 0;
}