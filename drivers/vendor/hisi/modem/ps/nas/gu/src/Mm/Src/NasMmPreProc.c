

/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "MM_Ext.h"
#include "NasMmlCtx.h"







#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_NAS_MM_PREPORC_C

/*****************************************************************************
   2 全局变量定义
*****************************************************************************/


/*****************************************************************************
   3 函数实现
*****************************************************************************/

VOS_VOID NAS_MM_ProcMmcMmSysInfoInd_CsNotSupport_PreProc( VOS_VOID )
{
    /* 系统消息中指示CS不支持，MM在idle态下状态迁移到No cell available */
    if ((MM_IDLE_NORMAL_SERVICE         == g_MmGlobalInfo.ucState)
     || (MM_IDLE_ATTEMPTING_TO_UPDATE   == g_MmGlobalInfo.ucState)
     || (MM_IDLE_LIMITED_SERVICE        == g_MmGlobalInfo.ucState)
     || (MM_IDLE_PLMN_SEARCH            == g_MmGlobalInfo.ucState)
     || (MM_IDLE_LOCATION_UPDATE_NEEDED == g_MmGlobalInfo.ucState))
    {
        g_MmGlobalInfo.ucMmServiceState = MM_NO_SERVICE;
        Mm_ComSetMmState(MM_IDLE_NO_CELL_AVAILABLE);
    }

    /* 回复MMCMM_ATTACH_CNF的操作,清除等待ATTACH结果标志位 */
    if (MM_WAIT_CS_ATTACH == (g_MmGlobalInfo.stAttachInfo.enAttachType & MM_WAIT_CS_ATTACH))
    {
        NAS_MM_SndMmcAttachCnf();
    }

    g_MmGlobalInfo.stAttachInfo.enAttachType = MM_WAIT_NULL_ATTACH;

    NAS_MM_SndMmcCsRegResultInd(MM_MMC_LU_PROC_FALSE,
                                MM_MMC_LU_RESULT_FAILURE,
                                NAS_MML_REG_FAIL_CAUSE_SERVING_CELL_DOMAIN_NOT_SUPPORT);

    NAS_MM_SndGmmLuResultInd(MMGMM_LU_FAILURE,
                             NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE);


    return;
}


VOS_UINT8 NAS_MM_IsNeedDiscardMmcMmSysInfoInd( VOS_VOID )
{
    /* MM在TC状态不处理来自MMC的系统消息。
       GMM中在Gmm_MsgDistribute_GMM_TC_ACTIVE 中已做了限制 */
    if (TEST_CONTROL_ACTIVE == g_MmGlobalInfo.ucState)
    {
        return VOS_TRUE;
    }

    /* 通话和等待建链过程，不处理系统消息，转发后丢弃，等待新的系统消息 */
    if (NAS_MML_NET_RAT_TYPE_GSM == NAS_MML_GetCurrNetRatType())
    {
        if ((WAIT_FOR_OUTGOING_MM_CONNECTION            == g_MmGlobalInfo.ucState)
         || (WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION == g_MmGlobalInfo.ucState)
         || (MM_CONNECTION_ACTIVE                       == g_MmGlobalInfo.ucState))
        {
            if ( MM_TIMER_RUNNING == NAS_MM_QryTimerStatus(MM_TIMER_CS_HO_WAIT_SYSINFO) )
            {
                Mm_TimerStop(MM_TIMER_CS_HO_WAIT_SYSINFO);

                /* 通知MMC HO SUCC */
                NAS_MM_SndMmcCsRegResultInd(MM_MMC_LU_PROC_FALSE, MM_MMC_LU_RESULT_SUCCESS, NAS_MML_REG_FAIL_CAUSE_HO_SUCC);
            }

            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}



VOS_VOID NAS_MM_ProcMmcMmSysInfoInd_NoImsi_PreProc( VOS_VOID )
{
    g_MmGlobalInfo.ucNtMod = g_MmGlobalInfo.ucNewNtMod;

    NAS_MM_SndMmcCsRegResultInd(MM_MMC_LU_PROC_FALSE,
                                MM_MMC_LU_RESULT_FAILURE,
                                NAS_MML_REG_FAIL_CAUSE_SIM_INVALID);

    NAS_MM_SndGmmLuResultInd(MMGMM_LU_FAILURE,
                             NAS_MML_REG_FAIL_CAUSE_ILLEGAL_MS);

    /* 非空闲态,不处理CS卡无效 */
    if (VOS_TRUE == NAS_MM_IsIdleState())
    {
        g_MmGlobalInfo.ucMmServiceState = MM_NO_IMSI;
        g_MmGlobalInfo.ucPreState       = g_MmGlobalInfo.ucState;
        Mm_ComSetMmState(MM_IDLE_NO_IMSI);
    }

    Mm_ComCheckDelayMmConn(MM_FALSE);

    return;
}
VOS_UINT8   NAS_MM_RcvMmcMmSysInfoInd_PreProc(
    VOS_VOID                           *pRcvMsg
)
{
    VOS_UINT8                           ucEventID;

    ucEventID       = 5;

    NAS_MM_LogMmCtxInfo();

    /* 停MM_TIMER_HO_WAIT_SYSINFO或MM_TIMER_EMERGENCY_CSFB_HO_WAIT_SYSINFO的处理与原来相同:
       只有在CSFB标志存在时，才停定时器. 如果需要做LAU，等待LAU结果;如果不需要做LAU, MM报注册假流程
       其他情况下不停定时器，等定时器超时 */
    if ( MM_TIMER_RUNNING == NAS_MM_QryTimerStatus(MM_TIMER_CS_HO_WAIT_SYSINFO) )
    {
        Mm_TimerStop(MM_TIMER_CS_HO_WAIT_SYSINFO);

        /* 通知MMC HO SUCC */
        NAS_MM_SndMmcCsRegResultInd(MM_MMC_LU_PROC_FALSE, MM_MMC_LU_RESULT_SUCCESS, NAS_MML_REG_FAIL_CAUSE_HO_SUCC);
    }

    /* 是否需要忽略系统消息 */
    if (VOS_TRUE == NAS_MM_IsNeedDiscardMmcMmSysInfoInd())
    {
        return MM_EVENT_ID_INVALID;
    }

    /* 检查系统消息的合法性 */
    if (VOS_FALSE == NAS_MM_CheckMmcMmSysInfoIE(pRcvMsg))
    {
        return MM_EVENT_ID_INVALID;
    }

    if (NAS_MML_NET_RAT_TYPE_GSM == NAS_MML_GetCurrNetRatType())
    {
        NAS_MM_UpdateNetworkInfo_GasSysInfo(pRcvMsg);
    }
    else
    {
        NAS_MM_UpdateNetworkInfo_WasSysInfo(pRcvMsg);
    }

    if (VOS_TRUE == NAS_MM_IsCsNotSupport(pRcvMsg))
    {
        NAS_MM_ProcMmcMmSysInfoInd_CsNotSupport_PreProc();

        return MM_EVENT_ID_INVALID;
    }

    if (VOS_FALSE == NAS_MML_GetSimCsRegStatus())
    {
        NAS_MM_ProcMmcMmSysInfoInd_NoImsi_PreProc();

        return MM_EVENT_ID_INVALID;
    }


    return ucEventID;
}
VOS_UINT8 NAS_MM_RcvMmcCoverageLostInd_PreProc(
    VOS_VOID                           *pRcvMsg
)
{
    VOS_UINT8                           ucEventID;

    ucEventID       = 21;

    /* 停MM_TIMER_NORMAL_CSFB_HO_WAIT_SYSINFO或MM_TIMER_EMERGENCY_CSFB_HO_WAIT_SYSINFO的处理与原来相同:
       只有在CSFB标志存在时，才停定时器. 如果需要做LAU，等待LAU结果;如果不需要做LAU, MM报注册假流程
       其他情况下不停定时器，等定时器超时 */
    if ( MM_TIMER_RUNNING == NAS_MM_QryTimerStatus(MM_TIMER_CS_HO_WAIT_SYSINFO) )
    {
        Mm_TimerStop(MM_TIMER_CS_HO_WAIT_SYSINFO);
    }

    return ucEventID;
}


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

