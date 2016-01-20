
#include <bsp_pm_om.h>

#define PM_OM_TEST_DUMP_SIZE (u32)(0x4000)
#define PM_OM_TEST_LOG_SIZE  (u32)(64)


/* ¶ÔÓ¦mcoreµÄpm_om_platform_init */
void pmom_test_init(void)
{
}

void pmom_dump_test_func(u32 magic, u32 size)
{
	char *dump_addr;
	char name[5];

	(*(unsigned *)(name)) = magic;
	name[4]=0;

	dump_addr = bsp_pm_dump_get((u32)magic, size);
	if (NULL == dump_addr)
	{
		pmom_pr_err("no pm om mem\n");
	}
	else
	{
		pmom_pr_err("[%s]0x%p\n", name, dump_addr);
	}
}
void pmom_dump_test(void)
{
	pmom_dump_test_func(PM_OM_ABB, PM_OM_TEST_DUMP_SIZE);
	pmom_dump_test_func(PM_OM_BBP, PM_OM_TEST_DUMP_SIZE);
	pmom_dump_test_func(PM_OM_DSP, PM_OM_TEST_DUMP_SIZE);
	pmom_dump_test_func(PM_OM_PMU, PM_OM_TEST_DUMP_SIZE);
	//pmom_dump_test_func(PM_OM_ABB, PM_OM_TEST_DUMP_SIZE);
}

void pmom_dump_test2(void)
{
	pmom_dump_test_func(PM_OM_ABB, PM_OM_TEST_DUMP_SIZE);
}

void pmom_log_test_func(u32 magic)
{
	static u8 write = 0;
	static u8 log[PM_OM_TEST_LOG_SIZE];
	u32 i = 0;

	for (i = 0; i < PM_OM_TEST_LOG_SIZE; i++)
	{
		log[write&(PM_OM_TEST_LOG_SIZE-1)] = write;
		write++;
	}

	if (PM_OM_OK != bsp_pm_log_type(magic, 0, PM_OM_TEST_LOG_SIZE, (void *)&log))
	{
		pmom_pr_err("pm_log_type err\n");
	}
}

void pmom_log_test(void)
{
	static u32 i = 0;

	if (i == 0)
	{
		pmom_log_test_func(PM_OM_ABB);
		pmom_log_test_func(PM_OM_ACLK);
	}
	else if (i == 1)
	{
		pmom_log_test_func(PM_OM_AIPC);
		pmom_log_test_func(PM_OM_AIPF);
	}
	else if (i == 2)
	{
		pmom_log_test_func(PM_OM_BBP);
		pmom_log_test_func(PM_OM_CCLK);
	}
	else if (i == 3)
	{
		pmom_log_test_func(PM_OM_CIPC);
		pmom_log_test_func(PM_OM_CIPF);
	}
	else if (i == 4)
	{
		pmom_log_test_func(PM_OM_CPUF);
		pmom_log_test_func(PM_OM_DSP);
	}
	else if (i == 5)
	{
		pmom_log_test_func(PM_OM_DPM);
		pmom_log_test_func(PM_OM_PMA);
	}
	else if (i == 6)
	{
		pmom_log_test_func(PM_OM_PMC);
		pmom_log_test_func(PM_OM_PMM);
		i = 0;
	}
	i++;
}

