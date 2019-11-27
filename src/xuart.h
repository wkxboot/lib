#ifndef  __XUART_H__
#define  __XUART_H__
#include "stdint.h"

#ifdef  __cplusplus
# define XUART_BEGIN  extern "C" {
# define XUART_END    }
#else
# define XUART_BEGIN
# define XUART_END
#endif

XUART_BEGIN

typedef void* xuart_handle_t;

#define  XUART_DEBUG   1

enum {
    XUART_ERROR_OK = 0,
    XUART_ERROR_INVALID_PARAM = 1,
    XUART_ERROR_HAL_DRIVER_NOT_REGISTER = 2,
    XUART_ERROR_UART_PORT_NOT_OPEN = 3,
    XUART_ERROR_UART_INIT_FAIL = 4,
    XUART_ERROR_UART_DEINIT_FAIL = 5
};
/*freertos下使用*/
#define  XUART_IN_FREERTOS                         1

#define  XUART_PRIORITY_BITS                       4
#define  XUART_PRIORITY_HIGH                       5
#define  XUART_MAX_INTERRUPT_PRIORITY             (XUART_PRIORITY_HIGH << (8 - XUART_PRIORITY_BITS))

/** xuart底层驱动*/
typedef struct 
{
    int (*init)(uint8_t port,uint32_t baudrate,uint8_t data_bit,uint8_t stop_bit); /**< 底层初始化*/
    int (*deinit)(uint8_t port);/**< 底层去初始化*/
    void (*enable_txe_it)(uint8_t port);/**< 使能发送中断*/
    void (*disable_txe_it)(uint8_t port);/**< 失能发送中断*/
    void (*enable_rxne_it)(uint8_t port);/**< 使能接收中断*/
    void (*disable_rxne_it)(uint8_t port);/**< 失能接收中断*/
}xuart_hal_driver_t;

/*内存管理*/
uint8_t *xuart_port_malloc(uint16_t size);
void xuart_port_free(void *ptr);

/**
* @brief  创建串口句柄
* @param read_buffer_size 读缓存大小
* @param write_buffer_size 写缓存大小
* @param driver 驱动指针
* @return NULL:失败 其他：串口句柄
* @note 可重入
*/
xuart_handle_t xuart_create(uint16_t read_buffer_size,uint16_t write_buffer_size,xuart_hal_driver_t *driver);
/**
* @brief 打开串口
* @param handle 串口句柄
* @param port 端口号
* @param baud_rates 波特率
* @param data_bits 数据位
* @param stop_bit 数据
* @return < 0：失败 XUART_ERROR_OK：成功
* @note 
*/
int xuart_open(xuart_handle_t handle,uint8_t port,uint32_t baud_rates,uint8_t data_bits,uint8_t stop_bits);
/**
* @brief  串口非阻塞的读数据
* @param handle 串口句柄
* @param buffer 数据地址
* @param size 期望读取的数量
* @return < 0：错误 其他：读取的数量
* @note 可重入
*/
int xuart_read(xuart_handle_t handle,uint8_t *buffer,uint32_t size);
/**
* @brief 串口非阻塞的写入数据
* @param handle 串口句柄
* @param buffer 数据地址
* @param size 期望写入的数量
* @return < 0：失败 其他：写入的数量
* @note 可重入
*/
int xuart_write(xuart_handle_t *handle,const uint8_t *buffer,uint32_t size);
/**
* @brief 串口缓存清除
* @param handle 串口句柄
* @return < 0：失败 XUART_ERROR_OK：成功
* @note 
*/
int xuart_clear(xuart_handle_t handle);
/**
* @brief 关闭串口
* @param handle 串口句柄
* @return < 0：失败 XUART_ERROR_OK：成功
* @note 
*/
int xuart_close(xuart_handle_t handle);
/**
* @brief 中断接收字节
* @param handle 串口句柄
* @param buffer 缓存地址
* @param size 数据量
* @return < 0：失败 其他：接收的数量
* @note
*/
int xuart_isr_write_bytes(xuart_handle_t handle,uint8_t *buffer,uint8_t size);
/**
* @brief 中断发送字节
* @param handle 串口句柄
* @param buffer 缓存地址
* @param size 数量
* @return < 0：失败 其他：发送的数量
* @note
*/
int xuart_isr_read_bytes(xuart_handle_t handle,uint8_t *buffer,uint8_t size);


#if XUART_IN_FREERTOS > 0
#include "cmsis_os.h"
/**
* @brief 串口等待数据
* @param handle 串口句柄
* @param timeout 超时时间
* @return < 0：失败 其他：缓存的数量
* @note 
*/
int xuart_select(xuart_handle_t handle,uint32_t timeout);
/**
* @brief  串口等待数据发送完毕
* @param handle 串口句柄
* @param timeout 超时时间
* @return < 0：失败 其他：发送缓存剩余的数量
* @note 
*/
int xuart_complete(xuart_handle_t handle,uint32_t timeout);
/**
* @brief 串口阻塞读取数据
* @param handle 串口句柄
* @param buffer 数据缓存指针
* @param size 数据期望读取数量
* @param timeout 超时时间
* @return < 0：失败 其他：实际读取的数量
* @note
*/
int xuart_read_block(xuart_handle_t handle,uint8_t *buffer,uint32_t size,uint32_t timeout);
/**
* @brief 串口阻塞写入数据
* @param handle 串口句柄
* @param buffer 数据缓存指针
* @param size 数据期望读取数量
* @param timeout 超时时间
* @return < 0：失败 其他：实际写入的数量
* @note
*/
int xuart_write_block(xuart_handle_t handle,uint8_t *buffer,uint32_t size,uint32_t timeout);


#endif

/**
*  critical configuration for IAR EWARM
*/

#ifdef __ICCARM__
#include "cmsis_iar.h"
  #if (defined (__ARM6M__) && (__CORE__ == __ARM6M__))             
    #define XUART_ENTER_CRITICAL()                             \
    {                                                          \
    unsigned int pri_mask;                                     \
    pri_mask = __get_PRIMASK();                                \
    __set_PRIMASK(1);
    
   #define XUART_EXIT_CRITICAL()                               \
    __set_PRIMASK(pri_mask);                                   \
    }
  #elif ((defined (__ARM7EM__) && (__CORE__ == __ARM7EM__)) || (defined (__ARM7M__) && (__CORE__ == __ARM7M__)))

   #define XUART_ENTER_CRITICAL()                              \
   {                                                           \
   unsigned int base_pri;                                      \
   base_pri = __get_BASEPRI();                                 \
   __set_BASEPRI(XUART_MAX_INTERRUPT_PRIORITY);   

   #define XUART_EXIT_CRITICAL()                               \
   __set_BASEPRI(base_pri);                                    \
  }
  #endif
#endif


  
#ifdef __CC_ARM
  #if (defined __TARGET_ARCH_6S_M)
    #define XUART_ENTER_CRITICAL()                                             
   {                                                            \
    unsigned int pri_mask;                                      \
    register unsigned char PRIMASK __asm( "primask");           \
    pri_mask = PRIMASK;                                         \
    PRIMASK = 1u;                                               \
    __schedule_barrier();

    #define XUART_EXIT_CRITICAL()                               \
    PRIMASK = pri_mask;                                         \
     __schedule_barrier();                                      \
    }
  #elif (defined(__TARGET_ARCH_7_M) || defined(__TARGET_ARCH_7E_M))

    #define XUART_ENTER_CRITICAL()                              \
    {                                                           \
     unsigned int base_pri;                                     \
     register unsigned char BASEPRI __asm( "basepri");          \
      base_pri = BASEPRI;                                       \
      BASEPRI = XUART_MAX_INTERRUPT_PRIORITY;                  \
      __schedule_barrier();

    #define XUART_EXIT_CRITICAL()                               \
     BASEPRI = base_pri;                                        \
     __schedule_barrier();                                      \
    }
  #endif
#endif  
  
  
  



XUART_END




#endif

