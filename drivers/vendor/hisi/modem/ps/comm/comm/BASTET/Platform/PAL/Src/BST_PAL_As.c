

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_Platform.h"
#include "BST_PAL_As.h"
#include "BST_OS_Memory.h"
#include "SysNvId.h"
#include "nvim_comminterface.h"
#include "NVIM_Interface.h"
#include "BastetRabmInterface.h"
#include "BastetRrcInterface.h"
#include "BST_SYS_MsgProc.h"


/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_PAL_AS_C
/*lint +e767*/
/******************************************************************************
   2 宏定义
******************************************************************************/

/******************************************************************************
   3 私有定义
******************************************************************************/

/*****************************************************************************
  4 函数声明
*****************************************************************************/
BST_MODEM_SIM_MODE_ENUM_UINT32  BST_PAL_AsApiGetCurSimMode( BST_VOID );
BST_BOOL BST_PAL_AsApiGetCsServiceState( BST_MODEM_ID_ENUM_UINT32 enModemId );
BST_BOOL BST_PAL_AsApiGetPsServiceState( BST_MODEM_ID_ENUM_UINT32 enModemId );
BST_BOOL BST_PAL_AsApiGetCsConnectState( BST_MODEM_ID_ENUM_UINT32 enModemId );


/******************************************************************************
   5 全局变量定义
******************************************************************************/
BST_DRV_RCV_AS_EVENT_T                  g_BstPalAsEventRcvCb    = BST_NULL_PTR;

BST_PAL_AS_INFO_STRU                    g_stBstPalAsInfo;

/******************************************************************************
   6 函数实现
******************************************************************************/



BST_ERR_ENUM_UINT8  BST_PAL_AsApiReleaseRrcConn( BST_VOID )
{
    BST_MODEM_ID_ENUM_UINT32           enPsModemId;
    BST_AS_RAT_MODE_UINT32             enPsRatMode;

    enPsModemId                        = BST_PAL_AsApiGetPsModemId();
    if ( enPsModemId >= BST_MODEM_MAX )
    {
        return BST_ERR_ILLEGAL_PARAM;
    }

    enPsRatMode                        = BST_PAL_AsApiGetModemRat( enPsModemId );

    if ( !BST_AS_IsRatSupport ( enPsRatMode ) )
    {
        return BST_ERR_ILLEGAL_PARAM;
    }

    switch ( enPsRatMode )
    {
        case BST_AS_RAT_WCDMA:
        case BST_AS_RAT_TDSCDMA:
            BST_DBG_LOG( "BST_DRV_CMD_RELEASE_RRC 3G" );
            BST_MSG_ProcRrrcRealseSend();
            break;

        case BST_AS_RAT_LTE:
            if(BST_FALSE == BST_PAL_AsApiGetCsServiceState( enPsModemId ))
            {
                BST_DBG_LOG( "BST_DRV_CMD_RELEASE_RRC 4G." );
                BST_MSG_ProcLRrcRealseSend();
            }
            break;

        default:
            break;
    }
    return BST_NO_ERROR_MSG;
}

BST_MODEM_SIM_MODE_ENUM_UINT32 BST_PAL_AsApiGetCurSimMode( BST_VOID )
{
#if  ( FEATURE_MULTI_MODEM != FEATURE_ON )
    return ( BST_SIGLE_SIM_MODE );
#else
    BST_AS_RAT_MODE_UINT32              enRatMode0;
    BST_AS_RAT_MODE_UINT32              enRatMode1;


    enRatMode0 = BST_PAL_AsApiGetModemRat(BST_MODEM_ID_1);
    enRatMode1 = BST_PAL_AsApiGetModemRat(BST_MODEM_ID_2);

    if((BST_AS_RAT_NONE == enRatMode0)
       &&(BST_AS_RAT_NONE == enRatMode1))
    {
        return ( BST_INVALID_SIM_MODE );
    }

    if((BST_AS_RAT_NONE != enRatMode0)
       &&(BST_AS_RAT_NONE != enRatMode1))
    {
        if ( BST_TRUE == g_stBstPalAsInfo.usDsdsRfShareSupportFlg  )
        {
            return ( BST_DUAL_DSDS_SIM_MODE );
        }
        else
        {
            return ( BST_DUAL_DSDA_SIM_MODE );
        }
    }


    return BST_SIGLE_SIM_MODE;

#endif
}


BST_AS_RAT_MODE_UINT32 BST_PAL_AsApiGetModemRat( BST_MODEM_ID_ENUM_UINT32 enModemId )
{
    if(enModemId >= BST_MODEM_MAX)
    {
        return BST_AS_RAT_NONE;
    }

    if((BST_FALSE == BST_PAL_AsApiGetCsServiceState(enModemId))
       &&(BST_FALSE == BST_PAL_AsApiGetPsServiceState(enModemId)))
    {
        return BST_AS_RAT_NONE;
    }

    return g_stBstPalAsInfo.stModemInfo[enModemId].enRatMode;
}


BST_MODEM_ID_ENUM_UINT32 BST_PAL_AsApiGetPsModemId( BST_VOID )
{
    return g_stBstPalAsInfo.enPsModemId;
}


BST_BOOL BST_PAL_AsApiGetCsServiceState( BST_MODEM_ID_ENUM_UINT32 enModemId )
{
    if(enModemId >= BST_MODEM_MAX)
    {
        return BST_FALSE;
    }

    return g_stBstPalAsInfo.stModemInfo[enModemId].bCsInService;
}


BST_BOOL BST_PAL_AsApiGetPsServiceState( BST_MODEM_ID_ENUM_UINT32 enModemId )
{
    if(enModemId >= BST_MODEM_MAX)
    {
        return BST_FALSE;
    }

    return g_stBstPalAsInfo.stModemInfo[enModemId].bPsInService;
}


BST_BOOL BST_PAL_AsApiGetCsConnectState( BST_MODEM_ID_ENUM_UINT32 enModemId )
{
    if(enModemId >= BST_MODEM_MAX)
    {
        return BST_FALSE;
    }

    return g_stBstPalAsInfo.stModemInfo[enModemId].bCsConnected;
}


#if (FEATURE_ON == FEATURE_DSDS)

BST_VOID BST_PAL_AsApiReadRfCfgNvim(BST_VOID)
{
    NV_MODEM_RF_SHARE_CFG_STRU          stNvimRfShareCfgInfo;

    PS_MEM_SET(&stNvimRfShareCfgInfo, 0x00, sizeof(NV_MODEM_RF_SHARE_CFG_STRU));

    /* 读NV项en_NV_Item_USER_CFG_OPLMN_EXTEND_LIST，失败，直接返回 */
    if (NV_OK != NV_Read(en_NV_MODEM_RF_SHARE_CFG,
                         &stNvimRfShareCfgInfo, sizeof(NV_MODEM_RF_SHARE_CFG_STRU)))
    {
        BST_RLS_LOG("BST_PAL_AsApiReadRfCfgNvim():WARNING: read en_NV_MODEM_RF_SHARE_CFG Error");

        return;
    }

    if (BST_TRUE != stNvimRfShareCfgInfo.usSupportFlag)
    {
        /* NV内容异常保护 */
        g_stBstPalAsInfo.usDsdsRfShareSupportFlg = BST_FALSE;
    }
    else
    {
        g_stBstPalAsInfo.usDsdsRfShareSupportFlg  = stNvimRfShareCfgInfo.usSupportFlag;
    }

    BST_RLS_LOG1("nv support dsds :%d",g_stBstPalAsInfo.usDsdsRfShareSupportFlg );

    return;
}


#endif


BST_VOID BST_PAL_AsMsgSetRatModeInfo(
    BST_MODEM_ID_ENUM_UINT32 enModemId,
    BST_AS_RAT_MODE_UINT32   enRatMode
)
{
    BST_UINT32                          ulAppData;
    BST_UINT32                          ulUsedLen;
    BST_AS_RAT_MODE_UINT32              enOldRatMode;
    BST_AS_RAT_MODE_UINT32              enNewRatMode;
    BST_DRV_AS_CS_EVENT_STRU            stCSEvent;


    if(enModemId >= BST_MODEM_MAX)
    {
        BST_RLS_LOG1( "BST_PAL_AsMsgSetRatModeInfo, enModemId is illegal=%u", enModemId );
        return;
    }


    enOldRatMode = BST_PAL_AsApiGetModemRat(enModemId);
    g_stBstPalAsInfo.stModemInfo[enModemId].enRatMode  = enRatMode;

    /*开机第一次需要尽快上报一次制式，后面根据PS modem再进行修正*/
    if( BST_FALSE == g_stBstPalAsInfo.bRatReported )
    {
        if( BST_AS_RAT_NONE != enRatMode )
        {
            ulAppData   = enRatMode;
            BST_RLS_LOG1( "Rcv As Event: BST_AS_EVT_L3_RAT_STATE, para=%u", ulAppData );
            ulUsedLen                   = BST_OS_SIZEOF( BST_AS_RAT_MODE_UINT32 );
            BST_PalAsEventRcvCb( BST_AS_EVT_L3_RAT_STATE, ulUsedLen, &ulAppData );
            g_stBstPalAsInfo.bRatReported = BST_TRUE;
            return;
        }
    }

    /*仅PS业务所在的modem的接入制式才需要上报*/
    if(enModemId == BST_PAL_AsApiGetPsModemId())
    {
        enNewRatMode = BST_PAL_AsApiGetModemRat(enModemId);
        if((enNewRatMode != enOldRatMode) && (BST_AS_RAT_NONE != enNewRatMode))
        {
            ulAppData   = enNewRatMode;
            BST_RLS_LOG1( "Rcv As Event: BST_AS_EVT_L3_RAT_STATE, para=%u", ulAppData );
            ulUsedLen                   = BST_OS_SIZEOF( BST_AS_RAT_MODE_UINT32 );
            BST_PalAsEventRcvCb( BST_AS_EVT_L3_RAT_STATE, ulUsedLen, &ulAppData );
        }

        /*if rat changed,need report the cs connect state of the modem to upper layer*/
        stCSEvent.ulSimMode                     = BST_PAL_AsApiGetCurSimMode();
        stCSEvent.bCSConnected[BST_MODEM_ID_1]  = BST_PAL_AsApiGetCsConnectState(BST_MODEM_ID_1);
        stCSEvent.bCSConnected[BST_MODEM_ID_2]  = BST_PAL_AsApiGetCsConnectState(BST_MODEM_ID_2);

        BST_DBG_LOG3 ( "BST_PAL_AsMsgModemIdChanged  cs1 exist:%u,cs2 exist:%u, ulSimMode:%u ",
              stCSEvent.bCSConnected[BST_MODEM_ID_1],
              stCSEvent.bCSConnected[BST_MODEM_ID_2],
              stCSEvent.ulSimMode);

        BST_PalAsEventRcvCb( BST_AS_EVT_L3_CS_EVENT_STATE,
                       ( BST_UINT32 )BST_OS_SIZEOF( BST_DRV_AS_CS_EVENT_STRU ),
                        &stCSEvent );
    }

    BST_RLS_LOG2( "BST_PAL_AsMsgSetRatModeInfo, enModemId :%d, Ratmode:%d", enModemId,enRatMode );

}

BST_VOID BST_PAL_AsMsgSetCsUsingInfo(
    BST_MODEM_ID_ENUM_UINT32 enModemId,
    BST_BOOL                 bCsConnected
)
{
    BST_DRV_AS_CS_EVENT_STRU            stCSEvent;

    if ( enModemId >= BST_MODEM_MAX )
    {
        return;
    }

    stCSEvent.ulSimMode             = BST_PAL_AsApiGetCurSimMode();
    BST_DBG_LOG3 ( "BST_PAL_AsMsgSetCsUsingInfo ModemId:%u, cs exist:%u, ulSimMode:%u ",
            enModemId,bCsConnected,stCSEvent.ulSimMode);

    g_stBstPalAsInfo.stModemInfo[enModemId].bCsConnected   = bCsConnected;

    stCSEvent.bCSConnected[BST_MODEM_ID_1]     = BST_PAL_AsApiGetCsConnectState(BST_MODEM_ID_1);
    stCSEvent.bCSConnected[BST_MODEM_ID_2]     = BST_PAL_AsApiGetCsConnectState(BST_MODEM_ID_2);

    BST_PalAsEventRcvCb( BST_AS_EVT_L3_CS_EVENT_STATE,
                       ( BST_UINT32 )BST_OS_SIZEOF( BST_DRV_AS_CS_EVENT_STRU ),
                        &stCSEvent );
    return;
}




BST_VOID BST_PAL_AsMsgSetServiceStatus(
    BST_MODEM_ID_ENUM_UINT32 enModemId,
    BST_BOOL                 bCsInservice,
    BST_BOOL                 bPsInservice
)
{
    BST_UINT32                          ulAppData;
    BST_UINT32                          ulUsedLen;
    BST_BOOL                            bOldPsSrvStatus;
    BST_BOOL                            bOldCsSrvStatus;
    BST_AS_RAT_MODE_UINT32              enOldRatMode;
    BST_AS_RAT_MODE_UINT32              enNewRatMode;
    BST_SRV_UTRAN_SRV_STATE_STRU        stSrvEvent;

    if(enModemId >= BST_MODEM_MAX)
    {
        BST_RLS_LOG1( "BST_PAL_AsMsgSetServiceStatus, enModemId is illegal=%u", enModemId );
        return;
    }

    enOldRatMode                   = BST_PAL_AsApiGetModemRat(enModemId);
    bOldPsSrvStatus                = BST_PAL_AsApiGetPsServiceState(enModemId);
    bOldCsSrvStatus                = BST_PAL_AsApiGetCsServiceState(enModemId);
    
    g_stBstPalAsInfo.stModemInfo[enModemId].bCsInService = bCsInservice;
    g_stBstPalAsInfo.stModemInfo[enModemId].bPsInService = bPsInservice;
    BST_RLS_LOG3( "BST_PAL_AsMsgSetServiceStatus, enModemId %u,CS service state:%d,Ps service state:%d",
            enModemId,bCsInservice, bPsInservice);

    if(enModemId != BST_PAL_AsApiGetPsModemId())
    {
        return;
    }

    /*if rat of ps modem changed ,need notify upper layer*/
    enNewRatMode  = BST_PAL_AsApiGetModemRat(enModemId);

    if((enNewRatMode != BST_AS_RAT_NONE) && (enNewRatMode != enOldRatMode ))
    {
        ulUsedLen                   = BST_OS_SIZEOF( BST_AS_RAT_MODE_UINT32 );
        ulAppData                   = enNewRatMode;

        BST_PalAsEventRcvCb( BST_AS_EVT_L3_RAT_STATE, ulUsedLen, &ulAppData );
        BST_RLS_LOG1( "BST_PAL_AsMsgSetServiceStatus, notify rat mode changed:%d", ulAppData );
    }

    /*if ps service state of ps modem changed ,need notify upper layer*/
    if( ( bOldPsSrvStatus != bPsInservice)
     || ( bOldCsSrvStatus != bCsInservice) )
    {
        stSrvEvent.bCsService       = bCsInservice;
        stSrvEvent.bPsService       = bPsInservice;
        BST_PalAsEventRcvCb( BST_AS_EVT_L3_SERVICE_STATE,
                            ( BST_UINT32 )BST_OS_SIZEOF( BST_SRV_UTRAN_SRV_STATE_STRU ),
                            &stSrvEvent );
    }
    return;


}



BST_VOID BST_PAL_AsMsgModemIdChanged(
    BST_MODEM_ID_ENUM_UINT32 enNewModemId )
{
    BST_AS_RAT_MODE_UINT32              enRatMode;
    BST_UINT32                          ulAppData;
    BST_UINT32                          ulUsedLen;
    BST_DRV_AS_CS_EVENT_STRU            stCSEvent;
    BST_SRV_UTRAN_SRV_STATE_STRU        stSrvEvent;

    if(enNewModemId >= BST_MODEM_MAX)
    {
        return;
    }
    
    g_stBstPalAsInfo.enPsModemId        = enNewModemId;

    /*if modem changed,need report the rat of new modem to upper layer*/
    enRatMode                           = BST_PAL_AsApiGetModemRat(enNewModemId);
    if(BST_AS_RAT_NONE != enRatMode)
    {
        ulUsedLen                       = BST_OS_SIZEOF( BST_AS_RAT_MODE_UINT32 );
        ulAppData                       = enRatMode;
        BST_PalAsEventRcvCb( BST_AS_EVT_L3_RAT_STATE, ulUsedLen, &ulAppData );
        BST_RLS_LOG1( "BST_PAL_AsMsgModemIdChanged, notify rat mode changed:%d", ulAppData );
    }

    /*if modem changed,need report the service state of new modem to upper layer*/
    stSrvEvent.bCsService               = BST_PAL_AsApiGetCsServiceState(enNewModemId);
    stSrvEvent.bPsService               = BST_PAL_AsApiGetPsServiceState(enNewModemId);
    BST_PalAsEventRcvCb( BST_AS_EVT_L3_SERVICE_STATE,
                        ( BST_UINT32 )BST_OS_SIZEOF( BST_SRV_UTRAN_SRV_STATE_STRU ),
                        &stSrvEvent );

    /*if modem changed,need report the cs connect state of new modem to upper layer*/
    stCSEvent.ulSimMode                     = BST_PAL_AsApiGetCurSimMode();
    stCSEvent.bCSConnected[BST_MODEM_ID_1]  = BST_PAL_AsApiGetCsConnectState(BST_MODEM_ID_1);
    stCSEvent.bCSConnected[BST_MODEM_ID_2]  = BST_PAL_AsApiGetCsConnectState(BST_MODEM_ID_2);

    BST_DBG_LOG3 ( "BST_PAL_AsMsgModemIdChanged  cs1 exist:%u,cs2 exist:%u, ulSimMode:%u ",
              stCSEvent.bCSConnected[BST_MODEM_ID_1],
              stCSEvent.bCSConnected[BST_MODEM_ID_2],
              stCSEvent.ulSimMode);
    
    BST_PalAsEventRcvCb( BST_AS_EVT_L3_CS_EVENT_STATE,
                       ( BST_UINT32 )BST_OS_SIZEOF( BST_DRV_AS_CS_EVENT_STRU ),
                        &stCSEvent );
}
BST_VOID BST_PAL_AsApiSndRptFlag (
    BST_MODEM_ID_ENUM_UINT32    enPsModemId,
    BST_UINT8                   ucReportFlg )
{

}


BST_ERR_ENUM_UINT8  BST_PAL_AsApiInitial( BST_DRV_RCV_AS_EVENT_T pfRcvCallBack )
{
    BST_MODEM_ID_ENUM_UINT32            enModemIndex;

    if( BST_NULL_PTR == pfRcvCallBack )
    {
        return BST_ERR_INVALID_PTR;
    }

    g_BstPalAsEventRcvCb                                         = pfRcvCallBack;
    g_stBstPalAsInfo.enPsModemId                                 = BST_MODEM_ID_1;
    g_stBstPalAsInfo.usDsdsRfShareSupportFlg                     = BST_FALSE;
    g_stBstPalAsInfo.bRatReported                                = BST_FALSE;

    for( enModemIndex = BST_MODEM_ID_1; enModemIndex < BST_MODEM_MAX; enModemIndex++)
    {
        g_stBstPalAsInfo.stModemInfo[enModemIndex].enRatMode     = BST_AS_RAT_NONE;
        g_stBstPalAsInfo.stModemInfo[enModemIndex].bCsInService  = BST_FALSE;
        g_stBstPalAsInfo.stModemInfo[enModemIndex].bPsInService  = BST_FALSE;
        g_stBstPalAsInfo.stModemInfo[enModemIndex].bCsConnected  = BST_FALSE;
    }

#if (FEATURE_ON == FEATURE_DSDS)
    BST_PAL_AsApiReadRfCfgNvim();
#endif
    return BST_NO_ERROR_MSG;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

