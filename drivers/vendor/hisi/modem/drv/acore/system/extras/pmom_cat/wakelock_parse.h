

#ifndef  __WAKELOCK_PARSE_H__
#define  __WAKELOCK_PARSE_H__

#ifdef __cplusplus
    extern "C" {
#endif
#include <linux/types.h>

enum lock_enum{
	WAKE_UNLOCK,
	WAKE_LOCK,
};

struct wakelock_om_s{
	u32 stamp;
	u32 lock_id;
	u32 lock_type;
	u32 lock_state;
	u32 cur_task_id;
};
typedef enum tagPWC_CLIENT_ID_E
{
    PWRCTRL_SLEEP_TLPS = 0,   /*MSP--fuxin*/
    PWRCTRL_SLEEP_PS_G0,        /*GU--ganlan*/
    PWRCTRL_SLEEP_PS_W0,       /*GU--ganlan*/
    PWRCTRL_SLEEP_PS_G1,        /*GU--ganlan*/
    PWRCTRL_SLEEP_PS_W1,       /*GU--ganlan*/
    PWRCTRL_SLEEP_FTM,           /*GU--zhangyizhan*/
    PWRCTRL_SLEEP_FTM_1,       /*GU--zhangyizhan*/
    PWRCTRL_SLEEP_NAS,           /*GU--zhangyizhan*/
    PWRCTRL_SLEEP_NAS_1,       /*GU--zhangyizhan*/
    PWRCTRL_SLEEP_OAM,		   /* LTE --yangzhi */
    PWRCTRL_SLEEP_SCI0,		   /* LTE --yangzhi */
    PWRCTRL_SLEEP_SCI1,		   /* LTE --yangzhi */
    PWRCTRL_SLEEP_DSFLOW,	   /* NAS --zhangyizhan */
    PWRCTRL_SLEEP_TEST,		   /* PM  ---shangmianyou */
    PWRCTRL_SLEEP_UART0,        /*UART0 -zhangliangdong */
    PWRCTRL_SLEEP_TDS,         /*TRRC&TL2----leixiantiao*/
    /*以下部分的ID已经不使用了，后期会删除*/
    PWRCTRL_SLEEP_RNIC,
    LOCK_ID_BOTTOM =32
}PWC_CLIENT_ID_E;

int wakelock_msg_parse(char *in_buf, unsigned int len, char *out_buf, unsigned int *out_ptr);

#ifdef __cplusplus
}
#endif

#endif
