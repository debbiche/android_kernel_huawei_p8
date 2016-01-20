
#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore
OBC_LOCAL_MOUDLE_NAME	?=bastet_ccore

#***********************************************************#
# compiler flags
#***********************************************************#
CC_USER_DEFINES   := -DM1_PORTING -DPEEK_DEBUG  -DUSE_ENODEB_2 -DPROCESS_TRACE -DVOS_OSA_CPU=0 -DBSP_CORE_MODEM
ifeq ($(strip $(CFG_FEATURE_UE_MODE_TDS)),FEATURE_ON)
CC_USER_DEFINES   += \
			-DSTACK_TD_EDGE_DM -DMEMO_BLOCK -DVERS_TD_ONLY_IP -DYNMIC_LOAD_DSP -DTDSCDMA_ONLY -DRRC_ASN_EXTEND -DBSP_CORE_MODEM
endif #end of CFG_FEATURE_UE_MODE_TDS
AS_USER_FLAGS   := 

#***********************************************************#
# Source File to compile
#***********************************************************#
PRJ_CONFIG_DIR  :=$(BALONG_TOPDIR)/config
PRJ_COMM_DIR	:=$(BALONG_TOPDIR)/include
PS_DIR			:=$(BALONG_TOPDIR)/modem/ps
PS_COMM_DIR  	:=$(PS_DIR)/comm
PS_INC_DIR    	:=$(PS_DIR)/inc
PS_NAS_SRC_DIR  :=$(PS_DIR)/nas

OBC_LOCAL_SRC_DIR := $(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/LwipSrc \
                   $(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/LwipSrc/arch \
                   $(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/LwipSrc/src/core/ipv4 \
                   $(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/LwipSrc/src/core \
                   $(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/LwipSrc/src/api \
                   $(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/LwipSrc/src/netif \
                   $(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/LwipSrc/src/netif/ppp \
                   $(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/Interface \
                   $(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/PreProc \
                   $(PS_COMM_DIR)/comm/BASTET/Library/Src/DSPP \
                   $(PS_COMM_DIR)/comm/BASTET/Library/Src/CommonTools \
                   $(PS_COMM_DIR)/comm/BASTET/PAL/Src \
                   $(PS_COMM_DIR)/comm/BASTET/Platform/OS/Src \
                   $(PS_COMM_DIR)/comm/BASTET/Platform/PAL/Src \
                   $(PS_COMM_DIR)/comm/BASTET/Platform/Driver/Src \
                   $(PS_COMM_DIR)/comm/BASTET/Platform/Sys/Src \
                   $(PS_COMM_DIR)/comm/BASTET/Service/Src \
                   $(PS_COMM_DIR)/comm/BASTET/Core/Src/Task \
                   $(PS_COMM_DIR)/comm/BASTET/Core/Src/Registry \
                   $(PS_COMM_DIR)/comm/BASTET/APP/Src \
                   $(PS_COMM_DIR)/comm/BASTET/APP/Src/HeartBeat \
                   $(PS_COMM_DIR)/comm/BASTET/APP/Src/Email \
                   $(PS_COMM_DIR)/comm/BASTET/TestCase


OBC_LOCAL_INC_DIR :=\
		$(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/LwipSrc \
		$(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/LwipSrc/arch/include \
		$(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/LwipSrc/src/include \
		$(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/LwipSrc/src/include/ipv4 \
		$(PS_COMM_DIR)/comm/BASTET/Library/Src/IP_STACK/Adapter \
		$(PS_COMM_DIR)/comm/BASTET/Library/Inc \
		$(PS_COMM_DIR)/comm/BASTET/Platform/OS/Inc \
		$(PS_COMM_DIR)/comm/BASTET/Platform/PAL/Inc \
		$(PS_COMM_DIR)/comm/BASTET/Platform/Driver/Inc \
		$(PS_COMM_DIR)/comm/BASTET/Platform/Sys/Inc \
		$(PS_COMM_DIR)/comm/BASTET/Service/Inc \
		$(PS_COMM_DIR)/comm/BASTET/Core/Inc \
		$(PS_COMM_DIR)/comm/BASTET/APP/Inc \
		$(PS_COMM_DIR)/comm/BASTET/PAL/Inc \
		$(PS_COMM_DIR)/comm/CDS/Inc \
		$(PS_COMM_DIR)/comm/MEM/Inc \
		$(PS_COMM_DIR)/comm/STL/Inc \
		$(PS_COMM_DIR)/comm/LINK/Inc \
		$(PRJ_COMM_DIR)/drv \
		$(PRJ_COMM_DIR)/oam/comm/osa\
		$(PRJ_COMM_DIR)/oam/comm/om \
		$(PRJ_COMM_DIR)/ps/tlps \
		$(PRJ_COMM_DIR)/ps/gups \
		$(PRJ_COMM_DIR)/med \
		$(PRJ_COMM_DIR)/nv/gu/codec \
		$(PRJ_COMM_DIR)/nv/gu/ttf \
		$(PRJ_COMM_DIR)/nv/gu/nas \
		$(PRJ_COMM_DIR)/oam/gu/om \
		$(PRJ_COMM_DIR)/oam/gu/log \
		$(PRJ_COMM_DIR)/taf \
		$(PRJ_COMM_DIR)/oam/comm/nvim \
		$(PRJ_CONFIG_DIR)/osa \
		$(PRJ_CONFIG_DIR)/nvim/include/gu \
		$(PS_INC_DIR)/gu \
		$(PS_COMM_DIR)/comm/BASTET/Library/Src/OPENSSL \
		$(PS_COMM_DIR)/comm/BASTET/Library/Src/OPENSSL/include


#***********************************************************
#include rules. must be droped at the bottom, OBB_BUILD_ACTION values: cc tqe lint fortify
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/$(OBB_BUILD_ACTION)_vxworks6.8_rules.mk


