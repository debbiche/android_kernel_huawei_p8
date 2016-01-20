

#ifndef __BST_CORE_SCHEDULER_H__
#define __BST_CORE_SCHEDULER_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "BST_Platform.h"
#include "BST_CORE_PTask.h"
#include "BST_OS_Timer.h"
#include "BST_OS_Thread.h"
#include "BST_OS_Sync.h"
#include "BST_SRV_AsEvnt.h"

#if(BST_OS_VER == BST_HISI_VOS)
#pragma pack(4)
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BST_TASK_SYS_BASE_TIMELEN       (( BST_TASK_SYS_TICKS )* \
                                        BST_OS_MS_IN_S*BST_OS_S_IN_MIN )//2 minuts
#define BST_TASK_SYS_RETRY_TIMELEN      (( 5 )*BST_OS_MS_IN_S )         //5 Second

#define BST_TASK_RELATIVE_RATE          ( 10 )

#define BST_TASK_ABSLT_TIME_MS          ( 60000 )

#define BST_TASK_IsNearEnough(rm,cyc)   ( ((rm) <= \
                                          ((cyc) * BST_TASK_RELATIVE_RATE )/100 )\
                                        &&( (rm)<BST_TASK_ABSLT_TIME_MS ) )

#define BST_TASK_RRC_SEND_TIMER         ( BST_OS_TIMER_PRECISION * 2 )

/* Thread 名字的长度 */
#define BST_BLOCKED_THREAD_LEN          ( 25 )
/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 枚举定义
*****************************************************************************/

/*****************************************************************************
  5 类/结构定义
*****************************************************************************/
typedef struct
{
    NODE                node;
    BST_CORE_CPTask    *pcTask;
}BST_CORE_PTASK_NODE_STRU;

class BST_CTaskSchdler
    : BST_SRV_CAsRcver, BST_OS_CTimerCb
{
BST_PUBLIC:
    BST_VOID            Attach( BST_CORE_CPTask    *pC_PTask );
    BST_VOID            Detach( BST_CORE_CPTask    *pC_PTask );
    BST_VOID            Suspend( BST_VOID );
    static              BST_CTaskSchdler *GetInstance( BST_VOID );
    static BST_VOID     AppThread( BST_VOID    *arg );
BST_PRIVATE:
                        BST_CTaskSchdler( BST_VOID );

    virtual            ~BST_CTaskSchdler( BST_VOID );

    BST_VOID            TimerExpired(
        BST_OS_TIMERID_T    ulId,
        BST_VOID* pvPara );

    BST_VOID            ScheduleTask( BST_CORE_CPTask  *pC_PTask );

    BST_VOID            Execute( BST_CORE_CPTask *pC_PTask );

    static BST_UINT32   ChkTaskExist(
        BST_CORE_CPTask        *pC_PTask );

    BST_UINT32          NearRrcTrig(
        BST_UINT32 const        ulSysTimerRemainMs );

    BST_VOID            AsEventCallback(
        BST_AS_EVT_ENUM_UINT32  enEvent,
        BST_UINT32              ulLength,
        BST_VOID               *pvData );

    BST_VOID            UtranBlockInfoChgProc( BST_UINT32 *pulBlockInfo );
    BST_VOID            TrigScheduleTask( BST_CORE_CPTask *pC_PTask);

    BST_BOOL            TryTaskExist( BST_VOID );
    BST_UINT32          m_ulSystemTick;
    BST_OS_TIMERID_T    m_ulTimerId;
    BST_BOOL            m_bTaskMissExec;
    BST_UINT32          m_ulBlockInfo;

#if( 0 != BST_BLOCKED_THREAD_NUM )
    BST_OS_MBX_T       *m_pastBlockTaskMbx[ BST_BLOCKED_THREAD_NUM ];
#endif
#ifdef BST_UNBLOCKED_OWN_THREAD
    BST_OS_MBX_T       *m_pstUnBlockTaskMbx;
#endif
};

/*****************************************************************************
  6 UNION定义
*****************************************************************************/


/*****************************************************************************
  7 全局变量声明
*****************************************************************************/


/*****************************************************************************
  8 函数声明
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

#if(BST_OS_VER == BST_HISI_VOS)
#pragma pack()
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(pop)
#endif

#endif
