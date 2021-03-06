


/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "PsTypeDef.h"
#include "Taf_Aps.h"
#include "MnApsComm.h"
#include "TafLog.h"
#include "TafApsCtx.h"
#include "MnComm.h"
#include "TafApsFsmMainTbl.h"
#include "TafApsSndInternalMsg.h"
#include "TafApsComFunc.h"

#include "TafApsSndSm.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/*lint -e958*/

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_TAF_APSDEALSMMSG_C


/*****************************************************************************
   2 全局变量定义
*****************************************************************************/

/*****************************************************************************
   3 函数实现
*****************************************************************************/


VOS_UINT32 Aps_CheckPdpType(
    VOS_UINT8                           ucPdpId,
    SM_TAF_PDP_ADDR_STRU               *pSmAddr
)
{
    VOS_UINT8                           ucPdpOrgan;
    VOS_UINT8                           ucPdpTypeNum;
    VOS_UINT8                           ucPdpTypeReq;
    VOS_UINT8                           ucPdpTypeAcc;

    /*--------------------------------------------------------------------------
       获取用户发起PDP激活请求的PDP type number
    --------------------------------------------------------------------------*/
    ucPdpTypeReq    = g_PdpEntity[ucPdpId].PdpAddr.ucPdpTypeNum;

    /*--------------------------------------------------------------------------
       获取PDP ACTIVATE ACCEPT消息中的PDP type organisation, PDP type number
       Refer to TS 24.008 section 10.5.6.4
    --------------------------------------------------------------------------*/
    ucPdpOrgan      = pSmAddr->aucPdpAddr[0] & 0x0F;
    ucPdpTypeNum    = pSmAddr->aucPdpAddr[1];

    /*--------------------------------------------------------------------------
       If PDP type organisation is 0(ETSI), PDP type num should be 0x01 for
       PDP-type PPP. All other values are reserved.
       If PDP type organisation is 1(IETF), PDP type num should be 0x21 (IPV4),
       0x57 (IPV6), or 0x8D (IPV4V6). All other values should be interpreted
       as IPV4. Refer to TS 24.008 section 10.5.6.4
    --------------------------------------------------------------------------*/
    switch(ucPdpOrgan)
    {
        case APS_ADDR_ORGAN_ETSI:
            if (APS_PDP_TYPE_NUM_PPP == ucPdpTypeNum)
            {
                ucPdpTypeAcc = APS_ADDR_PPP;
            }
            else
            {
               MN_ERR_LOG("Aps_CheckPdpType: PDP type is not supported!");
               return VOS_ERR;
            }
            break;

        case APS_ADDR_ORGAN_IETF:
#if (FEATURE_ON == FEATURE_IPV6)
            if ( (MN_APS_PDP_TYPE_NUM_IPV6   != ucPdpTypeNum)
              && (MN_APS_PDP_TYPE_NUM_IPV4V6 != ucPdpTypeNum) )
            {
                ucPdpTypeAcc = APS_ADDR_STATIC_IPV4;
            }
            else if (MN_APS_PDP_TYPE_NUM_IPV6 == ucPdpTypeNum)
            {
                ucPdpTypeAcc = MN_APS_ADDR_IPV6;
            }
            else
            {
                ucPdpTypeAcc = MN_APS_ADDR_IPV4V6;
            }
#else
            if (MN_APS_PDP_TYPE_NUM_IPV6 != ucPdpTypeNum)
            {
                ucPdpTypeAcc = APS_ADDR_STATIC_IPV4;
            }
            else
            {
               MN_ERR_LOG("Aps_CheckPdpType: PDP type IPV6 is not supported!");
               return VOS_ERR;
            }
#endif
            break;

        default:
            MN_ERR_LOG("Aps_CheckPdpType: Input Parameter error!");
            return VOS_ERR;
    }

    if (ucPdpTypeAcc != ucPdpTypeReq)
    {
#if (FEATURE_ON == FEATURE_IPV6)
        if ( (APS_ADDR_DYNAMIC_IPV4 == ucPdpTypeReq)
          && (APS_ADDR_STATIC_IPV4 == ucPdpTypeAcc) )
        {
            return VOS_OK;
        }
        else if ( (MN_APS_ADDR_IPV4V6 == ucPdpTypeReq)
               && (APS_ADDR_PPP != ucPdpTypeAcc) )
        {
            return VOS_OK;
        }
#else
        if ( (APS_ADDR_DYNAMIC_IPV4 == ucPdpTypeReq)
          && (APS_ADDR_STATIC_IPV4 == ucPdpTypeAcc) )
        {
            return VOS_OK;
        }
#endif
        else
        {
            MN_ERR_LOG("Aps_CheckPdpType: PDP type not matched!");
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


VOS_VOID Aps_ValidPacketFlowId(VOS_UINT8 *pucPacketFlowId)
{
    if ( ((*pucPacketFlowId > 3) && (*pucPacketFlowId < 8))
      || (*pucPacketFlowId > APS_PACK_FLOW_ID_BIGGEST) )
    {
        MN_WARN_LOG("Aps_ValidPacketFlowId: Invalid PFI has been allocated.");
        *pucPacketFlowId = 0;
    }
}


VOS_UINT32 Aps_PdpActCnfParaCheck(
    SMREG_PDP_ACTIVATE_CNF_STRU            *pSmActCnfMsg,
    APS_ACTCNF_PARA_ST                     *pActCnfPara
)
{
    VOS_UINT8                               ucQosLen;
    VOS_UINT32                              ulReturnValue;
    MMC_APS_RAT_TYPE_ENUM_UINT32            enRatType;



    pActCnfPara->OP_PdpAddr             = APS_FREE;
    pActCnfPara->OP_ProtCfgOpt          = APS_FREE;
    pActCnfPara->OP_Sapi                = APS_FREE;
    pActCnfPara->OP_RadioPriority       = APS_FREE;
    pActCnfPara->OP_PacketFlowId        = APS_FREE;
    pActCnfPara->OP_Spare               = APS_FREE;

    /*NSAPI*/
    if (APS_USED == pSmActCnfMsg->bitOpNsapi)
    {
        /*若返回信息中有ucNsapi*/
        if ( !(APS_JUDGE_NSAPI_VALID(pSmActCnfMsg->ucNsapi)))
        {
            APS_WARN_LOG("Aps_PdpActCnfParaCheck err! ");
            return  APS_PARA_INVALID;
        }
    }
    else
    {
        /*如果没有NSAPI，则丢弃该消息。NSAPI是必须的参数*/
        APS_WARN_LOG("there is no NSAPI. err! ");
        return  APS_PARA_INVALID;
    }
    pActCnfPara->ucNsapi                = pSmActCnfMsg->ucNsapi;

    /*QOS*/
    /*从SM传来的消息中拆包出QOS */
    ulReturnValue = Aps_UnpackSmQos(    &pSmActCnfMsg->stNegoQos,
                                        &ucQosLen,
                                        &pActCnfPara->PdpQos    );
    /*若拆包error*/
    if(APS_ERROR == ulReturnValue)
    {
        APS_WARN_LOG("Aps_PdpActCnfParaCheck err! ");
        return  APS_PARA_INVALID;
    }

    /*拆包成功则进行QOS合法性检查 */
    ulReturnValue = Aps_QosValidOrNot(&pActCnfPara->PdpQos);
    if ( APS_ERROR == ulReturnValue)
    {
        APS_WARN_LOG("Aps_PdpActCnfParaCheck QOS err! ");
        return  APS_PARA_INVALID;
    }

    /*Addr*/
    /*如果返回了ADDR, 则拷贝出来*/
    if ( APS_SELECT_THIS_FIELD == pSmActCnfMsg->bitOpPdpAddr)
    {
        ulReturnValue = Aps_CheckPdpType( pSmActCnfMsg->ucConnectId,
                                          &(pSmActCnfMsg->stPdpAddr) );
        if ( APS_ERROR == ulReturnValue )
        {
            return APS_PDPTYPE_INVALID;
        }

        ulReturnValue = Aps_DetachSmAddrStru(   &(pSmActCnfMsg->stPdpAddr),
                                                &(pActCnfPara->PdpAddr) );
        if ( APS_ERROR == ulReturnValue)
        {
            APS_WARN_LOG("Aps_PdpActCnfParaCheck err! ");
            return  APS_PARA_INVALID;
        }
        pActCnfPara->OP_PdpAddr         = APS_USED;


        /*检查此次收到的地址和目前已经激活的PDP的地址是否相同,如果相同,则
        为异常,可能原因为:本地的某PDP为激活态,而网络侧不存在此PDP.所以需要
        释放此PDP，并上报此PDP被去激活*/
        Aps_CheckAddrWithActPdp(pSmActCnfMsg->ucConnectId,
                                pActCnfPara->PdpAddr.aucIpAddr);

    }


    /*SAPI*/
    if (pSmActCnfMsg->bitOpSapi)
    {
        /* 6.1.3.1.1 Successful PDP context activation initiated by the mobile station*/
        if (!(APS_JUDGE_SAPI_VALID(pSmActCnfMsg->ucSapi)))
        {
            APS_WARN_LOG("Aps_PdpActCnfParaCheck Sapi err! ");
            return  APS_PARA_INVALID;
        }
        else
        {
            pActCnfPara->OP_Sapi        = APS_USED;
            pActCnfPara->ucSapi         = pSmActCnfMsg->ucSapi;
        }
    }

    /*ucRadioPriority*/
    if (pSmActCnfMsg->bitOpRadioPriority)
    {
        if (!(APS_JUDGE_RADIOPRI_VALID(pSmActCnfMsg->ucRadioPriority)))
        {
            APS_NORM_LOG("All other values are interpreted as priority level 4 by this version of the protocol.");
            pActCnfPara->ucRadioPriority = APS_RADIOPRI_4;
            pActCnfPara->OP_RadioPriority= APS_USED;
        }
        else
        {
            pActCnfPara->ucRadioPriority = pSmActCnfMsg->ucRadioPriority;
            pActCnfPara->OP_RadioPriority= APS_USED;
        }
    }


    enRatType = TAF_APS_GetCurrPdpEntityRatType();
    if ( MMC_APS_RAT_TYPE_GSM == enRatType)
    {
        /*PacketFlowId*/
        if ( pSmActCnfMsg->bitOpPacketFlowId )
        {
            pActCnfPara->OP_PacketFlowId    = APS_USED;
            pActCnfPara->ucPacketFlowId     = pSmActCnfMsg->stPacketFlowId.ucPacketFlowId;
            Aps_ValidPacketFlowId(&(pActCnfPara->ucPacketFlowId));
        }
    }

    /*PCO,处理DNS和NBNS */
    if (VOS_TRUE == pSmActCnfMsg->bitOpProtCfgOpt)
    {
        /*拆出DNS*/
        pActCnfPara->OP_ProtCfgOpt  = VOS_TRUE;
    }

    return  APS_PARA_VALID;
}


VOS_VOID    Aps_CheckAddrWithActPdp(    VOS_UINT8           ucPdpId,
                                        VOS_UINT8          *pucIpAddr)
{
    VOS_UINT8                           ucTmpPdpId;
    TAF_APS_STA_ENUM_UINT32             enState;

    for(ucTmpPdpId = 0; ucTmpPdpId < TAF_APS_MAX_PDPID; ucTmpPdpId++)
    {
        if(ucPdpId == ucTmpPdpId)
        {
            continue;
        }

        /*如果某PDP不是未激活，并且PDP类型为IP，并且是主激活,就比较IP地址*/
        enState = TAF_APS_GetPdpIdMainFsmState(ucTmpPdpId);
        if (((TAF_APS_STA_ACTIVE == enState)
            ||(TAF_APS_STA_MS_MODIFYING == enState))
          && (APS_ADDR_STATIC_IPV4 == g_PdpEntity[ucTmpPdpId].PdpAddr.ucPdpTypeNum)
          && (APS_PDP_ACT_PRI == g_PdpEntity[ucTmpPdpId].ActType))
        {
            /* 地址相同，则上报PDP去激活，发送内部去激活指示 */
            if ((pucIpAddr[0] == g_PdpEntity[ucTmpPdpId].PdpAddr.aucIpAddr[0]) &&
                (pucIpAddr[1] == g_PdpEntity[ucTmpPdpId].PdpAddr.aucIpAddr[1]) &&
                (pucIpAddr[2] == g_PdpEntity[ucTmpPdpId].PdpAddr.aucIpAddr[2]) &&
                (pucIpAddr[3] == g_PdpEntity[ucTmpPdpId].PdpAddr.aucIpAddr[3]) )
            {
                /*先上报此PDP被去激活*/
                TAF_APS_SndPdpDeActivateInd(ucTmpPdpId, TAF_PS_CAUSE_SM_NW_REGULAR_DEACTIVATION);

                /* 发送内部去激活指示 */
                TAF_APS_SndInterPdpDeactivateReq(ucTmpPdpId, SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
            }
        }

    }

    return;
}



/*****************************************************************************
 Prototype      : Aps_CheckQosSatisify()
 Description    : 获取用户定义的该PDP可以接受的MINQOS,并比较，判断网络返回的QOS
                  是否满足MINQOS的要求.
 Input          : ucPdpId
                  pNetNegotQos
 Output         :
 Return Value   : 取值: APS_PARA_VALID、APS_PARA_INVALID
 Date Accessed  :
 Date Update    :
 Calls          : Aps_GetPsMinQos()
                  Aps_CompareQos()
 Called By      :
 History        : ---
  1.Date        : 2005-
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32  Aps_CheckQosSatisify (  VOS_UINT8               ucPdpId,
                                    APS_PDP_QOS_STRU*       pNetNegotQos)
{
    VOS_UINT32                      ulRet;
    VOS_UINT8                       ucDefinedOrNot;
    VOS_UINT8                       ucQosOkOrNot;
    APS_PDP_QOS_STRU                MinQos;

    /*获取TAF设定的最小QOS;放在MinQos中 */
    ulRet = Aps_GetPsMinQos( ucPdpId, &ucDefinedOrNot, &MinQos );
    if ( TAF_ERR_ERROR == ulRet)
    {
        APS_WARN_LOG(" Can not get the MIN QOS of APP ! ");

        /*按照QOS满足MIN来处理*/
        return  APS_PARA_VALID;
    }

    if ( TAF_PDP_MIN_QOS_NOT_DEFINED == ucDefinedOrNot)
    {
        return  APS_PARA_VALID;
    }

    ulRet = Aps_CompareQos( pNetNegotQos, &MinQos, &ucQosOkOrNot );
    /*若Comparing the QOS error, 则认为QOS满足*/
    if ( APS_FAIL == ulRet)
    {
        APS_WARN_LOG("Comparing the QOSerror ! ");
        return  APS_PARA_VALID;
    }

    if ( APS_QOS_UP_TO_MIN == ucQosOkOrNot)
    {
        return  APS_PARA_VALID;
    }
    else
    {
        return  APS_PARA_INVALID;
    }
}

/**********************************************************
 Function:       Aps_GetPsMinQos
 Description:    APS模块收到网络的Qos参数后，与用户设置的最小
                 Qos参数进行比较以决定是接受还是拒绝
 Calls:          APS模块
 Data Accessed:
 Data Updated:
 Input:          ucCid
 Output:         pDefinedOrNot - 用户是否设置最小Qos参数
                 pQos - 用户设置的Qos参数
 Return:         TAF-SUCCESS - 成功
                 TAF-FAILURE - 失败
 Others:
**********************************************************/
VOS_UINT32 Aps_GetPsMinQos(
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                          *pDefinedOrNot,
    APS_PDP_QOS_STRU                   *pQos )
{
    VOS_UINT8           ucCid;

    ucCid               = g_PdpEntity[ucPdpId].stClientInfo.ucCid;

    if ( g_TafCidTab[ucCid].ucMinQosFlag )
    {
       *pDefinedOrNot = TAF_PDP_MIN_QOS_DEFINED;
        Taf_FillMinQos( &g_TafCidTab[ucCid], pQos );
        return TAF_SUCCESS;
    }


    /*最小可接受Qos未定义，APS可以认为任何Qos都可接受*/
    *pDefinedOrNot = TAF_PDP_MIN_QOS_NOT_DEFINED;

    return TAF_SUCCESS;
}
VOS_VOID Aps_PdpActCnfQosSatisfy(
    VOS_UINT8                           ucPdpId,
    APS_ACTCNF_PARA_ST                 *pActCnfPara,
    SMREG_PDP_ACTIVATE_CNF_STRU        *pStCnf
)
{
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    APS_INNER_SN_ACT_ST                 ApsInerSnAct;
    SM_TAF_QOS_STRU                     stQos;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatTypeSuspend;

    pstPdpEntity                        = TAF_APS_GetPdpEntInfoAddr(ucPdpId);
    enCurrRatType                       = TAF_APS_GetCurrPdpEntityRatType();
    enCurrRatTypeSuspend                = TAF_APS_GET_RAT_TYPE_IN_SUSPEND();

/*~~~~~~~~~~~~~~~~~~~~~~~向PDP状态机中填SM返回的参数开始~~~~~~~~~~~~~~~~~~~~*/

    /*QOS*/
    PS_MEM_CPY(       &g_PdpEntity[ucPdpId].PdpQos,
                      &pActCnfPara->PdpQos,
                      sizeof(APS_PDP_QOS_STRU) );

    /*PdpAddr*/
    if (VOS_TRUE == pActCnfPara->OP_PdpAddr)
    {
        pstPdpEntity->PdpAddr = pActCnfPara->PdpAddr;
    }

    /*ucNsapi*/
    g_PdpEntity[ucPdpId].PdpNsapiFlag  = APS_USED;
    g_PdpEntity[ucPdpId].ucNsapi       = pActCnfPara->ucNsapi;

    /*SAPI*/
    if ( pActCnfPara->OP_Sapi )
    {
        g_PdpEntity[ucPdpId].GprsPara.Op_Sapi  = APS_USED;
        g_PdpEntity[ucPdpId].GprsPara.ucSapi   = pActCnfPara->ucSapi;
    }

    /*ucRadioPriority*/
    if ( pActCnfPara->OP_RadioPriority )
    {
        g_PdpEntity[ucPdpId].GprsPara.Op_RadioPriority = APS_USED;
        g_PdpEntity[ucPdpId].GprsPara.ucRadioPriority
                                        = pActCnfPara->ucRadioPriority;
    }

    /*DNS*/
    if (pActCnfPara->OP_ProtCfgOpt)
    {
        g_PdpEntity[ucPdpId].PdpDnsFlag = APS_USED;
        g_PdpEntity[ucPdpId].PdpDns     = pActCnfPara->PcoDns;

        /* NBNS (WINS) */
        g_PdpEntity[ucPdpId].bitOpPdpNbns    = APS_USED;
        g_PdpEntity[ucPdpId].stPdpNbns       = pActCnfPara->PcoNbns;

        /* GATE WAY (PEER IP) */
        g_PdpEntity[ucPdpId].bitOpPdpGateWay = APS_USED;
        g_PdpEntity[ucPdpId].stPdpGateWay    = pActCnfPara->stPcoGateWay;
    }

    /*ucPacketFlowId*/
    if ( pStCnf->bitOpPacketFlowId )
    {
        g_PdpEntity[ucPdpId].GprsPara.OP_PacketFlowId = APS_USED;
        g_PdpEntity[ucPdpId].GprsPara.ucPacketFlowId
                                        = pActCnfPara->ucPacketFlowId;
    }

    /*Pco*/
    if (pStCnf->bitOpProtCfgOpt)
    {
        pstPdpEntity->PdpDnsFlag        = VOS_TRUE;

        MN_APS_UnpackPco(pstPdpEntity,
                         &pStCnf->stProtCfgOpt);
    }

#if (FEATURE_ON == FEATURE_IPV6)
    if (VOS_TRUE == pStCnf->bitOpSmCause)
    {
        pstPdpEntity->bitOpCause        = VOS_TRUE;
        pstPdpEntity->enCause           = TAF_APS_MapSmCause(pStCnf->enCause);
    }
#endif

    pstPdpEntity->enEmergencyFlg        = TAF_PDP_NOT_FOR_EMC;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~向PDP状态机中填参数完毕~~~~~~~~~~~~~~~~~~~~~~~~*/

    if ((MMC_APS_RAT_TYPE_WCDMA == enCurrRatType)
     || (MMC_APS_RAT_TYPE_WCDMA == enCurrRatTypeSuspend))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_CNF事件 */
        TAF_APS_SndPdpActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        /*当前是2G*/
        /*向SNDCP发起激活*/
        ApsInerSnAct.ucPdpId            = ucPdpId;
        ApsInerSnAct.pQos               = &pStCnf->stNegoQos;

        /* 激活SNDCP需要使用经过兼容性转换后的参数 */
        Aps_PackSmQos(&(g_PdpEntity[ucPdpId].PdpQos), &stQos);
        ApsInerSnAct.pQos               = &stQos;

        Aps_SmMsgModSnActInd(&ApsInerSnAct, APS_ACT_SN_NORMAL);

        /*填写状态机中的TRANSMODE*/
        g_PdpEntity[ucPdpId].GprsPara.Op_TransMode  = APS_USED;
        if (    (APS_3GPP_QOS_RELIABCLASS_ACKGTPLLCRLC_PRODATA
                   == g_PdpEntity[ucPdpId].PdpQos.ucReliabClass)
             || (APS_3GPP_QOS_RELIABCLASS_UNKGTP_ACKLLCRLC_PRODATA
                   == g_PdpEntity[ucPdpId].PdpQos.ucReliabClass) )
        {
            g_PdpEntity[ucPdpId].GprsPara.TransMode = RABM_ABM_ENUM;
        }
        else
        {
            g_PdpEntity[ucPdpId].GprsPara.TransMode = RABM_ADM_ENUM;
        }

        /*设置已经向SN发送激活消息的标识*/
        g_PdpEntity[ucPdpId].PdpProcTrackFlag               = APS_USED;
        g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot   = APS_USED;
    }

    return;
}

/**********************************************************
 Function:       Aps_GetPsAnswerMode
 Description:    APS模块收到网络发起的PDP激活请求后，需要向
                 TAFM查询当前所设置的应答模式。人工应答模式，
                 则需要向APP发指示并等待APP响应；自动应答模
                 式下，APS需要根据pAnswerType决定是自动发起
                 PDP激活还是自动拒绝网络请求
 Calls:          APS模块
 Data Accessed:
 Data Updated:
 Input:
 Output:         pAnswerMode - 应答模式
                 pAnswerType - 自动模式下接受或者拒绝
 Return:         TAF-SUCCESS - 成功
                 TAF-FAILURE - 失败
 Others:
**********************************************************/
VOS_UINT32  Aps_GetPsAnswerMode(
    TAF_PDP_ANSWER_MODE_ENUM_UINT8                    *pAnswerMode,
    TAF_PDP_ANSWER_TYPE_ENUM_UINT8                    *pAnswerType
)
{
    if ( (VOS_NULL_PTR == pAnswerMode)
      || (VOS_NULL_PTR == pAnswerType) )
    {
        APS_WARN_LOG( "Taf_GetPsAnswerMode:Input pointer is null.\r");
        return TAF_FAILURE;
    }

    *pAnswerMode = g_PsAnsMode.enAnsMode;
    *pAnswerType = g_PsAnsMode.enAnsType;
    return TAF_SUCCESS;
}



VOS_UINT32 Aps_ActIndAutoAns(
    TAF_PDP_ANSWER_TYPE_ENUM_UINT8                     AnswerType,
    VOS_UINT8                           ucPdpId,
    SMREG_PDP_ACTIVATE_IND_STRU        *pStActInd,
    APS_PDP_ACT_REQ_ST                 *pstApsPdpActReq
)
{
    VOS_UINT32                          ulRet;

    pstApsPdpActReq->OP_APN                 = APS_FREE;
    pstApsPdpActReq->OP_Sapi                = APS_FREE;
    pstApsPdpActReq->OP_AUTH                = APS_FREE;
    pstApsPdpActReq->OP_DNS                 = APS_FREE;
    pstApsPdpActReq->OP_NBNS                = APS_FREE;
    pstApsPdpActReq->Op_Xid_DCmp            = APS_FREE;
    pstApsPdpActReq->Op_Xid_PCmp            = APS_FREE;
    pstApsPdpActReq->OP_SPARE               = APS_FREE;
    pstApsPdpActReq->PdpAuth.AuthType       = TAF_PCO_NOT_AUTH;


    if ( TAF_PDP_ANSWER_TYPE_REJECT == AnswerType)
    {
        /*拒绝网络激活指示*/
        APS_WARN_LOG("Aps_ActIndAutoAns :the AnswerType is call reject");
        return APS_FAIL;
    }
    else
    {
        /*接受网络发起的激活 ,获取自动应答接受激活时所需的参数, 注意要把
          PDPTYPE、ADDR和APN信息填入 ApsPdpActReq中, TAFM在选择CID时要查看
          这3种参数
        */
        pstApsPdpActReq->ucPdpId            = ucPdpId;

        /*ADDR信息分解为ADDRTYPE和ADDR两个信息*/
        ulRet   = Aps_DetachSmAddrStru( &pStActInd->stOfferPdpAddr,
                                        &g_PdpEntity[ucPdpId].PdpAddr   );
        if ( APS_ERROR == ulRet)
        {
            APS_WARN_LOG("ERROR! Getting PpdAddr of ACT_IND fail ! Aps_PdpActInd()");
            return APS_FAIL;
        }
        g_PdpEntity[ucPdpId].PdpAddrFlag= APS_USED;

        /*打包激活参数所需的PDP类型和地址*/
        pstApsPdpActReq->ucPdpType          = g_PdpEntity[ucPdpId].PdpAddr.ucPdpTypeNum;

        if ( APS_ADDR_PPP != pstApsPdpActReq->ucPdpType )
        {
            /*如果是IP类型*/
            PS_MEM_CPY( pstApsPdpActReq->aucIpAddr,
                        g_PdpEntity[ucPdpId].PdpAddr.aucIpAddr,
                        TAF_IPV4_ADDR_LEN );
        }

        /*APN*/
        if ( pStActInd->bitOpAcsPointName)
        {
            g_PdpEntity[ucPdpId].PdpApnFlag = APS_USED;
            Aps_DetachSmApn( &pStActInd->stAcsPointName,
                             &g_PdpEntity[ucPdpId].PdpApn    );

            /*打包激活参数所需的APN*/
            pstApsPdpActReq->aucApn[0]      = g_PdpEntity[ucPdpId].PdpApn.ucLength;
            if (pstApsPdpActReq->aucApn[0] > APS_MAX_LENGTH_OF_APN)
            {
               pstApsPdpActReq->aucApn[0]  = APS_MAX_LENGTH_OF_APN;
            }

            /* 若APN长度不等于0, 则有APN, 进行拷贝 */
            if ( 0 != pstApsPdpActReq->aucApn[0])
            {
                pstApsPdpActReq->OP_APN     = APS_USED;
                PS_MEM_CPY(             &(pstApsPdpActReq->aucApn[1]),
                                        g_PdpEntity[ucPdpId].PdpApn.aucValue,
                                        pstApsPdpActReq->aucApn[0]);
            }
        }

        /*获取激活所需参数, TAFM将送来QOS、USERNAME、PASSCODE信息 */
        ulRet = TAF_APS_GetPsReqPdp(pstApsPdpActReq);
        if ( TAF_ERR_ERROR == ulRet )
        {
            APS_WARN_LOG("ERROR! Can not get Parameters for PDP Activation ! Aps_PdpActInd()");
            return APS_FAIL;
        }

        /*SAPI*/
        pstApsPdpActReq->OP_Sapi            = APS_USED;
        pstApsPdpActReq->ucSapi             = APS_SAPI_INIT;

        /*激活方式赋值为APS自动发起激活*/
        g_PdpEntity[ucPdpId].PdpProcTrackFlag            = APS_USED;
        g_PdpEntity[ucPdpId].PdpProcTrack.ucHowToInitAct = APS_TAF_AUTO_INIT_ACT;
    }

    return APS_SUCC;
}
VOS_VOID    Aps_ActIndRejRsp(       VOS_UINT8       ucPdpId )
{
    SMREG_PDP_ACTIVATE_REJ_RSP_STRU    *pstPdpActRejRsp;

    /* 构造消息并填充消息头 */
    pstPdpActRejRsp = (SMREG_PDP_ACTIVATE_REJ_RSP_STRU *)TAF_APS_AllocMsgToSmAndFillMsgHeader(ID_SMREG_PDP_ACTIVATE_REJ_RSP);
    if (VOS_NULL_PTR == pstPdpActRejRsp)
    {
        return;
    }

    /* 填写 ConnectId (CR)，当前CR等同于Pdp Id */
    pstPdpActRejRsp->ucConnectId = ucPdpId;

    /*拒绝原因设为" 资源不够用 " */
    pstPdpActRejRsp->enCause = SM_TAF_CAUSE_SM_NW_INSUFFICIENT_RESOURCES;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_TAF, pstPdpActRejRsp))
    {
        APS_WARN_LOG("Aps_ActIndRejRsp Snd msg fail!");
        return;
    }

    return;
}


VOS_UINT32  TAF_APS_GetPsReqPdp(APS_PDP_ACT_REQ_ST *pReqPdp)
{
    VOS_UINT8 ucIndex;

    for (ucIndex = 1; ucIndex <= TAF_MAX_CID; ucIndex++)
    {   /*取PDP表中第一个有效的PDP参数作为网络发起激活时PDP参数*/
        if (APS_USED == g_TafCidTab[ucIndex].ucUsed)
        {   /*CID表有效*/
            if (TAF_FALSE == Taf_CheckActCid(ucIndex))
            {   /*Cid未被使用*/
                if (TAF_SUCCESS == Taf_ComparePdp(pReqPdp, &g_TafCidTab[ucIndex].CidTab))
                {   /*PDP类型,地址内容，APN内容相同*/
                    if (TAF_SUCCESS == Aps_PackApsActReqPara(ucIndex,
                                                             pReqPdp->ucPdpId,
                                                             pReqPdp))
                    {   /*记录下应答该呼叫的PDP ID信息*/
                        TAF_APS_SetPdpEntClientInfo(pReqPdp->ucPdpId,
                                                    ucIndex,
                                                    TAF_APS_INVALID_MODULEID,
                                                    g_PsAnsMode.usClientId,
                                                    pReqPdp->ucPdpId + APS_MT_DIFF_VALU);

                        return TAF_SUCCESS;
                    }
                }
            }
        }
    }

    /*没有找到匹配的PDP参数表项，使用网络携带的参数，其他参数填写成默认的*/
    pReqPdp->OP_AUTH                = APS_FREE;
    pReqPdp->PdpAuth.AuthType       = TAF_PCO_NOT_AUTH;                    /*不鉴权*/
    PS_MEM_SET(&pReqPdp->PdpAuth.AuthContent, 0, sizeof(pReqPdp->PdpAuth.AuthContent));

    /*Qos填充为网络决定*/
    g_TafCidTab[0].ucQosTabFlag     = APS_FREE;
    Taf_FillQos(&g_TafCidTab[0], &pReqPdp->ReqQos);

    TAF_APS_SetPdpEntClientInfo(pReqPdp->ucPdpId,
                                TAF_APS_DEFAULT_CID,
                                TAF_APS_INVALID_MODULEID,
                                g_PsAnsMode.usClientId,
                                pReqPdp->ucPdpId + APS_MT_DIFF_VALU);

    return TAF_SUCCESS;
}



VOS_UINT32  Taf_ComparePdp(             APS_PDP_ACT_REQ_ST  *pApsPdp,
                                        TAF_PDP_PRIM_CONTEXT_STRU    *pTafCidPdp)
{
    VOS_UINT8                           i;
    VOS_UINT8                           aucTmpApn[TAF_MAX_APN_LEN + 1];
    VOS_UINT8                           aucIpAddr[TAF_IPV4_ADDR_LEN];

    if (pApsPdp->ucPdpType != pTafCidPdp->stPdpAddr.enPdpType)
    {
        return TAF_FAILURE;                                 /*地址类型不相同*/
    }

    /*地址类型相同，判断内容是否相同*/
    if ( APS_ADDR_STATIC_IPV4 == pApsPdp->ucPdpType)
    {   /*地址类型是静态IP，比较IP地址是否相同*/

        /*把CID表中的ADDR转换成4字节的地址*/
        PS_MEM_CPY(aucIpAddr, pTafCidPdp->stPdpAddr.aucIpv4Addr, TAF_IPV4_ADDR_LEN);

        for (i = 0; i < TAF_IPV4_ADDR_LEN; i ++)
        {
            if (aucIpAddr[i] != pApsPdp->aucIpAddr[i] )
            {
                return TAF_FAILURE;                         /*有一个不等，地址就不相同*/
            }
        }
    }

    /*地址类型和地址内容相同，判断APN是否相同*/
    /*先把CID参数形式的APN转换成网络协议形式的APN*/
    Taf_LenStr2Apn(&(pTafCidPdp->stApn), aucTmpApn);

    if (pApsPdp->aucApn[0] != aucTmpApn[0])
    {
        return TAF_FAILURE;                                 /*长度不相同*/
    }
    else
    {
       if (0 != VOS_MemCmp(             pApsPdp->aucApn,
                                        aucTmpApn,
                                        aucTmpApn[0] + 1))
       {
            return TAF_FAILURE;                             /*内容不相同*/
       }
    }
    return TAF_SUCCESS;

}
VOS_UINT32  Aps_PdpMdfCnfParaCheck (    SMREG_PDP_MODIFY_CNF_STRU   *pStMdfCnf,
                                        APS_MDFCNF_PARA_ST *pMdfCnfPara )
{

    VOS_UINT8                           ucQosLen;
    VOS_UINT32                          ulReturnValue;

    pMdfCnfPara->OP_QOS                 = APS_FREE;
    pMdfCnfPara->OP_SAPI                = APS_FREE;
    pMdfCnfPara->OP_RadioPri            = APS_FREE;
    pMdfCnfPara->OP_PFI                 = APS_FREE;
    pMdfCnfPara->OP_SPARE               = APS_FREE;

    /*NSAPI 无用，不检查*/

    /*QOS*/
    /*从SM传来的消息中拆包出QOS */
    if (pStMdfCnf->bitOpNegotiatedQos)
    {
        pMdfCnfPara->OP_QOS                 = APS_USED;
        ulReturnValue = Aps_UnpackSmQos(    &pStMdfCnf->stNegotiatedQos,
                                            &ucQosLen,
                                            &pMdfCnfPara->PdpQos    );
        if ( APS_ERROR == ulReturnValue)
        {   /*若拆包error*/
            APS_WARN_LOG("Aps_PdpMdfCnfParaCheck Aps_UnpackSmQos err! ");
            return  APS_PARA_INVALID;
        }

        /*拆包成功则进行QOS合法性检查 */
        ulReturnValue = Aps_QosValidOrNot(      &pMdfCnfPara->PdpQos    );
        if ( APS_ERROR == ulReturnValue)
        {
            APS_WARN_LOG("Aps_PdpMdfCnfParaCheck QOS err! ");
            return  APS_PARA_INVALID;
        }
    }

    /*SAPI*/
    if (pStMdfCnf->bitOpSapi)
    {
        /*APP发起修改时申请了SAPI，在接受网络修改CNF消息时才判断SAPI*/
        if (g_PdpModifyReqTable[pStMdfCnf->ucConnectId].OP_Sapi)
        {
            if (!(APS_JUDGE_SAPI_VALID(pStMdfCnf->ucSapi)))
            {
                APS_WARN_LOG("Aps_PdpMdfCnfParaCheck Sapi err! ");
                return  APS_PARA_INVALID;
            }
            else
            {
                pMdfCnfPara->OP_SAPI        = APS_USED;
                pMdfCnfPara->ucSapi         = pStMdfCnf->ucSapi;
            }
        }
    }

    /*ucRadioPriority*/
    if (pStMdfCnf->bitOpRadioPriority)
    {
        if (!(APS_JUDGE_RADIOPRI_VALID(pStMdfCnf->ucRadioPriority)))
        {
            APS_NORM_LOG("All other values are interpreted as priority level 4 by this version of the protocol.");
            pMdfCnfPara->OP_RadioPri    = APS_USED;
            pMdfCnfPara->ucRadioPriority= APS_RADIOPRI_4;
        }
        else
        {
            pMdfCnfPara->OP_RadioPri    = APS_USED;
            pMdfCnfPara->ucRadioPriority= pStMdfCnf->ucRadioPriority;
        }
    }

    if ( MMC_APS_RAT_TYPE_GSM == TAF_APS_GetCurrPdpEntityRatType())
    {
        /*PacketFlowId*/
        if ( pStMdfCnf->bitOpPacketFlowId )
        {
            pMdfCnfPara->OP_PFI             = APS_USED;
            pMdfCnfPara->ucPacketFlowId     = pStMdfCnf->stPacketFlowId.ucPacketFlowId;
            Aps_ValidPacketFlowId(&(pMdfCnfPara->ucPacketFlowId));
        }
    }

    return  APS_PARA_VALID;
}
VOS_VOID    Aps_PdpMdfCnfQosSatisfy (   VOS_UINT8          ucPdpId,
                                        APS_MDFCNF_PARA_ST          *pMdfCnfPara,
                                        SMREG_PDP_MODIFY_CNF_STRU   *pMdfCnf)
{
    APS_INNER_SN_MDF_ST                 ApsInerSnMdf;
    SM_TAF_QOS_STRU                     SnMdfQos;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;

    enCurrRatType                       = TAF_APS_GetCurrPdpEntityRatType();


/*~~~~~~~~~~~~~~~~~~~~~~~向PDP状态机中填SM返回的参数开始~~~~~~~~~~~~~~~~~~~~*/
    /*QOS*/
    if ( pMdfCnf->bitOpNegotiatedQos )
    {
        PS_MEM_CPY(       &g_PdpEntity[ucPdpId].PdpQos,
                          &pMdfCnfPara->PdpQos,
                          sizeof(APS_PDP_QOS_STRU) );
    }
    else
    {   /*将g_PdpModifyReqTable中的QOS拷贝到状态机中*/
        if ((APS_USED == g_PdpModifyReqTable[ucPdpId].Op_Qos) &&
            (APS_MDF_HAS_QOS == g_PdpModifyReqTable[ucPdpId].Op_Spare))
        {
            PS_MEM_CPY(   &g_PdpEntity[ucPdpId].PdpQos,
                          &(g_PdpModifyReqTable[ucPdpId].Qos),
                          sizeof(APS_PDP_QOS_STRU)  );

            if (0 == g_PdpEntity[ucPdpId].PdpQos.ucDelayClass)
            {
                g_PdpEntity[ucPdpId].PdpQos.ucDelayClass = APS_3GPP_QOS_DELAYCLASS_4;
            }

            if (0 == g_PdpEntity[ucPdpId].PdpQos.ucReliabClass)
            {
                g_PdpEntity[ucPdpId].PdpQos.ucReliabClass = APS_3GPP_QOS_RELIABCLASS_UNKGTPLLC_ACKRLC_PRODATA;
            }

            if (0 == g_PdpEntity[ucPdpId].PdpQos.ucPrecedenceClass)
            {
                g_PdpEntity[ucPdpId].PdpQos.ucPrecedenceClass = APS_3GPP_QOS_PRECEDENCECLASS_NORMPRI;
            }

            if (0 == g_PdpEntity[ucPdpId].PdpQos.ucPeakThrough)
            {
                g_PdpEntity[ucPdpId].PdpQos.ucPeakThrough = APS_3GPP_QOS_PEAKTHROUGH_1000_OCTPS;
            }

            if (0 == g_PdpEntity[ucPdpId].PdpQos.ucMeanThrough)
            {
                g_PdpEntity[ucPdpId].PdpQos.ucMeanThrough = APS_3GPP_QOS_MEANTHROUGH_BESTEFFORT;
            }


            g_PdpModifyReqTable[ucPdpId].Op_Spare   = APS_MDF_CNF_RENEW_QOS;
        }
        else
        {   /*清标识*/
            g_PdpModifyReqTable[ucPdpId].Op_Spare   = APS_MDF_NO_QOS;
        }
    }

    /*SAPI*/
    if (pMdfCnfPara->OP_SAPI)
    {   /*若返回了SAPI*/
        g_PdpEntity[ucPdpId].GprsPara.ucSapi       = pMdfCnfPara->ucSapi;
    }
    else
    {   /*若没有返回SAPI,则使用发起修改时APP的SAPI更新状态机中的SAPI*/
        if (g_PdpModifyReqTable[ucPdpId].OP_Sapi)
        {
            g_PdpEntity[ucPdpId].GprsPara.ucSapi
                            = g_PdpModifyReqTable[ucPdpId].ucSapi;
        }
    }

    /*ucRadioPriority*/
    g_PdpEntity[ucPdpId].GprsPara.ucRadioPriority
                                        = pMdfCnfPara->ucRadioPriority;

    /*ucPacketFlowId*/
    if ( pMdfCnf->bitOpPacketFlowId )
    {
        g_PdpEntity[ucPdpId].GprsPara.OP_PacketFlowId  = APS_USED;
        g_PdpEntity[ucPdpId].GprsPara.ucPacketFlowId
                                        = pMdfCnfPara->ucPacketFlowId;
    }

    /*TFT*/
    if (g_PdpModifyReqTable[ucPdpId].Op_Tft)
    {
        g_PdpEntity[ucPdpId].PdpTftFlag = APS_USED;
        g_PdpEntity[ucPdpId].PdpTft[0]  = g_PdpModifyReqTable[ucPdpId].Tft;
    }

/*~~~~~~~~~~~~~~~~~~~~~~~~~~向PDP状态机中填参数完毕~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (MMC_APS_RAT_TYPE_WCDMA == enCurrRatType)
    {
        /*上报激活成功 */
        TAF_APS_SndPdpModifyCnf(ucPdpId);
    }
    else
    {   /*当前是2G*/
        /*如果修改了QOS,则向SNDCP发起激活*/
        if ((pMdfCnf->bitOpNegotiatedQos)
            ||(APS_MDF_CNF_RENEW_QOS == g_PdpModifyReqTable[ucPdpId].Op_Spare))
        {
            ApsInerSnMdf.ucPdpId        = ucPdpId;
            if ( pMdfCnf->bitOpNegotiatedQos )
            {   /*网络协商了QOS，则用网络协商的QOS*/
                ApsInerSnMdf.pQos       = &pMdfCnf->stNegotiatedQos;
            }
            else
            {   /*网络没有协商则用UE申请的QOS*/
                Aps_PackSmQos( &g_PdpEntity[ucPdpId].PdpQos, &SnMdfQos );
                ApsInerSnMdf.pQos       = &SnMdfQos;
            }

            Aps_SmMsgModSnMdfInd(&ApsInerSnMdf);

        }
        else
        {
            /*QOS没有变化,则不通知SNDCP*/
            /*上报激活成功 */
            TAF_APS_SndPdpModifyCnf(ucPdpId);
        }
    }

    /*清标识*/
    g_PdpModifyReqTable[ucPdpId].Op_Spare = APS_MDF_NO_QOS;

    return;
}
VOS_UINT32  Aps_PdpMdfIndParaCheck (    SMREG_PDP_MODIFY_IND_STRU     *pStMdfInd,
                                        APS_MDFIND_PARA_ST *pMdfIndPara )
{

    VOS_UINT8                           ucQosLen;
    VOS_UINT32                          ulReturnValue;

    pMdfIndPara->OP_PdpAddr             = APS_FREE;
    pMdfIndPara->OP_Nsapi               = APS_FREE;
    pMdfIndPara->OP_Sapi                = APS_FREE;
    pMdfIndPara->OP_RadioPriority       = APS_FREE;
    pMdfIndPara->OP_PacketFlowId        = APS_FREE;
    pMdfIndPara->OP_Spare               = APS_FREE;

    /*NSAPI 无用，不检查*/

    /*QOS*/
    /*从SM传来的消息中拆包出QOS */
    ulReturnValue = Aps_UnpackSmQos(    &pStMdfInd->stNewQos,
                                        &ucQosLen,
                                        &pMdfIndPara->NewQos    );
    if ( APS_ERROR == ulReturnValue)
    {   /*若拆包error*/
        APS_WARN_LOG("Aps_UnpackSmQos err! ");
        return  APS_PARA_INVALID;
    }

    /*拆包成功则进行QOS合法性检查 */
    ulReturnValue = Aps_QosValidOrNot(&pMdfIndPara->NewQos);
    if ( APS_ERROR == ulReturnValue)
    {
        APS_WARN_LOG("Aps_PdpMdfIndParaCheck QOS err! ");
        return  APS_PARA_INVALID;
    }


    /*SAPI*/
    if (pStMdfInd->bitOpSapi)
    {
        if (!(APS_JUDGE_SAPI_VALID(pStMdfInd->ucSapi)))
        {
            APS_WARN_LOG("Aps_PdpMdfIndParaCheck Sapi err! ");
            return  APS_PARA_INVALID;
        }
        else
        {
            pMdfIndPara->OP_Sapi        = APS_USED;
            pMdfIndPara->ucSapi         = pStMdfInd->ucSapi;
        }
    }

    /*ucRadioPriority*/
    if (pStMdfInd->bitOpRadioPriority)
    {
        if (!(APS_JUDGE_RADIOPRI_VALID(pStMdfInd->ucRadioPriority)))
        {
            APS_NORM_LOG("All other values are interpreted as priority level 4 by this version of the protocol.");
            pMdfIndPara->OP_RadioPriority    = APS_USED;
            pMdfIndPara->ucRadioPriority     = APS_RADIOPRI_4;
        }
        else
        {
            pMdfIndPara->OP_RadioPriority   = APS_USED;
            pMdfIndPara->ucRadioPriority    = pStMdfInd->ucRadioPriority;
        }
    }

    /*Addr*/
    /*如果返回了ADDR, 则拷贝出来*/
    if ( APS_SELECT_THIS_FIELD == pStMdfInd->bitOpPdpAddr)
    {
        ulReturnValue = Aps_DetachSmAddrStru(   &(pStMdfInd->stPdpAddr),
                                                &(pMdfIndPara->PdpAddr) );
        if ( APS_ERROR == ulReturnValue)
        {
            APS_WARN_LOG("Aps_PdpMdfIndParaCheck err! ");
            return  APS_PARA_INVALID;
        }
        pMdfIndPara->OP_PdpAddr         = APS_USED;
    }

    if ( MMC_APS_RAT_TYPE_GSM == TAF_APS_GetCurrPdpEntityRatType())
    {
        /*PacketFlowId*/
        if ( pStMdfInd->bitOpPacketFlowId )
        {
            pMdfIndPara->OP_PacketFlowId    = APS_USED;
            pMdfIndPara->ucPacketFlowId     = pStMdfInd->stPacketFlowId.ucPacketFlowId;
            Aps_ValidPacketFlowId(&(pMdfIndPara->ucPacketFlowId));
        }
    }

    return  APS_PARA_VALID;
}



VOS_VOID Aps_PdpMdfIndQosSatisfy (
    VOS_UINT8                           ucPdpId,
    APS_MDFIND_PARA_ST                 *pMdfIndPara,
    SMREG_PDP_MODIFY_IND_STRU          *pStMdfInd
)
{
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    APS_INNER_SN_MDF_ST                 ApsInerSnMdf;
    SM_TAF_QOS_STRU                     stQos;

    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);


/*~~~~~~~~~~~~~~~~~~~~~~~向PDP状态机中填SM返回的参数开始~~~~~~~~~~~~~~~~~~~~*/
    /*QOS*/
    PS_MEM_CPY(                       &g_PdpEntity[ucPdpId].PdpQos,
                                      &pMdfIndPara->NewQos,
                                      sizeof(APS_PDP_QOS_STRU) );
    /*SAPI*/
    if ( pMdfIndPara->OP_Sapi)
    {
        g_PdpEntity[ucPdpId].GprsPara.ucSapi           = pMdfIndPara->ucSapi;
    }

    /*ucRadioPriority*/
    if ( pMdfIndPara->OP_RadioPriority)
    {
        g_PdpEntity[ucPdpId].GprsPara.ucRadioPriority
                                        = pMdfIndPara->ucRadioPriority;
    }

    /*ucPacketFlowId*/
    if ( pMdfIndPara->OP_PacketFlowId)
    {
        g_PdpEntity[ucPdpId].GprsPara.OP_PacketFlowId  = APS_USED;
        g_PdpEntity[ucPdpId].GprsPara.ucPacketFlowId
                                        = pMdfIndPara->ucPacketFlowId;
    }

    /*PdpAddr*/
    if (VOS_TRUE == pMdfIndPara->OP_PdpAddr)
    {
        pstPdpEntity->PdpAddr = pMdfIndPara->PdpAddr;
    }

/*~~~~~~~~~~~~~~~~~~~~~~~~~~向PDP状态机中填参数完毕~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*向SM发送修改响应*/
    Aps_PdpMdfRsp(ucPdpId);

    if ( MMC_APS_RAT_TYPE_WCDMA == TAF_APS_GetCurrPdpEntityRatType())
    {
        /*上报修改成功 */
        TAF_APS_SndPdpModifyInd(ucPdpId);
    }
    else
    {   /*当前是2G*/
        /*修改了QOS,向SNDCP发起激活*/
        ApsInerSnMdf.ucPdpId            = ucPdpId;
        ApsInerSnMdf.pQos               = &pStMdfInd->stNewQos;

        /* 激活SNDCP需要使用经过兼容性转换后的参数 */
        Aps_PackSmQos(&(g_PdpEntity[ucPdpId].PdpQos), &stQos);
        ApsInerSnMdf.pQos               = &stQos;

        Aps_SmMsgModSnMdfInd(&ApsInerSnMdf);
    }


    return;
}



VOS_VOID    Aps_PdpMdfRsp(              VOS_UINT8           ucPdpId)
{
    SMREG_PDP_MODIFY_RSP_STRU          *pstPdpModRsp;

    /* 构造消息并填充消息头 */
    pstPdpModRsp = (SMREG_PDP_MODIFY_RSP_STRU *)TAF_APS_AllocMsgToSmAndFillMsgHeader(ID_SMREG_PDP_MODIFY_RSP);
    if (VOS_NULL_PTR == pstPdpModRsp)
    {
        return;
    }

    /* 填写 ConnectId (CR)，当前CR等同于Pdp Id */
    pstPdpModRsp->ucConnectId = ucPdpId;

    pstPdpModRsp->bitOpNsapi = APS_USED;

    pstPdpModRsp->bitOpSpare = APS_FREE;

    pstPdpModRsp->ucNsapi    = g_PdpEntity[ucPdpId].ucNsapi;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_TAF, pstPdpModRsp))
    {
        APS_WARN_LOG("Aps_PdpMdfRsp Snd msg fail!");
        return;
    }

    return;
}
VOS_UINT32 Aps_PdpActSecCnfParaCheck(
    SMREG_PDP_ACTIVATE_SEC_CNF_STRU    *pStSecCnf,
    APS_ACTSECCNF_PARA_ST              *pActSecCnfPara
)
{

    VOS_UINT8                           ucQosLen;
    VOS_UINT32                          ulReturnValue;

    pActSecCnfPara->OP_Sapi             = APS_FREE;
    pActSecCnfPara->OP_RadioPriority    = APS_FREE;
    pActSecCnfPara->OP_PacketFlowId     = APS_FREE;
    pActSecCnfPara->OP_Spare            = APS_FREE;

    /*判断PdpId合法性*/
    if (!(APS_JUDEG_PDPID_VALID(pStSecCnf->ucConnectId)))
    {
        APS_WARN_LOG("Aps_PdpActSecCnfParaCheck err! ");
        return  APS_PDPID_INVALID;
    }

    /*NSAPI*/
    if (APS_USED == pStSecCnf->bitOpNsapi)
    {   /*若返回信息中有ucNsapi*/
        if ( !(APS_JUDGE_NSAPI_VALID(pStSecCnf->ucNsapi)))
        {
            APS_WARN_LOG("Aps_PdpActSecCnfParaCheck err! ");
            return  APS_PARA_INVALID;
        }
    }
    else
    {   /*如果没有NSAPI，则丢弃该消息。NSAPI是必须的参数*/
        APS_WARN_LOG("there is no NSAPI. err! ");
        return  APS_PARA_INVALID;
    }
    pActSecCnfPara->ucNsapi             = pStSecCnf->ucNsapi;


    /*QOS*/
    /*从SM传来的消息中拆包出QOS */
    ulReturnValue = Aps_UnpackSmQos(    &pStSecCnf->stNegotiatedQos,
                                        &ucQosLen,
                                        &pActSecCnfPara->PdpQos );
    /*若拆包error*/
    if(APS_ERROR == ulReturnValue)
    {
        APS_WARN_LOG("Aps_PdpActSecCnfParaCheck err! ");
        return  APS_PARA_INVALID;
    }

    /*拆包成功则进行QOS合法性检查 */
    ulReturnValue = Aps_QosValidOrNot(&pActSecCnfPara->PdpQos);
    if ( APS_ERROR == ulReturnValue)
    {
        APS_WARN_LOG("Aps_PdpActSecCnfParaCheck QOS err! ");
        return  APS_PARA_INVALID;
    }


    /*SAPI*/
    if (pStSecCnf->bitOpSapi)
    {
        /* 6.1.3.2.1 Successful Secondary PDP Context Activation Procedure Initiated by the MS */
        if (!(APS_JUDGE_SAPI_VALID(pStSecCnf->ucSapi)))
        {
            APS_WARN_LOG("Aps_PdpActSecCnfParaCheck Sapi err! ");
            return  APS_PARA_INVALID;
        }
        else
        {
            pActSecCnfPara->OP_Sapi     = APS_USED;
            pActSecCnfPara->ucSapi      = pStSecCnf->ucSapi;
        }
    }

    /*ucRadioPriority*/
    if (pStSecCnf->bitOpRadioPriority)
    {
        /* 根据24.008 10.5.7.2 All other values are interpreted as priority level 4
           by this version of the protocol. */
        if (!(APS_JUDGE_RADIOPRI_VALID(pStSecCnf->ucRadioPriority)))
        {
            APS_NORM_LOG("All other values are interpreted as priority level 4 by this version of the protocol.");
            pActSecCnfPara->OP_RadioPriority    = APS_USED;
            pActSecCnfPara->ucRadioPriority     = APS_RADIOPRI_4;
        }
        else
        {
            pActSecCnfPara->OP_RadioPriority    = APS_USED;
            pActSecCnfPara->ucRadioPriority = pStSecCnf->ucRadioPriority;
        }
    }

    if ( MMC_APS_RAT_TYPE_GSM == TAF_APS_GetCurrPdpEntityRatType())
    {
        /*PacketFlowId*/
        if ( pStSecCnf->bitOpPacketFlowId )
        {
            pActSecCnfPara->OP_PacketFlowId = APS_USED;
            pActSecCnfPara->ucPacketFlowId  = pStSecCnf->stPacketFlowId.ucPacketFlowId;
            Aps_ValidPacketFlowId(&(pActSecCnfPara->ucPacketFlowId));
        }
    }

    return  APS_PARA_VALID;
}
VOS_VOID    Aps_PdpActSecCnfQosSatisfy( VOS_UINT8              ucPdpId,
                                        APS_ACTSECCNF_PARA_ST             *pSecCnfPara,
                                        SMREG_PDP_ACTIVATE_SEC_CNF_STRU   *pStSecCnf   )
{

    APS_INNER_SN_ACT_ST                 ApsInerSnAct;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;

    enCurrRatType                       = TAF_APS_GetCurrPdpEntityRatType();

/*~~~~~~~~~~~~~~~~~~~~~~~向PDP状态机中填SM返回的参数开始~~~~~~~~~~~~~~~~~~~~*/

    /*QOS*/
    PS_MEM_CPY(       &g_PdpEntity[ucPdpId].PdpQos,
                      &pSecCnfPara->PdpQos,
                      sizeof(APS_PDP_QOS_STRU) );


    /*ucNsapi*/
    g_PdpEntity[ucPdpId].PdpNsapiFlag   = APS_USED;
    g_PdpEntity[ucPdpId].ucNsapi        = pSecCnfPara->ucNsapi;

    /*SAPI*/
    if ( pStSecCnf->bitOpSapi)
    {
        g_PdpEntity[ucPdpId].GprsPara.Op_Sapi   = APS_USED;
        g_PdpEntity[ucPdpId].GprsPara.ucSapi    = pSecCnfPara->ucSapi;
    }

    /*ucRadioPriority*/
    if ( pStSecCnf->bitOpRadioPriority )
    {
        g_PdpEntity[ucPdpId].GprsPara.Op_RadioPriority  = APS_USED;
        g_PdpEntity[ucPdpId].GprsPara.ucRadioPriority
                                        = pSecCnfPara->ucRadioPriority;
    }

    /*ucPacketFlowId*/
    if ( pStSecCnf->bitOpPacketFlowId )
    {
        g_PdpEntity[ucPdpId].GprsPara.OP_PacketFlowId   = APS_USED;
        g_PdpEntity[ucPdpId].GprsPara.ucPacketFlowId
                                        = pSecCnfPara->ucPacketFlowId;
    }

    g_PdpEntity[ucPdpId].enEmergencyFlg = TAF_PDP_NOT_FOR_EMC;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~向PDP状态机中填参数完毕~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (MMC_APS_RAT_TYPE_WCDMA == enCurrRatType)
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_CNF事件 */
        TAF_APS_SndPdpActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        /*当前是2G*/
        /*向SNDCP发起激活*/
        ApsInerSnAct.ucPdpId            = ucPdpId;
        ApsInerSnAct.pQos               = &pStSecCnf->stNegotiatedQos;

        Aps_SmMsgModSnActInd(&ApsInerSnAct, APS_ACT_SN_NORMAL);

        /*设置已经向SN发送激活消息的标识*/
        g_PdpEntity[ucPdpId].PdpProcTrackFlag               = APS_USED;
        g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot   = APS_USED;
    }

    return;
}

/*****************************************************************************
 Prototype      : Aps_SmMsgModSnActInd()
 Description    : APS收到SM的激活CNF后,将调用此函数向SNDCP发送激活指示消息.
                  该函数将查询PDP状态机以获得激活SNDCP所需的参数.
 Input          : pSnActInd,包括待激活的PDP和网络协商的QOS
                  ucActCause :#define APS_ACT_SN_NORMAL     0
                              #define APS_ACT_SN_SYS_CHNG   1
 Output         : NO
 Return Value   :
 Calls          : Aps_SmMsgModSnActIndParaPack()
                  Aps_SmMsgModGetSnActIndPara()
                  Aps_SmMsgModPackSnMsg()
 Called By      : Aps_PdpActCnf()
 History        : ---
  1.Date        : 2005-
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32  Aps_SmMsgModSnActInd  (     APS_INNER_SN_ACT_ST    *pInnerSnActInd,
                                        VOS_UINT8               ucActCause)
{
    VOS_UINT32                          ulRet;
    APS_SNDCP_ACTIVATE_IND_MSG         *pSnActIndMsg;

    /*申请消息*/
    /* BEGIN 2006-04-06 Modified by liuyang id:48197 for A32D01738*/
    pSnActIndMsg = (APS_SNDCP_ACTIVATE_IND_MSG *)PS_ALLOC_MSG(WUEPS_PID_TAF, sizeof(APS_SNDCP_ACTIVATE_IND_MSG) - VOS_MSG_HEAD_LENGTH);
    /*  END  2006-04-06 Modified by liuyang id:48197 for A32D01738*/
    if ( VOS_NULL_PTR == pSnActIndMsg )
    {
        APS_ERR_LOG("Aps_SmMsgModSnActInd VOS_AllocMsg err ");
        return TAF_FAILURE;
    }
    PS_MEM_SET(pSnActIndMsg,0x00,sizeof(APS_SNDCP_ACTIVATE_IND_MSG));
    /*打包消息头*/
    APS_PACK_SN_MSG_HEADER((*pSnActIndMsg), APS_SNDCP_ACTIVATE_IND_ST)

    /*赋值消息类型*/
    pSnActIndMsg->usMsgType             = APS_SN_ACT_IND_MSG_TYPE;

    /*填消息中的usPId*/
    pSnActIndMsg->usPId
            = (VOS_UINT16)g_PdpEntity[pInnerSnActInd->ucPdpId].ucNsapi;

    /*打包消息体*/
    Aps_SmMsgModSnActIndParaPack (      pInnerSnActInd,
                                        &pSnActIndMsg->ApsSnActInd  );

    /*填写激活SNDCP的原因*/
    pSnActIndMsg->ApsSnActInd.ucActSndcpType  = ucActCause;

    /*向SNDCP发送消息*/
    ulRet = PS_SEND_MSG( WUEPS_PID_TAF, pSnActIndMsg );
    if ( VOS_OK != ulRet )
    {
        APS_WARN_LOG("Send APS_SNDCP_ACTIVATE_IND_MSG ERR ");
        return  APS_FAIL;
    }
    APS_NORM_LOG("APS->SNDCP APS_SNDCP_ACTIVATE_IND_MSG ");

    return APS_SUCC;
}
VOS_VOID    Aps_SmMsgModSnActIndParaPack
                            (   APS_INNER_SN_ACT_ST        *pInnerSnActInd,
                                APS_SNDCP_ACTIVATE_IND_ST  *pSnActIndPara  )
{
    VOS_UINT8                           ucPdpId;

    ucPdpId                             = pInnerSnActInd->ucPdpId;

    pSnActIndPara->ucNsapi              = g_PdpEntity[ucPdpId].ucNsapi;
    pSnActIndPara->ucSapi               = g_PdpEntity[ucPdpId].GprsPara.ucSapi;
    pSnActIndPara->ulTLLI               = GMM_GetTlli();
    pSnActIndPara->ucRadioPriority      = g_PdpEntity[ucPdpId].GprsPara.ucRadioPriority;

    /*拷贝QoS:之所以不使用sizeof(ST_QOS)而使用sizeof(APS_ST_QOS),是因为QOS_STRU的长度为了兼容性的考虑，定为
      了16(R7)，而APS_ST_QOS结构体的长度则需依赖版本而定*/
    PS_MEM_CPY(                       &pSnActIndPara->NegoQos,
                                      pInnerSnActInd->pQos,
                                      sizeof(APS_ST_QOS) );

    #if(PS_UE_REL_VER < PS_PTL_VER_R5)
    if(pSnActIndPara->NegoQos.ulCnt > 11)
    {
        APS_WARN_LOG("Aps_SmMsgModSnActIndParaPack:QOS LEN IS MORE THAN 11");
        pSnActIndPara->NegoQos.ulCnt = 11;
    }
    #endif

    Aps_SetSnXidPara(ucPdpId, &pSnActIndPara->XidParameter);

    return;
}

/*****************************************************************************
 Prototype      : Aps_SetSnXidPara
 Description    :
 Input          : ucPdpId
 Output         : pSnXid
 Return Value   :
 Date Accessed  :
 Date Update    :
 Calls          :
 Called By      :
 History        : ---
  1.Date        : 2005-
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_VOID    Aps_SetSnXidPara(           VOS_UINT8           ucPdpId,
                                        SN_XID_REQ_ST      *pSnXid  )
{
    pSnXid->ucMask                      = 0;

    if (g_PdpEntity[ucPdpId].GprsPara.Op_Xid_PCmp)
    {
        pSnXid->ucMask                 |= SN_XID_PARA_P_CMPR;
        pSnXid->ucPCmprAlgrth           = g_PdpEntity[ucPdpId].GprsPara.ucPCmprAlgrth;
    }

    if (g_PdpEntity[ucPdpId].GprsPara.Op_Xid_DCmp)
    {
        pSnXid->ucMask                 |= SN_XID_PARA_D_CMPR;
        pSnXid->ucDCmprAlgrth           = g_PdpEntity[ucPdpId].GprsPara.ucDCmprAlgrth;
    }
    return;
}

/*****************************************************************************
 Prototype      : Aps_SmMsgModSnMdfInd()
 Description    : APS收到SM的修改CNF后,将调用此函数向SNDCP发送修改指示消息.
                  该函数将查询PDP状态机以获得修改SNDCP所需的参数.
 Input          : pApsInnerSnMdf,指向APS的子模块间使用的修改SNDCP的结构体，
                  该结构体包含两个参数:
                  ucPdpId,等于SM返回的消息中的参数ucCr,用于APS标识各PDP;
                  SM_TAF_QOS_STRU,网络返回的QOS结构体;
 Output         : NO
 Return Value   :
 Calls          : Aps_SmMsgModSnMdfIndParaPack()
                  Aps_SmMsgModPackSnMsg()
 Called By      : Aps_PdpMdfCnf()
 History        : ---
  1.Date        : 2005-
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32  Aps_SmMsgModSnMdfInd  ( APS_INNER_SN_MDF_ST*    pInnerSnMdfInd )
{

    VOS_UINT32                          ulRet;
    APS_SNDCP_MODIFY_IND_MSG           *pSnMdfIndMsg;

    /*申请消息*/
    /* BEGIN 2006-04-06 Modified by liuyang id:48197 for A32D01738*/
    pSnMdfIndMsg = (APS_SNDCP_MODIFY_IND_MSG *)PS_ALLOC_MSG(WUEPS_PID_TAF, (sizeof(APS_SNDCP_MODIFY_IND_MSG)- VOS_MSG_HEAD_LENGTH));
    /*  END  2006-04-06 Modified by liuyang id:48197 for A32D01738*/
    if ( VOS_NULL_PTR == pSnMdfIndMsg )
    {
        APS_ERR_LOG("Aps_SmMsgModSnMdfInd VOS_AllocMsg err ");
        return TAF_FAILURE;
    }

    /*打包消息头*/
    APS_PACK_SN_MSG_HEADER((*pSnMdfIndMsg), APS_SNDCP_MODIFY_IND_ST);

    /*赋值消息类型*/
    pSnMdfIndMsg->usMsgType             = APS_SN_MDF_IND_MSG_TYPE;

    /*填消息中的usPId*/
    pSnMdfIndMsg->usPId
            = (VOS_UINT16)g_PdpEntity[pInnerSnMdfInd->ucPdpId].ucNsapi;

    /*打包消息体*/
    Aps_SmMsgModSnMdfIndParaPack (      pInnerSnMdfInd,
                                        &pSnMdfIndMsg->ApsSnMdfInd  );

    /*向SNDCP发送消息*/
    ulRet = PS_SEND_MSG(WUEPS_PID_TAF, pSnMdfIndMsg );
    if ( VOS_OK != ulRet )
    {
        APS_WARN_LOG("Send APS_SNDCP_ACTIVATE_IND_MSG ERR ");
        return  APS_FAIL;
    }
    APS_NORM_LOG("APS->SNDCP APS_SNDCP_MODIFY_IND_MSG ");

    return APS_SUCC;
}

/*****************************************************************************
 Prototype      : Aps_SmMsgModSnMdfIndParaPack()
 Description    : 构造修改SNDCP所需的参数;
 Input          : ucPdpId
 Output         : pSnMdfIndPara,这些参数从APS的PDP状态机中获得;
 Return Value   :
 Date Accessed  :
 Date Update    :
 Calls          :
 Called By      :
 History        : ---
  1.Date        : 2005-
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_VOID    Aps_SmMsgModSnMdfIndParaPack
                            (   APS_INNER_SN_MDF_ST        *pInnerSnMdfInd,
                                APS_SNDCP_MODIFY_IND_ST    *pSnMdfIndPara   )
{
    VOS_UINT8                           ucPdpId;

    ucPdpId                             = pInnerSnMdfInd->ucPdpId;

    pSnMdfIndPara->OP_SNPduNum          = 0;
    pSnMdfIndPara->OP_RNPduNum          = 0;
    pSnMdfIndPara->SPARE                = 0;

    pSnMdfIndPara->ucNsapi              = g_PdpEntity[ucPdpId].ucNsapi;
    pSnMdfIndPara->ucSapi               = g_PdpEntity[ucPdpId].GprsPara.ucSapi;
    pSnMdfIndPara->ulTLLI               = GMM_GetTlli();
    pSnMdfIndPara->ucRadioPriority      = g_PdpEntity[ucPdpId].GprsPara.ucRadioPriority;

    /*拷贝QoS:之所以不使用sizeof(ST_QOS)而使用sizeof(APS_ST_QOS),是因为QOS_STRU的长度为了兼容性的考虑，定为
      了14，而APS_ST_QOS结构体的长度则需依赖版本而定*/
    PS_MEM_CPY(                       &pSnMdfIndPara->NegoQos,
                                      pInnerSnMdfInd->pQos,
                                      sizeof(APS_ST_QOS) );

}

/*****************************************************************************
 Prototype      : Aps_SmMsgModSnDeActInd()
 Description    : APS收到SM的去激活CNF后,将调用此函数向SNDCP发送去激活指示消息.
                  该函数将查询PDP状态机以获得去激活SNDCP所需的参数.
 Input          : ucPdpId,等于SM返回的消息中的参数ucCr,用于APS标识各PDP.
 Output         : NO
 Return Value   :
 Calls          : Aps_SmMsgModSnMdfIndParaPack()
                  Aps_SmMsgModPackSnMsg()
 Called By      : Aps_PdpDeActCnf()
 History        : ---
  1.Date        : 2005-
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32  Aps_SmMsgModSnDeActInd (    VOS_UINT8    ucPdpId )
{
    VOS_UINT32                          ulRet;
    APS_SNDCP_DEACTIVATE_IND_MSG       *pSnDeActIndMsg;

    /*申请消息*/
    /* BEGIN 2006-04-06 Modified by liuyang id:48197 for A32D01738*/
    pSnDeActIndMsg = (APS_SNDCP_DEACTIVATE_IND_MSG *)PS_ALLOC_MSG(WUEPS_PID_TAF,(sizeof(APS_SNDCP_DEACTIVATE_IND_MSG) - VOS_MSG_HEAD_LENGTH));
    /*  END  2006-04-06 Modified by liuyang id:48197 for A32D01738*/
    if ( VOS_NULL_PTR == pSnDeActIndMsg )
    {
        APS_ERR_LOG("Aps_SmMsgModSnDeActInd VOS_AllocMsg err ");
        return TAF_FAILURE;
    }

    /*打包消息头*/
    APS_PACK_SN_MSG_HEADER((*pSnDeActIndMsg), APS_SNDCP_DEACTIVATE_IND_ST);

    /*赋值消息类型*/
    pSnDeActIndMsg->usMsgType           = APS_SN_DEACT_IND_MSG_TYPE;

    /*填消息中的usPId*/
    pSnDeActIndMsg->usPId
            = (VOS_UINT16)g_PdpEntity[ucPdpId].ucNsapi;

    /*打包消息体*/
    Aps_SmMsgModSnDeActIndParaPack(     ucPdpId,
                                        &pSnDeActIndMsg->ApsSnDeActInd  );

    /*向SNDCP发送消息*/
    ulRet = PS_SEND_MSG( WUEPS_PID_TAF, pSnDeActIndMsg );
    if ( VOS_OK != ulRet )
    {
        APS_WARN_LOG("Send APS_SNDCP_DEACTIVATE_IND_MSG ERR ");
        return  APS_FAIL;
    }
    APS_NORM_LOG("APS->SNDCP APS_SNDCP_DEACTIVATE_IND_MSG ");

    return APS_SUCC;
}

/*****************************************************************************
 Prototype      : Aps_SmMsgModSnMdfIndParaPack()
 Description    : 构造修改SNDCP所需的参数;
 Input          : ucPdpId
 Output         : pSnMdfIndPara,这些参数从APS的PDP状态机中获得;
 Return Value   :
 Date Accessed  :
 Date Update    :
 Calls          :
 Called By      :
 History        : ---
  1.Date        : 2005-
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_VOID    Aps_SmMsgModSnDeActIndParaPack
                            (   VOS_UINT8                     ucPdpId,
                                APS_SNDCP_DEACTIVATE_IND_ST*  pSnDeActIndPara )
{
    pSnDeActIndPara->ucNsapi            = g_PdpEntity[ucPdpId].ucNsapi;
    pSnDeActIndPara->ucLLCReleaseInd    = APS_SN_NOT_REL_LLC;
    pSnDeActIndPara->ucXIDNegoInd       = APS_SN_XID_NEGO;
    pSnDeActIndPara->ulTLLI             = GMM_GetTlli();

    return;
}



VOS_UINT32 Aps_UnpackSmQos (
    SM_TAF_QOS_STRU                     *pSmQos,
    VOS_UINT8                           *pucQosLen,
    APS_PDP_QOS_ST                      *pApsQos
)
{
    /*参数检查*/
    if( (VOS_NULL == pSmQos)
       || (VOS_NULL == pucQosLen)
       || (VOS_NULL == pApsQos) )
    {
        APS_WARN_LOG("The input pointer parameter  can not be NULL , Aps_DetachSmQosStru()");
        return(APS_ERROR);
    }

    *pucQosLen = (VOS_UINT8)pSmQos->ulQosLen;

    /* QoS长度的兼容性检查 */
    if (*pucQosLen < 3)
    {
        APS_WARN_LOG("QoS len is abnormal, Aps_DetachSmQosStru()");
        return(APS_ERROR);
    }

    /* 初始化Qos目标结构体 */
    VOS_MemSet(pApsQos, 0x00, sizeof(APS_PDP_QOS_ST));

    pApsQos->ucDelayClass           = (pSmQos->aucQos[0] & APS_QOS_DELAY_CLASS)>>3;
    pApsQos->ucReliabClass          = (pSmQos->aucQos[0] & APS_QOS_RELIABILITY_CLASS);

    pApsQos->ucPeakThrough          = (pSmQos->aucQos[1] & APS_QOS_PEAK_THROUGHPUT)>>4;
    pApsQos->ucPrecedenceClass      = (pSmQos->aucQos[1] & APS_QOS_PRECEDENCE_CLASS);

    pApsQos->ucMeanThrough          = (pSmQos->aucQos[2] & APS_QOS_MEAN_THROUGHPUT);

    if (*pucQosLen > 3)
    {
        pApsQos->ucTrafficClass     = (pSmQos->aucQos[3] & APS_QOS_TRAFFIC_CLASS)>>5;
        pApsQos->ucDeliverOrder     = (pSmQos->aucQos[3] & APS_QOS_DELIVERY_ORDER)>>3;
        pApsQos->ucDeliverOfErrSdu  = (pSmQos->aucQos[3] & APS_QOS_DELIVERY_ERRORNEOUS_SDU);
    }

    if (*pucQosLen > 4)
    {
        pApsQos->ucMaximSduSize         = pSmQos->aucQos[4];
    }

    if (*pucQosLen > 5)
    {
        pApsQos->ucMaxBitRateForUp      = pSmQos->aucQos[5];
    }

    if (*pucQosLen > 6)
    {
        pApsQos->ucMaxBitRateForDown    = pSmQos->aucQos[6];
    }

    if (*pucQosLen > 7)
    {
        pApsQos->ucResidualBer          = (pSmQos->aucQos[7] & APS_QOS_RESIDUAL_BER)>>4;
        pApsQos->ucSduErrRatio          = (pSmQos->aucQos[7] & APS_QOS_SDU_ERROR_RATIO);
    }

    if (*pucQosLen > 8)
    {
        pApsQos->ucTransDelay           = (pSmQos->aucQos[8] & APS_QOS_TRANSFER_DELAY)>>2;
        pApsQos->ucTraffHandlPrior      = (pSmQos->aucQos[8] & APS_QOS_TRAFFIC_HANDLING_PRIORITY);
    }

    if (*pucQosLen > 9)
    {
        pApsQos->ucGuarantBitRateForUp  = pSmQos->aucQos[9];
    }

    if (*pucQosLen > 10)
    {
       pApsQos->ucGuarantBitRateForDown = pSmQos->aucQos[10];
    }

    #if(PS_UE_REL_VER >= PS_PTL_VER_R5)
    if (*pucQosLen > 11)
    {
        pApsQos->ucSrcStatisticsDescriptor = (pSmQos->aucQos[11] & APS_QOS_SSD);

        /* Signalling-Indication 的值只取该字节的第5位 */
        pApsQos->ucSignallingIndication    = ((pSmQos->aucQos[11] & APS_QOS_SIGNALLING_INDICATION) >> 4);
    }

    if (*pucQosLen > 12)
    {
        pApsQos->ucMaxBitRateForDownExt     = pSmQos->aucQos[12];
    }

    if (*pucQosLen > 13)
    {
        pApsQos->ucGuarantBitRateForDownExt = pSmQos->aucQos[13];
    }
    #endif

    #if(PS_UE_REL_VER >= PS_PTL_VER_R7)
    if (*pucQosLen > 14)
    {
        pApsQos->ucMaxBitRateForUpExt     = pSmQos->aucQos[14];
    }

    if (*pucQosLen > 15)
    {
        pApsQos->ucGuarantBitRateForUpExt = pSmQos->aucQos[15];
    }
    #endif

    return(APS_SUCCESS);
}


VOS_UINT32 MN_APS_DecodeIpcpConfigReqPkt(
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity,
    VOS_UINT8                          *pucProtocolPacket,
    VOS_UINT16                          usPktLen
)
{
    VOS_UINT8                           ucOptType;
    VOS_UINT8                           ucOptLen;

    /*-------------------------------------------------------------------
       只处理Gatewary IP (Peer IP)
    -------------------------------------------------------------------*/
    while (usPktLen)
    {
        /*lint -e961*/
        /* 获取IPCP Option类型 */
        ucOptType   = *pucProtocolPacket++;

        /* 获取IPCP Option长度 */
        ucOptLen    = *pucProtocolPacket++;
        /*lint +e961*/

        /* 如果剩余的IPCP Packet的长度比该IPCP Option的长度小, 则直接丢弃 */
        if (usPktLen < ucOptLen)
        {
            MN_ERR_LOG("MN_APS_DecodeIpcpCodeConfigReq: Invalid IPCP packet Len.");
            return VOS_ERR;
        }

        /* 如果IPCP Option的长度为0, 则直接丢弃 */
        if (0 == ucOptLen)
        {
            MN_ERR_LOG("MN_APS_DecodeIpcpCodeConfigReq: IPCP option length is zero.");
            return VOS_ERR;
        }

        switch (ucOptType)
        {
            case APS_PPP_IP_ADDR_ID:
                /*--------------------------------------------------------------
                   Peer IP地址长度必须等于6
                   OPTION TYPE(1 byte) + OPTION LEN(1 btye) + IPV4 LEN(4 bytes)
                --------------------------------------------------------------*/
                if (APS_PPP_IP_ADDR_LEN != ucOptLen)
                {
                    MN_ERR_LOG1("MN_APS_DecodeIpcpCodeConfigReq: Invalid IPCP ADDRESS Option. Len:",
                                ucOptLen);
                    return VOS_ERR;
                }
                else
                {
                    pstPdpEntity->stPdpGateWay.bitOpGateWay = VOS_TRUE;
                    PS_MEM_CPY(pstPdpEntity->stPdpGateWay.aucGateWay,
                               pucProtocolPacket,
                               TAF_IPV4_ADDR_LEN);

                    pucProtocolPacket += TAF_IPV4_ADDR_LEN;
                    usPktLen          -= ucOptLen;
                }
                break;

            default:
                MN_WARN_LOG1("MN_APS_DecodeIpcpCodeConfigReq: Option type not supported %d",ucOptType);
                pucProtocolPacket += (ucOptLen - 2);
                usPktLen -= ucOptLen;
                break;
        }
    }

    return VOS_OK;
}


VOS_UINT32 MN_APS_DecodeIpcpConfigNakAckRejPkt(
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity,
    VOS_UINT8                          *pucProtocolPacket,
    VOS_UINT16                          usPktLen
)
{
    VOS_UINT8                           ucOptType;
    VOS_UINT8                           ucOptLen;

    while (usPktLen)
    {
        /*lint -e961*/
        /* 获取IPCP Option类型 */
        ucOptType   = *pucProtocolPacket++;

        /* 获取IPCP Option长度 */
        ucOptLen    = *pucProtocolPacket++;
        /*lint +e961*/

        /* 如果剩余的IPCP Packet的长度比该IPCP Option的长度小, 则直接丢弃 */
        if (usPktLen < ucOptLen)
        {
            MN_ERR_LOG("MN_APS_DecodeIpcpConfigNakAckRejPkt: Invalid IPCP packet Len.");
            return VOS_ERR;
        }

        /* 如果IPCP Option的长度为0, 则直接丢弃 */
        if (0 == ucOptLen)
        {
            MN_ERR_LOG("MN_APS_DecodeIpcpConfigNakAckRejPkt: IPCP option length is zero.");
            return VOS_ERR;
        }

        switch (ucOptType)
        {
            case APS_PPP_PRI_DNS_ID:
                /*--------------------------------------------------------------
                   Primary DNS地址长度必须等于6
                   OPTION TYPE(1 byte) + OPTION LEN(1 btye) + IPV4 LEN(4 bytes)
                --------------------------------------------------------------*/
                if (APS_PPP_PRI_DNS_LEN != ucOptLen)
                {
                    MN_ERR_LOG1("MN_APS_DecodeIpcpConfigNakAckRejPkt: Invalid Pri DNS Option Len. Len:",
                                ucOptLen);
                    return VOS_ERR;
                }
                else
                {
                    pstPdpEntity->PdpDns.OP_PriDns      = VOS_TRUE;
                    PS_MEM_CPY(pstPdpEntity->PdpDns.aucPriDns,
                               pucProtocolPacket,
                               TAF_IPV4_ADDR_LEN);

                    pucProtocolPacket += TAF_IPV4_ADDR_LEN;
                    usPktLen          -= ucOptLen;
                }
                break;

            case APS_PPP_PRI_NBNS_ID:
                /*--------------------------------------------------------------
                   Primary NBNS地址长度必须等于6
                   OPTION TYPE(1 byte) + OPTION LEN(1 btye) + IPV4 LEN(4 bytes)
                --------------------------------------------------------------*/
                if (APS_PPP_PRI_NBNS_LEN != ucOptLen)
                {
                    MN_ERR_LOG1("MN_APS_DecodeIpcpConfigNakAckRejPkt: Invalid Pri NBNS Option Len. Len:",
                                ucOptLen);
                    return VOS_ERR;
                }
                else
                {
                    pstPdpEntity->stPdpNbns.OP_PriNbns  = VOS_TRUE;
                    PS_MEM_CPY(pstPdpEntity->stPdpNbns.aucPriNbns,
                               pucProtocolPacket,
                               TAF_IPV4_ADDR_LEN);

                    pucProtocolPacket += TAF_IPV4_ADDR_LEN;
                    usPktLen          -= ucOptLen;
                }
                break;

            case APS_PPP_SEC_DNS_ID:
                /*--------------------------------------------------------------
                   Secondary DNS地址长度必须等于6
                   OPTION TYPE(1 byte) + OPTION LEN(1 btye) + IPV4 LEN(4 bytes)
                --------------------------------------------------------------*/
                if (APS_PPP_SEC_DNS_LEN != ucOptLen)
                {
                    MN_ERR_LOG1("MN_APS_DecodeIpcpConfigNakAckRejPkt: Invalid Sec DNS Option Len. Len:",
                                ucOptLen);
                    return VOS_ERR;
                }
                else
                {
                    pstPdpEntity->PdpDns.OP_SecDns      = VOS_TRUE;
                    PS_MEM_CPY(pstPdpEntity->PdpDns.aucSecDns,
                               pucProtocolPacket,
                               TAF_IPV4_ADDR_LEN);

                    pucProtocolPacket += TAF_IPV4_ADDR_LEN;
                    usPktLen          -= ucOptLen;
                }
                break;

          case APS_PPP_SEC_NBNS_ID:
              /*--------------------------------------------------------------
                 Secondary NBNS地址长度必须等于6
                 OPTION TYPE(1 byte) + OPTION LEN(1 btye) + IPV4 LEN(4 bytes)
              --------------------------------------------------------------*/
                if (APS_PPP_SEC_NBNS_LEN != ucOptLen)
                {
                    MN_ERR_LOG1("MN_APS_DecodeIpcpConfigNakAckRejPkt: Invalid Sec NBNS Option Len. Len:",
                                ucOptLen);
                    return VOS_ERR;
                }
                else
                {
                    pstPdpEntity->stPdpNbns.OP_SecNbns  = VOS_TRUE;
                    PS_MEM_CPY(pstPdpEntity->stPdpNbns.aucSecNbns,
                               pucProtocolPacket,
                               TAF_IPV4_ADDR_LEN);

                    pucProtocolPacket += TAF_IPV4_ADDR_LEN;
                    usPktLen          -= ucOptLen;
                }
                break;

            default:
                MN_WARN_LOG1("MN_APS_DecodeIpcpConfigNakAckRejPkt: Option type not supported. Type:",
                             ucOptType);

                pucProtocolPacket += (ucOptLen - 2);
                usPktLen          -= ucOptLen;
                break;
        }
    }

    return VOS_OK;
}


VOS_UINT32 MN_APS_DecodeIpcpProtocolPkt(
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity,
    VOS_UINT8                          *pucProtocolPkt,
    VOS_UINT16                          usProtocolPktLen
)
{
    VOS_UINT8                           ucConfigType;
    VOS_UINT8                           ucIdentifier;
    VOS_UINT16                          usLength;

    while (usProtocolPktLen > 0)
    {
        /*lint -e961*/
        /* 获取Config type */
        ucConfigType  = *pucProtocolPkt++;

        /* 获取Identifier */
        ucIdentifier  = *pucProtocolPkt++;
        MN_NORM_LOG1("MN_APS_DecodeIpcpProtocolPacket: Identifier:", ucIdentifier);

        /* 获取Packet length */
        usLength  = (*pucProtocolPkt++) << 8;
        usLength |= *pucProtocolPkt++;
        /*lint +e961*/

        if (usProtocolPktLen < usLength)
        {
            MN_ERR_LOG("MN_APS_DecodeIpcpProtocolPacket: Invalid packet length.");
            return VOS_ERR;
        }

        usProtocolPktLen -= usLength;

        if (usLength < APS_PCO_CONFIG_HDR_LEN)
        {
            MN_ERR_LOG("MN_APS_DecodeIpcpProtocolPacket: Invalid IPCP packet length.");
            return VOS_ERR;
        }

        /* 从包长度中减去CONFIG_HDR_LEN */
        usLength -= APS_PCO_CONFIG_HDR_LEN;

        /* 只处理CONFIG_REQ/CONFIG_NAK/CONFIG_ACK/CONFIG_REJ, 其它的跳过 */
        switch (ucConfigType)
        {
            case APS_PPP_CODE_REQ:
                MN_NORM_LOG("MN_APS_DecodeIpcpProtocolPacket: CONFIG_REQ IPCP pkt.");
                if (VOS_OK != MN_APS_DecodeIpcpConfigReqPkt(pstPdpEntity,
                                                            pucProtocolPkt,
                                                            usLength))
                {
                    return VOS_ERR;
                }

                pucProtocolPkt += usLength;
                break;

            case APS_PPP_CODE_NAK:
                MN_NORM_LOG("MN_APS_DecodeIpcpProtocolPacket: CONFIG NAK IPCP pkt.");
                if (VOS_OK != MN_APS_DecodeIpcpConfigNakAckRejPkt(pstPdpEntity,
                                                                  pucProtocolPkt,
                                                                  usLength))
                {
                    return VOS_ERR;
                }

                pucProtocolPkt += usLength;
                break;


            case APS_PPP_CODE_ACK:
                MN_NORM_LOG("MN_APS_DecodeIpcpProtocolPacket: CONFIG ACK IPCP pkt.");
                if (VOS_OK != MN_APS_DecodeIpcpConfigNakAckRejPkt(pstPdpEntity,
                                                                  pucProtocolPkt,
                                                                  usLength))
                {
                    return VOS_ERR;
                }

                pucProtocolPkt += usLength;
                break;

            case APS_PPP_CODE_REJ:
                MN_NORM_LOG("MN_APS_DecodeIpcpProtocolPacket: CONFIG REJ IPCP pkt.");
                if (VOS_OK != MN_APS_DecodeIpcpConfigNakAckRejPkt(pstPdpEntity,
                                                                  pucProtocolPkt,
                                                                  usLength))
                {
                    return VOS_ERR;
                }

                pucProtocolPkt += usLength;
                break;

            default:
                MN_WARN_LOG1("MN_APS_DecodeIpcpProtocolPacket: Invalid IPCP code, Ignore. Type:",
                             ucConfigType);
                pucProtocolPkt += usLength;
                break;
        }
    }

    return VOS_OK;
}
VOS_UINT32 TAF_APS_DecodeIpv4PcscfProtocolPkt(
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity,
    VOS_UINT8                          *pucIpv4PcscfNum,
    VOS_UINT8                          *pucProtocolPacket,
    VOS_UINT16                          usProtocolPacketLen
)
{
    VOS_UINT8                           aucInvalidIpv4Addr[TAF_IPV4_ADDR_LEN] = {0,0,0,0};

    /* 检查存放IPv4 P-CSCF的Packet长度 */
    if (TAF_IPV4_ADDR_LEN != usProtocolPacketLen)
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,
            "TAF_APS_DecodeIpv4PcscfProtocolPkt: Wrong IPv4 P-CSCF Address");
        return VOS_ERR;
    }

    /*-------------------------------------------------------------
       只处理两个IPv4的P-CSCF, 如果PCO包含两个以上的IPv4 P-CSCF,
       只取前两个, 其它的忽略.
    -------------------------------------------------------------*/
    if (*pucIpv4PcscfNum == 0)
    {
        if (0 != PS_MEM_CMP(pucProtocolPacket,
                            aucInvalidIpv4Addr,
                            TAF_IPV4_ADDR_LEN))
        {
            pstPdpEntity->stPdpPcscf.bitOpPrimPcscfAddr = VOS_TRUE;
            PS_MEM_CPY(pstPdpEntity->stPdpPcscf.aucPrimPcscfAddr,
                       pucProtocolPacket,
                       TAF_IPV4_ADDR_LEN);
            (*pucIpv4PcscfNum)++;
        }
    }
    else if (*pucIpv4PcscfNum == 1)
    {
        if (0 != PS_MEM_CMP(pucProtocolPacket,
                            aucInvalidIpv4Addr,
                            TAF_IPV4_ADDR_LEN))
        {
            pstPdpEntity->stPdpPcscf.bitOpSecPcscfAddr = VOS_TRUE;
            PS_MEM_CPY(pstPdpEntity->stPdpPcscf.aucSecPcscfAddr,
                       pucProtocolPacket,
                       TAF_IPV4_ADDR_LEN);
            (*pucIpv4PcscfNum)++;
        }
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,
            "TAF_APS_DecodeIpv4PcscfProtocolPkt: More than two IPv6 P-CSCF addresses present, Ignore the others.");
    }

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_IPV6)

VOS_UINT32 MN_APS_DecodeIpV6DnsProtocolPkt(
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity,
    VOS_UINT8                          *pucIpV6DnsNum,
    VOS_UINT8                          *pucProtocolPacket,
    VOS_UINT16                          usProtocolPacketLen
)
{
    VOS_UINT8                           aucInvalidIpV6Addr[TAF_IPV6_ADDR_LEN];

    PS_MEM_SET(aucInvalidIpV6Addr, 0x00, TAF_IPV6_ADDR_LEN);

    if (TAF_IPV6_ADDR_LEN != usProtocolPacketLen)
    {
        MN_WARN_LOG("MN_APS_DecodeIpV6DnsProtocolPacket: Wrong IPV6 DNS Address");
        return VOS_ERR;
    }

    /*-------------------------------------------------------------------
       只处理两个IPV6的DNS, 如果PCO包含两个以上的IPV6 DNS, 只取前两个,
       其它的忽略.
    -------------------------------------------------------------------*/
    if (*pucIpV6DnsNum == 0)
    {
        if (0 != PS_MEM_CMP(pucProtocolPacket,
                            aucInvalidIpV6Addr,
                            TAF_IPV6_ADDR_LEN))
        {
            pstPdpEntity->stPdpIpV6Dns.bitOpPriDns = VOS_TRUE;
            PS_MEM_CPY(pstPdpEntity->stPdpIpV6Dns.aucPriDns,
                       pucProtocolPacket,
                       TAF_IPV6_ADDR_LEN);
            (*pucIpV6DnsNum)++;
        }
    }
    else if (*pucIpV6DnsNum == 1)
    {
        if (0 != PS_MEM_CMP(pucProtocolPacket,
                            aucInvalidIpV6Addr,
                            TAF_IPV6_ADDR_LEN))
        {
            pstPdpEntity->stPdpIpV6Dns.bitOpSecDns = VOS_TRUE;
            PS_MEM_CPY(pstPdpEntity->stPdpIpV6Dns.aucSecDns,
                       pucProtocolPacket,
                       TAF_IPV6_ADDR_LEN);
            (*pucIpV6DnsNum)++;
        }
    }
    else
    {
        MN_WARN_LOG("MN_APS_DecodeIpV6DnsProtocolPacket: More than two IPV6 DNS addresses present, Ignore the others.");
    }

    return VOS_OK;
}


VOS_UINT32 TAF_APS_DecodeIpv6PcscfProtocolPkt(
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity,
    VOS_UINT8                          *pucIpv6PcscfNum,
    VOS_UINT8                          *pucProtocolPacket,
    VOS_UINT16                          usProtocolPacketLen
)
{
    VOS_UINT8                           aucInvalidIpv6Addr[TAF_IPV6_ADDR_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    /* 检查存放IPv6 P-CSCF的Packet长度 */
    if (TAF_IPV6_ADDR_LEN != usProtocolPacketLen)
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,
            "TAF_APS_DecodeIpv6PcscfProtocolPkt: Wrong IPv6 P-CSCF Address");
        return VOS_ERR;
    }

    /*-------------------------------------------------------------
       只处理两个IPv6的P-CSCF, 如果PCO包含两个以上的IPv6 P-CSCF,
       只取前两个, 其它的忽略.
    -------------------------------------------------------------*/
    if (*pucIpv6PcscfNum == 0)
    {
        if (0 != PS_MEM_CMP(pucProtocolPacket,
                            aucInvalidIpv6Addr,
                            TAF_IPV6_ADDR_LEN))
        {
            pstPdpEntity->stPdpIpv6Pcscf.bitOpPrimPcscfAddr = VOS_TRUE;
            PS_MEM_CPY(pstPdpEntity->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                       pucProtocolPacket,
                       TAF_IPV6_ADDR_LEN);
            (*pucIpv6PcscfNum)++;
        }
    }
    else if (*pucIpv6PcscfNum == 1)
    {
        if (0 != PS_MEM_CMP(pucProtocolPacket,
                            aucInvalidIpv6Addr,
                            TAF_IPV6_ADDR_LEN))
        {
            pstPdpEntity->stPdpIpv6Pcscf.bitOpSecPcscfAddr = VOS_TRUE;
            PS_MEM_CPY(pstPdpEntity->stPdpIpv6Pcscf.aucSecPcscfAddr,
                       pucProtocolPacket,
                       TAF_IPV6_ADDR_LEN);
            (*pucIpv6PcscfNum)++;
        }
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,
            "TAF_APS_DecodeIpv6PcscfProtocolPkt: More than two IPv6 P-CSCF addresses present, Ignore the others.");
    }

    return VOS_OK;
}
#endif


VOS_UINT32 MN_APS_DecodeProtocolConfigPara(
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity,
    MN_APS_PROTOCOL_CFG_OPTIONS_STRU   *pstProtoCfgOptions
)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT16                          usProtocolId;
    VOS_UINT8                           ucIpv4PcscfNum;
#if (FEATURE_ON == FEATURE_IPV6)
    VOS_UINT8                           ucIpV6DnsNum;
    VOS_UINT8                           ucIpv6PcscfNum;
#endif

    usProtocolId                        = 0;
    ucIpv4PcscfNum                      = 0;
#if (FEATURE_ON == FEATURE_IPV6)
    ucIpV6DnsNum                        = 0;
    ucIpv6PcscfNum                      = 0;
#endif

    for (ulIndex = 0; ulIndex < pstProtoCfgOptions->ucProtocolIdsNum; ulIndex++)
    {
        /* 获取Protocol ID */
        usProtocolId  = pstProtoCfgOptions->stProtocolInfo[ulIndex].aucProtocolId[0] << 8;
        usProtocolId |= pstProtoCfgOptions->stProtocolInfo[ulIndex].aucProtocolId[1];

        MN_NORM_LOG1("MN_APS_DecodeProtocolConfigPara: Decode Protocol ID:",
                     usProtocolId);

        switch (usProtocolId)
        {
            case MN_APS_IPCP_PROTOCOL_ID:
                /* 解码IPCP Protocol (0x8021) */
                if (VOS_OK != MN_APS_DecodeIpcpProtocolPkt(
                                pstPdpEntity,
                                pstProtoCfgOptions->stProtocolInfo[ulIndex].aucProtocolContents,
                                pstProtoCfgOptions->stProtocolInfo[ulIndex].usProtocolLen))
                {
                    return VOS_ERR;
                }
                break;

            case TAF_APS_IPV4_PCSCF_PROTOCOL_ID:
                /* 解码IPv4 P-CSCF (0x000C) */
                TAF_APS_DecodeIpv4PcscfProtocolPkt(
                    pstPdpEntity,
                    &ucIpv4PcscfNum,
                    pstProtoCfgOptions->stProtocolInfo[ulIndex].aucProtocolContents,
                    pstProtoCfgOptions->stProtocolInfo[ulIndex].usProtocolLen);
                break;

#if (FEATURE_ON == FEATURE_IPV6)
            case MN_APS_IPV6_DNS_PROTOCOL_ID:
                /* 解码IPv6 DNS (0x0003) */
                MN_APS_DecodeIpV6DnsProtocolPkt(
                    pstPdpEntity,
                    &ucIpV6DnsNum,
                    pstProtoCfgOptions->stProtocolInfo[ulIndex].aucProtocolContents,
                    pstProtoCfgOptions->stProtocolInfo[ulIndex].usProtocolLen);
                break;

            case TAF_APS_IPV6_PCSCF_PROTOCOL_ID:
                /* 解码IPv6 P-CSCF (0x0001) */
                TAF_APS_DecodeIpv6PcscfProtocolPkt(
                    pstPdpEntity,
                    &ucIpv6PcscfNum,
                    pstProtoCfgOptions->stProtocolInfo[ulIndex].aucProtocolContents,
                    pstProtoCfgOptions->stProtocolInfo[ulIndex].usProtocolLen);
                break;
#endif

            default:
                MN_NORM_LOG1("MN_APS_DecodeProtocolConfigPara: Protocol ID not supported. Protocol ID:",
                             usProtocolId);
        }
    }

    return VOS_OK;
}
VOS_UINT32 MN_APS_UnpackProtocolOptionsList(
    MN_APS_PROTOCOL_CFG_OPTIONS_STRU   *pstProtoCfgOptions,
    SM_TAF_PROT_CFG_OPT_STRU           *pstSmPco
)
{
    VOS_UINT32                          ulPcoIndex;
    VOS_UINT32                          ulIndex;

    ulPcoIndex                          = 0;

    /*lint -e961*/
    /* 拆出Configuration protocol */
    pstProtoCfgOptions->ucConfigProtocol = pstSmPco->aucProtCfgOpt[ulPcoIndex++] & 0x7F;
    /*lint +e961*/
    pstProtoCfgOptions->ucProtocolIdsNum = 0;

    /* 拆出所有Protocol ID的内容 */
    for (ulIndex = 0; ((ulPcoIndex + 1) <= pstSmPco->ulPcoLen)
             && (ulIndex < MN_APS_MAX_PROTOCOL_ID_NUM); ulIndex++)
    {
        pstProtoCfgOptions->ucProtocolIdsNum++;
        /*lint -e961*/
        /* 拆出Protocol ID */
        pstProtoCfgOptions->stProtocolInfo[ulIndex].aucProtocolId[0]
                                            = pstSmPco->aucProtCfgOpt[ulPcoIndex++];
        pstProtoCfgOptions->stProtocolInfo[ulIndex].aucProtocolId[1]
                                            = pstSmPco->aucProtCfgOpt[ulPcoIndex++];

        /* 拆出Protocol length */
        pstProtoCfgOptions->stProtocolInfo[ulIndex].usProtocolLen
                                            = pstSmPco->aucProtCfgOpt[ulPcoIndex++];
        /*lint +e961*/

        if ( ((ulPcoIndex - 1) + pstProtoCfgOptions->stProtocolInfo[ulIndex].usProtocolLen)
             <= pstSmPco->ulPcoLen )
        {
            PS_MEM_CPY(pstProtoCfgOptions->stProtocolInfo[ulIndex].aucProtocolContents,
                       &pstSmPco->aucProtCfgOpt[ulPcoIndex],
                       pstProtoCfgOptions->stProtocolInfo[ulIndex].usProtocolLen);

            ulPcoIndex += pstProtoCfgOptions->stProtocolInfo[ulIndex].usProtocolLen;
        }
        else
        {
            /* 跳过其它的Protocol ID */
            pstProtoCfgOptions->stProtocolInfo[ulIndex].usProtocolLen = 0;
            ulPcoIndex = pstSmPco->ulPcoLen;
            ulPcoIndex++;
        }
    }

    return VOS_OK;
}
VOS_UINT32 MN_APS_UnpackPco(
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity,
    SM_TAF_PROT_CFG_OPT_STRU           *pstSmPco
)
{
    MN_APS_PROTOCOL_CFG_OPTIONS_STRU    stProtocolCfgOptions;

    /* 清除DNS相关的bitOp标志 */
    PS_MEM_SET(&pstPdpEntity->PdpDns, 0x00, sizeof(VOS_UINT32));
    PS_MEM_SET(&pstPdpEntity->stPdpNbns, 0x00, sizeof(VOS_UINT32));
    PS_MEM_SET(&pstPdpEntity->stPdpPcscf, 0x00, sizeof(VOS_UINT32));
#if (FEATURE_ON == FEATURE_IPV6)
    PS_MEM_SET(&pstPdpEntity->stPdpIpV6Dns, 0x00, sizeof(VOS_UINT32));
    PS_MEM_SET(&pstPdpEntity->stPdpIpv6Pcscf, 0x00, sizeof(VOS_UINT32));
#endif

    /* 拆出Protocol Configuration参数列表 */
    if (VOS_OK != MN_APS_UnpackProtocolOptionsList(&stProtocolCfgOptions,
                                                   pstSmPco))
    {
        MN_ERR_LOG("MN_APS_UnpackPco: Unpack PCO failed.");
        return VOS_ERR;
    }

    /* 解码Protocol Configuration参数 */
    if (VOS_OK != MN_APS_DecodeProtocolConfigPara(pstPdpEntity,
                                                  &stProtocolCfgOptions))
    {
        MN_ERR_LOG("MN_APS_UnpackPco: Decode PCO parameters failed.");
        return VOS_ERR;
    }

    return VOS_OK;

}

 /*******************************************************************************
 Function       : Aps_CheckOutPcoAuth()
 Description    : 从网络返回的PCO中拆出鉴权信息的PPP码流
 Calls          :
 Called         : Aps_PdpActRej()
 Date Accessed  :
 Date Update    :
 Input          : pSmPco
 Output         : pAuthPcoPPP
 Return         : APS_FAIL 函数执行失败
                  APS_SUCC 函数执行成功
Others          :
*******************************************************************************/
VOS_UINT32  Aps_CheckOutPcoAuth(        SM_TAF_PROT_CFG_OPT_STRU        *pSmPco,
                                        APS_PCO_AUTH_PPP_ST    *pAuthPcoPPP)
{
    VOS_UINT8                           ucLocation = 0;


    ucLocation++;                       /*跳过PCO头部的PCP type 0x80*/


    /*检查下面要处理的协议是否AUTH协议,若不是则向下跳*/
    while( (VOS_UINT32)(ucLocation + 1)  < pSmPco->ulPcoLen)
    {
        if ( ((0xC0 != pSmPco->aucProtCfgOpt[ucLocation])
               ||(0x23 != pSmPco->aucProtCfgOpt[ucLocation + 1]))
             &&((0xC2 != pSmPco->aucProtCfgOpt[ucLocation])
               ||(0x23 != pSmPco->aucProtCfgOpt[ucLocation + 1])))
        {
            ucLocation  += 2;           /*跳过协议码2个字节 */
            ucLocation  += (pSmPco->aucProtCfgOpt[ucLocation] + 1);
        }
        else
        {   /*若是IPCP,则跳出循环*/
            break;
        }
    }

    /*若没有AUTH协议,则退出函数*/
    if ( (VOS_UINT32)(ucLocation + 1) >= pSmPco->ulPcoLen )
    {
        APS_NORM_LOG("No IPCP in PCO.");
        return  APS_FAIL;
    }

    /*若没有跳出，则下面处理AUTH码流*/
    ucLocation          += 2;           /*跳过协议码2个字节 */

    pAuthPcoPPP->usLen  = (VOS_UINT16)pSmPco->aucProtCfgOpt[ucLocation];

    ucLocation          += 1;           /*跳过长度码1个字节 */
    pAuthPcoPPP->pProtIdCont    = &(pSmPco->aucProtCfgOpt[ucLocation]);


    return  APS_SUCC;
}

/*******************************************************************************
 Function       : Aps_CheckOutPcoOneIpcp()
 Description    : 从IPCP码流中拆除一个具体的NAK, ACK, REJ码流
 Calls          :
 Called         : Aps_PdpActRej()
 Date Accessed  :
 Date Update    :
 Input          : pIpcpPcpPPP
                  ucPcoType :REQ,ACK,NAK,REJ
 Output         : pOneIpcp
 Return         : APS_FAIL
                  APS_SUCC
 Others         :
*******************************************************************************/
VOS_UINT32  Aps_CheckOutPcoOneIpcp(     APS_PCO_IPCP_PPP_ST    *pIpcpPcpPPP,
                                        VOS_UINT8               ucPcoCode,
                                        APS_PCO_PPP_ST         *pOneIpcp)
{
    VOS_UINT8                           ucLocation = 0;


    /*检查下面要处理的code是否输入的code,若不是则向下跳*/
    while( (ucLocation + 1)  < pIpcpPcpPPP->usLen)
    {
        if ((ucPcoCode != pIpcpPcpPPP->pProtIdCont[ucLocation]))
        {   /*若不是输入的CODE*/

            /*IPV4长度的两个字节中,只有低字节有效,跳过该CODE协议的PPP码流*/
            ucLocation  += (pIpcpPcpPPP->pProtIdCont[ucLocation+3]);
        }
        else
        {   /*若是输入的CODE,则跳出循环*/
            break;
        }
    }

    /*若没有输入的CODE协议,则退出函数*/
    if ( (ucLocation + 1) >= pIpcpPcpPPP->usLen )
    {
        APS_NORM_LOG("No this CODE in PCO.");
        return  APS_FAIL;
    }

    /*若没有跳出，则下面处理IPCP码流*/
    /*IPV4长度的两个字节中，只有低字节有效*/
    pOneIpcp->usLen = (VOS_UINT16)pIpcpPcpPPP->pProtIdCont[ucLocation+3];

    pOneIpcp->pProtIdCont
                    = &(pIpcpPcpPPP->pProtIdCont[ucLocation]);

    return  APS_SUCC;
}


/*******************************************************************************
 Function       : Aps_CheckOutIpcpIpAddr()
 Description    : 从一个具体的NAK, ACK, REJIPCP码流中拆出其中的IPADDR的PPP码流
 Calls          :
 Called         : Aps_PdpActRej()
 Date Accessed  :
 Date Update    :
 Input          : pIpcpPcpPPP
                  ucIpcpId :    #define APS_PPP_IP_ADDR_ID      0x03
                                #define APS_PPP_PRI_DNS_ID      0x81
                                #define APS_PPP_SEC_DNS_ID      0x83
 Output         : pPppId
 Return         : APS_FAIL
                  APS_SUCC
 Others         :
*******************************************************************************/
VOS_UINT32  Aps_CheckOutOneIpcpOneId(   APS_PCO_IPCP_PPP_ST    *pOneIpcp,
                                        VOS_UINT8               ucIpcpId,
                                        APS_PPP_ID_ST          *pPppId)
{
    VOS_UINT8                           ucLoc = 0;

    ucLoc       += 4;                   /*跳过CODE1字节,Identifier1字节,长度2字节*/


    /*检查下面要处理的code是否输入的code,若不是则向下跳*/
    while( (ucLoc + 1)  < pOneIpcp->usLen)
    {
        if ((ucIpcpId != pOneIpcp->pProtIdCont[ucLoc]))
        {   /*若不是输入的ID*/

            /*IPV4长度的1个字节中,跳过该ID的PPP码流*/
            ucLoc  += (pOneIpcp->pProtIdCont[ucLoc+1]);
        }
        else
        {   /*若是输入的ID,则跳出循环*/
            break;
        }
    }

    /*若没有输入的ID协议,则退出函数*/
    if ( (ucLoc + 1) >= pOneIpcp->usLen )
    {
        APS_NORM_LOG("No this ID in IPCP.");
        return  APS_FAIL;
    }

    /*若没有跳出，则下面处理IPCP码流*/
    /*IPV4长度的1个字节中*/
    pPppId->usLen = (VOS_UINT16)pOneIpcp->pProtIdCont[ucLoc+1];

    pPppId->pProtIdCont
                    = &(pOneIpcp->pProtIdCont[ucLoc]);

    return  APS_SUCC;
}

/*******************************************************************************
 Function       : Aps_PPPDns2ApsDns()
 Description    : 从PPP DNS码流中拆到APS状态机的DNS结构中
 Calls          :
 Called         :
 Date Accessed  :
 Date Update    :
 Input          : pOneIpcp
 Output         : pPdpDns
 Return         : APS_FAIL
                  APS_SUCC
 Others         :
*******************************************************************************/
VOS_VOID    Aps_PPPDns2ApsDns(          APS_PCO_PPP_ST     *pOneIpcp,
                                        APS_PDP_DNS_ST     *pPdpDns)
{
    VOS_UINT8                           ucLoc = 0;
    /*lint -e961*/
    if ( 0x81 == pOneIpcp->pProtIdCont[ucLoc])
    {   /*主DNS*/
        ucLoc   += 2;                   /*跳过PriDns标识1字节和长度1字节*/
        pPdpDns->OP_PriDns              = APS_USED;
        pPdpDns->aucPriDns[0]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpDns->aucPriDns[1]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpDns->aucPriDns[2]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpDns->aucPriDns[3]           = pOneIpcp->pProtIdCont[ucLoc++];
    }
    else if ( 0x83 == pOneIpcp->pProtIdCont[ucLoc])
    {   /*副DNS*/
        ucLoc   += 2;                   /*跳过SecDns标识1字节和长度1字节*/
        pPdpDns->OP_SecDns              = APS_USED;
        pPdpDns->aucSecDns[0]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpDns->aucSecDns[1]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpDns->aucSecDns[2]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpDns->aucSecDns[3]           = pOneIpcp->pProtIdCont[ucLoc++];
    }
    else
    {
        APS_NORM_LOG("Not DNS PPP !");
    }
    /*lint +e961*/
    return;
}


/*******************************************************************************
 Function       : Aps_PPPNbns2ApsNbns()
 Description    : 从PPP NBNS码流中拆到APS状态机的NBNS结构中
 Calls          :
 Called         :
 Date Accessed  :
 Date Update    :
 Input          : pOneIpcp
 Output         : pPdpNbns
 Return         : APS_FAIL
                  APS_SUCC
 Others         :
*******************************************************************************/
VOS_VOID    Aps_PPPNbns2ApsNbns(          APS_PCO_PPP_ST     *pOneIpcp,
                                        APS_PDP_NBNS_ST     *pPdpNbns)
{
    VOS_UINT8                           ucLoc = 0;
    /*lint -e961*/
    if ( APS_PPP_PRI_NBNS_ID == pOneIpcp->pProtIdCont[ucLoc])
    {   /*主NBNS*/
        ucLoc   += 2;                   /*跳过PriNbns标识1字节和长度1字节*/
        pPdpNbns->OP_PriNbns              = APS_USED;
        pPdpNbns->aucPriNbns[0]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpNbns->aucPriNbns[1]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpNbns->aucPriNbns[2]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpNbns->aucPriNbns[3]           = pOneIpcp->pProtIdCont[ucLoc++];
    }
    else if ( APS_PPP_SEC_NBNS_ID == pOneIpcp->pProtIdCont[ucLoc])
    {   /*副NBNS*/
        ucLoc   += 2;                   /*跳过SecNbns标识1字节和长度1字节*/
        pPdpNbns->OP_SecNbns              = APS_USED;
        pPdpNbns->aucSecNbns[0]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpNbns->aucSecNbns[1]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpNbns->aucSecNbns[2]           = pOneIpcp->pProtIdCont[ucLoc++];
        pPdpNbns->aucSecNbns[3]           = pOneIpcp->pProtIdCont[ucLoc++];
    }
    else
    {
        APS_NORM_LOG("Not NBNS PPP !");
    }
    /*lint +e961*/
    return;
}




/*******************************************************************************
 Function       : Aps_PPPAddr2ApsAddr()
 Description    : 从PPP DNS码流中拆到APS状态机的DNS结构中
 Calls          :
 Called         :
 Date Accessed  :
 Date Update    :
 Input          : pOneIpcp
 Output         : pPdpDns
 Return         : APS_FAIL
                  APS_SUCC
 Others         :
*******************************************************************************/
VOS_VOID    Aps_PPPAddr2ApsAddr(        APS_PCO_PPP_ST     *pOneIpcp,
                                        VOS_UINT8          *pIpAddr)
{
    VOS_UINT8                           ucLoc = 0;
    /*lint -e961*/
    /*IP ADDR*/
    if ( 0x03 == pOneIpcp->pProtIdCont[ucLoc])
    {
        if ( 0 != pOneIpcp->pProtIdCont[ucLoc+1])
        {   /*如果有IPADDR*/
            ucLoc   += 2;                   /*跳过PriDns标识1字节和长度1字节*/
            pIpAddr[0]                  = pOneIpcp->pProtIdCont[ucLoc++];
            pIpAddr[1]                  = pOneIpcp->pProtIdCont[ucLoc++];
            pIpAddr[2]                  = pOneIpcp->pProtIdCont[ucLoc++];
            pIpAddr[3]                  = pOneIpcp->pProtIdCont[ucLoc++];
        }
    }
    /*lint +e961*/
    return;
}

/*****************************************************************************
 Prototype      : Aps_UnPackSmAddr
 Description    : 将SM传来的ADDR结构拆包到APS定义的STRUCT中,包括PDPTYPE和ADDR
 Input          : pSmAddr
 Output         : pApsAddr
 Return Value   :
 Date Accessed  :
 Date Update    :
 Calls          :
 Called By      :
 History        : ---
  1.Date        : 2005-
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32  Aps_DetachSmAddrStru (
    SM_TAF_PDP_ADDR_STRU               *pSmAddr,
    APS_PDP_ADDR_ST                    *pApsAddr
)
{
    VOS_UINT8                           ucPdpOrgan;

    if((VOS_NULL == pSmAddr) || (VOS_NULL == pApsAddr))
    {
        APS_WARN_LOG("ERROR! The input pointer parameter can not be NULL! Aps_DetachSmAddrStru()");
        return(APS_ERROR);
    }

    /* 拷贝PDPORGAN */
    ucPdpOrgan = (pSmAddr->aucPdpAddr[0]) & 0x0F;
    switch(ucPdpOrgan)
    {
        case APS_ADDR_ORGAN_ETSI:
        case APS_ADDR_ORGAN_IETF:
        case APS_ADDR_ORGAN_EMPTY_PDP_TYPE:
                pApsAddr->ucPdpTypeOrgan = (pSmAddr->aucPdpAddr[0]) & 0x0F;
                break;
        default:
            APS_WARN_LOG("ERROR! Input Parameter error! Aps_DetachSmAddrStru()");
            return(APS_ERROR);
    }


    /* 拷贝PDPTYPE */
    switch (pSmAddr->aucPdpAddr[1])
    {
        /* ppp */
        case APS_PDP_TYPE_NUM_PPP  :
            pApsAddr->ucPdpTypeNum = APS_ADDR_PPP;
            break;

        /* IPV4 */
        case APS_PDP_TYPE_NUM_IPV4 :
            pApsAddr->ucPdpTypeNum = APS_ADDR_STATIC_IPV4;
            break;

#if (FEATURE_ON == FEATURE_IPV6)
        /* IPV6 */
        case MN_APS_PDP_TYPE_NUM_IPV6:
            pApsAddr->ucPdpTypeNum = MN_APS_ADDR_IPV6;
            break;

        /* IPV4V6 */
        case MN_APS_PDP_TYPE_NUM_IPV4V6:
            pApsAddr->ucPdpTypeNum = MN_APS_ADDR_IPV4V6;
            break;
#endif

        default:
            /* All other values shall be interpreted as IPv4 address */
            pApsAddr->ucPdpTypeNum = APS_ADDR_STATIC_IPV4;
            break;
    }

    /*
    If PDP type number indicates IPv4, the Address information in octet 5 to
    octet 8 contains the IPv4 address. Bit 8 of octet 5 represents the most
    significant bit of the IP address and bit 1 of octet 8 the least significant
    bit.
    If PDP type number indicates IPv6, the Address information in octet 5 to octet
    20 contains the IPv6 address. Bit 8 of octet 5 represents the most significant
    bit of the IP address and bit 1 of octet 20 the least significant bit.
    If PDP type number indicates IPv4v6:
    The Address information in octet 5 to octet 8 contains the IPv4 address. Bit
    8 of octet 5 represents the most significant bit of the IP address and bit 1
    of octet 8 the least significant bit.
    The Address information in octet 9 to octet 24 contains the IPv6 address. Bit
    8 of octet 9 represents the most significant bit of the IP address and bit 1
    of octet 24 the least significant bit.
    If PDP type number indicates IPv4 or IPv4v6 and DHCPv4 is to be used to allocate
    the IPv4 address, the IPv4 address shall be coded as 0.0.0.0.
    */

    /* 如果是IPV4, 则拷贝4个字节的IP 地址 */
    if (APS_ADDR_STATIC_IPV4 == pApsAddr->ucPdpTypeNum)
    {
        PS_MEM_CPY(pApsAddr->aucIpAddr,
                   &(pSmAddr->aucPdpAddr[2]),
                   TAF_IPV4_ADDR_LEN);
    }

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是IPV6, 则拷贝16个字节的IP 地址 */
    if (MN_APS_ADDR_IPV6 == pApsAddr->ucPdpTypeNum)
    {
        PS_MEM_CPY(pApsAddr->aucIpV6Addr,
                   &(pSmAddr->aucPdpAddr[2]),
                   TAF_IPV6_ADDR_LEN);
    }

    /* 如果是IPV4V6, 则拷贝4个字节的IPV4 地址 ，再拷贝IPV6地址*/
    if (MN_APS_ADDR_IPV4V6 == pApsAddr->ucPdpTypeNum)
    {
        PS_MEM_CPY(pApsAddr->aucIpAddr,
                   &(pSmAddr->aucPdpAddr[2]),
                   TAF_IPV4_ADDR_LEN);

        PS_MEM_CPY(pApsAddr->aucIpV6Addr,
                   &(pSmAddr->aucPdpAddr[2 + TAF_IPV4_ADDR_LEN]),
                   TAF_IPV6_ADDR_LEN);

    }
#endif

    return(APS_SUCCESS);

}
VOS_VOID    Aps_DetachSmApn  (  SM_TAF_APN_STRU*      pSmApn,
                                APS_PDP_APN_ST*         pApsApn )
{

    /* APN的长度 */
    pApsApn->ucLength = (VOS_UINT8)pSmApn->ulApnLen;
    if ( pApsApn->ucLength >= APS_MAX_LENGTH_OF_APN)
    {
        pApsApn->ucLength = APS_MAX_LENGTH_OF_APN - 1;
    }

    /* 拷贝APN */
    PS_MEM_CPY(       pApsApn->aucValue,
                      pSmApn->aucApnAddr,
                      pApsApn->ucLength   );

    return;
}


VOS_UINT32 Aps_CompareQos_ForDelayClass(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ( 0 != pUeMinQos->ucDelayClass )
    {
        if(pNetNegotQos->ucDelayClass > pUeMinQos->ucDelayClass)
        {
            return APS_QOS_NOT_UP_TO_MIN;
        }
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForReliabClass(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ( 0 != pUeMinQos->ucReliabClass)
    {
        if(pNetNegotQos->ucReliabClass > pUeMinQos->ucReliabClass)
        {
            return APS_QOS_NOT_UP_TO_MIN;
        }
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForPeakThrough(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ( 0 != pUeMinQos->ucPeakThrough)
    {
        if(pNetNegotQos->ucPeakThrough < pUeMinQos->ucPeakThrough)
        {
            return APS_QOS_NOT_UP_TO_MIN;
        }
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForPrecedenceClass(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ( 0 != pUeMinQos->ucPrecedenceClass)
    {
        if(pNetNegotQos->ucPrecedenceClass > pUeMinQos->ucPrecedenceClass)
        {
            return APS_QOS_NOT_UP_TO_MIN;
        }
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForTrafficClass(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ( 0 != pUeMinQos->ucTrafficClass)
    {
        if(pNetNegotQos->ucTrafficClass > pUeMinQos->ucTrafficClass)
        {
            return APS_QOS_NOT_UP_TO_MIN;
        }
    }

    return APS_QOS_UP_TO_MIN;
}



VOS_UINT32 Aps_CompareQos_ForDeliverOrder(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ( 0 != pUeMinQos->ucDeliverOrder )
    {
        if(pNetNegotQos->ucDeliverOrder != pUeMinQos->ucDeliverOrder)
        {
            return APS_QOS_NOT_UP_TO_MIN;
        }
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForDeliverOfErrSdu(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ( 0 != pUeMinQos->ucDeliverOfErrSdu )
    {
        if(pNetNegotQos->ucDeliverOfErrSdu < pUeMinQos->ucDeliverOfErrSdu)
        {
            return APS_QOS_NOT_UP_TO_MIN;
        }
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForMaximSduSize(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ( 0 != pUeMinQos->ucMaximSduSize )
    {
        if(pNetNegotQos->ucMaximSduSize < pUeMinQos->ucMaximSduSize)
        {
            return APS_QOS_NOT_UP_TO_MIN;
        }
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForSduErrRatio(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ( 0 != pUeMinQos->ucSduErrRatio )
    {
        if(SDU_ERR_RATIO_HIGHEST == pNetNegotQos->ucSduErrRatio)
        {
            if(SDU_ERR_RATIO_HIGHEST != pUeMinQos->ucSduErrRatio)
            {
                return APS_QOS_NOT_UP_TO_MIN;
            }
        }
        else/* 其他数字, 则越小, 代表的错SDU率越低 */
        {
            if(pNetNegotQos->ucSduErrRatio < pUeMinQos->ucSduErrRatio)
            {
                return APS_QOS_NOT_UP_TO_MIN;
            }
        }
    }

    return APS_QOS_UP_TO_MIN;
}
VOS_UINT32 Aps_CompareQos_ForTransDelay(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ((APS_QOS_TRAFFIC_CLASS_INTERACTIVE != pNetNegotQos->ucTrafficClass)
         && (APS_QOS_TRAFFIC_CLASS_BACKGROUND != pNetNegotQos->ucTrafficClass))
    {
        if ( 0 != pUeMinQos->ucTransDelay )
        {
            if(pNetNegotQos->ucTransDelay > pUeMinQos->ucTransDelay)
            {
                return APS_QOS_NOT_UP_TO_MIN;
            }
        }
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForTrafficHandlePriority(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ((APS_QOS_TRAFFIC_CLASS_CONVERSATIONAL != pNetNegotQos->ucTrafficClass)
         && (APS_QOS_TRAFFIC_CLASS_STREAMING != pNetNegotQos->ucTrafficClass)
         && (APS_QOS_TRAFFIC_CLASS_BACKGROUND != pNetNegotQos->ucTrafficClass))
    {
        if (0 != pUeMinQos->ucTraffHandlPrior)
        {
            if(pNetNegotQos->ucTraffHandlPrior > pUeMinQos->ucTraffHandlPrior)
            {
                return APS_QOS_NOT_UP_TO_MIN;
            }
        }
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForMaxBitRateForUp(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    #if(PS_UE_REL_VER < PS_PTL_VER_R7)
    if ( 0 != pUeMinQos->ucMaxBitRateForUp )
    {
        if(pNetNegotQos->ucMaxBitRateForUp < pUeMinQos->ucMaxBitRateForUp)
        {
            return APS_QOS_NOT_UP_TO_MIN;
        }
    }
    #else
    if ( 0 == pUeMinQos->ucMaxBitRateForUpExt )
    {
        if ( 0 == pNetNegotQos->ucMaxBitRateForUpExt )
        {
            if ( 0 != pUeMinQos->ucMaxBitRateForUp )
            {
                if(pNetNegotQos->ucMaxBitRateForUp < pUeMinQos->ucMaxBitRateForUp)
                {
                    return APS_QOS_NOT_UP_TO_MIN;
                }
            }
        }
        else
        {
            /* 协商的Qos的最大上行比特率必然高于pUeMinQos的最大上行比特率 */
        }
    }
    else
    {
        if ( 0 == pNetNegotQos->ucMaxBitRateForUpExt )
        {
            /* 此种情况下，协商的Qos的最大上行比特率必然低于pUeMinQos的最大上行比特率 */
            return APS_QOS_NOT_UP_TO_MIN;
        }
        else
        {
            if ( pNetNegotQos->ucMaxBitRateForUpExt < pUeMinQos->ucMaxBitRateForUpExt )
            {
                return APS_QOS_NOT_UP_TO_MIN;
            }
        }
    }
    #endif

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForMaxBitRateForDown(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    #if(PS_UE_REL_VER < PS_PTL_VER_R5)
    if ( 0 != pUeMinQos->ucMaxBitRateForDown )
    {
        if(pNetNegotQos->ucMaxBitRateForDown < pUeMinQos->ucMaxBitRateForDown)
        {
            return APS_QOS_NOT_UP_TO_MIN;
        }
    }
    #else
    if ( 0 == pUeMinQos->ucMaxBitRateForDownExt )
    {
        if ( 0 == pNetNegotQos->ucMaxBitRateForDownExt )
        {
            if ( 0 != pUeMinQos->ucMaxBitRateForDown )
            {
                if(pNetNegotQos->ucMaxBitRateForDown < pUeMinQos->ucMaxBitRateForDown)
                {
                    return APS_QOS_NOT_UP_TO_MIN;
                }
            }
        }
        else
        {
            /* 协商的Qos的最大下行比特率必然高于pUeMinQos的最大下行比特率 */
        }
    }
    else
    {
        if ( 0 == pNetNegotQos->ucMaxBitRateForDownExt )
        {
            /* 此种情况下，协商的Qos的最大下行比特率必然低于pUeMinQos的最大下行比特率 */
            return APS_QOS_NOT_UP_TO_MIN;
        }
        else
        {
            if ( pNetNegotQos->ucMaxBitRateForDownExt < pUeMinQos->ucMaxBitRateForDownExt )
            {
                return APS_QOS_NOT_UP_TO_MIN;
            }
        }
    }
    #endif

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForResidualBer(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ( 0 != pUeMinQos->ucResidualBer )
    {
        if(pNetNegotQos->ucResidualBer < pUeMinQos->ucResidualBer)
        {
            return APS_QOS_NOT_UP_TO_MIN;
        }
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForGuarantBitRateForUp(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ((APS_QOS_TRAFFIC_CLASS_INTERACTIVE != pNetNegotQos->ucTrafficClass)
         && (APS_QOS_TRAFFIC_CLASS_BACKGROUND != pNetNegotQos->ucTrafficClass)
         && (APS_QOS_MAX_BITRATE_0KBPS != pNetNegotQos->ucMaxBitRateForUp))
    {
        #if(PS_UE_REL_VER < PS_PTL_VER_R7)
        if ( 0 != pUeMinQos->ucGuarantBitRateForUp )
        {
            if(pNetNegotQos->ucGuarantBitRateForUp < pUeMinQos->ucGuarantBitRateForUp)
            {
                return APS_QOS_NOT_UP_TO_MIN;
            }
        }
        #else
        if ( 0 == pUeMinQos->ucGuarantBitRateForUpExt )
        {
            if ( 0 == pNetNegotQos->ucGuarantBitRateForUpExt )
            {
                if ( 0 != pUeMinQos->ucGuarantBitRateForUp )
                {
                    if(pNetNegotQos->ucGuarantBitRateForUp < pUeMinQos->ucGuarantBitRateForUp)
                    {
                        return APS_QOS_NOT_UP_TO_MIN;
                    }
                }
            }
            else
            {
                /* 协商的Qos的可保证上行比特率必然高于pUeMinQos的可保证上行比特率 */
            }
        }
        else
        {
            if ( 0 == pNetNegotQos->ucGuarantBitRateForUpExt )
            {
                /* 此种情况下，协商的Qos的可保证上行比特率必然低于pUeMinQos的可保证上行比特率 */
                return APS_QOS_NOT_UP_TO_MIN;
            }
            else
            {
                if ( pNetNegotQos->ucGuarantBitRateForUpExt < pUeMinQos->ucGuarantBitRateForUpExt )
                {
                    return APS_QOS_NOT_UP_TO_MIN;
                }
            }
        }
        #endif
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos_ForGuarantBitRateForDown(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos
)
{
    if ((APS_QOS_TRAFFIC_CLASS_INTERACTIVE != pNetNegotQos->ucTrafficClass)
         && (APS_QOS_TRAFFIC_CLASS_BACKGROUND != pNetNegotQos->ucTrafficClass)
         && (APS_QOS_MAX_BITRATE_0KBPS != pNetNegotQos->ucMaxBitRateForDown))
    {
        #if(PS_UE_REL_VER < PS_PTL_VER_R5)
        if ( 0 != pUeMinQos->ucGuarantBitRateForDown)
        {
            if(pNetNegotQos->ucGuarantBitRateForDown < pUeMinQos->ucGuarantBitRateForDown)
            {
                return APS_QOS_NOT_UP_TO_MIN;
            }
        }
        #else
        if ( 0 == pUeMinQos->ucGuarantBitRateForDownExt )
        {
            if ( 0 == pNetNegotQos->ucGuarantBitRateForDownExt )
            {
                if ( 0 != pUeMinQos->ucGuarantBitRateForDown )
                {
                    if(pNetNegotQos->ucGuarantBitRateForDown < pUeMinQos->ucGuarantBitRateForDown)
                    {
                        return APS_QOS_NOT_UP_TO_MIN;
                    }
                }
            }
            else
            {
                /* 协商的Qos的可保证下行比特率必然高于pUeMinQos的可保证下行比特率 */
            }
        }
        else
        {
            if ( 0 == pNetNegotQos->ucGuarantBitRateForDownExt )
            {
                /* 此种情况下，协商的Qos的可保证下行比特率必然低于pUeMinQos的可保证下行比特率 */
                return APS_QOS_NOT_UP_TO_MIN;
            }
            else
            {
                if ( pNetNegotQos->ucGuarantBitRateForDownExt < pUeMinQos->ucGuarantBitRateForDownExt )
                {
                    return APS_QOS_NOT_UP_TO_MIN;
                }
            }
        }
        #endif
    }

    return APS_QOS_UP_TO_MIN;
}


VOS_UINT32 Aps_CompareQos(
    APS_PDP_QOS_STRU                    *pNetNegotQos,
    APS_PDP_QOS_STRU                    *pUeMinQos,
    VOS_UINT8                           *pucQosOkOrNot
)
{
    VOS_UINT32  ulCmpResult;

    *pucQosOkOrNot = APS_QOS_NOT_UP_TO_MIN;

    /* 比较pNetNegotQos 和 pUeMinQos所指向的以下各项:*/
    /* ucDelayClass */
    ulCmpResult = Aps_CompareQos_ForDelayClass(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucReliabClass */
    ulCmpResult = Aps_CompareQos_ForReliabClass(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucPeakThrough */
    ulCmpResult = Aps_CompareQos_ForPeakThrough(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /*Precedence class*/
    ulCmpResult = Aps_CompareQos_ForPrecedenceClass(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* Mean throughput  */
    /* 都是31
    if (  )
    {
        if(pNetNegotQos->ucMeanThrough < pUeMinQos->ucMeanThrough)
        {
            *pucQosOkOrNot = APS_QOS_NOT_UP_TO_MIN;
            return(APS_SUCCESS);
        }
    }
    */

    /* ucTrafficClass */
    ulCmpResult = Aps_CompareQos_ForTrafficClass(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucDeliverOrder */
    ulCmpResult = Aps_CompareQos_ForDeliverOrder(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucDeliverOfErrSdu */
    ulCmpResult = Aps_CompareQos_ForDeliverOfErrSdu(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucMaximSduSize */
    ulCmpResult = Aps_CompareQos_ForMaximSduSize(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucMaxBitRateForUp */
    ulCmpResult = Aps_CompareQos_ForMaxBitRateForUp(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucMaxBitRateForDown */
    ulCmpResult = Aps_CompareQos_ForMaxBitRateForDown(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucResidualBer */
    ulCmpResult = Aps_CompareQos_ForResidualBer(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucSduErrRatio,注意 7代表的错SDU率最高 */
    ulCmpResult = Aps_CompareQos_ForSduErrRatio(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucTransDelay */
    /* 24.008:The Transfer delay value is ignored if the Traffic Class is Interactive class or Background class */
    ulCmpResult = Aps_CompareQos_ForTransDelay(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /*  ucTrafficHandlePriority */
    /* 24.008: The Traffic handling priority value is ignored if the Traffic
       Class is Conversational class, Streaming class or Background class */
    ulCmpResult = Aps_CompareQos_ForTrafficHandlePriority(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucGuarantBitRateForUp */
    /* 24.008: The Guaranteed bit rate for uplink value is ignored if the Traffic Class
        is Interactive class or Background class, or Maximum bit rate for uplink
        is set to 0 kbps. */
    ulCmpResult = Aps_CompareQos_ForGuarantBitRateForUp(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* ucGuarantBitRateForDown */
    /* 24.008: The Guaranteed bit rate for downlink value is ignored if the Traffic Class
       is Interactive class or Background class, or Maximum bit rate for downlink
       is set to 0 kbps. */
    ulCmpResult = Aps_CompareQos_ForGuarantBitRateForDown(pNetNegotQos, pUeMinQos);
    if (APS_QOS_NOT_UP_TO_MIN == ulCmpResult)
    {
        return(APS_SUCCESS);
    }

    /* 满足MINQOS要求, 则置标志 */
    *pucQosOkOrNot = APS_QOS_UP_TO_MIN;

    return(APS_SUCCESS);
}/*end of Function */

/*lint +e958*/

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif
