
#ifndef __BSP_DUAL_MODEM_H_
#define __BSP_DUAL_MODEM_H_
/*lint --e{830}*/

extern int dual_modem_init(void);
extern void wakeup_modem_init(void);

#if defined(__CMSIS_RTOS) 
extern void bsp_dual_modem_disable_cb(void);
#endif

#endif

