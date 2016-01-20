

#ifndef  __BBP_PMU_H__
#define  __BBP_PMU_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include <linux/types.h>

enum ADPPOWER_OM_TYPE
{
    ADPPOWER_LOG_ERROR,/*流程*/
    ADPPOWER_LOG_ACTUAL/*实际操作了各模块钟电*/
}BBP_LOG_TYPE_E;
enum ADPPOWER_OM_OPS
{
    ADPPOWER_POWERON = 0,
    ADPPOWER_POWEROFF,
    ADPPOWER_BBPCLKON,
    ADPPOWER_BBPCLKOFF,
    ADPPOWER_PLLON,
    ADPPOWER_PLLOFF,
    ADPPOWER_OTHERS
};
enum ADPPOWER_OM_DEV
{
    ADPPOWER_PA = 0,
    ADPPOWER_RFIC,
    ADPPOWER_IRMBBP,
    ADPPOWER_G1BBP,
    ADPPOWER_G2BBP,
    ADPPOWER_TBBP,
    ADPPOWER_WBBP,
    ADPPOWER_TWBBP,
    ADPPOWER_BBPPLL,
    ADPPOWER_BBEPLL,
    ADPPOWER_ABBCH0G,
    ADPPOWER_ABBCH0LWT,
    ADPPOWER_ABBCH1G,
    ADPPOWER_ABBCH1LWT,
    DEV_OTHERS
};
enum ADPPOWER_OM_ERR
{
    ADPPOWER_PARAERR = 0,
    ADPPOWER_OPSERR,
    ADPPOWER_DEVERR,
    APPPERR_OTHERS
};
struct adp_power_errlog
{
    int ops_id;
    unsigned int   info_mode;       /*记录实现开关操作的模*/
    unsigned int   info_module;       /*记录实现开关操作的组件*/
    unsigned int   info_modem;       /*记录实现开关操作的卡(主卡还是副卡)*/
    unsigned int   err_id;           /*错误分支*/
    int   task_id;
};
struct adp_power_actlog
{
    int ops_id;
    int dev_id;
    u32 vote_lock; /*当前投票情况*/
    unsigned int   act_mode;       /*记录实现开关操作的模*/
    unsigned int   act_module;       /*记录实现开关操作的组件*/
    unsigned int   act_modem;       /*记录实现开关操作的卡(主卡还是副卡)*/
    int   task_id;
};

int adppower_msg_parse(unsigned int typeid, char *in_buf, unsigned int len, char *out_buf, unsigned int *out_ptr);

#ifdef __cplusplus
}
#endif

#endif   /* __BBP_PMU_H__ */