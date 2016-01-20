

#ifndef __BST_IP_RAW_CALL_BACK_H__
#define __BST_IP_RAW_CALL_BACK_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "BST_Platform.h"
#include "BST_IP_Define.h"
#include "BST_OS_Thread.h"
#include "BST_LIB_List.h"

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
  4 枚举定义
*****************************************************************************/

/*****************************************************************************
  5 结构定义
*****************************************************************************/
class BST_IP_CNetRcver;

typedef struct{
    NODE                            node;
    BST_FD_T                       *pstFd;
    BST_IP_CNetRcver               *pcRcver;
}BST_IP_RCVER_BINDER_STRU;

class BST_IP_CRcverMng
{
BST_PUBLIC:
    BST_VOID                       *Attach(
        BST_FD_T           *pstFd,
        BST_IP_CNetRcver   *pcRawRcver);

    BST_VOID                        Detach(
        BST_FD_T           *pstFd );

    static BST_IP_CRcverMng        *GetInstance( BST_VOID );

    BST_IP_ERR_T                    CnctedRcver(
        BST_FD_T            fd,
        BST_IP_ERR_T        InIpErrMsg );

    BST_VOID                        ErrMsgRcver(
        BST_FD_T            fd,
        BST_IP_ERR_T        InIpErrMsg );

    BST_VOID                        ClosedRcver(
        BST_FD_T            fd );

    BST_IP_PKTPROC_MODE_ENUM        PacketRcver(
        BST_FD_T            fd,
        const BST_UINT8    *const pdata,
        const BST_UINT16    length );

BST_PRIVATE:                       
    BST_IP_CRcverMng( BST_VOID );

   ~BST_IP_CRcverMng( BST_VOID );

    BST_IP_CNetRcver               *FindRcver( BST_FD_T fd );

    BST_VOID                        ClearFd( BST_FD_T fd );

    LIST                            m_stRcverList;
};

class BST_IP_CNetRcver
{
BST_PUBLIC:
    virtual BST_IP_ERR_T            Connectd( BST_FD_T fd )
    {
        return BST_IP_ERR_OK;
    }

    virtual BST_IP_PKTPROC_MODE_ENUM    Received(
        BST_FD_T            fd,
        const BST_UINT8    *const pucData,
        const BST_UINT16    usLength )
    {
        return BST_IP_PKT_REMOVE_PROC;
    }

    virtual BST_VOID                Closed( BST_FD_T fd )
    {
        return;
    }

    virtual BST_VOID                ErrClosed(
        BST_FD_T            fd,
        BST_IP_ERR_T        enIpErr )
    {
        return;
    }
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
