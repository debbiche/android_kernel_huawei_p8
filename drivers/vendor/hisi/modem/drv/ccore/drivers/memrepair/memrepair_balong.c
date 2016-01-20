
#include <product_config.h>

#ifdef MODEM_MEM_REPAIR

#include <bsp_memmap.h>
#include <hi_base.h>
#include <hi_ap_sctrl.h>
#include <hi_syscrg.h>
#include <hi_syssc.h>

#include <bsp_om.h>
#include <bsp_shared_ddr.h>
#include <bsp_hardtimer.h>
#endif

#include <bsp_memrepair.h>
#include <memrepair_balong.h>

#ifdef MODEM_MEM_REPAIR

/*sram中申请了0x20个字节*/
static unsigned int *mr_poweron_flag[MODEM_MEMREPAIR_BUTT] = {
	[MODEM_MEMREPAIR_DSP] =(unsigned int *)(SHM_MEM_MEMREPAIR_ADDR + 0),
	[MODEM_MEMREPAIR_IRM] =(unsigned int *)(SHM_MEM_MEMREPAIR_ADDR + MODEM_MEMREPAIR_IRM*4),
};

/*判断是否需要repair*/
int bsp_modem_is_need_memrepair(modem_memrepair_ip module)
{
    int ret = 0;
    unsigned int bit;

    switch(module){
		case MODEM_MEMREPAIR_DSP:
			bit = MODEM_MEMREPAIR_DSP_BIT;
			break;
		case MODEM_MEMREPAIR_IRM:
			bit = MODEM_MEMREPAIR_IRM_BIT;
			break;
		default:
			ret = 0;
			return ret;/*默认不需要repair*/
	}
    ret = (int)(get_hi_ap_sctrl_scbakdata14_bakdata() & (0x01UL<<bit));

    return ret;
}

int bsp_modem_memrepair(modem_memrepair_ip module)
{
	int ret = 0;
	unsigned int timestamp_busbusy;
    unsigned int timestamp_repairdone;
	unsigned int slicenew_busbusy = 0;
    unsigned int slicenew_repairdone = 0;
	unsigned int bus_stat,done_stat,err_flag = 0;
	unsigned int bit;


    if (!bsp_modem_is_need_memrepair(module))
        return 0;

	/*3.配置SYSCTRL中的SCPERCTRL3(0x20c) bit 3 bit4为0*/
	set_hi_ap_sctrl_scperctrl3_repair_frm_sram(0);
	set_hi_ap_sctrl_scperctrl3_repair_frm_efu(0);

    /*4.配置SYSCTRL中的SCPERCTRL2（0x208）(bit19:0)为1,等待1us*/
    set_hi_ap_sctrl_scperctrl2_repair_disable(0xfffff);

    udelay(1);

	switch(module){
		case MODEM_MEMREPAIR_DSP:
			bit = MODEM_MEMREPAIR_DSP_BIT;
			break;
		case MODEM_MEMREPAIR_IRM:
			bit = MODEM_MEMREPAIR_IRM_BIT;
			break;
		default:
			ret = -1;
			goto out;
	}

	/*5.使能待修复模块，配置SYSCTRL中的SCPERCTRL2(0x208)bit14为0，
	其他bit为1,[31:20] bit 保持不变*/
	set_hi_ap_sctrl_scperctrl2_repair_disable(~(1<<bit)&0xfffff);
	udelay(5);
	/*6.若首次上电，则配置SYSCTRL中的SCPERCTRL3 (0x20C) bit4为1,指示修复数据来源EFUSE*/
	if(*mr_poweron_flag[module] == MR_POWERON_FLAG){
		set_hi_ap_sctrl_scperctrl3_repair_frm_efu(1);
		*mr_poweron_flag[module] = 0;
	}
	else if(*mr_poweron_flag[module] == 0){
	/*6.非首次上电，则配置SYSCTRL中的SCPERCTRL3 (0x20C)bit3为1,指示修复数据来源sram*/
		set_hi_ap_sctrl_scperctrl3_repair_frm_sram(1);
	}
	else{/*内存被踩*/
		mr_print_error("shared ddr has trampled!module id is %d\n",module);/*内存被踩，后面流程还要继续走吗?*/
	}

    /*k3v3+新增*/
    /*8.读取SCTRL中的repair done，SCPERSTATUS3（0x228）bit[14]是否为1.如果为1，执行（9），否则，执行（13）*/
    timestamp_repairdone = bsp_get_slice_value();
    do{
        done_stat = get_hi_ap_sctrl_scperstatus3_efusec_timeout_dbg_info() & (1<<bit);
        if(done_stat != 0)
        {
        	/*9.查询SYSCTRL中的SCMRBBUSYSTAT（0x304）bit14，如果其为0，打印修复完成*/
            /*10.判断软件是否已经启动倒计时，如果没有启动，软件启动300us倒计时；如果已经启动，进入步骤（11）*/

        	timestamp_busbusy = bsp_get_slice_value();
        	do{
        		bus_stat = get_hi_ap_sctrl_scmrbbusystat_mrb_efuse_busy() & (1<<bit);/*304*/
        		slicenew_busbusy = bsp_get_slice_value();
        	}while((bus_stat != 0)&&(get_timer_slice_delta(timestamp_busbusy, slicenew_busbusy) < MEMRAIR_WAIT_TIMEOUT));
            /*11.软件判断倒计时是否超时，若没有超时，进入步骤（9）；若倒计时超时，打印修复时间不足信息，进入步骤（12）*/
        	if(bus_stat != 0){
        		mr_print_error("wait memrepair efuse busy bit timeout,wait slice is 0x%x\n",(slicenew_busbusy - timestamp_busbusy));
        		/* coverity[no_escape] */
        		for(;;) ;
        	}
            else
            {
                /*读取errflag*/
                err_flag = get_hi_ap_sctrl_scperstatus1_err_flag();
                if(!err_flag)
                    goto done;
                else
                {
                    mr_print_error("memrepair has errflag,errflag is 0x%x\n",err_flag);
                    /* coverity[no_escape] */
                    for(;;) ;
                }
            }
        }
        slicenew_repairdone = bsp_get_slice_value();
    /*13.判断是否已经启动30us倒计时，如果没有启动，软件启动30us倒计时，执行（14），如果已经启动，执行（14）*/
    }while((done_stat == 0)&&(get_timer_slice_delta(timestamp_repairdone, slicenew_repairdone) < MEMRAIR_DONE_WAIT_TIMEOUT));
    /*14.判断是否超时，如果没有超时，执行（8）；如果超时，打印修复没有启动错误，软件进入死循环*/
    if(done_stat == 0)
    {
        mr_print_error("wait memrepair done bit timeout,wait slice is %x\n",(slicenew_repairdone - timestamp_repairdone));
        /* coverity[no_escape] */
        for(;;) ;
    }

done:
	/*12. 配置SYSCTRL中的SCPERCTRL3(0x20c)bit 3 bit4为0，退出修复流程*/
	set_hi_ap_sctrl_scperctrl3_repair_frm_sram(0);/*bit 3*/
	set_hi_ap_sctrl_scperctrl3_repair_frm_efu(0);/*bit 4*/

out:
	return ret;
}

int bsp_get_memrepair_time(void)
{
    int need_flag;

    need_flag = bsp_modem_is_need_memrepair(MODEM_MEMREPAIR_DSP)||bsp_modem_is_need_memrepair(MODEM_MEMREPAIR_IRM);

    if (need_flag)
        return 900; /* us *//*验证后重新估计*/
    else
        return 0;
}

#else

int bsp_modem_memrepair(modem_memrepair_ip module){
	return 0;
}

int bsp_get_memrepair_time(void)
{
    return 0;
}
#endif

