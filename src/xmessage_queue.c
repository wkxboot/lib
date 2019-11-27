#include "string.h"
#include "stdint.h"
#include "xmessage_queue.h"
#include "debug_assert.h"


typedef enum {
    XMESSAGE_QUEUE_POSITION_HEAD = 1,
    XMESSAGE_QUEUE_POSITION_TAIL
}xmessage_queue_position_t;

typedef struct
{
    uint8_t is_ready;
    uint8_t item_size;
    uint8_t item_cnt;
    uint8_t *buffer;
    uint32_t read;
    uint32_t write;
}xmessage_queue_t;


/**
* @brief 创建message queue队列
* @param message_queue 队列指针
* @param item_size 对象大小
* @param item_cnt 对象数量
* @return fifo句柄
* @note
*/
xmessage_queue_handle_t xmessage_queue_create(uint8_t item_size,uint8_t item_cnt)
{
    uint8_t *buffer;
    xmessage_queue_t *message_queue;

    DEBUG_ASSERT(item_size > 0 && item_cnt > 0);

    message_queue = (xmessage_queue_t *)xmessage_queue_port_malloc(sizeof(xmessage_queue_t));
    if (message_queue == NULL) {
        goto error_handle;
    }
    buffer = xmessage_queue_port_malloc(item_size * item_cnt);
    if (buffer) {
        message_queue->buffer = buffer;
        message_queue->item_size = item_size;
        message_queue->item_cnt = item_cnt;
        message_queue->is_ready = 1;
        return message_queue;
    }

error_handle:
    xmessage_queue_port_free(message_queue);
    return NULL;
}

/**
* @brief 销毁message queue队列
* @param handle 队列指针
* @return 无
* @note
*/
void xmessage_queue_delete(xmessage_queue_handle_t handle)
{
    xmessage_queue_t *message_queue = (xmessage_queue_t *)handle;

    if (message_queue) {
        xmessage_queue_port_free(message_queue->buffer);
    }
    xmessage_queue_port_free(message_queue);
}

/**
* @brief 在队列指定位置获取一个对象
* @param handle 队列指针
* @param buffer 对象缓存指针
* @param position 位置
* @return 0：成功 -1：失败
* @note
*/
static int xmessage_queue_get(xmessage_queue_handle_t handle,void *buffer,xmessage_queue_position_t position)
{
    uint8_t offset;

    xmessage_queue_t *message_queue = (xmessage_queue_t *)handle;

    DEBUG_ASSERT(message_queue);
    DEBUG_ASSERT(message_queue->buffer);
    DEBUG_ASSERT(buffer);
    DEBUG_ASSERT(message_queue->is_ready == 1);

    /*是否有数据*/
    if (message_queue->write - message_queue->read == 0) {
        return -1;
    }

    if (position == XMESSAGE_QUEUE_POSITION_HEAD) {
        offset = message_queue->read % message_queue->item_cnt;
        memcpy(buffer,&message_queue->buffer[message_queue->item_size * offset],message_queue->item_size);
        message_queue->read ++;
    } else {
        message_queue->write --;
        offset = message_queue->write % message_queue->item_cnt;
        memcpy(buffer,&message_queue->buffer[message_queue->item_size * offset],message_queue->item_size);
    }

    return 0;
}

/**
* @brief 在队列指定位置放置一个对象
* @param handle 队列指针
* @param buffer 对象缓存指针
* @param position 位置
* @return 0：成功 -1：失败
* @note
*/
static int xmessage_queue_put(xmessage_queue_handle_t handle,void *buffer,xmessage_queue_position_t position)
{
    uint8_t offset;
    xmessage_queue_t *message_queue = (xmessage_queue_t *)handle;

    DEBUG_ASSERT(message_queue);
    DEBUG_ASSERT(message_queue->buffer);
    DEBUG_ASSERT(buffer);
    DEBUG_ASSERT(message_queue->is_ready == 1);

    /*是否有空间*/
    if (message_queue->write - message_queue->read >= message_queue->item_cnt) {
        return -1;
    }

    if (position == XMESSAGE_QUEUE_POSITION_TAIL) {
        offset = message_queue->write % message_queue->item_cnt;
        memcpy(&message_queue->buffer[message_queue->item_size * offset],buffer,message_queue->item_size);
        message_queue->write ++;
    } else {
        if (message_queue->read == 0) {
            /*同时增加读写数量*/
            message_queue->read += message_queue->item_cnt;
            message_queue->write += message_queue->item_cnt;
        }
        /*读的位置前移一位*/
        message_queue->read --;
        offset = message_queue->read % message_queue->item_cnt;
        memcpy(&message_queue->buffer[message_queue->item_size * offset],buffer,message_queue->item_size);
    }

    return 0;
}

/**
* @brief 在队列头部位置获取一个对象
* @param handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xmessage_queue_get_from_head(xmessage_queue_handle_t handle,void *buffer)
{
    int ret;
    xmessage_queue_t *message_queue = (xmessage_queue_t *)handle;

    ret = xmessage_queue_get(message_queue,buffer,XMESSAGE_QUEUE_POSITION_HEAD);

    return ret;
}

/**
* @brief 在队列尾部位置获取一个对象
* @param handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xmessage_queue_get_from_tail(xmessage_queue_handle_t handle,void *buffer)
{
    int ret;
    xmessage_queue_t *message_queue = (xmessage_queue_t *)handle;

    ret = xmessage_queue_get(message_queue,buffer,XMESSAGE_QUEUE_POSITION_TAIL);

    return ret;
}

/**
* @brief 在队列头部位置放入一个对象
* @param handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xmessage_queue_put_to_head(xmessage_queue_handle_t handle,void *buffer)
{
    int ret;
    xmessage_queue_t *message_queue = (xmessage_queue_t *)handle;

    ret = xmessage_queue_put(message_queue,buffer,XMESSAGE_QUEUE_POSITION_HEAD);

    return ret;
}

/**
* @brief 在队列尾部位置放入一个对象
* @param handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xmessage_queue_put_to_tail(xmessage_queue_handle_t handle,void *buffer)
{
    int ret;
    xmessage_queue_t *message_queue = (xmessage_queue_t *)handle;

    ret = xmessage_queue_put(message_queue,buffer,XMESSAGE_QUEUE_POSITION_TAIL);

    return ret;
}