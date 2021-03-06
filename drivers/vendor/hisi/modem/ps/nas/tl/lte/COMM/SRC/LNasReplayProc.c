/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : LNasReplayProc.c
  Description     : LNAS回放信息的处理
  History           :
  1.Name+ID       yyyy-mm-dd  Draft Enact

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "NasLmmCore.h"
#include  "LNasReplayProc.h"
#include  "NasEmmPubUGlobal.h"
#include  "NasLmmPubMFsm.h"
#include  "NasEmmAttDetInclude.h"
#include  "NasEmmTauSerInclude.h"
/*#include  "NasEmmMrrc.h"*/
#include  "NasEmmSecuInclude.h"
#include  "NasEsmInclude.h"
#include  "NasERabmPublic.h"
#include  "NasERabmMain.h"
#include  "NasLmmPubMOsa.h"

/*lint -e767*/
#define    THIS_FILE_ID            PS_FILE_ID_LNASREPLAYPROC_C
#define    THIS_NAS_FILE_ID        NAS_FILE_ID_LNASREPLAYPROC_C
/*lint +e767*/

/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
#if(VOS_WIN32 == VOS_OS_VER)
/*在单板环境暂不导出导入*/

NAS_MML_PS_BEARER_CONTEXT_STRU      g_stPsBearCtx[NAS_MML_MAX_PS_BEARER_NUM];
VOS_UINT8                           g_aucTmsi[4];
NAS_MML_EMERGENCY_NUM_LIST_STRU     g_stEmcNumList;

NAS_REPLAY_CTX_INFO_STRU g_astNasEmmReplay[] =
{
    {(VOS_UINT8*)&g_stEmmInfo, sizeof(g_stEmmInfo), VOS_TRUE, NAS_REPLAY_ExportEmmInfo, NAS_REPLAY_ImportEmmInfo},
    {(VOS_UINT8*)&g_stMmMainContext, sizeof(g_stMmMainContext), VOS_TRUE, NAS_REPLAY_ExportMmMainContext, NAS_REPLAY_ImportMmMainContex},
    {(VOS_UINT8*)&g_stEmmEsmMsgBuf, sizeof(g_stEmmEsmMsgBuf), VOS_TRUE, NAS_REPLAY_ExportEmmEsmMsgBuf, NAS_REPLAY_ImportEmmEsmMsgBuf},
    {(VOS_UINT8*)&g_stEmmAttCtrl, sizeof(g_stEmmAttCtrl), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_stEmmDetCtrl, sizeof(g_stEmmDetCtrl), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_NasEmmSecuControl, sizeof(g_NasEmmSecuControl), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_stEmmSERCtrlTbl, sizeof(g_stEmmSERCtrlTbl), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_stEmmTAUCtrlTbl, sizeof(g_stEmmTAUCtrlTbl), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_stEmmcGlobalCtrl, sizeof(g_stEmmcGlobalCtrl), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_ulSmcControl, sizeof(g_ulSmcControl), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_ulUeSecuCapMatch, sizeof(g_ulUeSecuCapMatch), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_stEmmRegStateBarInfo, sizeof(g_stEmmRegStateBarInfo), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_stEmmMrrcCtrl, sizeof(g_stEmmMrrcCtrl), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_stEmmMrrcMgmtData, sizeof(g_stEmmMrrcMgmtData), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},


};

VOS_UINT32 g_ulNasEmmReplayNum
            = sizeof(g_astNasEmmReplay)/sizeof(NAS_REPLAY_CTX_INFO_STRU);


NAS_REPLAY_CTX_INFO_STRU g_astNasEsmReplay[] =
{
    {(VOS_UINT8*)&g_stNasEsmEntity, sizeof(g_stNasEsmEntity), VOS_TRUE, NAS_REPLAY_ExportEsmEntity, NAS_Replay_ImportEsmEntity},
    {(VOS_UINT8*)&g_ulPcoFlag, sizeof(VOS_UINT32), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_ulDelSdfWithQosFlag, sizeof(VOS_UINT32), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_ulPcscfFlag, sizeof(VOS_UINT32), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_ulBcmFlag, sizeof(VOS_UINT32), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    /*{(VOS_UINT8*)&g_enIpv6DnsFlag, sizeof(VOS_UINT32), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},*/

};

VOS_UINT32 g_ulNasEsmReplayNum
            = sizeof(g_astNasEsmReplay)/sizeof(NAS_REPLAY_CTX_INFO_STRU);


NAS_REPLAY_CTX_INFO_STRU g_astNasErabmReplay[] =
{
    {(VOS_UINT8*)&g_stNasERabmEntity, sizeof(g_stNasERabmEntity), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)g_aucERabmIpData, sizeof(VOS_UINT8)* NAS_ERABM_IP_HEADER_BUFFER_LEN, VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_enERabmSrState, sizeof(g_enERabmSrState), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
    {(VOS_UINT8*)&g_enERabmUlDataPending, sizeof(g_enERabmUlDataPending), VOS_TRUE, VOS_NULL_PTR, VOS_NULL_PTR},
};

VOS_UINT32 g_ulNasErabmReplayNum
            = sizeof(g_astNasErabmReplay)/sizeof(NAS_REPLAY_CTX_INFO_STRU);

/*  同步接口导入表 */
LNAS_REPLAY_API_INFO_IMPORT_STRU g_LNasReplayApiImportTab[] =
{
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_PUB_INFO_IND,  NAS_REPLAY_SavePubInfo},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_GU_INFO_IND,    NAS_REPLAY_SaveGuInfo},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_MS_NETWORK_CAPACILITY_INFO_IND,        NAS_REPLAY_SaveMsNetCap},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_UEIDTMSI_IND,       NAS_REPLAY_SaveUeIdTmsi},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_CS_SERVICE_FLAG_INFO_IND,  NAS_REPLAY_SaveLteCsSrvCfg},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_TIN_INFO_IND,  NAS_REPLAY_SaveTinType},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_ADD_UPDATE_RSLT_INFO_IND,  NAS_REPLAY_SaveAddUpdateRslt},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_PS_BEAR_CTX_INFO_IND,  NAS_REPLAY_SavePsBearCtx},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_PSBER_EXIST_BEF_ISRACT_INFO_IND,  NAS_REPLAY_SavePsBearerExistBeforeIsrAct},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_BEAR_ISR_ACT_AFTER_INFO_IND,  NAS_REPLAY_SavePsBearerAfterIsrActExist},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_EMERGENCY_NUMLIST_INFO_IND,  NAS_REPLAY_SaveEmcNumList},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_PSREG_CONTAINDRX_INFO_IND,  NAS_REPLAY_SavePsRegContainDrxPara},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_EUTRAN_PS_DRX_LEN_INFO_IND,  NAS_REPLAY_SaveUeEutranPsDrxLen},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_DRX_TIMER_INFO_IND,  NAS_REPLAY_SaveNonDrxTimer},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_SPLIT_PG_CYCLE_CODE_INFO_IND,  NAS_REPLAY_SaveSplitPgCycleCode},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_SPLIT_ON_CCCH_INFO_IND,  NAS_REPLAY_SaveSplitOnCcch},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_CLASSMARK2_INFO_IND,  NAS_REPLAY_SaveMsClassMark2},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_CS_ATTACH_ALLOW_FLG_INFO_IND,  NAS_REPLAY_SaveCsAttachAllowFlg},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_PS_ATTACH_ALLOW_FLG_INFO_IND,  NAS_REPLAY_SavePsAttachAllowFlg},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_MS_MODE_INFO_IND,  NAS_REPLAY_SaveMsMode},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_PLMN_IS_FORBID_INFO_IND,  NAS_REPLAY_SavePlmnIdIsForbid},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_PLMN_EXAXTLY_CMP_INFO_IND,  NAS_REPLAY_SavePlmnExactlyCmpFlg},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_PS_REG_STATUS_INFO_IND,  NAS_REPLAY_SavePsRegStatus},
    {WUEPS_PID_MMC,   PS_PID_MM,  NAS_OM_LOG_SIM_TYPE_INFO_IND,  NAS_REPLAY_SaveSimType},
    {PS_PID_MM,   PS_PID_MM,  NAS_OM_LOG_BSP_KDF_KEY_MAKE_INFO_IND,  NAS_REPLAY_SaveKdfKey},
    {PS_PID_ESM,   PS_PID_ESM,  NAS_OM_LOG_SDF_PARA_INFO_IND,  NAS_REPLAY_SaveSdfPara},
    {PS_PID_ESM,   PS_PID_ESM,  NAS_OM_LOG_PDP_MANAGE_INFO_IND,  NAS_REPLAY_SavePdpManageInfo},
#if 0
    {PS_PID_MM,   PS_PID_MM,  NAS_OM_LOG_IS_DATA_PENDING,  NAS_REPLAY_SaveErabmUpDataPending},
    {PS_PID_ESM,   PS_PID_ESM,  NAS_OM_LOG_LTE_RAT_IS_EXIST_IND,  NAS_REPLAY_SaveLteRatIsExist},
    {PS_PID_MM,   PS_PID_MM,  NAS_OM_LOG_ISUP_SIG_PENDING_IND,  NAS_REPLAY_SaveUpSignalPending},
#endif
    {WUEPS_PID_USIM,   PS_PID_MM,  NAS_OM_LOG_CARD_IMSI_INFO_IND,  NAS_REPLAY_SaveCardImsi},
    {WUEPS_PID_USIM,   PS_PID_MM,  NAS_OM_LOG_USIMM_SERVICE_INFO_IND,  NAS_REPLAY_SaveServiceAvailable},
    {WUEPS_PID_USIM,   PS_PID_MM,  NAS_OM_LOG_USIMM_TEST_MODE_INFO_IND,  NAS_REPLAY_SaveUsimTestMode},
};


/*Nas_GetPubInfo API导入时的全局变量*/
NAS_LMM_REPLAY_PUB_INFO_STRU            g_astPubInfo[NAS_REPLAY_MAX_PUB_INFO];
VOS_UINT32                              g_ulPubInfoCount = 0;
VOS_UINT32                              g_ulPubInfoIndex = 0;

/*Nas_GetGuInfo API导入时的全局变量*/
NAS_LMM_REPLAY_GU_INFO_STRU             g_astGuInfo[NAS_REPLAY_MAX_GU_INFO];
VOS_UINT32                              g_ulGuInfoCount = 0;
VOS_UINT32                              g_ulGuInfoIndex = 0;

/*NAS_MML_GetMsNetworkCapability API导入时的全局变量*/
NAS_MML_MS_NETWORK_CAPACILITY_STRU g_astMsNetworkCapability[NAS_REPLAY_MAX_MS_NET_CAP];
VOS_UINT32               g_ulMsNetCapCount = 0;
VOS_UINT32               g_ulMsNetCapIndex = 0;


/*NAS_MML_GetUeIdTmsi API导入时的全局变量*/
NAS_REPLAY_UEID_TMSI_INFO_STRU          g_astUeIdTmsiInfo[NAS_REPLAY_MAX_UEID_TMSI_INFO];
VOS_UINT32                              g_ulUeIdTmsiInfoCount = 0;
VOS_UINT32                              g_ulUeIdTmsiInfoIndex = 0;

/*NAS_MML_GetLteCsServiceCfg API导入时的全局变量*/
NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8   g_astLteCsSrvCfg [NAS_REPLAY_MAX_LTE_CS_SRV_CFG];
VOS_UINT32                              g_ulLteCsSrvCfgCount = 0;
VOS_UINT32                              g_ulLteCsSrvCfgIndex = 0;

/*NAS_MML_GetTinType API导入时的全局变量*/
MMC_LMM_TIN_TYPE_ENUM_UINT32            g_astTinType[NAS_REPLAY_MAX_TIN_TYPE];
VOS_UINT32                              g_ulTinTypeCount;
VOS_UINT32                              g_ulTinTypeIndex;

/*NAS_MML_GetAdditionUpdateRslt API导入时的全局变量*/
NAS_LMM_ADDITIONAL_UPDATE_RSLT_ENUM_UINT32 g_astAddUpdateRslt[NAS_REPLAY_MAX_ADD_UPDATE_RSLT];
VOS_UINT32                              g_ulAddUpdateRsltCount;
VOS_UINT32                              g_ulAddUpdateRsltIndex;

/*NAS_MML_GetAdditionUpdateRslt API导入时的全局变量*/
NAS_REPLAY_PS_BEARER_CONTEXT_INFO_STRU g_astPsBearCtx[NAS_REPLAY_MAX_PS_BEARER_CTX];
VOS_UINT32               g_ulPsBearCtxCount;
VOS_UINT32               g_ulPsBearCtxIndex;

/*NAS_MML_IsPsBearerExistBeforeIsrAct API导入时的全局变量*/
VOS_UINT32                  g_astBefIsrAct[NAS_REPLAY_MAX_PS_BEARER_EXIST_BEFORE_ISR_ACT];
VOS_UINT32                  g_ulBeIsrActCount;
VOS_UINT32                  g_ulBefIsrActIndex;

/*NAS_MML_IsPsBearerAfterIsrActExist API导入时的全局变量*/
VOS_UINT32 g_astAfterIsrActExist[NAS_REPLAY_MAX_PS_BEARER_AFTER_ISR_ACT_EXIST];
VOS_UINT32               g_ulAfterIsrActExistCount;
VOS_UINT32               g_ulAfterIsrActExistIndex;

/*NAS_MML_GetEmergencyNumList API导入时的全局变量*/
NAS_REPLAY_EMERGENCY_NUMLISTS_INFO_STRU g_astEmcNumList[NAS_REPLAY_MAX_EMC_NUM_LIST];
VOS_UINT32               g_ulEmcNumListCount;
VOS_UINT32               g_ulEmcNumListIndex;

/*NAS_MML_GetPsRegContainDrx API导入时的全局变量*/
NAS_MML_PS_REG_CONTAIN_DRX_PARA_ENUM_UINT8 g_astPsRegContainDrxPara[NAS_REPLAY_MAX_PS_REG_CONTAIN_DRX_PARA];
VOS_UINT32               g_ulPsRegContainDrxParaCount;
VOS_UINT32               g_ulPsRegContainDrxParaIndex;

/*NAS_MML_GetUeEutranPsDrxLen API导入时的全局变量*/
VOS_UINT8  g_astUeEutranPsDrxLen[NAS_REPLAY_MAX_EUTRAN_DRX_LEN];
VOS_UINT32               g_ulUeEutranPsDrxLenCount;
VOS_UINT32               g_ulUeEutranPsDrxLenIndex;

/*NAS_MML_GetNonDrxTimer API导入时的全局变量*/
VOS_UINT8  g_astNonDrxTimer[NAS_REPLAY_MAX_NON_DRX_TIMER];
VOS_UINT32               g_ulNonDrxTimerCount;
VOS_UINT32               g_ulNonDrxTimerIndex;

/*NAS_MML_GetSplitPgCycleCode API导入时的全局变量*/
VOS_UINT8  g_astSplitPgCycleCode[NAS_REPLAY_MAX_SPLIT_PG_CYCLE_CODE];
VOS_UINT32               g_ulSplitPgCycleCodeCount;
VOS_UINT32               g_ulSplitPgCycleCodeIndex;

/*NAS_MML_GetSplitOnCcch API导入时的全局变量*/
VOS_UINT8  g_astSplitOnCcch[NAS_REPLAY_MAX_SPLIT_ON_CCCH];
VOS_UINT32               g_ulSplitOnCcchCount;
VOS_UINT32               g_ulSplitOnCcchIndex;

/*NAS_MML_Fill_IE_ClassMark2 API导入时的全局变量*/
NAS_REPLAY_MS_CLASSMARK2_STRU g_astMsClassMark2[NAS_REPLAY_MAX_MS_CLASSMARK2];
VOS_UINT32               g_ulMsClassMark2Count;
VOS_UINT32               g_ulMsClassMark2Index;

/*NAS_MML_GetCsAttachAllowFlg API导入时的全局变量*/
VOS_UINT8  g_astCsAttachAllowFlg[NAS_REPLAY_MAX_CS_ATTACH_ALLOW_FLG];
VOS_UINT32               g_ulCsAttachAllowFlgCount;
VOS_UINT32               g_ulCsAttachAllowFlgIndex;

/*NAS_MML_GetPsAttachAllowFlg API导入时的全局变量*/
VOS_UINT8  g_astPsAttachAllowFlg[NAS_REPLAY_MAX_PS_ATTACH_ALLOW_FLG];
VOS_UINT32               g_ulPsAttachAllowFlgCount;
VOS_UINT32               g_ulPsAttachAllowFlgIndex;

/*NAS_MML_GetMsMode API导入时的全局变量*/
VOS_UINT8  g_astMsMode[NAS_REPLAY_MAX_MS_MODE];
VOS_UINT32               g_ulMsModeCount;
VOS_UINT32               g_ulMsModeIndex;

/*Nas_PlmnIdIsForbid API导入时的全局变量*/
NAS_REPLAY_PLMNID_IS_FORBID_STRU g_astPlmnIdIsForbid[NAS_REPLAY_MAX_PLMNID_IS_FORBID];
VOS_UINT32               g_ulPlmnIdIsForbidCount;
VOS_UINT32               g_ulPlmnIdIsForbidIndex;

/*NAS_MML_GetPlmnExactlyComparaFlg API导入时的全局变量*/
VOS_UINT8  g_astPlmnExactlyCmpFlg[NAS_REPLAY_MAX_PLMN_EXACTLY_CMP_FLG];
VOS_UINT32               g_ulPlmnExactlyCmpFlgCount;
VOS_UINT32               g_ulPlmnExactlyCmpFlgIndex;

/*NAS_MML_GetPsRegStatus API导入时的全局变量*/
VOS_UINT8  g_astPsRegStatus[NAS_REPLAY_MAX_PS_REG_STATUS];
VOS_UINT32               g_ulPsRegStatusCount;
VOS_UINT32               g_ulPsRegStatusIndex;

/*NAS_MML_GetSimType API导入时的全局变量*/
NAS_MML_SIM_TYPE_ENUM_UINT8 g_astSimType[NAS_REPLAY_MAX_SIM_TYPE];
VOS_UINT32               g_ulSimTypeCount;
VOS_UINT32               g_ulSimTypeIndex;

/*BSP_KDF_KeyMakeBSP_KDF_KeyMake API导入时的全局变量*/
NAS_REPLAY_KDF_KEY_STRU g_astKdfKey[NAS_REPLAY_MAX_KDF_KEY];
VOS_UINT32               g_ulKdfKeyCount;
VOS_UINT32               g_ulKdfKeyIndex;

/*APP_GetSdfPara API导入时的全局变量*/
NAS_REPLAY_SDF_PARA_STRU g_astSdfPara[NAS_REPLAY_MAX_SDF_PARA];
VOS_UINT32               g_ulSdfParaCount;
VOS_UINT32               g_ulSdfParaIndex;

/*APP_GetSdfPara API导入时的全局变量*/
NAS_REPLAY_PDP_MANAGE_INFO_STRU g_astPdpManageInfo[NAS_REPLAY_MAX_PDP_MANAGE_INFO];
VOS_UINT32               g_ulPdpManageInfoCount;
VOS_UINT32               g_ulPdpManageInfoIndex;
#if 0
/*NAS_ERABM_IsDataPending API导入时的全局变量*/
EMM_ERABM_UP_DATA_PENDING_ENUM_UINT32 g_astErabmUpDataPending[NAS_REPLAY_MAX_ERABM_UP_DATA_PENDING];
VOS_UINT32               g_ulErabmUpDataPendingCount;
VOS_UINT32               g_ulErabmUpDataPendingIndex;

/*NAS_EMM_lteRatIsExist API导入时的全局变量*/
VOS_UINT32  g_astLteRatIsExist[NAS_REPLAY_MAX_LTE_RAT_IS_EXIST];
VOS_UINT32               g_ulLteRatIsExistCount;
VOS_UINT32               g_ulLteRatIsExistIndex;

/*NAS_ESM_IsUpSingnalPending API导入时的全局变量*/
EMM_ESM_UP_SINGNAL_PENDING_ENUM_UINT32 g_astUpSignalPending[NAS_REPLAY_MAX_UP_SIGNAL_PENDING];
VOS_UINT32               g_ulUpSignalPendingCount;
VOS_UINT32               g_ulUpSignalPendingIndex;
#endif
/*USIMM_GetCardIMSI API导入时的全局变量*/
NAS_REPLAY_CARD_IMSI_INFO g_astCardImsi[NAS_REPLAY_MAX_CARD_IMSI];
VOS_UINT32               g_ulCardImsiCount;
VOS_UINT32               g_ulCardImsiIndex;

/*USIMM_IsServiceAvailable API导入时的全局变量*/
NAS_REPLAY_SERVICE_AVAILABLE_STRU g_astServiceAvailable[NAS_REPLAY_MAX_SERVICE_AVAILABLE];
VOS_UINT32               g_ulServiceAvailableCount;
VOS_UINT32               g_ulServiceAvailableIndex;

/*USIMM_IsTestCard API导入时的全局变量*/
VOS_UINT32  g_astUsimTestMode[NAS_REPLAY_MAX_USIM_TEST_MODE];
VOS_UINT32               g_ulUsimTestModeCount;
VOS_UINT32               g_ulUsimTestModeIndex;

#endif
/*****************************************************************************
  3 Function
*****************************************************************************/
#if 0

VOS_UINT32 NAS_UnCompressData(
    VOS_UINT8                          *pucDest,
    VOS_UINT32                         *pulDestLen,
    VOS_UINT8                          *pucSrc,
    VOS_UINT32                         *pulSrcLen
)
{
    VOS_UINT32                          ulRslt;

    /* 格式为,cmpsSize(32BITs) + data(length=cmpsSize) */

    /* 前四个字节为cmpsSize,获取cmpsSize */
    PS_MEM_CPY(pulSrcLen, pucSrc, sizeof(VOS_UINT32));
    pucSrc += sizeof(VOS_UINT32);

    /* 从第五个字节开始存放压缩后的数据 */
    ulRslt  = (VOS_UINT32)_uncompress(pucDest, pulDestLen,
                                      pucSrc, *pulSrcLen);
    if (VOS_OK != ulRslt)
    {
         NAS_LMM_PUBM_LOG1_ERR("NAS_MML_UnCompressData: ulRslt",
                         ulRslt);

        return VOS_FALSE;
    }

    /* 补充4个cmpsSize空间 */
    *pulSrcLen += sizeof(VOS_UINT32);

    return VOS_TRUE;
}


VOS_UINT32  NAS_CompressData(
    VOS_UINT8                          *pucDest,
    VOS_UINT32                         *pulDestLen,
    VOS_UINT8                          *pucSrc,
    VOS_UINT32                          ulSrcLen
)
{
    VOS_UINT32                          ulRslt;

    /*格式为,cmpsSize(32BITs) + data(length=cmpsSize)*/
    if ((*pulDestLen) < sizeof(VOS_UINT32))
    {
         NAS_LMM_PUBM_LOG_ERR("NAS_MML_CompressData: 剩余空间不足");
        return VOS_FALSE;
    }

    /*预留前四个字节，从第五个字节开始存放压缩后的数据*/
    (*pulDestLen) -= sizeof(VOS_UINT32);

    ulRslt         = (VOS_UINT32)compress(pucDest + sizeof(VOS_UINT32), pulDestLen,
                                   pucSrc, ulSrcLen);

    if( VOS_OK != ulRslt )
    {
         NAS_LMM_PUBM_LOG1_ERR("NAS_MML_CompressData: compress", ulRslt);

        return VOS_FALSE;
    }

    /*在预留的前四个字节上，填充cmpsSize*/
    PS_MEM_CPY( pucDest, pulDestLen, sizeof(VOS_UINT32));

    /*补充4个字节长度*/
    (*pulDestLen) += sizeof(VOS_UINT32);

    return VOS_TRUE;
}
#endif

#if(VOS_WIN32 == VOS_OS_VER)
/*在单板环境暂不导出导入*/

VOS_UINT32 NAS_Replay_SaveCntxInfo
(
    VOS_UINT8                           *pucDestBufAddr, /* 压缩后的数据 */
    VOS_UINT32                          ulDestBufLen,
    NAS_REPLAY_CTX_INFO_STRU            *pstArrayAddr,
    VOS_UINT32                          ulArrayCnt,
    VOS_UINT32                          *pulCmpDatalen
)
{
    VOS_UINT32                         i;
    VOS_UINT8                          *pucEndBuf;
    VOS_UINT32                         ulCmLen;
    VOS_UINT32                         ulRslt;

    *pulCmpDatalen = 0;
    ulCmLen        = 0;

    /* 获取buf结尾地址，用于计算剩余空间*/
    pucEndBuf = (VOS_UINT8 *)pucDestBufAddr + ulDestBufLen;


    /* 导出全局变量,
       数据结构: 结构1数据长度|结构1数据|结构2数据长度|结构2数据|...|结构n数据长度|结构n数据|*/

    for (i = 0; i < ulArrayCnt; i++)
    {
        ulRslt = NAS_PCREPLAY_EXPORT_VAR_CONTINUE;
        ulCmLen = 0;

        if( VOS_NULL_PTR != pstArrayAddr[i].pExportFunc )
        {
            ulRslt = pstArrayAddr[i].pExportFunc(pucDestBufAddr,pucEndBuf,(VOS_UINT8 *)&pstArrayAddr[i],&ulCmLen);
        }

        if(NAS_PCREPLAY_EXPORT_VAR_CONTINUE == ulRslt)
        {
            pucDestBufAddr += ulCmLen;
            *pulCmpDatalen += ulCmLen;
            ulRslt = NAS_Replay_ExportVarible(pucDestBufAddr,pucEndBuf,&pstArrayAddr[i],&ulCmLen);
        }

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
            return VOS_FALSE;
        }

        *pulCmpDatalen += ulCmLen;
        pucDestBufAddr += ulCmLen;
    }

    return VOS_TRUE;
}

VOS_UINT32 NAS_Replay_ExportVarible
(
    VOS_UINT8                           *pucDest, /* 压缩后的数据 */
    const VOS_UINT8                           *pucDestEnd,
    NAS_REPLAY_CTX_INFO_STRU            *pstVar,
    VOS_UINT32                          *pulDatalen
)
{
    VOS_UINT32                          ulLen = NAS_LMM_NULL;
    VOS_UINT32                          ulRslt = VOS_FALSE;
    VOS_UINT32                          ulBufLen = NAS_LMM_NULL;

    (void)ulRslt;
    (void)ulBufLen;
    (void)pstVar;

    ulBufLen = (VOS_UINT32)(pucDestEnd - pucDest);

    #if(VOS_WIN32 == VOS_OS_VER)
    /*PC上测试数据不压缩*/
    if ((VOS_UINT32)(pucDestEnd - pucDest + 1) < (pstVar->ulItemLen + sizeof(VOS_UINT32)))
    {
        *pulDatalen = 0;
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    /*  不压缩数据的长度 */
    PS_MEM_CPY(pucDest, &pstVar->ulItemLen, sizeof(VOS_UINT32));
    pucDest += sizeof(VOS_UINT32);
    ulLen        = sizeof(VOS_UINT32);

    /* 不压缩数据内容 */
    PS_MEM_CPY(pucDest, pstVar->pucItemAddr, pstVar->ulItemLen);
    ulLen   += pstVar->ulItemLen;

    #else
    /*单板测试数据需压缩*/
    #if 0
    if (VOS_TRUE == pstVar->ulCompressFlg)
    {

        ulRslt = NAS_CompressData(pucDest, &ulBufLen , pstVar->pucItemAddr , pstVar->ulItemLen);
        if (VOS_FALSE == ulRslt)
        {
            *pulDatalen = 0;
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }

        ulLen = ulBufLen;
    }
    else
    {

        if ((VOS_UINT32)(pucDestEnd - pucDest + 1) < (pstVar->ulItemLen + sizeof(VOS_UINT32)))
        {
            *pulDatalen = 0;
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }

        /*  不压缩数据的长度 */
        PS_MEM_CPY(pucDest, &pstVar->ulItemLen, sizeof(VOS_UINT32));
        pucDest += sizeof(VOS_UINT32);
        ulLen        = sizeof(VOS_UINT32);

        /* 不压缩数据内容 */
        PS_MEM_CPY(pucDest, pstVar->pucItemAddr, pstVar->ulItemLen);
        ulLen   += pstVar->ulItemLen;
    }
    #endif
    #endif
    *pulDatalen = ulLen;

    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}
VOS_UINT32  NAS_EMM_ExportContextData(VOS_VOID)
{
    NAS_PC_REPLAY_RUNNING_CONTEXT_STRU  *pstReplayCtx = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulCmpDatalen = NAS_EMM_NULL;

    /*申请内存*/
    pstReplayCtx = (VOS_VOID *)NAS_LMM_MEM_ALLOC(NAS_PC_REPLAY_COMPRESS_DATA_LEN
                                                + sizeof(NAS_PC_REPLAY_RUNNING_CONTEXT_STRU)
                                                - NAS_EMM_4BYTES_LEN);

    /*判断申请结果，若失败退出*/
    if (NAS_EMM_NULL_PTR == pstReplayCtx)
    {
        return VOS_FALSE;
    }

    ulRslt = NAS_Replay_SaveCntxInfo(pstReplayCtx->aucData,
                                    NAS_PC_REPLAY_COMPRESS_DATA_LEN,
                                    g_astNasEmmReplay,
                                    g_ulNasEmmReplayNum,
                                    &ulCmpDatalen);

    if (VOS_FALSE == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_ExportContextData: DATA BUFF is failure!");
        NAS_LMM_MEM_FREE(pstReplayCtx);
        return VOS_FALSE;
    }

    pstReplayCtx->ulReceiverPid      = PS_PID_MM;
    pstReplayCtx->ulSenderPid        = PS_PID_MM;
    pstReplayCtx->ulSenderCpuId      = VOS_LOCAL_CPUID;
    pstReplayCtx->ulReceiverCpuId    = VOS_LOCAL_CPUID;
    pstReplayCtx->ulLength           = ulCmpDatalen;
    pstReplayCtx->ulMsgName          = NAS_EMM_REPLAY_CONTEXT;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstReplayCtx);

    NAS_LMM_MEM_FREE(pstReplayCtx);

    return ulRslt;
}
VOS_UINT32  NAS_ESM_ExportContextData(VOS_VOID)
{
    NAS_PC_REPLAY_RUNNING_CONTEXT_STRU  *pstReplayCtx = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulCmpDatalen = NAS_EMM_NULL;

    /*申请内存*/
    pstReplayCtx = (VOS_VOID *)NAS_LMM_MEM_ALLOC(NAS_PC_REPLAY_COMPRESS_DATA_LEN
                                                + sizeof(NAS_PC_REPLAY_RUNNING_CONTEXT_STRU)
                                                - NAS_EMM_4BYTES_LEN);

    /*判断申请结果，若失败退出*/
    if (NAS_EMM_NULL_PTR == pstReplayCtx)
    {
        return VOS_FALSE;
    }

    ulRslt = NAS_Replay_SaveCntxInfo(pstReplayCtx->aucData,
                                    NAS_PC_REPLAY_COMPRESS_DATA_LEN,
                                    g_astNasEsmReplay,
                                    g_ulNasEsmReplayNum,
                                    &ulCmpDatalen);

    if (VOS_FALSE == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_ESM_ExportContextData: DATA BUFF is failure!");
        NAS_LMM_MEM_FREE(pstReplayCtx);
        return VOS_FALSE;
    }

    pstReplayCtx->ulReceiverPid      = PS_PID_ESM;
    pstReplayCtx->ulSenderPid        = PS_PID_ESM;
    pstReplayCtx->ulSenderCpuId      = VOS_LOCAL_CPUID;
    pstReplayCtx->ulReceiverCpuId    = VOS_LOCAL_CPUID;
    pstReplayCtx->ulLength           = ulCmpDatalen;
    pstReplayCtx->ulMsgName          = NAS_ESM_REPLAY_CONTEXT;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstReplayCtx);

    NAS_LMM_MEM_FREE(pstReplayCtx);

    return VOS_TRUE;
}
VOS_UINT32  NAS_ERABM_ExportContextData(VOS_VOID)
{
    NAS_PC_REPLAY_RUNNING_CONTEXT_STRU  *pstReplayCtx = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulCmpDatalen = NAS_EMM_NULL;

    /*申请内存*/
    pstReplayCtx = (VOS_VOID *)NAS_LMM_MEM_ALLOC(NAS_PC_REPLAY_COMPRESS_DATA_LEN
                                                + sizeof(NAS_PC_REPLAY_RUNNING_CONTEXT_STRU)
                                                - NAS_EMM_4BYTES_LEN);

    /*判断申请结果，若失败退出*/
    if (NAS_EMM_NULL_PTR == pstReplayCtx)
    {
        return VOS_FALSE;
    }

    ulRslt = NAS_Replay_SaveCntxInfo(pstReplayCtx->aucData,
                                    NAS_PC_REPLAY_COMPRESS_DATA_LEN,
                                    g_astNasErabmReplay,
                                    g_ulNasErabmReplayNum,
                                    &ulCmpDatalen);

    if (VOS_FALSE == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_ERABM_ExportContextData: DATA BUFF is failure!");
        NAS_LMM_MEM_FREE(pstReplayCtx);
        return VOS_FALSE;
    }

    pstReplayCtx->ulReceiverPid      = PS_PID_RABM;
    pstReplayCtx->ulSenderPid        = PS_PID_RABM;
    pstReplayCtx->ulSenderCpuId      = VOS_LOCAL_CPUID;
    pstReplayCtx->ulReceiverCpuId    = VOS_LOCAL_CPUID;
    pstReplayCtx->ulLength           = ulCmpDatalen;
    pstReplayCtx->ulMsgName          = NAS_ERABM_REPLAY_CONTEXT;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstReplayCtx);

    NAS_LMM_MEM_FREE(pstReplayCtx);

    return VOS_TRUE;
}
VOS_UINT32  NAS_REPLAY_ExportEmmInfo
(
    VOS_UINT8                           *pucDest,
    const VOS_UINT8                           *pucDestEnd,
    const VOS_UINT8                           *pucVar,
    VOS_UINT32                          *pulDatalen
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTempLen = NAS_LMM_NULL;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEmcMsgLen;

    ulRslt = NAS_Replay_ExportVarible(pucDest,
                                    pucDestEnd,
                                    (NAS_REPLAY_CTX_INFO_STRU*)pucVar,
                                    pulDatalen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEmmInfo: DATA BUFF is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    pucDest += *pulDatalen;
    ulTempLen += *pulDatalen;
    if (NAS_EMM_NULL_PTR == g_stEmmInfo.pucEsmMsg)
    {
         /*打印*/
        NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEmmInfo: Point is null!");
        NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
        pucDest += sizeof(VOS_UINT32);
        ulTempLen += sizeof(VOS_UINT32);
        *pulDatalen = ulTempLen;
        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    /*计算指针数据长度，填充变量导出入参*/
    stReplayCtx.pucItemAddr = g_stEmmInfo.pucEsmMsg;
    stReplayCtx.ulCompressFlg = VOS_FALSE;
    stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
    stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
    g_stEmmInfo.pucEsmMsg += sizeof(VOS_UINT32);
    g_stEmmInfo.pucEsmMsg += sizeof(VOS_UINT32);
    NAS_LMM_MEM_CPY(&ulEmcMsgLen,g_stEmmInfo.pucEsmMsg,sizeof(VOS_UINT32));

    stReplayCtx.ulItemLen = sizeof(VOS_UINT32)/*OPID 长度*/
                            + sizeof(VOS_UINT32)/*ulIsEmcType 长度*/
                            + sizeof(VOS_UINT32)/*ESM MSG 长度*/
                            + ulEmcMsgLen;

    /*指针数据不为空，导出指针数据内容*/
    ulRslt = NAS_Replay_ExportVarible(pucDest,
                                    pucDestEnd,
                                    &stReplayCtx,
                                    pulDatalen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEmmInfo: Point data buff is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    /*pucDest += *pulDatalen;*/
    ulTempLen += *pulDatalen;

    /*填充导出结构体变量和指针数据的总长度*/
    *pulDatalen = ulTempLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}


VOS_UINT32  NAS_REPLAY_ExportEmmEsmMsgBuf
(
    VOS_UINT8                           *pucDest,
    const VOS_UINT8                           *pucDestEnd,
    const VOS_UINT8                           *pucVar,
    VOS_UINT32                          *pulDatalen
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTempLen = NAS_LMM_NULL;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEsmMsgLen = NAS_LMM_NULL;
    VOS_UINT8                           *pstMsg = NAS_LMM_NULL_PTR;

    ulRslt = NAS_Replay_ExportVarible(pucDest,
                                    pucDestEnd,
                                    (NAS_REPLAY_CTX_INFO_STRU*)pucVar,
                                    pulDatalen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEmmEsmMsgBuf: DATA BUFF is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    pucDest += *pulDatalen;
    ulTempLen += *pulDatalen;

    if (NAS_LMM_NULL == g_stEmmEsmMsgBuf.ulEsmMsgCnt)
    {
         /*打印*/
        NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEmmEsmMsgBuf: NUM is null!");

        NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
        pucDest += sizeof(VOS_UINT32);
        ulTempLen += sizeof(VOS_UINT32);
        *pulDatalen = ulTempLen;

        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    for(i = 0; i < g_stEmmEsmMsgBuf.ulEsmMsgCnt; i++)
    {
        if (NAS_EMM_NULL_PTR == g_stEmmEsmMsgBuf.apucEsmMsgBuf[i])
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEmmEsmMsgBuf: Point is null!");
            NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
            pucDest += sizeof(VOS_UINT32);
            ulTempLen += sizeof(VOS_UINT32);
            *pulDatalen = ulTempLen;
            continue;
        }

        /*计算指针数据长度，填充变量导出入参*/
        stReplayCtx.pucItemAddr = g_stEmmEsmMsgBuf.apucEsmMsgBuf[i];
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        pstMsg = g_stEmmEsmMsgBuf.apucEsmMsgBuf[i];
        pstMsg += NAS_PCREPLAY_LEN_VOS_MSG_HEADER;

        NAS_LMM_MEM_CPY(&ulEsmMsgLen,pstMsg,sizeof(VOS_UINT32));

        stReplayCtx.ulItemLen = EMM_LEN_VOS_MSG_HEADER
                                + ulEsmMsgLen;

        /*指针数据不为空，导出指针数据内容*/
        ulRslt = NAS_Replay_ExportVarible(pucDest,
                                        pucDestEnd,
                                        &stReplayCtx,
                                        pulDatalen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEmmEsmMsgBuf: Point data buff is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }

        pucDest += *pulDatalen;
        ulTempLen += *pulDatalen;
    }

    /*填充导出结构体变量和指针数据的总长度*/
    *pulDatalen = ulTempLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}
VOS_UINT32  NAS_REPLAY_ExportEmmFsmBufHighQueue
(
    VOS_UINT8 *pucDest,
    const VOS_UINT8 *pucDestEnd,
    VOS_UINT32  *pulDatalen
)
{
    VOS_UINT32                           i;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTempLen = NAS_LMM_NULL;
    NAS_LMM_BUFF_MSG_STRU               *pstBuffMsg;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEmmMsgLen = NAS_LMM_NULL;
    VOS_UINT8                           *pstMsg = NAS_LMM_NULL_PTR;

    pstBuffMsg = NAS_LMM_GetEmmFsmBufQueueHighAddr();

    if (NAS_LMM_NULL == NAS_LMM_GetEmmFsmBufQueueHighCnt())
    {
         /*打印*/
        NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEmmFsmBufHighQueue: NUM is null!");

        NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
        pucDest += sizeof(VOS_UINT32);
        ulTempLen += sizeof(VOS_UINT32);
        *pulDatalen = ulTempLen;

        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    for (i = 0; i < NAS_LMM_GetEmmFsmBufQueueHighCnt(); i++)
    {
        if (NAS_EMM_NULL_PTR == pstBuffMsg[i].pBuffMsg)
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEmmFsmBufHighQueue: Point is null!");
            NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
            pucDest += sizeof(VOS_UINT32);
            ulTempLen += sizeof(VOS_UINT32);
            *pulDatalen = ulTempLen;
            continue;
        }

        /*计算指针数据长度，填充变量导出入参*/
        stReplayCtx.pucItemAddr = pstBuffMsg[i].pBuffMsg;
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        pstMsg = pstBuffMsg[i].pBuffMsg;
        pstMsg += NAS_PCREPLAY_LEN_VOS_MSG_HEADER;
        NAS_LMM_MEM_CPY(&ulEmmMsgLen,pstMsg,sizeof(VOS_UINT32));

        stReplayCtx.ulItemLen = EMM_LEN_VOS_MSG_HEADER
                                + ulEmmMsgLen;

        /*指针数据不为空，导出指针数据内容*/
        ulRslt = NAS_Replay_ExportVarible(pucDest,
                                        pucDestEnd,
                                        &stReplayCtx,
                                        pulDatalen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEmmFsmBufHighQueue: Point data buff is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }

        pucDest += *pulDatalen;
        ulTempLen += *pulDatalen;

    }

    /*填充导出结构体变量和指针数据的总长度*/
    *pulDatalen = ulTempLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}

VOS_UINT32  NAS_REPLAY_ExportEmmFsmBufMidQueue
(
    VOS_UINT8 *pucDest,
    const VOS_UINT8 *pucDestEnd,
    VOS_UINT32  *pulDatalen
)
{
    VOS_UINT32                           i;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTempLen = NAS_LMM_NULL;
    NAS_LMM_BUFF_MSG_STRU               *pstBuffMsg;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEmmMsgLen = NAS_LMM_NULL;
    VOS_UINT8                           *pstMsg = NAS_LMM_NULL_PTR;

    pstBuffMsg = NAS_LMM_GetEmmFsmBufQueueMidAddr();

    if (NAS_LMM_NULL == NAS_LMM_GetEmmFsmBufQueueMidCnt())
    {
         /*打印*/
        NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEmmFsmBufMidQueue: NUM is null!");
        NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
        pucDest += sizeof(VOS_UINT32);
        ulTempLen += sizeof(VOS_UINT32);
        *pulDatalen = ulTempLen;
        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    for (i = 0; i < NAS_LMM_GetEmmFsmBufQueueMidCnt(); i++)
    {
        /*队列数不为0时，指针变量肯定不为NULL，此处是容错处理*/
        if (NAS_EMM_NULL_PTR == pstBuffMsg[i].pBuffMsg)
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEmmFsmBufMidQueue: Point is null!");
            NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
            pucDest += sizeof(VOS_UINT32);
            ulTempLen += sizeof(VOS_UINT32);
            *pulDatalen = ulTempLen;
            continue;
        }
        /*计算指针数据长度，填充变量导出入参*/
        stReplayCtx.pucItemAddr = pstBuffMsg[i].pBuffMsg;
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        pstMsg = pstBuffMsg[i].pBuffMsg;
        pstMsg += NAS_PCREPLAY_LEN_VOS_MSG_HEADER;
        NAS_LMM_MEM_CPY(&ulEmmMsgLen,pstMsg,sizeof(VOS_UINT32));

        stReplayCtx.ulItemLen = EMM_LEN_VOS_MSG_HEADER
                                + ulEmmMsgLen;

        /*指针数据不为空，导出指针数据内容*/
        ulRslt = NAS_Replay_ExportVarible(pucDest,
                                        pucDestEnd,
                                        &stReplayCtx,
                                        pulDatalen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEmmFsmBufMidQueue: Point data buff is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }

        pucDest += *pulDatalen;
        ulTempLen += *pulDatalen;

    }

    /*填充导出结构体变量和指针数据的总长度*/
    *pulDatalen = ulTempLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}
VOS_UINT32  NAS_REPLAY_ExportEmmFsmBufLowQueue
(
    VOS_UINT8 *pucDest,
    const VOS_UINT8 *pucDestEnd,
    VOS_UINT32  *pulDatalen
)
{
    VOS_UINT32                           i;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTempLen = NAS_LMM_NULL;
    NAS_LMM_BUFF_MSG_STRU               *pstBuffMsg;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEmmMsgLen = NAS_LMM_NULL;
    VOS_UINT8                           *pstMsg = NAS_LMM_NULL_PTR;

    pstBuffMsg = NAS_LMM_GetEmmFsmBufQueueLowAddr();

    if (NAS_LMM_NULL == NAS_LMM_GetEmmFsmBufQueueLowCnt())
    {
         /*打印*/
        NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEmmFsmBufLowQueue: NUM is null!");
        NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
        pucDest += sizeof(VOS_UINT32);
        ulTempLen += sizeof(VOS_UINT32);
        *pulDatalen = ulTempLen;
        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    for (i = 0; i < NAS_LMM_GetEmmFsmBufQueueLowCnt(); i++)
    {
        if (NAS_EMM_NULL_PTR == pstBuffMsg[i].pBuffMsg)
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEmmFsmBufLowQueue: Point is null!");
            NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
            pucDest += sizeof(VOS_UINT32);
            ulTempLen += sizeof(VOS_UINT32);
            *pulDatalen = ulTempLen;
            continue;
        }
        /*计算指针数据长度，填充变量导出入参*/
        stReplayCtx.pucItemAddr = pstBuffMsg[i].pBuffMsg;
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        pstMsg = pstBuffMsg[i].pBuffMsg;
        pstMsg += NAS_PCREPLAY_LEN_VOS_MSG_HEADER;
        NAS_LMM_MEM_CPY(&ulEmmMsgLen,pstMsg,sizeof(VOS_UINT32));

        stReplayCtx.ulItemLen = EMM_LEN_VOS_MSG_HEADER
                                + ulEmmMsgLen;

        /*指针数据不为空，导出指针数据内容*/
        ulRslt = NAS_Replay_ExportVarible(pucDest,
                                        pucDestEnd,
                                        &stReplayCtx,
                                        pulDatalen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEmmFsmBufLowQueue: Point data buff is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }

        pucDest += *pulDatalen;
        ulTempLen += *pulDatalen;

    }

    /*填充导出结构体变量和指针数据的总长度*/
    *pulDatalen = ulTempLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}


VOS_UINT32  NAS_REPLAY_ExportMmMainContext
(
    VOS_UINT8                           *pucDest,
    const VOS_UINT8                           *pucDestEnd,
    const VOS_UINT8                           *pucVar,
    VOS_UINT32                          *pulDatalen
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTempLen = NAS_LMM_NULL;

    ulRslt = NAS_Replay_ExportVarible(pucDest,
                                    pucDestEnd,
                                    (NAS_REPLAY_CTX_INFO_STRU*)pucVar,
                                    pulDatalen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportMmMainContext: DATA BUFF is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    pucDest += *pulDatalen;
    ulTempLen += *pulDatalen;
    ulRslt = NAS_REPLAY_ExportEmmFsmBufHighQueue(pucDest,pucDestEnd,pulDatalen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportMmMainContext: EmmFsmBufHighQueue data export is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    pucDest += *pulDatalen;
    ulTempLen += *pulDatalen;

    ulRslt = NAS_REPLAY_ExportEmmFsmBufMidQueue(pucDest,pucDestEnd,pulDatalen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportMmMainContext: EmmFsmBufMidQueue data export is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    pucDest += *pulDatalen;
    ulTempLen += *pulDatalen;
    ulRslt = NAS_REPLAY_ExportEmmFsmBufLowQueue(pucDest,pucDestEnd,pulDatalen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportMmMainContext: EmmFsmBufLowQueue data export is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    /*pucDest += *pulDatalen;*/
    ulTempLen += *pulDatalen;
    *pulDatalen = ulTempLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}
VOS_UINT32  NAS_REPLAY_ExportEsmDecodedNwMsg
(
    VOS_UINT8 *pucDest,
    const VOS_UINT8 *pucDestEnd,
    VOS_UINT32  *pulDatalen
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTempLen = NAS_LMM_NULL;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulNwMsgLen;

    for (i = 0; i < NAS_ESM_MAX_STATETBL_NUM; i++)
    {
        if (NAS_EMM_NULL_PTR == g_stNasEsmEntity.astStateTable[i].pstDecodedNwMsg)
        {
              /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEsmDecodedNwMsg: Point is null!");
            NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
            pucDest += sizeof(VOS_UINT32);
            ulTempLen += sizeof(VOS_UINT32);
            *pulDatalen = ulTempLen;
            continue;
        }
        /*计算指针数据长度，填充变量导出入参*/
        ulNwMsgLen = sizeof(NAS_ESM_NW_MSG_STRU);
        stReplayCtx.pucItemAddr = (VOS_UINT8*)g_stNasEsmEntity.astStateTable[i].pstDecodedNwMsg;
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.ulItemLen = ulNwMsgLen;

        /*指针数据不为空，导出指针数据内容*/
        ulRslt = NAS_Replay_ExportVarible(pucDest,
                                        pucDestEnd,
                                        &stReplayCtx,
                                        pulDatalen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEsmDecodedNwMsg: Point data buff is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }
        pucDest += *pulDatalen;
        ulTempLen += *pulDatalen;

    }

    /*填充导出结构体变量和指针数据的总长度*/
    *pulDatalen = ulTempLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}
VOS_UINT32  NAS_REPLAY_ExportEsmAttBuffItemNum
(
    VOS_UINT8 *pucDest,
    const VOS_UINT8 *pucDestEnd,
    VOS_UINT32  *pulDatalen
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTempLen = NAS_LMM_NULL;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEsmMsgLen = NAS_LMM_NULL;
    VOS_UINT8                           *pstMsg = NAS_LMM_NULL_PTR;

    if (NAS_LMM_NULL == g_stNasEsmEntity.stEsmBuffManageInfo.ucAttBuffItemNum)
    {
         /*打印*/
        NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEsmAttBuffItemNum: NUM is null!");
        NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
        pucDest += sizeof(VOS_UINT32);
        ulTempLen += sizeof(VOS_UINT32);
        *pulDatalen = ulTempLen;

        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    for (i = 0; i < g_stNasEsmEntity.stEsmBuffManageInfo.ucAttBuffItemNum; i++)
    {
        if (NAS_EMM_NULL_PTR == g_stNasEsmEntity.stEsmBuffManageInfo.paAttBuffer[i])
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEsmAttBuffItemNum: Point is null!");
            NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
            pucDest += sizeof(VOS_UINT32);
            ulTempLen += sizeof(VOS_UINT32);
            *pulDatalen = ulTempLen;
            continue;
        }
        /*计算指针数据长度，填充变量导出入参*/
        stReplayCtx.pucItemAddr = g_stNasEsmEntity.stEsmBuffManageInfo.paAttBuffer[i];
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        pstMsg = g_stNasEsmEntity.stEsmBuffManageInfo.paAttBuffer[i];
        pstMsg = pstMsg
                 + sizeof(VOS_UINT32)
                 + NAS_PCREPLAY_LEN_VOS_MSG_HEADER;
        NAS_LMM_MEM_CPY(&ulEsmMsgLen,pstMsg,sizeof(VOS_UINT32));

        stReplayCtx.ulItemLen = sizeof(VOS_UINT32)/*enDecodeRslt*/
                                + EMM_LEN_VOS_MSG_HEADER
                                + ulEsmMsgLen;

        /*指针数据不为空，导出指针数据内容*/
        ulRslt = NAS_Replay_ExportVarible(pucDest,
                                        pucDestEnd,
                                        &stReplayCtx,
                                        pulDatalen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEsmAttBuffItemNum: Point data buff is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }
        pucDest += *pulDatalen;
        ulTempLen += *pulDatalen;

    }

    /*填充导出结构体变量和指针数据的总长度*/
    *pulDatalen = ulTempLen;

    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}
VOS_UINT32  NAS_REPLAY_ExportEsmPtiBuffItemNum
(
    VOS_UINT8 *pucDest,
    const VOS_UINT8 *pucDestEnd,
    VOS_UINT32  *pulDatalen
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTempLen = NAS_LMM_NULL;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEsmMsgLen = NAS_LMM_NULL;
    VOS_UINT8                           *pstMsg = NAS_LMM_NULL_PTR;
    VOS_UINT32                          ulTimerlen = NAS_LMM_NULL;
    (void)ulTimerlen;

    if (NAS_LMM_NULL == g_stNasEsmEntity.stEsmBuffManageInfo.ucPtiBuffItemNum)
    {
         /*打印*/
        NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEsmPtiBuffItemNum: NUM is null!");
        NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
        pucDest += sizeof(VOS_UINT32);
        ulTempLen += sizeof(VOS_UINT32);
        *pulDatalen = ulTempLen;

        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    for (i = 0; i < g_stNasEsmEntity.stEsmBuffManageInfo.ucPtiBuffItemNum; i++)
    {

        if (NAS_EMM_NULL_PTR == g_stNasEsmEntity.stEsmBuffManageInfo.paPtiBuffer[i])
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ExportEsmPtiBuffItemNum: Point is null!");
            NAS_LMM_MEM_SET(pucDest, 0, sizeof(VOS_UINT32));
            pucDest += sizeof(VOS_UINT32);
            ulTempLen += sizeof(VOS_UINT32);
            *pulDatalen = ulTempLen;
            continue;
        }
        /*计算指针数据长度，填充变量导出入参*/
        stReplayCtx.pucItemAddr = g_stNasEsmEntity.stEsmBuffManageInfo.paPtiBuffer[i];
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        pstMsg = g_stNasEsmEntity.stEsmBuffManageInfo.paPtiBuffer[i];
        pstMsg = pstMsg
                + sizeof(VOS_UINT32)/*Pti and eps id*/
                + sizeof(VOS_UINT32)/*opid*/
                + sizeof(NAS_ESM_TIMER_STRU);
        ulTimerlen = sizeof(NAS_ESM_TIMER_STRU);

        NAS_LMM_MEM_CPY(&ulEsmMsgLen,pstMsg,sizeof(VOS_UINT32));

        stReplayCtx.ulItemLen = sizeof(VOS_UINT32)/*Pti and eps id*/
                                + sizeof(VOS_UINT32)/*opid*/
                                + sizeof(NAS_ESM_TIMER_STRU)
                                + sizeof(VOS_UINT32)/*ESM MSG LEN*/
                                + ulEsmMsgLen;

        /*指针数据不为空，导出指针数据内容*/
        ulRslt = NAS_Replay_ExportVarible(pucDest,
                                        pucDestEnd,
                                        &stReplayCtx,
                                        pulDatalen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEsmPtiBuffItemNum: Point data buff is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }
        pucDest += *pulDatalen;
        ulTempLen += *pulDatalen;

    }

    /*填充导出结构体变量和指针数据的总长度*/
    *pulDatalen = ulTempLen;

    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}
VOS_UINT32  NAS_REPLAY_ExportEsmEntity
(
    VOS_UINT8                           *pucDest,
    const VOS_UINT8                           *pucDestEnd,
    const VOS_UINT8                           *pucVar,
    VOS_UINT32                          *pulDatalen
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTempLen = NAS_LMM_NULL;

    ulRslt = NAS_Replay_ExportVarible(pucDest,
                                    pucDestEnd,
                                    (NAS_REPLAY_CTX_INFO_STRU*)pucVar,
                                    pulDatalen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEsmEntity: DATA BUFF is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    pucDest += *pulDatalen;
    ulTempLen += *pulDatalen;
    ulRslt = NAS_REPLAY_ExportEsmDecodedNwMsg(pucDest,pucDestEnd,pulDatalen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEsmEntity: DecodedNwMsg data export is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    pucDest += *pulDatalen;
    ulTempLen += *pulDatalen;
    ulRslt = NAS_REPLAY_ExportEsmAttBuffItemNum(pucDest,pucDestEnd,pulDatalen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEsmEntity:AttBuffItemNum data export is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    pucDest += *pulDatalen;
    ulTempLen += *pulDatalen;
    ulRslt = NAS_REPLAY_ExportEsmPtiBuffItemNum(pucDest,pucDestEnd,pulDatalen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ExportEsmEntity: Point data buff is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    ulTempLen += *pulDatalen;
    *pulDatalen = ulTempLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}


#ifdef __PS_WIN32_RECUR__

#if(VOS_WIN32 == VOS_OS_VER)
/*在单板环境暂不导入*/

VOS_VOID NAS_REPLAY_InitContext(
    const NAS_REPLAY_CTX_INFO_STRU        *pstCtxTbl,
    VOS_UINT32                      ulCount
)
{
    VOS_UINT32                      i;

    for(i = 0; i < ulCount; i++)
    {
        NAS_LMM_MEM_SET(pstCtxTbl[i].pucItemAddr,0,pstCtxTbl[i].ulItemLen);
    }

    return ;
}


VOS_UINT32 NAS_REPLAY_RestoreContext
(
    NAS_REPLAY_CTX_INFO_STRU        *pstCtxTbl,
    VOS_UINT32                      ulCount,
    VOS_UINT8                       *pucSrc
)
{
    VOS_UINT32                      i;
    VOS_UINT32                      ulRslt;
    VOS_UINT32                      ulUsedCmpDataLen;
    VOS_UINT32                      ulUnCmpLen;

    NAS_REPLAY_InitContext(pstCtxTbl, ulCount);

   /* 将pucSrc中的数据按照长度|数据的格式，逐项
       导入到上下文表格中指示的地址中
   */
    for(i = 0; i < ulCount; i++)
    {
        ulRslt    = NAS_PCREPLAY_EXPORT_VAR_CONTINUE;
        ulUsedCmpDataLen = 0;
        #if 0
        if (i == 4)
        {
            ulRslt    = NAS_PCREPLAY_EXPORT_VAR_CONTINUE;
        }
        #endif
        if ( VOS_NULL_PTR != pstCtxTbl[i].pImportFunc)
        {
            ulRslt = pstCtxTbl[i].pImportFunc((VOS_UINT8 *)&pstCtxTbl[i], pucSrc, &ulUsedCmpDataLen, &ulUnCmpLen);
        }

        if ( NAS_PCREPLAY_EXPORT_VAR_CONTINUE == ulRslt)
        {
            pucSrc += ulUsedCmpDataLen;
            ulRslt = NAS_REPLAY_RestoreVar(&pstCtxTbl[i], pucSrc, &ulUsedCmpDataLen, &ulUnCmpLen);
        }

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
            return VOS_ERR;
        }

        pucSrc += ulUsedCmpDataLen;
    }
    return VOS_OK;
}


VOS_UINT32 NAS_REPLAY_RestoreVar
(
    const NAS_REPLAY_CTX_INFO_STRU        *pstVar,
    const VOS_UINT8                       *pucSrc,
    VOS_UINT32                      *pulUsedCmpDataLen,
    VOS_UINT32                      *pulUnCmpLen
)
{
    VOS_UINT32                      ulRslt = VOS_FALSE;
    VOS_UINT32                      ulDataLen = NAS_LMM_NULL;
    VOS_UINT8                       *pucTmpBuf= NAS_LMM_NULL_PTR;
    VOS_UINT32                      ulDestLen = NAS_LMM_NULL;
    (void)pstVar;
    (void)pucSrc;
    (void)pulUsedCmpDataLen;
    (void)pulUnCmpLen;
    (void)ulRslt;
    (void)ulDataLen;
    (void)pucTmpBuf;
    (void)ulDestLen;

    ulDestLen = NAS_PC_REPLAY_UNCOMPRESS_DATA_LEN;

    #if(VOS_WIN32 == VOS_OS_VER)
    /*PC测试不需解压缩*/
    PS_MEM_CPY(&ulDataLen, pucSrc, sizeof(VOS_UINT32));
    /* 长度字节 + 数据长度 */
    *pulUsedCmpDataLen = ulDataLen + sizeof(VOS_UINT32);

    /* 设置版本是否匹配标志 */
    /*NAS_Replay_SetVerMatchFlg(((ulDataLen==pstVar->ulItemLen)?VOS_TRUE:VOS_FALSE));*/

     /* 实际数据的长度不能超过上下文表格中指示的数据长度,因为
    导入的数据长度超过上下文表格中的长度会造成越界*/
    if(ulDataLen > pstVar->ulItemLen)
    {
        ulDataLen = pstVar->ulItemLen;
    }

    /* 更新缓冲区指针 */
    pucSrc += sizeof(VOS_UINT32);
    PS_MEM_CPY(pstVar->pucItemAddr, pucSrc, ulDataLen);

    *pulUnCmpLen = ulDataLen;

    #else
    /*单板测试需解压缩*/
    #if 0
    pucTmpBuf = NAS_LMM_MEM_ALLOC(NAS_PC_REPLAY_UNCOMPRESS_DATA_LEN);
    if(VOS_NULL == pucTmpBuf)
    {
        *pulUsedCmpDataLen = 0;
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    /*若上下文表格中指示数据不需要压缩，则认为pucSrc中的
	对应项数据未进行压缩，直接取出长度并拷贝即可；
    若上下文表格中指示数据需要压缩，则将 pucSrc中的对应项
    解压到临时缓冲*/

    if(VOS_FALSE == pstVar->ulCompressFlg )
    {
        PS_MEM_CPY(&ulDataLen, pucSrc, sizeof(VOS_UINT32));
        /* 长度字节 + 数据长度 */
        *pulUsedCmpDataLen = ulDataLen + sizeof(VOS_UINT32);

        /* 设置版本是否匹配标志 */
        /*NAS_Replay_SetVerMatchFlg(((ulDataLen==pstVar->ulItemLen)?VOS_TRUE:VOS_FALSE));*/

         /* 实际数据的长度不能超过上下文表格中指示的数据长度,因为
        导入的数据长度超过上下文表格中的长度会造成越界*/
        if(ulDataLen > pstVar->ulItemLen)
        {
            ulDataLen = pstVar->ulItemLen;
        }

        /* 更新缓冲区指针 */
        pucSrc += sizeof(VOS_UINT32);
        PS_MEM_CPY(pstVar->pucItemAddr, pucSrc, ulDataLen);

        *pulUnCmpLen = ulDataLen;
    }
    else
    {
        /*调用解压缩接口*/
        ulRslt = NAS_UnCompressData(pucTmpBuf, &ulDestLen, pucSrc, &ulDataLen);
        if (VOS_FALSE == ulRslt)
        {
            PS_MEM_FREE(WUEPS_PID_TAF, pucTmpBuf);
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }
        pucSrc += ulDataLen;

        /* 缓冲区使用长度，给输出参数赋值 */
        *pulUsedCmpDataLen = ulDataLen;

        /* 设置版本是否匹配标志 */
        NAS_Replay_SetVerMatchFlg(((ulDestLen==pstVar->ulItemLen)?VOS_TRUE:VOS_FALSE));

        /* 实际数据的长度不能超过上下文表格中指示的数据长度,因为
        导入的数据长度超过上下文表格中的长度会造成越界*/
        if(ulDestLen > pstVar->ulItemLen)
        {
            ulDestLen = pstVar->ulItemLen;
        }
        PS_MEM_CPY(pstVar->pucItemAddr, pucTmpBuf, ulDestLen);

        *pulUnCmpLen = ulDestLen;
    }
    NAS_LMM_MEM_FREE(pucTmpBuf);
    #endif
    #endif
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}


VOS_UINT32  NAS_EMM_RestoreContextData(MsgBlock * pstMsg)
{
    NAS_PC_REPLAY_RUNNING_CONTEXT_STRU  *pstReplayCtx = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulRslt;

    pstReplayCtx = (NAS_PC_REPLAY_RUNNING_CONTEXT_STRU*)pstMsg;

    if ((PS_PID_MM != pstReplayCtx->ulSenderPid)
        || (PS_PID_MM != pstReplayCtx->ulReceiverPid)
        || (NAS_EMM_REPLAY_CONTEXT != pstReplayCtx->ulMsgName))
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_RestoreContextData: MSG or PID is err!");
        return VOS_FALSE;
    }

    ulRslt = NAS_REPLAY_RestoreContext(g_astNasEmmReplay,
                                        g_ulNasEmmReplayNum,
                                        pstReplayCtx->aucData);
    if (VOS_ERR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_RestoreContextData: DATA restore is failure!");
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32  NAS_ESM_RestoreContextData(MsgBlock * pstMsg)
{
    NAS_PC_REPLAY_RUNNING_CONTEXT_STRU  *pstReplayCtx = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulRslt;

    pstReplayCtx = (NAS_PC_REPLAY_RUNNING_CONTEXT_STRU*)pstMsg;

    if ((PS_PID_ESM != pstReplayCtx->ulSenderPid)
        || (PS_PID_ESM != pstReplayCtx->ulReceiverPid)
        || (NAS_ESM_REPLAY_CONTEXT != pstReplayCtx->ulMsgName))
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_ESM_RestoreContextData: MSG is err!");
        return VOS_FALSE;
    }

    ulRslt = NAS_REPLAY_RestoreContext(g_astNasEsmReplay,
                                        g_ulNasEsmReplayNum,
                                        pstReplayCtx->aucData);
    if (VOS_ERR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_ESM_RestoreContextData: DATA restore is failure!");
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32  NAS_ERABM_RestoreContextData(MsgBlock * pstMsg)
{
    NAS_PC_REPLAY_RUNNING_CONTEXT_STRU  *pstReplayCtx = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulRslt;

    pstReplayCtx = (NAS_PC_REPLAY_RUNNING_CONTEXT_STRU*)pstMsg;

    if ((PS_PID_RABM != pstReplayCtx->ulSenderPid)
        || (PS_PID_RABM != pstReplayCtx->ulReceiverPid)
        || (NAS_ERABM_REPLAY_CONTEXT != pstReplayCtx->ulMsgName))
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_ERABM_RestoreContextData: MSG is err!");
        return VOS_FALSE;
    }

    ulRslt = NAS_REPLAY_RestoreContext(g_astNasErabmReplay,
                                        g_ulNasErabmReplayNum,
                                        pstReplayCtx->aucData);
    if (VOS_ERR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_ERABM_RestoreContextData: DATA restore is failure!");
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32  NAS_REPLAY_ImportEmmInfo
(
    const VOS_UINT8                           *pucVar,
    const VOS_UINT8                           *pucSrc,
    VOS_UINT32                          *pulUsedCmpDataLen,
    VOS_UINT32                          *pulUnCmpLen
)
{
    VOS_UINT32                          ulRslt;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEmcMsgLen;
    VOS_UINT32                          ulTemptLen = NAS_LMM_NULL;
    VOS_UINT32                          ulCmpDataLen = NAS_LMM_NULL;

    ulRslt = NAS_REPLAY_RestoreVar((NAS_REPLAY_CTX_INFO_STRU*)pucVar,
                                    pucSrc,
                                    pulUsedCmpDataLen,
                                    pulUnCmpLen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportEmmInfo: DATA restore is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    pucSrc += *pulUsedCmpDataLen;
    ulCmpDataLen += *pulUsedCmpDataLen;
    ulTemptLen += *pulUnCmpLen;

    /*获取指针变量长度*/
    NAS_LMM_MEM_CPY(&ulEmcMsgLen, pucSrc, sizeof(VOS_UINT32));

    if (NAS_EMM_NULL == ulEmcMsgLen)
    {
         /*打印*/
        NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ImportEmmInfo: data len is null!");
        g_stEmmInfo.pucEsmMsg = NAS_EMM_NULL_PTR;
        pucSrc += sizeof(VOS_UINT32);
        ulCmpDataLen += sizeof(VOS_UINT32);
        *pulUsedCmpDataLen = ulCmpDataLen;
        *pulUnCmpLen = ulTemptLen;
        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    g_stEmmInfo.pucEsmMsg = NAS_COMM_AllocBuffItem(NAS_COMM_BUFF_TYPE_EMM,ulEmcMsgLen);

    stReplayCtx.pucItemAddr = g_stEmmInfo.pucEsmMsg;
    stReplayCtx.ulCompressFlg = VOS_FALSE;
    stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
    stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
    stReplayCtx.ulItemLen = ulEmcMsgLen;

    /*序列化数据长度不为0，导入数据内容*/
    ulRslt = NAS_REPLAY_RestoreVar(&stReplayCtx,
                                    pucSrc,
                                    pulUsedCmpDataLen,
                                    pulUnCmpLen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportEmmInfo: Point data buff is failure!");

        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    /*pucSrc += *pulUsedCmpDataLen;*/
    ulCmpDataLen += *pulUsedCmpDataLen;
    ulTemptLen += *pulUnCmpLen;

    /*获取压缩后数据长度*/
    *pulUsedCmpDataLen = ulCmpDataLen;

    /*获取解压缩后数据长度*/
    *pulUnCmpLen = ulTemptLen;

    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}


VOS_UINT32  NAS_REPLAY_ImportEmmEsmMsgBuf
(
    const VOS_UINT8                           *pucVar,
    const VOS_UINT8                           *pucSrc,
    VOS_UINT32                          *pulUsedCmpDataLen,
    VOS_UINT32                          *pulUnCmpLen
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulRslt;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEsmMsgLen = NAS_LMM_NULL;
    VOS_UINT32                          ulTemptLen = NAS_LMM_NULL;
    VOS_UINT32                          ulCmpDataLen = NAS_LMM_NULL;

    ulRslt = NAS_REPLAY_RestoreVar((NAS_REPLAY_CTX_INFO_STRU*)pucVar,
                                    pucSrc,
                                    pulUsedCmpDataLen,
                                    pulUnCmpLen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportEmmEsmMsgBuf: DATA restore is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    pucSrc += *pulUsedCmpDataLen;
    ulCmpDataLen += *pulUsedCmpDataLen;
    ulTemptLen += *pulUnCmpLen;

    if (NAS_LMM_NULL == g_stEmmEsmMsgBuf.ulEsmMsgCnt)
    {
        pucSrc += sizeof(VOS_UINT32);
        ulCmpDataLen += sizeof(VOS_UINT32);
        *pulUsedCmpDataLen = ulCmpDataLen;
        *pulUnCmpLen = ulTemptLen;
        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    for(i = 0; i < g_stEmmEsmMsgBuf.ulEsmMsgCnt; i++)
    {
        NAS_LMM_MEM_CPY(&ulEsmMsgLen,pucSrc,sizeof(VOS_UINT32));

        if (NAS_EMM_NULL == ulEsmMsgLen)
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ImportEmmEsmMsgBuf: data len is null!");

            pucSrc += sizeof(VOS_UINT32);
            ulCmpDataLen += sizeof(VOS_UINT32);

            *pulUsedCmpDataLen = ulCmpDataLen;
            *pulUnCmpLen = ulTemptLen;
            continue;
        }

        g_stEmmEsmMsgBuf.apucEsmMsgBuf[i] = NAS_COMM_AllocBuffItem(NAS_COMM_BUFF_TYPE_EMM,ulEsmMsgLen);
        stReplayCtx.pucItemAddr = g_stEmmEsmMsgBuf.apucEsmMsgBuf[i];
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.ulItemLen = ulEsmMsgLen;

        /*序列化数据长度不为0，导入数据内容*/
        ulRslt = NAS_REPLAY_RestoreVar(&stReplayCtx,
                                        pucSrc,
                                        pulUsedCmpDataLen,
                                        pulUnCmpLen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportEmmEsmMsgBuf: Point data buff is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }
        pucSrc += *pulUsedCmpDataLen;
        ulCmpDataLen += *pulUsedCmpDataLen;
        ulTemptLen += *pulUnCmpLen;

    }

    /*获取压缩后数据长度*/
    *pulUsedCmpDataLen = ulCmpDataLen;

    /*获取解压缩后数据长度*/
    *pulUnCmpLen = ulTemptLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}


VOS_UINT32  NAS_REPLAY_ImportEmmFsmBufHighQueue
(
    const VOS_UINT8                       *pucSrc,
    VOS_UINT32                      *pulUsedCmpDataLen,
    VOS_UINT32                      *pulUnCmpLen
)
{
    VOS_UINT32                           i;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTemptLen = NAS_LMM_NULL;
    VOS_UINT32                          ulCmpDataLen = NAS_LMM_NULL;
    NAS_LMM_BUFF_MSG_STRU               *pstBuffMsg;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEmmMsgLen;

    pstBuffMsg = NAS_LMM_GetEmmFsmBufQueueHighAddr();

    if (NAS_LMM_NULL == NAS_LMM_GetEmmFsmBufQueueHighCnt())
    {
        pucSrc += sizeof(VOS_UINT32);
        ulCmpDataLen += sizeof(VOS_UINT32);
        *pulUsedCmpDataLen = ulCmpDataLen;
        *pulUnCmpLen = ulTemptLen;
        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }
    for (i = 0; i < NAS_LMM_GetEmmFsmBufQueueHighCnt(); i++)
    {
        NAS_LMM_MEM_CPY(&ulEmmMsgLen,pucSrc,sizeof(VOS_UINT32));

        if (NAS_EMM_NULL == ulEmmMsgLen)
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ImportEmmFsmBufHighQueue: data len is null!");
            pucSrc += sizeof(VOS_UINT32);
            ulCmpDataLen += sizeof(VOS_UINT32);
            *pulUsedCmpDataLen = ulCmpDataLen;
            *pulUnCmpLen = ulTemptLen;
            continue;
        }
        pstBuffMsg[i].pBuffMsg = NAS_COMM_AllocBuffItem(NAS_COMM_BUFF_TYPE_EMM,ulEmmMsgLen);
        stReplayCtx.pucItemAddr = pstBuffMsg[i].pBuffMsg;
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.ulItemLen = ulEmmMsgLen;

        /*序列化数据长度不为0，导入数据内容*/
        ulRslt = NAS_REPLAY_RestoreVar(&stReplayCtx,
                                        pucSrc,
                                        pulUsedCmpDataLen,
                                        pulUnCmpLen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportMmMainContex: data import is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }
        pucSrc += *pulUsedCmpDataLen;
        ulCmpDataLen += *pulUsedCmpDataLen;
        ulTemptLen += *pulUnCmpLen;

    }

    /*获取压缩后数据长度*/
    *pulUsedCmpDataLen = ulCmpDataLen;

    /*获取解压缩后数据长度*/
    *pulUnCmpLen = ulTemptLen;

    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}
VOS_UINT32  NAS_REPLAY_ImportEmmFsmBufMidQueue
(
    const VOS_UINT8                       *pucSrc,
    VOS_UINT32                      *pulUsedCmpDataLen,
    VOS_UINT32                      *pulUnCmpLen
)
{
    VOS_UINT32                           i;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTemptLen = NAS_LMM_NULL;
    VOS_UINT32                          ulCmpDataLen = NAS_LMM_NULL;
    NAS_LMM_BUFF_MSG_STRU               *pstBuffMsg;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEmmMsgLen;

    pstBuffMsg = NAS_LMM_GetEmmFsmBufQueueMidAddr();

    if (NAS_LMM_NULL == NAS_LMM_GetEmmFsmBufQueueMidCnt())
    {
        pucSrc += sizeof(VOS_UINT32);
        ulCmpDataLen += sizeof(VOS_UINT32);
        *pulUsedCmpDataLen = ulCmpDataLen;
        *pulUnCmpLen = ulTemptLen;
        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    for (i = 0; i < NAS_LMM_GetEmmFsmBufQueueMidCnt(); i++)
    {
        NAS_LMM_MEM_CPY(&ulEmmMsgLen,pucSrc,sizeof(VOS_UINT32));

        if (NAS_EMM_NULL == ulEmmMsgLen)
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ImportEmmFsmBufMidQueue: data len is null!");
            pucSrc += sizeof(VOS_UINT32);
            ulCmpDataLen += sizeof(VOS_UINT32);
            *pulUsedCmpDataLen = ulCmpDataLen;
            *pulUnCmpLen = ulTemptLen;
            continue;
        }

        pstBuffMsg[i].pBuffMsg = NAS_COMM_AllocBuffItem(NAS_COMM_BUFF_TYPE_EMM,ulEmmMsgLen);
        stReplayCtx.pucItemAddr = pstBuffMsg[i].pBuffMsg;
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.ulItemLen = ulEmmMsgLen;

        /*序列化数据长度不为0，导入数据内容*/
        ulRslt = NAS_REPLAY_RestoreVar(&stReplayCtx,
                                        pucSrc,
                                        pulUsedCmpDataLen,
                                        pulUnCmpLen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportEmmFsmBufMidQueue: data import is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }
        pucSrc += *pulUsedCmpDataLen;
        ulCmpDataLen += *pulUsedCmpDataLen;
        ulTemptLen += *pulUnCmpLen;

    }

    /*获取压缩后数据长度*/
    *pulUsedCmpDataLen = ulCmpDataLen;

    /*获取解压缩后数据长度*/
    *pulUnCmpLen = ulTemptLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}
VOS_UINT32  NAS_REPLAY_ImportEmmFsmBufLowQueue
(
    const VOS_UINT8                       *pucSrc,
    VOS_UINT32                      *pulUsedCmpDataLen,
    VOS_UINT32                      *pulUnCmpLen
)
{
    VOS_UINT32                           i;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTemptLen = NAS_LMM_NULL;
    VOS_UINT32                          ulCmpDataLen = NAS_LMM_NULL;
    NAS_LMM_BUFF_MSG_STRU               *pstBuffMsg;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEmmMsgLen;

    pstBuffMsg = NAS_LMM_GetEmmFsmBufQueueLowAddr();

    if (NAS_LMM_NULL == NAS_LMM_GetEmmFsmBufQueueLowCnt())
    {
        pucSrc += sizeof(VOS_UINT32);
        ulCmpDataLen += sizeof(VOS_UINT32);
        *pulUsedCmpDataLen = ulCmpDataLen;
        *pulUnCmpLen = ulTemptLen;
        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    for (i = 0; i < NAS_LMM_GetEmmFsmBufQueueLowCnt(); i++)
    {
        NAS_LMM_MEM_CPY(&ulEmmMsgLen,pucSrc,sizeof(VOS_UINT32));
        if (NAS_EMM_NULL == ulEmmMsgLen)
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ImportEmmFsmBufLowQueue: data len is null!");
            pucSrc += sizeof(VOS_UINT32);
            ulCmpDataLen += sizeof(VOS_UINT32);

            *pulUsedCmpDataLen = ulCmpDataLen;
            *pulUnCmpLen = ulTemptLen;
            continue;
        }

        pstBuffMsg[i].pBuffMsg = NAS_COMM_AllocBuffItem(NAS_COMM_BUFF_TYPE_EMM,ulEmmMsgLen);
        stReplayCtx.pucItemAddr = pstBuffMsg[i].pBuffMsg;
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.ulItemLen = ulEmmMsgLen;

        /*序列化数据长度不为0，导入数据内容*/
        ulRslt = NAS_REPLAY_RestoreVar(&stReplayCtx,
                                        pucSrc,
                                        pulUsedCmpDataLen,
                                        pulUnCmpLen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportEmmFsmBufLowQueue: data import is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }
        pucSrc += *pulUsedCmpDataLen;
        ulCmpDataLen += *pulUsedCmpDataLen;
        ulTemptLen += *pulUnCmpLen;
    }

    /*获取压缩后数据长度*/
    *pulUsedCmpDataLen = ulCmpDataLen;

    /*获取解压缩后数据长度*/
    *pulUnCmpLen = ulTemptLen;

    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}
VOS_UINT32  NAS_REPLAY_ImportMmMainContex
(
    const VOS_UINT8                           *pucVar,
    const VOS_UINT8                           *pucSrc,
    VOS_UINT32                          *pulUsedCmpDataLen,
    VOS_UINT32                          *pulUnCmpLen
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTemptLen = NAS_LMM_NULL;
    VOS_UINT32                          ulCmpDataLen = NAS_LMM_NULL;

    ulRslt = NAS_REPLAY_RestoreVar((NAS_REPLAY_CTX_INFO_STRU*)pucVar,
                                    pucSrc,
                                    pulUsedCmpDataLen,
                                    pulUnCmpLen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportMmMainContex: DATA restore is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    pucSrc += *pulUsedCmpDataLen;
    ulCmpDataLen += *pulUsedCmpDataLen;
    ulTemptLen += *pulUnCmpLen;

    ulRslt = NAS_REPLAY_ImportEmmFsmBufHighQueue(pucSrc,pulUsedCmpDataLen,pulUnCmpLen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportMmMainContex: EmmFsmBufHighQueue data import is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    pucSrc += *pulUsedCmpDataLen;
    ulCmpDataLen += *pulUsedCmpDataLen;
    ulTemptLen += *pulUnCmpLen;
    ulRslt = NAS_REPLAY_ImportEmmFsmBufMidQueue(pucSrc,pulUsedCmpDataLen,pulUnCmpLen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportMmMainContex: EmmFsmBufMidQueue data import is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }
    pucSrc += *pulUsedCmpDataLen;
    ulCmpDataLen += *pulUsedCmpDataLen;
    ulTemptLen += *pulUnCmpLen;
    ulRslt = NAS_REPLAY_ImportEmmFsmBufLowQueue(pucSrc,pulUsedCmpDataLen,pulUnCmpLen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportMmMainContex: EmmFsmBufLowQueue data import is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    ulCmpDataLen += *pulUsedCmpDataLen;
    ulTemptLen += *pulUnCmpLen;

    /*获取压缩后数据长度*/
    *pulUsedCmpDataLen = ulCmpDataLen;

    /*获取解压缩后数据长度*/
    *pulUnCmpLen = ulTemptLen;

    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}


VOS_UINT32  NAS_REPLAY_ImportEsmDecodedNwMsg
(
    const VOS_UINT8                       *pucSrc,
    VOS_UINT32                      *pulUsedCmpDataLen,
    VOS_UINT32                      *pulUnCmpLen
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          i;
    VOS_UINT32                          ulTemptLen = NAS_LMM_NULL;
    VOS_UINT32                          ulCmpDataLen = NAS_LMM_NULL;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEsmMsgLen;

    for (i = 0; i < NAS_ESM_MAX_STATETBL_NUM; i++)
    {
        NAS_LMM_MEM_CPY(&ulEsmMsgLen,pucSrc,sizeof(VOS_UINT32));

        if (NAS_EMM_NULL == ulEsmMsgLen)
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ImportEsmDecodedNwMsg: data len is null!");
            pucSrc += sizeof(VOS_UINT32);
            ulCmpDataLen += sizeof(VOS_UINT32);
            *pulUsedCmpDataLen = ulCmpDataLen;
            *pulUnCmpLen = ulTemptLen;
            continue;
        }

        g_stNasEsmEntity.astStateTable[i].pstDecodedNwMsg = NAS_COMM_AllocBuffItem(NAS_COMM_BUFF_TYPE_ESM,ulEsmMsgLen);
        stReplayCtx.pucItemAddr = (VOS_UINT8*)g_stNasEsmEntity.astStateTable[i].pstDecodedNwMsg;
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.ulItemLen = ulEsmMsgLen;

        /*序列化数据长度不为0，导入数据内容*/
        ulRslt = NAS_REPLAY_RestoreVar(&stReplayCtx,
                                        pucSrc,
                                        pulUsedCmpDataLen,
                                        pulUnCmpLen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportEsmDecodedNwMsg: data import is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }
        pucSrc += *pulUsedCmpDataLen;
        ulCmpDataLen += *pulUsedCmpDataLen;
        ulTemptLen += *pulUnCmpLen;

    }

    /*获取压缩后数据长度*/
    *pulUsedCmpDataLen = ulCmpDataLen;

    /*获取解压缩后数据长度*/
    *pulUnCmpLen = ulTemptLen;

    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}
VOS_UINT32  NAS_REPLAY_ImportEsmAttBuffItemNum
(
    const VOS_UINT8                       *pucSrc,
    VOS_UINT32                      *pulUsedCmpDataLen,
    VOS_UINT32                      *pulUnCmpLen
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          i;
    VOS_UINT32                          ulTemptLen = NAS_LMM_NULL;
    VOS_UINT32                          ulCmpDataLen = NAS_LMM_NULL;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEsmMsgLen;

    if (NAS_LMM_NULL == g_stNasEsmEntity.stEsmBuffManageInfo.ucAttBuffItemNum)
    {
        /*打印*/
        NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ImportEsmAttBuffItemNum: NUM is null!");
        pucSrc += sizeof(VOS_UINT32);
        ulCmpDataLen += sizeof(VOS_UINT32);
        *pulUsedCmpDataLen = ulCmpDataLen;
        *pulUnCmpLen = ulTemptLen;
        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    for (i = 0; i < g_stNasEsmEntity.stEsmBuffManageInfo.ucAttBuffItemNum; i++)
    {
        NAS_LMM_MEM_CPY(&ulEsmMsgLen,pucSrc,sizeof(VOS_UINT32));
        if (NAS_EMM_NULL == *pulUnCmpLen)
        {
            /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ImportEsmAttBuffItemNum: data len is null!");
            pucSrc += sizeof(VOS_UINT32);
            ulCmpDataLen += sizeof(VOS_UINT32);
            *pulUsedCmpDataLen = ulCmpDataLen;
            *pulUnCmpLen = ulTemptLen;
            continue;
        }

        g_stNasEsmEntity.stEsmBuffManageInfo.paAttBuffer[i] = NAS_COMM_AllocBuffItem(NAS_COMM_BUFF_TYPE_ESM,ulEsmMsgLen);
        stReplayCtx.pucItemAddr = g_stNasEsmEntity.stEsmBuffManageInfo.paAttBuffer[i];
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.ulItemLen = ulEsmMsgLen;

        /*序列化数据长度不为0，导入数据内容*/
        ulRslt = NAS_REPLAY_RestoreVar(&stReplayCtx,
                                        pucSrc,
                                        pulUsedCmpDataLen,
                                        pulUnCmpLen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportEsmAttBuffItemNum: data import is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }
        pucSrc += *pulUsedCmpDataLen;
        ulCmpDataLen += *pulUsedCmpDataLen;
        ulTemptLen += *pulUnCmpLen;

    }

    /*获取压缩后数据长度*/
    *pulUsedCmpDataLen = ulCmpDataLen;

    /*获取解压缩后数据长度*/
    *pulUnCmpLen = ulTemptLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}


VOS_UINT32  NAS_REPLAY_ImportEsmPtiBuffItemNum
(
    const VOS_UINT8                       *pucSrc,
    VOS_UINT32                      *pulUsedCmpDataLen,
    VOS_UINT32                      *pulUnCmpLen
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          i;
    VOS_UINT32                          ulTemptLen = NAS_LMM_NULL;
    VOS_UINT32                          ulCmpDataLen = NAS_LMM_NULL;
    NAS_REPLAY_CTX_INFO_STRU            stReplayCtx;
    VOS_UINT32                          ulEsmMsgLen;

    if (NAS_LMM_NULL == g_stNasEsmEntity.stEsmBuffManageInfo.ucPtiBuffItemNum)
    {
        /*打印*/
        NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ImportEsmPtiBuffItemNum: NUM is null!");
        pucSrc += sizeof(VOS_UINT32);
        ulCmpDataLen += sizeof(VOS_UINT32);
        *pulUsedCmpDataLen = ulCmpDataLen;
        *pulUnCmpLen = ulTemptLen;
        return NAS_PCREPLAY_EXPORT_VAR_FINISH;
    }

    for (i = 0; i < g_stNasEsmEntity.stEsmBuffManageInfo.ucPtiBuffItemNum; i++)
    {
        NAS_LMM_MEM_CPY(&ulEsmMsgLen,pucSrc,sizeof(VOS_UINT32));
        if (NAS_EMM_NULL == ulEsmMsgLen)
        {
             /*打印*/
            NAS_LMM_PUBM_LOG_NORM("NAS_REPLAY_ImportEsmPtiBuffItemNum: data len is null!");
            pucSrc += sizeof(VOS_UINT32);
            ulCmpDataLen += sizeof(VOS_UINT32);
            *pulUsedCmpDataLen = ulCmpDataLen;
            *pulUnCmpLen = ulTemptLen;
            continue;
        }

        g_stNasEsmEntity.stEsmBuffManageInfo.paPtiBuffer[i] = NAS_COMM_AllocBuffItem(NAS_COMM_BUFF_TYPE_ESM,ulEsmMsgLen);
        stReplayCtx.pucItemAddr = g_stNasEsmEntity.stEsmBuffManageInfo.paPtiBuffer[i];
        stReplayCtx.ulCompressFlg = VOS_FALSE;
        stReplayCtx.pExportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.pImportFunc = NAS_LMM_NULL_PTR;
        stReplayCtx.ulItemLen = ulEsmMsgLen;

        /*序列化数据长度不为0，导入数据内容*/
        ulRslt = NAS_REPLAY_RestoreVar(&stReplayCtx,
                                        pucSrc,
                                        pulUsedCmpDataLen,
                                        pulUnCmpLen);

        if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
        {
             /*打印错误*/
            NAS_LMM_PUBM_LOG_ERR("NAS_REPLAY_ImportEsmPtiBuffItemNum: data import is failure!");
            return NAS_PCREPLAY_EXPORT_VAR_ERROR;
        }
        pucSrc += *pulUsedCmpDataLen;
        ulCmpDataLen += *pulUsedCmpDataLen;
        ulTemptLen += *pulUnCmpLen;
    }

    /*获取压缩后数据长度*/
    *pulUsedCmpDataLen = ulCmpDataLen;

    /*获取解压缩后数据长度*/
    *pulUnCmpLen = ulTemptLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}


VOS_UINT32  NAS_Replay_ImportEsmEntity
(
    const VOS_UINT8                           *pucVar,
    const VOS_UINT8                           *pucSrc,
    VOS_UINT32                          *pulUsedCmpDataLen,
    VOS_UINT32                          *pulUnCmpLen
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulTemptLen = NAS_LMM_NULL;
    VOS_UINT32                          ulCmpDataLen = NAS_LMM_NULL;

    ulRslt = NAS_REPLAY_RestoreVar((NAS_REPLAY_CTX_INFO_STRU*)pucVar,
                                    pucSrc,
                                    pulUsedCmpDataLen,
                                    pulUnCmpLen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_Replay_ImportEsmEntity: DATA restore is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    pucSrc += *pulUsedCmpDataLen;
    ulCmpDataLen += *pulUsedCmpDataLen;
    ulTemptLen += *pulUnCmpLen;
    ulRslt = NAS_REPLAY_ImportEsmDecodedNwMsg(pucSrc,pulUsedCmpDataLen,pulUnCmpLen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_Replay_ImportEsmEntity:EsmDecodedNwMsg data import is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    pucSrc += *pulUsedCmpDataLen;
    ulCmpDataLen += *pulUsedCmpDataLen;
    ulTemptLen += *pulUnCmpLen;
    ulRslt = NAS_REPLAY_ImportEsmAttBuffItemNum(pucSrc,pulUsedCmpDataLen,pulUnCmpLen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_Replay_ImportEsmEntity:EsmAttBuffItemNum( data import is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    pucSrc += *pulUsedCmpDataLen;
    ulCmpDataLen += *pulUsedCmpDataLen;
    ulTemptLen += *pulUnCmpLen;
    ulRslt = NAS_REPLAY_ImportEsmPtiBuffItemNum(pucSrc,pulUsedCmpDataLen,pulUnCmpLen);

    if (NAS_PCREPLAY_EXPORT_VAR_ERROR == ulRslt)
    {
         /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_Replay_ImportEsmEntity:EsmPtiBuffItemNum data import is failure!");
        return NAS_PCREPLAY_EXPORT_VAR_ERROR;
    }

    ulCmpDataLen += *pulUsedCmpDataLen;
    ulTemptLen += *pulUnCmpLen;

    /*获取压缩后数据长度*/
    *pulUsedCmpDataLen = ulCmpDataLen;

    /*获取解压缩后数据长度*/
    *pulUnCmpLen = ulTemptLen;
    return NAS_PCREPLAY_EXPORT_VAR_FINISH;
}


VOS_VOID LNAS_REPLAY_ImportAPIInfo(MsgBlock *pstMsg)
{
    VOS_UINT32                          i;
    PS_MSG_HEADER_STRU                  *pstPsMsg;

    pstPsMsg = (PS_MSG_HEADER_STRU*)pstMsg;

    for (i = 0; i < sizeof(g_LNasReplayApiImportTab)/sizeof(LNAS_REPLAY_API_INFO_IMPORT_STRU); i++)
    {
        if ((pstPsMsg->ulSenderPid   == g_LNasReplayApiImportTab[i].ulSenderPid)
         && (pstPsMsg->ulReceiverPid == g_LNasReplayApiImportTab[i].ulRcverPid)
         && (pstPsMsg->ulMsgName     == g_LNasReplayApiImportTab[i].ulMsgNme))
        {
            NAS_LMM_PUBM_LOG_NORM("LNAS_REPLAY_ImportAPIInfo: find import function");

            (void)g_LNasReplayApiImportTab[i].pImportFunc(pstMsg);
            break;
        }
    }

    return;
}


VOS_UINT32 NAS_REPLAY_SavePubInfo(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_PUB_INFO_STRU          *pstLogPubInfo;

    pstLogPubInfo = (NAS_LMM_LOG_PUB_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstLogPubInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePubInfo:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulPubInfoCount < NAS_REPLAY_MAX_PUB_INFO)
    {
        /*保存回放消息中内容*/
        g_astPubInfo[g_ulPubInfoCount].ulInfoType = pstLogPubInfo->ulInfoType;
        NAS_LMM_MEM_CPY(&g_astPubInfo[g_ulPubInfoCount].stPubInfo,
                        &pstLogPubInfo->stPubInfo,
                        sizeof(MMC_LMM_PUB_INFO_STRU));

        g_astPubInfo[g_ulPubInfoCount].ulRslt = pstLogPubInfo->ulRslt;

        g_ulPubInfoCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePubInfo:export Msg count is large Max count!");
    }

    return VOS_TRUE;
}



MMC_LMM_RESULT_ID_ENUM_UINT32  Stub_Nas_GetPubInfo
(
    MMC_LMM_INFO_TYPE_ENUM_UINT32    ulInfoType,
    MMC_LMM_PUB_INFO_STRU           *pPubInfo
)
{
    VOS_UINT32                          ulRslt;
    (void)ulInfoType;
    if (g_ulPubInfoIndex < g_ulPubInfoCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_Nas_GetPubInfo: Export Nas_GetPubInfo info!");

        /*导入出参和返回值*/
        NAS_LMM_MEM_CPY(pPubInfo,
                        &g_astPubInfo[g_ulPubInfoIndex].stPubInfo,
                        sizeof(MMC_LMM_PUB_INFO_STRU));

        ulRslt = g_astPubInfo[g_ulPubInfoIndex].ulRslt;

        g_ulPubInfoIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_Nas_GetPubInfo: Export Nas_GetPubInfo info ERR!");

        NAS_LMM_MEM_SET(pPubInfo, 0, sizeof(MMC_LMM_PUB_INFO_STRU));
        ulRslt = MMC_LMM_ERR_ID_BUTT;
    }
    return  ulRslt;
}
VOS_UINT32 NAS_REPLAY_SaveGuInfo(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_GU_INFO_STRU          *pstLogGuInfo;

    pstLogGuInfo = (NAS_LMM_LOG_GU_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstLogGuInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveGuInfo:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulGuInfoCount < NAS_REPLAY_MAX_GU_INFO)
    {
        /*保存回放消息中内容*/
        g_astGuInfo[g_ulGuInfoCount].ulInfoType = pstLogGuInfo->ulInfoType;
        NAS_LMM_MEM_CPY(&g_astGuInfo[g_ulGuInfoCount].stGuInfo,
                        &pstLogGuInfo->stGuInfo,
                        sizeof(NAS_GUMM_INFO_STRU));

        g_astGuInfo[g_ulGuInfoCount].ulRslt = pstLogGuInfo->ulRslt;

        g_ulGuInfoCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveGuInfo:export Msg count is large Max count!");
    }

    return VOS_TRUE;
}



MMC_LMM_RESULT_ID_ENUM_UINT32  Stub_Nas_GetGuInfo
(
    NAS_GUMM_INFO_TYPE_ENUM_UINT32  ulInfoType,
    NAS_GUMM_INFO_STRU*        pstGuInfo
)
{
    VOS_UINT32                          ulRslt;
    (void)ulInfoType;
    if (g_ulGuInfoIndex < g_ulGuInfoCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_Nas_GetGuInfo: Export Nas_GetPubInfo info!");

        /*导入出参和返回值*/
        /*ulInfoType = g_astGuInfo[g_ulGuInfoIndex].ulInfoType;*/

        NAS_LMM_MEM_CPY(pstGuInfo,
                        &g_astGuInfo[g_ulGuInfoIndex].stGuInfo,
                        sizeof(NAS_GUMM_INFO_STRU));

        ulRslt = g_astGuInfo[g_ulGuInfoIndex].ulRslt;

        g_ulGuInfoIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_Nas_GetGuInfo: Export Nas_GetPubInfo info ERR!");
        /*ulInfoType = MMC_LMM_INFO_TYPE_BUTT;*/
        NAS_LMM_MEM_SET(pstGuInfo, 0, sizeof(NAS_GUMM_INFO_STRU));
        ulRslt = MMC_LMM_ERR_ID_BUTT;
    }
    return  ulRslt;
}
VOS_UINT32 NAS_REPLAY_SaveMsNetCap(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_MS_NETWORK_CAPACILITY_INFO_STRU          *pstLogMsNetCapInfo;

    pstLogMsNetCapInfo = (NAS_LMM_LOG_MS_NETWORK_CAPACILITY_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstLogMsNetCapInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveMsNetCap:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulMsNetCapCount < NAS_REPLAY_MAX_MS_NET_CAP)
    {
        /*保存回放消息中内容*/
        NAS_LMM_MEM_CPY(&g_astMsNetworkCapability[g_ulMsNetCapCount],
                        &pstLogMsNetCapInfo->stMsNetworkCapbility,
                        sizeof(NAS_MML_MS_NETWORK_CAPACILITY_STRU));

        g_ulMsNetCapCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveGuInfo:export Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_VOID  Stub_NAS_MML_GetMsNetworkCapability
(
    NAS_MML_MS_NETWORK_CAPACILITY_STRU   *pstMsNetCap
)
{
    if (g_ulMsNetCapIndex < g_ulMsNetCapCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetMsNetworkCapability: Export NAS_MML_GetMsNetworkCapability info!");

        /*导入出参和返回值*/
        NAS_LMM_MEM_CPY(pstMsNetCap,
                        &g_astMsNetworkCapability[g_ulMsNetCapIndex],
                        sizeof(NAS_MML_MS_NETWORK_CAPACILITY_STRU));


        g_ulMsNetCapIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetMsNetworkCapability: Export NAS_MML_GetMsNetworkCapability info ERR!");

        NAS_LMM_MEM_SET(pstMsNetCap, 0, sizeof(NAS_MML_MS_NETWORK_CAPACILITY_STRU));

    }
    return;
}
VOS_UINT32 NAS_REPLAY_SaveUeIdTmsi(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_UEIDTMSI_INFO_STRU      *pstLogUeIdTmsiInfo;

    pstLogUeIdTmsiInfo = (NAS_LMM_LOG_UEIDTMSI_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstLogUeIdTmsiInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveUeIdTmsi:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulUeIdTmsiInfoCount < NAS_REPLAY_MAX_UEID_TMSI_INFO)
    {
        /*保存回放消息中内容*/

        NAS_LMM_MEM_CPY(g_astUeIdTmsiInfo[g_ulUeIdTmsiInfoCount].aucUeIdTmsi,
                        pstLogUeIdTmsiInfo->aucUeIdTmsi,
                        4);
        g_ulUeIdTmsiInfoCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveUeIdTmsi:export Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8* Stub_NAS_MML_GetUeIdTmsi
(
   VOS_VOID
)
{
    if (g_ulUeIdTmsiInfoIndex < g_ulUeIdTmsiInfoCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetUeIdTmsi: Export NAS_MML_GetUeIdTmsi info!");

        /*导入返回值*/
        NAS_LMM_MEM_CPY(g_aucTmsi,
                        g_astUeIdTmsiInfo[g_ulUeIdTmsiInfoIndex].aucUeIdTmsi,
                        4);

        g_ulUeIdTmsiInfoIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetUeIdTmsi: Export NAS_MML_GetUeIdTmsi info ERR!");

        NAS_LMM_MEM_SET(g_aucTmsi,0,4);

    }
    return g_aucTmsi;
}
VOS_UINT32 NAS_REPLAY_SaveLteCsSrvCfg(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_CS_SERVICE_FLAG_INFO_STRU      *pstLogCsSrvFlagInfo;

    pstLogCsSrvFlagInfo = (NAS_LMM_LOG_CS_SERVICE_FLAG_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstLogCsSrvFlagInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveLteCsSrvCfg:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulLteCsSrvCfgCount < NAS_REPLAY_MAX_LTE_CS_SRV_CFG)
    {
        /*保存回放消息中内容*/
        g_astLteCsSrvCfg[g_ulLteCsSrvCfgCount] = pstLogCsSrvFlagInfo->ucMmlCsService;

        g_ulLteCsSrvCfgCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveLteCsSrvCfg:export Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetLteCsServiceCfg
(
   VOS_VOID
)
{
    VOS_UINT8                           ucRslt;
    if (g_ulLteCsSrvCfgIndex < g_ulLteCsSrvCfgCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetLteCsServiceCfg: Export NAS_MML_GetUeIdTmsi info!");

        /*导入返回值*/
        ucRslt = g_astLteCsSrvCfg[g_ulLteCsSrvCfgIndex];

        g_ulLteCsSrvCfgIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetLteCsServiceCfg: Export NAS_MML_GetUeIdTmsi info ERR!");

        ucRslt = NAS_MML_LTE_SUPPORT_BUTT;

    }
    return ucRslt;
}
VOS_UINT32 NAS_REPLAY_SaveTinType(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_TIN_INFO_STRU      *pstLogTinInfo;

    pstLogTinInfo = (NAS_LMM_LOG_TIN_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstLogTinInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveTinType:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulTinTypeCount < NAS_REPLAY_MAX_TIN_TYPE)
    {
        /*保存回放消息中内容*/
        g_astTinType[g_ulTinTypeCount] = pstLogTinInfo->ulTinType;

        g_ulTinTypeCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveTinType:export Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_NAS_MML_GetTinType
(
   VOS_VOID
)
{
    VOS_UINT32                           ulRslt;
    if (g_ulTinTypeIndex < g_ulTinTypeCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetLteCsServiceCfg: Export NAS_MML_GetUeIdTmsi info!");

        /*导入返回值*/
        ulRslt = g_astTinType[g_ulTinTypeIndex];

        g_ulTinTypeIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetLteCsServiceCfg: Export NAS_MML_GetUeIdTmsi info ERR!");

        ulRslt = MMC_LMM_TIN_BUTT;

    }
    return ulRslt;
}
VOS_UINT32 NAS_REPLAY_SaveAddUpdateRslt(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_ADD_UPDATE_RSLT_INFO_STRU      *pstLogAddUpsRsltInfo;

    pstLogAddUpsRsltInfo = (NAS_LMM_LOG_ADD_UPDATE_RSLT_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstLogAddUpsRsltInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveAddUpdateRslt:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulAddUpdateRsltCount < NAS_REPLAY_MAX_ADD_UPDATE_RSLT)
    {
        /*保存回放消息中内容*/
        g_astAddUpdateRslt[g_ulAddUpdateRsltCount] = pstLogAddUpsRsltInfo->enAddUpdateRslt;

        g_ulAddUpdateRsltCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveAddUpdateRslt:export Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_NAS_MML_GetAdditionUpdateRslt
(
   VOS_VOID
)
{
    VOS_UINT32                           ulRslt;
    if (g_ulAddUpdateRsltIndex < g_ulAddUpdateRsltCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetAdditionUpdateRslt: Export NAS_MML_GetUeIdTmsi info!");

        /*导入返回值*/
        ulRslt = g_astAddUpdateRslt[g_ulAddUpdateRsltIndex];

        g_ulAddUpdateRsltIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetAdditionUpdateRslt: Export NAS_MML_GetUeIdTmsi info ERR!");

        ulRslt = NAS_LMM_ADDITIONAL_UPDATE_BUTT;

    }
    return ulRslt;
}
VOS_UINT32 NAS_REPLAY_SavePsBearCtx(MsgBlock * pstMsg)
{
    NAS_LMM_LOG_PS_BEAR_CTX_INFO_STRU      *pstLogPsBearCtxInfo;

    pstLogPsBearCtxInfo = (NAS_LMM_LOG_PS_BEAR_CTX_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstLogPsBearCtxInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsBearCtx:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulPsBearCtxCount < NAS_REPLAY_MAX_PS_BEARER_CTX)
    {
        /*保存回放消息中内容*/
        NAS_LMM_MEM_CPY(&g_astPsBearCtx[g_ulPsBearCtxCount],
                        pstLogPsBearCtxInfo->astPsBearerCtx,
                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU));

        g_ulPsBearCtxCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsBearCtx:export Msg count is large Max count!");
    }

    return VOS_TRUE;
}



NAS_MML_PS_BEARER_CONTEXT_STRU *  Stub_NAS_MML_GetPsBearerCtx
(
   VOS_VOID
)
{
    if (g_ulPsBearCtxIndex < g_ulPsBearCtxCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetPsBearerCtx: Import NAS_MML_GetPsBearerCtx info!");

        /*导入返回值*/

        NAS_LMM_MEM_CPY(        g_stPsBearCtx,
                                g_astPsBearCtx[g_ulPsBearCtxIndex].astPsBearerCtx,
                                sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)*NAS_MML_MAX_PS_BEARER_NUM);

        g_ulPsBearCtxIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetPsBearerCtx: Import NAS_MML_GetPsBearerCtx info ERR!");

        NAS_LMM_MEM_SET(        g_stPsBearCtx,
                                0,
                                sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)*NAS_MML_MAX_PS_BEARER_NUM);


    }
    return g_stPsBearCtx;
}
VOS_UINT32 NAS_REPLAY_SavePsBearerExistBeforeIsrAct(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_PSBER_EXIST_BEF_ISRACT_INFO_STRU      *pstBefISRActInfo;

    pstBefISRActInfo = (NAS_LMM_LOG_PSBER_EXIST_BEF_ISRACT_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstBefISRActInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsBearerExistBeforeIsrAct:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulBeIsrActCount < NAS_REPLAY_MAX_PS_BEARER_EXIST_BEFORE_ISR_ACT)
    {
        /*保存回放消息中内容*/
        g_astBefIsrAct[g_ulBeIsrActCount] = pstBefISRActInfo->ulExistFlag;

        g_ulBeIsrActCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsBearerExistBeforeIsrAct:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_NAS_MML_IsPsBearerExistBeforeIsrAct
(
   VOS_VOID
)
{
    VOS_UINT32                          ulRslt;
    if (g_ulBefIsrActIndex < g_ulBeIsrActCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_IsPsBearerExistBeforeIsrAct: Import NAS_MML_IsPsBearerExistBeforeIsrAct info!");

        /*导入返回值*/
        ulRslt = g_astBefIsrAct[g_ulBefIsrActIndex];

        g_ulBefIsrActIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_IsPsBearerExistBeforeIsrAct: Import NAS_MML_IsPsBearerExistBeforeIsrAct info ERR!");

        ulRslt = VOS_FALSE;

    }
    return ulRslt;
}
VOS_UINT32 NAS_REPLAY_SavePsBearerAfterIsrActExist(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_PSBER_EXIST_AFTER_ISRACT_INFO_STRU      *pstAftISRActInfo;

    pstAftISRActInfo = (NAS_LMM_LOG_PSBER_EXIST_AFTER_ISRACT_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstAftISRActInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsBearerAfterIsrActExist:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulAfterIsrActExistCount < NAS_REPLAY_MAX_PS_BEARER_AFTER_ISR_ACT_EXIST)
    {
        /*保存回放消息中内容*/
        g_astAfterIsrActExist[g_ulAfterIsrActExistCount] = pstAftISRActInfo->ulExistFlag;

        g_ulAfterIsrActExistCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsBearerAfterIsrActExist:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_NAS_MML_IsPsBearerAfterIsrActExist
(
   VOS_VOID
)
{
    VOS_UINT32                          ulRslt;
    if (g_ulAfterIsrActExistIndex < g_ulAfterIsrActExistCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_IsPsBearerAfterIsrActExist: Import NAS_MML_IsPsBearerAfterIsrActExist info!");

        /*导入返回值*/
        ulRslt = g_astAfterIsrActExist[g_ulAfterIsrActExistIndex];

        g_ulAfterIsrActExistIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_IsPsBearerAfterIsrActExist: Import NAS_MML_IsPsBearerAfterIsrActExist info ERR!");

        ulRslt = VOS_FALSE;

    }
    return ulRslt;
}
VOS_UINT32 NAS_REPLAY_SaveEmcNumList(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_EMERGENCY_NUMLIST_INFO_STRU      *pstEmcNumListInfo;

    pstEmcNumListInfo = (NAS_LMM_LOG_EMERGENCY_NUMLIST_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstEmcNumListInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveEmcNumList:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulEmcNumListCount < NAS_REPLAY_MAX_EMC_NUM_LIST)
    {
        /*保存回放消息中内容*/
        NAS_LMM_MEM_CPY(&g_astEmcNumList[g_ulEmcNumListCount].stMmlEmerNumList,
                        &pstEmcNumListInfo->stMmlEmerNumList,
                        sizeof(NAS_MML_EMERGENCY_NUM_LIST_STRU));

        g_ulEmcNumListCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveEmcNumList:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



NAS_MML_EMERGENCY_NUM_LIST_STRU*  Stub_NAS_MML_GetEmergencyNumList
(
   VOS_VOID
)
{

    if (g_ulEmcNumListIndex < g_ulEmcNumListCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetEmergencyNumList: Import NAS_MML_GetEmergencyNumList info!");

        /*导入返回值*/
        NAS_LMM_MEM_CPY(&g_stEmcNumList,
                        &g_astEmcNumList[g_ulEmcNumListIndex].stMmlEmerNumList,
                       sizeof(NAS_MML_EMERGENCY_NUM_LIST_STRU));

        g_ulEmcNumListIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetEmergencyNumList: Import NAS_MML_GetEmergencyNumList info ERR!");

        NAS_LMM_MEM_SET(&g_stEmcNumList,
                        0,
                       sizeof(NAS_MML_EMERGENCY_NUM_LIST_STRU));

    }
    return &g_stEmcNumList;
}
VOS_UINT32 NAS_REPLAY_SavePsRegContainDrxPara(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_PSREG_CONTAINDRX_INFO_STRU       *pstDrxPara;

    pstDrxPara = (NAS_LMM_LOG_PSREG_CONTAINDRX_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstDrxPara)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsRegContainDrxPara:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulPsRegContainDrxParaCount < NAS_REPLAY_MAX_PS_REG_CONTAIN_DRX_PARA)
    {
        /*保存回放消息中内容*/
        g_astPsRegContainDrxPara[g_ulPsRegContainDrxParaCount] = pstDrxPara->enPsRegisterContainDrx;

        g_ulPsRegContainDrxParaCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsRegContainDrxPara:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetPsRegContainDrx
(
   VOS_VOID
)
{
    NAS_MML_PS_REG_CONTAIN_DRX_PARA_ENUM_UINT8     ucPsDrxPara;
    if (g_ulPsRegContainDrxParaIndex < g_ulPsRegContainDrxParaCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetPsRegContainDrx: Import NAS_MML_GetPsRegContainDrx info!");

        /*导入返回值*/
        ucPsDrxPara = g_astPsRegContainDrxPara[g_ulPsRegContainDrxParaIndex];

        g_ulPsRegContainDrxParaIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetPsRegContainDrx: Import NAS_MML_GetPsRegContainDrx info ERR!");

        ucPsDrxPara = NAS_MML_PS_REG_CONTAIN_BUTT;

    }
    return ucPsDrxPara;
}
VOS_UINT32 NAS_REPLAY_SaveUeEutranPsDrxLen(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_UE_EUTRAN_PS_DRX_LEN_INFO_STRU       *pstPsDrxLen;

    pstPsDrxLen = (NAS_LMM_LOG_UE_EUTRAN_PS_DRX_LEN_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstPsDrxLen)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveUeEutranPsDrxLen:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulUeEutranPsDrxLenCount < NAS_REPLAY_MAX_EUTRAN_DRX_LEN)
    {
        /*保存回放消息中内容*/
        g_astUeEutranPsDrxLen[g_ulUeEutranPsDrxLenCount] = pstPsDrxLen->ucEutranDrxLen;

        g_ulUeEutranPsDrxLenCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveUeEutranPsDrxLen:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetUeEutranPsDrxLen
(
   VOS_VOID
)
{
    VOS_UINT8                               ucEutranPsDrxLen;
    if (g_ulUeEutranPsDrxLenIndex < g_ulUeEutranPsDrxLenCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetUeEutranPsDrxLen: Import NAS_MML_GetUeEutranPsDrxLen info!");

        /*导入返回值*/
        ucEutranPsDrxLen = g_astUeEutranPsDrxLen[g_ulUeEutranPsDrxLenIndex];

        g_ulUeEutranPsDrxLenIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetUeEutranPsDrxLen: Import NAS_MML_GetUeEutranPsDrxLen info ERR!");

        ucEutranPsDrxLen = 0xff;

    }
    return ucEutranPsDrxLen;
}
VOS_UINT32 NAS_REPLAY_SaveNonDrxTimer(MsgBlock * pstMsg)
{

   NAS_LMM_LOG_NON_DRX_TIMER_INFO_STRU       *pstPsDrxTimer;

    pstPsDrxTimer = (NAS_LMM_LOG_NON_DRX_TIMER_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstPsDrxTimer)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveNonDrxTimer:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulNonDrxTimerCount < NAS_REPLAY_MAX_NON_DRX_TIMER)
    {
        /*保存回放消息中内容*/
        g_astNonDrxTimer[g_ulNonDrxTimerCount] = pstPsDrxTimer->ucNonDrxTimer;

        g_ulNonDrxTimerCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveUeEutranPsDrxLen:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetNonDrxTimer
(
   VOS_VOID
)
{
    VOS_UINT8                               ucEutranPsDrxTimer;
    if (g_ulNonDrxTimerIndex < g_ulNonDrxTimerCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetNonDrxTimer: Import NAS_MML_GetNonDrxTimer info!");

        /*导入返回值*/
        ucEutranPsDrxTimer = g_astNonDrxTimer[g_ulNonDrxTimerIndex];

        g_ulNonDrxTimerIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetNonDrxTimer: Import NAS_MML_GetNonDrxTimer info ERR!");

        ucEutranPsDrxTimer = 0xff;

    }
    return ucEutranPsDrxTimer;
}
VOS_UINT32 NAS_REPLAY_SaveSplitPgCycleCode(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_DRX_SPLIT_PG_CYCLE_CODE_INFO_STRU       *pstPsDrxPgCode;

    pstPsDrxPgCode = (NAS_LMM_LOG_DRX_SPLIT_PG_CYCLE_CODE_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstPsDrxPgCode)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveSplitPgCycleCode:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulSplitPgCycleCodeCount < NAS_REPLAY_MAX_SPLIT_PG_CYCLE_CODE)
    {
        /*保存回放消息中内容*/
        g_astSplitPgCycleCode[g_ulSplitPgCycleCodeCount] = pstPsDrxPgCode->ucSplitPgCycleCode;

        g_ulSplitPgCycleCodeCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveSplitPgCycleCode:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetSplitPgCycleCode
(
   VOS_VOID
)
{
    VOS_UINT8                               ucPsDrxPgCode;
    if (g_ulSplitPgCycleCodeIndex < g_ulSplitPgCycleCodeCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetSplitPgCycleCode: Import NAS_MML_GetSplitPgCycleCode info!");

        /*导入返回值*/
        ucPsDrxPgCode = g_astSplitPgCycleCode[g_ulSplitPgCycleCodeIndex];

        g_ulSplitPgCycleCodeIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetSplitPgCycleCode: Import NAS_MML_GetSplitPgCycleCode info ERR!");

        ucPsDrxPgCode = 0xff;

    }
    return ucPsDrxPgCode;
}
VOS_UINT32 NAS_REPLAY_SaveSplitOnCcch(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_DRX_SPLIT_ON_CCCH_INFO_STRU       *pstSplitOnCcch;

    pstSplitOnCcch = (NAS_LMM_LOG_DRX_SPLIT_ON_CCCH_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstSplitOnCcch)
    {
        NAS_LMM_PUBM_LOG_INFO(":NAS_REPLAY_SaveSplitOnCcch:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulSplitOnCcchCount < NAS_REPLAY_MAX_SPLIT_ON_CCCH)
    {
        /*保存回放消息中内容*/
        g_astSplitOnCcch[g_ulSplitOnCcchCount] = pstSplitOnCcch->ucSplitCcch;

        g_ulSplitOnCcchCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO(":NAS_REPLAY_SaveSplitOnCcch:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetSplitOnCcch
(
   VOS_VOID
)
{
    VOS_UINT8                               ucPsDrxOnCcch;
    if (g_ulSplitOnCcchIndex < g_ulSplitOnCcchCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetSplitOnCcch: Import NAS_MML_GetSplitOnCcch info!");

        /*导入返回值*/
        ucPsDrxOnCcch = g_astSplitOnCcch[g_ulSplitOnCcchIndex];

        g_ulSplitOnCcchIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetSplitOnCcch: Import NAS_MML_GetSplitOnCcch info ERR!");

        ucPsDrxOnCcch = 0xff;

    }
    return ucPsDrxOnCcch;
}
VOS_UINT32 NAS_REPLAY_SaveMsClassMark2(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_CLASSMARK2_INFO_STRU       *pstClassMark2;

    pstClassMark2 = (NAS_LMM_LOG_CLASSMARK2_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstClassMark2)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_Save MsClassMark2:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulMsClassMark2Count < NAS_REPLAY_MAX_MS_CLASSMARK2)
    {
        /*保存回放消息中内容*/
        NAS_LMM_MEM_CPY(g_astMsClassMark2[g_ulMsClassMark2Count].aucMsClassMark2,
                        pstClassMark2->aucMsClassMark2,
                        NAS_MML_CLASSMARK2_LEN);

        g_ulMsClassMark2Count ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("::NAS_REPLAY_Save MsClassMark2:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_VOID  Stub_NAS_MML_Fill_IE_ClassMark2
(
   VOS_UINT8  *pClassMark2
)
{
    if (g_ulMsClassMark2Index < g_ulMsClassMark2Count)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_Fill_IE_ClassMark2: Import NAS_MML_Fill_IE_ClassMark2 info!");

        /*导入出参*/
        NAS_LMM_MEM_CPY(    pClassMark2,
                            g_astMsClassMark2[g_ulMsClassMark2Index].aucMsClassMark2,
                            NAS_MML_CLASSMARK2_LEN);


        g_ulMsClassMark2Index ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_Fill_IE_ClassMark2: Import NAS_MML_Fill_IE_ClassMark2 info ERR!");

        NAS_LMM_MEM_SET(    pClassMark2,
                            0,
                            NAS_MML_CLASSMARK2_LEN);

    }
    return;
}
VOS_UINT32 NAS_REPLAY_SaveCsAttachAllowFlg(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_CS_ATTACH_ALLOW_FLG_INFO_STRU       *pstCsAttAllowFlag;

    pstCsAttAllowFlag = (NAS_LMM_LOG_CS_ATTACH_ALLOW_FLG_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstCsAttAllowFlag)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveCsAttachAllowFlg:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulCsAttachAllowFlgCount < NAS_REPLAY_MAX_CS_ATTACH_ALLOW_FLG)
    {
        /*保存回放消息中内容*/
        g_astCsAttachAllowFlg[g_ulCsAttachAllowFlgCount] = pstCsAttAllowFlag->ucCsAttachAllow;

        g_ulCsAttachAllowFlgCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveCsAttachAllowFlg:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetCsAttachAllowFlg
(
   VOS_VOID
)
{
    VOS_UINT8                               ucCsAttAllowFlag;
    if (g_ulCsAttachAllowFlgIndex < g_ulCsAttachAllowFlgCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetCsAttachAllowFlg: Import NAS_MML_GetCsAttachAllowFlg info!");

        /*导入返回值*/
        ucCsAttAllowFlag  =  g_astCsAttachAllowFlg[g_ulCsAttachAllowFlgIndex];

        g_ulCsAttachAllowFlgIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetCsAttachAllowFlg: Import NAS_MML_GetCsAttachAllowFlg info ERR!");

        ucCsAttAllowFlag = VOS_FALSE;
    }
    return ucCsAttAllowFlag;
}
VOS_UINT32 NAS_REPLAY_SavePsAttachAllowFlg(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_PS_ATTACH_ALLOW_FLG_INFO_STRU       *pstPsAttAllowFlag;

    pstPsAttAllowFlag = (NAS_LMM_LOG_PS_ATTACH_ALLOW_FLG_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstPsAttAllowFlag)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsAttachAllowFlg:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulPsAttachAllowFlgCount < NAS_REPLAY_MAX_PS_ATTACH_ALLOW_FLG)
    {
        /*保存回放消息中内容*/
        g_astPsAttachAllowFlg[g_ulPsAttachAllowFlgCount] = pstPsAttAllowFlag->ucPsAttachAllow;

        g_ulPsAttachAllowFlgCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsAttachAllowFlg:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetPsAttachAllowFlg
(
   VOS_VOID
)
{
    VOS_UINT8                               ucPsAttAllowFlag;
    if (g_ulPsAttachAllowFlgIndex < g_ulPsAttachAllowFlgCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetPsAttachAllowFlg: Import NAS_MML_GetPsAttachAllowFlg info!");

        /*导入返回值*/
        ucPsAttAllowFlag  =  g_astPsAttachAllowFlg[g_ulPsAttachAllowFlgIndex];

        g_ulPsAttachAllowFlgIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetPsAttachAllowFlg: Import NAS_MML_GetPsAttachAllowFlg info ERR!");

        ucPsAttAllowFlag = VOS_FALSE;
    }
    return ucPsAttAllowFlag;
}
VOS_UINT32 NAS_REPLAY_SaveMsMode(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_MS_MODE_INFO_STRU       *pstMsMode;

    pstMsMode = (NAS_LMM_LOG_MS_MODE_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstMsMode)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveMsMode:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulMsModeCount < NAS_REPLAY_MAX_MS_MODE)
    {
        /*保存回放消息中内容*/
        g_astMsMode[g_ulMsModeCount] = pstMsMode->ucMsMode;

        g_ulMsModeCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveMsMode:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetMsMode
(
   VOS_VOID
)
{
    NAS_MML_MS_MODE_ENUM_UINT8          ucMsMode;
    if (g_ulMsModeIndex < g_ulMsModeCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetMsMode: Import NAS_MML_GetMsMode info!");

        /*导入返回值*/
        ucMsMode  =  g_astMsMode[g_ulMsModeIndex];

        g_ulMsModeIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetMsMode: Import NAS_MML_GetMsMode info ERR!");

        ucMsMode = NAS_MML_MS_MODE_BUTT;
    }
    return ucMsMode;
}
VOS_UINT32 NAS_REPLAY_SavePlmnIdIsForbid(MsgBlock * pstMsg)
{

    NAS_LMM_LOG_PLMNID_IS_FOBID_INFO_STRU       *pstPlmnIdIsForbid;

    pstPlmnIdIsForbid = (NAS_LMM_LOG_PLMNID_IS_FOBID_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstPlmnIdIsForbid)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePlmnIdIsForbid:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulPlmnIdIsForbidCount < NAS_REPLAY_MAX_PLMNID_IS_FORBID)
    {
        /*保存回放消息中内容*/
        NAS_LMM_MEM_CPY(&g_astPlmnIdIsForbid[g_ulPlmnIdIsForbidCount].stPlmn,
                        &pstPlmnIdIsForbid->stPlmn,
                        sizeof(MMC_LMM_PLMN_ID_STRU));
        g_astPlmnIdIsForbid[g_ulPlmnIdIsForbidCount].ulRslt = pstPlmnIdIsForbid->ulRslt;
        g_ulPlmnIdIsForbidCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePlmnIdIsForbid:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_Nas_PlmnIdIsForbid
(
   MMC_LMM_PLMN_ID_STRU  *pstPlmnId
)
{
    MMC_LMM_PLMN_CTRL_ENUM_UINT32       ulRslt;

    (void)pstPlmnId;
    if (g_ulPlmnIdIsForbidIndex < g_ulPlmnIdIsForbidCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_Nas_PlmnIdIsForbid: Import Nas_PlmnIdIsForbid info!");

        /*导入返回值*/
        ulRslt = g_astPlmnIdIsForbid[g_ulPlmnIdIsForbidIndex].ulRslt;
        g_ulPlmnIdIsForbidIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_Nas_PlmnIdIsForbid: Import Nas_PlmnIdIsForbid info ERR!");

        ulRslt = MMC_LMM_PLMN_CTRL_BUTT;
    }
    return ulRslt;
}
VOS_UINT32 NAS_REPLAY_SavePlmnExactlyCmpFlg(MsgBlock * pstMsg)
{
    NAS_LMM_LOG_PLMN_EXACTLY_COMPARE_INFO_STRU       *pstPlmnCom;

    pstPlmnCom = (NAS_LMM_LOG_PLMN_EXACTLY_COMPARE_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstPlmnCom)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePlmnExactlyCmpFlg:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulPlmnExactlyCmpFlgCount < NAS_REPLAY_MAX_PLMN_EXACTLY_CMP_FLG)
    {
        /*保存回放消息中内容*/
        g_astPlmnExactlyCmpFlg[g_ulPlmnExactlyCmpFlgCount] = pstPlmnCom->ucRslt;

        g_ulPlmnExactlyCmpFlgCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePlmnExactlyCmpFlg:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetPlmnExactlyComparaFlg
(
   VOS_VOID
)
{
    VOS_UINT8                           ucRslt;
    if (g_ulPlmnExactlyCmpFlgIndex < g_ulPlmnExactlyCmpFlgCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetPlmnExactlyComparaFlg: Import NAS_MML_GetPlmnExactlyComparaFlg info!");

        /*导入返回值*/
        ucRslt  =  g_astPlmnExactlyCmpFlg[g_ulPlmnExactlyCmpFlgIndex];

        g_ulPlmnExactlyCmpFlgIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetPlmnExactlyComparaFlg: Import NAS_MML_GetPlmnExactlyComparaFlg info ERR!");

        ucRslt = VOS_FALSE;
    }
    return ucRslt;
}
VOS_UINT32 NAS_REPLAY_SavePsRegStatus(MsgBlock * pstMsg)
{
    NAS_LMM_LOG_PS_REG_STATUS_INFO_STRU       *pstPsRegStatus;

    pstPsRegStatus = (NAS_LMM_LOG_PS_REG_STATUS_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstPsRegStatus)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsRegStatus:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulPsRegStatusCount < NAS_REPLAY_MAX_PS_REG_STATUS)
    {
        /*保存回放消息中内容*/
        g_astPsRegStatus[g_ulPsRegStatusCount] = pstPsRegStatus->ucPsRegStatus;

        g_ulPsRegStatusCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePsRegStatus:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetPsRegStatus
(
   VOS_VOID
)
{
    NAS_MML_REG_STATUS_ENUM_UINT8       ucPsRegStatus;
    if (g_ulPsRegStatusIndex < g_ulPsRegStatusCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetPsRegStatus: Import NAS_MML_GetPsRegStatus info!");

        /*导入返回值*/
        ucPsRegStatus  =  g_astPsRegStatus[g_ulPsRegStatusIndex];

        g_ulPsRegStatusIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetPsRegStatus: Import NAS_MML_GetPsRegStatus info ERR!");

        ucPsRegStatus = NAS_MML_REG_STATUS_BUTT;
    }
    return ucPsRegStatus;
}
VOS_UINT32 NAS_REPLAY_SaveSimType(MsgBlock * pstMsg)
{
    NAS_LMM_LOG_SIM_TYPE_INFO_STRU       *pstSimType;

    pstSimType = (NAS_LMM_LOG_SIM_TYPE_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstSimType)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveSimType:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulSimTypeCount < NAS_REPLAY_MAX_SIM_TYPE)
    {
        /*保存回放消息中内容*/
        g_astSimType[g_ulSimTypeCount] = pstSimType->ucSimType;

        g_ulSimTypeCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveSimType:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT8  Stub_NAS_MML_GetSimType
(
   VOS_VOID
)
{
    NAS_MML_SIM_TYPE_ENUM_UINT8         ucSimType;
    if (g_ulSimTypeIndex < g_ulSimTypeCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_MML_GetSimType: Import NAS_MML_GetSimType info!");

        /*导入返回值*/
        ucSimType  =  g_astSimType[g_ulSimTypeIndex];

        g_ulSimTypeIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_MML_GetSimType: Import NAS_MML_GetSimType info ERR!");

        ucSimType = NAS_MML_SIM_TYPE_BUTT;
    }
    return ucSimType;
}
VOS_UINT32 NAS_REPLAY_SaveKdfKey(MsgBlock * pstMsg)
{
    NAS_LMM_LOG_KDF_KEY_INFO_STRU       *pstKdfKey;

    pstKdfKey = (NAS_LMM_LOG_KDF_KEY_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstKdfKey)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveKdfKey:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulKdfKeyCount < NAS_REPLAY_MAX_KDF_KEY)
    {
        /*保存回放消息中内容*/
        NAS_LMM_MEM_CPY(&g_astKdfKey[g_ulKdfKeyCount].stBspInputPara,
                        &pstKdfKey->stBspInputPara,
                        sizeof(NAS_LMM_BSP_INPUT_PARA_STRU));
        NAS_LMM_MEM_CPY(&g_astKdfKey[g_ulKdfKeyCount].stBspOutputPara,
                        &pstKdfKey->stBspOutputPara,
                        sizeof(NAS_LMM_BSP_OUTPUT_PARA_STRU));

        g_astKdfKey[g_ulKdfKeyCount].lRslt = pstKdfKey->lRslt;
        g_ulKdfKeyCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveKdfKey:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_INT32  Stub_BSP_KDF_KeyMake
(
    NAS_LMM_BSP_INPUT_PARA_STRU         *pstBspInputPara,
    NAS_LMM_BSP_OUTPUT_PARA_STRU        *pstBspOutputPara
)
{
    VOS_INT32                           lRslt;
    (void)pstBspInputPara;
    if (g_ulKdfKeyIndex < g_ulKdfKeyCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_BSP_KDF_KeyMake: Import BSP_KDF_KeyMake info!");

        /*导入出参和返回值*/
        /*NAS_LMM_MEM_CPY(pstBspInputPara,
                        &g_astKdfKey[g_ulKdfKeyIndex].stBspInputPara,
                        sizeof(NAS_LMM_BSP_INPUT_PARA_STRU));*/
        NAS_LMM_MEM_CPY(pstBspOutputPara,
                        &g_astKdfKey[g_ulKdfKeyIndex].stBspOutputPara,
                        sizeof(NAS_LMM_BSP_OUTPUT_PARA_STRU));

        lRslt = g_astKdfKey[g_ulKdfKeyIndex].lRslt;

        g_ulKdfKeyIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_BSP_KDF_KeyMake: Import BSP_KDF_KeyMake info ERR!");

        /*NAS_LMM_MEM_CPY(pstBspInputPara,
                        0,
                        sizeof(NAS_LMM_BSP_INPUT_PARA_STRU));*/
        NAS_LMM_MEM_SET(pstBspOutputPara,
                        0,
                        sizeof(NAS_LMM_BSP_OUTPUT_PARA_STRU));

        lRslt = BSP_ERROR;
    }
    return lRslt;
}


VOS_UINT32 NAS_REPLAY_SaveSdfPara(MsgBlock * pstMsg)
{
    NAS_OM_LOG_SDF_PARA_INFO_STRU       *pstSdfPara;

    pstSdfPara = (NAS_OM_LOG_SDF_PARA_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstSdfPara)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveSdfPara:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulSdfParaCount < NAS_REPLAY_MAX_SDF_PARA)
    {
        /*保存回放消息中内容*/
        g_astSdfPara[g_ulSdfParaCount].ulSdfNum = pstSdfPara->ulSdfNum;
        NAS_LMM_MEM_CPY(&g_astSdfPara[g_ulSdfParaCount].stSdfPara,
                        &pstSdfPara->stSdfPara,
                        sizeof(APP_ESM_SDF_PARA_STRU));
        g_astSdfPara[g_ulKdfKeyCount].ulRslt = pstSdfPara->ulRslt;
        g_ulSdfParaCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveSdfPara:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_APP_GetSdfPara
(
    VOS_UINT32                         *pulSdfNum,
    APP_ESM_SDF_PARA_STRU              *pstSdfPara
)
{
    VOS_UINT32                           ulRslt;
    if (g_ulSdfParaIndex < g_ulSdfParaCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_APP_GetSdfPara: Import APP_GetSdfPara info!");

        /*导入返回值*/
        *pulSdfNum = g_astSdfPara[g_ulSdfParaIndex].ulSdfNum;
        NAS_LMM_MEM_CPY(pstSdfPara,
                        &g_astSdfPara[g_ulSdfParaIndex].stSdfPara,
                        sizeof(APP_ESM_SDF_PARA_STRU));

        ulRslt = g_astSdfPara[g_ulSdfParaIndex].ulRslt;

        g_ulSdfParaIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_APP_GetSdfPara: Import APP_GetSdfPara info ERR!");

        pulSdfNum = 0;
        NAS_LMM_MEM_SET(pstSdfPara,
                        0,
                        sizeof(APP_ESM_SDF_PARA_STRU));
        ulRslt = APP_FAILURE;
    }
    return ulRslt;
}
VOS_UINT32 NAS_REPLAY_SavePdpManageInfo(MsgBlock * pstMsg)
{
    NAS_OM_LOG_PDP_MANAGER_INFO_STRU       *pstPdpManageInfo;

    pstPdpManageInfo = (NAS_OM_LOG_PDP_MANAGER_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstPdpManageInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePdpManageInfo:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulPdpManageInfoCount < NAS_REPLAY_MAX_PDP_MANAGE_INFO)
    {
        /*保存回放消息中内容*/
        NAS_LMM_MEM_CPY(&g_astPdpManageInfo[g_ulPdpManageInfoCount].stPdpManageInfo,
                        &pstPdpManageInfo->stPdpManageInfo,
                        sizeof(APP_ESM_PDP_MANAGE_INFO_STRU));
        g_astPdpManageInfo[g_ulPdpManageInfoCount].ulRslt = pstPdpManageInfo->ulRslt;
        g_ulPdpManageInfoCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SavePdpManageInfo:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_APP_GetPdpManageInfo
(
    APP_ESM_PDP_MANAGE_INFO_STRU  *pstPdpManageInfo
)
{
    VOS_UINT32                           ulRslt;
    if (g_ulPdpManageInfoIndex < g_ulPdpManageInfoCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_AAPP_GetPdpManageInfo: Import APP_GetPdpManageInfo info!");

        /*导入返回值*/
        NAS_LMM_MEM_CPY(pstPdpManageInfo,
                        &g_astPdpManageInfo[g_ulPdpManageInfoIndex].stPdpManageInfo,
                        sizeof(APP_ESM_PDP_MANAGE_INFO_STRU));

        ulRslt = g_astPdpManageInfo[g_ulPdpManageInfoIndex].ulRslt;

        g_ulPdpManageInfoIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_APP_GetPdpManageInfo: Import APP_GetPdpManageInfo info ERR!");

        NAS_LMM_MEM_SET(pstPdpManageInfo,
                        0,
                        sizeof(APP_ESM_PDP_MANAGE_INFO_STRU));
        ulRslt = APP_FAILURE;
    }
    return ulRslt;
}

#if 0
VOS_UINT32 NAS_REPLAY_SaveErabmUpDataPending(MsgBlock * pstMsg)
{
    NAS_OM_LOG_ERABM_DATA_PENDING_INFO_STRU       *pstDataPending;

    pstDataPending = (NAS_OM_LOG_ERABM_DATA_PENDING_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstDataPending)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveErabmUpDataPending:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulErabmUpDataPendingCount < NAS_REPLAY_MAX_ERABM_UP_DATA_PENDING)
    {
        /*保存回放消息中内容*/
        g_astErabmUpDataPending[g_ulErabmUpDataPendingCount] = pstDataPending->ulRslt;
        g_ulErabmUpDataPendingCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveErabmUpDataPending:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_NAS_ERABM_IsDataPending
(
    VOS_VOID
)
{
    EMM_ERABM_UP_DATA_PENDING_ENUM_UINT32         ulRslt;
    if (g_ulErabmUpDataPendingIndex < g_ulErabmUpDataPendingCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_ERABM_IsDataPending: Import NAS_ERABM_IsDataPending info!");

        /*导入返回值*/
        ulRslt = g_astErabmUpDataPending[g_ulErabmUpDataPendingIndex];

        g_ulErabmUpDataPendingIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_ERABM_IsDataPending: Import NAS_ERABM_IsDataPending info ERR!");

        ulRslt = EMM_ERABM_UP_DATA_PENDING_BUTT;
    }
    return ulRslt;
}
VOS_VOID NAS_LMM_LogIsDataPending
(
    VOS_UINT32  ulRslt
)
{
    NAS_OM_LOG_ERABM_DATA_PENDING_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_IS_DATA_PENDING;

    pstMsg = (NAS_OM_LOG_ERABM_DATA_PENDING_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_OM_LOG_ERABM_DATA_PENDING_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogIsDataPending: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid             = PS_PID_ESM;
    pstMsg->ulReceiverPid           = PS_PID_MM;
    pstMsg->ulLength                = sizeof(NAS_OM_LOG_ERABM_DATA_PENDING_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName               = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ulRslt                  = ulRslt;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}



VOS_UINT32 NAS_REPLAY_SaveLteRatIsExist(MsgBlock * pstMsg)
{
    NAS_OM_LOG_LTE_RAT_EXIST_INFO_STRU       *pstLteRatInfo;

    pstLteRatInfo = (NAS_OM_LOG_LTE_RAT_EXIST_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstLteRatInfo)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveLteRatIsExist:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulLteRatIsExistCount < NAS_REPLAY_MAX_LTE_RAT_IS_EXIST)
    {
        /*保存回放消息中内容*/
        g_astLteRatIsExist[g_ulLteRatIsExistCount] = pstLteRatInfo->ulRslt;
        g_ulLteRatIsExistCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveLteRatIsExist:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_NAS_EMM_lteRatIsExist
(
    VOS_VOID
)
{
    VOS_UINT32         ulRslt;
    if (g_ulLteRatIsExistIndex < g_ulLteRatIsExistCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_EMM_lteRatIsExist: Import NAS_EMM_lteRatIsExist info!");

        /*导入返回值*/
        ulRslt = g_astLteRatIsExist[g_ulErabmUpDataPendingIndex];

        g_ulLteRatIsExistIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_EMM_lteRatIsExist: Import NAS_EMM_lteRatIsExist info ERR!");

        ulRslt = 0;
    }
    return ulRslt;
}
VOS_VOID NAS_LMM_LogLteRatIsExist
(
    VOS_UINT32  ulRslt
)
{
    NAS_OM_LOG_LTE_RAT_EXIST_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_LTE_RAT_IS_EXIST_IND;

    pstMsg = (NAS_OM_LOG_LTE_RAT_EXIST_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_OM_LOG_LTE_RAT_EXIST_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogLteRatIsExist: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid             = PS_PID_MM;
    pstMsg->ulReceiverPid           = PS_PID_MM;
    pstMsg->ulLength                = sizeof(NAS_OM_LOG_LTE_RAT_EXIST_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName               = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ulRslt                  = ulRslt;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}



VOS_UINT32 NAS_REPLAY_SaveUpSignalPending(MsgBlock * pstMsg)
{
    NAS_OM_LOG_ESM_UPSIG_PENDING_INFO_STRU       *pstSigPending;

    pstSigPending = (NAS_OM_LOG_ESM_UPSIG_PENDING_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstSigPending)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveUpSignalPending:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulUpSignalPendingCount < NAS_REPLAY_MAX_UP_SIGNAL_PENDING)
    {
        /*保存回放消息中内容*/
        g_astUpSignalPending[g_ulUpSignalPendingCount] = pstSigPending->ulRslt;
        g_ulUpSignalPendingCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveUpSignalPending:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_NAS_ESM_IsUpSingnalPending
(
    VOS_VOID
)
{
    EMM_ESM_UP_SINGNAL_PENDING_ENUM_UINT32         ulRslt;
    if (g_ulUpSignalPendingIndex < g_ulUpSignalPendingCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_NAS_ESM_IsUpSingnalPending: Import NAS_ESM_IsUpSingnalPending info!");

        /*导入返回值*/
        ulRslt = g_astUpSignalPending[g_ulUpSignalPendingIndex];

        g_ulUpSignalPendingIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_NAS_ESM_IsUpSingnalPending: Import NAS_ESM_IsUpSingnalPending info ERR!");

        ulRslt = EMM_ESM_UP_SINGNAL_PENDING_BUTT;
    }
    return ulRslt;
}
VOS_VOID NAS_LMM_LogIsUpSigPend
(
    EMM_ESM_UP_SINGNAL_PENDING_ENUM_UINT32  enUpSigPend
)
{
    NAS_OM_LOG_ESM_UPSIG_PENDING_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_ISUP_SIG_PENDING_IND;

    pstMsg = (NAS_OM_LOG_ESM_UPSIG_PENDING_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_OM_LOG_ESM_UPSIG_PENDING_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogIsUpSigPend: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = PS_PID_ESM;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_OM_LOG_ESM_UPSIG_PENDING_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ulRslt                 = enUpSigPend;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}
#endif



VOS_UINT32 NAS_REPLAY_SaveCardImsi(MsgBlock * pstMsg)
{
    NAS_LMM_LOG_CARD_IMSI_INFO_STRU       *pstCardImsi;

    pstCardImsi = (NAS_LMM_LOG_CARD_IMSI_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstCardImsi)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveCardImsi:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulCardImsiCount < NAS_REPLAY_MAX_CARD_IMSI)
    {
        /*保存回放消息中内容*/
        NAS_LMM_MEM_CPY(g_astCardImsi[g_ulCardImsiCount].aucImsi,
                        pstCardImsi->aucImsi,
                        9);
        g_astCardImsi[g_ulCardImsiCount].ulRslt = pstCardImsi->ulRslt;
        g_ulCardImsiCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveCardImsi:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_USIMM_GetCardIMSI
(
    VOS_UINT8      *pucImsi
)
{
    VOS_UINT32         ulRslt;
    if (g_ulCardImsiIndex < g_ulCardImsiCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_USIMM_GetCardIMSI: Import USIMM_GetCardIMSI info!");

        /*导入出参和返回值*/
        NAS_LMM_MEM_CPY(pucImsi,
                        g_astCardImsi[g_ulCardImsiIndex].aucImsi,
                        9);
        ulRslt = g_astCardImsi[g_ulCardImsiIndex].ulRslt;

        g_ulCardImsiIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_USIMM_GetCardIMSI: Import USIMM_GetCardIMSI info ERR!");

        ulRslt = USIMM_API_RESULT_BUTT;
    }
    return ulRslt;
}
VOS_UINT32 NAS_REPLAY_SaveServiceAvailable(MsgBlock * pstMsg)
{
    NAS_LMM_LOG_USIMM_SERVICE_INFO_STRU       *pstUsimSer;

    pstUsimSer = (NAS_LMM_LOG_USIMM_SERVICE_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstUsimSer)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveServiceAvailable:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulServiceAvailableCount < NAS_REPLAY_MAX_SERVICE_AVAILABLE)
    {
        /*保存回放消息中内容*/
        g_astServiceAvailable[g_ulServiceAvailableCount].enService = pstUsimSer->enService;
        g_astServiceAvailable[g_ulServiceAvailableCount].ulRslt = pstUsimSer->ulRslt;
        g_ulServiceAvailableCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveServiceAvailable:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_USIMM_IsServiceAvailable
(
    UICC_SERVICES_TYPE_ENUM_UINT32  ulServiceType
)
{
    VOS_UINT32         ulRslt;
    (void)ulServiceType;
    if (g_ulServiceAvailableIndex < g_ulServiceAvailableCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_USIMM_IsServiceAvailable: Import USIMM_IsServiceAvailable info!");

        /*导入返回值*/

        ulRslt = g_astServiceAvailable[g_ulServiceAvailableIndex].ulRslt;

        g_ulServiceAvailableIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_USIMM_IsServiceAvailable: Import USIMM_IsServiceAvailable info ERR!");

        ulRslt = USIMM_API_RESULT_BUTT;
    }
    return ulRslt;
}
VOS_UINT32 NAS_REPLAY_SaveUsimTestMode(MsgBlock * pstMsg)
{
    NAS_LMM_LOG_USIM_TEST_MODE_INFO_STRU       *pstUsimTestMode;

    pstUsimTestMode = (NAS_LMM_LOG_USIM_TEST_MODE_INFO_STRU*)(VOS_VOID *)pstMsg;

    if (NAS_EMM_NULL_PTR == pstUsimTestMode)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveUsimTestMode:Point is null!");
        return VOS_FALSE;
    }
    /*回放信息的导出次数小于保存的最大值*/
    if (g_ulUsimTestModeCount < NAS_REPLAY_MAX_USIM_TEST_MODE)
    {
        /*保存回放消息中内容*/
        g_astUsimTestMode[g_ulUsimTestModeCount] = pstUsimTestMode->ulRslt;
        g_ulUsimTestModeCount ++;
    }
    else
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_REPLAY_SaveUsimTestMode:Import Msg count is large Max count!");
    }

    return VOS_TRUE;
}



VOS_UINT32  Stub_USIMM_IsTestCard
(
    VOS_VOID
)
{
    VOS_UINT32         ulRslt;
    if (g_ulUsimTestModeIndex < g_ulUsimTestModeCount)
    {
        NAS_LMM_PUBM_LOG_INFO("Stub_USIMM_IsTestCard: Import USIMM_IsTestCard info!");

        /*导入返回值*/
        ulRslt = g_astUsimTestMode[g_ulUsimTestModeIndex];

        g_ulUsimTestModeIndex ++;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("Stub_USIMM_IsTestCard: Import USIMM_IsTestCard info ERR!");

        ulRslt = USIMM_API_RESULT_BUTT;
    }
    return ulRslt;
}

#endif
#endif

#endif
VOS_VOID NAS_LMM_LogPubInfo
(
    MMC_LMM_INFO_TYPE_ENUM_UINT32    ulInfoType,
    const MMC_LMM_PUB_INFO_STRU      *pPubInfo,
    MMC_LMM_RESULT_ID_ENUM_UINT32    ulRslt
)
{
    NAS_LMM_LOG_PUB_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_PUB_INFO_IND;

    pstMsg = (NAS_LMM_LOG_PUB_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_PUB_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogPubUInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_PUB_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;
    pstMsg->ulInfoType                  = ulInfoType;

    NAS_LMM_MEM_CPY(                    &pstMsg->stPubInfo,
                                        pPubInfo,
                                        sizeof(MMC_LMM_PUB_INFO_STRU));

    pstMsg->ulRslt                      = ulRslt;
    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogGuInfo
(
    NAS_GUMM_INFO_TYPE_ENUM_UINT32       ulInfoType,
    const NAS_GUMM_INFO_STRU             *pstGuInfo,
    MMC_LMM_RESULT_ID_ENUM_UINT32        ulRslt
)
{
    NAS_LMM_LOG_GU_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_GU_INFO_IND;

    pstMsg = (NAS_LMM_LOG_GU_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_GU_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogGuInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_GU_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;
    pstMsg->ulInfoType                  = ulInfoType;
    NAS_LMM_MEM_CPY(                    &pstMsg->stGuInfo,
                                        pstGuInfo,
                                        sizeof(NAS_GUMM_INFO_STRU));

    pstMsg->ulRslt                      = ulRslt;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogTinInfo
(
    MMC_LMM_TIN_TYPE_ENUM_UINT32        ulTinType
)
{
    NAS_LMM_LOG_TIN_INFO_STRU           *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_TIN_INFO_IND;

    pstMsg = (NAS_LMM_LOG_TIN_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_TIN_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogTinInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_TIN_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;
    pstMsg->ulTinType                   = ulTinType;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogMsNetworkCapabilityInfo
(
    const NAS_MML_MS_NETWORK_CAPACILITY_STRU  *pstMsNetworkCapbility
)
{
    NAS_LMM_LOG_MS_NETWORK_CAPACILITY_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_MS_NETWORK_CAPACILITY_INFO_IND;

    pstMsg = (NAS_LMM_LOG_MS_NETWORK_CAPACILITY_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_MS_NETWORK_CAPACILITY_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogMsNetworkCapabilityInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_MS_NETWORK_CAPACILITY_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    NAS_LMM_MEM_CPY(                    &pstMsg->stMsNetworkCapbility,
                                        pstMsNetworkCapbility,
                                        sizeof(NAS_MML_MS_NETWORK_CAPACILITY_STRU));

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogUeIdTmsiInfo
(
    const VOS_UINT8                          *pucTmsi
)
{
    NAS_LMM_LOG_UEIDTMSI_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_UEIDTMSI_IND;

    pstMsg = (NAS_LMM_LOG_UEIDTMSI_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_UEIDTMSI_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogUeIdTmsiInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_UEIDTMSI_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    NAS_LMM_MEM_CPY(pstMsg->aucUeIdTmsi,pucTmsi,4);

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogCsServiceCfgInfo
(
    NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8   ulCsService
)
{
    NAS_LMM_LOG_CS_SERVICE_FLAG_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_CS_SERVICE_FLAG_INFO_IND;

    pstMsg = (NAS_LMM_LOG_CS_SERVICE_FLAG_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_CS_SERVICE_FLAG_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogCsServiceCfgInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_CS_SERVICE_FLAG_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ucMmlCsService              = ulCsService;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogAdditionUpdateRsltInfo
(
    NAS_LMM_ADDITIONAL_UPDATE_RSLT_ENUM_UINT32   enAddUpdateRslt
)
{
    NAS_LMM_LOG_ADD_UPDATE_RSLT_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_ADD_UPDATE_RSLT_INFO_IND;

    pstMsg = (NAS_LMM_LOG_ADD_UPDATE_RSLT_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_ADD_UPDATE_RSLT_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogAdditionUpdateRsltInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_ADD_UPDATE_RSLT_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->enAddUpdateRslt             = enAddUpdateRslt;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogPsBearerCtxInfo
(
    const NAS_MML_PS_BEARER_CONTEXT_STRU      *pstPsBearerCtx
)
{
    NAS_LMM_LOG_PS_BEAR_CTX_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_PS_BEAR_CTX_INFO_IND;

    pstMsg = (NAS_LMM_LOG_PS_BEAR_CTX_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_PS_BEAR_CTX_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogPsBearerCtxInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_PS_BEAR_CTX_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    NAS_LMM_MEM_CPY(                    pstMsg->astPsBearerCtx,
                                        pstPsBearerCtx,
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)*NAS_MML_MAX_PS_BEARER_NUM);

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogIsBearIsrActBeforeInfo
(
    VOS_UINT32      ulRslt
)
{
    NAS_LMM_LOG_PSBER_EXIST_BEF_ISRACT_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_PSBER_EXIST_BEF_ISRACT_INFO_IND;

    pstMsg = (NAS_LMM_LOG_PSBER_EXIST_BEF_ISRACT_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_PSBER_EXIST_BEF_ISRACT_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogIsBearIsrActBeforeInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_PSBER_EXIST_BEF_ISRACT_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ulExistFlag                 = ulRslt;
    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogIsBearIsrActAfterInfo
(
     VOS_UINT32                          ulExistFlag
)
{
    NAS_LMM_LOG_PSBER_EXIST_AFTER_ISRACT_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_BEAR_ISR_ACT_AFTER_INFO_IND;

    pstMsg = (NAS_LMM_LOG_PSBER_EXIST_AFTER_ISRACT_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_PSBER_EXIST_AFTER_ISRACT_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogIsBearIsrActAfterInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_PSBER_EXIST_AFTER_ISRACT_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ulExistFlag          = ulExistFlag;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogEmergencyNumListInfo
(
   const NAS_MML_EMERGENCY_NUM_LIST_STRU    *pstMmlEmerNumList
)
{
    NAS_LMM_LOG_EMERGENCY_NUMLIST_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_EMERGENCY_NUMLIST_INFO_IND;

    pstMsg = (NAS_LMM_LOG_EMERGENCY_NUMLIST_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_EMERGENCY_NUMLIST_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogEmergencyNumListInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_EMERGENCY_NUMLIST_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    NAS_LMM_MEM_CPY(                    &pstMsg->stMmlEmerNumList,
                                        pstMmlEmerNumList,
                                        sizeof(NAS_MML_EMERGENCY_NUM_LIST_STRU));

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogPsRegContainDrxInfo
(
    NAS_MML_PS_REG_CONTAIN_DRX_PARA_ENUM_UINT8    enPsRegisterContainDrx
)
{
    NAS_LMM_LOG_PSREG_CONTAINDRX_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_PSREG_CONTAINDRX_INFO_IND;

    pstMsg = (NAS_LMM_LOG_PSREG_CONTAINDRX_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_PSREG_CONTAINDRX_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogPsRegContainDrxInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_PSREG_CONTAINDRX_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->enPsRegisterContainDrx      = enPsRegisterContainDrx;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogUeEutranPsDrxLenInfo
(
    VOS_UINT8                           ucEutranDrxLen
)
{
    NAS_LMM_LOG_UE_EUTRAN_PS_DRX_LEN_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_EUTRAN_PS_DRX_LEN_INFO_IND;

    pstMsg = (NAS_LMM_LOG_UE_EUTRAN_PS_DRX_LEN_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_UE_EUTRAN_PS_DRX_LEN_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogUeEutranPsDrxLenInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_UE_EUTRAN_PS_DRX_LEN_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ucEutranDrxLen          = ucEutranDrxLen;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogNonDrxTimerInfo
(
    VOS_UINT8                           ucNonDrxTimer
)
{
    NAS_LMM_LOG_NON_DRX_TIMER_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_DRX_TIMER_INFO_IND;

    pstMsg = (NAS_LMM_LOG_NON_DRX_TIMER_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_NON_DRX_TIMER_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogNonDrxTimerInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_NON_DRX_TIMER_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ucNonDrxTimer          = ucNonDrxTimer;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogDrxSplitPgCycleCodeInfo
(
    VOS_UINT8                           ucSplitPgCycleCode
)
{
    NAS_LMM_LOG_DRX_SPLIT_PG_CYCLE_CODE_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_SPLIT_PG_CYCLE_CODE_INFO_IND;

    pstMsg = (NAS_LMM_LOG_DRX_SPLIT_PG_CYCLE_CODE_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_DRX_SPLIT_PG_CYCLE_CODE_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogDrxSplitPgCycleCodeInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_DRX_SPLIT_PG_CYCLE_CODE_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ucSplitPgCycleCode          = ucSplitPgCycleCode;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogDrxSplitOnCcch
(
    VOS_UINT8                           ucSplitCcch
)
{
    NAS_LMM_LOG_DRX_SPLIT_ON_CCCH_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_SPLIT_ON_CCCH_INFO_IND;

    pstMsg = (NAS_LMM_LOG_DRX_SPLIT_ON_CCCH_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_DRX_SPLIT_ON_CCCH_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogDrxSplitOnCcch: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_DRX_SPLIT_ON_CCCH_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ucSplitCcch     = ucSplitCcch;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogClassMark2Info
(
    const VOS_UINT8  *pClassMark2
)
{
    NAS_LMM_LOG_CLASSMARK2_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_CLASSMARK2_INFO_IND;

    pstMsg = (NAS_LMM_LOG_CLASSMARK2_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_CLASSMARK2_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogClassMark2Info: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_CLASSMARK2_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    NAS_LMM_MEM_CPY(                    pstMsg->aucMsClassMark2,
                                        pClassMark2,
                                        sizeof(NAS_MML_CLASSMARK2_LEN));

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogCsAttachAllowFlgInfo
(
    VOS_UINT8                           ucCsAttachAllow
)
{
    NAS_LMM_LOG_CS_ATTACH_ALLOW_FLG_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_CS_ATTACH_ALLOW_FLG_INFO_IND;

    pstMsg = (NAS_LMM_LOG_CS_ATTACH_ALLOW_FLG_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_CS_ATTACH_ALLOW_FLG_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogCsAttachAllowFlgInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_CS_ATTACH_ALLOW_FLG_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ucCsAttachAllow             = ucCsAttachAllow;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogPsAttachAllowFlgInfo
(
    VOS_UINT8                           ucPsAttachAllow
)
{
    NAS_LMM_LOG_PS_ATTACH_ALLOW_FLG_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_PS_ATTACH_ALLOW_FLG_INFO_IND;

    pstMsg = (NAS_LMM_LOG_PS_ATTACH_ALLOW_FLG_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_PS_ATTACH_ALLOW_FLG_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogPsAttachAllowFlgInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_PS_ATTACH_ALLOW_FLG_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ucPsAttachAllow             = ucPsAttachAllow;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}



VOS_VOID NAS_LMM_LogMsMode
(
    VOS_UINT8                          ucMsMode
)
{
    NAS_LMM_LOG_MS_MODE_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_MS_MODE_INFO_IND;

    pstMsg = (NAS_LMM_LOG_MS_MODE_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_MS_MODE_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogMsMode: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_MS_MODE_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ucMsMode          = ucMsMode;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogPlmnIsForbid
(
    const MMC_LMM_PLMN_ID_STRU                *pstPlmn,
    VOS_UINT32                          ulRslt
)
{
    NAS_LMM_LOG_PLMNID_IS_FOBID_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_PLMN_IS_FORBID_INFO_IND;

    pstMsg = (NAS_LMM_LOG_PLMNID_IS_FOBID_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_PLMNID_IS_FOBID_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogPlmnIsForbid: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_PLMNID_IS_FOBID_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;
    NAS_LMM_MEM_CPY(&pstMsg->stPlmn, pstPlmn, sizeof(MMC_LMM_PLMN_ID_STRU));
    pstMsg->ulRslt          = ulRslt;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogPlmnexactlyCmpFlag
(
    VOS_UINT8                          ucRslt
)
{
    NAS_LMM_LOG_PLMN_EXACTLY_COMPARE_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_PLMN_EXAXTLY_CMP_INFO_IND;

    pstMsg = (NAS_LMM_LOG_PLMN_EXACTLY_COMPARE_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_PLMN_EXACTLY_COMPARE_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogPlmnexactlyCmpFlag: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_PLMN_EXACTLY_COMPARE_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ucRslt          = ucRslt;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogPsRegStatus
(
    VOS_UINT8                          ucPsRegStatus
)
{
    NAS_LMM_LOG_PS_REG_STATUS_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_PS_REG_STATUS_INFO_IND;

    pstMsg = (NAS_LMM_LOG_PS_REG_STATUS_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_PS_REG_STATUS_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogPsRegStautus: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_PS_REG_STATUS_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ucPsRegStatus   = ucPsRegStatus;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogSimType
(
    VOS_UINT8                          ucSimType
)
{
    NAS_LMM_LOG_SIM_TYPE_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_SIM_TYPE_INFO_IND;

    pstMsg = (NAS_LMM_LOG_SIM_TYPE_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_SIM_TYPE_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogSimType: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_MMC;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_SIM_TYPE_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ucSimType       = ucSimType;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogBspKdfKeyMake
(
    const NAS_LMM_BSP_INPUT_PARA_STRU         *pstBspInputPara,
    const NAS_LMM_BSP_OUTPUT_PARA_STRU        *pstBspOutputPara,
    VOS_INT32                                   lRslt
)
{
    NAS_LMM_LOG_KDF_KEY_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_BSP_KDF_KEY_MAKE_INFO_IND;

    pstMsg = (NAS_LMM_LOG_KDF_KEY_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_KDF_KEY_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogBspKdfKeyMake: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = PS_PID_MM;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_KDF_KEY_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    NAS_LMM_MEM_CPY(                    &pstMsg->stBspInputPara,
                                        pstBspInputPara,
                                        sizeof(NAS_LMM_BSP_INPUT_PARA_STRU));

    NAS_LMM_MEM_CPY(                    &pstMsg->stBspOutputPara,
                                        pstBspOutputPara,
                                        sizeof(NAS_LMM_BSP_OUTPUT_PARA_STRU));

    pstMsg->lRslt                      = lRslt;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogSdfPara
(
    const VOS_UINT32                         *pulSdfNum,
    const APP_ESM_SDF_PARA_STRU        *pstSdfPara,
    VOS_UINT32                          ulRslt
)
{
    NAS_OM_LOG_SDF_PARA_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_SDF_PARA_INFO_IND;

    pstMsg = (NAS_OM_LOG_SDF_PARA_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_OM_LOG_SDF_PARA_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogUsimServiceInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = PS_PID_ESM;
    pstMsg->ulReceiverPid   = PS_PID_ESM;
    pstMsg->ulLength        = sizeof(NAS_OM_LOG_SDF_PARA_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ulSdfNum                    = *pulSdfNum;

    NAS_LMM_MEM_CPY(                    &pstMsg->stSdfPara,
                                        pstSdfPara,
                                        sizeof(APP_ESM_SDF_PARA_STRU));

    pstMsg->ulRslt                      = ulRslt;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogPdpManageInfo
(
    const APP_ESM_PDP_MANAGE_INFO_STRU   *pstPdpManageInfo,
    VOS_UINT32  ulRslt
)
{
    NAS_OM_LOG_PDP_MANAGER_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_PDP_MANAGE_INFO_IND;

    pstMsg = (NAS_OM_LOG_PDP_MANAGER_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_OM_LOG_PDP_MANAGER_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogPdpManageInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid             = PS_PID_ESM;
    pstMsg->ulReceiverPid           = PS_PID_ESM;
    pstMsg->ulLength                = sizeof(NAS_OM_LOG_PDP_MANAGER_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName               = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ulRslt                  = ulRslt;
    NAS_LMM_MEM_CPY(                &pstMsg->stPdpManageInfo,
                                    pstPdpManageInfo,
                                    sizeof(APP_ESM_PDP_MANAGE_INFO_STRU));

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogCardIMSIInfo
(
    const VOS_UINT8 *pucImsi,
    VOS_UINT32      ulRslt
)
{
    NAS_LMM_LOG_CARD_IMSI_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_CARD_IMSI_INFO_IND;

    pstMsg = (NAS_LMM_LOG_CARD_IMSI_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_CARD_IMSI_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogCardIMSIInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_USIM;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_CARD_IMSI_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    NAS_LMM_MEM_CPY(pstMsg->aucImsi, pucImsi, NAS_LMM_NVIM_IMSI_FILE_LEN);

    pstMsg->ulRslt = ulRslt;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


VOS_VOID NAS_LMM_LogUsimServiceInfo
(
   VOS_UINT32     enServiceType,
   VOS_UINT32     ulRslt
)
{
    NAS_LMM_LOG_USIMM_SERVICE_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_USIMM_SERVICE_INFO_IND;

    pstMsg = (NAS_LMM_LOG_USIMM_SERVICE_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_USIMM_SERVICE_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogUsimServiceInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_USIM;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_USIMM_SERVICE_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->enService                   = enServiceType;
    pstMsg->ulRslt                      = ulRslt;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}



VOS_VOID NAS_LMM_LogUsimTestModeInfo
(
    VOS_UINT32                          ulRslt
)
{
    NAS_LMM_LOG_USIM_TEST_MODE_INFO_STRU            *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgName = NAS_OM_LOG_USIMM_TEST_MODE_INFO_IND;

    pstMsg = (NAS_LMM_LOG_USIM_TEST_MODE_INFO_STRU*)NAS_LMM_MEM_ALLOC(sizeof(NAS_LMM_LOG_USIM_TEST_MODE_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_LogUsimTestModeInfo: mem alloc fail!.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_USIM;
    pstMsg->ulReceiverPid   = PS_PID_MM;
    pstMsg->ulLength        = sizeof(NAS_LMM_LOG_USIM_TEST_MODE_INFO_STRU) - NAS_EMM_LEN_VOS_MSG_HEADER;

    pstMsg->ulMsgName       = ulMsgName + PS_MSG_ID_LNAS_TO_OM_BASE;

    pstMsg->ulRslt          = ulRslt;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_LMM_MEM_FREE(pstMsg);

    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

















