

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_SRV_Event.h"
#include "BST_OS_Memory.h"
#include "BST_DBG_MemLeak.h"
#include "BST_SRV_AsEvnt.h"
#include "BST_SRV_AsCtrl.h"
#include "BST_DSPP_TrsLayer.h"
#include "BST_IP_LwipApi.h"
#include "BST_APP_MainTask.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_SRV_EVENT_CPP
/*lint +e767*/

/******************************************************************************
   2 宏定义
******************************************************************************/

/*****************************************************************************
  3 函数声明
*****************************************************************************/
extern "C" BST_VOID     BST_SRV_InitMainThread( BST_VOID );

extern "C" BST_IP_ERR_T BST_SRV_RcvTcpIpEvent(
    BST_IP_EVENT_STRU      *pstEvent );

extern "C" BST_UINT32   BST_SRV_RcvAcomData(
    BST_UINT32              ulLength,
    BST_UINT8              *pucData );

extern "C" BST_UINT32   BST_SRV_RcvAsEvent(
    BST_AS_EVT_ENUM_UINT32  enAsEvent,
    BST_UINT32              ulLength,
    BST_VOID               *pvData );

/******************************************************************************
   4 私有定义
******************************************************************************/

/******************************************************************************
   5 全局变量定义
******************************************************************************/

/******************************************************************************
   6 函数实现
******************************************************************************/

/*****************************************************************************
 函 数 名  : BST_SRV_CEvent
 功能描述  : BST_SRV_CEvent类的构造函数
 输入参数  : usInProcID ;ID of protocol adapter
           : usInTaskID ;Task ID
 输出参数  :
 返 回 值  : BST_VOID
 被调函数  :
 调用函数  :
 修改历史  :
    1.日    期   : 2014/01/01
      作    者   : Davy
      修改内容   : 创建函数
*****************************************************************************/
BST_SRV_CEvent::BST_SRV_CEvent( BST_VOID )
{
    m_pstEventMbx                       = BST_NULL_PTR;
    BST_OS_CreateThread ( ( BST_INT8 *)"Bastet EventProc Thread",
                          ( BST_OS_THREAD_T)BST_SRV_CEvent::RcvThread,
                          &m_pstEventMbx,
                          BST_OS_STACK_SIZE_L,
                          BST_OS_PRIO_LOW );

}

/*****************************************************************************
 函 数 名  : ~BST_SRV_CEvent
 功能描述  : Destructor function of class BST_SRV_CEvent
 输入参数  : BST_VOID
 输出参数  :
 返 回 值  : BST_VOID
 被调函数  :
 调用函数  :
 修改历史  :
    1.日    期   : 2014/01/01
      作    者   : Davy
      修改内容   : 创建函数
*****************************************************************************/
BST_SRV_CEvent::~BST_SRV_CEvent( BST_VOID )
{
    m_pstEventMbx                       = BST_NULL_PTR;
}

BST_SRV_CEvent *BST_SRV_CEvent::GetInstance( BST_VOID )
{
    static BST_SRV_CEvent *pcSrvEvent = BST_NULL_PTR;

    if ( BST_NULL_PTR == pcSrvEvent )
    {
        pcSrvEvent = new BST_SRV_CEvent();
    }
    return pcSrvEvent;
}



BST_VOID BST_SRV_CEvent::RcvThread( BST_VOID *pvArg )
{
    BST_OS_MBX_T                      **pstMailBox;
    BST_EVT_HEAD_STRU                  *pstEvtMsg;
    BST_AS_EVT_STRU                    *pstAsEvt;
    BST_ACOM_EVT_STRU                  *pstAcomEvt;
    BST_TIMER_EVT_STRU                 *pstTimerEvt;
    BST_NET_EVENT_STRU                 *pstNetEvt;
    BST_DSPP_CTrsDlVa                  *pcTrslayer;
    BST_SRV_CAsEvt                     *pcAsMng;
    BST_OS_TIMERID_T                    ulTimerId;
    BST_UINT32                          ulRtnVal;

    BST_ASSERT_NULL( pvArg );
    pstMailBox                          = (BST_OS_MBX_T **)pvArg;
   *pstMailBox                          = BST_OS_CreateMbx( BST_OS_GetCurrentThreadId(), BST_M_MBOX_SIZE );
    BST_ASSERT_NULL( *pstMailBox );
    BST_SetThreadInitFlag( BST_MAIN_THREAD_FLAG );

    for (;;)
    {
        ulRtnVal                        = BST_OS_RecvMail( *pstMailBox, (BST_VOID **)&pstEvtMsg, BST_OS_SYS_FOREVER );

        if( BST_OS_SYS_TIMEOUT == ulRtnVal )
        {
            continue;
        }
        if( BST_NULL_PTR == pstEvtMsg )
        {
            continue;
        }
        BST_DBG_LOG1( "BST_SRV_CEvent::RcvThread pstEvtMsg->enId=%d",
                      pstEvtMsg->enId );
        switch( pstEvtMsg->enId )
        {
        case BST_SRV_ACOM_EVENT:
            pcTrslayer                  = BST_DSPP_CTrsDlVa::GetInstance();
            if ( BST_NULL_PTR == pcTrslayer )
            {
                break;
            }
            pstAcomEvt                  = (BST_ACOM_EVT_STRU *)pstEvtMsg;
            pcTrslayer->RxApi( pstAcomEvt->pucData, (BST_UINT16)pstAcomEvt->ulLength );
            break;

        case BST_SRV_AS_EVENT:
            pcAsMng                     = BST_SRV_CAsEvt::GetInstance();
            if ( BST_NULL_PTR == pcAsMng )
            {
                break;
            }
            pstAsEvt                    = (BST_AS_EVT_STRU *)pstEvtMsg;
            pcAsMng->AsEventRcv( pstAsEvt->enAsEvent, pstAsEvt->ulLength,(BST_VOID *)pstAsEvt->pucData );
            break;

        case BST_SRV_TIMER_EVENT:
            pstTimerEvt                 = (BST_TIMER_EVT_STRU *)pstEvtMsg;
            if( BST_OS_SIZEOF(ulTimerId) != pstTimerEvt->ulLength )
            {
                break;
            }
            BST_OS_MEMCPY( &ulTimerId, pstTimerEvt->pucData, BST_OS_SIZEOF(ulTimerId) );
            BST_OS_TimerDoCallBack( ulTimerId );
            break;

        case BST_SRV_TCPIP_EVENT:
            pstNetEvt                   = (BST_NET_EVENT_STRU *)pstEvtMsg;
            if( BST_OS_SIZEOF(BST_IP_EVENT_STRU) != pstNetEvt->ulLength )
            {
                break;
            }
            BST_IP_CallBackEntry ( ( BST_IP_EVENT_STRU * )pstNetEvt->pucData );
            break;

        default:
            break;
        }

        if( BST_NULL_PTR != pstEvtMsg->pucData )
        {
            BST_OS_FREE( pstEvtMsg->pucData );
        }
        BST_OS_FREE( pstEvtMsg );

#if ( BST_VER_TYPE == BST_UT_VER )
        break;
#endif

    }
}


/*lint -e438 -e429*/
BST_ERR_ENUM_UINT8 BST_SRV_CEvent::RcvTcpIpEvent ( BST_IP_EVENT_STRU *pstEvent )
{
    BST_NET_EVENT_STRU                 *pstNetEvent;
    BST_IP_EVENT_STRU                  *pstLwipEvent;

    if ( BST_NULL_PTR == m_pstEventMbx )
    {
        return BST_ERR_INVALID_PTR;
    }

    if ( BST_NULL_PTR == pstEvent )
    {
        return BST_ERR_INVALID_PTR;
    }

    pstLwipEvent                        = ( BST_IP_EVENT_STRU *)BST_OS_MALLOC
                                          ( BST_OS_SIZEOF ( BST_IP_EVENT_STRU ) );
    BST_ASSERT_NULL_RTN( pstLwipEvent, BST_ERR_NO_MEMORY );

    pstNetEvent                         = ( BST_NET_EVENT_STRU *)BST_OS_MALLOC
                                          ( BST_OS_SIZEOF ( BST_NET_EVENT_STRU ) );
    if ( BST_NULL_PTR == pstNetEvent )
    {
        BST_OS_FREE( pstLwipEvent );
        return BST_ERR_INVALID_PTR;
    }

    BST_OS_MEMCPY ( pstLwipEvent, pstEvent, BST_OS_SIZEOF ( BST_IP_EVENT_STRU ) );

    pstNetEvent->enId                  = BST_SRV_TCPIP_EVENT;
    pstNetEvent->pucData               = ( BST_UINT8 * )pstLwipEvent;
    pstNetEvent->ulLength              = BST_OS_SIZEOF ( BST_IP_EVENT_STRU );

    return BST_OS_SendMail( m_pstEventMbx, pstNetEvent );
}
/*lint +e438 +e429*/

/*lint -e438 -e429*/
BST_ERR_ENUM_UINT8 BST_SRV_CEvent::RcvAcomData( BST_UINT32 ulLength, BST_UINT8 *pucData )
{
    BST_ACOM_EVT_STRU                  *pstAcpuEvent;

    if( BST_NULL_PTR == m_pstEventMbx )
    {
        return BST_ERR_INVALID_PTR;
    }
    if( BST_NULL_PTR == pucData )
    {
        return BST_ERR_INVALID_PTR;
    }
    if( 0 == ulLength )
    {
        return BST_ERR_PAR_LEN;
    }
    pstAcpuEvent                        = (BST_ACOM_EVT_STRU *)BST_OS_MALLOC( BST_OS_SIZEOF(BST_ACOM_EVT_STRU) );
    BST_ASSERT_NULL_RTN( pstAcpuEvent, BST_ERR_NO_MEMORY );

    pstAcpuEvent->pucData               = (BST_UINT8 *)BST_OS_MALLOC( ulLength );
    if ( BST_NULL_PTR == pstAcpuEvent->pucData )
    {
        BST_OS_FREE( pstAcpuEvent );
        return BST_ERR_NO_MEMORY;
    }

    pstAcpuEvent->enId                  = BST_SRV_ACOM_EVENT;
    pstAcpuEvent->ulLength              = ulLength;
    BST_OS_MEMCPY( pstAcpuEvent->pucData, pucData, ulLength );

    return BST_OS_SendMail( m_pstEventMbx, pstAcpuEvent );
}
/*lint +e438 +e429*/

/*lint -e438 -e429*/
BST_UINT32  BST_SRV_CEvent::RcvTimerExpired ( BST_OS_TIMERID_T ulTimerId )
{
    BST_TIMER_EVT_STRU                 *pstSendTimerEvent;

    if( BST_NULL_PTR == m_pstEventMbx )
    {
        return BST_ERR_INVALID_PTR;
    }

    pstSendTimerEvent                   = ( BST_TIMER_EVT_STRU *)BST_OS_MALLOC
                                          ( BST_OS_SIZEOF(BST_TIMER_EVT_STRU) );
    BST_ASSERT_NULL_RTN( pstSendTimerEvent, BST_ERR_NO_MEMORY );

    pstSendTimerEvent->pucData          = ( BST_UINT8 *)BST_OS_MALLOC
                                          ( BST_OS_SIZEOF(ulTimerId) );
    if ( BST_NULL_PTR == pstSendTimerEvent->pucData )
    {
        BST_OS_FREE( pstSendTimerEvent );
        return BST_ERR_NO_MEMORY;
    }

    pstSendTimerEvent->ulLength         = BST_OS_SIZEOF( ulTimerId );
    BST_OS_MEMCPY( pstSendTimerEvent->pucData, &ulTimerId, pstSendTimerEvent->ulLength );

    pstSendTimerEvent->enId             = BST_SRV_TIMER_EVENT;

    return BST_OS_SendMail( m_pstEventMbx, pstSendTimerEvent );
}
/*lint +e438 +e429*/

/*lint -e438 -e429*/
BST_ERR_ENUM_UINT8 BST_SRV_CEvent::RcvAsEvent(
    BST_AS_EVT_ENUM_UINT32  enAsEvent,
    BST_UINT32              ulLength,
    BST_VOID               *pvData )
{
    BST_AS_EVT_STRU                    *pstSendAsEvent;

    if( BST_NULL_PTR == m_pstEventMbx )
    {
        return BST_ERR_INVALID_PTR;
    }
    if( !BST_SRV_IsValidAsEvent( enAsEvent ) )
    {
        return BST_ERR_INVALID_PTR;
    }
    pstSendAsEvent                      = (BST_AS_EVT_STRU *)BST_OS_MALLOC( BST_OS_SIZEOF(BST_AS_EVT_STRU) );
    BST_ASSERT_NULL_RTN( pstSendAsEvent, BST_ERR_NO_MEMORY );

    if( ( BST_NULL_PTR == pvData ) || ( 0 == ulLength ) )
    {
        pstSendAsEvent->pucData         = BST_NULL_PTR;
        pstSendAsEvent->ulLength        = 0;
    }
    else
    {
        pstSendAsEvent->pucData         = (BST_UINT8 *)BST_OS_MALLOC( ulLength );
        if ( BST_NULL_PTR == pstSendAsEvent->pucData )
        {
            BST_RLS_LOG( "BST_SRV_CEvent::RcvAsEvent No Memory" );
            BST_OS_FREE( pstSendAsEvent );
            return BST_ERR_NO_MEMORY;
        }
        pstSendAsEvent->ulLength        = ulLength;
        BST_OS_MEMCPY( pstSendAsEvent->pucData, pvData, ulLength );
    }

    pstSendAsEvent->enId                = BST_SRV_AS_EVENT;
    pstSendAsEvent->enAsEvent           = enAsEvent;

    return BST_OS_SendMail( m_pstEventMbx, pstSendAsEvent );
}
/*lint +e438 +e429*/

BST_UINT32 BST_SRV_RcvAcomData( BST_UINT32 ulLength, BST_UINT8 *pucData )
{
    BST_SRV_CEvent                       *pEvtProc;

    pEvtProc                            = BST_SRV_CEvent::GetInstance();
    BST_ASSERT_NULL_RTN( pEvtProc, 0 );

    pEvtProc->RcvAcomData( ulLength, pucData );
    return ulLength;
}

BST_UINT32 BST_SRV_RcvAsEvent(
    BST_AS_EVT_ENUM_UINT32  enAsEvent,
    BST_UINT32              ulLength,
    BST_VOID               *pvData )
{
    BST_SRV_CEvent                     *pEvtProc;
    BST_ERR_ENUM_UINT8                  ucRtnVal;

    pEvtProc                            = BST_SRV_CEvent::GetInstance();
    BST_ASSERT_NULL_RTN( pEvtProc, 0 );

    ucRtnVal                            = pEvtProc->RcvAsEvent( enAsEvent, ulLength, pvData );
    if( BST_NO_ERROR_MSG == ucRtnVal )
    {
        return ulLength;
    }
    BST_RLS_LOG1( "BST_SRV_RcvAsEvent pEvtProc->RcvAsEvent Err=%u",
                  ucRtnVal );
    return 0;
}


BST_UINT32 BST_SRV_RcvTimerEvent( BST_OS_TIMERID_T ulTimerId )
{
    BST_SRV_CEvent                     *pEvtProc;
    BST_UINT32                          ulRtnVal;

    pEvtProc                            = BST_SRV_CEvent::GetInstance();
    BST_ASSERT_NULL_RTN( pEvtProc, 0 );

    ulRtnVal                            = pEvtProc->RcvTimerExpired( ulTimerId );
    if( BST_NO_ERROR_MSG == ulRtnVal )
    {
        return BST_OS_SIZEOF(BST_OS_TIMER_STRU);
    }
    BST_RLS_LOG1( "BST_SRV_RcvTimerEvent pEvtProc->RcvTimerExpired Err=%u",
                  ulRtnVal );
    return 0;
}


BST_IP_ERR_T BST_SRV_RcvTcpIpEvent( BST_IP_EVENT_STRU *pstEvent )
{
    BST_SRV_CEvent                     *pEvtProc;
    BST_ERR_ENUM_UINT8                  ucRtnVal;

    pEvtProc                            = BST_SRV_CEvent::GetInstance();
    BST_ASSERT_NULL_RTN( pEvtProc, BST_IP_ERR_ARG );

    ucRtnVal                            = pEvtProc->RcvTcpIpEvent( pstEvent );
    switch ( ucRtnVal )
    {
        case BST_ERR_NO_MEMORY:
            return BST_IP_ERR_MEM;

        case BST_ERR_INVALID_PTR:
            return BST_IP_ERR_VAL;

        case BST_NO_ERROR_MSG:
            return BST_IP_ERR_OK;

        default:
            return BST_IP_ERR_ARG;
    }
}


BST_VOID BST_SRV_InitMainThread( BST_VOID )
{
    BST_SRV_CEvent::GetInstance();
    BST_SRV_CAsEvt::GetInstance();
    BST_SRV_CAsCtrl::GetInstance();
    BST_APP_CMainTask::GetInstance();
}

