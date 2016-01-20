

#ifndef __BST_IP_LWIP_API_H__
#define __BST_IP_LWIP_API_H__


#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack(4)
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(push, 4)
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "lwip/def.h"
#include "BST_Platform.h"
#include "BST_IP_Define.h"
#include "BST_IP_SocketClone.h"

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#ifdef BST_SSL_SUPPORT
#define BST_IP_SOCKET_FUNCTION_TABLE \
/*                  |       Socket      |        Connect      |       Clone      |       Send       |      Receive     |       Bind      |     Close       |*/ \
/* TYPE_BSD      */ {  BST_IP_BsdSocket, BST_IP_BsdConnect,    BST_IP_BsdTcpClone, BST_IP_BsdSend,    BST_IP_BsdReceive, BST_IP_BsdBind,    BST_IP_BsdClose}, \
/* TYPE_SSL      */ {  BST_IP_SslSocket, BST_IP_SslConnect,    BST_NULL_PTR,       BST_IP_SslSend,    BST_IP_SslReceive, BST_IP_SslBind,    BST_IP_SslClose}, \
/* RAW_TCP       */ {  BST_IP_RawSocket, BST_IP_RawTcpConnect, BST_IP_RawTcpClone, BST_IP_RawTcpSend, BST_NULL_PTR,      BST_IP_RawTcpBind, BST_IP_RawTcpClose}, \
/* RAW_UDP       */ {  BST_IP_RawSocket, BST_IP_RawUdpConnect, BST_NULL_PTR,       BST_IP_RawUdpSend, BST_NULL_PTR,      BST_IP_RawUdpBind, BST_IP_RawUdpClose}
#else
#define BST_IP_SOCKET_FUNCTION_TABLE \
/*                  |       Socket      |        Connect      |       Clone      |       Send       |      Receive     |       Bind      |     Close       |*/ \
/* TYPE_BSD      */ {  BST_IP_BsdSocket, BST_IP_BsdConnect,    BST_IP_BsdTcpClone, BST_IP_BsdSend,    BST_IP_BsdReceive, BST_IP_BsdBind,    BST_IP_BsdClose}, \
/* TYPE_SSL      */ {  BST_NULL_PTR,     BST_NULL_PTR,         BST_NULL_PTR,       BST_NULL_PTR,      BST_NULL_PTR,      BST_NULL_PTR,      BST_NULL_PTR }, \
/* RAW_TCP       */ {  BST_IP_RawSocket, BST_IP_RawTcpConnect, BST_IP_RawTcpClone, BST_IP_RawTcpSend, BST_NULL_PTR,      BST_IP_RawTcpBind, BST_IP_RawTcpClose}, \
/* RAW_UDP       */ {  BST_IP_RawSocket, BST_IP_RawUdpConnect, BST_NULL_PTR,       BST_IP_RawUdpSend, BST_NULL_PTR,      BST_IP_RawUdpBind, BST_IP_RawUdpClose}
#endif
#define BST_IP_SOCKET_SUPPORT_TABLE \
    BST_SCKT_TYPE_BSD,                /* 标准阻塞型BSD接口 */ \
    BST_SCKT_TYPE_SSL,                /* SSL接口*/\
    BST_SCKT_TYPE_RAW_TCP,            /* LWIP 所采用的RAW-TCP接口，支持实体Connect, 支持Seq查/设 */ \
    BST_SCKT_TYPE_RAW_UDP             /* LWIP 所采用的RAW-UDP接口，支持流量值查询*/ \

#define BST_IP_SocketSupportTblHead()   ( &g_BST_IP_SocketSupportTable[0] ) 
#define BST_IP_SocketFunTblHead()       ( &g_BST_IP_SocketFunTable[0] )
#define BST_IP_GetSocketFunGrp(s_type)  ( &g_BST_IP_SocketFunTable[s_type])

/* for Pc-lint error 801: goto */
#define BST_IP_TCP_RCVAPI_FREE()                            \
    q                                   = p;                \
    for(;;)                                                 \
    {                                                       \
        if( BST_IP_IsLwipNull(q) )                          \
        {                                                   \
            break;                                          \
        }                                                   \
        BST_IP_ApiUnRegistPacket( ( BST_IP_PKT_ID_T ) q );  \
        q                               = q->next;          \
    }                                                       \
    pbuf_free( p );                                         \
    BST_OS_ThreadUnLock( tThreadLockCnt );                  \
    return( ucErrMsg )

#define BST_IP_UDP_RCVAPI_FREE()                            \
    q                                   = p;                \
    for(;;)                                                 \
    {                                                       \
        if( BST_IP_IsLwipNull(q) )                          \
        {                                                   \
            break;                                          \
        }                                                   \
        BST_IP_ApiUnRegistPacket( ( BST_IP_PKT_ID_T ) q );  \
        q                               = q->next;          \
    }                                                       \
    pbuf_free( p );                                         \
    BST_OS_ThreadUnLock( tThreadLockCnt );                  \
    return

typedef BST_IP_ERR_T (*BST_IP_API_SOKT_T)( BST_FD_T                *pfd,
                                           BST_ARG_T               *Arg,
                                           BST_UINT16               usProtocol );
typedef BST_IP_ERR_T (*BST_IP_API_CONN_T)( BST_FD_T                 fd,
                                           BST_ARG_T                Arg,
                                           BST_IP_SOCKET_ADD_T     *pAdd );
typedef BST_IP_ERR_T (*BST_IP_API_CLON_T)( BST_FD_T                 fd,
                                           BST_IP_SOCKET_ADD_T     *pAdd );
typedef BST_IP_ERR_T (*BST_IP_API_SEND_T)( BST_FD_T                 fd,
                                           BST_UINT8               *pData,
                                           BST_UINT16               usLength );
typedef BST_IP_ERR_T (*BST_IP_API_RECV_T)( BST_FD_T                 fd,
                                           BST_UINT8               *pData,
                                           BST_UINT16               usLength );
typedef BST_IP_ERR_T (*BST_IP_API_BIND_T)( BST_FD_T                 fd,
                                           BST_UINT16               usPort );
typedef BST_IP_ERR_T (*BST_IP_API_CLSE_T)( BST_FD_T                 fd,
                                           BST_ARG_T                Arg );

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 枚举定义
*****************************************************************************/


/*****************************************************************************
  5 结构定义
*****************************************************************************/
typedef struct{
    BST_IP_API_SOKT_T                   pfSocket;
    BST_IP_API_CONN_T                   pfConnect;
    BST_IP_API_CLON_T                   pfClone;
    BST_IP_API_SEND_T                   pfSend;
    BST_IP_API_RECV_T                   pfReceive;
    BST_IP_API_BIND_T                   pfBind;
    BST_IP_API_CLSE_T                   pfClose;
}BST_IP_SOCKET_FUN_LUT_STRU;

/*****************************************************************************
  6 UNION定义
*****************************************************************************/


/*****************************************************************************
  7 全局变量声明
*****************************************************************************/
extern const BST_IP_SOCKET_FUN_LUT_STRU  g_BST_IP_SocketFunTable[ BST_SCKT_TYPE_NUMBER ];

/*****************************************************************************
  8 函数声明
*****************************************************************************/
BST_IP_ERR_T BST_IP_CheckSocketSupport( BST_SCKT_TYPE_ENUM_UINT32 stSocketType );

/* RAW API相关接口 */
BST_IP_ERR_T BST_IP_RawSocket       ( BST_FD_T                 *pfd,
                                      BST_ARG_T                *Arg,
                                      BST_UINT16                usProtocol );

BST_IP_ERR_T BST_IP_RawTcpSocket    ( BST_FD_T                 *pfd );
BST_IP_ERR_T BST_IP_RawTcpConnect   ( BST_FD_T                  fd,
                                      BST_ARG_T                 Arg,
                                      BST_IP_SOCKET_ADD_T      *pAdd );
BST_IP_ERR_T BST_IP_RawTcpClone     ( BST_FD_T                  fd,
                                      BST_IP_SOCKET_ADD_T      *pAdd );
BST_IP_ERR_T BST_IP_RawTcpSend      ( BST_FD_T                  fd,
                                      BST_UINT8                *pData,
                                      BST_UINT16                usLength );
BST_IP_ERR_T BST_IP_RawTcpBind      ( BST_FD_T                  fd,
                                      BST_UINT16                usPort );
BST_IP_ERR_T BST_IP_RawTcpClose     ( BST_FD_T                  fd,
                                      BST_ARG_T                 Arg );
BST_IP_ERR_T BST_IP_RawUdpSocket    ( BST_FD_T                 *pfd );
BST_IP_ERR_T BST_IP_RawUdpConnect   ( BST_FD_T                  fd,
                                      BST_ARG_T                 Arg,
                                      BST_IP_SOCKET_ADD_T      *pAdd );
BST_IP_ERR_T BST_IP_RawUdpSend      ( BST_FD_T                  fd,
                                      BST_UINT8                *pData,
                                      BST_UINT16                usLength );
BST_IP_ERR_T BST_IP_RawUdpBind      ( BST_FD_T                  fd,
                                      BST_UINT16                usPort );
BST_IP_ERR_T BST_IP_RawUdpClose     ( BST_FD_T                  fd,
                                      BST_ARG_T                 Arg );
BST_VOID BST_IP_CallBackEntry       ( BST_IP_EVENT_STRU        *pstEvent );

/* BSD标准阻塞型接口 */
BST_IP_ERR_T BST_IP_BsdSocket       ( BST_FD_T                 *pfd,
                                      BST_ARG_T                *Arg,
                                      BST_UINT16                usProtocol );
BST_IP_ERR_T BST_IP_BsdConnect      ( BST_FD_T                  fd,
                                      BST_ARG_T                 Arg,
                                      BST_IP_SOCKET_ADD_T      *pAdd );
BST_IP_ERR_T BST_IP_BsdTcpClone     ( BST_FD_T fd,
                                      BST_IP_SOCKET_ADD_T      *pAdd );
BST_IP_ERR_T BST_IP_BsdSend         ( BST_FD_T                  fd,
                                      BST_UINT8                *pData,
                                      BST_UINT16                usLength );
BST_IP_ERR_T BST_IP_BsdReceive      ( BST_FD_T                  fd,
                                      BST_UINT8                *pData,
                                      BST_UINT16                usLength );
BST_IP_ERR_T BST_IP_BsdBind         ( BST_FD_T                  fd,
                                      BST_UINT16                usPort );
BST_IP_ERR_T BST_IP_BsdClose        ( BST_FD_T                  fd,
                                      BST_ARG_T                 Arg );
#ifdef BST_SSL_SUPPORT
/*SSL接口*/
BST_IP_ERR_T BST_IP_SslSocket       ( BST_FD_T                 *pfd,
                                      BST_ARG_T                *Arg,
                                      BST_UINT16                usProtocol );
BST_IP_ERR_T BST_IP_SslConnect      ( BST_FD_T                  fd,
                                      BST_ARG_T                 Arg,
                                      BST_IP_SOCKET_ADD_T      *pAdd );
BST_IP_ERR_T BST_IP_SslTcpClone     ( BST_FD_T                  fd,
                                      BST_IP_SOCKET_ADD_T      *pAddr,
                                      BST_IP_SKT_PROPTY_STRU   *pstProperty );
BST_IP_ERR_T BST_IP_SslSend         ( BST_FD_T                  fd,
                                      BST_UINT8                *pData,
                                      BST_UINT16                usLength );
BST_IP_ERR_T BST_IP_SslReceive      ( BST_FD_T                  fd,
                                      BST_UINT8                *pData,
                                      BST_UINT16                usLength );
BST_IP_ERR_T BST_IP_SslBind         ( BST_FD_T                  fd,
                                      BST_UINT16                usPort );
BST_IP_ERR_T BST_IP_SslClose        ( BST_FD_T                  fd,
                                      BST_ARG_T                 Arg );
#endif
/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack()
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(pop)
#endif


#endif
