
#include <product_config.h>
#include <bsp_memmap.h>
#include <bsp_reset.h>
#include <bsp_icc.h>
#include <wakelock.h>
#include <wdt_balong.h>
#include <modem_start.h>
#include <bsp_dual_modem.h>
#include "reset_balong.h"

#ifndef NOC_LOCK_ID
#define NOC_LOCK_ID 16
#endif

struct mcore_reset_ctrl g_mreset_ctrl = {0};


#define  reset_print_debug(fmt, ...) \
do {                               \
    if (g_mreset_ctrl.print_sw)    \
        reset_print_err(fmt, ##__VA_ARGS__);\
} while (0)

#define CCORE_RST_TIMEOUT_NUM         (327) /*10ms*/
#define CHECK_TIMEOUT(a)   (get_timer_slice_delta(a, bsp_get_slice_value()) < CCORE_RST_TIMEOUT_NUM)

#ifdef CONFIG_BALONG_MODEM_RESET
extern void ccore_ipc_disable(void);
extern void ccore_ipc_enable(void);
extern void reset_ipc_status_bakup(void);
extern void reset_ipc_status_resume(void);
extern void ipc_modem_reset_cb(DRV_RESET_CALLCBFUN_MOMENT stage, int userdata);
extern void cpufreq_resume(void);
extern int bsp_ipf_reset_ccore_lpm3_cb(DRV_RESET_CALLCBFUN_MOMENT eparam, int userdata);
void modem_a9_unreset(void)
{
    /* 16 a9 start addr */
    *(u32 *)STAMP_UNRESET_M3_A9_STAR_ADDR = bsp_get_slice_value();
#ifdef ROM_COMPRESS
    writel(MCORE_TEXT_START_ADDR_COMPRESSED >> 16, PCTRL_PERI_CTRL11);
#else
    writel(MCORE_TEXT_START_ADDR >> 16, PCTRL_PERI_CTRL11);
#endif

    /* 18 unreset a9*/
    *(u32 *)STAMP_UNRESET_M3_UNRESET_A9 = bsp_get_slice_value();
    writel(0x1<<15, HI_SYSCTRL_BASE_ADDR + HI_CRG_SRSTDIS1_OFFSET);
    writel(0x1<<6, HI_SYSCTRL_BASE_ADDR + HI_CRG_SRSTDIS1_OFFSET);
}
void reset_m3_stamp_addr_value(void)
{
    *(u32 *)STAMP_RESET_M3_BASE_ADDR = 0x33333333;
    *(u32 *)STAMP_RESET_M3_BUSERROR_STEP1 = 0x0;
    *(u32 *)STAMP_RESET_M3_BUSERROR_STEP2 = 0x0;
    *(u32 *)STAMP_RESET_M3_BUSERROR_STEP3 = 0x0;
    *(u32 *)STAMP_RESET_M3_NOC_PROBE_DISABLE = 0x0;
    *(u32 *)STAMP_RESET_M3_NOC_ENTER_LPMODE_CONFIG = 0x0;
    *(u32 *)STAMP_RESET_M3_NOC_ENTER_LPMODE_STEP1 = 0x0;
    *(u32 *)STAMP_RESET_M3_NOC_ENTER_LPMODE_STEP2 = 0x0;
    *(u32 *)STAMP_RESET_M3_CLEAN_NMI = 0x0;
    *(u32 *)STAMP_RESET_M3_RESET_SUBSYS = 0x0;
    *(u32 *)STAMP_UNRESET_M3_BASE_ADDR = 0x33333333;
    *(u32 *)STAMP_UNRESET_M3_UNRESET_SUBSYS = 0x0;
    *(u32 *)STAMP_UNRESET_M3_A9_STAR_ADDR = 0x0;
    *(u32 *)STAMP_UNRESET_M3_UNRESET_A9 = 0x0;
}

int modem_subsys_reset(void)
{
    u32 tmp = 0;
    u32 slicebegin = 0;
    reset_m3_stamp_addr_value();

    /* ap侧访问modem子系统, 是否modem子系统总线已经挂死: PMCtrl中，modem global总线是否挂死[17]==0x1 */
    *(u32 *)STAMP_RESET_M3_BUSERROR_STEP1 = bsp_get_slice_value();
    tmp = readl(PMC_PERI_INT0_STAT);
    tmp &= PMC_PERI_INT0_STAT_MDM_MSK;
    if(PMC_PERI_INT0_STAT_MDM_PEND == tmp)
	{
         ++g_mreset_ctrl.ap_access_mdm_bus_err_cnt;
        return (int)RESET_MCORE_BEFORE_AP_TO_MDM_BUS_ERR;
    }

    /* modem侧访问AP, 是否modem子系统被挂死: PCtrl中，modem 子系统是否挂死[9]==0x0 */
    *(u32 *)STAMP_RESET_M3_BUSERROR_STEP2 = bsp_get_slice_value();
    slicebegin = bsp_get_slice_value();
    do
	{
		tmp = readl(PCTRL_PERI_STAT3);
		tmp &= PCTRL_PERI_STAT3_MDM_MSK;
	}while((tmp == 0x0) && CHECK_TIMEOUT(slicebegin));
    if(!tmp)
	{
        ++g_mreset_ctrl.mdm_access_ap_bus_err_cnt;
        return (int)RESET_MCORE_BEFORE_MDM_TO_AP_BUS_ERR;
    }

    /* modem访问DDRC，是否已经挂死: ddrc phy中，modem 访问DDR是否挂死[0]==0x1 */
    *(u32 *)STAMP_RESET_M3_BUSERROR_STEP3 = bsp_get_slice_value();
    slicebegin = bsp_get_slice_value();
    do
	{
		tmp = readl(MDDRC_AXI_PORT_STATUS);
		tmp &= MDDRC_AXI_PORT_MDM_MSK;
	}while((MDDRC_AXI_PORT_MDM_BUSY == tmp) && CHECK_TIMEOUT(slicebegin));
    if(MDDRC_AXI_PORT_MDM_BUSY == tmp)
	{
		++g_mreset_ctrl.mdm_ddr_access_err_cnt;
        return (int)RESET_MCORE_BEFORE_MODEM_ACCESS_DDRC_ERR;
    }

    /* 配置error probe特性disable */
    *(u32 *)STAMP_RESET_M3_NOC_PROBE_DISABLE = bsp_get_slice_value();
    tmp = readl(NOC_BUS_CFG_FAULT_EN);
    tmp &= ~(NOC_BUS_CFG_ERR_PROBE_MSK);
    writel(tmp, NOC_BUS_CFG_FAULT_EN);

    /* 配置NOC进入低功耗模式 */
	hwspin_lock_timeout(NOC_LOCK_ID, WAITFVR);
    *(u32 *)STAMP_RESET_M3_NOC_ENTER_LPMODE_CONFIG = bsp_get_slice_value();
    tmp = readl(PMC_NOC_POWER_IDLEREQ);
    tmp |= PMC_NOC_POWER_IDLE_MDM_MSK;
    writel(tmp, PMC_NOC_POWER_IDLEREQ);
	hwspin_unlock(NOC_LOCK_ID);

    /* 等待NOC进入低功耗模式, 0b'1表示进入 */
    slicebegin = bsp_get_slice_value();
    *(u32 *)STAMP_RESET_M3_NOC_ENTER_LPMODE_STEP1 = bsp_get_slice_value();
    do
	{
		tmp = readl(PMC_NOC_POWER_IDLESTAT);
		tmp &= PMC_NOC_POWER_IDLE_MDM_MSK;
	}while(tmp != PMC_NOC_POWER_IDLE_MDM_REQIN && CHECK_TIMEOUT(slicebegin));
    if(tmp != PMC_NOC_POWER_IDLE_MDM_REQIN)
	{
        return (int)RESET_MCORE_BEFORE_NOC_POWER_IDLEIN_ERR;
    }

	/* 等待NOC进入低功耗模式响应, 0b'1表示进入响应 */
    *(u32 *)STAMP_RESET_M3_NOC_ENTER_LPMODE_STEP2 = bsp_get_slice_value();
    slicebegin = bsp_get_slice_value();
    do
	{
		tmp = readl(PMC_NOC_POWER_IDLEACK);
		tmp &= PMC_NOC_POWER_IDLE_MDM_MSK;
	}while(tmp != PMC_NOC_POWER_IDLE_MDM_ACKIN && CHECK_TIMEOUT(slicebegin));
    if(tmp != PMC_NOC_POWER_IDLE_MDM_ACKIN)
	{
        return (int)RESET_MCORE_BEFORE_NOC_POWER_IDLEINACK_ERR;
    }

    /*清除NMI中断*/
    *(u32 *)STAMP_RESET_M3_CLEAN_NMI = bsp_get_slice_value();
    tmp = readl(PERI_CRG_PERCTRL3);
    tmp |= PERI_CRG_MCPU_FIQ_MSK;
    writel(tmp, PERI_CRG_PERCTRL3);

     /*reset subsys*/
    *(u32 *)STAMP_RESET_M3_RESET_SUBSYS = bsp_get_slice_value();
    writel(0x10000000, PERI_CRG_PERRSTEN4);
    return 0;
}

int modem_subsys_unreset(void)
{
    u32 tmp = 0;
    u32 slicebegin = 0;

    /*unreset subsys*/
    writel(0x804, PERI_CRG_PEREN5);
    writel(0x18000000, PERI_CRG_PERRSTDIS0);
    writel(0x10000000, PERI_CRG_PERRSTDIS4);

    /* 配置NOC退出低功耗模式 */
	hwspin_lock_timeout(NOC_LOCK_ID, WAITFVR);
    tmp = readl(PMC_NOC_POWER_IDLEREQ);
    tmp &= (~PMC_NOC_POWER_IDLE_MDM_MSK);
    writel(tmp, PMC_NOC_POWER_IDLEREQ);
	hwspin_unlock(NOC_LOCK_ID);

    /* 等待NOC退出低功耗模式 0b'0表示退出 */
    slicebegin = bsp_get_slice_value();
    do
	{
		tmp = readl(PMC_NOC_POWER_IDLESTAT);
		tmp &= PMC_NOC_POWER_IDLE_MDM_MSK;
	}while(tmp && CHECK_TIMEOUT(slicebegin));
    if(tmp){
        return (int)RESET_MCORE_BEFORE_NOC_POWER_IDLEOUT_ERR;
    }

	/* 等待NOC退出低功耗响应 0b'0表示退出响应 */
    slicebegin = bsp_get_slice_value();
    do
	{
		tmp = readl(PMC_NOC_POWER_IDLEACK);
		tmp &= PMC_NOC_POWER_IDLE_MDM_MSK;
	}while(tmp && CHECK_TIMEOUT(slicebegin));
    if(tmp){
        return (int)RESET_MCORE_BEFORE_NOC_POWER_IDLEOUTACK_ERR;
    }

    /* 配置error probe特性enable */
    tmp = readl(NOC_BUS_CFG_FAULT_EN);
    tmp |= NOC_BUS_CFG_ERR_PROBE_MSK;
    writel(tmp, NOC_BUS_CFG_FAULT_EN);

    /* sec config*/
    /*writel(0x0, HI_SYSCTRL_BASE_ADDR + HI_SEC_CTRL0_OFFSET);*/
	tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_SEC_CTRL0_OFFSET);
	tmp = tmp & ~((u32)0x1 << 9);  /*nsec-read ok*/
	writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_SEC_CTRL0_OFFSET);
	
	/* disable acp region */
	writel(0x0, HI_SYSCTRL_BASE_ADDR + 0x470);
	writel(0x0, HI_SYSCTRL_BASE_ADDR + 0x474);

    /* 1 2 mtcmos power up and wait for complete */
    writel(0x1<<1, HI_SYSCTRL_BASE_ADDR + HI_PWR_CTRL6_OFFSET);

    do
	{
		tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_PWR_STAT1_OFFSET);
		tmp = (tmp >> 1) & 0x1;
	}while(!tmp);
    udelay(30);

    mdm_a9pll_init();

	mdm_bbepll_init();

    mdm_bbppll_init();

    /* a9 dsp ref_clk enable*/
    writel(0x3<<24, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKEN3_OFFSET);

    /* 13 bus div: 0,0,3*/
    writel(0x40040300, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);

    /* 14 switch on a9 pll*/
    writel(0x40010300, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);

    /* 15 enable a9 and L2 clk */
    writel(0x3<<3, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKEN4_OFFSET);

    /* 17 disable iso */
    writel(0x1<<1, HI_SYSCTRL_BASE_ADDR + HI_PWR_CTRL5_OFFSET);

    *(u32 *)STAMP_UNRESET_M3_UNRESET_SUBSYS = bsp_get_slice_value();

	return 0;
}

extern void pm_mdma9_nvic_disable(void);
extern void pm_mdma9_nvic_enable(void);
extern void bsp_socp_ccore_reset_stop_channel(void);

 s32 msg_from_ap_ipc(union ipc_data *msg)
{
	u32 reset_info = 0;
    int irqlock = 0;
	u32 action = 0xff;
	u32 stage  = 0xff;
	s32 ret = RESET_ERROR;

	if(msg->data[0]== AP_TO_LPM3_IPC_FLAG)
	{
		reset_info = (u32)msg->data[1];
		stage  = RESET_STAGE_GET(reset_info);
		action = RESET_STAGE_GET(reset_info);
		printk("reset_info=0x%x, stage=0x%x, action=0x%x\n", reset_info, stage, action);
		local_irq_save(irqlock);
	    switch(stage)
	    {
			/* 复位前 */
			case DRV_RESET_CALLCBFUN_RESET_BEFORE:
				wake_lock(MODEM_MASK);

				g_mreset_ctrl.main_stage = 0;
				ccore_msg_switch_off(g_mreset_ctrl.multicore_msg_switch, CCORE_STATUS);
				pm_mdma9_nvic_disable(); /* 屏蔽唤醒C核的中断 */
				ccore_ipc_disable();
				(void)bsp_ipf_reset_ccore_lpm3_cb(DRV_RESET_CALLCBFUN_RESET_BEFORE, 0);
				ipc_modem_reset_cb(DRV_RESET_CALLCBFUN_RESET_BEFORE, 0);
				reset_ipc_status_bakup();
				bsp_socp_ccore_reset_stop_channel();
				bsp_dual_modem_disable_cb();

				ret = modem_subsys_reset();
				if (ret)
				{
					msg->data[1] = (u32)ret;
					break;
				}

				ret = modem_subsys_unreset();
				if (ret)
				{
					msg->data[1] = (u32)ret;
					break;
				}

				cpufreq_resume();
				reset_ipc_status_resume();
				msg->data[1] = (u32)RESET_MCORE_BEFORE_RESET_OK;

				if (MODEM_POWER_OFF == action)
				{
					wake_unlock(MODEM_MASK);
				}
				break;

			/* 复位中 */
			case DRV_RESET_CALLCBFUN_RESETING:
				wake_lock(MODEM_MASK);

				ccore_msg_switch_on(g_mreset_ctrl.multicore_msg_switch, CCORE_STATUS);
				pm_mdma9_nvic_enable(); /* 屏蔽唤醒C核的中断重新打开 */
				ipc_modem_reset_cb(DRV_RESET_CALLCBFUN_RESETING, 0);
				ccore_ipc_enable();
				//ccore_freq_drop();
				bsp_wdt_enable();
				modem_a9_unreset();
				msg->data[1] = (u32)RESET_MCORE_RESETING_OK;
				break;

			/* 复位后 */
			case DRV_RESET_CALLCBFUN_RESET_AFTER:
				(void)bsp_ipf_reset_ccore_lpm3_cb(DRV_RESET_CALLCBFUN_RESET_AFTER, 0);
				msg->data[1] = (u32)RESET_MCORE_AFTER_RESET_OK;
				break;
			default:
				break;
	    }
		local_irq_restore(irqlock);

	}
	return 0;
}

 s32 bsp_reset_init(void)
{
	memset((void *)&g_mreset_ctrl, 0, sizeof(g_mreset_ctrl));
	g_mreset_ctrl.multicore_msg_switch = 1;

	if(ipc_request_callback(OBJ_MODEM, CMD_SETTING, msg_from_ap_ipc))
	{
		reset_print_err("ipc_request_callback error\n");
		goto fail;
	}

	g_mreset_ctrl.boot_mode = CCORE_BOOT_NORMAL;
	g_mreset_ctrl.state = 1;

	reset_print_err("ok\n");
	return 0;
fail:
	reset_print_err("error\n");
	return 1;
}

 s32 bsp_reset_ccpu_status_get(void)
{
	enum RESET_MULTICORE_CHANNEL_STATUS channel_status = CCORE_STATUS;
	return (g_mreset_ctrl.multicore_msg_switch & channel_status) ? 1 : 0;
}

#else  /* !CONFIG_BALONG_MODEM_RESET */

s32 bsp_reset_ccpu_status_get(void)
{
	reset_print_err("is stub");
	return 1;
}

#endif /* end of CONFIG_BALONG_MODEM_RESET */

