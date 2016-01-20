#ifndef _BSP_M3PM_M3DPM_LOG_H_
#define _BSP_M3PM_M3DPM_LOG_H_

enum m3_mdm_dpm_log_mode
{
    m3_mdm_dpm_log_begin,
    m3_mdm_dpm_log_temp,
    m3_mdm_dpm_log_ipf,
    m3_mdm_dpm_log_butt
};

enum m3_mdm_pm_log_mode
{
    m3_mdm_pm_log_a9,
    m3_mdm_pm_log_a9_sr,
    m3_mdm_dpm_log_fail
};


#define M3PM_LOG_MODEM_DOWN_OFFSET      0
#define M3PM_LOG_MODEM_DOWN_SIZE        0x8

#define M3PM_LOG_MODEM_UP_OFFSET        (M3PM_LOG_MODEM_DOWN_OFFSET+M3PM_LOG_MODEM_DOWN_SIZE)
#define M3PM_LOG_MODEM_UP_SIZE          (0xC)

#define M3PM_LOG_MODEM_SUSPEND_OFFSET   (M3PM_LOG_MODEM_UP_OFFSET+M3PM_LOG_MODEM_UP_SIZE)
#define M3PM_LOG_MODEM_SUSPEND_SIZE     (0x10)

#define M3PM_LOG_MODEM_RESUME_OFFSET    (M3PM_LOG_MODEM_SUSPEND_OFFSET+M3PM_LOG_MODEM_SUSPEND_SIZE)
#define M3PM_LOG_MODEM_RESUME_SIZE      (0x8)

#define M3PM_LOG_MODEM_RESUME_OK_OFFSET (M3PM_LOG_MODEM_RESUME_OFFSET+M3PM_LOG_MODEM_RESUME_SIZE)
#define M3PM_LOG_MODEM_RESUME_OK_SIZE   (0xC)

#define M3PM_LOG_MODEM_DPM_OFFSET       (M3PM_LOG_MODEM_RESUME_OK_OFFSET+M3PM_LOG_MODEM_RESUME_OK_SIZE)
#define M3PM_LOG_MODEM_DPM_SIZE         (0x4*m3_mdm_dpm_log_butt) 

#define M3PM_LOG_MDM_A9_SIZE            (M3PM_LOG_MODEM_DOWN_SIZE+M3PM_LOG_MODEM_UP_SIZE)
#define M3PM_LOG_MDM_SR_SIZE            (M3PM_LOG_MODEM_SUSPEND_SIZE+M3PM_LOG_MODEM_RESUME_SIZE+M3PM_LOG_MODEM_RESUME_OK_SIZE)
#define M3PM_LOG_MDM_A9_SR_SIZE         (M3PM_LOG_MDM_A9_SIZE+M3PM_LOG_MDM_SR_SIZE)


#ifdef CONFIG_PM_OM
void m3_mdm_sr_update(void);
void m3_mdm_pm_dpm_log(void);

#else
static inline void m3_mdm_sr_update(void){return;}
static inline void m3_mdm_pm_dpm_log(void){return;}

#endif

#endif

