

#ifndef __BST_CORE_REGISTRYDEFINE_H__
#define __BST_CORE_REGISTRYDEFINE_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "BST_Platform.h"

#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack(4)
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BST_CORE_REGTABL_INIT                                                                                           \
    { BST_PID_INVALID,                  BST_CORE_PID_FIX_LEN,   0                                                   },  \
    { BST_PID_FEATURE_SUPPORT,          BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF(BST_UINT32)                           },  \
    { BST_PID_NET_STATE,                BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF(BST_NET_STATE_STRU)                   },  \
    { BST_PID_TIMESTAMP,                BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF(BST_DRV_NET_TIME_CLBRT_STRU)          },  \
    { BST_PID_SOCKET_INFO,              BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF(BST_IP_SOCKET_ADD_T)                  },  \
    { BST_PID_PERIODIC,                 BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF(BST_UINT32)                           },  \
    { BST_PID_PROTOCOL_TYPE,            BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF(BST_PRTC_TYPE_ENUM_UINT32)            },  \
    { BST_PID_SOCKET_PROPERTY,          BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF( BST_IP_SKT_PROPTY_STRU )             },  \
    { BST_PID_CIPHER_MODE,              BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF(BST_APP_ENCRP_MODE_ENUM_UINT32)       },  \
    { BST_PID_CIPHER_CODE,              BST_CORE_PID_VAR_LEN,   0                                                   },  \
    { BST_PID_TRAFFIC_FLOW,             BST_CORE_PID_VAR_LEN,   0                                                   },  \
    { BST_PID_TRAFFIC_LIMITE,           BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF( BST_UINT32 )                         },  \
    { BST_PID_MODEM_RAB_ID,             BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF( BST_DRV_NET_MODEM_RABID_STRU )       },  \
    /*Email*/                                                                                                           \
    { BST_PID_EMAIL_PROTOCOL_TYPE,      BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF( MAIL_BOX_TYPE_ENUM_UINT32 )          },  \
    { BST_PID_EMAIL_SYNC_TYPE,          BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF( SYNC_TYPE_ENUM_UINT32)               },  \
    { BST_PID_EMAIL_ACCOUNT,            BST_CORE_PID_VAR_LEN,   0                                                   },  \
    { BST_PID_EMAIL_PASSWORD,           BST_CORE_PID_VAR_LEN,   0                                                   },  \
    { BST_PID_EMAIL_LAST_UID,           BST_CORE_PID_VAR_LEN,   0                                                   },  \
    { BST_PID_EMAIL_DEVICE_ID,          BST_CORE_PID_VAR_LEN,   0                                                   },  \
    { BST_PID_EMAIL_EXCG_HTTP_INFO,     BST_CORE_PID_VAR_LEN,   0                                                   },  \
    { BST_PID_IMAP_ID_CMD,              BST_CORE_PID_VAR_LEN,   0                                                   },  \
    { BST_PID_EMAIL_EXCG_WXML_INFO,      BST_CORE_PID_VAR_LEN,  0                                                   },  \
                                                                                                                        \
    /*Heartbeat*/                                                                                                       \
    { BST_PID_HB_SEND_CONTENT,          BST_CORE_PID_VAR_LEN,   0                                                   },  \
    { BST_PID_HB_RESP_CONTENT,          BST_CORE_PID_VAR_LEN,   0                                                   },  \
    { BST_PID_HB_SERIAL_NUM,            BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF(SERNO_HB_INFO_STRU)                   },  \
    { BST_PID_HB_CHECK_TYPE,            BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF(BST_APP_HB_CHECK_TYPE_ENUM_UINT32)    },  \
    { BST_PID_HB_TX_RETRY_MAX,          BST_CORE_PID_FIX_LEN,   BST_OS_SIZEOF( BST_HB_RETRY_INFO )                  }


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum BST_PARAM_ID_ENUM
{
    BST_PID_INVALID                     = 0,
    BST_PID_FEATURE_SUPPORT,            /*1 */
    BST_PID_NET_STATE,                  /*2 */
    BST_PID_TIMESTAMP,                  /*3 */
    BST_PID_SOCKET_INFO,                /*4 */
    BST_PID_PERIODIC,                   /*5 */
    BST_PID_PROTOCOL_TYPE,              /*6 */
    BST_PID_SOCKET_PROPERTY,            /*7 */
    BST_PID_CIPHER_MODE,                /*8 */
    BST_PID_CIPHER_CODE,                /*9 */
    BST_PID_TRAFFIC_FLOW,               /*10*/
    BST_PID_TRAFFIC_LIMITE,             /*11*/
    BST_PID_MODEM_RAB_ID,               /*12*/

    /*email*/
    BST_PID_EMAIL_PROTOCOL_TYPE,        /*13*/
    BST_PID_EMAIL_SYNC_TYPE,            /*14*/
    BST_PID_EMAIL_ACCOUNT,              /*15*/
    BST_PID_EMAIL_PASSWORD,             /*16*/
    BST_PID_EMAIL_LAST_UID,             /*17*/
    BST_PID_EMAIL_DEVICE_ID,            /*18*/
    BST_PID_EMAIL_EXCG_HTTP_INFO,       /*19*/
    BST_PID_IMAP_ID_CMD,                /*20*/
    BST_PID_EMAIL_EXCG_WXML_INFO,       /*21*/

    /*heartbeat*/
    BST_PID_HB_SEND_CONTENT,            /*22*/
    BST_PID_HB_RESP_CONTENT,            /*23*/
    BST_PID_HB_SERIAL_NUM,              /*24*/
    BST_PID_HB_CHECK_TYPE,              /*25*/
    BST_PID_HB_TX_RETRY_MAX,            /*26*/
    BST_PID_DEF_BUTT
};
typedef BST_UINT16                      BST_CORE_PID_ENUM_UINT16;

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack()
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(pop)
#endif

#endif
