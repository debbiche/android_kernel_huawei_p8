
#include <bsp_hardtimer.h>
#include <bsp_pm_om.h>
#include "pm_om_platform.h"
#include "pm_om_debug.h"

struct pm_om_debug g_pmom_debug;

int pm_om_debug_init(void)
{
	struct pm_om_ctrl *ctrl = pm_om_ctrl_get();

	memset((void *)&g_pmom_debug, 0, sizeof(g_pmom_debug));

	g_pmom_debug.stat.waket_prev    = bsp_get_slice_value();
	g_pmom_debug.stat.waket_min     = 0xffffffff;
	g_pmom_debug.stat.logt_print_sw = ctrl->log.smem->nv_cfg.reserved;

	g_pmom_debug.ctrl = ctrl;
	ctrl->debug = (void *)&g_pmom_debug;

	return 0;
}

void pm_om_wakeup_log(void)
{
	if (g_pmom_debug.stat.wrs_flag == 1)
	{
		bsp_pm_log(PM_OM_PMLG, 0, NULL);
		g_pmom_debug.stat.wrs_flag = 0;
	}
}

void pm_om_wakeup_stat(void)
{
	u32 cur_slice   = bsp_get_slice_value();
	u32 delta_slice = 0;

	g_pmom_debug.stat.wakeup_cnt++;

	delta_slice = delta_slice - g_pmom_debug.stat.waket_prev;
	g_pmom_debug.stat.waket_prev = cur_slice;
	if (delta_slice > g_pmom_debug.stat.waket_max)
	{
		g_pmom_debug.stat.waket_max = delta_slice;
	}
	if (delta_slice < g_pmom_debug.stat.waket_min)
	{
		g_pmom_debug.stat.waket_min = delta_slice;
	}

	/* 32768hz£¬×ª»»³ÉÃë */
	delta_slice /= TIME_SLICE_FREQ;
	g_pmom_debug.stat.wrs_flag = 1;
	pmom_print("[C SR]pm om wakeup\n");
}

u32 pm_om_log_time_rec(u32 time_log_start)
{
	u32 ret = 0;
	u32 delta_slice = bsp_get_slice_value();
	delta_slice = delta_slice - time_log_start;

	if (g_pmom_debug.stat.logt_print_sw && delta_slice > g_pmom_debug.stat.logt_max)
	{
		g_pmom_debug.stat.logt_max = delta_slice;
		ret = delta_slice;
	}
	return ret;
}

void pm_om_dbg_on(void)
{
	bsp_mod_level_set(BSP_MODU_PM_OM, BSP_LOG_LEVEL_DEBUG);
}

void pm_om_dbg_off(void)
{
	bsp_mod_level_set(BSP_MODU_PM_OM, BSP_LOG_LEVEL_ERROR);
}

void pm_om_dump_show(void)
{
	u32 i = 0;
	char *magic = NULL;
	struct pm_om_ctrl *ctrl = pm_om_ctrl_get();

	pmom_print("**********************************************\n");
	pmom_print("init_flag : 0x%x\n", ctrl->dump.init_flag);
	pmom_print("base_addr : 0x%x\n", ctrl->dump.base);
	pmom_print("length    : 0x%x\n", PM_OM_DUMP_SIZE);
	pmom_print("region_num: 0x%x\n", ctrl->dump.region_cnt);

	for (i = 0; i < ctrl->dump.region_cnt; i++)
	{
		magic = (char *)(&(ctrl->dump.cfg->entry_tbl[i].magic));
		pmom_print("[%c%c%c%c]    : 0x%x\n",magic[0], magic[1], magic[2], magic[3], ctrl->dump.cfg->entry_tbl[i].len);
	}
	pmom_print("**********************************************\n");
}

void pm_om_log_show(void)
{
	struct pm_om_ctrl *ctrl = pm_om_ctrl_get();
	pmom_print("**********************************************\n");
	pmom_print("init_flag   : 0x%x\n", ctrl->log.init_flag);
	pmom_print("app_is_alive: 0x%x\n", ctrl->log.app_is_alive);
	pmom_print("buf_is_full : 0x%x\n", ctrl->log.buf_is_full);
	pmom_print("threshold   : 0x%x\n", ctrl->log.threshold);
	pmom_print("debug       : 0x%p\n", ctrl->debug);
	pmom_print("platform    : 0x%p\n", ctrl->platform);
	pmom_print("**********************************************\n");
}

void pm_om_debug_show(void)
{
	pmom_print("**********************************************\n");
	pmom_print("logt_print_sw: 0x%x\n", g_pmom_debug.stat.logt_print_sw);
	pmom_print("logt_max\t: 0x%x\n", g_pmom_debug.stat.logt_max);
	pmom_print("**********************************************\n");
}

void pm_om_wake_interval_set(u32 interval)
{
	struct pm_om_ctrl *ctrl = pm_om_ctrl_get();
	struct pm_om_platform *platform = (struct pm_om_platform *)ctrl->platform;

	platform->wakeup_interval = interval;
}

void pm_om_help(void)
{
	pmom_print("***********************\n");
	pmom_print("pm_om_dump_show\n");
	pmom_print("pm_om_log_show\n");
	pmom_print("pm_om_debug_show\n");
	pmom_print("pm_om_dbg_on\n");;
	pmom_print("pm_om_dbg_off\n");
	pmom_print("pm_om_wake_interval_set\n");
	pmom_print("***********************\n");
}
