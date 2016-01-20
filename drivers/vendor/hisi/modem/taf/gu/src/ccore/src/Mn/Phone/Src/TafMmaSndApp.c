


/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "PsTypeDef.h"
#include "MnComm.h"
#include "TafSdcCtx.h"
#include "TafLog.h"
#include "TafMmaSndApp.h"
#include "MmaMmcInterface.h"
#include "MmaAppLocal.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MMA_SND_APP_C


/*****************************************************************************
   2 全局变量定义和宏
****************************************************************************/


/*****************************************************************************
   3 函数实现
*****************************************************************************/

VOS_VOID TAF_MMA_SndSysCfgSetCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt,
    TAF_PHONE_ERROR                     usErrorCause
)
{
    TAF_MMA_SYS_CFG_CNF_STRU           *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_SYS_CFG_CNF_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(TAF_MMA_SYS_CFG_CNF_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSysCfgSetCnf:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_SYS_CFG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = pstCtrl->ulModuleId;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_SYS_CFG_SET_CNF;
    pstSndMsg->usClientId               = pstCtrl->usClientId;
    pstSndMsg->ucOpid                   = pstCtrl->ucOpId;
    pstSndMsg->enRslt                   = enRslt;
    pstSndMsg->usErrorCause             = usErrorCause;

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSysCfgSetCnf:Send Msg Failed");
        return;
    }

    return;

}



VOS_VOID TAF_MMA_SndPhoneModeSetCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_PH_MODE                         ucPhMode,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt,
    TAF_PHONE_ERROR                     usErrorCause
)
{
    TAF_MMA_PHONE_MODE_SET_CNF_STRU    *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_PHONE_MODE_SET_CNF_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(TAF_MMA_PHONE_MODE_SET_CNF_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndPhoneModeSetCnf:Alloc Msg Failed");

        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_PHONE_MODE_SET_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = pstCtrl->ulModuleId;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_PHONE_MODE_SET_CNF;
    pstSndMsg->usClientId               = pstCtrl->usClientId;
    pstSndMsg->ucOpid                   = pstCtrl->ucOpId;
    pstSndMsg->ucPhMode                 = ucPhMode;
    pstSndMsg->enRslt                   = enRslt;
    pstSndMsg->usErrorCause             = usErrorCause;

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndPhoneModeSetCnf:Send Msg Failed");

        return;
    }

    return;

}



VOS_VOID TAF_MMA_SndAcqBestNetworkCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt,
    MMC_MMA_ACQ_CNF_STRU               *pstAcqCnfMsg
)
{
    TAF_MMA_ACQ_CNF_STRU               *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_ACQ_CNF_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                         WUEPS_PID_MMA,
                                         sizeof(TAF_MMA_ACQ_CNF_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndAcqBestNetworkCnf:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_ACQ_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = pstCtrl->ulModuleId;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_ACQ_BEST_NETWORK_CNF;
    pstSndMsg->usClientId               = pstCtrl->usClientId;
    pstSndMsg->ucOpid                   = pstCtrl->ucOpId;
    pstSndMsg->enRslt                   = enRslt;
    pstSndMsg->stPlmnId.Mcc             = pstAcqCnfMsg->stPlmnId.ulMcc;
    pstSndMsg->stPlmnId.Mnc             = pstAcqCnfMsg->stPlmnId.ulMnc;
    pstSndMsg->usArfcn                  = pstAcqCnfMsg->usArfcn;

    /* 转换并设置当前的Plmn Priority Class */
    TAF_MMA_ConvertPlmnPriorityClass(pstAcqCnfMsg->enPrioClass, &(pstSndMsg->enPrioClass));

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndAcqBestNetworkCnf:Send Msg Failed");
        return;
    }

    return;

}
VOS_VOID TAF_MMA_SndRegCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt,
    MMC_MMA_REG_CNF_STRU               *pstRegCnfMsg
)
{
    TAF_MMA_REG_CNF_STRU               *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_REG_CNF_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                         WUEPS_PID_MMA,
                                         sizeof(TAF_MMA_REG_CNF_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndRegCnf:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_REG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = pstCtrl->ulModuleId;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_REG_CNF;
    pstSndMsg->usClientId               = pstCtrl->usClientId;
    pstSndMsg->ucOpid                   = pstCtrl->ucOpId;
    pstSndMsg->enRslt                   = enRslt;
    pstSndMsg->stPlmnId.Mcc             = pstRegCnfMsg->stPlmnId.ulMcc;
    pstSndMsg->stPlmnId.Mnc             = pstRegCnfMsg->stPlmnId.ulMnc;
    pstSndMsg->usArfcn                  = pstRegCnfMsg->usArfcn;

    /* 转换并设置当前的Plmn Priority Class */
    TAF_MMA_ConvertPlmnPriorityClass(pstRegCnfMsg->enPrioClass, &(pstSndMsg->enPrioClass));

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndRegCnf:Send Msg Failed");
        return;
    }

    return;

}
VOS_VOID TAF_MMA_SndPowerSaveCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt
)
{
    TAF_MMA_POWER_SAVE_CNF_STRU        *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_POWER_SAVE_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                 WUEPS_PID_MMA,
                                                 sizeof(TAF_MMA_POWER_SAVE_CNF_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndPowerSaveCnf:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_POWER_SAVE_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = pstCtrl->ulModuleId;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_POWER_SAVE_CNF;
    pstSndMsg->usClientId               = pstCtrl->usClientId;
    pstSndMsg->ucOpid                   = pstCtrl->ucOpId;
    pstSndMsg->enRslt                   = enRslt;

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndPowerSaveCnf:Send Msg Failed");
        return;
    }

    return;

}


VOS_VOID TAF_MMA_SndDetachCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enDetachRslt,
    TAF_PHONE_ERROR                     usErrorCause
)
{
    TAF_MMA_DETACH_CNF_STRU            *pstDetachCnf = VOS_NULL_PTR;

    /* 申请消息包TAF_MMA_DETACH_CNF_STRU */
    pstDetachCnf = (TAF_MMA_DETACH_CNF_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           WUEPS_PID_MMA,
                                           sizeof(TAF_MMA_DETACH_CNF_STRU));

    /* 内存申请失败，返回 */
    if (VOS_NULL_PTR == pstDetachCnf)
    {
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstDetachCnf + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_DETACH_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 根据输入参数填充TAF_MMA_DETACH_CNF_STRU */
    pstDetachCnf->ulSenderPid       = WUEPS_PID_MMA;
    pstDetachCnf->ulReceiverPid     = pstCtrl->ulModuleId;
    pstDetachCnf->ulMsgName         = ID_TAF_MMA_DETACH_CNF;

    pstDetachCnf->usClientId        = pstCtrl->usClientId;
    pstDetachCnf->ucOpid            = pstCtrl->ucOpId;
    pstDetachCnf->enRslt            = enDetachRslt;
    pstDetachCnf->usErrorCause      = usErrorCause;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMA, pstDetachCnf))
    {
        return;
    }

    return;

}


#if (FEATURE_ON == FEATURE_CL_INTERWORK)


VOS_VOID TAF_MMA_SndCmmcaServiceInd(VOS_VOID)
{
    TAF_MMA_SERVICE_STATUS_IND_STRU    *pstSndMsg    = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;

    /* 申请内存  */
    pstSndMsg = (TAF_MMA_SERVICE_STATUS_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(TAF_MMA_SERVICE_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndCmmcaServiceInd: Memory allocation is failure");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstSndMsg) + VOS_MSG_HEAD_LENGTH, 0, sizeof(TAF_MMA_SERVICE_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* fill in header information */
    pstSndMsg->ulSenderCpuId      = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid        = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId    = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid      = WUEPS_PID_CMMCA;
    pstSndMsg->ulLength           = sizeof(TAF_MMA_SERVICE_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* fill in message name */
    pstSndMsg->ulMsgName          = ID_TAF_MMA_SERVICE_STATUS_IND;

    /* fill in clientid and opid */
    pstSndMsg->usClientId         = CMMCA_CLIENT_ID;
    pstSndMsg->ucOpid             = 0x0;

    /* fill in message content */
    TAF_MMA_ConvertServiceStatus(TAF_SDC_GetCsServiceStatus(), &(pstSndMsg->CsSrvSta));
    TAF_MMA_ConvertServiceStatus(TAF_SDC_GetPsServiceStatus(), &(pstSndMsg->PsSrvSta));

    pstSndMsg->ucCsSimValid       = TAF_SDC_GetSimCsRegStatus();
    pstSndMsg->ucPsSimValid       = TAF_SDC_GetSimPsRegStatus();

    /* send message */
    ulRet = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);
    if (VOS_OK != ulRet)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndCmmcaServiceInd: Send message is failure");
    }

    return;
}



VOS_VOID TAF_MMA_SndAcqInd(
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt,
    MMC_MMA_ACQ_IND_STRU               *pstAcqIndMsg
)
{
    TAF_MMA_ACQ_IND_STRU               *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_ACQ_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                         WUEPS_PID_MMA,
                                         sizeof(TAF_MMA_ACQ_IND_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndAcqInd:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_ACQ_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = WUEPS_PID_CMMCA;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_ACQ_IND;
    pstSndMsg->usClientId               = CMMCA_CLIENT_ID;
    pstSndMsg->ucOpid                   = 0;
    pstSndMsg->enRslt                   = enRslt;
    pstSndMsg->stPlmnId.Mcc             = pstAcqIndMsg->stPlmnId.ulMcc;
    pstSndMsg->stPlmnId.Mnc             = pstAcqIndMsg->stPlmnId.ulMnc;
    pstSndMsg->usArfcn                  = pstAcqIndMsg->usArfcn;

    /* 转换并设置当前的Plmn Priority Class */
    TAF_MMA_ConvertPlmnPriorityClass(pstAcqIndMsg->enPrioClass, &(pstSndMsg->enPrioClass));

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndAcqInd:Send Msg Failed");
        return;
    }

    return;

}
VOS_VOID TAF_MMA_SndCmmcaSysInfoInd(
    VOS_UINT32                                              ulMcc,
    VOS_UINT32                                              ulMnc,
    VOS_UINT16                                              usArfcn,
    MMC_MMA_PLMN_PRIORITY_CLASS_ENUM_UINT8                  enPrioClass
)
{
    TAF_MMA_SYS_INFO_IND_STRU          *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_SYS_INFO_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(TAF_MMA_SYS_INFO_IND_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSysInfoInd:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = WUEPS_PID_CMMCA;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_SYS_INFO_IND;
    pstSndMsg->usClientId               = CMMCA_CLIENT_ID;
    pstSndMsg->ucOpid                   = 0;
    pstSndMsg->stPlmnId.Mcc             = ulMcc;
    pstSndMsg->stPlmnId.Mnc             = ulMnc;
    pstSndMsg->usArfcn                  = usArfcn;

    /* 转换并设置当前的Plmn Priority Class */
    TAF_MMA_ConvertPlmnPriorityClass(enPrioClass, &(pstSndMsg->enPrioClass));

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSysInfoInd:Send Msg Failed");
        return;
    }

    return;

}
VOS_VOID TAF_MMA_SndSimStatusInd(
    TAF_MMA_USIMM_CARD_TYPE_ENUM_UINT32           enCardType,                    /* 卡类型:SIM、USIM、ROM-SIM  */
    TAF_MMA_CARD_STATUS_ENUM_UINT8                enCardStatus
)
{
    TAF_MMA_SIM_STATUS_IND_STRU        *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_SIM_STATUS_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(TAF_MMA_SIM_STATUS_IND_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSimStatusInd:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_SIM_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = WUEPS_PID_CMMCA;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_SIM_STATUS_IND;
    pstSndMsg->usClientId               = CMMCA_CLIENT_ID;
    pstSndMsg->ucOpid                   = 0;
    pstSndMsg->enCardType               = enCardType;
    pstSndMsg->enCardStatus             = enCardStatus;

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);
    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSimStatusInd:Send Msg Failed");
        return;
    }

    return;

}

#endif
VOS_VOID TAF_MMA_SndAtRssiInd(VOS_VOID)
{
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent;

    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("TAF_MMA_SndAtRssiInd:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    /*填充事件*/
    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));

    TAF_MMA_BuildRssiInd(pstPhoneEvent);

    Taf_PhoneEvent(pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);

    return;
}

/*lint -e429*/

VOS_VOID TAF_MMA_BuildRssiInd(
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent
)
{
    TAF_SDC_APPNETWORK_INFO            *pstAppNwInfo    = VOS_NULL_PTR;
    TAF_SDC_SYS_MODE_ENUM_UINT8         enAppSysMode;

    pstAppNwInfo  = TAF_SDC_GetAppNetworkInfo();
    enAppSysMode  = TAF_SDC_GetAppSysMode();

    /* 初始化OP项 */
    MMA_InitEventInfoOP(pstPhoneEvent);


    pstPhoneEvent->OP_Rssi                          = MMA_TRUE;
    pstPhoneEvent->PhoneEvent                       = TAF_PH_EVT_RSSI_CHANGE_IND;

    /*仅做上报一个小区的相关信息*/
    pstPhoneEvent->RssiValue.ucRssiNum = 1;
    pstPhoneEvent->RssiValue.enRatType              = TAF_SDC_GetAppSysMode();
    pstPhoneEvent->RssiValue.ucCurrentUtranMode     = TAF_SDC_GetAppUtranMode();
    pstPhoneEvent->RssiValue.aRssi[0].ucChannalQual = pstAppNwInfo->stRssiValue.ucChannalQual;



    switch(enAppSysMode)
    {
        case TAF_SDC_SYS_MODE_GSM:
             pstPhoneEvent->RssiValue.aRssi[0].u.stGCellSignInfo.sRssiValue = pstAppNwInfo->stRssiValue.u.st2GCellSignInfo.sRssiValue;

             MMA_GetRssiLevelFromValue(pstAppNwInfo->stRssiValue.u.st2GCellSignInfo.sRssiValue,
                                     &(pstPhoneEvent->RssiValue.aRssi[0].ucRssiValue));

            break;

        case TAF_SDC_SYS_MODE_WCDMA:
             pstPhoneEvent->RssiValue.aRssi[0].u.stWCellSignInfo.sRscpValue = pstAppNwInfo->stRssiValue.u.st3GCellSignInfo.sRscpValue;
             pstPhoneEvent->RssiValue.aRssi[0].u.stWCellSignInfo.sEcioValue = pstAppNwInfo->stRssiValue.u.st3GCellSignInfo.sEcioValue;

             MMA_GetRssiLevelFromValue(pstAppNwInfo->stRssiValue.u.st3GCellSignInfo.sRscpValue,
                                     &(pstPhoneEvent->RssiValue.aRssi[0].ucRssiValue));

            break;

#if (FEATURE_ON == FEATURE_LTE)
        case TAF_SDC_SYS_MODE_LTE:
             pstPhoneEvent->RssiValue.aRssi[0].ucChannalQual                     = TAF_PH_BER_UNKNOWN;
             pstPhoneEvent->RssiValue.aRssi[0].u.stLCellSignInfo.sRssi           = pstAppNwInfo->stRssiValue.u.st4GCellSignInfo.sRssi;
             pstPhoneEvent->RssiValue.aRssi[0].u.stLCellSignInfo.sRsd            = pstAppNwInfo->stRssiValue.u.st4GCellSignInfo.sRsd;
             pstPhoneEvent->RssiValue.aRssi[0].u.stLCellSignInfo.sRsrp           = pstAppNwInfo->stRssiValue.u.st4GCellSignInfo.sRsrp;
             pstPhoneEvent->RssiValue.aRssi[0].u.stLCellSignInfo.sRsrq           = pstAppNwInfo->stRssiValue.u.st4GCellSignInfo.sRsrq;
             pstPhoneEvent->RssiValue.aRssi[0].u.stLCellSignInfo.lSINR           = pstAppNwInfo->stRssiValue.u.st4GCellSignInfo.lSINR;
             pstPhoneEvent->RssiValue.aRssi[0].u.stLCellSignInfo.stCQI.usRI      = pstAppNwInfo->stRssiValue.u.st4GCellSignInfo.stCQI.usRI;
             pstPhoneEvent->RssiValue.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[0] = pstAppNwInfo->stRssiValue.u.st4GCellSignInfo.stCQI.ausCQI[0];
             pstPhoneEvent->RssiValue.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[1] = pstAppNwInfo->stRssiValue.u.st4GCellSignInfo.stCQI.ausCQI[1];

             TAF_MMA_GetRssiLevelFromLteRssi(pstAppNwInfo->stRssiValue.u.st4GCellSignInfo.sRssi,
                                           &(pstPhoneEvent->RssiValue.aRssi[0].ucRssiValue));

            break;
#endif

        default:
            MMA_ERRORLOG("TAF_MMA_BuildRssiInd:RAT is INVALID.");
            break;
    }

    return;
}
/*lint +e429*/

TAF_PH_RAT_TYPE_ENUM_UINT8 TAF_MMA_ConvertMmlRatTypeToTafRatType(NAS_MML_NET_RAT_TYPE_ENUM_UINT8 enMmlRatType)
{
    switch (enMmlRatType)
    {
        case NAS_MML_NET_RAT_TYPE_GSM:
             return TAF_PH_RAT_GSM;

        case NAS_MML_NET_RAT_TYPE_WCDMA:
             return TAF_PH_RAT_WCDMA;

        case NAS_MML_NET_RAT_TYPE_LTE:
             return TAF_PH_RAT_LTE;

        default:
            PS_LOG(WUEPS_PID_MMA, VOS_NULL, PS_PRINT_WARNING, "TAF_MMA_ConvertMmlRatTypeToTafRatType:WARNING: The input in invalid");
            return TAF_PH_RAT_BUTT;
    }
}


VOS_VOID TAF_MMA_SndSrchedPlmnInfoInd(
    MMC_MMA_SRCHED_PLMN_INFO_IND_STRU  *pstSrchedPlmnInfoInd
)
{
    TAF_MMA_SRCHED_PLMN_INFO_IND_STRU  *pstSndMsg      = VOS_NULL_PTR;
    VOS_UINT32                          i;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_SRCHED_PLMN_INFO_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                         WUEPS_PID_MMA,
                                         sizeof(TAF_MMA_SRCHED_PLMN_INFO_IND_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSrchedPlmnInfoInd:Alloc Msg Failed");
        return;
    }
    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_SRCHED_PLMN_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充上报事件 */
    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = WUEPS_PID_AT;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_SRCHED_PLMN_INFO_IND;
    pstSndMsg->usClientId               = MN_GetRealClientId(MMA_CLIENTID_BROADCAST, WUEPS_PID_TAF);
    pstSndMsg->ucOpid                   = MMA_OP_ID_INTERNAL;

    pstSndMsg->enRat = TAF_MMA_ConvertMmlRatTypeToTafRatType(pstSrchedPlmnInfoInd->enRat);

    if (MMC_MMA_MAX_SRCHED_LAI_NUM < pstSrchedPlmnInfoInd->ulLaiNum)
    {
        pstSrchedPlmnInfoInd->ulLaiNum = MMC_MMA_MAX_SRCHED_LAI_NUM;
    }
    pstSndMsg->ulLaiNum = pstSrchedPlmnInfoInd->ulLaiNum;
    if (TAF_MMA_MAX_SRCHED_LAI_NUM < pstSndMsg->ulLaiNum)
    {
        pstSndMsg->ulLaiNum = TAF_MMA_MAX_SRCHED_LAI_NUM;
    }

    for (i = 0; i < pstSndMsg->ulLaiNum; i++)
    {
        pstSndMsg->astLai[i].stPlmnId.Mcc = pstSrchedPlmnInfoInd->astLai[i].stPlmnId.ulMcc;
        pstSndMsg->astLai[i].stPlmnId.Mnc = pstSrchedPlmnInfoInd->astLai[i].stPlmnId.ulMnc;

        /* 将PLMN ID转为BCD格式 */
        MMA_PlmnId2Bcd(&pstSndMsg->astLai[i].stPlmnId);

        pstSndMsg->astLai[i].usLac        = pstSrchedPlmnInfoInd->astLai[i].usLac;
    }

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);
    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSrchedPlmnInfoInd:Send Msg Failed");
    }
}



VOS_VOID TAF_MMA_SndAtRegStatus(
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enCnDomainId,
    MMA_MMC_REG_STATE_ENUM_UINT8        enRegState
)
{
    TAF_PHONE_EVENT_INFO_STRU          *pstPhoneEvent = VOS_NULL_PTR;
    TAF_SDC_PLMN_ID_STRU               *pstPlmnId     = VOS_NULL_PTR;

    pstPlmnId     = TAF_SDC_GetCurrCampPlmnId();
    pstPhoneEvent = (TAF_PHONE_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMA,
                                        sizeof(TAF_PHONE_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstPhoneEvent)
    {
        MMA_ERRORLOG("TAF_MMA_SndAtRegStatus:ERROR:ALLOC MEMORY FAIL.");
        return;
    }

    PS_MEM_SET(pstPhoneEvent, 0, sizeof(TAF_PHONE_EVENT_INFO_STRU));
    MMA_InitEventInfoOP(pstPhoneEvent);
    
    /* 填写CREG/CGREG对应的OP项 */
    if (MMA_MMC_SRVDOMAIN_CS == enCnDomainId)
    {
        pstPhoneEvent->CurRegSta.OP_CsRegState = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_Lac        = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_CellId     = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_ucAct      = MMA_TRUE;
        pstPhoneEvent->CurRegSta.RegState = enRegState;
    }
    else if (MMA_MMC_SRVDOMAIN_PS == enCnDomainId)
    {
        pstPhoneEvent->CurRegSta.OP_PsRegState = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_Lac        = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_Rac        = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_CellId     = MMA_TRUE;
        pstPhoneEvent->CurRegSta.OP_ucAct      = MMA_TRUE;
        pstPhoneEvent->CurRegSta.ucPsRegState = enRegState;
    }
    else
    {
        /* 服务域填的不对，释放已经分配的内存空间，直接返回 */
        PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
        return;
    }

    /* 驻留信息 */
    pstPhoneEvent->CurRegSta.ucRac               = TAF_SDC_GetCurrCampRac();
    pstPhoneEvent->CurRegSta.CellId.ucCellNum    = 1;
    pstPhoneEvent->CurRegSta.CellId.aulCellId[0] = TAF_SDC_GetCurrCampCellId();
    pstPhoneEvent->CurRegSta.usLac               = TAF_SDC_GetCurrCampLac();

    PS_MEM_CPY(&pstPhoneEvent->CurRegSta.Plmn, pstPlmnId, sizeof(pstPhoneEvent->CurRegSta.Plmn));

    pstPhoneEvent->PhoneEvent              = TAF_PH_EVT_SYSTEM_INFO_IND;
    pstPhoneEvent->OP_CurRegSta            = MMA_TRUE;

    /* 将MMC上报的系统子模式转为+CREG/+CGREG命令参数[ACT]中协议规定的接入技术值 */
    NAS_MMA_TransferSysSubMode2ActForAtCmdCreg(TAF_SDC_GetSysSubMode(), &(pstPhoneEvent->CurRegSta.ucAct));

    switch (TAF_SDC_GetCampSysMode())
    {
#if (FEATURE_ON == FEATURE_LTE)
        case TAF_SDC_SYS_MODE_LTE:
            pstPhoneEvent->RatType = TAF_PH_INFO_LTE_RAT;
            break;
#endif

        case TAF_SDC_SYS_MODE_WCDMA:
            pstPhoneEvent->RatType = TAF_PH_INFO_WCDMA_RAT;
            break;
            
        case TAF_SDC_SYS_MODE_GSM:
            pstPhoneEvent->RatType = TAF_PH_INFO_GSM_RAT;
            break;

        default:
            pstPhoneEvent->RatType = TAF_PH_INFO_NONE_RAT;
            break;
    }

    Taf_PhoneEvent(pstPhoneEvent);

    PS_MEM_FREE(WUEPS_PID_MMA, pstPhoneEvent);
    return;
}

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif



