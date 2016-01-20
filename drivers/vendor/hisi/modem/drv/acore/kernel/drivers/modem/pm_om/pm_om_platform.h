

#ifndef __PM_OM_PLATFORM_H__
#define __PM_OM_PLATFORM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <linux/string.h>
#include <linux/semaphore.h>
#include <linux/wakelock.h>
#include <linux/miscdevice.h>
#include <linux/suspend.h>
#include <bsp_dump.h>
#include <bsp_icc.h>
#include <bsp_pm_om.h>

#define PM_OM_DUMP_ID           DUMP_AP_PM_OM
#define PM_OM_DUMP_SIZE         (0x10000)
#define PM_OM_ICC_ACORE_CHN_ID  (ICC_CHANNEL_ID_MAKEUP(ICC_CHN_IFC, IFC_RECV_FUNC_PM_OM))
#define PM_OM_CPUID             (IPC_CORE_ACORE)
#define PM_OM_WAKEUP_INTERVAL   (3000)
#define PM_OM_LOG_THRE_CORE_NUM (4)   /* 有几个核需要配置水线 */
#define PM_OM_LOG_THRE_BUF_SIZE (200) /* 从文件节点读取水线的buffer大小 */

#define memset_s(dest, destMax, val, count) memset(dest, val, count)
#define memcpy_s(dest, destMax, src, count) memcpy(dest, src, count)

struct pm_om_platform
{
	struct pm_om_ctrl *ctrl;
	struct miscdevice misc;  /* misc device */
	wait_queue_head_t wq;    /* wait queue */
	spinlock_t        lock;
	u32 wakeup_interval;
	u32 timestamp;
	u32 fopen_cnt;
	struct wake_lock  wake_lock;
	struct notifier_block pm_notify;
};

int pm_om_fwrite_trigger(void);
int pm_om_platform_init(void);

#ifdef __cplusplus
}
#endif

#endif  /* __PM_OM_PLATFORM_H__ */
