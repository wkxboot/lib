#ifndef  __XUART_H__
#define  __XUART_H__
#include "stdint.h"
#include "circle_buffer.h"

#ifdef  __cplusplus
# define XUART_BEGIN  extern "C" {
# define XUART_END    }
#else
# define XUART_BEGIN
# define XUART_END
#endif

XUART_BEGIN


#define  XUART_DEBUG   1

/*freertos下使用*/
#define  SERIAL_IN_FREERTOS                         1

#define  SERIAL_PRIORITY_BITS                       3
#define  SERIAL_PRIORITY_HIGH                       2
#define  SERIAL_MAX_INTERRUPT_PRIORITY             (SERIAL_PRIORITY_HIGH << (8 - SERIAL_PRIORITY_BITS))


typedef struct 
{
    int (*init)(uint8_t port,uint32_t baudrate,uint8_t data_bit,uint8_t stop_bit);
    int (*deinit)(uint8_t port);
    void (*enable_txe_it)(uint8_t port);
    void (*disable_txe_it)(uint8_t port);
    void (*enable_rxne_it)(uint8_t port);
    void (*disable_rxne_it)(uint8_t port);
}xuart_hal_driver_t;


typedef struct
{
    struct {
        uint8_t port;
        uint32_t baudrate;
        uint8_t data_bit;
        uint8_t stop_bit;
        uint8_t parity_bit;
    }setting;
    uint8_t is_port_open;
    circle_buffer_t recv;
    circle_buffer_t send;
    uint8_t is_txe_int_enable;
    uint8_t is_rxne_int_enable;
}xuart_handle_t;



/*
* @brief  从串口非阻塞的读取数据
* @param handle 串口句柄
* @param dst 数据目的地址
* @param size 期望读取的数量
* @return 读取的数量
* @note 可重入
*/
uint32_t xuart_read(xuart_handle_t *handle,uint8_t *dst,uint32_t size);


/*
* @brief  从串口非阻塞的写入指定数量的数据
* @param handle 串口句柄
* @param src 数据源地址
* @param size 期望写入的数量
* @return 实际写入的数量
* @note 可重入
*/
uint32_t xuart_write(xuart_handle_t *handle,const uint8_t *src,uint32_t size);

/*
* @brief 串口缓存清除
* @param handle 串口句柄
* @return 无
* @note 
*/
void xuart_clear(xuart_handle_t *handle);

/*
* @brief 打开串口
* @param handle 创建输出的串口句柄指针
* @param port 端口号
* @param baudrate 波特率
* @param data_bits 数据位
* @param stop_bit 数据
* @param handle 创建输出的串口句柄指针
* @return -1：失败 0：成功
* @note 
*/
int xuart_open(xuart_handle_t *handle,uint8_t port,uint32_t baudrate,uint8_t data_bit,uint8_t stop_bit,
                              uint8_t *rx_buffer,uint32_t rx_size,uint8_t *tx_buffer,uint32_t tx_size);

/*
* @brief  关闭串口
* @param handle 串口句柄
* @return -1： 失败 0：失败
* @note 
*/
int xuart_close(xuart_handle_t *handle);

/*
* @brief 串口全局初始化
* @param driver 串口硬件驱动
* @return none
* @note
*/
void xuart_init(xuart_hal_driver_t *driver);

/*
* @brief 串口中断接收一个字节
* @param handle 串口句柄
* @param recv 接收到的字节
* @return none
* @note
*/
uint32_t xuart_isr_put_char(xuart_handle_t *handle,uint8_t recv);

/*
* @brief 串口中断发送一个字节
* @param handle 串口句柄
* @param recv 需要发送的字节
* @return none
* @note
*/
uint32_t xuart_isr_get_char(xuart_handle_t *handle,uint8_t *send);



#if SERIAL_IN_FREERTOS > 0
/*
* @brief 串口等待数据
* @param handle 串口句柄
* @param timeout 超时时间
* @return 接收缓存中的数据量
* @note 
*/
uint32_t serial_select(xuart_handle_t *handle,uint32_t timeout);

/*
* @brief  串口等待数据发送完毕
* @param handle 串口句柄
* @param timeout 超时时间
* @return 发送缓存中的数据量
* @note 
*/
uint32_t serial_complete(xuart_handle_t *handle,uint32_t timeout);

#endif

/*
*  serial critical configuration for IAR EWARM
*/

#ifdef __ICCARM__
#include "cmsis_iar.h"
  #if (defined (__ARM6M__) && (__CORE__ == __ARM6M__))             
    #define XUART_ENTER_CRITICAL()                             \
    {                                                          \
    unsigned int pri_mask;                                     \
    pri_mask = __get_PRIMASK();                                \
    __set_PRIMASK(1);
    
   #define XUART_EXIT_CRITICAL()                              \
    __set_PRIMASK(pri_mask);                                   \
    }
  #elif ((defined (__ARM7EM__) && (__CORE__ == __ARM7EM__)) || (defined (__ARM7M__) && (__CORE__ == __ARM7M__)))

   #define XUART_ENTER_CRITICAL()                              \
   {                                                           \
   unsigned int base_pri;                                      \
   base_pri = __get_BASEPRI();                                 \
   __set_BASEPRI(SERIAL_MAX_INTERRUPT_PRIORITY);   

   #define XUART_EXIT_CRITICAL()                              \
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

    #define XUART_EXIT_CRITICAL()                              \
    PRIMASK = pri_mask;                                         \
     __schedule_barrier();                                      \
    }
  #elif (defined(__TARGET_ARCH_7_M) || defined(__TARGET_ARCH_7E_M))

    #define XUART_ENTER_CRITICAL()                             \
    {                                                           \
     unsigned int base_pri;                                     \
     register unsigned char BASEPRI __asm( "basepri");          \
      base_pri = BASEPRI;                                       \
      BASEPRI = SERIAL_MAX_INTERRUPT_PRIORITY;                  \
      __schedule_barrier();

    #define XUART_EXIT_CRITICAL()                              \
     BASEPRI = base_pri;                                        \
     __schedule_barrier();                                      \
    }
  #endif
#endif  
  
  
  



XUART_END




#endif

