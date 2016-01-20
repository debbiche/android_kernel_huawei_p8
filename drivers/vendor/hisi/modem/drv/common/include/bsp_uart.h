/*
 * bsp_uart.h 
 *
 * Copyright (C) 2012 Huawei Corporation
 * 
 *author:wangxiandong
 */
#ifndef __BSP_UART_H__
#define __BSP_UART_H__
/*lint --e{628}*/

#include <product_config.h>

#ifdef CONFIG_CCORE_PM
int balongv7r2_uart_suspend(void);
int balongv7r2_uart_resume(void);
#else
static inline int balongv7r2_uart_suspend(void) {return 0;}
static inline int balongv7r2_uart_resume(void) {return 0;}
#endif
int balongv7r2_m_uart_suspend(void);
int balongv7r2_m_uart_resume(void);
void printksync(const char *fmt, ...);
#endif

