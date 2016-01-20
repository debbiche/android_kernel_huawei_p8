

#ifndef __BST_CORE_P_TASK_H__
#define __BST_CORE_P_TASK_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "BST_Platform.h"
#include "BST_CORE_Task.h"


#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack(4)
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BST_CORE_GetCycleFromFct(ulFct) ( (0x01U) << (ulFct) )
#define BST_CORE_IsCycleFctValid(ulFct) ( ( ( ulFct ) >= BST_TASK_MIN_CYCLEN )&&( ( ulFct )<= BST_TASK_MAX_CYCLEN ) )
#define BST_CORE_GetCycleToMs(ulCyc)    ( (ulCyc)*BST_OS_MS_IN_S*BST_OS_S_IN_MIN )
/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 枚举定义
*****************************************************************************/

enum BST_TASK_BLOCK_STATE_ENUM{
    BST_TASK_BLOCK_STATE_NORMAL,
    BST_TASK_BLOCK_STATE_EXPIRED,
    BST_TASK_BLOCK_STATE_IGNORE,
};
typedef BST_UINT8                       BST_TASK_BLOCK_STATE_ENUM_UINT8;

/*****************************************************************************
  5 类/结构定义
*****************************************************************************/
class BST_CTaskSchdler;

class BST_CORE_CPTask :
    BST_PUBLIC BST_CTask
{
BST_PUBLIC:
    BST_ERR_ENUM_UINT8                  ConfigCycleTicks( BST_UINT32 ulTickFct );
    BST_VOID                            Suspend         ( BST_VOID );
    BST_UINT32                          IsBusy          ( BST_VOID );
    BST_VOID                            Finished        ( BST_VOID );
    friend class                        BST_CTaskSchdler;
BST_PROTECTED:
    BST_TICK_T                          m_ulCycle;
    BST_TICK_T                          m_ulSuspCounter;
    BST_UINT32                          m_ulBusyFlag;
    BST_TASK_BLOCK_STATE_ENUM_UINT8     m_ulTaskBlockState;
    BST_VOID                            PEntry          ( BST_VOID );
                                        BST_CORE_CPTask ( BST_PROCID_T in_usProcId,
                                                          BST_TASKID_T in_usTaskId );
    virtual                            ~BST_CORE_CPTask ( BST_VOID );
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

#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack()
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(pop)
#endif

#endif


