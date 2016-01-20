

#ifndef __BST_SRV_DEFINE_H__
#define __BST_SRV_DEFINE_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "BST_Platform.h"

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
  2 宏定义
*****************************************************************************/
#define BST_SRV_EVENT_HEAD              BST_SRV_EVT_ENUM_UINT32     enId;\
                                        BST_UINT32                  ulLength;\
                                        BST_UINT8                  *pucData;
#define BST_SRV_IsValidAsEvent(enEvt)   ( ((enEvt) > BST_AS_EVT_NONE)&&((enEvt) < BST_AS_EVT_MAX) )

#define BST_AS_RAT_NONE                 ( 0x00U )
#define BST_AS_RAT_GPRS                 ( 0x01U )
#define BST_AS_RAT_CDMA                 ( 0x02U )
#define BST_AS_RAT_EVDO                 ( 0x04U )
#define BST_AS_RAT_WCDMA                ( 0x08U )
#define BST_AS_RAT_TDSCDMA              ( 0x10U )
#define BST_AS_RAT_LTE                  ( 0x20U )
#define BST_AS_IsRatSupport(ulRat)      ( 0 != ( (ulRat) & BST_AS_RAT_SUPPORT_FLAGSET ) )
#define BST_AS_IsRatPppMode(ulRat)      ( 0 != ( (ulRat) & BST_IP_PPP_RAT_FLAGSET ) )

#define BST_AS_UNBLOCKED                ( 0x00U )
#define BST_AS_BLOCKED_BY_NO_SERVICE    ( 0x01U )
#define BST_AS_BLOCKED_BY_CS_CONFLICT   ( 0x02U )

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 枚举定义
*****************************************************************************/
enum BST_RRC_STATE_ENUM
{
    BST_AS_RRC_STATE_INVALID            = 0,
    BST_AS_RRC_CONNECTED,
    BST_AS_RRC_RELEASED
};
typedef BST_UINT32                      BST_AS_RRC_STATE_ENUM_UINT32;

enum BST_AS_EVT_ENUM
{
    BST_AS_EVT_NONE                     = 0U,
    BST_AS_EVT_L3_RRC_STATE,
    BST_AS_EVT_L3_AP_TRS_RCV_IND,
    BST_AS_EVT_L3_RAT_STATE,
    BST_AS_EVT_L3_SERVICE_STATE,
    BST_AS_EVT_L3_CS_EVENT_STATE,
    BST_AS_EVT_L3_BLOCK_INFO,
    BST_AS_EVT_MAX                      = 30U
};
typedef BST_UINT32                      BST_AS_EVT_ENUM_UINT32;

enum BST_SRV_EVT_ENUM
{
    BST_SRV_AS_EVENT                    = 1,
    BST_SRV_ACOM_EVENT,
    BST_SRV_TIMER_EVENT,
    BST_SRV_TCPIP_EVENT
};
typedef BST_UINT32                      BST_SRV_EVT_ENUM_UINT32;
typedef BST_UINT32                      BST_AS_RAT_MODE_UINT32;
/*****************************************************************************
  5 类/结构定义
*****************************************************************************/
typedef struct
{
    BST_SRV_EVENT_HEAD
}BST_EVT_HEAD_STRU;

typedef struct
{
    BST_SRV_EVENT_HEAD
    BST_AS_EVT_ENUM_UINT32              enAsEvent;
}BST_AS_EVT_STRU;

typedef struct
{
    BST_SRV_EVENT_HEAD
}BST_ACOM_EVT_STRU;

typedef struct
{
    BST_SRV_EVENT_HEAD
}BST_TIMER_EVT_STRU;

typedef struct
{
    BST_SRV_EVENT_HEAD
}BST_NET_EVENT_STRU;

typedef struct
{
    BST_BOOL    bCsService;
    BST_BOOL    bPsService;
    BST_UINT16  usReserved;
}BST_SRV_UTRAN_SRV_STATE_STRU;

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


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



#endif
