#ifndef  __WK2124_UART_HAL_DRIVER_H__
#define  __WK2124_UART_HAL_DRIVER_H__
 
#ifdef  __cplusplus
#define  WK2124_UART_HAL_DRIVER_BEGIN extern "C" {
#define  WK2124_UART_HAL_DRIVER_END   }
#else
#define  WK2124_UART_HAL_DRIVER_BEGIN
#define  WK2124_UART_HAL_DRIVER_END
#endif
 
WK2124_UART_HAL_DRIVER_BEGIN
#include "xuart.h"
extern xuart_hal_driver_t wk2124_xuart_hal_driver;

WK2124_UART_HAL_DRIVER_END
#endif
