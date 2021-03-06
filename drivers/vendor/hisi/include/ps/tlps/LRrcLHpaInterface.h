

#ifndef __LRRCLHPAINTERFACE_H__
#define __LRRCLHPAINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "LPSCommon.h"

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 macro
*****************************************************************************/

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

#define LRRC_LHPA_MSG_HDR                  (PS_MSG_ID_LHPA_TO_RRC_BASE)


/*****************************************************************************
  4 Enum
*****************************************************************************/
/*****************************************************************************
 枚举名    : LRRC_LHPA_MSG_ID_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : HPA与RRC的消息原语类型
*****************************************************************************/
enum LRRC_LHPA_MSG_ID_ENUM
{
    /* HPA发给RRC的错误指示原语 */
    ID_LRRC_LHPA_ERROR_IND                          = (LRRC_LHPA_MSG_HDR + 0x00),   /* _H2ASN_MsgChoice LRRC_LHPA_ERROR_IND_STRU */
    ID_LRRC_LHPA_MSG_ID_BUTT
};
typedef VOS_UINT32   LRRC_LHPA_MSG_ID_ENUM_UINT32;

/*****************************************************************************
 枚举名    : LRRC_LHPA_ERR_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 从邮箱错误指示的消息类型
*****************************************************************************/
enum LRRC_LHPA_ERR_TYPE_ENUM
{
    /* 关键信息丢失（除空闲态和连接态测量IND之外的其他原语）,
        当关键信息丢失时协议栈需要执行释放操作，通过LRRC_LPHY_REL_ALL_ REQ
        或LRRC_LPHY_SET_WORK_MODE_REQ（切从模）实现。
        非关键信息丢失不需要释放*/
    LRRC_LHPA_ERR_TYPE_SLAV_UP_OK                   = 0x0,
    LRRC_LHPA_ERR_TYPE_SLAV_UP_CRITICAL_MSG_LOST    = 0x1,  /* 关键信息丢失，（除空闲态和连接态测量IND之外的其他原语） */
    LRRC_LHPA_ERR_TYPE_SLAV_UP_NONCRITICAL_MSG_LOST = 0x2,  /* 非关键信息丢失，（空闲态和连接态测量IND） */
    LRRC_LHPA_ERR_TYPE_BUTT
};

typedef VOS_UINT16 LRRC_LHPA_ERR_TYPE_ENUM_UINT16;

/*****************************************************************************
 结构名称   : LRRC_LHPA_ERROR_IND_STRU
 协议表格   :
 ASN.1 描述 :
 结构说明   : LHPA对LRRC的上行邮箱错误指示
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    LRRC_LHPA_MSG_ID_ENUM_UINT32            enMsgId;        /*_H2ASN_Skip*/
    LRRC_LHPA_ERR_TYPE_ENUM_UINT16          enErrType;
    VOS_UINT16                              usReserved;
} LRRC_LHPA_ERROR_IND_STRU;
/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/

/*****************************************************************************
  9 OTHERS
*****************************************************************************/

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of 发送模块+接收模块+Interface.h */


