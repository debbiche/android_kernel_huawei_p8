#include <boot.h>
#include "nv_balong.h"
#include <bsp_pm_om.h>

struct pm_om_fastboot
{
	struct pm_om_smem_cfg *smem;
};

struct pm_om_fastboot g_pm_om_fastboot;

void pm_om_smem_init(void)
{
	u32 mem_size = 0;
	struct pm_om_smem_cfg *smem = NULL;
	DRV_PM_OM_CFG_STRU nv_cfg;

	g_pm_om_fastboot.smem = smem;

	/* NV控制是否打开pm log */
	memset((void *)&nv_cfg, 0, sizeof(nv_cfg));
	if(NV_OK != bsp_nvm_read(NV_ID_DRV_PM_OM, (u8*)&(nv_cfg), sizeof(nv_cfg)))
	{
		cprintf("[%s]: nv[0x%x] read fail\n", __FUNCTION__, NV_ID_DRV_PM_OM);
	}

	if (PM_OM_USE_SHARED_DDR == nv_cfg.mem_ctrl)
	{
		smem     = (struct pm_om_smem_cfg *)SHM_MEM_PM_OM_ADDR;
		mem_size = SHM_MEM_PM_OM_SIZE;
	}
	else
	{
		smem     = (struct pm_om_smem_cfg *)(DDR_SOCP_ADDR + DDR_SOCP_SIZE - PM_OM_SOCP_MEM_SIZE);
		mem_size = PM_OM_SOCP_MEM_SIZE;
	}

	/* 文件头初始化 */
	smem->file_head.magic    = PM_OM_MAGIC_PMLG;
	smem->file_head.len      = mem_size;
	smem->file_head.version  = 0;
	smem->file_head.reserved = PM_OM_PROT_MAGIC1;

	smem->sn                 = 0;
	smem->fwrite_sw          = 0;
	memcpy((void *)&(smem->nv_cfg), (void *)&nv_cfg, sizeof(nv_cfg));

	/* log: ring buffer信息结构初始化 */
	smem->rb_info.protect_magic = PM_OM_PROT_MAGIC1;
	smem->rb_info.read          = 0;
	smem->rb_info.write         = 0;
	smem->rb_info.size          = mem_size - sizeof(struct pm_om_smem_cfg);
}

