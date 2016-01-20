
#include <drv_pm.h>
#include <bsp_pm_om.h>

#ifndef CONFIG_PM_OM_BALONG
int bsp_pm_log(u32 mod_id, u32 data_len , void *data)
{
	return -1;
}

int bsp_pm_log_type(u32 mod_id, u32 type, u32 data_len , void *data)
{
	return -1;
}

void* bsp_pm_log_addr_get(void)
{
	return NULL;
}

void* bsp_pm_dump_get(u32 magic, u32 len)
{
	return NULL;
}

#endif /* CONFIG_PM_OM_BALONG */

int mdrv_pm_log(int mod_id,  unsigned int data_len , void *data)
{
	if (PM_MOD_AP_OSA == mod_id)
		mod_id = PM_OM_AOSA;
	else if (PM_MOD_CP_OSA == mod_id)
		mod_id = PM_OM_COSA;
	else if (PM_MOD_CP_MSP == mod_id)
		mod_id = PM_OM_CMSP;
	else
		pmom_pr_err("err: invalid mod_id\n");
	return bsp_pm_log((u32)mod_id, data_len, data);
}