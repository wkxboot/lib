#ifndef  __WK2124_H__
#define  __WK2124_H__

#include "stdint.h"
#ifdef  __cplusplus
#define  WK2124_BEGIN extern "C" {
#define  WK2124_END   }
#else
#define  WK2124_BEGIN
#define  WK2124_END
#endif
 
WK2124_BEGIN

#define  WK2124_OSC_HZ  11059200.0

typedef enum {
    WK2124_PARITY_NONE,/*无校验*/
    WK2124_PARITY_ODD, /*奇数校验*/
    WK2124_PARITY_EVEN /*偶数校验*/
}wk2124_parity_t;

typedef enum {
    WK2124_DATA_BITS_8,
    WK2124_DATA_BITS_9
}wk2124_data_bits_t;

typedef enum {
    WK2124_STOP_BITS_1,
    WK2124_STOP_BITS_2
}wk2124_stop_bits_t;

typedef struct
{
    uint8_t index;
    uint32_t baud_rates;
    wk2124_data_bits_t data_bits;
    wk2124_stop_bits_t stop_bits;
    wk2124_parity_t parity;

    uint8_t fifo_tx_int_level;
    uint8_t fifo_rx_int_level;
}wk2124_config_t;

/*全局中断标志位*/
#define  WK2124_UART0_INT_FLAG                   (1 << 0)
#define  WK2124_UART1_INT_FLAG                   (1 << 1)
#define  WK2124_UART2_INT_FLAG                   (1 << 2)
#define  WK2124_UART3_INT_FLAG                   (1 << 3)
/*uart fifo中断标志位*/
#define  WK2124_UART_FIFO_RECV_ERR_INT_FLAG      (1 << 7)
#define  WK2124_UART_FIFO_SEND_EMPTY_INT_FLAG    (1 << 3)
#define  WK2124_UART_FIFO_SEND_LEVEL_INT_FLAG    (1 << 2)
#define  WK2124_UART_FIFO_RECV_TIMEOUT_INT_FLAG  (1 << 1)
#define  WK2124_UART_FIFO_RECV_LEVEL_INT_FLAG    (1 << 0)
/*fifo状态标志位*/
#define  WK2124_UART_FIFO_RECV_NOT_EMPTY         (1 << 3)
#define  WK2124_UART_FIFO_SEND_NOT_EMPTY         (1 << 2)
#define  WK2124_UART_FIFO_SEND_FULL              (1 << 1)
#define  WK2124_UART_FIFO_SEND_BUSY              (1 << 0)
/*uart中断使能位*/
#define  WK2124_UART_FIFO_SEND_EMPTY_INT         (1 << 3)
#define  WK2124_UART_FIFO_SEND_LEVEL_INT         (1 << 2)
#define  WK2124_UART_FIFO_RECV_TIMEOUT_INT       (1 << 1)
#define  WK2124_UART_FIFO_RECV_LEVEL_INT         (1 << 0)
#define  WK2124_UART_FIFO_RECV_ERROR_INT         (1 << 7)


/**
* @brief 使能串口
* @details
* @param index 串口标号
* @return 无
* @attention
* @note
*/
void wk2124_enable_uart(int index);

/**
* @brief 禁止串口
* @details
* @param index 串口标号
* @return 无
* @attention
* @note
*/
void wk2124_disable_uart(int index);

/**
* @brief 串口参数配置
* @details
* @param index 串口参数指针
* @return 无
* @attention
* @note
*/
void wk2124_uart_config(wk2124_config_t *config);

/**
* @brief 使能uart指定的中断
* @details
* @param uart_index 串口标号
* @param int_type 中断类型
* @return 无
* @attention
* @note
*/
void wk2124_uart_int_enable(uint8_t uart_index,uint8_t int_type);

/**
* @brief 禁止uart指定的中断
* @details
* @param uart_index 串口标号
* @param int_type 中断类型
* @return 无
* @attention
* @note
*/
void wk2124_uart_int_disable(uint8_t uart_index,uint8_t int_type);


/**
* @brief 获取指定全局中断标志
* @details
* @param 无
* @return 中断标志
* @attention
* @note
*/
uint8_t wk2124_get_int_flags();

/**
* @brief 获取指定串口的中断标志
* @details
* @param index 串口标号
* @return 中断标志
* @attention
* @note
*/
uint8_t wk2124_get_uart_fifo_int_flags(uint8_t uart_index);

/**
* @brief 获取指定串口的fifo待发送字节数量
* @details
* @param index 串口标号
* @return 字节数量
* @attention
* @note
*/
uint8_t wk2124_uart_get_fifo_send_size(uint8_t uart_index);

/**
* @brief 获取指定串口的fifo接收的字节数量
* @details
* @param index 串口标号
* @return 字节数量
* @attention
* @note
*/
uint8_t wk2124_uart_get_fifo_recv_size(uint8_t uart_index);

/**
* @brief 获取指定串口的fifo状态
* @details
* @param index 串口标号
* @return 字节数量
* @attention
* @note
*/
uint8_t wk2124_uart_get_fifo_status(uint8_t uart_index);

/**
* @brief 清除指定串口的发送fifo
* @details
* @param index 串口标号
* @return 无
* @attention
* @note
*/
void wk2124_uart_fifo_send_clear(uint8_t uart_index);

/**
* @brief 清除指定串口的接收fifo
* @details
* @param index 串口标号
* @return 无
* @attention
* @note
*/
void wk2124_uart_fifo_recv_clear(uint8_t uart_index);

/**
* @brief 读取指定串口接收FIFO数据
* @details
* @param uart_index 串口标号
* @param buffer 数据缓存指针
* @param cnt 数量
* @return 无
* @attention
* @note
*/
void wk2124_read_uart_fifo(uint8_t uart_index,uint8_t *buffer,uint8_t cnt);

/**
* @brief 向指定串口的发送FIFO写入数据
* @details
* @param uart_index 串口标号
* @param buffer 数据缓存指针
* @param cnt 数量
* @return 无
* @attention
* @note
*/
void wk2124_write_uart_fifo(uint8_t uart_index,uint8_t *buffer,uint8_t cnt);


WK2124_END
#endif




