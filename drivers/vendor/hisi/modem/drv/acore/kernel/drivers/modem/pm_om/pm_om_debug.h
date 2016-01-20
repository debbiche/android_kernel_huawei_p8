
#ifndef __PM_OM_DEBUG_H__
#define __PM_OM_DEBUG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <bsp_pm_om.h>

#define TIME_SLICE_FREQ  (32768)
struct pm_om_stat
{
	u32 wakeup_cnt;
	u32 waket_prev;
	u32 waket_min;
	u32 waket_max;
	u32 logt_max;
	u32 logt_print_sw;
	u32 wrs_flag;
};

struct pm_om_debug
{
	struct pm_om_stat stat;
	struct pm_om_ctrl *ctrl;
};

int pm_om_debug_init(void);
void pm_om_wakeup_stat(void);
u32 pm_om_log_time_rec(u32 time_log_start);
static inline void pm_om_log_time_print(u32 log_sw)
{
	if (log_sw > 0)
	{
		pmom_pr_err("elapse time is %d(slice)\n", log_sw);
	}
}


#ifdef __cplusplus
}
#endif

#endif  /* __PM_OM_DEBUG_H__ */
