

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_IP_RcverMng.h"
#include "BST_DBG_MemLeak.h"
#include "BST_Platform.h"
#include "BST_IP_PreProc.h"
#include "BST_IP_LwipApi.h"
#include "lwip/sockets.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "BST_OS_Memory.h"
#include "BST_APP_MainTask.h"
#ifdef   BST_SSL_SUPPORT
#include "BST_OS_Openssl_Rename.h"
#include "openssl/ssl.h"
#endif

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_IP_LWIPAPI_CPP
/*lint +e767*/

/*****************************************************************************
  2 函数声明
*****************************************************************************/
#ifdef   BST_SSL_SUPPORT
static SSL *BST_IP_SslInit(BST_ARG_T *Arg);
#endif

extern "C" int lwip_cloneconnect( int s,
                                  const struct sockaddr *name,
                                  socklen_t namelen );
/******************************************************************************
  3 私有定义
******************************************************************************/

/******************************************************************************
  4 全局变量定义
******************************************************************************/
const BST_IP_SOCKET_FUN_LUT_STRU    g_BST_IP_SocketFunTable[ BST_SCKT_TYPE_NUMBER ]
                                        = { BST_IP_SOCKET_FUNCTION_TABLE };
const BST_SCKT_TYPE_ENUM_UINT32     g_BST_IP_SocketSupportTable[]
                                        = { BST_IP_SOCKET_SUPPORT_TABLE };

/******************************************************************************
  5 函数实现
******************************************************************************/


BST_IP_ERR_T BST_IP_CheckSocketSupport( BST_SCKT_TYPE_ENUM_UINT32 enSocketType )
{
    BST_UINT16                          usTblLen, i;
    const BST_SCKT_TYPE_ENUM_UINT32    *penSupportTblHead;

    usTblLen                            = BST_OS_GetTableLength( g_BST_IP_SocketSupportTable );
    penSupportTblHead                   = BST_IP_SocketSupportTblHead();

    for( i=0; i<usTblLen; i++ )
    {
        if( penSupportTblHead[i] == enSocketType )
        {
            return BST_IP_ERR_OK;
        }
    }
    return BST_IP_ERR_ARG;
}


err_t BST_IP_TcpOverFlowCB( BST_UINT32 ulRealFlowValue )
{
    BST_APP_CMainTask                  *pMainTask;
    BST_ERR_ENUM_UINT8                  enRtnVal;

    pMainTask                           = BST_APP_CMainTask::GetInstance();
    if ( BST_NULL_PTR == pMainTask )
    {
        return BST_IP_ERR_MEM;
    }
    enRtnVal                            = pMainTask->TrafficOverFlow( ulRealFlowValue );
    if ( BST_NO_ERROR_MSG != enRtnVal )
    {
        BST_RLS_LOG1( "BST_IP_TcpOverFlowCB Error=%u", enRtnVal );
        return BST_IP_ERR_BUF;
    }
    return BST_IP_ERR_OK;
}


err_t BST_IP_TcpConnectedCB( BST_VOID *arg, struct tcp_pcb *tpcb, err_t err )
{
    err_t                               lErrType;
    BST_IP_CRcverMng                   *pcRcverMnger;
    BST_FD_T                            fd;
    BST_OS_LOCKCNT_T                    tThreadLockCnt;

    BST_ASSERT_NULL_RTN( tpcb, ERR_ARG );

    tThreadLockCnt                      = BST_OS_ThreadLock();
    pcRcverMnger                        = BST_IP_CRcverMng::GetInstance();
    BST_DBG_LOG1( "BST_IP_Tcp connect successfully , err: %d", err );

    if ( BST_NULL_PTR == pcRcverMnger )
    {
        BST_OS_ThreadUnLock( tThreadLockCnt );
        return ERR_MEM;
    }
    fd.pFd                              = tpcb;
    lErrType                            = pcRcverMnger->CnctedRcver( fd, err );
    BST_OS_ThreadUnLock( tThreadLockCnt );
    return lErrType;
}


BST_VOID BST_IP_TcpErrCB( BST_VOID *arg, BST_IP_ERR_T err )
{
    BST_IP_CRcverMng                   *pcRcverMnger;
    BST_FD_T                            FdTmp;
    BST_OS_LOCKCNT_T                    tThreadLockCnt;

    tThreadLockCnt                      = BST_OS_ThreadLock();
    pcRcverMnger                        = BST_IP_CRcverMng::GetInstance();
    if ( ( BST_NULL_PTR == pcRcverMnger ) || ( BST_IP_IsLwipNull(arg) ) )
    {
        BST_OS_ThreadUnLock( tThreadLockCnt );
        return;
    }
    FdTmp.pFd                           = arg;
    pcRcverMnger->ErrMsgRcver( FdTmp, err );

    BST_OS_ThreadUnLock( tThreadLockCnt );
}


/*lint -e438*/
err_t BST_IP_TcpRecvCB( BST_VOID *arg, struct tcp_pcb * tpcb, struct pbuf *p, BST_IP_ERR_T err )
{
    err_t                               ucErrMsg;
    BST_UINT16                          usCopyedLength;
    BST_IP_CRcverMng                   *pcRcverMnger;
    BST_IP_PKTPROC_MODE_ENUM            enProcMode;
    struct pbuf                        *q;
    BST_UINT8                          *pucSdu;
    BST_FD_T                            FdTmp;
    BST_OS_LOCKCNT_T                    tThreadLockCnt;

    BST_ASSERT_NULL_RTN( tpcb, ERR_ARG );

    tThreadLockCnt                      = BST_OS_ThreadLock();
    FdTmp.pFd                           = tpcb;
    pcRcverMnger                        = BST_IP_CRcverMng::GetInstance();
    pucSdu                              = BST_NULL_PTR;
    q                                   = BST_NULL_PTR;
    usCopyedLength                      = 0;
    ucErrMsg                            = ERR_OK;
    enProcMode                          = BST_IP_PKT_REMOVE_PROC;

    if( BST_NULL_PTR == pcRcverMnger )
    {
        ucErrMsg                        = ERR_MEM;
        BST_IP_TCP_RCVAPI_FREE();
    }

   /*                                          *
    * p=NULL means this pcb is closed by Remote*
    *                                          */
    if( BST_IP_IsLwipNull(p) )
    {
        pcRcverMnger->ClosedRcver ( FdTmp );
        BST_OS_ThreadUnLock( tThreadLockCnt );
        return ERR_OK;
    }

    if ( p->tot_len > 0 )
    {
        pucSdu = ( BST_UINT8 * )BST_OS_MALLOC( p->tot_len );
    }
    else
    {
        ucErrMsg                        = ERR_BUF;
        tcp_recved( tpcb, p->tot_len );
        BST_IP_TCP_RCVAPI_FREE();
    }

    if ( BST_NULL_PTR == pucSdu )
    {
        ucErrMsg                        = ERR_BUF;
        tcp_recved( tpcb, p->tot_len );
        BST_IP_TCP_RCVAPI_FREE();
    }

    q                                   = p;
    usCopyedLength                      = 0;

    while ( BST_NULL_PTR != q )
    {
        if ( ( q->len > 0 ) && ( BST_NULL_PTR != q->payload )
             && ( q->len + usCopyedLength <= p->tot_len ) )
        {
            BST_OS_MEMCPY( pucSdu + usCopyedLength, q->payload, q->len );
        }
        else
        {
            break;
        }

        usCopyedLength                 += q->len;
        q                               = q->next;
    }

    if ( usCopyedLength > 0 )
    {
        BST_IP_ApiRecordLastSktProp( (BST_IP_PKT_ID_T)p );
        /*                                                                       *
        *The Received Message must put it here where is before call Application CB.
        *This can avoid app close socket in callback when the rcv_wnd != TCP_WND.
        *                                                                        */
        tcp_recved( tpcb, p->tot_len );
        enProcMode                      = pcRcverMnger->PacketRcver( FdTmp,
                                                                     ( BST_UINT8 * )pucSdu,
                                                                     usCopyedLength );
        if( BST_IP_PKT_FORWARD_PROC == enProcMode )
        {
            BST_IP_ApiForwordPacket( (BST_IP_PKT_ID_T)p );
        }

        ucErrMsg                        = ERR_OK;
    }
    else
    {
        ucErrMsg                        = ERR_BUF;
    }
    BST_OS_FREE( pucSdu );
    BST_IP_TCP_RCVAPI_FREE();
}
/*lint +e438*/

/*lint -e438*/
BST_VOID BST_IP_UdpRecvCB( BST_VOID *arg, struct udp_pcb *upcb, struct pbuf *p,  BST_IPADDR_U32T *addr, BST_UINT16 port )
{
    BST_UINT16                          usCopyedLength;
    BST_IP_CRcverMng                   *pcRcverMnger;
    BST_IP_PKTPROC_MODE_ENUM            enProcMode;
    struct pbuf                        *q;
    BST_UINT8                          *pucSdu;
    BST_FD_T                            FdTmp;
    BST_OS_LOCKCNT_T                    tThreadLockCnt;

    BST_ASSERT_NULL( addr );
    BST_ASSERT_NULL( upcb );
    BST_ASSERT_NULL( p );

    tThreadLockCnt                      = BST_OS_ThreadLock();
    FdTmp.pFd                           = upcb;
    pucSdu                              = BST_NULL_PTR;
    q                                   = BST_NULL_PTR;
    usCopyedLength                      = 0;
    enProcMode                          = BST_IP_PKT_REMOVE_PROC;
    pcRcverMnger                        = BST_IP_CRcverMng::GetInstance();

    if( BST_NULL_PTR == pcRcverMnger )
    {
        BST_IP_UDP_RCVAPI_FREE();
    }

    if ( p->tot_len > 0 )
    {
        pucSdu = ( BST_UINT8 * )BST_OS_MALLOC( p->tot_len );
    }
    else
    {
        BST_IP_UDP_RCVAPI_FREE();
    }

    if ( BST_NULL_PTR == pucSdu )
    {
        BST_IP_UDP_RCVAPI_FREE();
    }

    q                                   = p;
    usCopyedLength                      = 0;

    while ( BST_NULL_PTR != q )
    {
        if ( ( q->len > 0 ) && ( BST_NULL_PTR != q->payload )
             && ( q->len + usCopyedLength <= p->tot_len ) )
        {
            BST_OS_MEMCPY( pucSdu + usCopyedLength, q->payload, q->len );
        }
        else
        {
            break;
        }

        usCopyedLength                 += q->len;
        q                               = q->next;
    }

    if ( usCopyedLength > 0 )
    {
        enProcMode                      = pcRcverMnger->PacketRcver( FdTmp,
                                                                ( BST_UINT8 * )pucSdu,
                                                                 usCopyedLength );
        if( BST_IP_PKT_FORWARD_PROC == enProcMode )
        {
            BST_IP_ApiForwordPacket( (BST_IP_PKT_ID_T)p );
        }
    }
    BST_OS_FREE( pucSdu );
    BST_IP_UDP_RCVAPI_FREE();
}
/*lint +e438*/

BST_IP_ERR_T BST_IP_RawTcpSocket( BST_FD_T *pfd )
{
    struct tcp_pcb                     *pstPcb;

    pstPcb                              = tcp_new();
    BST_DBG_LOG1( "BST_IP_RawTcpSocket, pstPcb = %p", pstPcb );
    if ( !BST_IP_IsLwipNull(pstPcb) )
    {
        tcp_recv( pstPcb, LWIP_NULL_PTR );
        tcp_err( pstPcb, LWIP_NULL_PTR );
        tcp_arg( pstPcb, pstPcb );
        pstPcb->flags                  |= TF_NODELAY;
        pfd->pFd                        = pstPcb;
        return BST_IP_ERR_OK;
    }
    else
    {
        return BST_IP_ERR_MEM;
    }
}


BST_IP_ERR_T BST_IP_RawTcpConnect( BST_FD_T fd, BST_ARG_T Arg, BST_IP_SOCKET_ADD_T *pAddr )
{
    struct ip_addr                      stLwipAddr;
    struct tcp_pcb                     *pstPcb;


    if( !BST_IP_IsRawFdValid( fd ) )
    {
        return BST_IP_ERR_MEM;
    }

    if( BST_NULL_PTR == pAddr )
    {
        return BST_IP_ERR_MEM;
    }

    BST_IP_GetU32Addr( stLwipAddr.addr,
                       pAddr->aucRemoteIp[3],
                       pAddr->aucRemoteIp[2],
                       pAddr->aucRemoteIp[1],
                       pAddr->aucRemoteIp[0] );
    pstPcb                              = ( struct tcp_pcb * )fd.pFd;

    BST_DBG_LOG2("ip = %d,remote port = %d", stLwipAddr.addr, pAddr->usRemotePort);
    return tcp_connect( pstPcb, &stLwipAddr, pAddr->usRemotePort, LWIP_NULL_PTR );
}


BST_IP_ERR_T BST_IP_RawTcpClone( BST_FD_T fd, BST_IP_SOCKET_ADD_T *pAddr )
{
    struct ip_addr                      stLwipAddr;
    struct tcp_pcb                     *pstPcb;
    BST_IP_ERR_T                        lRtnVal;

    lRtnVal                             = BST_IP_ERR_OK;
    if( !BST_IP_IsRawFdValid( fd ) )
    {
        return BST_IP_ERR_MEM;
    }
    if( BST_NULL_PTR == pAddr )
    {
        return BST_IP_ERR_MEM;
    }

    BST_IP_GetU32Addr( stLwipAddr.addr,
                       pAddr->aucRemoteIp[3],
                       pAddr->aucRemoteIp[2],
                       pAddr->aucRemoteIp[1],
                       pAddr->aucRemoteIp[0] );
    pstPcb                              = ( struct tcp_pcb * )fd.pFd;
    pstPcb->local_port                  = pAddr->usLocalPort;

    lRtnVal                             = BST_IP_TcpCloneConn( pstPcb,
                                                              &stLwipAddr,
                                                               LWIP_NULL_PTR,
                                                               pAddr->usRemotePort );
    return lRtnVal;
}



BST_IP_ERR_T BST_IP_RawTcpSend( BST_FD_T fd, BST_UINT8* pData, BST_UINT16 usLength )
{
    BST_IP_ERR_T                        stErrMsg;
    struct tcp_pcb                     *pstPcb;

    if( !BST_IP_IsRawFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }
    if( BST_NULL_PTR == pData )
    {
        return BST_IP_ERR_MEM;
    }
    pstPcb                              = ( struct tcp_pcb * )fd.pFd;
    if( 0 == pstPcb->snd_wnd_max )
    {
        return BST_IP_ERR_WIN;
    }

    stErrMsg                            = tcp_write( pstPcb,
                                                     pData,
                                                     usLength,
                                                     TCP_WRITE_FLAG_COPY | TCP_WRITE_FLAG_MORE );
    if( BST_IP_ERR_OK != stErrMsg )
    {
        BST_RLS_LOG1("BST_IP_RawTcpSend tcp_write Error:", stErrMsg );
    }

    stErrMsg                            = tcp_output( pstPcb );
    if( BST_IP_ERR_OK != stErrMsg )
    {
        BST_RLS_LOG1("BST_IP_RawTcpSend tcp_output Error:", stErrMsg );
    }
    return stErrMsg;
}


BST_IP_ERR_T BST_IP_RawTcpBind( BST_FD_T fd, BST_UINT16 usPort )
{
    struct tcp_pcb                     *pstPcb;
    struct ip_addr                      stLwipAddr;

    if( !BST_IP_IsRawFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }
    pstPcb                              = ( struct tcp_pcb * )fd.pFd;
    stLwipAddr.addr                     = BST_IP_ADDRESS_ANY;
    return tcp_bind( pstPcb, &stLwipAddr, usPort );
}


BST_IP_ERR_T BST_IP_RawTcpClose( BST_FD_T fd, BST_ARG_T Arg )
{
    struct tcp_pcb                     *pstPcb;

    if( !BST_IP_IsRawFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }
    pstPcb                              = ( struct tcp_pcb * )fd.pFd;
    /****************************************************************************\
    | If the Socket is in TIME_WAIT or CLOSING, It means the Client has sent FIN |
    | and get the FIN_ACK successfully, We don't need to close it one more time. |
    \****************************************************************************/
    BST_DBG_LOG1( "Lwip BST_IP_RawTcpClose, state=%u", pstPcb->state );
    if ( ( TIME_WAIT == pstPcb->state )
       ||( CLOSING   == pstPcb->state ) )
    {
        return BST_IP_ERR_CONN;
    }

    /* When Pcb in SYN_SENT state, double close would cause serious memory leak!
     * Return BST_IP_ERR_CLR to notice upper layer not close again. */
    if ( SYN_SENT == pstPcb->state )
    {
        tcp_close( pstPcb );
        return BST_IP_ERR_CLR;
    }
    else
    {
        return tcp_close( pstPcb );
    }
}



BST_IP_ERR_T BST_IP_RawUdpSocket( BST_FD_T *pfd )
{
    struct udp_pcb                     *pstNewUdpPcb;

    BST_ASSERT_NULL_RTN( pfd, ERR_ARG );
    pstNewUdpPcb                        = udp_new();
    if( !BST_IP_IsLwipNull( pstNewUdpPcb ) )
    {
        udp_recv( pstNewUdpPcb,(udp_recv_fn)BST_IP_UdpRecvCB, LWIP_NULL_PTR );
        pfd->pFd                        = pstNewUdpPcb;
        return BST_IP_ERR_OK;
    }
    else
    {
        return BST_IP_ERR_MEM;
    }
}


BST_IP_ERR_T BST_IP_RawUdpConnect( BST_FD_T fd, BST_ARG_T Arg, BST_IP_SOCKET_ADD_T *pAddr )
{
    struct udp_pcb                     *upcb;
    struct ip_addr                      stLwipAddr;

    if( !BST_IP_IsRawFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }
    if( BST_NULL_PTR == pAddr )
    {
        return BST_IP_ERR_MEM;
    }
    BST_IP_GetU32Addr( stLwipAddr.addr,
                       pAddr->aucRemoteIp[3],
                       pAddr->aucRemoteIp[2],
                       pAddr->aucRemoteIp[1],
                       pAddr->aucRemoteIp[0] );

    upcb                                = (struct udp_pcb *)fd.pFd;
    return udp_connect( upcb, &stLwipAddr, pAddr->usRemotePort );
}


BST_IP_ERR_T BST_IP_RawUdpSend( BST_FD_T fd, BST_UINT8* pData, BST_UINT16 usLength )
{
    struct pbuf                        *pstPbuf;
    struct udp_pcb                     *upcb;

    if( BST_NULL_PTR == pData )
    {
        return BST_IP_ERR_MEM;
    }
    if( !BST_IP_IsRawFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }
    upcb                                = (struct udp_pcb *)fd.pFd;
    pstPbuf                             = pbuf_alloc( PBUF_RAW, usLength, PBUF_POOL );
    if( BST_IP_IsLwipNull(pstPbuf) )
    {
        return BST_IP_ERR_BUF;
    }
    BST_OS_MEMCPY( pstPbuf->payload, pData, usLength );
    return udp_send( upcb, pstPbuf );
}


BST_IP_ERR_T BST_IP_RawUdpBind( BST_FD_T fd, BST_UINT16 usPort )
{
    struct udp_pcb                     *upcb;
    struct ip_addr                      stLwipAddr;

    if( !BST_IP_IsRawFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }
    upcb                                = ( struct udp_pcb * )fd.pFd;
    stLwipAddr.addr                     = BST_IP_ADDRESS_ANY;
    return udp_bind( upcb, &stLwipAddr, usPort );
}


BST_IP_ERR_T BST_IP_RawUdpClose( BST_FD_T fd, BST_ARG_T Arg )
{
    struct udp_pcb                     *upcb;

    if( !BST_IP_IsRawFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }
    upcb                                = ( struct udp_pcb * )fd.pFd;
    udp_remove( upcb );
    return BST_IP_ERR_OK;
}


BST_IP_ERR_T BST_IP_RawSocket( BST_FD_T *pfd, BST_ARG_T *Arg, BST_UINT16 usProtocol )
{
    if( BST_NULL_PTR == pfd )
    {
        return BST_IP_ERR_ARG;
    }

    switch( usProtocol )
    {
        case BST_IP_PROTOCOL_UDP:
            return BST_IP_RawUdpSocket( pfd );
        case BST_IP_PROTOCOL_TCP:
            return BST_IP_RawTcpSocket( pfd );
        default :
            return BST_IP_ERR_ARG;
    }
}


BST_VOID BST_IP_CallBackEntry( BST_IP_EVENT_STRU *pstEvent )
{
    if( BST_NULL_PTR == pstEvent )
    {
        return;
    }
    switch ( pstEvent->enId )
    {
        case LWIP_EVENT_ACCEPT:
            break;

        case LWIP_EVENT_SENT:
            break;

        case LWIP_EVENT_RECV:
            BST_IP_TcpRecvCB( pstEvent->pvArg, pstEvent->pstPcb, pstEvent->pstBuf, pstEvent->enErrMsg );
            break;

        case LWIP_EVENT_CONNECTED:
            BST_IP_TcpConnectedCB( pstEvent->pvArg, pstEvent->pstPcb, pstEvent->enErrMsg );
            break;

        case LWIP_EVENT_OVERFLOW:
            if ( BST_NULL_PTR == pstEvent->pvArg )
            {
                break;
            }
            BST_IP_TcpOverFlowCB( *( (BST_UINT32 *)pstEvent->pvArg ) );
            break;

        case LWIP_EVENT_POLL:
            break;

        case LWIP_EVENT_ERR:
            BST_IP_TcpErrCB( pstEvent->pvArg, pstEvent->enErrMsg );
            break;

        default:
            break;
    }
}


BST_IP_ERR_T BST_IP_BsdSocket( BST_FD_T *pfd, BST_ARG_T *Arg, BST_UINT16 usProtocol )
{
    BST_UINT32                          ulTimeout;
    ulTimeout                           = BST_IP_RX_TIME_OUT;
    if( BST_NULL_PTR == pfd )
    {
        return BST_IP_ERR_ARG;
    }
    switch( usProtocol )
    {
        case BST_IP_PROTOCOL_UDP:
            pfd->lFd                    = lwip_socket( AF_INET, SOCK_DGRAM, 0 );
            break;
        case BST_IP_PROTOCOL_TCP:
            pfd->lFd                    = lwip_socket( AF_INET, SOCK_STREAM, 0 );    //TCP对应SOCK_STREAM
            break;
        default :
            pfd->lFd                    = BST_IP_ERR_MEM;
            return BST_IP_ERR_ARG;
    }

    if( BST_IP_IsBsdFdValid( (*pfd) ) )
    {
        if ( 0 == lwip_setsockopt( pfd->lFd, SOL_SOCKET, SO_RCVTIMEO, &ulTimeout, BST_OS_SIZEOF( ulTimeout ) ) )
        {
            return BST_IP_ERR_OK;
        }

        return BST_IP_ERR_VAL;
    }
    else
    {
        return BST_IP_ERR_MEM;
    }
}


BST_IP_ERR_T BST_IP_BsdConnect( BST_FD_T fd, BST_ARG_T Arg, BST_IP_SOCKET_ADD_T *pAdd )
{
    struct sockaddr_in                  stRemoteAddr;

    if( !BST_IP_IsBsdFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }
    if( BST_NULL_PTR == pAdd )
    {
        return BST_IP_ERR_MEM;
    }
    stRemoteAddr.sin_len                = BST_OS_SIZEOF(stRemoteAddr);
    stRemoteAddr.sin_family             = AF_INET;
    stRemoteAddr.sin_port               = htons( pAdd->usRemotePort );
    BST_IP_GetU32Addr( stRemoteAddr.sin_addr.s_addr,
                       pAdd->aucRemoteIp[3],
                       pAdd->aucRemoteIp[2],
                       pAdd->aucRemoteIp[1],
                       pAdd->aucRemoteIp[0] );

    return (BST_IP_ERR_T)lwip_connect( fd.lFd,
                                       (struct sockaddr*)&stRemoteAddr,
                                       BST_OS_SIZEOF(stRemoteAddr) );
}


BST_IP_ERR_T BST_IP_BsdTcpClone( BST_FD_T fd, BST_IP_SOCKET_ADD_T *pAddr )
{
    struct sockaddr_in                  stRemoteAddr;

    if( !BST_IP_IsBsdFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }
    if( BST_NULL_PTR == pAddr )
    {
        return BST_IP_ERR_MEM;
    }

    stRemoteAddr.sin_len                = BST_OS_SIZEOF(stRemoteAddr);
    stRemoteAddr.sin_family             = AF_INET;
    stRemoteAddr.sin_port               = pAddr->usRemotePort;
    BST_IP_GetU32Addr( stRemoteAddr.sin_addr.s_addr,
                       pAddr->aucRemoteIp[3],
                       pAddr->aucRemoteIp[2],
                       pAddr->aucRemoteIp[1],
                       pAddr->aucRemoteIp[0] );

    return (BST_IP_ERR_T)lwip_cloneconnect( fd.lFd,
                                            (struct sockaddr*)&stRemoteAddr,
                                            BST_OS_SIZEOF(stRemoteAddr) );
}


BST_IP_ERR_T BST_IP_BsdSend( BST_FD_T fd, BST_UINT8* pData, BST_UINT16 usLength )
{
    BST_INT32                           lRtnVal;
    if( !BST_IP_IsBsdFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }
    if( BST_NULL_PTR == pData )
    {
        return BST_IP_ERR_MEM;
    }

    lRtnVal                             = (BST_IP_ERR_T)lwip_write( fd.lFd, pData, usLength );
    if (lRtnVal < 0)
    {
        BST_RLS_LOG1( "BST_IP_BsdSend Err No. is %d", lRtnVal );
        return lRtnVal;
    }

    return BST_IP_ERR_OK;
}


BST_IP_ERR_T BST_IP_BsdReceive( BST_FD_T fd, BST_UINT8* pData, BST_UINT16 usLength )
{
    BST_IP_ERR_T                        lRtnVal;
    BST_IP_ERR_T                        lTmpVal;
    BST_UINT8                          *pucTmpData;
    BST_UINT16                          usDefLength;
    usDefLength                         = BST_IP_MTU_SIZE;

    if( !BST_IP_IsBsdFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }
    if( BST_NULL_PTR == pData || usLength <= 0 )
    {
        return BST_IP_ERR_MEM;
    }

    lRtnVal                             = (BST_IP_ERR_T)lwip_read( fd.lFd, pData, usLength );

    if ( lRtnVal < usLength )
    {
        return lRtnVal;
    }

    /* 读空socket缓冲区里的数据 */
    pucTmpData                          = (BST_UINT8 *)BST_OS_MALLOC( usDefLength );
    if ( BST_NULL_PTR == pucTmpData )
    {
        return lRtnVal;
    }
    BST_OS_MEMSET( pucTmpData, 0, usDefLength );

    while( 1 )
    {
        lTmpVal                         = (BST_IP_ERR_T)lwip_read( fd.lFd, pucTmpData, usDefLength );
        if ( lTmpVal < usDefLength )
        {
            break;
        }
    }
    BST_OS_FREE( pucTmpData );

    return lRtnVal;
}
BST_IP_ERR_T BST_IP_BsdBind( BST_FD_T fd, BST_UINT16 usPort )
{
    struct sockaddr_in                  stLocalAddr;

    if( !BST_IP_IsBsdFdValid(fd) )
    {
        return BST_IP_ERR_MEM;
    }

    stLocalAddr.sin_len                 = BST_OS_SIZEOF(stLocalAddr);
    stLocalAddr.sin_family              = AF_INET;
    stLocalAddr.sin_port                = PP_HTONS( usPort );
    stLocalAddr.sin_addr.s_addr         = BST_IP_ADDRESS_ANY;

    return (BST_IP_ERR_T)lwip_bind( fd.lFd, ( struct sockaddr *)&stLocalAddr, BST_OS_SIZEOF(stLocalAddr) );
}


BST_IP_ERR_T BST_IP_BsdClose( BST_FD_T fd, BST_ARG_T Arg )
{
    if( !BST_IP_IsBsdFdValid(fd) )
    {
        BST_RLS_LOG("BST_IP_BsdClose fd is invalid");
        return BST_IP_ERR_MEM;
    }
    return (BST_IP_ERR_T)lwip_close( fd.lFd );
}

/*================================================================================================*/
/*ssl 接口*/
#ifdef BST_SSL_SUPPORT

BST_IP_ERR_T BST_IP_SslSocket( BST_FD_T *pfd, BST_ARG_T *Arg, BST_UINT16 usProtocol )
{
    SSL                    *pstSsl;
    BST_INT32               lFd;
    BST_IP_ERR_T            lErrRtnVal;
    if ( BST_NULL_PTR == pfd )
    {
        return BST_IP_ERR_ARG;
    }

    lErrRtnVal               = BST_IP_BsdSocket( pfd, Arg, usProtocol );
    if ( BST_IP_ERR_OK != lErrRtnVal )
    {
        BST_RLS_LOG( "BST_IP_SslSocket BST_IP_BsdSocket error" );
        return BST_IP_ERR_ARG;
    }

    lFd                     = pfd->lFd;

    pstSsl                  = BST_IP_SslInit( Arg );

    if ( BST_NULL_PTR == pstSsl )
    {
        BST_RLS_LOG( "BST_IP_SslInit error" );
        return BST_IP_ERR_MEM;
    }

    if ( BST_FALSE == SSL_set_fd( pstSsl, lFd ) )
    {
        BST_RLS_LOG( "SSL_set_fd error" );
        return BST_IP_ERR_MEM;
    }
    pfd->pFd                = pstSsl;
    return BST_IP_ERR_OK;
}

BST_STATIC SSL *BST_IP_SslInit( BST_ARG_T *Arg )
{
    SSL_METHOD              *pstMethod;
    SSL_CTX                 *pstCtx;
    SSL                     *pstSsl;

    if ( BST_NULL_PTR == Arg )
    {
        return BST_NULL_PTR;
    }

    SSL_library_init();

    pstMethod               = ( SSL_METHOD * )TLSv1_client_method();

    if (BST_NULL_PTR == pstMethod)
    {
        BST_RLS_LOG( "BST_IP_SslInit TLSv1_client_method error" );
        return BST_NULL_PTR;
    }

    pstCtx                  = SSL_CTX_new( pstMethod );

    if ( BST_NULL_PTR == pstCtx)
    {
        BST_RLS_LOG( "BST_IP_SslInit SSL_CTX_new error" );
        return BST_NULL_PTR;
    }

    SSL_CTX_set_verify( pstCtx, SSL_VERIFY_NONE, BST_NULL_PTR );
    pstSsl                  = SSL_new( pstCtx );
    *Arg                    = pstCtx;
    return pstSsl;
}

BST_IP_ERR_T BST_IP_SslConnect( BST_FD_T fd, BST_ARG_T Arg, BST_IP_SOCKET_ADD_T *pAdd )
{
    SSL                    *pstSsl;
    BST_INT32               ret;
    BST_FD_T                lSocketFd;

    pstSsl                  = (SSL *)fd.pFd;
    lSocketFd.lFd           = SSL_get_fd(pstSsl);

    if (BST_IP_ERR_OK != BST_IP_BsdConnect(lSocketFd, Arg, pAdd) )
    {
        BST_RLS_LOG( "BST_IP_SslConnect BST_IP_BsdConnect error" );
        return BST_IP_ERR_MEM;
    }

    ret                     = SSL_connect( pstSsl );

    /* 返回值等于1表示connect成功 */
    if ( 1 == ret )
    {
        return BST_IP_ERR_OK;
    }
    ret                     = SSL_get_error( pstSsl, ret );

    BST_RLS_LOG1( "BST_IP_SslConnect Err No. is %d", ret );
    return  BST_IP_ERR_VAL;
}
BST_IP_ERR_T BST_IP_SslTcpClone( BST_FD_T fd, BST_IP_SOCKET_ADD_T *pAddr, BST_IP_SKT_PROPTY_STRU* pstProperty )
{

    return BST_IP_ERR_OK;
}

BST_IP_ERR_T BST_IP_SslSend( BST_FD_T fd, BST_UINT8* pData, BST_UINT16 usLength )
{
    SSL                                *pstSsl;
    BST_INT32                           ret;

    if ( BST_NULL_PTR == fd.pFd)
    {
        return BST_IP_ERR_ARG;
    }

    if ( BST_NULL_PTR == pData )
    {
        return BST_IP_ERR_ARG;
    }

    pstSsl                             = (SSL *)fd.pFd;
    ret                                = SSL_write( pstSsl, pData, usLength);
    ret                                = SSL_get_error( pstSsl, ret );

    if ( SSL_ERROR_NONE != ret)
    {
        BST_RLS_LOG1( "BST_IP_SslSend Err No. is %d", ret );
        return  BST_IP_ERR_VAL;
    }

    return BST_IP_ERR_OK;
}

BST_IP_ERR_T BST_IP_SslReceive( BST_FD_T fd, BST_UINT8* pData, BST_UINT16 usLength )
{
    SSL                *pstSsl;
    BST_UINT8          *pucTmpBuf;
    BST_INT32           lRtnVal;
    BST_INT32           lCopyedLen;
    BST_INT32           lSelect;
    BST_INT32           lSockFd;
    fd_set              stRdFds;
    timeval             stTval;
    stTval.tv_sec       = 0;
    stTval.tv_usec      = 0;
    if ( BST_NULL_PTR == fd.pFd )
    {
        return BST_IP_ERR_ARG;
    }
    if ( BST_NULL_PTR == pData )
    {
        return BST_IP_ERR_ARG;
    }

    lCopyedLen          = 0;
    pstSsl              = (SSL *)fd.pFd;
    lSockFd             = SSL_get_fd( pstSsl );

    pucTmpBuf           = (BST_UINT8 *)BST_OS_MALLOC( usLength );

    if ( BST_NULL_PTR == pucTmpBuf )
    {
        return BST_IP_ERR_MEM;
    }
    
    do {
        FD_ZERO( &stRdFds );
        FD_SET( lSockFd, &stRdFds );

        lRtnVal         = SSL_read( pstSsl, pucTmpBuf, usLength );

        
        if ( lRtnVal <= 0 )
        {
            BST_RLS_LOG( "BST_IP_SslReceive SSL_read error" );
            break;
        }
        if ( 0 == lCopyedLen )
        {
            BST_OS_MEMCPY( pData, pucTmpBuf, lRtnVal );
            lCopyedLen  = lRtnVal;
            BST_DBG_LOG1( "BST_IP_SslReceive lCopyedLen", lCopyedLen );
        }
        /*如果ssl内部缓冲区没有数据可读，则判断IP协议栈是否有数据可读*/
        if ( !SSL_pending( pstSsl ) )
        {
            lSelect     = lwip_select( lSockFd + 1, &stRdFds, NULL, NULL, &stTval );
            if ( lSelect < 0)
            {
                BST_RLS_LOG( "BST_IP_SslReceive lwip_select error" );
                break;
            }

            /*如果协议栈也没有数据可读，则说明这次读取完毕，退出*/
            if ( !FD_ISSET( lSockFd,&stRdFds ) )
            {
                BST_DBG_LOG( "BST_IP_SslReceive socket is not data" );
                break;
            }
        }
    } while( 1 );

    BST_OS_FREE( pucTmpBuf );
    return lCopyedLen;
}

BST_IP_ERR_T BST_IP_SslBind( BST_FD_T fd, BST_UINT16 usPort )
{
    SSL                                *pstSsl;
    BST_FD_T                           lSocketFd;

    if ( BST_NULL_PTR == fd.pFd )
    {
        BST_RLS_LOG("BST_IP_SslBind fd.pFd is NULL.");
        return BST_IP_ERR_ARG;
    }

    pstSsl                              = (SSL *)fd.pFd;
    lSocketFd.lFd                       = SSL_get_fd( pstSsl );

    return BST_IP_BsdBind( lSocketFd, usPort );
}

BST_IP_ERR_T BST_IP_SslClose( BST_FD_T fd, BST_ARG_T Arg )
{
    SSL                                *pstSsl;
    SSL_CTX                            *pstCtx;
    BST_FD_T                            lSocketFd;

    if ( BST_NULL_PTR == fd.pFd )
    {
        BST_RLS_LOG("BST_IP_SslClose fd.pFd is NULL");
        return BST_IP_ERR_ARG;
    }
    if ( BST_NULL_PTR == Arg )
    {
        BST_RLS_LOG("BST_IP_SslClose Arg is NULL");
        return BST_IP_ERR_ARG;
    }
    pstSsl                              = (SSL *)fd.pFd;
    pstCtx                              = (SSL_CTX *)Arg;
    /*获取协议栈中socket的fd*/
    lSocketFd.lFd                       = SSL_get_fd( pstSsl );
    SSL_shutdown( pstSsl );
    if ( BST_IP_ERR_OK !=BST_IP_BsdClose( lSocketFd, BST_NULL_PTR ) )
    {
        BST_RLS_LOG( "BST_IP_SslClose BST_IP_BsdClose is not OK" );
        return BST_IP_ERR_MEM;
    }
    SSL_set_session( pstSsl, BST_NULL_PTR );
    SSL_free(pstSsl);
    SSL_CTX_free(pstCtx);

    return BST_IP_ERR_OK;
}
#endif
