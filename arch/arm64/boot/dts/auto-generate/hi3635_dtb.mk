#Copyright Huawei Technologies Co., Ltd. 1998-2011. All rights reserved.
#This file is Auto Generated 

dtb-y += hi3635/GEMINI_703LT_04_config.dtb
dtb-y += hi3635/GEMINI_703L_02_config.dtb
dtb-y += hi3635/GEMINI_701L_02_config.dtb
dtb-y += hi3635/GEMINI_702L_02_config.dtb
dtb-y += hi3635/GEMINI_703L_03_config.dtb
dtb-y += hi3635/GEMINI_701L_01_config.dtb
dtb-y += hi3635/GEMINI_702L_01_config.dtb
dtb-y += hi3635/GEMINI_703L_01_config.dtb
dtb-y += hi3635/GRACE_CLG_V3_config.dtb
dtb-y += hi3635/GRACE_TUGL_V3_6402ES_config.dtb
dtb-y += hi3635/GRACE_TUGL_VN1_64G_config.dtb
dtb-y += hi3635/GRACE_TUGL_V0_config.dtb
dtb-y += hi3635/GRACE_UL_VN1_64G_HighCPU_config.dtb
dtb-y += hi3635/GRACE_CLG_VN1_64G_config.dtb
dtb-y += hi3635/GRACE_UL_V3_config.dtb
dtb-y += hi3635/GRACE_UL_V4_64G_config.dtb
dtb-y += hi3635/GRACE_CLG_V1_config.dtb
dtb-y += hi3635/GRACE_TUGL_V4_ANT_config.dtb
dtb-y += hi3635/GRACE_CLG_V4_64G_config.dtb
dtb-y += hi3635/GRACE_CLG_V4_config.dtb
dtb-y += hi3635/GRACE_UL_VN1_64G_config.dtb
dtb-y += hi3635/GRACE_CLG_V3_6402ES_config.dtb
dtb-y += hi3635/GRACE_TUGL_V4_64G_config.dtb
dtb-y += hi3635/GRACE_CLG_VN1_64G_HighCPU_NewPA_config.dtb
dtb-y += hi3635/GRACE_CLG_VN1_NewPA_config.dtb
dtb-y += hi3635/GRACE_TUGL_V4_64G_HighCPU_config.dtb
dtb-y += hi3635/GRACE_UL_V1_config.dtb
dtb-y += hi3635/GRACE_CLG_V4_64G_HighCPU_config.dtb
dtb-y += hi3635/GRACE_TUGL_V3_config.dtb
dtb-y += hi3635/GRACE_CLG_V4_ANT_config.dtb
dtb-y += hi3635/GRACE_TUGL_VN1_64G_HighCPU_config.dtb
dtb-y += hi3635/GRACE_CLG_V0_config.dtb
dtb-y += hi3635/GRACE_CLG_VN1_config.dtb
dtb-y += hi3635/GRACE_UL_V4_64G_HighCPU_config.dtb
dtb-y += hi3635/GRACE_UL_VN1_config.dtb
dtb-y += hi3635/GRACE_UL_V4_ANT_config.dtb
dtb-y += hi3635/GRACE_TUGL_V4_config.dtb
dtb-y += hi3635/GRACE_CLG_VN1_64G_HighCPU_config.dtb
dtb-y += hi3635/GRACE_TUGL_V1_config.dtb
dtb-y += hi3635/GRACE_TUGL_VN1_config.dtb
dtb-y += hi3635/GRACE_UL_V4_config.dtb
dtb-y += hi3635/GRACE_CLG_VN1_64G_NewPA_config.dtb
dtb-y += hi3635/PLK_TL00_VA_config.dtb
dtb-y += hi3635/PLK_UL00_VA_config.dtb
dtb-y += hi3635/PLK_L01_VB_config.dtb
dtb-y += hi3635/PLK_UL00_VB_config.dtb
dtb-y += hi3635/PLK_AL10_VB_config.dtb
dtb-y += hi3635/PLK_AL10_VA_config.dtb
dtb-y += hi3635/PLK_CL00_VA_config.dtb
dtb-y += hi3635/PLK_L01_VA_config.dtb
dtb-y += hi3635/PLK_TL00H_VA_config.dtb
dtb-y += hi3635/PLK_L02_VB_config.dtb
dtb-y += hi3635/PLK_L02_VA_config.dtb
dtb-y += hi3635/PLK_CL00_VB_config.dtb
dtb-y += hi3635/PLK_TL00H_VB_config.dtb
dtb-y += hi3635/PLK_TL00_VB_config.dtb
dtb-y += hi3635/LITTLEBOY_ALL_V0_config.dtb
dtb-y += hi3635/LITTLEBOY_ALL_V1_config.dtb
dtb-y += hi3635/CARRERA_CLG_V3_config.dtb
dtb-y += hi3635/CARRERA_UL_V3_config.dtb
dtb-y += hi3635/CARRERA_CLG_V1_config.dtb
dtb-y += hi3635/CARRERA_CLG_V3_66T_config.dtb
dtb-y += hi3635/CARRERA_UL_V3_66T_config.dtb
dtb-y += hi3635/CARRERA_UL_V1_config.dtb
dtb-y += hi3635/CARRERA_TUGL_V3_config.dtb
dtb-y += hi3635/CARRERA_TUGL_V3_66T_config.dtb
dtb-y += hi3635/CARRERA_TUGL_V1_config.dtb
dtb-y += hi3635/MOZART_802L_02_config.dtb
dtb-y += hi3635/MOZART_803L_02_config.dtb
dtb-y += hi3635/MOZART_801L_02_config.dtb
dtb-y += hi3635/DAVINCE_703LC_02_config.dtb
dtb-y += hi3635/DAVINCE_703LC_01_config.dtb
dtb-y += hi3635/DAVINCE_703L_02_config.dtb
dtb-y += hi3635/DAVINCE_701L_01_config.dtb
dtb-y += hi3635/DAVINCE_703L_01_config.dtb
dtb-y += hi3635/hi3635_asic_config.dtb

targets += hi3635_dtb
targets += $(dtb-y)

# *.dtb used to be generated in the directory above. Clean out the
# old build results so people don't accidentally use them.
hi3635_dtb: $(addprefix $(obj)/, $(dtb-y))
	$(Q)rm -f $(obj)/../*.dtb

clean-files := *.dtb

#end of file
