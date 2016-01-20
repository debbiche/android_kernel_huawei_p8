

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_Platform.h"
#include "BST_PAL_As.h"
#include "BST_DRV_As.h"
#include "BST_OS_Memory.h"
#include "BST_PAL_Net.h"
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_DRV_AS_C
/*lint +e767*/
/******************************************************************************
   2 宏定义
******************************************************************************/

#define BST_DRV_AsSetRrcExist( bState ) ( g_BstRrcExistFlag = bState )
/*****************************************************************************
  3 函数声明
*****************************************************************************/
BST_UINT32          BST_SRV_RcvAsEvent(
    BST_AS_EVT_ENUM_UINT32  enAsEvent,
    BST_UINT32              ulLength,
    BST_VOID               *pvData );

BST_ERR_ENUM_UINT8  BST_DRV_AsInit( BST_VOID *pvArg );

BST_VOID            BST_DRV_AsEventCallBack(
    BST_AS_EVT_ENUM_UINT32  enAsEvent,
    BST_UINT32              ulLength,
    BST_VOID               *pvData );

BST_ERR_ENUM_UINT8  BST_DRV_AsIoCtrl(
    BST_UINT32              ulCmd,
    BST_VOID               *pvData );

BST_VOID    BST_DRV_AsCsEvtProc( BST_DRV_AS_CS_EVENT_STRU *pstEvent );
BST_VOID    BST_DRV_AsPsSrvProc( BST_BOOL bInservice );
BST_VOID    BST_DRV_BlockDataService(BST_UINT8 bitFlag);
BST_VOID    BST_DRV_UnBlockDataService(BST_UINT8 bitFlag);


/******************************************************************************
   4 私有定义
******************************************************************************/
BST_UINT32      g_ulBstDrvRrcExistFlg   = BST_FALSE;
BST_BOOL        g_BstReportFlag         = BST_TRUE;
BST_UINT8       g_ucDsBlockFlg          = BST_AS_BLOCKED_BY_NO_SERVICE;
BST_UINT32      g_BstDrvLayerRatMode    = BST_AS_RAT_NONE;
/******************************************************************************
   5 全局变量定义
******************************************************************************/
BST_DRV_STRU    g_BstDrvAsHandle =
{
    BST_DRV_AsInit,
    BST_NULL_PTR,
    BST_NULL_PTR,
    BST_DRV_AsIoCtrl,
    BST_FALSE
};

/******************************************************************************
   6 函数实现
******************************************************************************/


BST_ERR_ENUM_UINT8 BST_DRV_AsInit( BST_VOID *pvArg )
{
    BST_ERR_ENUM_UINT8                  ucRtnVal;
    if( BST_DRV_IsDeviceInited(BST_DRV_AsGetDvcHandle()) )
    {
        return BST_NO_ERROR_MSG;
    }
    ucRtnVal                            = BST_PAL_AsApiInitial( BST_DRV_AsEventCallBack );
    if( BST_NO_ERROR_MSG == ucRtnVal )
    {
        g_BstDrvAsHandle.ulIsInited     = BST_TRUE;
    }
    g_ucDsBlockFlg                      = BST_AS_BLOCKED_BY_NO_SERVICE;
    g_ulBstDrvRrcExistFlg               = BST_FALSE;
    BST_DRV_AsSetReportFlag( BST_TRUE );
    return ucRtnVal;
}


BST_VOID BST_DRV_AsEventCallBack(
    BST_AS_EVT_ENUM_UINT32          enAsEvent,
    BST_UINT32                      ulLength,
    BST_VOID                       *pvData )
{
    BST_UINT32                      ulRrcExist;
    BST_UINT32                      ulRrcState;
    switch( enAsEvent )
    {
        case BST_AS_EVT_L3_RRC_STATE:
            ulRrcExist              = *( (BST_UINT32 *)pvData );
            if( ulRrcExist == g_ulBstDrvRrcExistFlg )
            {
                return;
            }
            if ( BST_TRUE == ulRrcExist )
            {
                ulRrcState          = BST_AS_RRC_CONNECTED;
            }
            else if ( BST_FALSE == ulRrcExist )
            {
                ulRrcState          = BST_AS_RRC_RELEASED;
            }
            else
            {
                ulRrcState          = BST_AS_RRC_STATE_INVALID;
            }
            g_ulBstDrvRrcExistFlg   = ulRrcExist;
           *((BST_UINT32 *)pvData)  = ulRrcState;
            break;

        case BST_AS_EVT_L3_SERVICE_STATE:
            BST_DRV_AsPsSrvProc(
                ((BST_SRV_UTRAN_SRV_STATE_STRU *)pvData)->bPsService);
            break;

        case BST_AS_EVT_L3_RAT_STATE:
            g_BstDrvLayerRatMode    = *( (BST_UINT32 *)pvData );
            break;

        case BST_AS_EVT_L3_CS_EVENT_STATE:
            BST_DRV_AsCsEvtProc( (BST_DRV_AS_CS_EVENT_STRU *)pvData );
            break;

        default:
            break;
    }
    BST_SRV_RcvAsEvent( enAsEvent, ulLength, pvData );
}


BST_ERR_ENUM_UINT8 BST_DRV_AsIoCtrl( BST_UINT32 ulCmd, BST_VOID *pvData )
{
    BST_ERR_ENUM_UINT8                  ucRtnVal = BST_NO_ERROR_MSG;

    switch( ulCmd )
    {
        case BST_DRV_CMD_GET_AS_STATE:
            BST_ASSERT_NULL_RTN( pvData, BST_ERR_INVALID_PTR );
           *( (BST_UINT8 *)pvData ) = g_ucDsBlockFlg;
            ucRtnVal                = BST_NO_ERROR_MSG;
            break;

        case BST_DRV_CMD_SET_REPORT_FLAG:
            BST_ASSERT_NULL_RTN( pvData, BST_ERR_INVALID_PTR );
            BST_DRV_AsSetReportFlag( *( (BST_UINT8*)pvData ) );
            ucRtnVal                = BST_NO_ERROR_MSG;
            break;

        case BST_DRV_CMD_GET_REPORT_FLAG:
            BST_ASSERT_NULL_RTN( pvData, BST_ERR_INVALID_PTR );
           *( (BST_BOOL *)pvData )  = g_BstReportFlag;
            ucRtnVal                = BST_NO_ERROR_MSG;
            break;

        case BST_DRV_CMD_GET_RAT_MODE:
            BST_ASSERT_NULL_RTN( pvData, BST_ERR_INVALID_PTR );
           *( (BST_UINT32*)pvData ) = g_BstDrvLayerRatMode;
            ucRtnVal                = BST_NO_ERROR_MSG;
            break;

        case BST_DRV_CMD_RELEASE_RRC:
            if ( !BST_DRV_AsIsRrcExisted() )
            {
                return BST_NO_ERROR_MSG;
            }
            ucRtnVal                = BST_PAL_AsApiReleaseRrcConn();
            break;

        case BST_DRV_CMD_SET_TRX_ENABLE:
            BST_PAL_NetApiSetTrxMsgEnable( *( (BST_UINT32 *)pvData ) );
            break;

        default:
            break;
    }
    return ucRtnVal;
}


BST_VOID BST_DRV_AsCsEvtProc( BST_DRV_AS_CS_EVENT_STRU *pstEvent )
{
    BST_MODEM_SIM_MODE_ENUM_UINT32      ulSimMode;
    BST_MODEM_ID_ENUM_UINT32            ulPsModemId;
    BST_AS_RAT_MODE_UINT32              ulPsRatMode;
    BST_MODEM_ID_ENUM_UINT32            ulCsModemId;

    BST_ASSERT_NULL( pstEvent );
    ulSimMode                           = pstEvent->ulSimMode;
    ulPsModemId                         = BST_PAL_AsApiGetPsModemId();

    if( (BST_FALSE == pstEvent->bCSConnected[BST_MODEM_ID_1])
        &&(BST_FALSE == pstEvent->bCSConnected[BST_MODEM_ID_2]))
    {
        BST_DRV_UnBlockDataService(BST_AS_BLOCKED_BY_CS_CONFLICT);
        return;
    }
    /*if both cs and ps have cs event, sim mode must be dsda mode,so only consider cs call for ps modem*/
    else if((BST_TRUE == pstEvent->bCSConnected[BST_MODEM_ID_1])
        &&(BST_TRUE == pstEvent->bCSConnected[BST_MODEM_ID_2]))
    {
        ulCsModemId = ulPsModemId;
    }
    else if(BST_TRUE == pstEvent->bCSConnected[BST_MODEM_ID_1])
    {
        ulCsModemId = BST_MODEM_ID_1; 
    }
    else
    {   
        ulCsModemId = BST_MODEM_ID_2; 
    }


    BST_DBG_LOG2 ( "BST_DRV_AsCsEvtProc ulSimMode:%u, ulPsModemId:%u ",\
                   ulSimMode,\
                   ulPsModemId );
    BST_ASSERT_NORM( BST_INVALID_SIM_MODE == ulSimMode );
    BST_ASSERT_NORM( ulPsModemId >= BST_MODEM_MAX );

    if( BST_SIGLE_SIM_MODE != ulSimMode )
    {
        if ( ulCsModemId != ulPsModemId )
        {
            if ( BST_DUAL_DSDS_SIM_MODE == ulSimMode )
            {
                BST_DRV_BlockDataService(BST_AS_BLOCKED_BY_CS_CONFLICT);
            }
            else
            {
                BST_DRV_UnBlockDataService(BST_AS_BLOCKED_BY_CS_CONFLICT);
            }
            return;
        }
    }

    ulPsRatMode                         = BST_PAL_AsApiGetModemRat( ulPsModemId );

    BST_DBG_LOG1 ( "BST_DRV_AsCsEvtProc ulPsRatMode:%u ",\
                   ulPsRatMode );

    if ( ( BST_AS_RAT_CDMA == ulPsRatMode )
       ||( BST_AS_RAT_GPRS == ulPsRatMode ) )
    {
        BST_DRV_BlockDataService(BST_AS_BLOCKED_BY_CS_CONFLICT);
    }
    else
    {
        BST_DRV_UnBlockDataService(BST_AS_BLOCKED_BY_CS_CONFLICT);
    }
}

BST_VOID BST_DRV_AsSetReportFlag(BST_UINT8 ucReportFlg)
{
    BST_MODEM_ID_ENUM_UINT32            enPsModemId;

    enPsModemId                         = BST_PAL_AsApiGetPsModemId();

    if(g_BstReportFlag != ucReportFlg)
    {
        g_BstReportFlag = ucReportFlg;
        BST_RLS_LOG1( "Set Modem Report AP Flag: %d", g_BstReportFlag );
        BST_PAL_AsApiSndRptFlag(enPsModemId,ucReportFlg);
    }
}


BST_VOID BST_DRV_AsPsSrvProc( BST_BOOL bInservice )
{
    if( BST_FALSE == bInservice )
    {
        BST_DRV_BlockDataService(BST_AS_BLOCKED_BY_NO_SERVICE); 
    }
    else
    {    
        BST_DRV_UnBlockDataService(BST_AS_BLOCKED_BY_NO_SERVICE); 
    }
    
}



BST_VOID BST_DRV_BlockDataService(BST_UINT8 bitFlag)
{
    BST_UINT8                           ucOldBlockFlag;
    BST_UINT32                          ulAppData;

    ucOldBlockFlag                      = g_ucDsBlockFlg;
    g_ucDsBlockFlg                     |= bitFlag;
    BST_DBG_LOG2("BST_DRV_BlockDataService,old:, new:", ucOldBlockFlag, g_ucDsBlockFlg);
    if(ucOldBlockFlag != g_ucDsBlockFlg)
    {
        ulAppData                       = g_ucDsBlockFlg;
        BST_SRV_RcvAsEvent( BST_AS_EVT_L3_BLOCK_INFO, sizeof(ulAppData), &ulAppData );
    }
}



BST_VOID BST_DRV_UnBlockDataService(BST_UINT8 bitFlag)
{
    BST_UINT8                           ucOldBlockFlag;
    BST_UINT32                          ulAppData;

    ucOldBlockFlag                      = g_ucDsBlockFlg;

    g_ucDsBlockFlg &= (~bitFlag);
    BST_DBG_LOG2("BST_DRV_UnBlockDataService,old:,new:", ucOldBlockFlag, g_ucDsBlockFlg);
    if(ucOldBlockFlag != g_ucDsBlockFlg)
    {
        ulAppData                       = g_ucDsBlockFlg;
        BST_SRV_RcvAsEvent( BST_AS_EVT_L3_BLOCK_INFO, sizeof(ulAppData), &ulAppData );
    }

}

