


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CDS.h"
#include "CdsBastetProc.h"
#include "CdsUlProc.h"
#include "CdsDlProc.h"
#include "CdsMsgProc.h"
#include "CdsDebug.h"
#include "CdsIpfCtrl.h"
#include "CdsSoftFilter.h"
//#include "BST_Config.h"
#include "QosFcCommon.h"
#include "TtfNvInterface.h"
#include "SysNvId.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_CDS_BST_PROC_C
/*lint +e767*/

#if (CDS_FEATURE_ON == CDS_FEATURE_BASTET)
/*****************************************************************************
  1 宏定义
*****************************************************************************/
/*Only IP Head + SrcPort + DstPort Length is Enough to judge Protol*/
#define CDS_BST_IP_PORTS_LEN        ( 0x04U )
#define CDS_BST_TCP_STD_LEN         ( 20U )
#define CDS_GetBastetEntity()       ( &g_CdsBstEntity )
#define CDS_GetUsedModemId()        ( g_CdsBstEntity.usModemId )

#define CDS_IsBstHookClosed()       ( CDS_BST_HOOK_LEVEL_NONE        \
                                        == g_CdsBstEntity.ucHookFlag )

#define CDS_IsBstHeadHookClosed()   ( 0 == ( g_CdsBstEntity.ucHookFlag & \
                                      CDS_BST_HOOK_LEVEL_HEAD ) )

#define CDS_IsBstFullHookClosed()   ( 0 == ( g_CdsBstEntity.ucHookFlag & \
                                      CDS_BST_HOOK_LEVEL_FULL ) )
#define CDS_BST_TRACE_MAX_BYTE_LEN     (1514)                  /*捕获TCP/IP协议栈报文最长长度*/

/*****************************************************************************
  2 外部函数声明
*****************************************************************************/
#ifdef BST_MODEM_SELF
extern VOS_VOID DSPP_TEST_EMAIL( VOS_UINT32 ulIpAddr );
#endif
/******************************************************************************
   3 私有定义
******************************************************************************/
VOS_VOID    CDS_ConfigBastetNetState    (
    const BST_CDS_NET_DEVICE_STATE_STRU    *pstNetMsg );

VOS_VOID    CDS_ConfigBastetTxRxEnable  (
    const BST_CDS_TX_RX_RPT_ENABLE_STRU    *pstFlagMsg );

VOS_VOID    CDS_BstHookPacket           (
    const VOS_UINT32                        ulType,
    const TTF_MEM_ST                       *pstIpPkt );

VOS_UINT32  CDS_FwProcBastetData        (
    const VOS_UINT8                        *pData,
    const VOS_UINT32                        ulLen );

VOS_UINT32  CDS_RxDataFromBastet        (
    const VOS_UINT8                        *pData,
    const VOS_UINT32                        ulLen );

VOS_UINT32  CDS_RegBstDlRcvCB           (
    const BST_CDS_REG_RCV_CB_STRU          *pstRegDlRcvFunMsg );

VOS_UINT32 CDS_ConfigAcoreModemRabid    (
    const BST_CDS_NET_ACORE_MODEM_RABID_STRU *pstModemRabIdMsg );

extern VOS_VOID CDS_BST_HOOKIPData(const VOS_UINT32 ulType, VOS_UINT8 *pucData, VOS_UINT16 usLen);

/******************************************************************************
   4 全局变量定义
******************************************************************************/
CDS_BST_ENTITY_STRU g_CdsBstEntity          = { 0 };
/******************************************************************************
   5 函数实现
******************************************************************************/


VOS_VOID CDS_BST_MsgProc(MsgBlock  *pstMsg)
{
    if ( VOS_NULL_PTR == pstMsg )
    {
        CDS_INFO_LOG(UEPS_PID_CDS,"CDS_BST_MsgProc : Input NULL PTR.");
        return;
    }
    CDS_INFO_LOG1( UEPS_PID_CDS, "CDS_BST_MsgProc : MsgId", TTF_GET_MSG_NAME(pstMsg) );
    switch( TTF_GET_MSG_NAME(pstMsg) )
    {
        /* BASTET发给CDS的消息原语: 注册下行回调函数 */
        case ID_BST_CDS_REG_RCV_CB_REQ:
            CDS_RegBstDlRcvCB( (BST_CDS_REG_RCV_CB_STRU*)pstMsg );
            break;

        /* BASTET发给CDS的消息原语: ACORE使用的MODEM_RABID消息*/
        case ID_BST_CDS_NET_MODEM_RAB_IND:
            CDS_ConfigAcoreModemRabid( (const BST_CDS_NET_ACORE_MODEM_RABID_STRU *)
                                       pstMsg );
            break;

        /* BASTET发给CDS的消息原语: 当前NET_DEVICE状态 */
        case ID_BST_CDS_NET_DEVICE_UP_IND:
            CDS_ConfigBastetNetState( (BST_CDS_NET_DEVICE_STATE_STRU *)pstMsg );
            break;

        /* BASTET发给CDS的消息原语: 配置CDS是否发送TX_RX_IND消息 */
        case ID_BST_CDS_TX_RX_RPT_CTRL_REQ:
            CDS_ConfigBastetTxRxEnable( (BST_CDS_TX_RX_RPT_ENABLE_STRU *)pstMsg );
            break;

        default:
            CDS_INFO_LOG(UEPS_PID_CDS,"CDS_BST_MsgProc : Unknown Message.");
            break;
    }
    return;
}


VOS_UINT32 CDS_DecodeBstDlPacket( TTF_MEM_ST *pstIpPkt, CDS_BST_IP_INFO_STRU *pstIpInfo )
{
    IPV4_HDR_STRU          *pstIpv4Hdr  = VOS_NULL_PTR;
    TCP_HDR_STRU           *pstTcpHdr   = VOS_NULL_PTR;
    VOS_UINT8              *pucIpData   = VOS_NULL_PTR;
    VOS_UINT32              ulHdrLen    = 0;
    VOS_UINT32              ulCopeidFlag= PS_FALSE;

    /*保存IP头中信息*/
    pucIpData               = pstIpPkt->pData;
    pstIpv4Hdr              = (IPV4_HDR_STRU *)((VOS_UINT32)( pucIpData ));
    ulHdrLen                = (pstIpv4Hdr->ucIpHdrLen << 2) + CDS_BST_IP_PORTS_LEN;

    /* UMTS情况下，TTF_MEM存在级联情况，不能直接调用softfilter函数*/
    if ( pstIpPkt->usUsed < ulHdrLen )
    {
        pucIpData           = PS_MEM_ALLOC( UEPS_PID_CDS, ulHdrLen );
        if( VOS_NULL_PTR == pucIpData )
        {
            CDS_DBG_BST_FILTER_ALLOC_MEM_FAIL_NUM(1);
            CDS_INFO_LOG1( UEPS_PID_CDS, "CDS_BST_DecodeDl: No memory to Alloc Length", ulHdrLen );
            return PS_FALSE;
        }
        if( PS_SUCC != TTF_MemGet( UEPS_PID_CDS, pstIpPkt, 0,
                                   pucIpData, (VOS_UINT16)ulHdrLen ) )
        {
            CDS_DBG_BST_FILTER_ERROR_COPY_PKT_NUM(1);
            CDS_INFO_LOG1( UEPS_PID_CDS, "CDS_BST_DecodeDl Error Copy %u", ulCopeidFlag );
            PS_MEM_FREE( UEPS_PID_CDS, pucIpData );
            return PS_FALSE;
        }
        CDS_DBG_BST_FILTER_SUCCS_COPY_PKT_NUM(1);
        /* 更新IP头信息 */
        ulCopeidFlag        = PS_TRUE;
        pstIpv4Hdr          = (IPV4_HDR_STRU *)((VOS_UINT32)(pucIpData));
        CDS_INFO_LOG1( UEPS_PID_CDS, "CDS_BST_DecodeDl: Coped OK", ulHdrLen );
    }
#ifdef BST_MODEM_SELF
    if ( IPV4_PROTOCOL_TCP == pstIpv4Hdr->ucProtocol )
    {
        DSPP_TEST_EMAIL( pstIpv4Hdr->unDestAddr.ulIpAddr );
    }
#endif
    /* 非TCP/UDP包，不进行处理 */
    if( ( IPV4_PROTOCOL_TCP != pstIpv4Hdr->ucProtocol )
      &&( IPV4_PROTOCOL_UDP != pstIpv4Hdr->ucProtocol ) )
    {
        CDS_INFO_LOG1( UEPS_PID_CDS,
                       "CDS_BST_DecodeDl: Unknow Protocol Packet",
                       pstIpv4Hdr->ucProtocol );
        CDS_DBG_BST_FILTER_WRONG_PROTOCOL_NUM(1);
        if ( PS_TRUE == ulCopeidFlag )
        {
            PS_MEM_FREE( UEPS_PID_CDS, pucIpData );
        }
        return PS_FALSE;
    }
    pstIpInfo->usProtocol   = pstIpv4Hdr->ucProtocol;
    /* IP 分片，不能确认端口，不处理 */
    if ( IP_NTOHS( pstIpv4Hdr->usOffset ) & ( IPV4_DF_MASK - 0x01U ) )
    {
        CDS_INFO_LOG1( UEPS_PID_CDS,
                       "CDS_BST_DecodeDl: Fragement Packet",
                       IP_NTOHS( pstIpv4Hdr->usOffset ) );
        CDS_DBG_BST_FILTER_FRAGEMENTD_PKT_NUM(1);
        if ( PS_TRUE == ulCopeidFlag )
        {
            PS_MEM_FREE( UEPS_PID_CDS, pucIpData );
        }
        return PS_FALSE;
    }
    pstTcpHdr               = (TCP_HDR_STRU *)((VOS_UINT32)(pucIpData +
                                               (pstIpv4Hdr->ucIpHdrLen << 2)));
    pstIpInfo->usDstPort    = IP_NTOHS( pstTcpHdr->usDstPort );
    if ( PS_TRUE == ulCopeidFlag )
    {
        PS_MEM_FREE( UEPS_PID_CDS, pucIpData );
    }
    return PS_TRUE;
}
VOS_UINT32 CDS_IsBastetPacket( TTF_MEM_ST *pstIpPkt )
{
    CDS_BST_ENTITY_STRU    *pstBstEntity    = VOS_NULL_PTR;
    CDS_BST_IP_INFO_STRU    stIpInfo        = {0};

    if ( PS_FALSE == CDS_GetBastetSupportFlag())
    {
        return PS_FALSE;
    }

    pstBstEntity            = CDS_GetBastetEntity();
    if ( VOS_NULL_PTR == pstBstEntity->pfPortCheck )
    {
        return PS_FALSE;
    }

    if ( CDS_BASTET_NET_DEVICE_UNKNOW == pstBstEntity->usNetState )
    {
        CDS_DBG_BST_FILTER_NID_ERR_NUM(1);
        return PS_FALSE;
    }
   /*                      *
    * 工作在垃圾包处理模式 *
    *                      */
    if ( CDS_BASTET_NET_DEVICE_DOWN == pstBstEntity->usNetState )
    {
        CDS_DBG_BST_FILTER_RUBBISH_NUM(1);
        return PS_TRUE;
    }
    if ( VOS_NULL_PTR == pstIpPkt )
    {
        CDS_ERROR_LOG( UEPS_PID_CDS, "CDS_IsBastetPacket: pstIpPkt = NULL" );
        return PS_FALSE;
    }
    if ( IP_VERSION_V4 != IP_GET_VERSION( pstIpPkt->pData) )
    {
        CDS_DBG_BST_FILTER_NOT_IPVER4_PKT_NUM(1);
        CDS_INFO_LOG( UEPS_PID_CDS, "CDS_IsBastetPacket: Not Ip V4 Packet" );
        return PS_FALSE;
    }
    if ( PS_FALSE == CDS_DecodeBstDlPacket( pstIpPkt, &stIpInfo ) )
    {
        CDS_INFO_LOG( UEPS_PID_CDS, "CDS_IsBastetPacket: Decode Ip V4 Packet Error" );
        return PS_FALSE;
    }
    return pstBstEntity->pfPortCheck( stIpInfo.usProtocol, stIpInfo.usDstPort );
}


VOS_VOID CDS_DlProcBastetData( TTF_MEM_ST  *pstIpPkt,
                               VOS_UINT8    ucDeftRabId,
                               VOS_UINT16   usModemId,
                               VOS_UINT32   ulSduLen )
{
    CDS_BST_ENTITY_STRU    *pstBstEntity;

    if ( VOS_NULL_PTR == pstIpPkt )
    {
        CDS_DBG_BST_DL_RX_FUN_PARA_ERR(1);
        CDS_ERROR_LOG( UEPS_PID_CDS,"Enter CDS_DlProcBastetData: pstIpPkt = NULL " );
        return;
    }

    pstBstEntity            = CDS_GetBastetEntity();
    if ( VOS_NULL_PTR == pstBstEntity->pfRcvDlPacket )
    {
        CDS_DBG_BST_DL_RX_FUN_PARA_ERR(1);
        CDS_ERROR_LOG( UEPS_PID_CDS,"Enter CDS_DlProcBastetData: pfRcvDlPacket = NULL " );
        return;
    }

    CDS_DBG_BST_DL_RX_BST_PKT_NUM(1);
    CDS_DL_SAVE_LEN_MODEMID_RABID_TO_TTF( pstIpPkt,
                                          ulSduLen,
                                          usModemId,
                                          ucDeftRabId );
    pstBstEntity->pfRcvDlPacket( (VOS_UINT8 *)pstIpPkt,
                                 (VOS_UINT16)sizeof(TTF_MEM_ST) );
    /*钩包*/
    CDS_BstHookPacket( CDS_BST_HOOK_TYPE_DL, pstIpPkt );
    /*发送消息*/
    CDS_DBG_BST_DL_SEND_TO_NIC_SUCC_NUM(1);
    return;
}


VOS_UINT32 CDS_RxDataFromBastet( const VOS_UINT8 *pData, const VOS_UINT32 ulLen )
{
    CDS_ENTITY_STRU            *pstCdsEntity= VOS_NULL_PTR;
    TTF_MEM_ST                 *pstTtfPkt   = VOS_NULL_PTR;
    VOS_INT32                   lLock;

    CDS_DBG_BST_UL_RX_FUN_CALL_NUM(1);
    /*入参判断*/
    if ( VOS_NULL_PTR == pData )
    {
        CDS_DBG_BST_UL_RX_FUN_PARA_ERR(1);
        CDS_INFO_LOG( UEPS_PID_CDS,"CDS_RxDataFromBastet: NULL pData" );
        return 0;
    }

    if( ( ulLen > ETH_MAX_MTU ) || ( 0 == ulLen ) )
    {
        CDS_DBG_BST_UL_RX_FUN_PARA_ERR(1);
        CDS_INFO_LOG1( UEPS_PID_CDS, "CDS_RxDataFromBastet: Packet Length Error.", ulLen );
        return 0;
    }

    if ( CDS_GetUsedModemId() >= MODEM_ID_BUTT )
    {
        CDS_DBG_BST_UL_RX_FUN_PARA_ERR(1);
        CDS_INFO_LOG1( UEPS_PID_CDS,"CDS_RxDataFromBastet: Invalid Modem Id.", CDS_GetUsedModemId() );
        return 0;
    }
    pstCdsEntity                = CDS_GetCdsEntity( CDS_GetUsedModemId() );
    if (VOS_NULL_PTR == pstCdsEntity)
    {
        CDS_DBG_BST_UL_RX_FUN_PARA_ERR(1);
        CDS_INFO_LOG1( UEPS_PID_CDS, "CDS_RxDataFromBastet: Modem ID Error.", CDS_GetUsedModemId() );
        return 0;
    }
    CDS_DBG_BST_UL_RX_NIC_PKT_NUM(1);

    /*申请TTF内存,并将其拷贝到TTF内存中*/
    pstTtfPkt                   = CDS_AllocTtfMem(ulLen);
    if ( VOS_NULL_PTR == pstTtfPkt )
    {
        CDS_DBG_BST_UL_ALLOC_MEM_FAIL_NUM(1);
        CDS_INFO_LOG( UEPS_PID_CDS, "CDS_RxDataFromBastet: Alloc Mem Fail." );
        return 0;
    }
    PS_MEM_CPY( pstTtfPkt->pData, pData, ulLen );

    /*入队*/
    lLock                       = VOS_SplIMP();
    if ( PS_SUCC != LUP_EnQue( pstCdsEntity->pstBastetUlQue, pstTtfPkt ) )
    {
        CDS_DBG_BST_UL_EN_QUE_FAIL_NUM(1);
        TTF_MemFree( UEPS_PID_CDS, pstTtfPkt );
        VOS_Splx(lLock);
        return 0;
    }
    CDS_DBG_BST_UL_EN_QUE_SUCC_NUM(1);
    VOS_Splx(lLock);
    /*wakeup cds*/
    CDS_SendEventToCds( CDS_EVENT_UL_BASTET_PROC );
    CDS_DBG_BST_UL_TRIGGER_BST_EVENT_NUM(1);
    return ulLen;
}


VOS_UINT32 CDS_UlProcBastetData( VOS_VOID )
{
    VOS_UINT32              ulCnt;
    VOS_INT32               lLock;
    VOS_UINT16              usFilterResult;
    TTF_MEM_ST             *pstIpPkt;
    CDS_ENTITY_STRU        *pstCdsEntity;
    CDS_BST_ENTITY_STRU    *pstBstEntity;

    pstBstEntity            = CDS_GetBastetEntity();
    pstCdsEntity            = CDS_GetCdsEntity( pstBstEntity->usModemId );

    if (  pstBstEntity->usModemId >= MODEM_ID_BUTT )
    {
        CDS_ERROR_LOG( UEPS_PID_CDS, "Enter CDS_UlProcBastetData: WRONG Modem ID " );
        return PS_FAIL;
    }

    if (VOS_NULL_PTR == pstCdsEntity)
    {
        CDS_ERROR_LOG( UEPS_PID_CDS, "Enter CDS_UlProcBastetData: CDS_GetCdsEntity Fail " );
        return PS_FAIL;
    }

    for ( ulCnt = 0; ulCnt < CDS_BASTET_QUE_SIZE; ulCnt ++ )
    {
        lLock               = VOS_SplIMP();
        if (PS_SUCC != LUP_DeQue( pstCdsEntity->pstBastetUlQue, ( VOS_VOID **)&pstIpPkt) )
        {
            VOS_Splx(lLock);
            break;
        }
        VOS_Splx(lLock);
        if ( MMC_CDS_MODE_LTE == pstCdsEntity->enRanMode )
        {
            usFilterResult      = 0;
            /*LTE上行软过滤*/
            if (PS_SUCC != CDS_IpSoftFilter(pstIpPkt, &usFilterResult, pstCdsEntity))
            {
                CDS_DBG_BST_UL_SOFT_FILTER_FAIL_NUM(1);
                TTF_MemFree( UEPS_PID_CDS, pstIpPkt );
                continue;
            }
            /*将过滤结果存到TTF中*/
            CDS_UL_SAVE_IPFRSLT_TO_TTF(pstIpPkt, usFilterResult);
        }
        else if ( MMC_CDS_MODE_GU == pstCdsEntity->enRanMode )
        {
            CDS_UL_SAVE_IPFRSLT_MODEMID_RABID_TO_TTF( pstIpPkt, 0,
                                                      pstBstEntity->usModemId,  pstBstEntity->ucRabId );
        }
        else
        {
            TTF_MemFree( UEPS_PID_CDS, pstIpPkt );
            continue;
        }
        /*钩包*/
        CDS_BstHookPacket( CDS_BST_HOOK_TYPE_UL, pstIpPkt );
        CDS_UlDispatchDataByRanMode( pstCdsEntity, pstIpPkt );
        CDS_DBG_BST_UL_SEND_PKT_TO_RAN_NUM(1);
    }
    return PS_SUCC;
}
VOS_UINT32 CDS_FwProcBastetData(
    const VOS_UINT8        *pData,
    const VOS_UINT32        ulLen )
{
    TTF_MEM_ST             *pstPktItem;

    CDS_DBG_BST_FW_PROC_BST_EVENT_NUM(1);
    if ( VOS_NULL_PTR == pData )
    {
        CDS_DBG_BST_FW2AP_FUN_PARA_ERR(1);
        CDS_INFO_LOG( UEPS_PID_CDS, "CDS_FwProcBastetData Null Message" );
        return 0;
    }
    if ( (VOS_UINT32)sizeof(TTF_MEM_ST) != ulLen )
    {
        CDS_DBG_BST_FW2AP_FUN_PARA_ERR(1);
        CDS_INFO_LOG( UEPS_PID_CDS, "CDS_FwProcBastetData Wrong TTF Length" );
        return 0;
    }
    pstPktItem                  =(TTF_MEM_ST *)pData;
    /*钩包*/
    CDS_BstHookPacket( CDS_BST_HOOK_TYPE_FW, pstPktItem );

    /*入队*/
    if (PS_SUCC != CDS_RxDlSdu(pstPktItem))
    {
        CDS_DBG_BST_FW2AP_QUE_FAIL_NUM(1);
        return 0;
    }
    CDS_SendEventToCds( CDS_EVENT_DL_DATA_PROC );
    CDS_DBG_BST_FW2AP_QUE_SUCC_NUM(1);
    /*返回TTF格式长度，代表数据正常发送，为0表示没有发送成功*/
    return (VOS_UINT32)sizeof(TTF_MEM_ST);
}
VOS_VOID CDS_BstHookPacketHead(
    const VOS_UINT32        ulType,
    const TTF_MEM_ST       *pstIpPkt )
{
    VOS_UINT16              usUsedLen   = 0;
    VOS_UINT16              usHeadLen   = 0;
    VOS_UINT8              *pucData     = VOS_NULL_PTR;
    VOS_UINT16              usDataLen   = 0;

    usHeadLen       = ( ( pstIpPkt->pData[0]&0x0f )<< 2 );
    usDataLen       = usHeadLen + CDS_BST_TCP_STD_LEN;
    if ( pstIpPkt->usUsed >= ( usHeadLen + CDS_BST_TCP_STD_LEN ) )
    {
        pucData     = pstIpPkt->pData;
        usUsedLen   = 0;
    }
    else
    {
        usUsedLen   = CDS_BST_TCP_STD_LEN + usHeadLen;
        pucData     = (VOS_UINT8 *)PS_MEM_ALLOC( UEPS_PID_CDS, usUsedLen );
        if ( VOS_NULL_PTR == pucData )
        {
            CDS_INFO_LOG1( UEPS_PID_CDS, "CDS_BstHookPacket No Memory, Size", usUsedLen );
            return;
        }
        if ( PS_SUCC != TTF_MemGet( UEPS_PID_CDS, (TTF_MEM_ST *)pstIpPkt,
                                    0, pucData, usUsedLen ) )
        {
            CDS_INFO_LOG( UEPS_PID_CDS, "CDS_BstHookPacket Get IP+TCP Head Error" );
            PS_MEM_FREE( UEPS_PID_CDS, pucData );
            return;
        }
    }

    CDS_BST_HOOKIPData(ulType,pucData,usDataLen);
    if ( 0 != usUsedLen )
    {
        PS_MEM_FREE( UEPS_PID_CDS, pucData);
    }
    return;
}


VOS_VOID CDS_BstHookPacket(
    const VOS_UINT32        ulType,
    const TTF_MEM_ST       *pstIpPkt )
{

    VOS_UINT8              *pucTmpData;
    VOS_UINT16              usLen;

    if ( CDS_IsBstHookClosed() )
    {
        return;
    }
    if ( VOS_NULL_PTR == pstIpPkt )
    {
        CDS_INFO_LOG( UEPS_PID_CDS, "CDS_BstHookPacket Null Message" );
        return;
    }

    if ( !CDS_IsBstHeadHookClosed() )
    {
        CDS_BstHookPacketHead( ulType, pstIpPkt );
    }

    if ( CDS_IsBstFullHookClosed() )
    {
        return;
    }

    usLen      = (VOS_UINT16)TTF_MemGetLen(UEPS_PID_CDS, (TTF_MEM_ST *)pstIpPkt);
    pucTmpData = VOS_NULL_PTR;

    pucTmpData = (VOS_UINT8 *)PS_MEM_ALLOC(UEPS_PID_CDS,usLen);
    if ( VOS_NULL_PTR == pucTmpData )
    {
        CDS_INFO_LOG1( UEPS_PID_CDS, "CDS_BstHookPacket No Memory, Size", usLen );
        return;
    }
    if(TTF_MemGet(UEPS_PID_CDS,(TTF_MEM_ST *)pstIpPkt, 0U, pucTmpData, usLen))
    {
         PS_MEM_FREE(UEPS_PID_CDS, pucTmpData);
         return;
    }
    CDS_BST_HOOKIPData(ulType,pucTmpData,usLen);
    PS_MEM_FREE(UEPS_PID_CDS, pucTmpData);
    return;
}


VOS_VOID CDS_ConfigBastetNetState( const BST_CDS_NET_DEVICE_STATE_STRU *pstNetMsg )
{
    CDS_BST_ENTITY_STRU        *pstBstEntity = VOS_NULL_PTR;
    pstBstEntity                = CDS_GetBastetEntity();
    if ( VOS_NULL_PTR == pstNetMsg )
    {
        CDS_INFO_LOG( UEPS_PID_CDS, "CDS_ConfigBastetNetState Null Message" );
        return;
    }
    pstBstEntity->usNetState    = (VOS_UINT16)pstNetMsg->ulNetDeviceState;
}


VOS_UINT32 CDS_RegBstDlRcvCB( const BST_CDS_REG_RCV_CB_STRU *pstRegDlRcvFunMsg )
{
    CDS_BST_ENTITY_STRU        *pstBstEntity    = VOS_NULL_PTR;
    pstBstEntity                = CDS_GetBastetEntity();

    if ( VOS_NULL_PTR == pstRegDlRcvFunMsg->pfRcvCb )
    {
        CDS_ERROR_LOG( UEPS_PID_CDS, "CDS_RegBstDlRcvCB NULL DATA" );
        return PS_FAIL;
    }
    pstBstEntity->pfRcvDlPacket = pstRegDlRcvFunMsg->pfRcvCb;
    pstBstEntity->pfPortCheck   = pstRegDlRcvFunMsg->pfChkPortCb;
    return PS_SUCC;
}



VOS_UINT32 CDS_ConfigAcoreModemRabid(
    const BST_CDS_NET_ACORE_MODEM_RABID_STRU *pstModemRabIdMsg )
{
    CDS_BST_ENTITY_STRU        *pstBstEntity    = VOS_NULL_PTR;
    pstBstEntity                = CDS_GetBastetEntity();

    pstBstEntity->usModemId     = pstModemRabIdMsg->usModemId;
    pstBstEntity->ucRabId       = (VOS_UINT8)pstModemRabIdMsg->usRabId;
    CDS_INFO_LOG2( UEPS_PID_CDS, "CDS_ConfigAcoreModemRabid config modem id:%d,RABID:%d",
                           pstBstEntity->usModemId,  pstBstEntity->ucRabId);
    return PS_SUCC;
}

VOS_VOID CDS_ConfigBastetTxRxEnable( const BST_CDS_TX_RX_RPT_ENABLE_STRU *pstFlagMsg )
{
    CDS_BST_ENTITY_STRU        *pstBstEntity    = VOS_NULL_PTR;

    pstBstEntity                = CDS_GetBastetEntity();
    if ( VOS_NULL_PTR == pstFlagMsg )
    {
        CDS_INFO_LOG( UEPS_PID_CDS, "CDS_ConfigBastetTxRxEnable Null Message" );
        return;
    }
    pstBstEntity->ucRptEnable   = (VOS_UINT8)pstFlagMsg->ulEnableFlag;
    return;
}


VOS_VOID CDS_BstSendRsmCbMsg( VOS_VOID )
{
    BST_CDS_SEND_CB_INFO_STRU  *pstChnlInfoMsg  = VOS_NULL_PTR;
    VOS_UINT32                  ulMsgLen;

    ulMsgLen                    = sizeof( BST_CDS_SEND_CB_INFO_STRU );
    pstChnlInfoMsg              = CDS_ALLOC_MSG_WITH_HDR( ulMsgLen );

    if (VOS_NULL_PTR == pstChnlInfoMsg)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS,"CDS_UpdateBastetModemId : Alloc Msg Fail. Size .", ulMsgLen );
        return;
    }

    /*填写消息内容*/
    CDS_CFG_MSG_HDR( pstChnlInfoMsg, UEPS_PID_BASTET );
    pstChnlInfoMsg->ulMsgId     = ID_BST_CDS_CHANNEL_INFO_IND;
    pstChnlInfoMsg->pSendCbFun  = CDS_RxDataFromBastet;
    pstChnlInfoMsg->pResmCbFun  = CDS_FwProcBastetData;

    /*发送消息*/
    CDS_SEND_MSG( pstChnlInfoMsg );
    return;
}


VOS_VOID CDS_BstSendTxRxInd( VOS_VOID )
{
    BST_CDS_TX_RX_MSG_STRU     *pstTrxMsg;
    CDS_BST_ENTITY_STRU        *pstBstEntity;
    VOS_UINT32                  ulMsgLen;

    if ( PS_FALSE == CDS_GetBastetSupportFlag() )
    {
        return;
    }



    pstBstEntity                = CDS_GetBastetEntity();
    if ( PS_FALSE == pstBstEntity->ucRptEnable )
    {
        return;
    }
    ulMsgLen                = sizeof( BST_CDS_TX_RX_MSG_STRU );
    pstTrxMsg               = CDS_ALLOC_MSG_WITH_HDR( ulMsgLen );

    if (VOS_NULL_PTR == pstTrxMsg)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS,"CDS_NoticeBastetTxRx : Alloc Msg Fail. Size .",ulMsgLen);
        return;
    }

    pstBstEntity->ucRptEnable   = PS_FALSE;

    /*填写消息内容*/
    CDS_CFG_MSG_HDR( pstTrxMsg, UEPS_PID_BASTET );
    pstTrxMsg->ulMsgId      = ID_BST_CDS_TX_RX_IND;
    /*发送消息*/
    CDS_SEND_MSG(pstTrxMsg);
    return;
}


VOS_VOID CDS_BstInit(VOS_VOID)
{
    VOS_UINT32                          ulReturnCode;
    BASTET_SUPPORT_FLG_STRU             stBastetFlag;

    /* 读NV项en_NV_Item_SVLTE_FLAG，失败，直接返回 */
    ulReturnCode = NV_ReadEx(MODEM_ID_0,en_NV_Item_Bastet_CONFIG,
                      &stBastetFlag, sizeof(BASTET_SUPPORT_FLG_STRU));
    if (PS_SUCC != ulReturnCode)
    {
         CDS_INFO_LOG(UEPS_PID_CDS, "read nv en_NV_Item_Bastet_CONFIG failed.");
         return;
    }

    /* 赋值到全局变量中 */
    g_CdsBstEntity.ucNvFlag            = stBastetFlag.ucActiveFlg;
    g_CdsBstEntity.ucHookFlag          = stBastetFlag.ucHookFlg;
    g_CdsBstEntity.ucRabId             = FC_MIN_RABID;
    g_CdsBstEntity.ucRptEnable         = PS_TRUE;
    g_CdsBstEntity.usNetState          = CDS_BASTET_NET_DEVICE_UNKNOW;
    g_CdsBstEntity.usModemId           = MODEM_ID_0;
    g_CdsBstEntity.pfRcvDlPacket       = VOS_NULL_PTR;
    g_CdsBstEntity.pfPortCheck         = VOS_NULL_PTR;
    CDS_BstSendRsmCbMsg();
    return;
}




PS_BOOL_ENUM_UINT8 CDS_GetBastetSupportFlag(VOS_VOID)
{
    CDS_BST_ENTITY_STRU        *pstBstEntity;

    pstBstEntity                = CDS_GetBastetEntity();

    return pstBstEntity->ucNvFlag;
}



/*lint -save -e669*/
VOS_VOID CDS_BST_HOOKIPData( const VOS_UINT32 ulType, VOS_UINT8 *pucData, VOS_UINT16 usLen )
{
    BST_CDS_HOOK_STRU          *pstHookInfoMsg  = VOS_NULL_PTR;
    VOS_UINT32                  ulMsgLen;
    VOS_UINT32                  uldatalen;
    VOS_UINT32                  ulMsgId;
    VOS_UINT32                  ulSenderPid;
    VOS_UINT32                  ulReceiverPid;

    if(VOS_NULL_PTR == pucData)
    {
        return;
    }

    if( ulType == CDS_BST_HOOK_TYPE_UL)
    {
        ulMsgId                         = ID_BST_CDS_UL_IP_PKG_INFO_IND;
        ulSenderPid                     = UEPS_PID_BASTET;
        ulReceiverPid                   = UEPS_PID_CDS;
    }
    else if( ulType == CDS_BST_HOOK_TYPE_DL)
    {
        ulMsgId                         = ID_BST_CDS_DL_IP_PKG_INFO_IND;
        ulSenderPid                     = UEPS_PID_CDS;
        ulReceiverPid                   = UEPS_PID_BASTET;

    }
    else if( ulType == CDS_BST_HOOK_TYPE_FW)
    {   
        ulMsgId                         = ID_BST_CDS_FW_IP_PKG_INFO_IND;
        ulSenderPid                     = UEPS_PID_BASTET;
        ulReceiverPid                   = UEPS_PID_CDS;
    }
    else
    {
        return;
    }

    ulMsgLen                            = offsetof(BST_CDS_HOOK_STRU, aucData);
    uldatalen                           = PS_MIN(CDS_BST_TRACE_MAX_BYTE_LEN, usLen);
    pstHookInfoMsg                      = CDS_ALLOC_MSG_WITH_HDR( ulMsgLen + uldatalen );

    if (VOS_NULL_PTR == pstHookInfoMsg)
    {
        CDS_ERROR_LOG1(UEPS_PID_CDS,"CDS_BST_HOOKIPData : Alloc Msg Fail. Size .", ulMsgLen + uldatalen );
        return;
    }

    /*填写消息内容*/
    pstHookInfoMsg->ulSenderCpuId       = VOS_LOCAL_CPUID;
    pstHookInfoMsg->ulSenderPid         = ulSenderPid;
    pstHookInfoMsg->ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstHookInfoMsg->ulReceiverPid       = ulReceiverPid;
    pstHookInfoMsg->ulMsgId             = ulMsgId;
    pstHookInfoMsg->usLen               = (VOS_UINT16)uldatalen;

    PS_MEM_CPY( pstHookInfoMsg->aucData, pucData, uldatalen );

    OM_TraceMsgHook(pstHookInfoMsg);

    PS_FREE_MSG(UEPS_PID_CDS, pstHookInfoMsg);
/*lint -e429*/
    return;
}
/*lint +e429*/
/*lint -restore*/
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

