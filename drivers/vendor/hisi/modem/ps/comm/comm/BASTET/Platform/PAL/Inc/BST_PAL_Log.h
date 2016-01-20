

#ifndef  __BST_PAL_LOG_H__
#define  __BST_PAL_LOG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "PsLogdef.h"
#include "OmCommon.h"
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
#define BST_PAL_ERROR_LOG(MsgString)\
    LPS_LOG(  UEPS_PID_BASTET, 0,LOG_LEVEL_ERROR, (MsgString) )
#define BST_PAL_ERROR_LOG1(MsgString,Para1)\
    LPS_LOG1( UEPS_PID_BASTET, 0,LOG_LEVEL_ERROR, (MsgString), (Para1))
#define BST_PAL_ERROR_LOG2(MsgString,Para1,Para2)\
    LPS_LOG2( UEPS_PID_BASTET, 0,LOG_LEVEL_ERROR, (MsgString), (Para1), (Para2) )
#define BST_PAL_ERROR_LOG3(MsgString,Para1,Para2,Para3)\
    LPS_LOG3( UEPS_PID_BASTET, 0,LOG_LEVEL_ERROR, (MsgString),\
            (Para1), (Para2), (Para3) )
#define BST_PAL_ERROR_LOG4(MsgString,Para1,Para2,Para3,Para4)\
    LPS_LOG4( UEPS_PID_BASTET, 0,LOG_LEVEL_ERROR, (MsgString),\
            (Para1), (Para2), (Para3), (Para4) )

#define BST_PAL_INFO_LOG(MsgString)\
    LPS_LOG(  UEPS_PID_BASTET, 0,LOG_LEVEL_INFO, (MsgString) )
#define BST_PAL_INFO_LOG1(MsgString,Para1)\
    LPS_LOG1( UEPS_PID_BASTET, 0,LOG_LEVEL_INFO, (MsgString), (Para1))
#define BST_PAL_INFO_LOG2(MsgString,Para1,Para2)\
    LPS_LOG2( UEPS_PID_BASTET, 0,LOG_LEVEL_INFO, (MsgString), (Para1), (Para2) )
#define BST_PAL_INFO_LOG3(MsgString,Para1,Para2,Para3)\
    LPS_LOG3( UEPS_PID_BASTET, 0,LOG_LEVEL_INFO, (MsgString),\
            (Para1), (Para2), (Para3) )
#define BST_PAL_INFO_LOG4(MsgString,Para1,Para2,Para3,Para4)\
    LPS_LOG4( UEPS_PID_BASTET, 0,LOG_LEVEL_INFO, (MsgString),\
            (Para1), (Para2), (Para3), (Para4) )


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
