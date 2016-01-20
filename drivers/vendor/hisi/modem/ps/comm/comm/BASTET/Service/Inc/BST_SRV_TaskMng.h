

#ifndef __BST_SEV_TASK_MANAGER_H__
#define __BST_SEV_TASK_MANAGER_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "BST_CORE_Task.h"
#include "BST_CORE_Regedit.h"
#include "BST_LIB_SnGenerator.h"
#include "BST_APP_Define.h"
#include "BST_OS_Thread.h"
#include "BST_DSPP_Define.h"

#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack(4)
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 结构定义
*****************************************************************************/
typedef struct{
    NODE            node;
    BST_CTask      *pcTask;
}BST_SRV_TASK_NODE_STRU;

/*****************************************************************************
  5 类定义
*****************************************************************************/
class BST_SRV_CTaskMng
{
BST_PUBLIC:
    static BST_SRV_CTaskMng *GetInstance( BST_VOID );

    BST_DSPP_OFSTADDR_T     Add(
        BST_DSPP_OBJTADDR_T         usObjAddr );

    BST_ERR_ENUM_UINT8      Remove(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr );

    BST_ERR_ENUM_UINT8      RemoveAll( BST_VOID );

    BST_ERR_ENUM_UINT8      Start(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr );

    BST_ERR_ENUM_UINT8      StartAll( BST_VOID );

    BST_ERR_ENUM_UINT8      Stop(
        BST_DSPP_OBJTADDR_T     usObjAddr,
        BST_DSPP_OFSTADDR_T     usOfstAddr );

    BST_ERR_ENUM_UINT8      StopAll( BST_VOID );

    BST_ERR_ENUM_UINT8      Config(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr,
        BST_CORE_PID_ENUM_UINT16    enPid,
        BST_DSPP_CMDLEN_T           usLength,
        BST_VOID                   *pData );

   BST_UINT16              Inquire(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr,
        BST_CORE_PID_ENUM_UINT16    enPid,
        BST_DSPP_CMDLEN_T           usLength,
        BST_VOID                   *pData );

    BST_UINT16              GetLen(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr,
        BST_CORE_PID_ENUM_UINT16    enPid );

    BST_CTask              *Create(
        BST_DSPP_OBJTADDR_T         usObjAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr )const;

BST_PRIVATE:
                                        BST_SRV_CTaskMng( BST_VOID );
    virtual                            ~BST_SRV_CTaskMng( BST_VOID );
    LIST                                m_stTaskList;
    BST_Lib_CSnGenrator<BST_TASKID_T>  *m_apcTaskIdGnrtor[BST_OBJ_TYPE_MAX];
    BST_CORE_CRegedit 				   *m_pcRegedit;
    BST_OS_SPINLOCK_T                   m_SpinLock;

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
