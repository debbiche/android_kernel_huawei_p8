

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_CORE_Regedit.h"
#include "BST_DSPP_CtrLayer.h"
#include "BST_APP_HeartBeat.h"
#include "BST_APP_EmailBox.h"
#include "BST_SRV_TaskMng.h"
#include "BST_DBG_MemLeak.h"
#include "BST_APP_MainTask.h"

/*lint -e767*/
#define    THIS_FILE_ID     PS_FILE_ID_BST_SRV_TASKMNG_CPP
/*lint +e767*/

/******************************************************************************
   2 函数实现
******************************************************************************/



BST_SRV_CTaskMng *BST_SRV_CTaskMng::GetInstance( BST_VOID )
{
    static BST_SRV_CTaskMng            *pcTaskManager = BST_NULL_PTR;

    if( BST_NULL_PTR == pcTaskManager )
    {
        pcTaskManager                   = new BST_SRV_CTaskMng;
    }
    return pcTaskManager;
}

/*lint -e429*/
BST_SRV_CTaskMng::BST_SRV_CTaskMng( BST_VOID )
{
    BST_UINT32              ulCnt;
    BST_OS_LOCKCNT_T        tThreadLockCnt;
    BST_SRV_TASK_NODE_STRU *pstTaskNode;
    tThreadLockCnt          = BST_OS_ThreadLock();
    lstInit( &m_stTaskList );
    BST_OS_InitSpinLock( &m_SpinLock );
    for( ulCnt = 0; ulCnt<BST_OBJ_TYPE_MAX; ulCnt++ )
    {
        m_apcTaskIdGnrtor[ ulCnt ]  = new BST_Lib_CSnGenrator<BST_DSPP_OFSTADDR_T>
                                      ( BST_DSPP_OFFSET_ADDR_MIN,
                                        BST_DSPP_OFFSET_ADDR_MAX );
    }
    pstTaskNode             = ( BST_SRV_TASK_NODE_STRU *)BST_OS_MALLOC
                              ( BST_OS_SIZEOF ( BST_SRV_TASK_NODE_STRU ) );
    if( BST_NULL_PTR != pstTaskNode )
    {
        pstTaskNode->pcTask = BST_APP_CMainTask::GetInstance();
        lstAdd( &m_stTaskList, ( NODE *)pstTaskNode );
    }
    m_pcRegedit             = BST_CORE_CRegedit::GetInstance();
    BST_OS_ThreadUnLock( tThreadLockCnt );
}
/*lint +e429*/

/*lint -e429*/
BST_DSPP_OFSTADDR_T BST_SRV_CTaskMng::Add( BST_DSPP_OBJTADDR_T usObjAddr )
{
    BST_DSPP_OFSTADDR_T                 usOfstAddr;
    BST_SRV_TASK_NODE_STRU             *pstTaskNode;
    BST_UINT32                          ulSpinlockCnt;

    if( !BST_DSPP_IsObjIdValid(usObjAddr) )
    {
        return BST_INVALID_SNID;
    }
    if ( BST_NULL_PTR == m_pcRegedit )
    {
        return BST_INVALID_SNID;
    }
    /*
     *Main Task is also a Task, But it don't use app-resource.
     */
    if ( (BST_UINT32)m_stTaskList.count >= ( BST_MAX_APP_NUMBER + 1 ) )
    {
        return BST_INVALID_SNID;
    }
    ulSpinlockCnt                       = BST_OS_SpinLock( &m_SpinLock );
    usOfstAddr                          = m_apcTaskIdGnrtor[usObjAddr]->NewId();

    if( BST_INVALID_SNID == usOfstAddr )
    {
        BST_RLS_LOG( "BST_SRV_CTaskMng::Add usOfstAddr=NULL" );
        BST_OS_SpinUnLock( &m_SpinLock , ulSpinlockCnt);
        return BST_INVALID_SNID;
    }
    pstTaskNode             = ( BST_SRV_TASK_NODE_STRU *)BST_OS_MALLOC
                              ( BST_OS_SIZEOF ( BST_SRV_TASK_NODE_STRU ) );
    BST_ASSERT_NULL_RTN( pstTaskNode, BST_INVALID_SNID );

    pstTaskNode->pcTask     = Create( usObjAddr, usOfstAddr );

    if ( BST_NULL_PTR == pstTaskNode->pcTask )
    {
        m_apcTaskIdGnrtor[usObjAddr]->RmvId( usOfstAddr );
        BST_OS_SpinUnLock( &m_SpinLock , ulSpinlockCnt);
        return BST_INVALID_SNID;
    }
    else
    {
        if( BST_APP_TYPE_SYSTASK != usObjAddr )
        {
            lstAdd( &m_stTaskList, ( NODE *)pstTaskNode );
        }
        BST_OS_SpinUnLock( &m_SpinLock , ulSpinlockCnt);
        return usOfstAddr;
    }
}
/*lint +e429*/

/*lint -e438*/
BST_ERR_ENUM_UINT8  BST_SRV_CTaskMng::Remove(
    BST_DSPP_OBJTADDR_T usObjAddr,
    BST_DSPP_OFSTADDR_T usOfstAddr )
{
    BST_SRV_TASK_NODE_STRU *pstTaskIdx;
    BST_SRV_TASK_NODE_STRU *pstTaskNextIdx;
    BST_OS_LOCKCNT_T        tThreadLockCnt;
    BST_ERR_ENUM_UINT8      ucRtnVal;

    if( !BST_DSPP_IsObjIdValid(usObjAddr) )
    {
        return BST_ERR_UNKNOW_OBJ;
    }
    if( BST_INVALID_SNID == usOfstAddr )
    {
        return BST_ERR_UNKNOW_OBJ;
    }
    if( BST_APP_TYPE_SYSTASK == usObjAddr )
    {
        return BST_ERR_ILLEGAL_PARAM;
    }

    ucRtnVal            = BST_ERR_UNKNOW_OBJ;
    tThreadLockCnt      = BST_OS_ThreadLock();

    for ( pstTaskIdx = ( BST_SRV_TASK_NODE_STRU *)lstFirst( &m_stTaskList);
          BST_NULL_PTR != pstTaskIdx;
          pstTaskIdx = pstTaskNextIdx )
    {
        pstTaskNextIdx  = ( BST_SRV_TASK_NODE_STRU *)
                          lstNext( (NODE *) pstTaskIdx );
        if ( ( ( pstTaskIdx->pcTask->m_usTaskId == usOfstAddr )
            || ( BST_DSPP_BROADCASE_ADDRESS == usOfstAddr ) )
            && ( pstTaskIdx->pcTask->m_usProcId == usObjAddr ) )
        {
            if ( BST_NULL_PTR != pstTaskIdx->pcTask )
            {
                pstTaskIdx->pcTask->Stop();
                delete pstTaskIdx->pcTask;
            }
            lstDelete( &m_stTaskList,(NODE *)pstTaskIdx );
            BST_OS_FREE( pstTaskIdx );
            BST_DBG_LOG2( "BST_SRV_CTaskMng::Remove usObjAddr, usOfstAddr",
                           usObjAddr, usOfstAddr );
            ucRtnVal                    = BST_NO_ERROR_MSG;
            m_apcTaskIdGnrtor[usObjAddr]->RmvId( usOfstAddr );
            break;//匹配成功后删除然后跳出，不用在往下遍历
        }
    }
    BST_OS_ThreadUnLock( tThreadLockCnt );
    return ucRtnVal;
}
/*lint +e438*/

BST_ERR_ENUM_UINT8  BST_SRV_CTaskMng::RemoveAll( BST_VOID )
{
    BST_SRV_TASK_NODE_STRU *pstTaskIdx;
    BST_SRV_TASK_NODE_STRU *pstTaskNextIdx;
    BST_UINT32              ulCnt;
    BST_OS_LOCKCNT_T        tThreadLockCnt;
    BST_ERR_ENUM_UINT8      ucRtnVal;

    ucRtnVal            = BST_NO_ERROR_MSG;
    tThreadLockCnt      = BST_OS_ThreadLock();
    for ( pstTaskIdx = ( BST_SRV_TASK_NODE_STRU *)lstFirst( &m_stTaskList);
          BST_NULL_PTR != pstTaskIdx;
          pstTaskIdx = pstTaskNextIdx )
    {
        pstTaskNextIdx  = ( BST_SRV_TASK_NODE_STRU *)
                          lstNext( (NODE *) pstTaskIdx );

        if ( BST_NULL_PTR != pstTaskIdx->pcTask )
        {
            if( BST_APP_TYPE_SYSTASK
             == pstTaskIdx->pcTask->m_usProcId )
            {
                continue;
            }
            pstTaskIdx->pcTask->Stop();
            delete( pstTaskIdx->pcTask );
        }
        lstDelete( &m_stTaskList,(NODE *)pstTaskIdx );
        BST_OS_FREE( pstTaskIdx );
    }
    for( ulCnt = 0; ulCnt<BST_OBJ_TYPE_MAX; ulCnt++ )
    {
        m_apcTaskIdGnrtor[ ulCnt ]->RmvId();
    }
    BST_DBG_LOG( "BST_SRV_CTaskMng::RemoveAll" );
    BST_OS_ThreadUnLock( tThreadLockCnt );
    return ucRtnVal;
}

BST_ERR_ENUM_UINT8  BST_SRV_CTaskMng::Start(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr )
{
    BST_SRV_TASK_NODE_STRU *pstTaskIdx;
    BST_SRV_TASK_NODE_STRU *pstTaskNextIdx;
    BST_UINT32              ulSpinlockCnt;
    BST_ERR_ENUM_UINT8      ucRtnVal;

    if( !BST_DSPP_IsObjIdValid(usObjAddr) )
    {
        return BST_ERR_UNKNOW_OBJ;
    }

    if( BST_INVALID_SNID == usOfstAddr )
    {
        return BST_ERR_UNKNOW_OBJ;
    }
    ucRtnVal            = BST_ERR_UNKNOW_OBJ;
    ulSpinlockCnt       = BST_OS_SpinLock( &m_SpinLock );
    for ( pstTaskIdx = ( BST_SRV_TASK_NODE_STRU *)lstFirst( &m_stTaskList);
          BST_NULL_PTR != pstTaskIdx;
          pstTaskIdx = pstTaskNextIdx )
    {
        pstTaskNextIdx  = ( BST_SRV_TASK_NODE_STRU *)
                          lstNext( (NODE *) pstTaskIdx );
        if ( BST_NULL_PTR == pstTaskIdx->pcTask )
        {
            continue;
        }
        if ( ( ( pstTaskIdx->pcTask->m_usTaskId == usOfstAddr )
              || ( BST_DSPP_BROADCASE_ADDRESS   == usOfstAddr ) )
            && ( pstTaskIdx->pcTask->m_usProcId == usObjAddr ) )
        {
            BST_DBG_LOG2( "BST_SRV_CTaskMng: ProcId=%d, TaskId=%d is Started!",usObjAddr, usOfstAddr );
            pstTaskIdx->pcTask->Start();
            ucRtnVal                    = BST_NO_ERROR_MSG;
            break;//匹配成功后跳出，不用在往下遍历
        }
    }
    BST_OS_SpinUnLock( &m_SpinLock , ulSpinlockCnt);
    return ucRtnVal;
}

BST_ERR_ENUM_UINT8  BST_SRV_CTaskMng::StartAll( BST_VOID )
{
    BST_SRV_TASK_NODE_STRU *pstTaskIdx;
    BST_SRV_TASK_NODE_STRU *pstTaskNextIdx;
    BST_UINT32              ulSpinlockCnt;

    ulSpinlockCnt                       = BST_OS_SpinLock( &m_SpinLock );
    for ( pstTaskIdx = ( BST_SRV_TASK_NODE_STRU *)lstFirst( &m_stTaskList);
          BST_NULL_PTR != pstTaskIdx;
          pstTaskIdx = pstTaskNextIdx )
    {
        pstTaskNextIdx  = ( BST_SRV_TASK_NODE_STRU *)
                          lstNext( (NODE *) pstTaskIdx );
        if ( BST_NULL_PTR != pstTaskIdx->pcTask )
        {
            BST_DBG_LOG2( "BST_SRV_CTaskMng: ProcId=%d, TaskId=%d is Started!",
                          pstTaskIdx->pcTask->m_usProcId,
                          pstTaskIdx->pcTask->m_usTaskId );
            pstTaskIdx->pcTask->Start();
        }
    }
    BST_OS_SpinUnLock( &m_SpinLock , ulSpinlockCnt);
    return BST_NO_ERROR_MSG;
}

BST_ERR_ENUM_UINT8  BST_SRV_CTaskMng::StopAll( BST_VOID )
{
    BST_SRV_TASK_NODE_STRU *pstTaskIdx;
    BST_SRV_TASK_NODE_STRU *pstTaskNextIdx;
    BST_UINT32              ulSpinlockCnt;

    ulSpinlockCnt                       = BST_OS_SpinLock( &m_SpinLock );
    for ( pstTaskIdx = ( BST_SRV_TASK_NODE_STRU *)lstFirst( &m_stTaskList);
          BST_NULL_PTR != pstTaskIdx;
          pstTaskIdx = pstTaskNextIdx )
    {
        pstTaskNextIdx  = ( BST_SRV_TASK_NODE_STRU *)
                          lstNext( (NODE *) pstTaskIdx );
        if ( BST_NULL_PTR != pstTaskIdx->pcTask )
        {
            BST_DBG_LOG2( "BST_SRV_CTaskMng: ProcId=%d, TaskId=%d is Stoped!",
                          pstTaskIdx->pcTask->m_usProcId,
                          pstTaskIdx->pcTask->m_usTaskId );
            pstTaskIdx->pcTask->Stop();
        }
    }
    BST_OS_SpinUnLock( &m_SpinLock , ulSpinlockCnt);
    return BST_NO_ERROR_MSG;}

BST_ERR_ENUM_UINT8  BST_SRV_CTaskMng::Stop(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr )
{
    BST_SRV_TASK_NODE_STRU *pstTaskIdx;
    BST_SRV_TASK_NODE_STRU *pstTaskNextIdx;
    BST_UINT32              ulSpinlockCnt;
    BST_ERR_ENUM_UINT8      ucRtnVal;

    if( !BST_DSPP_IsObjIdValid(usObjAddr) )
    {
        return BST_ERR_UNKNOW_OBJ;
    }

    if( BST_INVALID_SNID == usOfstAddr )
    {
        return BST_ERR_UNKNOW_OBJ;
    }
    ucRtnVal            = BST_ERR_UNKNOW_OBJ;
    ulSpinlockCnt       = BST_OS_SpinLock( &m_SpinLock );
    for ( pstTaskIdx = ( BST_SRV_TASK_NODE_STRU *)lstFirst( &m_stTaskList);
          BST_NULL_PTR != pstTaskIdx;
          pstTaskIdx = pstTaskNextIdx )
    {
        pstTaskNextIdx  = ( BST_SRV_TASK_NODE_STRU *)
                          lstNext( (NODE *) pstTaskIdx );
        if ( BST_NULL_PTR == pstTaskIdx->pcTask )
        {
            continue;
        }
        if ( ( ( pstTaskIdx->pcTask->m_usTaskId == usOfstAddr )
              || ( BST_DSPP_BROADCASE_ADDRESS   == usOfstAddr ) )
            && ( pstTaskIdx->pcTask->m_usProcId == usObjAddr ) )
        {
            BST_DBG_LOG2( "BST_SRV_CTaskMng: ProcId=%d, TaskId=%d is Stoped!",usObjAddr, usOfstAddr );
            pstTaskIdx->pcTask->Stop();
            ucRtnVal                    = BST_NO_ERROR_MSG;
            break;//匹配成功后跳出，不用在往下遍历
        }
    }
    BST_OS_SpinUnLock( &m_SpinLock , ulSpinlockCnt);
    return ucRtnVal;
}

BST_UINT16 BST_SRV_CTaskMng::GetLen(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr,
        BST_CORE_PID_ENUM_UINT16    enPid )
{
    if ( !BST_CORE_IsPidValid( enPid ) )
    {
        return 0;
    }
    if( BST_NULL_PTR == m_pcRegedit )
    {
        return 0;
    }
    return m_pcRegedit->GetLen( usObjAddr, usOfstAddr, enPid );
}

BST_UINT16 BST_SRV_CTaskMng::Inquire(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr,
        BST_CORE_PID_ENUM_UINT16    enPid,
        BST_DSPP_CMDLEN_T           usLength,
        BST_VOID                   *pData )
{
    if ( !BST_CORE_IsPidValid( enPid ) )
    {
        return 0;
    }
    if( BST_NULL_PTR == m_pcRegedit )
    {
        return 0;
    }
    if( BST_NULL_PTR == pData )
    {
        return 0;
    }
    if( 0 == usLength )
    {
        return 0;
    }
    return  m_pcRegedit->Inquire( usObjAddr, usOfstAddr, enPid, usLength, pData );
}

BST_ERR_ENUM_UINT8  BST_SRV_CTaskMng::Config(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr,
        BST_CORE_PID_ENUM_UINT16    enPid,
        BST_DSPP_CMDLEN_T           usLength,
        BST_VOID                   *pData )
{
    BST_ERR_ENUM_UINT8      enRtnVal;
    enRtnVal                = BST_NO_ERROR_MSG;

    if ( !BST_CORE_IsPidValid( enPid ) )
    {
        return BST_ERR_PAR_UNKNOW;
    }
    if( BST_NULL_PTR == m_pcRegedit )
    {
        return BST_ERR_NO_MEMORY;
    }
    if( BST_NULL_PTR == pData )
    {
        return BST_ERR_INVALID_PTR;
    }
    if( 0 == usLength )
    {
        return BST_ERR_PAR_LEN;
    }

    enRtnVal            = m_pcRegedit->Config( usObjAddr, usOfstAddr,
                                               enPid, usLength, pData );
    return enRtnVal;
}

BST_CTask *BST_SRV_CTaskMng::Create(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr ) const
{
    BST_PROCID_T                        usProcId;
    BST_TASKID_T                        usTaskId;
    BST_CTask                          *pcTask;

    if( !BST_DSPP_IsObjIdValid(usObjAddr) )
    {
        return BST_NULL_PTR;
    }
    if( BST_INVALID_SNID == usOfstAddr )
    {
        return BST_NULL_PTR;
    }
    usTaskId                            = usOfstAddr;
    usProcId                            = usObjAddr;
    pcTask                              = BST_NULL_PTR;
    switch ( usObjAddr )
    {
        case BST_APP_TYPE_MAILBOX:
            pcTask                      = new BST_APP_CEmailBox( usProcId, usTaskId );
            break;

        case BST_APP_TYPE_HEARTBEAT:
            pcTask                      = new HeartBeat( usProcId, usTaskId );
            break;

        case BST_APP_TYPE_SYSTASK:
            pcTask                      = BST_APP_CMainTask::GetInstance();
            break;
        default:
            pcTask                      = BST_NULL_PTR;
    }

    return pcTask;
}

BST_SRV_CTaskMng::~BST_SRV_CTaskMng()
{
    BST_UINT32                          ulCnt;
    BST_OS_LOCKCNT_T                    tThreadLockCnt;
    tThreadLockCnt                      = BST_OS_ThreadLock();
    m_pcRegedit                         = BST_NULL_PTR;
#if ( BST_OS_VER != BST_QCOM_ROS )
    try
    {
#endif
        for( ulCnt = 0; ulCnt<BST_OBJ_TYPE_MAX; ulCnt++ )
        {
            if( BST_NULL_PTR != m_apcTaskIdGnrtor[ ulCnt ] )
            {
                delete m_apcTaskIdGnrtor[ ulCnt ];
            }
        }
#if ( BST_OS_VER != BST_QCOM_ROS )
    }
    catch (...)
    {
        BST_RLS_LOG("BST_SRV_CTaskMng destructor exception");
    }
#endif
    BST_OS_ThreadUnLock( tThreadLockCnt );
}
