#include "wk2124.h"

#define  UART_INDEX_MASK     (0x03)
#define  UART_INDEX_VBIT(x)  ((x & UART_INDEX_MASK) << 4)

#define  REG_WRITE_MASK      (0 << 6)
#define  REG_READ_MASK       (1 << 6)
#define  FIFO_WRITE_MASK     (2 << 6)
#define  FIFO_READ_MASK      (3 << 6)




/*全局寄存器*/
#define  GENA_REG_ADDR    (0x00)
#define  GRST_REG_ADDR    (0x01)
#define  GIER_REG_ADDR    (0x10)
#define  GIFR_REG_ADDR    (0x11)

/*uart寄存器*/
/*page0*/
#define  SPAGE_REG_ADDR  (0x03)/*页选择寄存器*/
#define  SCR_REG_ADDR    (0x04)/*控制寄存器*/

#define  LCR_REG_ADDR    (0x05)/*配置寄存器*/
#define  FCR_REG_ADDR    (0x06)/*FIFO控制寄存器*/

#define  SIER_REG_ADDR   (0x07)/*中断使能寄存器*/
#define  SIFR_REG_ADDR   (0x08)/*中断标志寄存器*/

#define  TFCNT_REG_ADDR  (0x09)/*FIFO待发送计数器*/
#define  RFCNT_REG_ADDR  (0x0a)/*FIFO接收计数器*/

#define  FSR_REG_ADDR    (0x0b)/*FIFO状态寄存器*/
#define  LSR_REG_ADDR    (0x0c)/*接收状态寄存器*/
#define  FDAT_REG_ADDR   (0x0d)/*FIFO数据寄存器*/
/*page1*/
#define  BAUD1_REG_ADDR  (0x04)/*波特率高字节寄存器*/
#define  BAUD0_REG_ADDR  (0x05)/*波特率低字节寄存器*/
#define  PRES_REG_ADDR   (0x06)/*波特率小数寄存器*/
#define  RFTL_REG_ADDR   (0x07)/*接收FIFO触发阈值寄存器*/
#define  TFTL_REG_ADDR   (0x08)/*发送FIFO触发阈值寄存器*/


void wk2124_spi_read(uint8_t *buffer,uint8_t size);
void wk2124_spi_write(uint8_t *buffer,uint8_t size);
void wk2124_cs_enable();
void wk2124_cs_disable();


static void wk2124_read_register(uint8_t reg_addr,uint8_t *buffer,uint8_t cnt)
{
    uint8_t command = 0;

    command = reg_addr | REG_READ_MASK;
    wk2124_cs_enable();
    wk2124_spi_write(&command, 1);
    wk2124_spi_read(buffer, cnt);
    wk2124_cs_disable();

}

static void wk2124_write_register(uint8_t reg_addr,uint8_t *buffer,uint8_t cnt)
{
    uint8_t command = 0;

    command = reg_addr | REG_WRITE_MASK;
    wk2124_cs_enable();
    wk2124_spi_write(&command, 1);
    wk2124_spi_write(buffer, cnt);
    wk2124_cs_disable();

}

static void wk2124_read_uart_register(uint8_t uart_index,uint8_t reg_addr,uint8_t *buffer,uint8_t cnt)
{
    uint8_t command = 0;

    command = reg_addr | UART_INDEX_VBIT(uart_index) | REG_READ_MASK;
    wk2124_cs_enable();
    wk2124_spi_write(&command, 1);
    wk2124_spi_read(buffer, cnt);
    wk2124_cs_disable();

}

static void wk2124_write_uart_register(uint8_t uart_index,uint8_t reg_addr,uint8_t *buffer,uint8_t cnt)
{
    uint8_t command = 0;

    command = reg_addr | UART_INDEX_VBIT(uart_index) | REG_WRITE_MASK;
    wk2124_cs_enable();
    wk2124_spi_write(&command, 1);
    wk2124_spi_write(buffer, cnt);
    wk2124_cs_disable();

}
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
void wk2124_read_uart_fifo(uint8_t uart_index,uint8_t *buffer,uint8_t cnt)
{
    uint8_t command = 0;

    command = UART_INDEX_VBIT(uart_index) | FIFO_READ_MASK;
    wk2124_cs_enable();
    wk2124_spi_write(&command, 1);
    wk2124_spi_read(buffer, cnt);
    wk2124_cs_disable();

}
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
void wk2124_write_uart_fifo(uint8_t uart_index,uint8_t *buffer,uint8_t cnt)
{
    uint8_t command = 0;

    command = UART_INDEX_VBIT(uart_index) | FIFO_WRITE_MASK;
    wk2124_cs_enable();
    wk2124_spi_write(&command, 1);
    wk2124_spi_write(buffer, cnt);
    wk2124_cs_disable();

}

/**
* @brief 使能串口
* @details
* @param index 串口标号
* @return 无
* @attention
* @note
*/
void wk2124_enable_uart(int index)
{

    uint8_t register_value;

    /*使能串口时钟*/
    wk2124_read_register(GENA_REG_ADDR,&register_value,1);
    register_value |= 1 << index;
    wk2124_write_register(GENA_REG_ADDR, &register_value,1);
    
   /*复位串口*/
    wk2124_read_register(GRST_REG_ADDR,&register_value,1);
    register_value |= 1 << index;
    wk2124_write_register(GRST_REG_ADDR, &register_value,1);

    /*使能串口中断*/
    wk2124_read_register(GIER_REG_ADDR,&register_value,1);
    register_value |= 1 << index;
    wk2124_write_register(GIER_REG_ADDR, &register_value,1);
}

/**
* @brief 禁止串口
* @details
* @param index 串口标号
* @return 无
* @attention
* @note
*/
void wk2124_disable_uart(int index)
{

     uint8_t register_value;
    
     /*禁止串口时钟*/
     wk2124_read_register(GENA_REG_ADDR,&register_value,1);
     register_value &= ~(1 << index);
     wk2124_write_register(GENA_REG_ADDR, &register_value,1);
     
     /*禁止串口中断*/
     wk2124_read_register(GIER_REG_ADDR,&register_value,1);
     register_value &= ~(1 << index);
     wk2124_write_register(GIER_REG_ADDR, &register_value,1);

}

/**
* @brief 串口参数配置
* @details
* @param index 串口参数指针
* @return 无
* @attention
* @note
*/
void wk2124_uart_config(wk2124_config_t *config)
{

    float reg_value =  WK2124_OSC_HZ / 16.0 / (float)config->baud_rates;
    uint8_t band_rates_value[3];

    uint8_t config_value = 0;
    
    uint8_t page = 1;
    /*切换到page1*/
    wk2124_write_uart_register(config->index,SPAGE_REG_ADDR,&page,1);

    /*配置波特率*/
    band_rates_value[0] = ((uint16_t)reg_value - 1) >> 8;
    band_rates_value[1] = ((uint16_t)reg_value - 1) & 0xFF;
    band_rates_value[2] = (uint8_t)(16.0 * (reg_value - (uint16_t)reg_value));
    wk2124_write_uart_register(config->index,BAUD1_REG_ADDR,band_rates_value,3);
   
    /*配置触点中断*/
    config_value = config->fifo_tx_int_level;
    wk2124_write_uart_register(config->index,TFTL_REG_ADDR,&config_value,1);

    config_value = config->fifo_rx_int_level;
    wk2124_write_uart_register(config->index,RFTL_REG_ADDR,&config_value,1);

    /*切换到page0*/
    page = 0;
    wk2124_write_uart_register(config->index,SPAGE_REG_ADDR,&page,1);

    /*配置数据位，停止位，校验位*/
    config_value = 0;
    if (config->data_bits == WK2124_DATA_BITS_9) {
        config_value |= (1 << 3);
        if (config->parity == WK2124_PARITY_ODD) {
            config_value |= (1 << 1);
        } else {
            config_value |= (2 << 1);
        }
    }
    if (config->stop_bits == WK2124_STOP_BITS_2) {
        config_value |= (1 << 0);
    }
    wk2124_write_uart_register(config->index,LCR_REG_ADDR,&config_value,1);

    /*配置fifo*/
    config_value = 0;
    config_value |= (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0);/*使能发送fifo，接收fifo，并复位*/
    wk2124_write_uart_register(config->index,FCR_REG_ADDR,&config_value,1);

    /*配置控制寄存器*/
    config_value = 0;
    config_value |= (1 << 0) | (1 << 1);/*使能接收和发送*/
    wk2124_write_uart_register(config->index,SCR_REG_ADDR,&config_value,1);
}

/**
* @brief 使能uart指定的中断
* @details
* @param uart_index 串口标号
* @param int_type 中断类型
* @return 无
* @attention
* @note
*/
void wk2124_uart_int_enable(uint8_t uart_index,uint8_t int_type)
{
    /*使能中断*/
    uint8_t config_value;
    wk2124_read_uart_register(uart_index,SIER_REG_ADDR,&config_value,1);
    config_value |= int_type;
    wk2124_write_uart_register(uart_index,SIER_REG_ADDR,&config_value,1);
}

/**
* @brief 禁止uart指定的中断
* @details
* @param uart_index 串口标号
* @param int_type 中断类型
* @return 无
* @attention
* @note
*/
void wk2124_uart_int_disable(uint8_t uart_index,uint8_t int_type)
{
    /*禁止中断*/
    uint8_t config_value;
    wk2124_read_uart_register(uart_index,SIER_REG_ADDR,&config_value,1);
    config_value &= ~int_type;
    wk2124_write_uart_register(uart_index,SIER_REG_ADDR,&config_value,1);
}


/**
* @brief 获取指定全局中断标志
* @details
* @param 无
* @return 中断标志
* @attention
* @note
*/
uint8_t wk2124_get_int_flags()
{
    uint8_t int_flags;

    wk2124_read_register(GIFR_REG_ADDR,&int_flags,1);
    return int_flags;
}

/**
* @brief 获取指定串口的中断标志
* @details
* @param index 串口标号
* @return 中断标志
* @attention
* @note
*/
uint8_t wk2124_get_uart_fifo_int_flags(uint8_t uart_index)
{
    uint8_t int_flags;

    wk2124_read_uart_register(uart_index,SIFR_REG_ADDR,&int_flags,1);
    return int_flags;
}

/**
* @brief 获取指定串口的fifo待发送字节数量
* @details
* @param index 串口标号
* @return 字节数量
* @attention
* @note
*/
uint8_t wk2124_uart_get_fifo_send_size(uint8_t uart_index)
{
    uint8_t cnt;
    
    wk2124_read_uart_register(uart_index,TFCNT_REG_ADDR,&cnt, 1);
    return cnt;
}
/**
* @brief 获取指定串口的fifo接收的字节数量
* @details
* @param index 串口标号
* @return 字节数量
* @attention
* @note
*/
uint8_t wk2124_uart_get_fifo_recv_size(uint8_t uart_index)
{
    uint8_t cnt;
    
    wk2124_read_uart_register(uart_index,RFCNT_REG_ADDR,&cnt, 1);
    return cnt;
}

/**
* @brief 获取指定串口的fifo状态
* @details
* @param index 串口标号
* @return 字节数量
* @attention
* @note
*/
uint8_t wk2124_uart_get_fifo_status(uint8_t uart_index)
{
    uint8_t status;

    wk2124_read_uart_register(uart_index,FSR_REG_ADDR,&status,1);

    return status;
}

/**
* @brief 清除指定串口的发送fifo
* @details
* @param index 串口标号
* @return 无
* @attention
* @note
*/
void wk2124_uart_fifo_send_clear(uint8_t uart_index)
{
    uint8_t config_value;

    wk2124_read_uart_register(uart_index,FCR_REG_ADDR,&config_value,1);
    config_value |= (1 << 0);
    wk2124_write_uart_register(uart_index,FCR_REG_ADDR,&config_value,1);
}

/**
* @brief 清除指定串口的接收fifo
* @details
* @param index 串口标号
* @return 无
* @attention
* @note
*/
void wk2124_uart_fifo_recv_clear(uint8_t uart_index)
{
    uint8_t config_value;

    wk2124_read_uart_register(uart_index,FCR_REG_ADDR,&config_value,1);
    config_value |= (1 << 1);
    wk2124_write_uart_register(uart_index,FCR_REG_ADDR,&config_value,1);
}