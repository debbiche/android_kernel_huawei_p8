dtb-y += GRACE_CLG_V3_config.dtb
dtb-y += GRACE_TUGL_V3_6402ES_config.dtb
dtb-y += GRACE_TUGL_VN1_64G_config.dtb
dtb-y += GRACE_TUGL_V0_config.dtb
dtb-y += GRACE_UL_VN1_64G_HighCPU_config.dtb
dtb-y += GRACE_CLG_VN1_64G_config.dtb
dtb-y += GRACE_UL_V3_config.dtb
dtb-y += GRACE_UL_V4_64G_config.dtb
dtb-y += GRACE_CLG_V1_config.dtb
dtb-y += GRACE_TUGL_V4_ANT_config.dtb
dtb-y += GRACE_CLG_V4_64G_config.dtb
dtb-y += GRACE_CLG_V4_config.dtb
dtb-y += GRACE_UL_VN1_64G_config.dtb
dtb-y += GRACE_CLG_V3_6402ES_config.dtb
dtb-y += GRACE_TUGL_V4_64G_config.dtb
dtb-y += GRACE_CLG_VN1_64G_HighCPU_NewPA_config.dtb
dtb-y += GRACE_CLG_VN1_NewPA_config.dtb
dtb-y += GRACE_TUGL_V4_64G_HighCPU_config.dtb
dtb-y += GRACE_UL_V1_config.dtb
dtb-y += GRACE_CLG_V4_64G_HighCPU_config.dtb
dtb-y += GRACE_TUGL_V3_config.dtb
dtb-y += GRACE_CLG_V4_ANT_config.dtb
dtb-y += GRACE_TUGL_VN1_64G_HighCPU_config.dtb
dtb-y += GRACE_CLG_V0_config.dtb
dtb-y += GRACE_CLG_VN1_config.dtb
dtb-y += GRACE_UL_V4_64G_HighCPU_config.dtb
dtb-y += GRACE_UL_VN1_config.dtb
dtb-y += GRACE_UL_V4_ANT_config.dtb
dtb-y += GRACE_TUGL_V4_config.dtb
dtb-y += GRACE_CLG_VN1_64G_HighCPU_config.dtb
dtb-y += GRACE_TUGL_V1_config.dtb
dtb-y += GRACE_TUGL_VN1_config.dtb
dtb-y += GRACE_UL_V4_config.dtb
dtb-y += GRACE_CLG_VN1_64G_NewPA_config.dtb

targets += dtbs

dtbs: $(addprefix $(obj)/, $(dtb-y))
	$(Q)rm -f $(obj)/../*.dtb

clean-files := *.dtb
