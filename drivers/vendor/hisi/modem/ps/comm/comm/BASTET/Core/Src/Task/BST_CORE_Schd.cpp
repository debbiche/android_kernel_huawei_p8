

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_CORE_Schd.h"
#include "BST_OS_Thread.h"
#include "BST_DBG_MemLeak.h"
#include "BST_SRV_TaskMng.h"
#include "BST_DRV_As.h"
#include "BST_DSPP_Define.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_CORE_SCHD_CPP
/*lint +e767*/
/*****************************************************************************
  2 函数声明
*****************************************************************************/
extern "C" BST_VOID BST_SRV_InitAppThreads ( BST_VOID );
/******************************************************************************
   4 私有定义
******************************************************************************/

/******************************************************************************
   5 全局变量定义
******************************************************************************/
BST_UINT16  g_ucAppInitCnt;
LIST        g_stPTaskList;

/******************************************************************************
   6 函数实现
******************************************************************************/


/*lint -e429*/
BST_VOID BST_CTaskSchdler::Attach ( BST_CORE_CPTask *pC_PTask )
{
    BST_CORE_PTASK_NODE_STRU   *pstPtaskNode;

    if ( !BST_OS_IsTimerValid (m_ulTimerId) )
    {
        BST_RLS_LOG1( "BST_CTaskSchdler::Attach m_ulTimerId=%u is invalid",
                      m_ulTimerId );
        return;
    }
    if ( BST_NULL_PTR == pC_PTask )
    {
        BST_RLS_LOG( "BST_CTaskSchdler::Attach pC_PTask=NULL" );
        return;
    }
    for ( pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstFirst( &g_stPTaskList );
          pstPtaskNode!= BST_NULL_PTR;
          pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstNext((NODE *)pstPtaskNode) )
    {
        if( pstPtaskNode->pcTask == pC_PTask )
        {
            return;
        }
    }
    /*
     * 申请链表节点资源，用于存储任务
     */
    pstPtaskNode                = ( BST_CORE_PTASK_NODE_STRU *)BST_OS_MALLOC
                                  ( BST_OS_SIZEOF( BST_CORE_PTASK_NODE_STRU ) );
    if( BST_NULL_PTR == pstPtaskNode )
    {
        BST_RLS_LOG( "BST_CTaskSchdler::Attach pstPtaskNode=NULL" );
        return;
    }
    /*
     * 初始化任务数据，添加如任务列表
     */
    pC_PTask->m_ulSuspCounter   = 0;
    pstPtaskNode->pcTask        = pC_PTask;
    lstAdd( &g_stPTaskList, (NODE *)pstPtaskNode );
    /*
     * 如果此时系统TICK定时已经关闭，且PS域服务状态正常，那么需要再次启动定时
     */
    if(  BST_TRUE == BST_OS_TimerIsStop( m_ulTimerId ) )
    {
        m_ulSystemTick          = 0;
        BST_OS_TimerStart ( m_ulTimerId, BST_TASK_SYS_BASE_TIMELEN );
        BST_DBG_LOG1( "BST_CTaskSchdler::Attach Start Scheduler Timer:%u",
                      BST_TASK_SYS_BASE_TIMELEN );
    }
}
/*lint +e429*/

/*lint -e438*/
BST_VOID BST_CTaskSchdler::Detach ( BST_CORE_CPTask *pC_PTask )
{
    BST_CORE_PTASK_NODE_STRU   *pstPtaskNode;

    if ( !BST_OS_IsTimerValid (m_ulTimerId) )
    {
        BST_RLS_LOG1( "BST_CTaskSchdler::Detach m_ulTimerId=%u is invalid",
                      m_ulTimerId );
        return;
    }
    if ( BST_NULL_PTR == pC_PTask )
    {
        BST_RLS_LOG( "BST_CTaskSchdler::Detach pC_PTask=NULL" );
        return;
    }
    /*
     * 遍历列表，找到pctask相符的任务，从链表删除，释放资源
     */
    for ( pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstFirst( &g_stPTaskList );
          pstPtaskNode!= BST_NULL_PTR;
          pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstNext((NODE *)pstPtaskNode) )
    {
        if( pstPtaskNode->pcTask != pC_PTask )
        {
            continue;
        }
        lstDelete( &g_stPTaskList, (NODE *)pstPtaskNode );
        BST_OS_FREE( pstPtaskNode );
        break;
    }
    /*
     * 注销后，如果认为列表为空，则直接关闭定时器，不进行任何调度
     */
    if ( 0 == lstCount( &g_stPTaskList ) )
    {
        BST_DBG_LOG( "BST_CTaskSchdler::Detach Stop Scheduler Timer" );
        BST_OS_TimerStop ( m_ulTimerId );
    }
}
/*lint +e438*/

BST_VOID BST_CTaskSchdler::Suspend ( BST_VOID )
{
    BST_CORE_PTASK_NODE_STRU   *pstPtaskNode;

    if ( 0 == lstCount( &g_stPTaskList ) )
    {
        BST_RLS_LOG( "BST_CTaskSchdler::Suspend g_stPTaskList count=0" );
        return;
    }
    /*
     * 遍历任务表，找到相关任务对齐进行挂起
     */
    for ( pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstFirst( &g_stPTaskList );
          pstPtaskNode!= BST_NULL_PTR;
          pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstNext((NODE *)pstPtaskNode) )
    {
        if ( BST_NULL_PTR == pstPtaskNode->pcTask )
        {
            continue;
        }
        if ( BST_TASK_STATE_START == pstPtaskNode->pcTask->GetState () )
        {
            pstPtaskNode->pcTask->Suspend();
        }
    }
}


BST_CTaskSchdler* BST_CTaskSchdler::GetInstance ( BST_VOID )
{
    static BST_CTaskSchdler    *pTaskScheduler = BST_NULL_PTR;

    if ( BST_NULL_PTR == pTaskScheduler )
    {
        pTaskScheduler          = new BST_CTaskSchdler;
    }
    return pTaskScheduler;
}


BST_CTaskSchdler::BST_CTaskSchdler ( BST_VOID )
{
#if( 0 != BST_BLOCKED_THREAD_NUM )
    BST_UINT32              ulCnt;
    BST_CHAR                cThreadName[BST_BLOCKED_THREAD_LEN];
    BST_OS_MEMSET( cThreadName, 0, BST_OS_SIZEOF( cThreadName ) );
#endif
    m_ulTimerId             = BST_OS_TimerCreateCpp ( this, BST_NULL_PTR );
    m_ulSystemTick          = 0;
    m_ulBlockInfo           = BST_AS_BLOCKED_BY_NO_SERVICE;
    m_bTaskMissExec         = BST_FALSE;
    g_ucAppInitCnt          = 0;
    lstInit( &g_stPTaskList );
#ifdef BST_UNBLOCKED_OWN_THREAD
    /*
     * 如果非阻塞型应用有自己的独立线程，则创建相关线程
     */
    BST_OS_CreateThread ( (BST_INT8 *)"Bastet UnBlocked Thread",
                          (BST_OS_THREAD_T)AppThread,
                          &m_pstUnBlockTaskMbx,
                           BST_OS_STACK_SIZE_XL,
                           BST_OS_PRIO_LOW );
#endif
#if( 0 != BST_BLOCKED_THREAD_NUM )
    /*
     * 如果阻塞性应用进程不为0，则初始化各个线程，并传递任务邮箱指针
     */
    for ( ulCnt = 0; ulCnt < BST_BLOCKED_THREAD_NUM; ulCnt ++ )
    {
        m_pastBlockTaskMbx[ ulCnt ]     = BST_NULL_PTR;

        snprintf( cThreadName, BST_OS_SIZEOF( cThreadName ), "Bastet %d Blocked Thread", ulCnt );
        BST_OS_CreateThread ( (BST_INT8 *)cThreadName,
                              (BST_OS_THREAD_T)BST_CTaskSchdler::AppThread,
                              &m_pastBlockTaskMbx[ulCnt],
                               BST_OS_STACK_SIZE_XL,
                               BST_OS_PRIO_HIGH );
        BST_DBG_LOG2 ( "BST_CTaskSchdler::BST_CTaskSchdler Blocked Task %u Create, MailBoxId=%p ",
                       ulCnt, &m_pastBlockTaskMbx[ulCnt] );
    }
#endif
    RegAsNotice( BST_AS_EVT_L3_RRC_STATE );
    RegAsNotice( BST_AS_EVT_L3_BLOCK_INFO );
}


BST_CTaskSchdler::~BST_CTaskSchdler ( BST_VOID )
{
#ifdef BST_UNBLOCKED_OWN_THREAD
    m_pstUnBlockTaskMbx = BST_NULL_PTR;
#endif
}


BST_VOID BST_CTaskSchdler::TimerExpired(
    BST_OS_TIMERID_T    ulId,
    BST_VOID           *pvPara)
{
    BST_CORE_PTASK_NODE_STRU   *pstPtaskNode;
    BST_CORE_PTASK_NODE_STRU   *pstPtaskNodeNext;

    if ( !BST_OS_IsTimerValid (m_ulTimerId) )
    {
        BST_RLS_LOG1( "BST_CTaskSchdler::TimerExpired m_ulTimerId=%u is invalid",
                      m_ulTimerId );
        return;
    }
    if ( ulId != m_ulTimerId )
    {
        BST_RLS_LOG2( "BST_CTaskSchdler::TimerExpired ulId=%u,m_ulTimerId=%u",
                      ulId, m_ulTimerId );
        return;
    }
    /*
     * 如果没有任务，则直接返回，不做任何操作
     */
    if ( 0 == lstCount( &g_stPTaskList ) )
    {
        BST_RLS_LOG( "BST_CTaskSchdler::TimerExpired g_stPTaskList count=0" );
        return;
    }

    /*
     * 更新系统TICK值
     */
    m_ulSystemTick         += BST_TASK_SYS_TICKS;
    BST_DBG_LOG1 ( "BST_CTaskSchdler::TimerExpired Scheduler TimeOut, Tick=%d ",
                   m_ulSystemTick );

    /*
     * 遍历任务列表，获取任务并进行相应调度
     */
    for ( pstPtaskNode = ( BST_CORE_PTASK_NODE_STRU *)lstFirst( &g_stPTaskList );
          pstPtaskNode!= BST_NULL_PTR;
          pstPtaskNode = pstPtaskNodeNext )
    {
        pstPtaskNodeNext    = ( BST_CORE_PTASK_NODE_STRU *)
                              lstNext((NODE *)pstPtaskNode);
        if ( BST_NULL_PTR == pstPtaskNode->pcTask )
        {
            continue;
        }
        /*
         * 如果任务不为空，则根据状态进行调度
         */
        ScheduleTask ( pstPtaskNode->pcTask );
    }
    /*
     * 再次启动系统TICK定时器
     */
    BST_OS_TimerStart ( m_ulTimerId, BST_TASK_SYS_BASE_TIMELEN );
}

BST_BOOL BST_CTaskSchdler::TryTaskExist ( BST_VOID )
{
    BST_UINT32                  ulNextTick;
    BST_TASK_STATE_ENUM_UINT8   enTaskState;
    BST_SRV_CTaskMng           *pcTaskManager;
    BST_CORE_PTASK_NODE_STRU   *pstPtaskNode;

    ulNextTick                  = 0;
    pcTaskManager               = BST_SRV_CTaskMng::GetInstance ();
    BST_ASSERT_NULL_RTN( pcTaskManager, BST_FALSE );
    /*
     * 在当前系统值加一个TICK单位，获得下一时刻TICK值
     */
    ulNextTick                  = m_ulSystemTick + BST_TASK_SYS_TICKS;

    for ( pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstFirst( &g_stPTaskList );
          pstPtaskNode!= BST_NULL_PTR;
          pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstNext((NODE *)pstPtaskNode ) )
    {
        if ( BST_NULL_PTR == pstPtaskNode->pcTask )
        {
            continue;
        }
        /*
         * 如果任务没有执行完成，处于繁忙状态，则立即返回
         */
        if ( BST_TRUE == pstPtaskNode->pcTask->IsBusy () )
        {
            continue;
        }
       /*
        * 如果TICK值能够被任务周期整除，说明到了该任务周期
        */
        if ( 0 == ( ulNextTick % pstPtaskNode->pcTask->m_ulCycle ) )
        {
            enTaskState         = pstPtaskNode->pcTask->GetState ();
            if ( BST_TASK_STATE_START == enTaskState )
            {
                return BST_TRUE;
            }
            else
            {
                continue;
            }
        }
        else
        {
            continue;
        }
    }
    return BST_FALSE;
}


BST_VOID BST_CTaskSchdler::Execute( BST_CORE_CPTask *pC_PTask )
{
    BST_RUN_MODE_ENUM_UINT8     enTaskRunMode;
#if( 0 != BST_BLOCKED_THREAD_NUM )
    BST_STATIC BST_UINT32       ulUnblkedCnt = 0;
#endif

    enTaskRunMode               = pC_PTask->GetRunMode ();
    pC_PTask->m_ulSuspCounter   = 0;
    /*
     * 根据任务运行模式，调度执行相关任务
     */
    switch ( enTaskRunMode )
    {
        /*
         * 如果阻塞性任务，且没有单独线程，则直接执行，如果有任务则发邮件
         * 不同进程由ulUnblkedCnt统一分配，平均各线程任务负荷
         */
        case BST_TASK_RUN_BLOCKED:
#if( 0 != BST_BLOCKED_THREAD_NUM )
            if ( ulUnblkedCnt >= BST_BLOCKED_THREAD_NUM )
            {
                ulUnblkedCnt    = 0;
            }
            if ( BST_NULL_PTR == m_pastBlockTaskMbx[ulUnblkedCnt] )
            {
                BST_RLS_LOG1 ( "BST_CTaskSchdler::Execute Blocked Thread %d is not ready",
                               ulUnblkedCnt );
                return;
            }
            BST_OS_SendMail ( m_pastBlockTaskMbx[ulUnblkedCnt], pC_PTask );
            ulUnblkedCnt++;
#else
            pC_PTask->PEntry();
#endif
            break;
        /*
         * 如果非阻塞性任务，且没有单独线程，则直接执行，如果有任务则发邮件
         */
        case BST_TASK_RUN_UNBLOCKED:
#ifdef BST_UNBLOCKED_OWN_THREAD
            if ( BST_NULL_PTR == m_pstUnBlockTaskMbx )
            {
                BST_RLS_LOG ( "BST_CTaskSchdler::Execute UnBlocked Thread is not ready" );
                return;
            }
            BST_OS_SendMail ( m_pstUnBlockTaskMbx, pC_PTask );
#else
            pC_PTask->PEntry();
#endif
            break;

        default:
            break;
    }
}


BST_VOID    BST_CTaskSchdler::ScheduleTask( BST_CORE_CPTask *pC_PTask )
{
    BST_SRV_CTaskMng           *pcTaskManager;
    BST_TASK_STATE_ENUM_UINT8   enTaskState;

    pcTaskManager               = BST_SRV_CTaskMng::GetInstance ();
    BST_ASSERT_NULL( pcTaskManager );

    /*
     * 入口检查
     */
    if ( BST_NULL_PTR == pC_PTask )
    {
        BST_RLS_LOG( "BST_CTaskSchdler::ScheduleTask pC_PTask=NULL" );
        return;
    }
    /*
     * 如果上次执行的事件尚未完成，本次不运行
     */
    if ( BST_TRUE == pC_PTask->IsBusy () )
    {
        BST_RLS_LOG( "BST_CTaskSchdler::ScheduleTask pC_PTask Is Busy" );
        return;
    }
    /*
     * 如果系统TICK可以被任务周期整除，说明到了整数倍对齐时间
     */
    if ( 0 != m_ulSystemTick%pC_PTask->m_ulCycle )
    {
        return;
    }
    enTaskState                 = pC_PTask->GetState ();

    BST_DBG_LOG3( "BST_CTaskSchdler::ScheduleTask enTaskState:%d,m_ulTaskBlockState:%d,m_ulBlockInfo:%d",
                 enTaskState, pC_PTask->m_ulTaskBlockState,m_ulBlockInfo);
    switch ( enTaskState )
    {
        /*
         * 如果任务停止，只设置suspendCounter为0，不做任何事情
         */
        case BST_TASK_STATE_STOPED:
            pC_PTask->m_ulSuspCounter    = 0;
            break;

        /*
         * 如果任务挂起超过最大限度，则立即删除之
         */
        case BST_TASK_STATE_SUSPEND:
            pC_PTask->m_ulSuspCounter ++;
            if ( pC_PTask->m_ulSuspCounter >= BST_TASK_MAX_SYSPEND )
            {
                pcTaskManager->Remove ( pC_PTask->m_usProcId, pC_PTask->m_usTaskId );
            }
            break;
        /*
         * 如果认为处于运行态，则调度执行
         */
        case BST_TASK_STATE_START:
            switch(pC_PTask->m_ulTaskBlockState)
            {
                /*当前处于normal状态，在周期的整数倍时，如果当前为unblock状态，则执行任务
                               否则设置当前task的任务状态为expired
                            */
                case BST_TASK_BLOCK_STATE_NORMAL:
                    if( BST_AS_UNBLOCKED == m_ulBlockInfo )
                    {
                        Execute( pC_PTask );
                    }
                    else
                    {
                        pC_PTask->m_ulTaskBlockState = BST_TASK_BLOCK_STATE_EXPIRED;
                        m_bTaskMissExec              = BST_TRUE;
                    }
                    break;
                /*当前处于EXPIRED状态，如果恢复unblock状态，直接调度，该分支为异常保护分支*/
                case BST_TASK_BLOCK_STATE_EXPIRED:
                    if( BST_AS_UNBLOCKED == m_ulBlockInfo )
                    {
                        Execute( pC_PTask );
                        pC_PTask->m_ulTaskBlockState = BST_TASK_BLOCK_STATE_NORMAL;
                    }
                    break;
                 default:
                    break;

            }
            break;
        default:
            break;
    }

    if(BST_TASK_BLOCK_STATE_IGNORE == pC_PTask->m_ulTaskBlockState)
    {
        pC_PTask->m_ulTaskBlockState = BST_TASK_BLOCK_STATE_NORMAL;
    }
}
BST_VOID BST_CTaskSchdler::AsEventCallback(
    BST_AS_EVT_ENUM_UINT32    enEvent,
    BST_UINT32                ulLength,
    BST_VOID                 *pvData )
{
    BST_UINT32          ulPara;

    switch( enEvent )
    {
        /*
         * 仅处理RRC状态信息，当RRC连接成功时，查看当前剩余时间是否靠近
         */
        case BST_AS_EVT_L3_RRC_STATE:
            if ( BST_OS_SIZEOF(ulPara) != ulLength )
            {
                break;
            }
            if ( BST_NULL_PTR == pvData )
            {
                break;
            }
            BST_OS_MEMCPY( &ulPara, pvData, ulLength );
            if( BST_AS_RRC_CONNECTED == ulPara )
            {
                ulPara      = BST_OS_TimerIsStop( m_ulTimerId );
                if ( BST_TRUE == ulPara )
                {
                    break;
                }
                ulPara      = BST_OS_TimeGetRemain( m_ulTimerId );
                if ( 0 == ulPara )
                {
                    break;
                }
                ulPara      = NearRrcTrig ( ulPara );
                BST_OS_TimerStart( m_ulTimerId, ulPara );
            }
            break;

        case BST_AS_EVT_L3_BLOCK_INFO:
            if ( BST_OS_SIZEOF(BST_UINT32) != ulLength )
            {
                BST_RLS_LOG1("BST_CTaskSchdler::AsEventCallback Error blockinfo Length = %u:", ulLength );
                break;
            }

            UtranBlockInfoChgProc( (BST_UINT32 *)pvData );
            break;
        default:
            break;
    }
}


BST_UINT32 BST_CTaskSchdler::NearRrcTrig ( BST_UINT32 const ulSysTimerRemainMs )
{
    BST_CORE_PTASK_NODE_STRU   *pstPtaskNode;
    BST_UINT32                  ulNextSysTick;
    BST_UINT32                  ulNextLongCycle;

    ulNextLongCycle             = 0;
    ulNextSysTick               = m_ulSystemTick + BST_TASK_SYS_TICKS;
    /*
     * 遍历任务列表，找出所有有效任务
     */
    for( pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstFirst( &g_stPTaskList );
         pstPtaskNode!= BST_NULL_PTR;
         pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstNext((NODE *)pstPtaskNode) )
    {
        if( BST_NULL_PTR == pstPtaskNode->pcTask )
        {
            continue;
        }
        if( 0 == pstPtaskNode->pcTask->m_ulCycle )
        {
            continue;
        }
        if( BST_TRUE == pstPtaskNode->pcTask->IsBusy () )
        {
            continue;
        }
        if( BST_TASK_STATE_START != pstPtaskNode->pcTask->GetState () )
        {
            continue;
        }
        /*
         * 查看是否有任务周期与Tick整除项，代码周期到来
         */
        if( 0 == ( ulNextSysTick % pstPtaskNode->pcTask->m_ulCycle ) )
        {
            if( pstPtaskNode->pcTask->m_ulCycle > ulNextLongCycle )
            {
                ulNextLongCycle = pstPtaskNode->pcTask->m_ulCycle;
            }
        }
    }
    /*
     * 获取当前系统周期，并取比例，如果剩余时间低于该值，则说明足够靠近
     */
    ulNextLongCycle             = BST_CORE_GetCycleToMs( ulNextLongCycle );
    if( BST_TASK_IsNearEnough( ulSysTimerRemainMs, ulNextLongCycle ) )
    {
        BST_RLS_LOG("BST_CTaskSchdler::NearRrcTrig Closing To RRC Enough, Periodic Runing after 200Ms");
        return BST_TASK_RRC_SEND_TIMER;
    }
    else
    {
        return ulSysTimerRemainMs;
    }
}


BST_UINT32 BST_CTaskSchdler::ChkTaskExist ( BST_CORE_CPTask *pC_PTask )
{
    BST_CORE_PTASK_NODE_STRU   *pstPtaskNode;

    if ( BST_NULL_PTR == pC_PTask )
    {
        BST_RLS_LOG( "BST_CTaskSchdler::ChkTaskExist pC_PTask=NULL" );
        return BST_FALSE;
    }

    if ( 0 == lstCount( &g_stPTaskList ) )
    {
        BST_RLS_LOG( "BST_CTaskSchdler::ChkTaskExist g_stPTaskList count=0" );
        return BST_FALSE;
    }

    /*
     * 遍历任务表，对比pcPtask，相等的记为找到
     */
    for ( pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstFirst( &g_stPTaskList );
          pstPtaskNode!= BST_NULL_PTR;
          pstPtaskNode = (BST_CORE_PTASK_NODE_STRU *)lstNext((NODE *)pstPtaskNode) )
    {
        if ( BST_NULL_PTR == pstPtaskNode->pcTask )
        {
            continue;
        }
        if ( pC_PTask == pstPtaskNode->pcTask )
        {
            return BST_TRUE;
        }
    }
    return BST_FALSE;
}


BST_VOID BST_CTaskSchdler::AppThread ( BST_VOID* arg )
{
    BST_CORE_CPTask    *pcTask;
    BST_OS_MBX_T      **pstMailBox;
    BST_OS_LOCKCNT_T    tThreadLockCnt;
    BST_UINT32          ulRtnVal;

    if ( BST_NULL_PTR == arg )
    {
        return;
    }
    /*
     * 线程参数初始化，包括邮箱，运行标识
     */
    tThreadLockCnt      = BST_OS_ThreadLock ();
    pstMailBox          = (BST_OS_MBX_T **)arg;
   *pstMailBox          = BST_OS_CreateMbx
                        ( BST_OS_GetCurrentThreadId (), BST_MAX_APP_NUMBER );
    BST_SetThreadInitFlag ( BST_APP_THREAD_FLAG << g_ucAppInitCnt );
    g_ucAppInitCnt++;
    BST_OS_ThreadUnLock ( tThreadLockCnt );

    /*
     * 进入线程主循环，等待邮件，邮件内容为即将执行的任务
     */
    for(;;)
    {
        ulRtnVal        = BST_OS_RecvMail
                       ( *pstMailBox, (BST_VOID **)&pcTask, BST_OS_SYS_FOREVER );
        BST_ASSERT_NORM( BST_OS_SYS_TIMEOUT == ulRtnVal );
        if ( BST_NULL_PTR == pcTask )
        {
            continue;
        }
        if ( BST_FALSE == ChkTaskExist (pcTask) )
        {
            continue;
        }
        if ( BST_TRUE == pcTask->IsBusy () )
        {
            continue;
        }
        pcTask->PEntry ();
    }
}

BST_VOID BST_CTaskSchdler::UtranBlockInfoChgProc( BST_UINT32 *pulBlockInfo )
{
    BST_INT32                   lTaskCnt;
    BST_UINT32                  ulNextTickRemainTime;
    BST_UINT32                  ulBlockInfo;
    BST_CORE_PTASK_NODE_STRU   *pstPtaskNode;
    BST_CORE_PTASK_NODE_STRU   *pstPtaskNodeNext;
    BST_BOOL                    bTimerStop;

    if( BST_NULL_PTR == pulBlockInfo)
    {
        return;
    }
    ulBlockInfo     = (BST_UINT32 )*pulBlockInfo;
    BST_DBG_LOG2( "BST_CTaskSchdler::UtranBlockInfoChgProc blockold=u%, blockNew=u%",
                  m_ulBlockInfo,ulBlockInfo);

    if ( m_ulBlockInfo == ulBlockInfo )
    {
        return;
    }
    m_ulBlockInfo           = ulBlockInfo;

    if ( BST_AS_UNBLOCKED == m_ulBlockInfo )
    {

        BST_DBG_LOG1( "BST_CTaskSchdler::UtranBlockInfoChgProc :%d", m_bTaskMissExec );
        if(BST_FALSE == m_bTaskMissExec)
        {
            return;
        }
        lTaskCnt            = lstCount( &g_stPTaskList );

        BST_DBG_LOG1( "BST_CTaskSchdler::UtranBlockInfoChgProc TaskNum=u%", lTaskCnt );

        if ( 0 == lTaskCnt )
        {
            return;
        }
        ulNextTickRemainTime = 0;
        bTimerStop      = BST_OS_TimerIsStop( m_ulTimerId );
        if ( BST_FALSE == bTimerStop )
        {
            ulNextTickRemainTime      = BST_OS_TimeGetRemain( m_ulTimerId );
            BST_OS_TimerStop ( m_ulTimerId );
        }

        BST_DBG_LOG2( "BST_CTaskSchdler::UtranBlockInfoChgProc stop:%d,timerremain=u%", bTimerStop,ulNextTickRemainTime );
        /*
         * 遍历任务列表，获取任务并进行相应调度
         */
        for ( pstPtaskNode = ( BST_CORE_PTASK_NODE_STRU *)lstFirst( &g_stPTaskList );
              pstPtaskNode!= BST_NULL_PTR;
              pstPtaskNode = pstPtaskNodeNext )
        {
            pstPtaskNodeNext    = ( BST_CORE_PTASK_NODE_STRU *)
                                  lstNext((NODE *)pstPtaskNode);
            if ( BST_NULL_PTR == pstPtaskNode->pcTask )
            {
                continue;
            }
            /*
             * 如果任务不为空，则根据状态进行调度
             */
            TrigScheduleTask ( pstPtaskNode->pcTask);
        }

        if((0 == ulNextTickRemainTime) || (ulNextTickRemainTime > BST_TASK_SYS_BASE_TIMELEN))
        {
            ulNextTickRemainTime = BST_TASK_SYS_BASE_TIMELEN;
        }

        BST_OS_TimerStart ( m_ulTimerId, ulNextTickRemainTime );

    }

}



BST_VOID    BST_CTaskSchdler::TrigScheduleTask( BST_CORE_CPTask *pC_PTask)
{
    BST_SRV_CTaskMng           *pcTaskManager;
    BST_TASK_STATE_ENUM_UINT8   enTaskState;
    BST_UINT32                  ulRemainTickNum;

    pcTaskManager               = BST_SRV_CTaskMng::GetInstance ();
    BST_ASSERT_NULL( pcTaskManager );

    /*
     * 入口检查
     */
    if ( BST_NULL_PTR == pC_PTask )
    {
        BST_RLS_LOG( "BST_CTaskSchdler::TrigScheduleTask pC_PTask=NULL" );
        return;
    }
    /*
     * 如果上次执行的事件尚未完成，本次不运行
     */
    if ( BST_TRUE == pC_PTask->IsBusy () )
    {
        BST_RLS_LOG( "BST_CTaskSchdler::TrigScheduleTask pC_PTask Is Busy" );
        return;
    }
    /*
     * 如果系统TICK可以被任务周期整除，说明到了整数倍对齐时间
     */
    enTaskState                 = pC_PTask->GetState ();

    BST_DBG_LOG2( "BST_CTaskSchdler::TrigScheduleTask enTaskState:%d,m_ulTaskBlockState:%d",
                       enTaskState,pC_PTask->m_ulTaskBlockState );

    switch ( enTaskState )
    {
        case BST_TASK_STATE_START:
            if( BST_TASK_BLOCK_STATE_EXPIRED ==  pC_PTask->m_ulTaskBlockState )
            {
                Execute( pC_PTask );
                ulRemainTickNum = pC_PTask->m_ulCycle - (m_ulSystemTick % pC_PTask->m_ulCycle);
                if( ulRemainTickNum > BST_TASK_SYS_TICKS )
                {
                    pC_PTask->m_ulTaskBlockState    = BST_TASK_BLOCK_STATE_NORMAL;
                }
                else
                {
                    pC_PTask->m_ulTaskBlockState    = BST_TASK_BLOCK_STATE_IGNORE;
                }
                m_bTaskMissExec                     = BST_FALSE;
            }
            return;
        default:
            break;
    }
}



BST_VOID BST_SRV_InitAppThreads ( BST_VOID )
{
    BST_CTaskSchdler::GetInstance ();
}
