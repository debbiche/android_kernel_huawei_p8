

#ifndef __RCMPHYINTERFACE_H__
#define __RCMPHYINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "product_config.h"

#ifndef LPS_RTT
#include "vos.h"
#include "phyoaminterface.h"
#else
#define VOS_MSG_HEADER
#endif

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*最多支持的RF数目*/
#define RCM_RF_SUPPORT_NUM                      ( RCM_RF_ID_BUTT )

/*最多支持的PHY mode数目*/
#define RCM_MODE_SUPPORT_NUM                    ( RCM_MODE_ID_BUTT )

/*最多支持的MODEM数目*/
#define RCM_MODEM_SUPPORT_NUM                   ( RCM_MODEM_ID_BUTT )

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 枚举名    : RCM_PHY_MSG_ID_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : RCM <-> PHY 接口消息ID
*****************************************************************************/
enum RCM_PHY_MSG_ID_ENUM
{
    ID_PHY_RCM_START_TAS_CNF                = 0x180e,                           /* _H2ASN_MsgChoice PHY_RCM_START_TAS_CNF_STRU */
    ID_PHY_RCM_STOP_TAS_CNF                 = 0x180f,                           /* _H2ASN_MsgChoice PHY_RCM_STOP_TAS_CNF_STRU */
    ID_PHY_RCM_SET_DPDT_CNF                 = 0x1810,                           /* _H2ASN_MsgChoice PHY_RCM_SET_DPDT_CNF_STRU */
    ID_PHY_RCM_TX_BLANKING_IND              = 0x1811,                           /* _H2ASN_MsgChoice PHY_RCM_TX_BLANKING_IND_STRU */
    ID_PHY_RCM_RATMODE_IND                  = 0x1812,                           /* _H2ASN_MsgChoice PHY_RCM_RATMODE_IND_STRU */
    ID_MSP_RCM_MAILBOX_READ_IND             = 0x1813,                           /* _H2ASN_MsgChoice RCM_PHY_MSG_HEADER_STRU */
    ID_PHY_RCM_ACCESS_STATE_IND             = 0x1814,                           /* _H2ASN_MsgChoice PHY_RCM_ACCESS_STATE_IND_STRU */
    ID_PHY_RCM_SEARCHING_STATE_IND          = 0x1815,                           /* _H2ASN_MsgChoice PHY_RCM_SEARCHING_STATE_IND_STRU */
    ID_PHY_RCM_SIGNAL_STATE_IND             = 0x1816,                           /* _H2ASN_MsgChoice PHY_RCM_SIGNAL_STATE_IND_STRU */
    ID_PHY_RCM_IDLE_HAPPY_IND               = 0x1817,                           /* _H2ASN_MsgChoice PHY_RCM_IDLE_HAPPY_IND_STRU */
                                                                                /* 0x1818已经分配给MTC -> RCM message使用 */
    ID_PHY_RCM_VOICE_CALL_STATE_IND         = 0x1819,                           /* _H2ASN_MsgChoice PHY_RCM_VOICE_CALL_STATE_IND_STRU */
    ID_PHY_RCM_SEARCHING_STATE_REQ          = 0x181a,                           /* _H2ASN_MsgChoice PHY_RCM_SEARCHING_STATE_REQ_STRU */
                                                                                /* 0x181b,0x181c已经分配给CMMCA -> RCM message使用 */
    ID_PHY_RCM_AGENT_SET_DPDT_CNF           = 0x181d,                           /* _H2ASN_MsgChoice PHY_RCM_AGENTC_SET_DPDT_CNF_STRU */

    ID_RCM_PHY_START_TAS_REQ                = 0x188e,                           /* _H2ASN_MsgChoice RCM_PHY_START_TAS_REQ_STRU */
    ID_RCM_PHY_STOP_TAS_REQ                 = 0x188f,                           /* _H2ASN_MsgChoice RCM_PHY_STOP_TAS_REQ_STRU */
    ID_RCM_PHY_SET_DPDT_REQ                 = 0x1890,                           /* _H2ASN_MsgChoice RCM_PHY_SET_DPDT_REQ_STRU */
    ID_RCM_PHY_TX_BLANKING_IND              = 0x1891,                           /* _H2ASN_MsgChoice RCM_PHY_TX_BLANKING_IND */
    ID_RCM_PHY_SEARCHING_STATE_CNF          = 0x1892,                           /* _H2ASN_MsgChoice RCM_PHY_SEARCHING_STATE_CNF_STRU */
    ID_RCM_PHY_AGENT_SET_DPDT_REQ           = 0x1893,                           /* _H2ASN_MsgChoice RCM_PHY_AGENTC_SET_DPDT_REQ_STRU */

    ID_RCM_PHY_MSG_BUTT

};
typedef VOS_UINT16 RCM_PHY_MSG_ID_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_MODE_ID_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 物理层模式标示 排列次序和校准次序一致
*****************************************************************************/
enum RCM_MODE_ID_ENUM
{
    RCM_LTE                             = 0,                                    /* LTE */
    RCM_WCDMA                           = 1,                                    /* WCDMA */
    RCM_1X                              = 2,                                    /* 1X */
    RCM_HRPD                            = 3,                                    /* HRPD */
    RCM_TD                              = 4,                                    /* TD */
    RCM_G0                              = 5,                                    /* G0 */
    RCM_G1                              = 6,                                    /* G1 */
    RCM_G2                              = 7,                                    /* G2 */
    RCM_MODE_ID_BUTT
};
typedef VOS_UINT16  RCM_MODE_ID_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_MODEM_ID_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : MODEM ID
*****************************************************************************/
enum RCM_MODEM_ID_ENUM
{
    RCM_MODEM_1                         = 0,                                    /* MODEM0 */
    RCM_MODEM_2                         = 1,                                    /* MODEM1 */
    RCM_MODEM_3                         = 2,                                    /* MODEM2 */
    RCM_MODEM_ID_BUTT
};
typedef VOS_UINT16  RCM_MODEM_ID_ENUM_UINT16;


/*****************************************************************************
 枚举名    : RCM_STOP_TAS_TYPE_ENUM_UINT16
 枚举说明  : RCM取消物理层TAS授权方式
             0:关闭TAS，将天线切回PHY默认态
             1:暂停TAS，将天线保持在当前状态
*****************************************************************************/
enum RCM_STOP_TAS_TYPE_ENUM
{
    RCM_STOP_TAS_TYPE_STOP                = 0,
    RCM_STOP_TAS_TYPE_KEEP                = 1,
    RCM_STOP_TAS_TYPE_BUTT
};
typedef VOS_UINT16 RCM_STOP_TAS_TYPE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TAS_CONNECT_STATE_ENUM_UINT16
 枚举说明  : modem业务连接状态
             0:非连接状态
             1:连接状态
*****************************************************************************/
enum RCM_TAS_CONNECT_STATE_ENUM
{
    RCM_TAS_CONNECT_NO_EXIST             = 0,
    RCM_TAS_CONNECT_EXIST                = 1,
    RCM_TAS_CONNECT_BUTT
};
typedef VOS_UINT16 RCM_TAS_CONNECT_STATE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TAS_POWER_STATE_ENUM_UINT16
 枚举说明  : modem_power上下电状态即是否处于飞行模式
             0:下电即飞行模式
             1:上电即非飞行模式
*****************************************************************************/
enum RCM_TAS_POWER_STATE_ENUM
{
    RCM_TAS_POWER_STATE_OFF           = 0,
    RCM_TAS_POWER_STATE_ON            = 1,
    RCM_TAS_POWER_STATE_BUTT
};
typedef VOS_UINT16 RCM_TAS_POWER_STATE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_ACCESS_STATE_ENUM_UINT16
 枚举说明  : modem接入状态
             0:非接入状态
             1:接入状态
*****************************************************************************/
enum RCM_ACCESS_STATE_ENUM
{
    RCM_ACCESS_STATE_NON_ACCESS   = 0,
    RCM_ACCESS_STATE_ACCESS       = 1,
    RCM_ACCESS_STATE_UNHAPPY      = 2,
    RCM_ACCESS_STATE_BUTT
};
typedef VOS_UINT16 RCM_ACCESS_STATE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_SEARCHING_STATE_ENUM_UINT16
 枚举说明  : modem搜寻状态
             0:非搜寻状态
             1:搜寻状态
*****************************************************************************/
enum RCM_SEARCHING_STATE_ENUM
{
    RCM_SEARCHING_STATE_NON_SEARCH    = 0,
    RCM_SEARCHING_STATE_SEARCH        = 1,
    RCM_SEARCHING_STATE_BUTT
};
typedef VOS_UINT16 RCM_SEARCHING_STATE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_SIGNAL_STATE_ENUM_UINT16
 枚举说明  : modem讯号状态
             0:讯号POOR状态
             1:讯号GOOD状态
*****************************************************************************/
enum RCM_SIGNAL_STATE_ENUM
{
    RCM_SIGNAL_STATE_POOR         = 0,
    RCM_SIGNAL_STATE_GOOD         = 1,
    RCM_SIGNAL_STATE_BUTT
};
typedef VOS_UINT16 RCM_SIGNAL_STATE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_IDLE_HAPPY_ENUM_UINT16
 枚举说明  : modem Idle Happy状态
             0:Idle Unhappy状态
             1:Idle Happy状态
*****************************************************************************/
enum RCM_IDLE_HAPPY_ENUM
{
    RCM_IDLE_HAPPY_UNHAPPY        = 0,
    RCM_IDLE_HAPPY_HAPPY          = 1,
    RCM_IDLE_HAPPY_BUTT
};
typedef VOS_UINT16 RCM_IDLE_HAPPY_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_VOICE_CALL_STATE_ENUM_UINT16
 枚举说明  : modem Background or Active Call状态
             0:Active Call状态
             1:Background Call状态
*****************************************************************************/
enum RCM_VOICE_CALL_STATE_ENUM
{
    RCM_VOICE_CALL_ACTIVE           = 0,
    RCM_VOICE_CALL_BACKGROUND       = 1,
    RCM_VOICE_CALL_BUTT
};
typedef VOS_UINT16 RCM_VOICE_CALL_STATE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TAS_DPDT_CONTROL_ENUM_UINT16
 枚举说明  : DPDT命令对应控制模式
             0:直通
             1:交换
*****************************************************************************/
enum RCM_TAS_DPDT_CONTROL_ENUM
{
    RCM_TAS_DPDT_PASS_THROUGH         = 0,
    RCM_TAS_DPDT_SWAP                 = 1,
    RCM_TAS_DPDT_BUTT
};
typedef VOS_UINT16 RCM_TAS_DPDT_CONTROL_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TAS_SEARCH_METHOD_ENUM_UINT16
 枚举说明  : 物理层的搜索方式
             0:进行原本搜索流程
             1:TAS专门搜索流程
*****************************************************************************/
enum RCM_TAS_SEARCH_METHOD_ENUM
{
    RCM_TAS_SEARCH_METHOD_ORIGINAL          = 0,
    RCM_TAS_SEARCH_METHOD_TAS               = 1,
    RCM_TAS_SEARCH_METHOD_BUTT
};
typedef VOS_UINT16 RCM_TAS_SEARCH_METHOD_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_AGENTC_DPDT_DELAY_CTRL_ENUM_UINT16
 枚举说明  : DPDT延迟切换操作指示:
             0:收到取消授权消息后停止一切dpdt操作
             1:取消DPDT延迟切换
             2:启动DPDT延迟切换
             3:更新最优DPDT值
             4:取消DPDT立即切换
             5:启动DPDT立即切换
             6:用于连接态下算法切换天线
*****************************************************************************/
enum RCM_AGENTC_DPDT_CTRL_ENUM
{
    RCM_AGENTC_DPDT_CTRL_STOP                  = 0,
    RCM_AGENTC_DPDT_DELAY_CTRL_STOP            = 1,
    RCM_AGENTC_DPDT_DELAY_CTRL_START           = 2,
    RCM_AGENTC_DPDT_DELAY_CTRL_UPDATE          = 3,
    RCM_AGENTC_DPDT_DIRECT_CTRL_STOP           = 4,
    RCM_AGENTC_DPDT_DIRECT_CTRL_START          = 5,
    RCM_AGENTC_DPDT_DIRECT_CTRL_UPDATE         = 6,
    RCM_AGENTC_DPDT_CTRL_BUTT
};
typedef VOS_UINT16 RCM_AGENTC_DPDT_CTRL_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TAS_INHERIT_STATE_ENUM_UINT16
 枚举说明  : 主从模变更场景标记
             0:非主从模变更场景
             1:主从模变更场景
*****************************************************************************/
enum RCM_TAS_INHERIT_STATE_ENUM
{
    RCM_TAS_INHERIT_NONEXIST        = 0,
    RCM_TAS_INHERIT_EXIST           = 1,
    RCM_TAS_INHERIT_BUTT
};
typedef VOS_UINT16 RCM_TAS_INHERIT_STATE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TAS_AUTHORIZE_PHY_STATE_ENUM_UINT16
 枚举说明  : 物理层是否有授权
             0:无授权
             1:有授权
*****************************************************************************/
enum RCM_TAS_AUTHORIZE_PHY_STATE_ENUM
{
    RCM_TAS_AUTHORIZE_DISABLE        = 0,
    RCM_TAS_AUTHORIZE_ENABLE         = 1,
    RCM_TAS_AUTHORIZE_BUTT
};
typedef VOS_UINT16 RCM_TAS_AUTHORIZE_PHY_STATE_ENUM_UINT16;

#ifdef LPS_RTT
/*****************************************************************************
 枚举名    :VOS_RATMODE_ENUM_UINT32
 协议表格  :
 ASN.1描述 :
 枚举说明  :睡眠唤醒时,接入模式定义
            注意；双卡双待的枚举值定义与协议栈和DRV协商为GULT;
            需要和DrvInterface.h中的PWC_COMM_MODE_E保持一致

*****************************************************************************/
enum VOS_RATMODE_ENUM
{
    VOS_RATMODE_GSM                         = 0,
    VOS_RATMODE_WCDMA                       = 1,
    VOS_RATMODE_LTE                         = 2,
    VOS_RATMODE_TDS                         = 3,
    VOS_RATMODE_BUTT
};
typedef VOS_UINT32 VOS_RATMODE_ENUM_UINT32;

#define VOS_VOID void
#endif
/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 结构名    : RCM_PHY_START_TAS_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知PHY授权TAS
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_TAS_DPDT_CONTROL_ENUM_UINT16            enMasterDpdt;                   /* 原主模dpdt状态 */
    RCM_TAS_INHERIT_STATE_ENUM_UINT16           enInheritCtrl;                  /* 主从模变更场景控制 */
}RCM_PHY_START_TAS_REQ_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_START_TAS_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知RCM授权完成指示
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_TAS_DPDT_CONTROL_ENUM_UINT16            enMasterDpdt;                   /* 原主模dpdt状态 */
    RCM_TAS_INHERIT_STATE_ENUM_UINT16           enInheritCtrl;                  /* 主从模变更场景控制 */
}PHY_RCM_START_TAS_CNF_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_STOP_TAS_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知PHY取消授权TAS
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_STOP_TAS_TYPE_ENUM_UINT16               enStopTasType;                  /* 取消TAS授权方式 */
    VOS_UINT16                                  usRsv2;                         /* 保留字段 */
}RCM_PHY_STOP_TAS_REQ_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_STOP_TAS_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知RCM取消授权完成指示
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_STOP_TAS_TYPE_ENUM_UINT16               enStopTasType;                  /* 取消TAS授权方式 */
}PHY_RCM_STOP_TAS_CNF_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_SET_DPDT_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知PHY设置DPDT控制字
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_TAS_DPDT_CONTROL_ENUM_UINT16            enDpdtValue;                    /* DPDT控制字，0:直通，1:交换 */
    VOS_UINT16                                  usRsv2;                          /* 保留字段 */
}RCM_PHY_SET_DPDT_REQ_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_SET_DPDT_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知RCM设置DPDT控制字完成指示
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
}PHY_RCM_SET_DPDT_CNF_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_TX_BLANKING_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知被控modem TX_BLANKING起始时间和终止时间
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT32                                  ulStartSlice;                   /* 起始slice */
    VOS_UINT32                                  ulStopSlice;                    /* 终止slice */
}RCM_PHY_TX_BLANKING_IND_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_TX_BLANKING_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知RCM主控modem TX_BLANKING起始时间和终止时间
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT32                                  ulStartSlice;                   /* 起始slice */
    VOS_UINT32                                  ulStopSlice;                    /* 终止slice */
    VOS_UINT16                                  usDpdtStatus;                   /* 切换后dpdt的状态 */
    VOS_UINT16                                  usRsv2;                         /* 保留字段 */
}PHY_RCM_TX_BLANKING_IND_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_RATMODE_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PHY通知RCM当前modem的主从模模式
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_RATMODE_ENUM_UINT32                     enMasterMode;                   /* 主模模式 */
    VOS_RATMODE_ENUM_UINT32                     enSlaveMode;                    /* 从模模式 */
    RCM_TAS_DPDT_CONTROL_ENUM_UINT16            enMasterDpdt;                   /* 原主模dpdt状态 */
    RCM_TAS_AUTHORIZE_PHY_STATE_ENUM_UINT16     enPhyAuthorizeFlg;              /* 原主模授权状态 */
}PHY_RCM_RATMODE_IND_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_MSG_HEADER_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RCM与物理层交互消息结构头定义，必须确保rcm发给phy的消息结构如下定义，
             赋值时应将usRsv赋为0，因为msp邮箱的存储的消息包第一个四字节为消息id。
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
}RCM_PHY_MSG_HEADER_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_ACCESS_STATE_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知RCM PHY Access 状态
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_ACCESS_STATE_ENUM_UINT16                enAccessState;                  /* Access State */
    VOS_UINT16                                  usRsv2;
}PHY_RCM_ACCESS_STATE_IND_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_SEARCHING_STATE_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知RCM PHY Searching 状态
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_SEARCHING_STATE_ENUM_UINT16             enSearchingState;               /* Searching State */
    VOS_UINT16                                  usRsv2;
}PHY_RCM_SEARCHING_STATE_IND_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_SIGNAL_STATE_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知RCM PHY Signal 状态
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_SIGNAL_STATE_ENUM_UINT16                enSignalState;                  /* Signal State */
    VOS_UINT16                                  usRsv2;
}PHY_RCM_SIGNAL_STATE_IND_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_IDLE_HAPPY_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知RCM PHY Signal 状态
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_IDLE_HAPPY_ENUM_UINT16                  enIdleHappy;                    /* Idle Happy State */
    VOS_UINT16                                  usRsv2;
}PHY_RCM_IDLE_HAPPY_IND_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_VOICE_CALL_STATE_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知RCM Voice Call 状态
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_VOICE_CALL_STATE_ENUM_UINT16            enVoiceCallState;               /* Voice Call State */
    VOS_UINT16                                  usRsv2;
}PHY_RCM_VOICE_CALL_STATE_IND_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_SEARCHING_STATE_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知RCM PHY Searching 状态
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_SEARCHING_STATE_ENUM_UINT16             enSearchingState;               /* Searching State */
    VOS_UINT16                                  usRsv2;
}PHY_RCM_SEARCHING_STATE_REQ_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_SEARCHING_STATE_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知PHY可以执行Searching并用何种方式搜索
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_TAS_SEARCH_METHOD_ENUM_UINT16           enSearchMethod;                 /* PHY Search Method */
    VOS_UINT16                                  usRsv2;
}RCM_PHY_SEARCHING_STATE_CNF_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_AGENTC_SET_DPDT_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知代理PHY切换DPDT任务控制信息
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_AGENTC_DPDT_CTRL_ENUM_UINT16            enTaskCtrl;                     /* DPDT切换任务控制 */
    RCM_TAS_DPDT_CONTROL_ENUM_UINT16            enDpdtValue;                    /* DPDT控制字，0:直通，1:交换 */
}RCM_PHY_AGENTC_SET_DPDT_REQ_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_AGENTC_SET_DPDT_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 代理PHY对切换DPDT任务控制请求信息的回复
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_AGENTC_DPDT_CTRL_ENUM_UINT16            enTaskCtrl;                     /* DPDT切换任务控制 */
    RCM_TAS_DPDT_CONTROL_ENUM_UINT16            enDpdtValue;                    /* DPDT控制字，0:直通，1:交换 */
}PHY_RCM_AGENTC_SET_DPDT_CNF_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of RcmPhyInterface.h */



