#ifndef  __XFIFO_H__
#define  __XFIFO_H__
 
#ifdef  __cplusplus
#define  XFIFO_BEGIN extern "C" {
#define  XFIFO_END   }
#else
#define  XFIFO_BEGIN
#define  XFIFO_END
#endif
 
XFIFO_BEGIN

extern uint8_t * xfifo_port_malloc(uint32_t size);
extern void xfifo_port_free(void *ptr);

typedef void* xfifo_handle_t;

/**
* @brief 创建fifo队列
* @param fifo 队列指针
* @param item_size 对象大小
* @param item_cnt 对象数量
* @return fifo句柄
* @note
*/
xfifo_handle_t xfifo_create(uint8_t item_size,uint8_t item_cnt);
/**
* @brief 销毁fifo队列
* @param fifo_handle 队列指针
* @return 无
* @note
*/
void xfifo_delete(xfifo_handle_t fifo_handle);

/**
* @brief 在队列头部位置获取一个对象
* @param fifo_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xfifo_get_from_head(xfifo_handle_t fifo_handle,void *buffer);

/**
* @brief 在队列尾部位置获取一个对象
* @param fifo_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xfifo_get_from_tail(xfifo_handle_t fifo_handle,void *buffer);

/**
* @brief 在队列头部位置放入一个对象
* @param fifo_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xfifo_put_to_head(xfifo_handle_t fifo_handle,void *buffer);

/**
* @brief 在队列尾部位置放入一个对象
* @param fifo_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xfifo_put_to_tail(xfifo_handle_t fifo_handle,void *buffer);



XFIFO_END
#endif

