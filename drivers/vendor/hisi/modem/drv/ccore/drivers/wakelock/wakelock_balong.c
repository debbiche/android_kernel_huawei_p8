
#include <osl_thread.h>
#include <bsp_wakelock.h>
#include <bsp_lowpower_mntn.h>
#include <bsp_pm_om.h>
#include "wakelock.h"
/*lint --e{525,830}*/

/*此全局变量用于记录投票情况，每bit代表一票*/
static unsigned int has_wakelock = 0;



enum lock_enum{
	WAKE_UNLOCK,
	WAKE_LOCK,
};

struct wakelock_om_s{
	u32 stamp;
	u32 lock_id;
	u32 lock_type;
	u32 lock_state;
	int cur_task_id;
}wakelock_dump_log={0,0,0,0,0};

struct wakelock_om_s *record=&wakelock_dump_log;

void bsp_wakelock_init(void){
	void* dump_base_addr = NULL;
	dump_base_addr=bsp_pm_dump_get(PM_OM_WAKE,sizeof(struct wakelock_om_s));
	if(dump_base_addr){
		record = (struct wakelock_om_s*)dump_base_addr;
	}
}
void wake_lock_init(struct wake_lock *lock, int lock_id, const char *name)
{
    if (!lock){
        vote_printf("This wakelock is NULL !! \n");
        return;
    }
    if((lock_id < 0 )||(lock_id >= LOCK_ID_BOTTOM))
    {
        vote_printf("wakelock lock_id define error,wake_lock_init is failure!! \n");
        return;
    }
    if (name)
            lock->name = name;
    lock->lockid = (unsigned int)lock_id;
    return;
}
static inline void wakelock_dump_log_record(enum lock_enum type,struct wake_lock *lock){
     record->stamp= bsp_get_slice_value();
     record->lock_id = lock->lockid;
     record->lock_type = (u32)type;
     record->cur_task_id= osl_task_self();
     record->lock_state = has_wakelock;
     (void)bsp_pm_log_type(PM_OM_WAKE,0,sizeof(struct wakelock_om_s),(void*)record);
}
void wake_lock(struct wake_lock *lock)
{
    unsigned long flags = 0;
    int lockid = 0;
    if (!lock){
        vote_printf("This wakelock is NULL !! \n");
        return;
   }
    lockid = (int)lock->lockid;
    if((lockid < 0 )||(lockid >= LOCK_ID_BOTTOM))
    {
        vote_printf("wakelock lock_id define error,wake_lock is failure!! \n");
        return;
    }
    local_irq_save(flags);
    lockid = (int)lock->lockid;
   if (has_wakelock & ((unsigned int)0x1 << lockid)){
        //vote_printf("LOCK ID: %s ,has been locked!! \n", lock->name);
   }else{
        has_wakelock |= (unsigned int)((unsigned int)0x1 << lockid);
        *((u32 *)g_lowpower_shared_addr)= has_wakelock;
   }
    wakelock_dump_log_record(WAKE_LOCK,lock);
    local_irq_restore(flags);
    return;
}
void wake_unlock(struct wake_lock *lock)
{
    unsigned long flags = 0;
    int lockid = 0;
    if (!lock){
        vote_printf("This wakelock is NULL !! \n");
        return;
   }
    lockid = (int)lock->lockid;
    if((lockid < 0 )||(lockid >= LOCK_ID_BOTTOM))
    {
        vote_printf("wakelock lock_id define error,wake_unlock is failure!! \n");
        return;
    }
    local_irq_save(flags);
    lockid = (int)lock->lockid;
   if (has_wakelock & ((unsigned int)0x1 << lockid)){
        has_wakelock &= (unsigned int)(~((unsigned int)0x1 << lockid));
        *((u32 *)g_lowpower_shared_addr)= has_wakelock;
   }else{
        //vote_printf("LOCK ID: %s, has been unlocked!! \n", lock->name);
   }
    wakelock_dump_log_record(WAKE_UNLOCK,lock);
    local_irq_restore(flags);
    return;
}
int wake_lock_active(struct wake_lock *lock)
{
    unsigned int lockid = 0;
    if (!lock){
        vote_printf("This wakelock is NULL !! \n");
        return -1;
    }
    lockid = lock->lockid;
    if (has_wakelock & ((unsigned int)0x1 << lockid)){
        return 1;
    }else{
        return 0;
    }
}
unsigned int  has_wake_lock(int type)
{
    return has_wakelock;
}

/*
*此函数用于产品线STANDYBY模式下清理所有投票
*其他场景请勿引用
*/
void all_vote_wake_unlock(void)
{
    has_wakelock = 0;
    return;
}
/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_StandbyStateCcpu/BSP_PWRCTRL_StandbyStateAcpu
 功能描述  : AT^PSTANDBY
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
 unsigned int BSP_PWRCTRL_StandbyStateAcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime)
 {
	return 0;
 }
 unsigned int BSP_PWRCTRL_StandbyStateCcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime)
 {
	return 0;
 }

/**************************************************************************
*此处接口用于wakelock LLT，请勿使用
***************************************************************************/
#define LOCK_TEST_OK        0
#define LOCK_TEST_ERROR  1

static struct wake_lock llt_lock;
static struct wake_lock llt_lock1;
static struct wake_lock *llt_lock2 = NULL;

int wake_lock_init_case1(void)
{
    wake_lock_init(&llt_lock, PWRCTRL_SLEEP_NAS, "test_wakelock");
    wake_lock_init(&llt_lock1, PWRCTRL_SLEEP_NAS, "test_wakelock1");
    wake_lock_init(llt_lock2, PWRCTRL_SLEEP_NAS, "test_wakelock2");
    debug_wake_lock(PWRCTRL_SLEEP_NAS);
    debug_wake_unlock(PWRCTRL_SLEEP_NAS);
    debug_wakelock();
    return LOCK_TEST_OK;
}
int wake_lock_case1(void)
{
    int ret = 0;
    llt_lock1.lockid = 33;
    wake_lock(&llt_lock1);
    ret = wake_lock_active (&llt_lock1);/* [false alarm]:误报 */
    wake_lock(llt_lock2);
    ret = wake_lock_active (llt_lock2);/* [false alarm]:误报 */
    wake_lock(&llt_lock);
    wake_lock(&llt_lock);

    ret = (int)has_wake_lock(0);/* [false alarm]:误报 */
    ret = wake_lock_active (&llt_lock);
    if (ret)
        return LOCK_TEST_OK;
    else
        return LOCK_TEST_ERROR;
}

int wake_unlock_case1(void)
{
    int ret = 0;

    llt_lock1.lockid = 33;
    wake_unlock(&llt_lock1);
    ret = wake_lock_active (&llt_lock1);/* [false alarm]:误报 */

    wake_unlock(llt_lock2);
    wake_unlock(&llt_lock);
    wake_unlock(&llt_lock);
    ret = wake_lock_active (&llt_lock);
    if (!ret)
        return LOCK_TEST_OK;
    else
        return LOCK_TEST_ERROR;
}

/**************************************************************************
*此处接口用于调试，请勿使用
***************************************************************************/
void debug_wake_lock(PWC_CLIENT_ID_E id)
{
    struct wake_lock lock;
    lock.lockid = id;
    wake_lock(&lock);
}
void debug_wake_unlock(PWC_CLIENT_ID_E id)
{
    struct wake_lock lock;
    lock.lockid = id;
    wake_unlock(&lock);
}

void debug_wakelock(void)
{
   unsigned int i = 0;
   unsigned int lockstat[32] = {0};
   unsigned int lockmap = has_wakelock;
   for (i = 0; i < LOCK_ID_BOTTOM ; i++){
       if (lockmap & ((unsigned int)0x1 << i)){
           lockstat[i] = 1;
       }
   }

   /*
   *组件需要在此添加打印信息，用于查询所有组件的投票情况
   *请保证此打印顺序于drv_wakelock.h中的一致
   */
   vote_printf("All module vote status(1: oppose sleeping, 0: approve sleeping)\n");
   vote_printf("PWRCTRL_SLEEP_LT_PS:    status: %d   LOCK_ID : %d\n", lockstat[0], 0);
   vote_printf("PWRCTRL_SLEEP_PS_G0:    status: %d   LOCK_ID : %d\n", lockstat[1], 1);
   vote_printf("PWRCTRL_SLEEP_PS_W0:    status: %d   LOCK_ID : %d\n", lockstat[2], 2);
   vote_printf("PWRCTRL_SLEEP_PS_G1:    status: %d   LOCK_ID : %d\n", lockstat[3], 3);
   vote_printf("PWRCTRL_SLEEP_PS_W1:    status: %d   LOCK_ID : %d\n", lockstat[4], 4);
   vote_printf("PWRCTRL_SLEEP_FTM:      status: %d   LOCK_ID : %d\n", lockstat[5], 5);
   vote_printf("PWRCTRL_SLEEP_FTM_1:    status: %d   LOCK_ID : %d\n", lockstat[6], 6);
   vote_printf("PWRCTRL_SLEEP_NAS:      status: %d   LOCK_ID : %d\n", lockstat[7], 7);
   vote_printf("PWRCTRL_SLEEP_NAS_1:    status: %d   LOCK_ID : %d\n", lockstat[8], 8);
   vote_printf("PWRCTRL_SLEEP_OAM:      status: %d   LOCK_ID : %d\n", lockstat[9], 9);
   vote_printf("PWRCTRL_SLEEP_SCI0:     status: %d   LOCK_ID : %d\n", lockstat[10], 10);
   vote_printf("PWRCTRL_SLEEP_SCI1:     status: %d   LOCK_ID : %d\n", lockstat[11], 11);
   vote_printf("PWRCTRL_SLEEP_DSFLOW:   status: %d   LOCK_ID : %d\n", lockstat[12], 12);
   vote_printf("PWRCTRL_SLEEP_PM:       status: %d   LOCK_ID : %d\n", lockstat[13], 13);
   vote_printf("PWRCTRL_SLEEP_UART0:    status: %d   LOCK_ID : %d\n", lockstat[14], 14);
   vote_printf("wake lock:         debug_wake_lock(LOCK_ID)\n");
   vote_printf("wake unlock:       debug_wake_unlock(LOCK_ID)\n");
}

