#include "fsl_spi.h"
#include "fsl_clock.h"
#include "pin_mux.h"
#include "wk2124.h"
#include "wk2124_uart_hal_driver.h"

/*******************************************************************************
*
*       wk2124 uart hal driver在IAR freertos下的移植
*
********************************************************************************/
static int wk2124_uart_hal_init(uint8_t port,uint32_t baud_rates,uint8_t data_bits,uint8_t stop_bits);
static int wk2124_uart_hal_deinit(uint8_t port);
static void wk2124_uart_hal_enable_txe_it(uint8_t port);
static void wk2124_uart_hal_disable_txe_it(uint8_t port);
static void wk2124_uart_hal_enable_rxne_it(uint8_t port);
static void wk2124_uart_hal_disable_rxne_it(uint8_t port);

/*nxp uart驱动结构体*/
xuart_hal_driver_t xuart_wk2124_hal_driver = {
.init = wk2124_uart_hal_init,
.deinit = wk2124_uart_hal_deinit,
.enable_txe_it = wk2124_uart_hal_enable_txe_it,
.disable_txe_it = wk2124_uart_hal_disable_txe_it,
.enable_rxne_it = wk2124_uart_hal_enable_rxne_it,
.disable_rxne_it = wk2124_uart_hal_disable_rxne_it
};

#define  WK2124_UART_PORT_BASE  9

static int wk2124_uart_hal_init(uint8_t port,uint32_t baud_rates,uint8_t data_bits,uint8_t stop_bits)
{
    uint8_t index;
    wk2124_config_t config;

    if (port < WK2124_UART_PORT_BASE) {
        return -1;
    }
    index = port - WK2124_UART_PORT_BASE;

    config.index = index;
    config.baud_rates = baud_rates;
    if (data_bits == 1) {
        config.stop_bits = WK2124_STOP_BITS_1;
    } else {
        config.stop_bits = WK2124_STOP_BITS_2;
    }

    if (data_bits == 8) {
        config.data_bits = WK2124_DATA_BITS_8;
    } else {
        config.data_bits = WK2124_DATA_BITS_9;
    }

    config.parity = WK2124_PARITY_NONE;
    config.fifo_tx_int_level = 1;
    config.fifo_rx_int_level = 32;

    wk2124_enable_uart(index);
    wk2124_uart_config(&config);

    return 0;
}

static int wk2124_uart_hal_deinit(uint8_t port)
{
    return 0;
}

static void wk2124_uart_hal_enable_txe_it(uint8_t port)
{
    uint8_t index;

    index = port - WK2124_UART_PORT_BASE;
    wk2124_uart_int_enable(index,WK2124_UART_FIFO_SEND_LEVEL_INT);
}

static void wk2124_uart_hal_disable_txe_it(uint8_t port)
{
    uint8_t index;

    index = port - WK2124_UART_PORT_BASE;
    wk2124_uart_int_disable(index,WK2124_UART_FIFO_SEND_LEVEL_INT);
}


static void wk2124_uart_hal_enable_rxne_it(uint8_t port)
{
    uint8_t index;

    index = port - WK2124_UART_PORT_BASE;
    wk2124_uart_int_enable(index,WK2124_UART_FIFO_RECV_TIMEOUT_INT);
}


static void wk2124_uart_hal_disable_rxne_it(uint8_t port)
{
    uint8_t index;

    index = port - WK2124_UART_PORT_BASE;
    wk2124_uart_int_disable(index,WK2124_UART_FIFO_RECV_TIMEOUT_INT);
}
