/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NasMmcPreProcTbl.h
  版 本 号   : 初稿
  作    者   : zhoujun /40661
  生成日期   : 2011年04月22日
  最近修改   :
  功能描述   : NasMmcPreProcTbl.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年04月22日
    作    者   : zhoujun /40661
    修改内容   : 创建文件

******************************************************************************/
#ifndef _NAS_MMC_FSM_PREPROC_H_
#define _NAS_MMC_FSM_PREPROC_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include  "vos.h"
#include  "PsTypeDef.h"
#include  "NasFsm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  1 全局变量定义
*****************************************************************************/

extern NAS_STA_STRU                            g_astNasMmcPreProcessFsmTbl[];


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define NAS_MMC_GetPreProcessStaTbl()                   (g_astNasMmcPreProcessFsmTbl)


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  10 函数声明
*****************************************************************************/
VOS_UINT32 NAS_MMC_GetPreProcessStaTblSize( VOS_VOID  );


NAS_FSM_DESC_STRU * NAS_MMC_GetPreFsmDescAddr(VOS_VOID);



#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of NasMmcFsmTbl.h */
