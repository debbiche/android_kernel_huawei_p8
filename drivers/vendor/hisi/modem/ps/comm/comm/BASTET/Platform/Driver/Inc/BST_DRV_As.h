
#ifndef __BST_DRV_AS_H__
#define __BST_DRV_AS_H__

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
#include "BST_Platform.h"
#include "BST_DRV_Common.h"
#include "BST_SRV_Define.h"

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BST_DRV_AsGetDvcHandle()        ( &g_BstDrvAsHandle )
#define BST_DRV_AS_INVALID_CTRLPARA     ( 0 )
/*查询当前是否存在RRC连接*/
#define BST_DRV_AsIsRrcExisted()        ( BST_TRUE == g_ulBstDrvRrcExistFlg )

/*查询当前是否允许向AP上报QMI/ICC消息（避免AP唤醒）*/


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 枚举定义
*****************************************************************************/
enum{
    BST_MODEM_ID_1                      = 0U,
    BST_MODEM_ID_2,
    BST_MODEM_MAX
};
typedef BST_UINT32                      BST_MODEM_ID_ENUM_UINT32;

enum{
    BST_SIGLE_SIM_MODE                  = 0U,
    BST_DUAL_DSDS_SIM_MODE,
    BST_DUAL_DSDA_SIM_MODE,
    BST_INVALID_SIM_MODE
};
typedef BST_UINT32                      BST_MODEM_SIM_MODE_ENUM_UINT32;

/*****************************************************************************
  5 类/结构定义
*****************************************************************************/
typedef struct{
    BST_AS_EVT_ENUM_UINT32              ulId;
    BST_UINT32                          ulPara;
}BST_DRV_AS_EVENT_STRU;

typedef struct{
    BST_MODEM_SIM_MODE_ENUM_UINT32      ulSimMode;
    BST_BOOL                            bCSConnected[BST_MODEM_MAX];
    BST_UINT16                          usReserved;
}BST_DRV_AS_CS_EVENT_STRU;

/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 全局变量声明
*****************************************************************************/
extern BST_DRV_STRU                     g_BstDrvAsHandle;
extern BST_UINT32                         g_ulBstDrvRrcExistFlg;
extern BST_BOOL                         g_BstReportFlag;
/*****************************************************************************
  8 函数声明
*****************************************************************************/
BST_VOID    BST_DRV_AsSetReportFlag( BST_UINT8 ucReportFlg );
BST_VOID    BST_DRV_As_CsEventInd( BST_DRV_AS_CS_EVENT_STRU *pstEvent );
BST_VOID    BST_DRV_As_PsSrvStateChangeInd(BST_BOOL bInservice );
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
