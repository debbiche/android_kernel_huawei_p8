

#ifndef __RESET_BALONG_H__
#define __RESET_BALONG_H__

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#include <osl_common.h>
#include <ipc_center.h>


/* MDDRC */
#define MDDRC_AXI_PORT_STATUS			(HI_MDDRC_REGBASE_ADDR + 0x600)	/* 0x40220600 */
#define MDDRC_AXI_PORT_MDM_MSK			((u32)0x1 << 0)
#define MDDRC_AXI_PORT_MDM_BUSY			((u32)0x1 << 0)

/* PMC */
#define PMC_NOC_POWER_IDLEREQ			(HI_PMC_REGBASE_ADDR + 0x380)	/* 0x40231380 */
#define PMC_NOC_POWER_IDLEACK			(HI_PMC_REGBASE_ADDR + 0x384)	/* 0x40231384 */
#define PMC_NOC_POWER_IDLESTAT			(HI_PMC_REGBASE_ADDR + 0x388)	/* 0x40231388 */
#define PMC_PERI_INT0_STAT				(HI_PMC_REGBASE_ADDR + 0x3A4)	/* 0x402313A4 */
#define PMC_PERI_INT0_STAT_MDM_MSK		((u32)0x1 << 17)
#define PMC_PERI_INT0_STAT_MDM_PEND		((u32)0x1 << 17)
#define PMC_NOC_POWER_IDLE_MDM_MSK		((u32)0x1 << 3)
#define PMC_NOC_POWER_IDLE_MDM_REQIN	((u32)0x1 << 3)
#define PMC_NOC_POWER_IDLE_MDM_ACKIN	((u32)0x1 << 3)


/* PERI_CRG  */
#define PERI_CRG_PEREN5					(HI_LP_PERI_CRG_REG_ADDR + 0x050)	/* 0x40235050 */
#define PERI_CRG_PERRSTDIS0				(HI_LP_PERI_CRG_REG_ADDR + 0x064)	/* 0x40235064 */
#define PERI_CRG_PERRSTEN4				(HI_LP_PERI_CRG_REG_ADDR + 0x090)	/* 0x40235090 */
#define PERI_CRG_PERRSTDIS4				(HI_LP_PERI_CRG_REG_ADDR + 0x094)	/* 0x40235094 */
#define PERI_CRG_PERCTRL3				(HI_LP_PERI_CRG_REG_ADDR + 0x12C)	/* 0x4023512C */
#define PERI_CRG_MCPU_FIQ_MSK			((u32)0x1 << 12)

/* NOC BUS */
#define NOC_BUS_CFG_FAULT_EN			(HI_NOC_BUS_REGBASE_ADDR + 0x408)
#define NOC_BUS_CFG_ERR_PROBE_MSK		((u32)0x1 << 0)


/* PCTRL */
#define PCTRL_PERI_STAT3				(HI_PCTRL_REGBASE_ADDR +0x0A0)	/* 0xA8A090A0 */
#define PCTRL_PERI_CTRL11				(HI_PCTRL_REGBASE_ADDR +0xC18)	/* 0xA8A09C18 */
#define PCTRL_PERI_STAT3_MDM_MSK		((u32)0x1 << 9)

#define reset_print_err(fmt, ...)		(printk("[%s] "fmt"\n", __FUNCTION__, ##__VA_ARGS__))
#define AP_TO_LPM3_IPC_FLAG				(OBJ_AP<<24|OBJ_MODEM<<16|CMD_SETTING<<8)

struct mcore_reset_ctrl
{
	u32 boot_mode;
	u32 multicore_msg_switch;
	u32 main_stage;
	u32 ap_access_mdm_bus_err_cnt;
	u32 mdm_access_ap_bus_err_cnt;
	u32 mdm_ddr_access_err_cnt;
	u32 print_sw;
	u32 state;
};

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif    /*  __RESET_BALONG_H__ */
