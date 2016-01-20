

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_IP_RcverMng.h"
#include "BST_DBG_MemLeak.h"
#include "lwip/tcp.h"
#include "BST_OS_Memory.h"
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_IP_RCVERMNG_CPP
/*lint +e767*/
/*****************************************************************************
  2 函数声明
*****************************************************************************/

/******************************************************************************
   4 私有定义
******************************************************************************/

/******************************************************************************
   5 全局变量定义
******************************************************************************/

/******************************************************************************
   6 函数实现
******************************************************************************/


BST_VOID  *BST_IP_CRcverMng::Attach( BST_FD_T *pstFd,  BST_IP_CNetRcver *pcRawRcver )
{
    BST_IP_RCVER_BINDER_STRU   *pstRcvBinder;

    if( BST_NULL_PTR == pcRawRcver )
    {
        return BST_NULL_PTR;
    }
    if( BST_NULL_PTR == pstFd )
    {
        return BST_NULL_PTR;
    }
    pstRcvBinder                = ( BST_IP_RCVER_BINDER_STRU *)
                                    BST_OS_MALLOC( BST_OS_SIZEOF(BST_IP_RCVER_BINDER_STRU) );
    if( BST_NULL_PTR == pstRcvBinder )
    {
        return BST_NULL_PTR;
    }
    pstRcvBinder->pstFd         = pstFd;
    pstRcvBinder->pcRcver       = pcRawRcver;
    lstAdd( &m_stRcverList, (NODE *)pstRcvBinder );
    return pstRcvBinder;
}


BST_VOID  BST_IP_CRcverMng::ClearFd ( BST_FD_T fd )
{
    BST_IP_RCVER_BINDER_STRU        *pstRcverIdx;

    if( !BST_IP_IsRawFdValid(fd) )
    {
        return;
    }
    for ( pstRcverIdx = ( BST_IP_RCVER_BINDER_STRU *)lstFirst( &m_stRcverList );
          pstRcverIdx!= BST_NULL_PTR;
          pstRcverIdx = ( BST_IP_RCVER_BINDER_STRU *)lstNext( (NODE *)pstRcverIdx ) )
    {
        if ( BST_NULL_PTR == pstRcverIdx->pcRcver)
        {
            continue;
        }
        if ( BST_NULL_PTR == pstRcverIdx->pstFd )
        {
            continue;
        }
        if ( pstRcverIdx->pstFd->pFd == fd.pFd )
        {
            pstRcverIdx->pstFd->pFd = BST_NULL_PTR;
            return;
        }
    }
    return;
}

BST_VOID BST_IP_CRcverMng::Detach( BST_FD_T *pstFd )
{
    BST_IP_RCVER_BINDER_STRU   *pstRcverIdx;
    BST_IP_RCVER_BINDER_STRU   *pstRcverNextIdx;

    if ( BST_NULL_PTR == pstFd )
    {
        return;
    }

    for ( pstRcverIdx = ( BST_IP_RCVER_BINDER_STRU *)lstFirst( &m_stRcverList );
          pstRcverIdx!= BST_NULL_PTR;
          pstRcverIdx = pstRcverNextIdx )
    {
        pstRcverNextIdx         = ( BST_IP_RCVER_BINDER_STRU *)
                                  lstNext( (NODE *)pstRcverIdx ) ;
        if ( pstRcverIdx->pstFd == pstFd )
        {
            lstDelete( &m_stRcverList, (NODE *)pstRcverIdx );
            BST_OS_FREE( pstRcverIdx );
            continue;
        }
    }
    return;
}

BST_IP_CRcverMng* BST_IP_CRcverMng::GetInstance( BST_VOID )
{
    static BST_IP_CRcverMng            *pcNetRcverMng = BST_NULL_PTR;

    if( BST_NULL_PTR == pcNetRcverMng )
    {
        pcNetRcverMng                   = new BST_IP_CRcverMng;
    }
    return pcNetRcverMng;
}

BST_IP_ERR_T BST_IP_CRcverMng::CnctedRcver( BST_FD_T fd, BST_IP_ERR_T InIpErrMsg )
{
    BST_IP_CNetRcver                   *pRcvItem;

    if ( !BST_IP_IsRawFdValid( fd ) )
    {
        return BST_IP_ERR_ARG;
    }
    pRcvItem                            = FindRcver( fd );
    if ( BST_IP_ERR_OK != InIpErrMsg )
    {
        pRcvItem->ErrClosed( fd, InIpErrMsg );
        return InIpErrMsg;
    }
    if ( BST_NULL_PTR == pRcvItem )
    {
        return BST_IP_ERR_OK;
    }
    return pRcvItem->Connectd( fd );
}

BST_VOID BST_IP_CRcverMng::ErrMsgRcver( BST_FD_T fd, BST_IP_ERR_T InIpErrMsg )
{
    BST_IP_CNetRcver                   *pRcvItem;

    pRcvItem                            = FindRcver( fd );
    if ( BST_NULL_PTR == pRcvItem )
    {
        return;
    }

    if ( BST_IP_IsConnErr( InIpErrMsg ) )
    {
        pRcvItem->ErrClosed( fd, InIpErrMsg );
        ClearFd( fd );
    }
    else
    {
        BST_RLS_LOG1( "BST_IP_CRcverMng::ErrMsgRcver Unexpected IpErrMsg: %d",
                      InIpErrMsg);
    }
    return;
}


BST_VOID  BST_IP_CRcverMng::ClosedRcver ( BST_FD_T fd )
{
    BST_IP_CNetRcver                   *pRcvItem;

    if( !BST_IP_IsRawFdValid( fd ) )
    {
        return;
    }

    pRcvItem                            = FindRcver( fd );
    if ( BST_NULL_PTR == pRcvItem )
    {
        return;
    }

    pRcvItem->Closed( fd );
    ClearFd( fd );
    return;
}


BST_IP_PKTPROC_MODE_ENUM BST_IP_CRcverMng::PacketRcver(
    BST_FD_T                fd,
    const BST_UINT8        *const pdata,
    const BST_UINT16        length )
{
    BST_IP_CNetRcver                   *pRcvItem;

    if( !BST_IP_IsRawFdValid( fd ) )
    {
        return BST_IP_PKT_REMOVE_PROC;
    }
    pRcvItem                            = FindRcver( fd );
    if ( BST_NULL_PTR == pRcvItem )
    {
        return BST_IP_PKT_REMOVE_PROC;
    }

    if ( BST_IP_IsLwipNull( pdata ) )
    {
        return BST_IP_PKT_REMOVE_PROC;
    }
    else
    {
        return pRcvItem->Received( fd, pdata, length );
    }
}

BST_IP_CRcverMng::BST_IP_CRcverMng( BST_VOID )
{
    lstInit( &m_stRcverList );
    return;
}

BST_IP_CRcverMng::~BST_IP_CRcverMng( BST_VOID )
{
    BST_RLS_LOG("BST_IP_CRcverMng: m_pstRcverTable Error");
}

BST_IP_CNetRcver* BST_IP_CRcverMng::FindRcver( BST_FD_T fd )
{
    BST_IP_RCVER_BINDER_STRU        *pstRcverIdx;

    if( !BST_IP_IsRawFdValid(fd) )
    {
        return BST_NULL_PTR;
    }
    for ( pstRcverIdx = ( BST_IP_RCVER_BINDER_STRU *)lstFirst( &m_stRcverList );
          pstRcverIdx!= BST_NULL_PTR;
          pstRcverIdx = ( BST_IP_RCVER_BINDER_STRU *)lstNext( (NODE *)pstRcverIdx ) )
    {
        if ( BST_NULL_PTR == pstRcverIdx->pcRcver)
        {
            continue;
        }
        if ( BST_NULL_PTR == pstRcverIdx->pstFd )
        {
            continue;
        }
        if ( pstRcverIdx->pstFd->pFd == fd.pFd )
        {
            return pstRcverIdx->pcRcver;
        }
    }
    return BST_NULL_PTR;
}



