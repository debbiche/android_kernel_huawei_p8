

#ifndef __BST_DSPP_CTRLAYER_H__
#define __BST_DSPP_CTRLAYER_H__

#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack(4)
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(push, 4)
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "BST_DSPP_LayerProcBase.h"
#include "BST_DSPP_Define.h"
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BST_DSPP_OFFSET_ADDR_MIN        ( 0x0001 )
#define BST_DSPP_OFFSET_ADDR_MAX        ( BST_DSPP_OFFSET_ADDR_MIN + BST_MAX_APP_NUMBER )
#define BST_DSPP_BROADCASE_ADDRESS      ( 0x0000 )

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 枚举定义
*****************************************************************************/
enum
{
    BST_DSPP_CP_OBJT_ADD_L              = 0x00,
    BST_DSPP_CP_OBJT_ADD_H,
    BST_DSPP_CP_OFST_ADD_L,
    BST_DSPP_CP_OFST_ADD_H,
    BST_DSPP_CP_OPRT_ERR,
    BST_DSPP_CP_APL_VER,
    BST_DSPP_CP_HEAD_LEN
};
/*****************************************************************************
  5 STRUCT定义
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


/*****************************************************************************
  9 类定义
*****************************************************************************/
class BST_DSPP_CCtrDl
    : BST_PUBLIC BST_DSPP_CDlLayerBase
{
BST_PUBLIC:
    BST_DSPP_CCtrDl(
        BST_DSPP_LAYER_ID_ENUM_UINT8  enLayerId,
        BST_DSPP_VERION_ENUM_UINT8    enLayerVer );

   ~BST_DSPP_CCtrDl( BST_VOID );

BST_PROTECTED:
    BST_ERR_ENUM_UINT8          GetActionErrorType(
        BST_UINT16              usLength,
        BST_UINT8 const        *pucData );

    BST_DSPP_OBJTADDR_T         GetObjType(
        BST_UINT16              usLength,
        BST_UINT8 const        *pucData );

    BST_DSPP_OFSTADDR_T         GetOfseAdd(
        BST_UINT16              usLength,
        BST_UINT8 const        *pucData );

    BST_DSPP_VERION_ENUM_UINT8  GetAPver(
        BST_UINT16              usLength,
        BST_UINT8 const        *pucData );

    BST_VOID                    SetAPver(
        BST_DSPP_VERION_ENUM_UINT8    enValue,
        BST_UINT16                    usLength,
        BST_UINT8 const              *pucData );

    BST_VOID                    SetObjType(
        BST_DSPP_OBJTADDR_T           enValue,
        BST_UINT16                    usLength,
        BST_UINT8 const              *pucData );


    BST_VOID                    SetOfseAdd(
         BST_DSPP_OFSTADDR_T           enValue,
         BST_UINT16                    usLength,
         BST_UINT8 const              *pucData );

    BST_VOID                    SetActionErrorType(
        BST_ACT_ERR_ENUM_UINT8        enValue,
        BST_UINT16                    usLength,
        BST_UINT8 const              *pucData );

};

class BST_DSPP_CCtrDlVa
    : BST_PUBLIC BST_DSPP_CCtrDl
{
BST_PUBLIC:
    BST_DSPP_CCtrDlVa   ( BST_VOID );
   ~BST_DSPP_CCtrDlVa   ( BST_VOID );

BST_PRIVATE:
    BST_ERR_ENUM_UINT8      ParseHeadInfo(
        BST_DSPP_HEAD_STRU           *const pstHead,
        BST_UINT8 const              *pucData,
        BST_UINT16                    usLength );

    BST_ERR_ENUM_UINT8      UnpackInternal(
        BST_DSPP_HEAD_STRU           *const pstRxHeadInfo,
        BST_DSPP_VERION_ENUM_UINT8   *const penNextVer );

    BST_DSPP_CDlLayerBase  *GetNextStrategy(
        BST_DSPP_VERION_ENUM_UINT8    enNextVer );
};

class BST_DSPP_CCtrUlVa
    : BST_PUBLIC BST_DSPP_CUlLayerBase
{
BST_PUBLIC:
    BST_DSPP_CCtrUlVa   ( BST_VOID );
   ~BST_DSPP_CCtrUlVa   ( BST_VOID );

BST_PROTECTED:
    BST_ERR_ENUM_UINT8      CheckObjtAdd(
        BST_DSPP_OBJTADDR_T usObjtAddr );

    BST_ERR_ENUM_UINT8      CheckOfstAdd(
        BST_DSPP_OFSTADDR_T usOfstAddr );

    BST_ERR_ENUM_UINT8      PackInternal(
        BST_DSPP_HEAD_STRU         *const pstTxHeadInfo,
        BST_DSPP_VERION_ENUM_UINT8 *const penNextVer );

    BST_DSPP_CUlLayerBase  *GetNextStrategy(
        BST_DSPP_VERION_ENUM_UINT8  enNextVer );
};


#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack()
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(pop)
#endif



#endif //CONTROL_LAYER_H

