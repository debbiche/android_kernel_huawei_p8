

#ifndef __BST_PAL_TIMER_H__
#define __BST_PAL_TIMER_H__

#include "BST_Platform.h"
#include "vos.h"
#include "VosPidDef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack(4)
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(push, 4)
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BST_OS_PalSleepMs( ulCnt )      ( VOS_TaskDelay( ulCnt ) )
/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 枚举定义
*****************************************************************************/
/*****************************************************************************
  5 类/结构定义
*****************************************************************************/

/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 全局变量声明
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/
BST_VOID    BST_OS_PalTimerStart( BST_UINT32 ulLength );
BST_VOID    BST_OS_PalTimerStop ( BST_VOID );
BST_VOID    BST_OS_PalTimerInit ( BST_VOID );
BST_UINT32  BST_OS_PalTimerNowMs( BST_VOID );
/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack()
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(pop)
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif
