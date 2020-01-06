#ifndef  __XTIMER_H__
#define  __XTIMER_H__

#ifdef  __cplusplus
#define  XTIMER_BEGIN extern "C" {
#define  XTIMER_END   }
#else
#define  XTIMER_BEGIN
#define  XTIMER_END
#endif
 
XTIMER_BEGIN

 /** @brief 定时器定义*/
typedef struct
{
    uint32_t timeout;/**< 定时器定时值*/
    uint32_t start;/**< 定时器开始时间*/
}xtimer_t;

#define time_after(a,b)         \
((int32_t)(b) - (int32_t)(a) < 0)

#define time_before(a,b) time_after(b,a)
 
#define time_after_eq(a,b)       \
((int32_t)(a) - (int32_t)(b) >= 0)

#define time_before_eq(a,b) time_after_eq(b,a)

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
int xtimer_init(xtimer_t *timer,uint32_t timeout);

/**
* @brief 定时器是否超时
* @details
* @param timer 定时器指针
* @return 0：未超时 1：超时
* @note
*/
uint8_t xtimer_is_timeout(xtimer_t *timer);



XTIMER_END

#endif
