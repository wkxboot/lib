#ifndef  __XMESSAGE_QUEUE_H__
#define  __XMESSAGE_QUEUE_H__
 
#ifdef  __cplusplus
#define  XMESSAGE_QUEUE_BEGIN extern "C" {
#define  XMESSAGE_QUEUE_END   }
#else
#define  XMESSAGE_QUEUE_BEGIN
#define  XMESSAGE_QUEUE_END
#endif
 
XMESSAGE_QUEUE_BEGIN

extern uint8_t * xmessage_queue_port_malloc(uint32_t size);
extern void xmessage_queue_port_free(void *ptr);

typedef void* xmessage_queue_handle_t;

/**
* @brief 创建message queue队列
* @param message queue 队列指针
* @param item_size 对象大小
* @param item_cnt 对象数量
* @return message queue句柄
* @note
*/
xmessage_queue_handle_t xmessage_queue_create(uint8_t item_size,uint8_t item_cnt);
/**
* @brief 销毁message queue队列
* @param message queue_handle 队列指针
* @return 无
* @note
*/
void xmessage_queue_delete(xmessage_queue_handle_t handle);

/**
* @brief 在队列头部位置获取一个对象
* @param message queue_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xmessage_queue_get_from_head(xmessage_queue_handle_t handle,void *buffer);

/**
* @brief 在队列尾部位置获取一个对象
* @param message queue_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xmessage_queue_get_from_tail(xmessage_queue_handle_t handle,void *buffer);

/**
* @brief 在队列头部位置放入一个对象
* @param message queue_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xmessage_queue_put_to_head(xmessage_queue_handle_t handle,void *buffer);

/**
* @brief 在队列尾部位置放入一个对象
* @param message queue_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xmessage_queue_put_to_tail(xmessage_queue_handle_t handle,void *buffer);



XMESSAGE_QUEUE_END
#endif

