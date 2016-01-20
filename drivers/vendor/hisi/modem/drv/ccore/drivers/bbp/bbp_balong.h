
/*lint --e{537}*/
#ifndef _BBP_BALONG_H_
#define _BBP_BALONG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_memmap.h"
#include "bsp_bbp.h"

typedef enum{
	BBP_IRM = 0,
	BBP_G1,
	BBP_G2,
	BBP_TW,
	BBP_W,
	BBP_L,
	BBP_T,
	BBP_BUTT
}BBP_MODE_ID_E;
typedef enum{
    BBP_LTEINT_EN_IN,          /*0*/
    BBP_LTEINT_EN_CLS,         /*1*/
    BBP_LTEINT_EN_UNMASK,      /*2*/
    BBP_LTEINT_DIS_IN,         /*3*/
    BBP_LTEINT_DIS_MASK,       /*4*/
    BBP_LTEINT_HANDLE_IN,      /*5*/
    BBP_LTEINT_HANDLE_CLS,     /*6*/
    BBP_LTEINT_HANDLE_FUNC,    /*7*/
    BBP_LTEINT_CLEAR_IN,       /*8*/
    BBP_LTEINT_CLEAR_CLS,      /*9*/
    BBP_LTEINT_BUTT
}BBP_LTEINTSTAMP_E;
typedef enum{
    BBP_TDSINT_EN_IN,          /*0*/
    BBP_TDSINT_EN_CLS,         /*1*/
    BBP_TDSINT_EN_UNMASK,      /*2*/
    BBP_TDSINT_DIS_IN,         /*3*/
    BBP_TDSINT_DIS_MASK,       /*4*/
    BBP_TDSINT_CLEAR_IN,       /*5*/
    BBP_TDSINT_CLEAR_CLS,      /*6*/
    BBP_TDSINT_HANDLE_IN,      /*7*/
    BBP_TDSINT_HANDLE_FUNC,    /*8*/
    BBP_TDSINT_BUTT
}BBP_TDSINTSTAMP_E;
typedef enum{
    BBP_PREPAIR_IN,            /*0*/
    BBP_PREPAIR_CLKEN,         /*1*/
    BBP_PREPAIR_DMACFG,        /*2*/
    BBP_PREPAIR_END,           /*3*/
    BBP_SUSPEND_IN,            /*4*/
    BBP_SUSPEND_DMAOK,         /*5*/
    BBP_SUSPEND_CLKDIS,        /*6*/
    BBP_SUSPEND_BBCRST,        /*7*/
    BBP_SUSPEND_END,           /*8*/
    BBP_SUSPEND_BUTT
}BBP_SUSPENDSTAMP_E;
typedef enum{
    BBP_RESUME_IN,             /*0*/
    BBP_RESUME_BBCUNRST,       /*1*/
    BBP_RESUME_DMACFG,         /*2*/
    BBP_RESUME_END,            /*3*/
    BBP_COMPLETE_IN,           /*4*/
    BBP_COMPLETE_DMAOK,        /*5*/
    BBP_COMPLETE_CLKDIS,       /*6*/
    BBP_COMPLETE_END,          /*7*/
    BBP_RESUME_BUTT
}BBP_RESUMESTAMP_E;
struct bbp_om_info
{
    u32 stamp_ltetf[BBP_LTEINT_BUTT];
    u32 stamp_tdstf[BBP_TDSINT_BUTT];
    u32 stamp_pwron[BBP_BUTT];
    u32 stamp_pwroff[BBP_BUTT];
    u32 stamp_clkon[BBP_BUTT];
    u32 stamp_clkoff[BBP_BUTT];
    u32 stamp_suspend[BBP_SUSPEND_BUTT];
    u32 stamp_resume[BBP_RESUME_BUTT];
};

typedef struct tagBBP_DEBUG_INFO_S
{
    u32 bbpdltbcnt;
    u32 bbptimercnt;
    u32 bbptdscnt;
    BSPBBPIntDlTbFunc  bbpdltbfunc;
    BSPBBPIntTimerFunc bbptimerfunc;
    BSPBBPIntTdsFunc   g_BBPIntTdsTFFunc;
    struct bbp_om_info *om;                      /*可维可测相关*/
    struct bbp_om_info ominfo;  /*dump空间分配失败时记录到该空间*/
}BBP_INFO_S;

/*CHn addr = CH0 addr + 0x10*n,n指的是dma通道*/
#define BBP_FAST_CH0_BBP_ADDR_OFFSET    0x0a00  /*BBP起始地址*/
#define BBP_FAST_CH0_SOC_ADDR_OFFSET    0x0a04 /*SOC起始地址，可以是DDR或者TCM*/
#define BBP_FAST_CH0_CTL_OFFSET    0x0a0c /*控制寄存器*/

#define  BBP_TASK_STK_SIZE            (0x1000)

/*函数声明*/
int bbp_poweron(void);
int bbp_poweroff(void);

#ifdef __cplusplus
}
#endif

#endif
