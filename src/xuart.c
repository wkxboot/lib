/**
****************************************************************************************                                                                                                                                                       
*                                                                            
*  This program is free software; you can redistribute it and/or modify      
*  it under the terms of the GNU General Public License version 3 as         
*  published by the Free Software Foundation.                                
*                                                                            
*  @file       xuart.c
*  @brief      uart库
*  @author     wkxboot
*  @version    v1.0.0
*  @date       2019/7/3
*  @copyright  <h4><left>&copy; copyright(c) 2019 wkxboot 1131204425@qq.com</center></h4>  
*                                                                            
*                                                                            
****************************************************************************************/
#include "circle_buffer.h"
#include "xuart.h"
#include "debug_assert.h"

 /** xuart句柄*/
typedef struct
{
    struct {
        uint8_t port;/**< 端口号*/
        uint32_t baudrate;/**< 波特率*/
        uint8_t data_bit;/**< 数据位*/
        uint8_t stop_bit;/**< 停止位*/
        uint8_t parity_bit;/**< 校验位*/
    }setting;/**< uart配置*/
    uint8_t is_port_open;/**< 是否已经打开*/
    circle_buffer_t read;/**< 接收缓存指针*/
    circle_buffer_t write;/**< 发送缓存地址*/
    uint8_t is_txe_int_enable;/**< 是否允许发送中断*/
    uint8_t is_rxne_int_enable;/**< 是否允许接收中断*/
    xuart_hal_driver_t *driver;
    uint8_t is_driver_register;
}xuart_instance_t;


/**
* @brief  创建串口句柄
* @param read_buffer_size 读缓存大小
* @param write_buffer_size 写缓存大小
* @param driver 驱动指针
* @return NULL:失败 其他：串口句柄
* @note 可重入
*/
xuart_handle_t xuart_create(uint16_t read_buffer_size,uint16_t write_buffer_size,xuart_hal_driver_t *driver)
{
    uint8_t *ptr_read = NULL,*ptr_write = NULL;
    xuart_instance_t *instance_handle;

    DEBUG_ASSERT(driver);
    DEBUG_ASSERT(read_buffer_size > 0 && write_buffer_size > 0);
    
    instance_handle = (xuart_instance_t *)xuart_port_malloc(sizeof(xuart_instance_t));
    if (instance_handle == NULL) {
        goto error_handle;
    }
    memset(instance_handle,0,sizeof(xuart_instance_t));

    ptr_read = xuart_port_malloc(read_buffer_size);
    ptr_write = xuart_port_malloc(write_buffer_size);
    if (ptr_read == NULL || ptr_write == NULL) {
        goto error_handle;
    }
    circle_buffer_init(&instance_handle->read,ptr_read,read_buffer_size);
    circle_buffer_init(&instance_handle->write,ptr_write,write_buffer_size);
    instance_handle->driver = driver;
    instance_handle->is_driver_register = 1;
    return instance_handle;

error_handle:
    if (instance_handle) {
        xuart_port_free(instance_handle);
    }
    if (ptr_read) {
        xuart_port_free(ptr_read);
    }
    if (ptr_write) {
        xuart_port_free(ptr_write);
    }
    return NULL;
}

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
int xuart_open(xuart_handle_t handle,uint8_t port,uint32_t baud_rates,uint8_t data_bits,uint8_t stop_bits)
{
    int rc;
    xuart_instance_t *instance_handle;

    DEBUG_ASSERT(handle);
    instance_handle = (xuart_instance_t *)handle;
    if (instance_handle->is_driver_register == 0) {
        return -XUART_ERROR_HAL_DRIVER_NOT_REGISTER;
    }
    rc = instance_handle->driver->init(port,baud_rates,data_bits,stop_bits);
    if (rc != 0){
        return -XUART_ERROR_UART_INIT_FAIL;
    }

    XUART_ENTER_CRITICAL();
    instance_handle->setting.port = port;
    instance_handle->setting.baudrate = baud_rates;
    instance_handle->setting.data_bit = data_bits;
    instance_handle->setting.stop_bit = stop_bits;
    instance_handle->driver->enable_rxne_it(instance_handle->setting.port);
    instance_handle->driver->disable_txe_it(instance_handle->setting.port);
    instance_handle->is_rxne_int_enable = 1;
    instance_handle->is_txe_int_enable = 0;
    instance_handle->is_port_open = 1;
    XUART_EXIT_CRITICAL();

    return XUART_ERROR_OK;
}


/**
* @brief  串口非阻塞的读数据
* @param handle 串口句柄
* @param buffer 数据地址
* @param size 期望读取的数量
* @return < 0：错误 其他：读取的数量
* @note 可重入
*/
int xuart_read(xuart_handle_t handle,uint8_t *buffer,uint32_t size)
{
    uint32_t read = 0;
    xuart_instance_t *instance_handle;

    DEBUG_ASSERT(handle);
    instance_handle = (xuart_instance_t *)handle;
    if (instance_handle->is_port_open == 0) {
        return -XUART_ERROR_UART_PORT_NOT_OPEN;
    }

    XUART_ENTER_CRITICAL();
    read = circle_buffer_read(&instance_handle->read,buffer,size);

    if (read > 0 && instance_handle->is_rxne_int_enable == 0) {
        instance_handle->is_rxne_int_enable = 1;
        instance_handle->driver->enable_rxne_it(instance_handle->setting.port);  
    }
    XUART_EXIT_CRITICAL();

    return read;
}

/**
* @brief 串口非阻塞的写入数据
* @param handle 串口句柄
* @param buffer 数据地址
* @param size 期望写入的数量
* @return < 0：失败 其他：写入的数量
* @note 可重入
*/
int xuart_write(xuart_handle_t handle,const uint8_t *buffer,uint32_t size)
{
    uint32_t write = 0;

    xuart_instance_t *instance_handle;

    DEBUG_ASSERT(handle);
    DEBUG_ASSERT(buffer);
    instance_handle = (xuart_instance_t *)handle;
    if (instance_handle->is_port_open == 0) {
        return -XUART_ERROR_UART_PORT_NOT_OPEN;
    }

    XUART_ENTER_CRITICAL();
    write = circle_buffer_write(&instance_handle->write,buffer,size);
    if (write > 0 && instance_handle->is_txe_int_enable == 0){
        instance_handle->is_txe_int_enable = 1;
        instance_handle->driver->enable_txe_it(instance_handle->setting.port);
    }
    XUART_EXIT_CRITICAL();

    return write;
}


/**
* @brief 串口缓存清除
* @param handle 串口句柄
* @return < 0：失败 XUART_ERROR_OK：成功
* @note 
*/
int xuart_clear(xuart_handle_t handle)
{
    xuart_instance_t *instance_handle;

    DEBUG_ASSERT(handle);
    instance_handle = (xuart_instance_t *)handle;
    if (instance_handle->is_port_open == 0) {
        return -XUART_ERROR_UART_PORT_NOT_OPEN;
    }
    
    XUART_ENTER_CRITICAL();
    instance_handle->is_txe_int_enable = 0;
    instance_handle->is_rxne_int_enable = 1;
    instance_handle->driver->disable_txe_it(instance_handle->setting.port);
    instance_handle->driver->enable_rxne_it(instance_handle->setting.port);
    circle_buffer_flush(&instance_handle->read);
    circle_buffer_flush(&instance_handle->write);
    XUART_EXIT_CRITICAL();

    return XUART_ERROR_OK;
}

/**
* @brief 关闭串口
* @param handle 串口句柄
* @return < 0：失败 XUART_ERROR_OK：成功
* @note 
*/
int xuart_close(xuart_handle_t handle)
{
    int rc = 0;
    xuart_instance_t *instance_handle;

    DEBUG_ASSERT(handle);
    instance_handle = (xuart_instance_t *)handle;
    if (instance_handle->is_port_open == 0) {
        return XUART_ERROR_OK;
    }
    rc = instance_handle->driver->deinit(instance_handle->setting.port);
    if (rc != 0){
        return -XUART_ERROR_UART_DEINIT_FAIL;
    }

    XUART_ENTER_CRITICAL();
    instance_handle->driver->disable_rxne_it(instance_handle->setting.port);
    instance_handle->driver->disable_txe_it(instance_handle->setting.port);
    instance_handle->is_rxne_int_enable = 0;
    instance_handle->is_txe_int_enable = 0;
    instance_handle->is_port_open = 0;
    circle_buffer_flush(&instance_handle->read);
    circle_buffer_flush(&instance_handle->write); 
    XUART_EXIT_CRITICAL();

    return XUART_ERROR_OK;
}

/**
* @brief 中断接收字节
* @param handle 串口句柄
* @param buffer 缓存地址
* @param size 数据量
* @return < 0：失败 其他：接收的数量
* @note
*/
int xuart_isr_write_bytes(xuart_handle_t handle,uint8_t *buffer,uint8_t size)
{
    uint32_t write;
    xuart_instance_t *instance_handle;

    DEBUG_ASSERT(handle);
    instance_handle = (xuart_instance_t *)handle;
    if (instance_handle->is_port_open == 0) {
        return -XUART_ERROR_UART_PORT_NOT_OPEN;
    }
    
    write = circle_buffer_write(&instance_handle->read,buffer,size);
    if (write < size ) {
        /*禁止接收中断*/
        instance_handle->is_rxne_int_enable = 0;
        instance_handle->driver->disable_rxne_it(instance_handle->setting.port);
    }  
    return write;
}

/**
* @brief 中断发送字节
* @param handle 串口句柄
* @param buffer 缓存地址
* @param size 数量
* @return < 0：失败 其他：发送的数量
* @note
*/
int xuart_isr_read_bytes(xuart_handle_t handle,uint8_t *buffer,uint8_t size)
{
    uint32_t read;
    xuart_instance_t *instance_handle;

    DEBUG_ASSERT(handle);
    instance_handle = (xuart_instance_t *)handle;
    if (instance_handle->is_port_open == 0) {
        return -XUART_ERROR_UART_PORT_NOT_OPEN;
    }

    read = circle_buffer_read(&instance_handle->write,buffer,size);
    if (read == 0) {
        /*禁止发送中断*/
        instance_handle->is_txe_int_enable = 0;
        instance_handle->driver->disable_txe_it(instance_handle->setting.port);
    }
    return read;
}


#if XUART_IN_FREERTOS  > 0
#include "xtimer.h"
/**
* @brief 串口等待数据
* @param handle 串口句柄
* @param timeout 超时时间
* @return < 0：失败 其他：缓存的数量
* @note 
*/
int xuart_select(xuart_handle_t handle,uint32_t timeout)
{
    xtimer_t timer;
    xuart_instance_t *instance_handle;

    DEBUG_ASSERT(handle);
    instance_handle = (xuart_instance_t *)handle;
    if (instance_handle->is_port_open == 0) {
        return -XUART_ERROR_UART_PORT_NOT_OPEN;
    }
    xtimer_init(&timer,timeout);
    while (!xtimer_is_timeout(&timer) && circle_buffer_size(&instance_handle->read) == 0) {
        osDelay(1);
    }
     
    return circle_buffer_size(&instance_handle->read);
}

/**
* @brief  串口等待数据发送完毕
* @param handle 串口句柄
* @param timeout 超时时间
* @return < 0：失败 其他：发送缓存剩余的数量
* @note 
*/
int xuart_complete(xuart_handle_t handle,uint32_t timeout)
{
    xtimer_t timer;
    xuart_instance_t *instance_handle;

    DEBUG_ASSERT(handle);
    instance_handle = (xuart_instance_t *)handle;
    if (instance_handle->is_port_open == 0) {
        return -XUART_ERROR_UART_PORT_NOT_OPEN;
    }
    xtimer_init(&timer,timeout);
    while (!xtimer_is_timeout(&timer) && circle_buffer_size(&instance_handle->write) > 0) {
        osDelay(1);
    }

    return circle_buffer_size(&instance_handle->write);
}

/**
* @brief 串口阻塞读取数据
* @param handle 串口句柄
* @param buffer 数据缓存指针
* @param size 数据期望读取数量
* @param timeout 超时时间
* @return < 0：失败 其他：实际读取的数量
* @note
*/
int xuart_read_block(xuart_handle_t handle,uint8_t *buffer,uint32_t size,uint32_t timeout)
{
    uint32_t total = 0;
    int read;
    xtimer_t timer;

    if (timeout == 0) {
        return xuart_read(handle,buffer,size);
    }
    xtimer_init(&timer,timeout);
    while (!xtimer_is_timeout(&timer) && total < size) {
        read = xuart_read(handle,buffer + total,size - total);
        if (read < 0) {
            return read;
        }
        total += read;
        if (total < size) {
            osDelay(1);
        }
    }
    return total;
}

/**
* @brief 串口阻塞写入数据
* @param handle 串口句柄
* @param buffer 数据缓存指针
* @param size 数据期望读取数量
* @param timeout 超时时间
* @return < 0：失败 其他：实际写入的数量
* @note
*/
int xuart_write_block(xuart_handle_t handle,uint8_t *buffer,uint32_t size,uint32_t timeout)
{
    uint32_t total = 0;
    int write;
    xtimer_t timer;

    if (timeout == 0) {
        return xuart_write(handle,buffer,size);
    }
    xtimer_init(&timer,timeout);
    while (!xtimer_is_timeout(&timer) && total < size) {
        write = xuart_write(handle,buffer + total,size - total);
        if (write < 0) {
            return write;
        }
        total += write;
        if (total < size) {
            osDelay(1);
        }
    }
    return total;
}

#endif

