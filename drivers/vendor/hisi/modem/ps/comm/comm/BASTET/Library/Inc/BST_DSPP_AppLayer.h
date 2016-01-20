

#ifndef __BST_DSPP_APPLAYER_H__
#define __BST_DSPP_APPLAYER_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "BST_DSPP_LayerProcBase.h"
#include "BST_DSPP_Define.h"
#include "BST_SRV_TaskMng.h"
#include "BST_APP_Define.h"

#if(BST_OS_VER == BST_HISI_VOS)
#pragma pack(4)
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BST_DSPP_AP_BYTE2PID( byte0, byte1 )    (( BST_CORE_PID_ENUM_UINT16 )\
                                                BST_DSPP_U8_TO_U16( byte0, byte1 ))

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 枚举定义
*****************************************************************************/
enum
{
    BST_DSPP_AP_CMD_LEN_L               = 0x00,
    BST_DSPP_AP_CMD_LEN_H,
    BST_DSPP_AP_CMD_ID_L,
    BST_DSPP_AP_CMD_ID_H,
    BST_DSPP_AP_CMD_HEAD_LEN
};
/*****************************************************************************
  5 类/结构定义
*****************************************************************************/

class BST_DSPP_CAppDlVa :
    BST_PUBLIC BST_DSPP_CDlLayerBase
{
BST_PUBLIC:
    BST_ERR_ENUM_UINT8          UnpackInternal(
        BST_DSPP_HEAD_STRU         *const pstRxHeadInfo,
        BST_DSPP_VERION_ENUM_UINT8 *const pucNextVer );

    static BST_DSPP_CAppDlVa   *GetInstance( BST_VOID );

    BST_DSPP_CDlLayerBase      *GetNextStrategy
        ( BST_DSPP_VERION_ENUM_UINT8    enNextVer );
BST_PRIVATE:
    BST_DSPP_CAppDlVa( BST_VOID );

   ~BST_DSPP_CAppDlVa( BST_VOID );

    BST_ERR_ENUM_UINT8          Inquire(
        BST_DSPP_OBJTADDR_T     usObjtAddr,
        BST_DSPP_OFSTADDR_T     usOfstAddr,
        BST_DSPP_HEAD_STRU     *const pstRxHeadInfo );

    BST_ERR_ENUM_UINT8          Config(
        BST_DSPP_OBJTADDR_T     usObjtAddr,
        BST_DSPP_OFSTADDR_T     usOfstAddr,
        BST_DSPP_HEAD_STRU     *const pstRxHeadInfo );

    BST_ERR_ENUM_UINT8          Add(
        BST_DSPP_HEAD_STRU     *const pstRxHeadInfo,
        BST_DSPP_OFSTADDR_T    *const pusOfstAddr,
        BST_DSPP_OBJTADDR_T     usObjtAddr );

    BST_ERR_ENUM_UINT8          Remove(
        BST_DSPP_OBJTADDR_T     usObjtAddr,
        BST_DSPP_OFSTADDR_T     usOfstAddr );

    BST_ERR_ENUM_UINT8          ParseHeadInfo(
        BST_DSPP_HEAD_STRU     *const pstHead,
        BST_UINT8 const        *pucData,
        BST_UINT16              usLength );

    BST_UINT16                  ReadOut(
        BST_DSPP_OBJTADDR_T     usObjtAddr,
        BST_DSPP_OFSTADDR_T     usOfstAddr,
        BST_UINT8              *pucSrcBuff,
        BST_UINT8              *pucDstBuff,
        BST_UINT16              usSrcLen,
        BST_UINT16              usDstLen );

    BST_ERR_ENUM_UINT8          ProcAction(
        BST_DSPP_HEAD_STRU     *const pstRxHeadInfo,
        BST_DSPP_OFSTADDR_T    *pusOfstAddr,
        BST_DSPP_OBJTADDR_T     usObjtAddr );

    BST_SRV_CTaskMng                   *m_pcTaskManager;    
};

class BST_DSPP_CAppUlVa :
    BST_PUBLIC BST_DSPP_CUlLayerBase
{
BST_PUBLIC:
    static BST_DSPP_CAppUlVa   *GetInstance( BST_VOID );

    BST_ERR_ENUM_UINT8          Response(
        BST_DSPP_HEAD_STRU     *const pstOrigHeadInfo,
        BST_ACT_ENUM_UINT8      ucErrMsg );

    BST_ERR_ENUM_UINT8          Report(
        BST_DSPP_OBJTADDR_T         usObjtAddr, 
        BST_DSPP_OFSTADDR_T         usOfstAddr, 
        BST_DSPP_CMDLEN_T           usCmdLen, 
        BST_CORE_PID_ENUM_UINT16    enCmdId, 
        BST_UINT8                  *pucCmdData );

    BST_ERR_ENUM_UINT8          Report(
        BST_DSPP_OBJTADDR_T         usObjtAddr,
        BST_DSPP_OFSTADDR_T         usOfstAddr,
        BST_ERR_ENUM_UINT8          enErr );

BST_PROTECTED:                          
    BST_DSPP_CAppUlVa( BST_VOID );

   ~BST_DSPP_CAppUlVa( BST_VOID );

    BST_ERR_ENUM_UINT8          PackInternal(
        BST_DSPP_HEAD_STRU           *const pstTxHeadInfo,
        BST_DSPP_VERION_ENUM_UINT8   *const pucNextVer );

    BST_DSPP_CUlLayerBase      *GetNextStrategy(
        BST_DSPP_VERION_ENUM_UINT8    ucNextVer );
};



#if(BST_OS_VER == BST_HISI_VOS)
#pragma pack()
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(pop)
#endif


#endif


