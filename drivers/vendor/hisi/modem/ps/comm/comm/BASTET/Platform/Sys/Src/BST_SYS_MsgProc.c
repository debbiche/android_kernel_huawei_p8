

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_Platform.h"
#include "BST_SYS_MsgProc.h"
#include "BST_DRV_As.h"
#include "BST_OS_Memory.h"
#include "MmaBastetInterface.h"
#include "BastetRabmInterface.h"
#include "BastetRrcInterface.h"
#include "BST_PAL_As.h"
#include "BastetWrrcinterface.h"
#include "BastetTrrcInterface.h"


/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_MSGPROC_C
/*lint +e767*/
/******************************************************************************
   2 宏定义
******************************************************************************/
#define BST_CONVERT_ENUM_TO_BOOL_TYPE(value)  ((value==PS_TRUE)? BST_TRUE : BST_FALSE)   
/******************************************************************************
   3 私有定义
******************************************************************************/

/*****************************************************************************
  4 函数声明
*****************************************************************************/

/******************************************************************************
 5 全局变量定义
******************************************************************************/

/******************************************************************************
  6 函数实现
******************************************************************************/


BST_VOID BST_MSG_ProcMmaRecv( MsgBlock *pstMsg, BST_MODEM_ID_ENUM_UINT32 enModemId )
{ 
    BST_UINT32                          ulAppData; 
    MSG_HEADER_STRU                    *pstMsgHeader   = VOS_NULL_PTR;

    if ( BST_NULL_PTR == pstMsg )
    {
        BST_DBG_LOG( "BST_MSG_ProcMmaRecv, msg is null ptr");
        return ;
    }
          
    pstMsgHeader = (MSG_HEADER_STRU *)pstMsg;

    switch( pstMsgHeader->ulMsgName )
    {
       case ID_MMA_BASTET_RAT_MODE_IND:
       {          
            MMA_BASTET_RAT_MODE_IND_STRU  *pstCurRatMsg = (MMA_BASTET_RAT_MODE_IND_STRU *)pstMsg;
            switch ( pstCurRatMsg->enRatMode )
            {
                case BST_RATMODE_WCDMA:
                    BST_RLS_LOG( "Current Rat is WCDMA" );
                    ulAppData           = BST_AS_RAT_WCDMA;
                    break;

                case BST_RATMODE_LTE:
                    BST_RLS_LOG( "Current Rat is LTE" );
                    ulAppData           = BST_AS_RAT_LTE;
                    break;

                case BST_RATMODE_GSM:
                    BST_RLS_LOG( "Current Rat is GSM(GPRS)" );
                    ulAppData           = BST_AS_RAT_GPRS;
                    break;
                case BST_RATMODE_TDS:
                    BST_RLS_LOG( "Current Rat is TDS" );
                    ulAppData           = BST_AS_RAT_TDSCDMA;
                    break;
                default:
                    ulAppData           = BST_AS_RAT_NONE;
                    break;
            }
            BST_PAL_AsMsgSetRatModeInfo(enModemId,ulAppData);
            break;
       }
       case ID_MMA_BASTET_CS_SERVICE_IND:
       {       
            MMA_BASTET_CS_SERVICE_IND_STRU  *pstCsSrvMsg = (MMA_BASTET_CS_SERVICE_IND_STRU *)pstMsg;
            BST_BOOL                         bCSConnected;
            bCSConnected   = BST_CONVERT_ENUM_TO_BOOL_TYPE(pstCsSrvMsg->bCSConnected);
            BST_PAL_AsMsgSetCsUsingInfo(enModemId, bCSConnected);
            break;
       }
       case ID_MMA_BASTET_SERVICE_STATUE_IND:
       {            
            MMA_BASTET_SERVICE_STATUS_IND_STRU *pstSrvMsg = (MMA_BASTET_SERVICE_STATUS_IND_STRU *)pstMsg;
            BST_BOOL                            bCsInservice;
            BST_BOOL                            bPsInservice;
            
            bCsInservice   = BST_CONVERT_ENUM_TO_BOOL_TYPE(pstSrvMsg->bCsInservice);
            bPsInservice   = BST_CONVERT_ENUM_TO_BOOL_TYPE(pstSrvMsg->bPsInservice);
            
            BST_PAL_AsMsgSetServiceStatus(enModemId, bCsInservice, bPsInservice);
            break;
       }
       default:
           BST_RLS_LOG1( "MMA unexpect event:%d", pstMsgHeader->ulMsgName );
           return;
           
    }


    return;
}



BST_VOID BST_MSG_ProcWrrcRecv( MsgBlock *pstMsg)
{ 
    MSG_HEADER_STRU                    *pstMsgHeader   = VOS_NULL_PTR;
    BST_UINT32                          ulRrcState;

    if ( BST_NULL_PTR == pstMsg )
    {
        BST_DBG_LOG( "BST_MSG_ProcWrrcRecv, msg is null ptr");
        return ;
    }
          
    pstMsgHeader = (MSG_HEADER_STRU *)pstMsg;

    switch( pstMsgHeader->ulMsgName )
    {
       case ID_WRRC_BASTET_RRC_STATUS_IND:
       {          
            WRRC_BASTET_STATUS_IND_STRU  *pstWrrcMsg = (WRRC_BASTET_STATUS_IND_STRU *)pstMsg;
            if(BST_MODEM_ID_1 != BST_PAL_AsApiGetPsModemId())
            {
                BST_RLS_LOG( "ID_WAS_BASTET_RRC_STATUS, current ps modem not for m0");
                break;
            }

            if(BST_AS_RAT_WCDMA != BST_PAL_AsApiGetModemRat(BST_MODEM_ID_1))
            {
                BST_RLS_LOG( "ID_WAS_BASTET_RRC_STATUS, current rat not wcdma");
                break;
            }
            BST_RLS_LOG1( "BST_MSG_ProcWrrcRecv, enRrcState:%d",pstWrrcMsg->enRrcState);
            if((BASTET_WRRC_STATE_CELL_DCH == pstWrrcMsg->enRrcState) 
              || (BASTET_WRRC_STATE_CELL_FACH == pstWrrcMsg->enRrcState))
            {
                ulRrcState              = BST_TRUE;
            }
            else
            {
                ulRrcState              = BST_FALSE;
            }
            BST_PalAsEventRcvCb( BST_AS_EVT_L3_RRC_STATE, 
                                 ( BST_UINT16 )BST_OS_SIZEOF(ulRrcState),
                                 &ulRrcState);
            break;
       }
       default:
           BST_RLS_LOG1( "Wrr unexpect event:%d", pstMsgHeader->ulMsgName );
           return;
           
    }
    return;
}




BST_VOID BST_MSG_ProcTrrcRecv( MsgBlock *pstMsg)
{ 
    MSG_HEADER_STRU                    *pstMsgHeader   = VOS_NULL_PTR;
    BST_UINT32                          ulRrcState;

    if ( BST_NULL_PTR == pstMsg )
    {
        BST_DBG_LOG( "BST_MSG_ProcTrrcRecv, msg is null ptr");
        return ;
    }
          
    pstMsgHeader = (MSG_HEADER_STRU *)pstMsg;

    switch( pstMsgHeader->ulMsgName )
    {
       case ID_TRRC_BASTET_RRC_STATUS_IND:
       {          
            TRRC_BASTET_STATUS_IND_STRU  *pstTrrcMsg = (TRRC_BASTET_STATUS_IND_STRU *)pstMsg;
            if(BST_MODEM_ID_1 != BST_PAL_AsApiGetPsModemId())
            {
                BST_RLS_LOG( "BST_MSG_ProcTrrcRecv, current ps modem not for m0");
                break;
            }

            if(BST_AS_RAT_TDSCDMA != BST_PAL_AsApiGetModemRat(BST_MODEM_ID_1))
            {
                BST_RLS_LOG( "BST_MSG_ProcTrrcRecv, current rat not tds");
                break;
            }
            BST_RLS_LOG1( "BST_MSG_ProcTrrcRecv, enRrcState:%d",pstTrrcMsg->enRrcState);
            if( (BASTET_TRRC_STATE_CELL_DCH == pstTrrcMsg->enRrcState)
              ||(BASTET_TRRC_STATE_CELL_FACH == pstTrrcMsg->enRrcState))
            {
                ulRrcState              = BST_TRUE;
            }
            else
            {
                ulRrcState              = BST_FALSE;
            }
            BST_PalAsEventRcvCb( BST_AS_EVT_L3_RRC_STATE, 
                                 ( BST_UINT16 )BST_OS_SIZEOF(ulRrcState),
                                 &ulRrcState);
            break;
       }
       default:
           BST_RLS_LOG1( "TRRC unexpect event:%d", pstMsgHeader->ulMsgName );
           return;
           
    }

    return;
}




BST_VOID BST_MSG_ProcLrrcRecv( MsgBlock *pstMsg)
{ 
    MSG_HEADER_STRU                    *pstMsgHeader   = VOS_NULL_PTR;
    BST_UINT32                          ulRrcState;

    if ( BST_NULL_PTR == pstMsg )
    {
        BST_DBG_LOG( "BST_MSG_ProcLrrcRecv, msg is null ptr");
        return ;
    }
          
    pstMsgHeader = (MSG_HEADER_STRU *)pstMsg;

    switch( pstMsgHeader->ulMsgName )
    {
       case ID_LRRC_BASTET_RRC_STATUS_IND:
       {          
            BASTET_RRC_LRRC_STATUS_IND_STRU  *pstRrcMsg 
                = (BASTET_RRC_LRRC_STATUS_IND_STRU *)pstMsg;
            if(BST_MODEM_ID_1 != BST_PAL_AsApiGetPsModemId())
            {
                BST_RLS_LOG( "BST_MSG_ProcLrrcRecv, current ps modem not for m0");
                break;
            }

            if(BST_AS_RAT_LTE != BST_PAL_AsApiGetModemRat(BST_MODEM_ID_1))
            {
                BST_RLS_LOG( "BST_MSG_ProcLrrcRecv, current rat not lte");
                break;
            }

            BST_RLS_LOG1( "BST_MSG_ProcLrrcRecv, enRrcState:%d",pstRrcMsg->enRrcState);

            if(BASTET_LRRC_STATE_CONNECTED == pstRrcMsg->enRrcState)
            {
                ulRrcState              = BST_TRUE;
            }
            else
            {
                ulRrcState              = BST_FALSE;
            }
            BST_PalAsEventRcvCb( BST_AS_EVT_L3_RRC_STATE, 
                                 ( BST_UINT16 )BST_OS_SIZEOF(ulRrcState),
                                 &ulRrcState);
            break;
       }
       default:
           BST_RLS_LOG1( "LRRC unexpect event:%d", pstMsgHeader->ulMsgName );
           return;
           
    }
    return;
}



BST_VOID BST_MSG_ProcRrrcRealseSend(BST_VOID)
{
    BASTETRABM_RRC_REL_REQ_STRU         *pstSndMsg;
    VOS_UINT32                           ulRslt;
    VOS_UINT32                           ulReceiverPid;
    BST_MODEM_ID_ENUM_UINT32             enPsModemId;

    
    enPsModemId                              = BST_PAL_AsApiGetPsModemId();
    if(BST_MODEM_ID_1 == enPsModemId)
    {
        ulReceiverPid = I0_WUEPS_PID_RABM;
    }
    else if(BST_MODEM_ID_2 == enPsModemId)
    {
        ulReceiverPid = I1_WUEPS_PID_RABM;
    }
    else
    {    
        BST_DBG_LOG1("BST_MSG_ProcRrrcRealseSend:ERROR: Wrong modemId:%d",enPsModemId);
        return;
    }
    /* 申请内存  */
    pstSndMsg = (BASTETRABM_RRC_REL_REQ_STRU *)PS_ALLOC_MSG(UEPS_PID_BASTET,
                                               sizeof(BASTETRABM_RRC_REL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstSndMsg )
    {
        /* 内存申请失败 */
        BST_DBG_LOG("BST_MSG_ProcRrrcRealseSend:ERROR: Memory Alloc Error for pstMsg");
        return;
    }

    PS_MEM_SET( (VOS_CHAR*)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0x00,
                (sizeof(BASTETRABM_RRC_REL_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写相关参数 */
    pstSndMsg->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstSndMsg->stMsgHeader.ulReceiverPid     = ulReceiverPid;
    pstSndMsg->stMsgHeader.ulLength          = sizeof(BASTETRABM_RRC_REL_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstSndMsg->stMsgHeader.ulMsgName         = ID_BASTET_RABM_SET_RELEASE_RRC_REQ;

    /* 调用VOS发送原语 */
    ulRslt = PS_SEND_MSG(UEPS_PID_BASTET, pstSndMsg);
    if ( VOS_OK != ulRslt )
    {
        BST_DBG_LOG("BST_MSG_ProcRrrcRealseSend:ERROR:PS_SEND_MSG ");
        return;
    }

    return;
}




BST_VOID BST_MSG_ProcLRrcRealseSend(BST_VOID)
{
    BASTET_RRC_LTE_TO_IDLE_REQ_STRU     *pstSndMsg;
    VOS_UINT32                           ulRslt;

    /* 申请内存  */
    pstSndMsg = (BASTET_RRC_LTE_TO_IDLE_REQ_STRU *)PS_ALLOC_MSG(UEPS_PID_BASTET,
                 sizeof(BASTET_RRC_LTE_TO_IDLE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstSndMsg )
    {
        /* 内存申请失败 */
        BST_DBG_LOG("BST_MSG_ProcRrrcRealseSend:ERROR: Memory Alloc Error for pstMsg");
        return;
    }

    PS_MEM_SET((VOS_CHAR*)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0x00,
               (sizeof(BASTET_RRC_LTE_TO_IDLE_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写相关参数 */
    pstSndMsg->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstSndMsg->stMsgHeader.ulReceiverPid     = PS_PID_ERRC;
    pstSndMsg->stMsgHeader.ulLength          = sizeof(BASTET_RRC_LTE_TO_IDLE_REQ_STRU)
                                             - VOS_MSG_HEAD_LENGTH;
    pstSndMsg->stMsgHeader.ulMsgName         = ID_BASTET_LRRC_CON_TO_IDLE_NTF;

    /* 调用VOS发送原语 */
    ulRslt = PS_SEND_MSG(UEPS_PID_BASTET, pstSndMsg);
    if ( VOS_OK != ulRslt )
    {
        BST_DBG_LOG("BST_MSG_ProcRrrcRealseSend:ERROR:PS_SEND_MSG ");
        return;
    }

    return;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


