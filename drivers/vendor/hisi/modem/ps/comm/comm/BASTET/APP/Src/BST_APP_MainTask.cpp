
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_APP_MainTask.h"
#include "BST_CORE_Schd.h"
#include "BST_APP_Define.h"
#include "BST_OS_Memory.h"
#include "BST_DBG_MemLeak.h"
#include "BST_SRV_TaskMng.h"
#include "BST_DRV_Net.h"
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_APP_MAINTASK_CPP
/*lint +e767*/
/******************************************************************************
   2 宏定义
******************************************************************************/

/*****************************************************************************
  3 函数声明
*****************************************************************************/
extern "C" BST_ERR_ENUM_UINT8 SysGetLocalIpInfo( NET_DRV_LOCAL_IP_INFO_STRU *pstLocalIpInfo );
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
 函 数 名  : BST_APP_CMainTask
 功能描述  : BST_APP_CMainTask类的构造函数
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
BST_APP_CMainTask::BST_APP_CMainTask( BST_PROCID_T usInProcID, BST_TASKID_T usInTaskID )
    : BST_CTask( usInProcID, usInTaskID )
{
    BST_ERR_ENUM_UINT8                  enRetVal;
    BST_CORE_CRegedit                  *pcRegedit;
    BST_OS_LOCKCNT_T                    tThreadLockCnt;

    pcRegedit                           = BST_CORE_CRegedit::GetInstance();
    BST_ASSERT_NULL( pcRegedit );
    tThreadLockCnt                      = BST_OS_ThreadLock ();

    lstInit( &m_stNpTaskList );
    enRetVal                            = pcRegedit->Regist( this,
                                                             this,
                                                             BST_PID_NET_STATE,
                                                             BST_OS_SIZEOF( BST_NET_STATE_STRU ),
                                                             BST_NULL_PTR );

    enRetVal                            = pcRegedit->Regist( this,
                                                             this,
                                                             BST_PID_TIMESTAMP,
                                                             BST_OS_SIZEOF( BST_DRV_NET_TIME_CLBRT_STRU ),
                                                             BST_NULL_PTR );

    enRetVal                            = pcRegedit->Regist( this,
                                                             this,
                                                             BST_PID_TRAFFIC_LIMITE,
                                                             BST_OS_SIZEOF( BST_UINT32 ),
                                                             BST_NULL_PTR );

    enRetVal                            = pcRegedit->Regist( this,
                                                             this,
                                                             BST_PID_TRAFFIC_FLOW,
                                                             0,
                                                             BST_NULL_PTR );/*注册EMAIL流量查询*/

    enRetVal                            = pcRegedit->Regist( this,
                                                             this,
                                                             BST_PID_MODEM_RAB_ID,
                                                             BST_OS_SIZEOF( BST_DRV_NET_MODEM_RABID_STRU ),
                                                             BST_NULL_PTR );/*注册MODEM_ID,RAB_ID消息*/
    ( BST_VOID )pcRegedit->Update( m_usProcId, m_usTaskId,
                                   BST_PID_TRAFFIC_FLOW, ( BST_UINT16 )BST_APP_TASK_TRFCFLOW_COUNT_SIZE, BST_NULL_PTR );
    if ( BST_NO_ERROR_MSG != enRetVal )
    {
        BST_DBG_LOG1( "Regist BST_PID_NET_STATE Fail! errcode:%d", enRetVal );
    }
    RegAsNotice( BST_AS_EVT_L3_RAT_STATE );

    BST_OS_ThreadUnLock ( tThreadLockCnt );
}

/*****************************************************************************
 函 数 名  : ~BST_APP_CMainTask
 功能描述  : Destructor function of class BST_APP_CMainTask
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
BST_APP_CMainTask::~BST_APP_CMainTask( BST_VOID )
{
#if ( BST_OS_VER != BST_QCOM_ROS )
    try
    {
#endif
        BST_CORE_CRegedit                  *pcRegedit;
        pcRegedit                           = BST_CORE_CRegedit::GetInstance();
        BST_ASSERT_NULL( pcRegedit );
        pcRegedit->unRegist( this );
#if ( BST_OS_VER != BST_QCOM_ROS )
    }
    catch(...)
    {
        BST_RLS_LOG("BST_APP_CMainTask destructor exception!");
    }
#endif
}

/*lint -e429*/
BST_VOID BST_APP_CMainTask::Attach ( BST_CORE_CNPTask  *pC_NPTask )
{
    BST_UINT32                  ulNewLen;
    BST_CORE_CRegedit          *pcRegedit;
    BST_APP_NPTASK_NODE_STRU   *pstNpTaskNode;
    pcRegedit                   = BST_CORE_CRegedit::GetInstance();

    BST_ASSERT_NULL( pcRegedit );
    BST_ASSERT_NULL( pC_NPTask );

    pstNpTaskNode               = ( BST_APP_NPTASK_NODE_STRU *)BST_OS_MALLOC
                                  ( BST_OS_SIZEOF( BST_APP_NPTASK_NODE_STRU ) );
    BST_ASSERT_NULL( pstNpTaskNode );
    pstNpTaskNode->pcNpTask     = pC_NPTask;
    lstAdd( &m_stNpTaskList, (NODE *)pstNpTaskNode );

    ulNewLen                    = BST_APP_TASK_TRFCFLOW_COUNT_SIZE
                                + ( BST_UINT16 )lstCount( &m_stNpTaskList )
                                * ( BST_UINT16 )BST_OS_SIZEOF( BST_APP_TASK_TRFCFLOW_STRU );

    pcRegedit->Update( m_usProcId, m_usTaskId, BST_PID_TRAFFIC_FLOW,
                     ( BST_UINT16 )ulNewLen, BST_NULL_PTR );
}
/*lint +e429*/

/*lint -e438*/
BST_VOID BST_APP_CMainTask::Detach ( BST_CORE_CNPTask  *pC_NPTask )
{
    BST_UINT32                  ulNewLen;
    BST_CORE_CRegedit          *pcRegedit;
    BST_APP_NPTASK_NODE_STRU   *pstNpTaskIdx;

    pcRegedit                   = BST_CORE_CRegedit::GetInstance();
    BST_ASSERT_NULL( pcRegedit );
    BST_ASSERT_NULL( pC_NPTask );

    for ( pstNpTaskIdx = ( BST_APP_NPTASK_NODE_STRU *)lstFirst( &m_stNpTaskList );
          pstNpTaskIdx!= BST_NULL_PTR;
          pstNpTaskIdx = ( BST_APP_NPTASK_NODE_STRU *)lstNext( (NODE *)pstNpTaskIdx ) )
    {
        if ( pstNpTaskIdx->pcNpTask == pC_NPTask )
        {
            lstDelete( &m_stNpTaskList, (NODE *)pstNpTaskIdx );
            BST_OS_FREE( pstNpTaskIdx );
            break;
        }
    }

    ulNewLen                        = BST_APP_TASK_TRFCFLOW_COUNT_SIZE
                                    + ( BST_UINT16 )lstCount( &m_stNpTaskList )
                                    * ( BST_UINT16 )BST_OS_SIZEOF( BST_APP_TASK_TRFCFLOW_STRU );

    pcRegedit->Update( m_usProcId, m_usTaskId, BST_PID_TRAFFIC_FLOW,
                     ( BST_UINT16 )ulNewLen, BST_NULL_PTR );
}
/*lint +e438*/

/*lint -e438*/
BST_ERR_ENUM_UINT8 BST_APP_CMainTask::TrafficOverFlow( BST_UINT32 ulRealFlowValue )
{
    BST_INT32           lTotalLen;
    BST_ERR_ENUM_UINT8  enRtnVal;
    BST_DSPP_CReport   *pcReporter;
    BST_UINT8          *pucBuffer;

    BST_RLS_LOG1( "MainTask::TrafficOverFlow, RealFlow=%u", ulRealFlowValue );

    enRtnVal            = BST_NO_ERROR_MSG;
    lTotalLen           = ( BST_INT32 )lstCount( &m_stNpTaskList );
    if ( lTotalLen <= 0 )
    {
        return BST_ERR_ITEM_NOT_EXISTED;
    }
    lTotalLen          *= ( BST_UINT16 )BST_OS_SIZEOF
                          ( BST_APP_TASK_TRFCFLOW_STRU );
    lTotalLen          += ( BST_INT32 )BST_APP_TASK_TRFCFLOW_COUNT_SIZE;

    pucBuffer           = ( BST_UINT8 *)BST_OS_MALLOC( (BST_UINT32)lTotalLen );
    if ( BST_NULL_PTR == pucBuffer )
    {
        return BST_ERR_NO_MEMORY;
    }

    lTotalLen           = GetTotalTrFlow( pucBuffer, (BST_UINT16)lTotalLen );
    if ( 0 == lTotalLen )
    {
        BST_OS_FREE( pucBuffer );
        return BST_ERR_ITEM_NOT_EXISTED;
    }

    pcReporter          = new BST_DSPP_CReport( m_usProcId, m_usTaskId );

    pcReporter->Report( BST_PID_TRAFFIC_FLOW, ( BST_UINT16 )lTotalLen, pucBuffer );
    delete pcReporter;
    BST_OS_FREE( pucBuffer );
    return enRtnVal;
}
/*lint +e438*/
/*****************************************************************************
 函 数 名  : ~BST_APP_CMainTask
 功能描述  : Destructor function of class BST_APP_CMainTask
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
BST_VOID BST_APP_CMainTask::Entry( BST_VOID )
{
}

/*****************************************************************************
 函 数 名  : ConfigNotice
 功能描述  : Config notice function of sys main proc
 输入参数  : enInPid   ;input task pid
               : usNewLen  ;config data length
               : pcNewVal  ;config data buffer
 输出参数  :
 返 回 值  : BST_UINT8
 被调函数  :
 调用函数  :
 修改历史  :
    1.日    期   : 2014/01/01
      作    者   : Davy
      修改内容   : 创建函数
*****************************************************************************/
BST_UINT16  BST_APP_CMainTask::Inquired( BST_CORE_PID_ENUM_UINT16   enParamId,
                                         BST_UINT16                 usDataSize,
                                         BST_VOID                  *const pData )
{
    BST_UINT16                          usRtnLen;

    BST_ASSERT_NORM_RTN( !BST_CORE_IsPidValid ( enParamId ), BST_CORE_INVALID_INQUIRED_LEN );
    BST_ASSERT_NULL_RTN( pData, BST_CORE_INVALID_INQUIRED_LEN );
    BST_ASSERT_0_RTN   ( usDataSize, BST_CORE_INVALID_INQUIRED_LEN );

    usRtnLen                            = 0;
    switch ( enParamId )
    {
        case BST_PID_TRAFFIC_FLOW:
            usRtnLen                    = GetTotalTrFlow( ( BST_UINT8 *)pData, usDataSize );
            break;

        default:
            usRtnLen                    = 0;
            break;
    }
    return usRtnLen;
}

/*****************************************************************************
 函 数 名  : ConfigNotice
 功能描述  : Config notice function of sys main proc
 输入参数  : enInPid   ;input task pid
               : usNewLen  ;config data length
               : pcNewVal  ;config data buffer
 输出参数  :
 返 回 值  : BST_UINT8
 被调函数  :
 调用函数  :
 修改历史  :
    1.日    期   : 2014/01/01
      作    者   : Davy
      修改内容   : 创建函数
*****************************************************************************/
BST_ERR_ENUM_UINT8 BST_APP_CMainTask::Configed( BST_CORE_PID_ENUM_UINT16 enInPid,
                                                BST_UINT16               usNewLen,
                                                const BST_VOID *const    pcNewVal,
                                                BST_UINT8              **pucNewAddr )
{
    BST_DRV_STRU                       *pNetDrvHandle;
    BST_ERR_ENUM_UINT8                  ucRtnVal;

    if ( !BST_CORE_IsPidValid ( enInPid ) )
    {
        return BST_ERR_PAR_UNKNOW;
    }

    if ( BST_NULL_PTR == pcNewVal )
    {
        return BST_ERR_INVALID_PTR;
    }

    if ( BST_NULL_PTR == pucNewAddr )
    {
        return BST_ERR_INVALID_PTR;
    }

    if ( 0 == usNewLen ) 
    {
        return BST_ERR_PAR_LEN;
    }

    ucRtnVal                            = BST_ERR_PAR_UNKNOW;
   *pucNewAddr                          = BST_NULL_PTR;
    pNetDrvHandle                       = BST_DRV_NetGetDvcHandle();
    BST_DBG_LOG1( "BST_APP_CMainTask::Configed enInPid=%u",
                  enInPid );
    switch ( enInPid )
    {
        case BST_PID_NET_STATE:
            if( usNewLen != BST_OS_SIZEOF( BST_NET_STATE_STRU ) )
            {
                return BST_ERR_PAR_LEN;
            }
            ucRtnVal                    = ConfigNetState( (BST_NET_STATE_STRU *)pcNewVal );
            break;

        case BST_PID_TIMESTAMP:
            if( usNewLen != BST_OS_SIZEOF( BST_DRV_NET_TIME_CLBRT_STRU ) )
            {
                return BST_ERR_PAR_LEN;
            }
            ucRtnVal                    = pNetDrvHandle->pfIoCtrl( BST_DRV_CMD_SET_NET_TIMESTAMP, (BST_VOID *)pcNewVal );
            break;

        case BST_PID_TRAFFIC_LIMITE:
            if( usNewLen != BST_OS_SIZEOF( BST_UINT32 ) )
            {
                return BST_ERR_PAR_LEN;
            }
            ucRtnVal                = pNetDrvHandle->pfIoCtrl( BST_DRV_CMD_SET_TRFC_FLOW_LMT, (BST_VOID *)pcNewVal );
            break;

        case BST_PID_MODEM_RAB_ID:
            if ( usNewLen != BST_OS_SIZEOF( BST_DRV_NET_MODEM_RABID_STRU ) )
            {
                return BST_ERR_PAR_LEN;
            }
            ucRtnVal                = pNetDrvHandle->pfIoCtrl( BST_DRV_CMD_SET_MODEM_RABID, (BST_VOID *)pcNewVal );
            break;

        default:
            break;
    }

    return ucRtnVal;
}



BST_APP_CMainTask *BST_APP_CMainTask::GetInstance( BST_VOID )
{
    static BST_APP_CMainTask *pSys      = BST_NULL_PTR;

    if ( BST_NULL_PTR == pSys )
    {
        pSys                            = new BST_APP_CMainTask( BST_APP_TYPE_SYSTASK, BST_APP_MAIN_TASK_ID );
    }

    return pSys;
}


BST_VOID  BST_APP_CMainTask::Start( BST_VOID )
{
    BST_SRV_CTaskMng                   *pcTaskManager;
    pcTaskManager                       = BST_SRV_CTaskMng::GetInstance();
    BST_ASSERT_NULL( pcTaskManager );
    pcTaskManager->StartAll();
}


BST_VOID  BST_APP_CMainTask::Stop(BST_VOID)
{
    BST_SRV_CTaskMng                   *pcTaskManager;
    pcTaskManager                       = BST_SRV_CTaskMng::GetInstance();
    BST_ASSERT_NULL( pcTaskManager );
    pcTaskManager->StopAll();
}


BST_VOID  BST_APP_CMainTask::Remove( BST_VOID )
{
    BST_SRV_CTaskMng                   *pcTaskManager;
    pcTaskManager                       = BST_SRV_CTaskMng::GetInstance();
    BST_ASSERT_NULL( pcTaskManager );
    pcTaskManager->RemoveAll();
}

/*****************************************************************************
 函 数 名  : RadioEvent
 功能描述  : radio event which notify all the task what event happened
 输入参数  : enEvent  ;radio event
           : pData    ;event args
 输出参数  : BST_VOID
 返 回 值  : BST_VOID
 被调函数  :
 调用函数  :
 修改历史  :
    1.日    期   : 2014/01/01
      作    者   : Davy
      修改内容   : 创建函数
*****************************************************************************/
BST_VOID BST_APP_CMainTask::AsEventCallback( BST_AS_EVT_ENUM_UINT32 ulAsEvt, BST_UINT32 ulLength, BST_VOID *pData )
{
    BST_UINT32                          ulPara;
    BST_DSPP_CReport                   *pcReporter;    
    BST_UINT32                          ulReportErrVal;
    BST_DRV_STRU                       *pNetDriver;
    BST_DRV_NET_PKT_MODE_ENUM_UINT32    enPktMode;
    BST_ERR_ENUM_UINT8                  ucRtnVal;

    enPktMode                           = BST_DRV_PKT_MODE_IP;
    pNetDriver                          = BST_DRV_NetGetDvcHandle();

    switch ( ulAsEvt )
    {
        case BST_AS_EVT_L3_RAT_STATE:
            {
                if ( ulLength != BST_OS_SIZEOF(ulPara) )
                {
                    break;
                }
                BST_OS_MEMCPY( &ulPara, pData, ulLength );
                if ( BST_AS_IsRatSupport( ulPara ) )
                {
                    BST_DBG_LOG1( "Bastet Support Rat=%d", ulPara );
                    ulReportErrVal     = BST_SYS_SUPPORT_CFG;
                }
                else
                {
                    BST_DBG_LOG1( "Bastet Unsupport Rat=%d", ulPara );
                    ulReportErrVal     = BST_SYS_SUPPORT_NONE;
                    Remove();
                }

#if( BST_IP_SUPPORT_PPP == BST_FEATURE_ON )
                if ( BST_AS_IsRatPppMode( ulPara ) )
                {
                    enPktMode           = BST_DRV_PKT_MODE_PPP;
                }
                else
#endif
                {
                    enPktMode           = BST_DRV_PKT_MODE_IP;
                }

                ucRtnVal                = pNetDriver->pfIoCtrl( BST_DRV_CMD_NET_PKT_MODE,
                                                                &enPktMode );
                if ( BST_NO_ERROR_MSG != ucRtnVal )
                {
                    BST_RLS_LOG2("Config Packet Mode=%d Error=%d", enPktMode, ucRtnVal );
                }

                pcReporter              = new BST_DSPP_CReport( m_usProcId, m_usTaskId );

                pcReporter->Report( BST_PID_FEATURE_SUPPORT,
                                    BST_OS_SIZEOF( BST_UINT32 ),
                                    (BST_VOID *)&ulReportErrVal );
                delete pcReporter;
            }
            break;

        default:
            break;
    }
}

/*****************************************************************************
 函 数 名  : ConfigLocalIp
 功能描述  : Config LocalIP of the LwIP Stack
 输入参数  : pcNewVal  ;4 bytes buffer(IP addr)
 输出参数  :
 返 回 值  : BST_ERR_ENUM_UINT8 
 被调函数  :
 调用函数  :
 修改历史  :
    1.日    期   : 2014/01/01
      作    者   : Davy
      修改内容   : 创建函数
*****************************************************************************/
BST_ERR_ENUM_UINT8  BST_APP_CMainTask::SaveLocalIp( NET_DRV_LOCAL_IP_INFO_STRU *pstLocalMsg )
{
    if ( BST_NULL_PTR == pstLocalMsg )
    {
        return BST_ERR_PAR_UNKNOW;
    }
    BST_OS_MEMCPY( &m_LocalIpInfo, pstLocalMsg, BST_OS_SIZEOF( NET_DRV_LOCAL_IP_INFO_STRU ) );
    return BST_NO_ERROR_MSG;
}


BST_ERR_ENUM_UINT8 BST_APP_CMainTask::ConfigNetState( BST_NET_STATE_STRU *pstNetState )
{
    BST_DRV_STRU                       *pNetDriver;
    BST_CTaskSchdler                   *pcSchdler;
    NET_DRV_LOCAL_IP_INFO_STRU          stLocalInfo;
    BST_ERR_ENUM_UINT8                  ucRtnVal;

    pcSchdler                           = BST_CTaskSchdler::GetInstance();
    pNetDriver                          = BST_DRV_NetGetDvcHandle();
    ucRtnVal                            = BST_NO_ERROR_MSG;
    BST_ASSERT_NULL_RTN( pcSchdler, BST_ERR_INVALID_PTR );

    if( BST_DRV_NETSTATE_UP == pstNetState->enState )
    {
        stLocalInfo.ulGateWay               = htonl( pstNetState->stLocal.ulGateWay );
        stLocalInfo.ulIp                    = htonl( pstNetState->stLocal.ulIp );
        stLocalInfo.ulMask                  = htonl( pstNetState->stLocal.ulMask );

        SaveLocalIp( &stLocalInfo );
        BST_DBG_LOG( "BST_APP_CMainTask::ConfigNetState BST_DRV_NETSTATE_UP!");
        if( !BST_DRV_IsDeviceInited( pNetDriver ) )
        {
            BST_DBG_LOG( "First Init LwIP!" );
            if( BST_NULL_PTR != pNetDriver->pfInit )
            {
                ucRtnVal                = pNetDriver->pfInit( &stLocalInfo );
            }
        }
        else
        {
            if( BST_NULL_PTR != pNetDriver->pfIoCtrl )
            {
                ucRtnVal                = pNetDriver->pfIoCtrl( BST_DRV_CMD_LOCAL_ADDR,
                                                                &stLocalInfo );
            }
        }
        return ucRtnVal;
    }
    else if( BST_DRV_NETSTATE_UNKOWN == pstNetState->enState )
    {
        BST_RLS_LOG( "BST_APP_CMainTask::ConfigNetState BST_DRV_CMD_NET_OTHER!" );
        if( BST_NULL_PTR != pNetDriver->pfIoCtrl )
        {
            ucRtnVal                    = pNetDriver->pfIoCtrl( BST_DRV_CMD_NET_UNKOWN,
                                                                BST_NULL_PTR );
        }

        pcSchdler->Suspend();
        return ucRtnVal;
    }
    else if( BST_DRV_NETSTATE_DOWN == pstNetState->enState )
    {
        stLocalInfo.ulGateWay               = htonl( pstNetState->stLocal.ulGateWay );
        stLocalInfo.ulIp                    = htonl( pstNetState->stLocal.ulIp );
        stLocalInfo.ulMask                  = htonl( pstNetState->stLocal.ulMask );

        SaveLocalIp( &stLocalInfo );
        BST_RLS_LOG( "BST_APP_CMainTask::ConfigNetState BST_DRV_NETSTATE_DOWN!" );
        if( BST_NULL_PTR != pNetDriver->pfIoCtrl )
        {
            ucRtnVal                    = pNetDriver->pfIoCtrl( BST_DRV_CMD_NET_DOWN,
                                                                BST_NULL_PTR );
        }
        pcSchdler->Suspend();
        return ucRtnVal;
    }
    else
    {
        BST_DBG_LOG( "Wrong Bastet State!" );
        return BST_ERR_ILLEGAL_PARAM;
    }
}

/*****************************************************************************
 函 数 名  : GetTotalTrFlow
 功能描述  : Get the Total Traffice Flow of Tasks.
 输入参数  : pucBuf[]       data buffer for save the BST_APP_TASK_TRFCFLOW_STRUs
             usBufLen       Maxim Length of the buffer
 输出参数  : BST_VOID
 返 回 值  : BST_UINT16     The real used buffer length
 被调函数  :
 调用函数  :
 修改历史  :
    1.日    期   : 2014/10/22
      作    者   : Davy
      修改内容   : 创建函数
*****************************************************************************/
/*lint -e429*/
BST_UINT16 BST_APP_CMainTask::GetTotalTrFlow( BST_UINT8 pucBuf[], BST_UINT16 usBufLen )
{
    BST_IP_ERR_T                cIpErr;
    BST_APP_NPTASK_NODE_STRU   *pstNpTaskIdx;
    BST_APP_NPTASK_NODE_STRU   *pstNpTaskNextIdx;
    BST_APP_TASK_TRFCFLOW_STRU  stTaskTrfFlow;
    BST_UINT16                  usCopyCnt;
    BST_UINT16                  usUnitSize;
    BST_UINT16                  usTrfcFlowCnt;

    BST_ASSERT_NULL_RTN ( pucBuf, 0 );
    usCopyCnt                   = 0;
    usUnitSize                  = ( BST_UINT16 )BST_OS_SIZEOF( BST_APP_TASK_TRFCFLOW_STRU );
    usTrfcFlowCnt               = 0;
    usCopyCnt                  += BST_APP_TASK_TRFCFLOW_COUNT_SIZE;

    for ( pstNpTaskIdx= ( BST_APP_NPTASK_NODE_STRU *)lstFirst( &m_stNpTaskList ); 
          pstNpTaskIdx != BST_NULL_PTR ; 
          pstNpTaskIdx  = pstNpTaskNextIdx )
    {
        pstNpTaskNextIdx        = ( BST_APP_NPTASK_NODE_STRU *)lstNext
                                  ( (NODE *) pstNpTaskIdx );

        if ( ( usBufLen - usCopyCnt ) < usUnitSize )
        {
            BST_RLS_LOG1( "BST_APP_CMainTask::GetTotalTrFlow usCopyCnt=%u",
                          usCopyCnt );
            BST_OS_MEMCPY( pucBuf, &usTrfcFlowCnt, BST_APP_TASK_TRFCFLOW_COUNT_SIZE );
            return usCopyCnt;
        }
        stTaskTrfFlow.strFlowInfo.ulRxByte = 0;
        stTaskTrfFlow.strFlowInfo.ulTxByte = 0;
        cIpErr                  = pstNpTaskIdx->pcNpTask->m_pSocket->IoCtrl
                                  ( BST_SCKT_CMD_GET_TRAFFIC_FLOW,
                                    &stTaskTrfFlow.strFlowInfo );
        if ( BST_IP_ERR_OK != cIpErr )
        {
            continue;
        }

        if( ( 0 == stTaskTrfFlow.strFlowInfo.ulRxByte )
         && ( 0 == stTaskTrfFlow.strFlowInfo.ulTxByte ) )
        {
            continue;
        }
        stTaskTrfFlow.usProcId  = pstNpTaskIdx->pcNpTask->m_usProcId;
        stTaskTrfFlow.usTaskId  = pstNpTaskIdx->pcNpTask->m_usTaskId;
        BST_OS_MEMCPY( ( pucBuf + usCopyCnt ), &stTaskTrfFlow, usUnitSize );
        usCopyCnt              += usUnitSize;
        usTrfcFlowCnt ++;
    }
    BST_OS_MEMCPY( pucBuf, &usTrfcFlowCnt, BST_APP_TASK_TRFCFLOW_COUNT_SIZE );
    return usCopyCnt;
}

