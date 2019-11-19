#include "string.h"
#include "stdint.h"
#include "xfifo.h"
#include "debug_assert.h"


typedef enum {
    XFIFO_POSITION_HEAD = 1,
    XFIFO_POSITION_TAIL
}xfifo_position_t;

typedef struct
{
    uint8_t item_size;
    uint8_t item_cnt;
    uint8_t *buffer;
    uint32_t read;
    uint32_t write;
}xfifo_t;


/**
* @brief 创建fifo队列
* @param fifo 队列指针
* @param item_size 对象大小
* @param item_cnt 对象数量
* @return fifo句柄
* @note
*/
xfifo_handle_t xfifo_create(uint8_t item_size,uint8_t item_cnt)
{
    uint8_t *buffer;
    xfifo_t *fifo;

    DEBUG_ASSERT_FALSE(item_size > 0 && item_cnt > 0);

    fifo = (xfifo_t *)xfifo_port_malloc(sizeof(xfifo_t));
    if (fifo == NULL) {
        goto error_handle;
    }
    buffer = xfifo_port_malloc(item_size * item_cnt);
    if (buffer) {
        fifo->buffer = buffer;
        fifo->item_size = item_size;
        fifo->item_cnt = item_cnt;
        return fifo;
    }

error_handle:
    xfifo_port_free(fifo);
    return NULL;
}

/**
* @brief 销毁fifo队列
* @param fifo_handle 队列指针
* @return 无
* @note
*/
void xfifo_delete(xfifo_handle_t fifo_handle)
{
    xfifo_t *fifo = (xfifo_t *)fifo_handle;

    if (fifo) {
        xfifo_port_free(fifo->buffer);
    }
    xfifo_port_free(fifo);
}

/**
* @brief 在队列指定位置获取一个对象
* @param fifo_handle 队列指针
* @param buffer 对象缓存指针
* @param position 位置
* @return 0：成功 -1：失败
* @note
*/
static int xfifo_get(xfifo_handle_t fifo_handle,void *buffer,xfifo_position_t position)
{
    uint8_t offset;

    xfifo_t *fifo = (xfifo_t *)fifo_handle;

    DEBUG_ASSERT_NULL(fifo);
    DEBUG_ASSERT_NULL(fifo->buffer);
    DEBUG_ASSERT_NULL(buffer);
    DEBUG_ASSERT_FALSE(fifo->is_ready == 1);

    /*是否有数据*/
    if (fifo->write - fifo->read == 0) {
        return -1;
    }

    if (position == XFIFO_POSITION_HEAD) {
        offset = fifo->read % fifo->item_cnt;
        memcpy(buffer,&fifo->buffer[fifo->item_size * offset],fifo->item_size);
        fifo->read ++;
    } else {
        fifo->write --;
        offset = fifo->write % fifo->item_cnt;
        memcpy(buffer,&fifo->buffer[fifo->item_size * offset],fifo->item_size);
    }

    return 0;
}

/**
* @brief 在队列指定位置放置一个对象
* @param fifo_handle 队列指针
* @param buffer 对象缓存指针
* @param position 位置
* @return 0：成功 -1：失败
* @note
*/
static int xfifo_put(xfifo_handle_t fifo_handle,void *buffer,xfifo_position_t position)
{
    uint8_t offset;
    xfifo_t *fifo = (xfifo_t *)fifo_handle;

    DEBUG_ASSERT_NULL(fifo);
    DEBUG_ASSERT_NULL(fifo->buffer);
    DEBUG_ASSERT_NULL(buffer);
    DEBUG_ASSERT_FALSE(fifo->is_ready == 1);

    /*是否有空间*/
    if (fifo->write - fifo->read >= fifo->item_cnt) {
        return -1;
    }

    if (position == XFIFO_POSITION_TAIL) {
        offset = fifo->write % fifo->item_cnt;
        memcpy(&fifo->buffer[fifo->item_size * offset],buffer,fifo->item_size);
        fifo->write ++;
    } else {
        /*同时增加读写数量*/
        fifo->read += fifo->item_cnt;
        fifo->write += fifo->item_cnt;
        /*读的位置前移一位*/
        fifo->read --;
        offset = fifo->read % fifo->item_cnt;
        memcpy(&fifo->buffer[fifo->item_size * offset],buffer,fifo->item_size);
    }

    return 0;
}

/**
* @brief 在队列头部位置获取一个对象
* @param fifo_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xfifo_get_from_head(xfifo_handle_t fifo_handle,void *buffer)
{
    int ret;
    xfifo_t *fifo = (xfifo_t *)fifo_handle;

    ret = xfifo_get(fifo,buffer,XFIFO_POSITION_HEAD);

    return ret;
}

/**
* @brief 在队列尾部位置获取一个对象
* @param fifo_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xfifo_get_from_tail(xfifo_handle_t fifo_handle,void *buffer)
{
    int ret;
    xfifo_t *fifo = (xfifo_t *)fifo_handle;

    ret = xfifo_get(fifo,buffer,XFIFO_POSITION_TAIL);

    return ret;
}

/**
* @brief 在队列头部位置放入一个对象
* @param fifo_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xfifo_put_to_head(xfifo_handle_t fifo_handle,void *buffer)
{
    int ret;
    xfifo_t *fifo = (xfifo_t *)fifo_handle;

    ret = xfifo_put(fifo,buffer,XFIFO_POSITION_HEAD);

    return ret;
}

/**
* @brief 在队列尾部位置放入一个对象
* @param fifo_handle 队列指针
* @param buffer 对象缓存指针
* @return 0：成功 -1：失败
* @note
*/
int xfifo_put_to_tail(xfifo_handle_t fifo_handle,void *buffer)
{
    int ret;
    xfifo_t *fifo = (xfifo_t *)fifo_handle;

    ret = xfifo_put(fifo,buffer,XFIFO_POSITION_TAIL);

    return ret;
}