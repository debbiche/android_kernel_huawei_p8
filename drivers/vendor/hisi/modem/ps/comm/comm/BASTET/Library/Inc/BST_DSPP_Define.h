
#ifndef __BST_DSPP_DEFINE_H__
#define __BST_DSPP_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "BST_Platform.h"
#include "BST_LIB_List.h"

#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack(4)
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BST_DSPP_IsObjIdValid( ProcId ) ( ( (ProcId) > BST_APP_TYPE_MIN ) && ( (ProcId) < BST_OBJ_TYPE_MAX ) )
#define BST_DSPP_IsVersionValid(VerId)  ( ( (VerId) > BST_DSPP_VER_INVALID ) && ( (VerId) < BST_DSPP_VER_BUTT ) )
#define BST_DSPP_U8_TO_U16( u8L, u8H )  ( ( (u8L) & 0xff ) + ( ( (u8H)<<8 ) & 0xff00 ) )
#define BST_DSPP_U16_TO_U8H( u16_data ) ( BST_UINT8 )( ( (u16_data)>>8 ) & 0xff )
#define BST_DSPP_U16_TO_U8L( u16_data ) ( BST_UINT8 )( ( (u16_data) ) & 0xff )
#define BST_DSPP_INVALID_ADDR           ( BST_INVALID_SNID )
/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum BST_DSPP_VERION_ENUM
{
    BST_DSPP_VER_INVALID                = 0,
    BST_DSPP_VER_A                      = 1,
    BST_DSPP_VER_B,
    BST_DSPP_VER_C,
    BST_DSPP_VER_VOID,
    BST_DSPP_VER_BUTT
};
typedef BST_UINT8                       BST_DSPP_VERION_ENUM_UINT8;

enum BST_DSPP_LAYER_ID_ENUM
{
    BST_DSPP_LAYER_INVALID              = 0,
    BST_DSPP_LAYER_TRS                  = 1,
    BST_DSPP_LAYER_CTR,
    BST_DSPP_LAYER_APP,
    BST_DSPP_LAYER_BUTT
};
typedef BST_UINT8                       BST_DSPP_LAYER_ID_ENUM_UINT8;

enum BST_DSPP_PRIMITIVE_ENUM
{
    BST_DSPP_PRMTV_INVALID              = 0,
    BST_DSPP_PRMTV_REQUEST              = 1,
    BST_DSPP_PRMTV_RESPONSE,
    BST_DSPP_PRMTV_REPORT,
    BST_DSPP_PRMTV_CONFIRM,
    BST_DSPP_PRMTV_ACK
};
typedef BST_UINT8                       BST_DSPP_PRIMITIVE_ENUM_UINT8;
/*****************************************************************************
  4 类型定义
*****************************************************************************/
typedef BST_UINT16                      BST_DSPP_OFSTADDR_T;
typedef BST_UINT16                      BST_DSPP_OBJTADDR_T;
typedef BST_UINT16                      BST_DSPP_PKTSN_T;
typedef BST_UINT16                      BST_DSPP_PKTPC_T;
typedef BST_UINT8                       BST_DSPP_PRMTV_T;
typedef BST_UINT16                      BST_DSPP_CMDLEN_T;

/*****************************************************************************
  5 数据结构定义
*****************************************************************************/
typedef struct
{
    NODE                                node;
    BST_UINT8                          *pucData;
    BST_UINT16                          usLength;
    BST_DSPP_PKTPC_T                    usPicNum;
} BST_DSPP_PIC_STRU;

typedef struct
{
BST_PUBLIC:
    BST_DSPP_VERION_ENUM_UINT8          enTpVer;
    BST_DSPP_VERION_ENUM_UINT8          enCpVer;
    BST_DSPP_VERION_ENUM_UINT8          enApVer;
    BST_DSPP_PRIMITIVE_ENUM_UINT8       enPrmvType;
    BST_UINT16                          usLength;
    BST_DSPP_PKTSN_T                    usPktNum;
    BST_UINT32                          ulZeroCopy;
    BST_UINT8                          *pucData;
    union
    {
        struct
        {
            BST_ACT_ENUM_UINT8          ucActType;
            BST_DSPP_OBJTADDR_T         usObjtAddr;
            BST_DSPP_OFSTADDR_T         usOfstAddr;
            BST_UINT8                   aucReserved[2];
        } stCpVerA;
        struct
        {
            BST_ACT_ENUM_UINT8          ucActType;
            BST_DSPP_OBJTADDR_T         usObjtAddr;
            BST_DSPP_OFSTADDR_T         usOfstAddr;
            BST_UINT8                   aucReserved[2];
        } stCpVerB;
    };
} BST_DSPP_HEAD_STRU;

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

