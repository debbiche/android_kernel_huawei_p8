/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : apb_mipi_balong.h */
/* Version       : 2.0 */
/* Created       : 2013-03-13*/
/* Last Modified : */
/* Description   :  The C union definition file for the module apb_mipi*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
#ifndef BBP_MIPI_BALONG_H
#define BBP_MIPI_BALONG_H
#include "osl_types.h"
#include "bsp_mipi.h"
#define LOW_32BIT_MASK         (0xFFFFFFFF)
#define MIPI_GRANT_FLAG_IS_HIGH (0x1)
#define MIPI_REG_WAIT_TIMEOUT (1000000)
#define MIPI_IS_INITIALIZED_FLAG (0x123)


#define DSP_MIPI0_EN_IMI 0x00f8
#define DSP_MIPI0_EN_IMI_MASK ~(1<<0)
#define MIPI_GRANT_DSP_FLAG_IS_HIGH 1
#define RD_END_FLAG_MIPI_SOFT_FLAG_IS_HIGH 1
#define MIPI_EXTENDED_WR_DATA_FRAME_SIZE 8
#define MIPI_TASK_DELAY_TICKS	2
#define MIPI_READ_END_FLAG 1	/*only read has end_flag*/
#define MIPI_CMD_TYPE_READ		2
#define MIPI_CMD_TYPE_WRITE		0
#define MIPI_WRITE_END_FLAG		0
#define BIT_CNT					8	/*bit number in a byte*/

#define MIPI_HIGH_OFFSET				32
#define MIPI_CMD_TYPE_PARITY1_OFFSET	  4

/*config frame*/
#define MIPI_TRANS_TYPE_OFFSET	61
#define MIPI_SLAVE_ADDR_OFFSET	57
#define MIPI_CMD_TYPE_OFFSET		53
#define MIPI_BYTE_CNT_OFFSET		49
#define MIPI_PARITY1_OFFSET		48
#define MIPI_REG_ADDR_OFSET		40
#define MIPI_PARITY2_OFFSET		39
#define MIPI_END_FLAG_OFFSET		34
#define MIPI_DATA_OFFSET			31
#define MIPI_PARITY3_OFFSET		30

#define MIPI_EXTENDED_WR_DATA_FRAME_MASK    (((u32)(0x1)<<MIPI_EXTENDED_WR_DATA_FRAME_SIZE)-1)
#define MIPI_EXTENDED_WR_FIRST_DATA_FRAME_GET(data_high, data_low)  \
(((data_high&MIPI_EXTENDED_WR_DATA_FRAME_MASK)<<1)|((data_low>>31)&0x1))

/*通道绑定相关寄存器*/
#define MIPI_UNBIND_EN 0xb90
#define MIPI_CH_SEL0 0xb94
#define MIPI_CH_SEL1 0xb98

/*类型定义*/
typedef struct
{
	u32 mipi_cmd_low;
	u32 mipi_cmd_high;
}MIPI_CMD_STRU;

typedef struct
{
	u32 en_pastar;
	u32 slave_addr;
	u32 wr_cmd_type;
	u32	rd_cmd_type;
	u32	byte_cnt;
	u32	advance_time;
}MIPI_INIT_STRU;



#define MIPI_MAX_BYTE_COUNT     (4)

enum MIPI_CMD_TYPE
{
    MIPI_CMD_TYPE_R0_W   = 0,    /* Register0 Write */
    MIPI_CMD_TYPE_R_W    = 1,    /* Register Write */
    MIPI_CMD_TYPE_R_R    = 2,    /* Register Read */
    MIPI_CMD_TYPE_ER_W   = 3,    /* Extended Register Write */
    MIPI_CMD_TYPE_ER_R   = 4,    /* Extended Register Read */
    MIPI_CMD_TYPE_HS_R_R = 5,    /* Half Speed Register Read */
    MIPI_CMD_TYPE_HS_ER_R= 6,    /* Half Speed Extended Register Read */
    MIPI_CMD_TYPE_MAX,
};

#define MIPI_CMD_FORMAT_R0_W    (1) /* Register0 Write, 1 bit */
#define MIPI_CMD_FORMAT_R_W     (2) /* Register Write, 3 bits */
#define MIPI_CMD_FORMAT_R_R     (3) /* Register Read, 3 bits */
#define MIPI_CMD_FORMAT_ER_W    (0) /* Extended Register Write, 4 bits */
#define MIPI_CMD_FORMAT_ER_R    (2) /* Extended Register Read, 4 bits */
#define MIPI_CMD_FORMAT_HS_R_R  (3) /* Half Speed Register Read, 3 bits */
#define MIPI_CMD_FORMAT_HS_ER_R (3) /* Half Speed Extended Register Read, 3 bits */

/* Register 0 Write */
union mipi_cmd_r0
{
    struct
    {
        u32 mipi_id         : 3 ;
        u32 reserver        : 13;
        u32 parity_cmd_frame: 1 ;
        u32 byte_0          : 7 ;
        u32 cmd_format      : 1 ;
        u32 slave_id        : 4 ;
        u32 cmd_type        : 3 ;
    } attr;
    u32 value;
};

/* Register Read/Write */
union mipi_cmd
{
    struct
    {
        u32 mipi_id         : 3 ;
        u32 reserver        : 4 ;
        u32 parity_byte_0   : 1 ;
        u32 byte_0          : 8 ;
        u32 parity_cmd_frame: 1 ;
        u32 reg_addr        : 5 ;
        u32 cmd_format      : 3 ;
        u32 slave_id        : 4 ;
        u32 cmd_type        : 3 ;
    } attr;
    u32 value;
};

/* Extended Register Read/Write */
union mipi_ex_cmd
{
    struct
    {
        u64 mipi_id         : 3 ;
        u64 parity_byte_3   : 1 ;
        u64 byte_3          : 8 ;
        u64 parity_byte_2   : 1 ;
        u64 byte_2          : 8 ;
        u64 parity_byte_1   : 1 ;
        u64 byte_1          : 8 ;
        u64 parity_byte_0   : 1 ;
        u64 byte_0          : 8 ;
        u64 parity_addr_frame:1 ;
        u64 reg_addr        : 8 ;
        u64 parity_cmd_frame: 1 ;
        u64 byte_count      : 4 ;
        u64 cmd_format      : 4 ;
        u64 slave_id        : 4 ;
        u64 cmd_type        : 3 ;
    } attr;
    struct
    {
        u32 low_value;
        u32 high_value;
    } cmd;
    u64 value;
};


#define MIPI_READ_END_FLAG_BITS_IN_BYTE0    (3)



#ifndef INLINE
#define INLINE __inline__
#endif

#define HI_SET_GET_MIPI(__full_name__,__reg_name,__reg_type, __reg_base, __reg_offset)  \
static INLINE void set_##__full_name__(unsigned int val,MIPI_CTRL_ENUM ctrl_id)  \
{\
	__reg_type  reg_obj; \
	reg_obj.u32 = readl((__reg_base[ ctrl_id ]) + __reg_offset); \
	reg_obj.bits.__reg_name = val; \
	writel(reg_obj.u32, (__reg_base[ ctrl_id ]) + __reg_offset); \
} \
static INLINE unsigned int get_##__full_name__(MIPI_CTRL_ENUM ctrl_id)  \
{\
	__reg_type  reg_obj; \
	reg_obj.u32 = readl((__reg_base[ ctrl_id ]) + __reg_offset); \
	return reg_obj.bits.__reg_name; \
}
#if 0
#define HI_SET_GET(__full_name__,__reg_name,__reg_type, __reg_base, __reg_offset)  \
static INLINE void set_##__full_name__(unsigned int val,MIPI_CTRL_ENUM ctrl_id)  \
{\
	__reg_type  reg_obj; \
	reg_obj.u32 = readl((__reg_base[crtl_id]) + __reg_offset); \
	reg_obj.bits.##__reg_name = val; \
	writel(reg_obj.u32, (__reg_base[crtl_id]) + __reg_offset); \
} \
static INLINE unsigned int get_##__full_name__(MIPI_CTRL_ENUM crtl_id)  \
{\
	__reg_type  reg_obj; \
	reg_obj.u32 = readl((__reg_base[crtl_id]) + __reg_offset); \
	return reg_obj.bits.##__reg_name; \
}
#endif

#endif
