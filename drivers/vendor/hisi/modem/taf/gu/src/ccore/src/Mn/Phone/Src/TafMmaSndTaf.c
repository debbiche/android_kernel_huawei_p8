




/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "PsTypeDef.h"
#include "MnComm.h"
#include "TafSdcCtx.h"
#include "TafLog.h"
#include "TafMmaSndTaf.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MMA_SND_TAF_C


/*****************************************************************************
   2 全局变量定义和宏
****************************************************************************/


/*****************************************************************************
   3 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : TAF_MMA_SndPowerIndMsg
 功能描述  : 通知taf模块关机消息
 输入参数  : VOS_UINT32  ulUsimSta
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月27日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID TAF_MMA_SndPowerIndMsg(VOS_VOID)
{
    TAF_MMA_POWER_OFF_IND_STRU           *pstSndMsg;
    VOS_UINT32                          ulRst;

    pstSndMsg = VOS_NULL_PTR;

    pstSndMsg = (TAF_MMA_POWER_OFF_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(TAF_MMA_POWER_OFF_IND_STRU));
    if( VOS_NULL_PTR == pstSndMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndPowerIndMsg:Alloc Msg Failed");
        return;
    }

    pstSndMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstSndMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMA;
    pstSndMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstSndMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_TAF;

    pstSndMsg->MsgHeader.ulMsgName         = MMA_TAF_POWER_OFF_IND;

    PS_MEM_SET(pstSndMsg->aucReserve, 0x00, sizeof(pstSndMsg->aucReserve));

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);
    if( VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndPowerIndMsg:Send Msg Failed");
        return;
    }

    return;
}


VOS_VOID TAF_MMA_SndSpmServiceStatusChangeNotify(VOS_VOID)
{
    VOS_UINT32                                              ulRet;
    MMA_TAF_SERVICE_STATUS_CHANGE_NOTIFY_STRU              *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_TAF_SERVICE_STATUS_CHANGE_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_TAF_SERVICE_STATUS_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndSpmServiceStatusChangeNotify:ERROR: Memory Alloc Error for pstMsg");
        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_TAF_SERVICE_STATUS_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = WUEPS_PID_TAF;
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->ulLength         = sizeof(MMA_TAF_SERVICE_STATUS_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgName        = ID_MMA_TAF_SERVICE_STATUS_CHANGE_NOTIFY;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndSpmServiceStatusChangeNotify:ERROR:PS_SEND_MSG FAILURE");
        return;
    }

    return;
}


VOS_VOID TAF_MMA_SndSpmNetworkCapabilityChangeNotify(VOS_VOID)
{
    VOS_UINT32                                              ulRet;
    MMA_TAF_NETWORK_CAPABILITY_CHANGE_NOTIFY_STRU          *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_TAF_NETWORK_CAPABILITY_CHANGE_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_TAF_NETWORK_CAPABILITY_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndSpmNetworkCapabilityChangeNotify:ERROR: Memory Alloc Error for pstMsg");
        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_TAF_NETWORK_CAPABILITY_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = WUEPS_PID_TAF;
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->ulLength         = sizeof(MMA_TAF_NETWORK_CAPABILITY_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgName        = ID_MMA_TAF_NETWORK_CAPABILITY_CHANGE_NOTIFY;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndSpmNetworkCapabilityChangeNotify:ERROR:PS_SEND_MSG FAILURE");
        return;
    }

    return;
}


VOS_VOID TAF_MMA_SndSpmRatChangeNotify(VOS_VOID)
{
    VOS_UINT32                                              ulRet;
    MMA_TAF_RAT_CHANGE_NOTIFY_STRU                         *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_TAF_RAT_CHANGE_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_TAF_RAT_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndSpmRatChangeNotify:ERROR: Memory Alloc Error for pstMsg");
        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_TAF_RAT_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = WUEPS_PID_TAF;
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->ulLength         = sizeof(MMA_TAF_RAT_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgName        = ID_MMA_TAF_RAT_CHANGE_NOTIFY;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndSpmRatChangeNotify:ERROR:PS_SEND_MSG FAILURE");
        return;
    }

    return;
}


VOS_UINT32 TAF_MMA_SndMsgCsServiceChangeNotify(
    TAF_CS_SERVICE_ENUM_UINT32          enCsState
)
{
    VOS_UINT32                          ulRet;
    MMA_MSG_CS_SERVICE_IND             *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MSG_CS_SERVICE_IND *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MSG_CS_SERVICE_IND) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMsgCsServiceChangeNotify:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MSG_CS_SERVICE_IND) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = WUEPS_PID_TAF;
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->ulLength         = sizeof(MMA_MSG_CS_SERVICE_IND) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgName        = ID_MMA_MSG_CS_SERVICE_CHANGE_NOTIFY;
    pstMsg->enCSState        = enCsState;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMsgCsServiceChangeNotify:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;

}

/*****************************************************************************
 函 数 名  : MN_PH_SndMsgUsimStatus
 功能描述  : 通知MSG模块当前卡状态
 输入参数  : VOS_UINT32  ulUsimSta
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年3月28日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MN_PH_SndMsgUsimStatus(VOS_UINT32  ulUsimSta)
{
    MNPH_USIM_STATUS_IND_STRU           *pstSndMsg;
    VOS_UINT32                          ulRst;

    pstSndMsg = VOS_NULL_PTR;

    pstSndMsg = (MNPH_USIM_STATUS_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(MNPH_USIM_STATUS_IND_STRU));
    if( VOS_NULL_PTR == pstSndMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"MN_PH_SndMsgUsimStatus:Alloc Msg Failed");
        return;
    }

    pstSndMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstSndMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMA;
    pstSndMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstSndMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_TAF;

    pstSndMsg->MsgHeader.ulMsgName         = MN_USIM_STATUS_IND;

    if ( USIMM_CARD_SERVIC_AVAILABLE == ulUsimSta )
    {
        pstSndMsg->enUsimStatus            = MNPH_USIM_STATUS_AVILABLE;
    }
    else
    {
        pstSndMsg->enUsimStatus            = MNPH_USIM_STATUS_NOT_AVAILABLE;
    }

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);
    if( VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"MN_PH_SndMsgUsimStatus:Send Msg Failed");
        return;
    }

    return;
}

#if (FEATURE_ON == FEATURE_IMS)

VOS_VOID TAF_MMA_SndSpmImsVoiceCapInd(
    VOS_UINT8                           ucAvail
)
{
    VOS_UINT32                                              ulRet;
    MMA_TAF_IMS_VOICE_CAP_IND_STRU                         *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_TAF_IMS_VOICE_CAP_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_TAF_IMS_VOICE_CAP_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndSpmImsVoiceCapInd:ERROR: Memory Alloc Error for pstMsg");
        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_TAF_IMS_VOICE_CAP_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = WUEPS_PID_TAF;
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->ulLength         = sizeof(MMA_TAF_IMS_VOICE_CAP_IND_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgName        = ID_MMA_TAF_IMS_VOICE_CAP_IND;

    pstMsg->ucImsVoiceAvail  = ucAvail;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndSpmImsVoiceCapInd:ERROR:PS_SEND_MSG FAILURE");
        return;
    }

    return;
}
#endif



#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif



