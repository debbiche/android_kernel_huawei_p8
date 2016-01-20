
#include <product_config.h>

#ifdef MODEM_MEM_REPAIR

#include <bsp_memmap.h>
#include <hi_base.h>
#include <hi_ap_sctrl.h>
#include <hi_syscrg.h>
#include <hi_syssc.h>
#endif

#include <bsp_memrepair.h>
#include <memrepair_balong.h>

#ifdef MODEM_MEM_REPAIR
/*检测是否有坏点*/
int bad_mem_check(u32 addr, u32 value, u32 len)
{
    u32 reg_value = 0;
    int iret = 0;
    u32 i = 0;

    memset((void *)addr,value,len);
    for(i = 0;i < len;i++)
    {
        reg_value = (u32)readl(addr++);
        if(reg_value != value)
        {
            mr_print_error("bad mem addr 0x%x,value is 0x%x,should be 0x%x. \n",addr,reg_value,value);
            iret = -1;
        }
    }
    /*对value取反，再检测一次*/
    value = ~value;

    memset((void *)addr,value,len);
    for(i = 0;i < len;i++)
    {
        reg_value = (u32)readl(addr++);
        if(reg_value != value)
        {
            mr_print_error("bad mem addr 0x%x,value is 0x%x,should be 0x%x. \n",addr,reg_value,value);
            iret = -1;
        }
    }

    mr_print_error("mem check finsh.\n");

    return iret;
}
/*验证irmbbpmem空间*/
void irmbbp_memrepair_test(void)
{
    int iret = 0;
    u32 addr = 0xe1c10000;
    u32 value= 0;
    u32 len = 0x8000;

    /*bbp irm memory*/
    /*先检测是否有坏点*/
    iret = bad_mem_check(addr,value,len);
    if(!iret)
    {
        mr_print_error("bbp irm memory have no bad mem.\n");
    }
    else
    {
        /*执行memrepair*/
        iret = bsp_modem_memrepair(MODEM_MEMREPAIR_IRM);
        if(iret)
        {
            mr_print_error("repair error!\n");
        }
        /*重新检测坏点*/
        iret = bad_mem_check(addr,value,len);
        if(iret)
        {
            mr_print_error("repair failed!\n");
        }
        else
        {
            mr_print_error("repair ok!\n");
        }
    }
}
/*验证bbemem空间*/
void bbe_memrepair_test(void)
{
    int iret = 0;
    u32 value= 0x0;
    u32 addr1 = 0xe2840000;
    u32 addr2 = 0xe2740000;
    u32 len1 = 0xc0000;
    u32 len2 = 0xc0000;

    /*bbp irm memory*/
    /*先检测是否有坏点*/
    iret = bad_mem_check(addr1,value,len1);
    iret |= bad_mem_check(addr2,value,len2);
    if(!iret)
    {
        mr_print_error("bbe memory have no bad mem.\n");
    }
    else
    {
        /*执行memrepair*/
        iret = bsp_modem_memrepair(MODEM_MEMREPAIR_DSP);
        if(iret)
        {
            mr_print_error("repair error!\n");
        }
        /*重新检测坏点*/
        iret = bad_mem_check(addr1,value,len1);
        iret |= bad_mem_check(addr2,value,len2);
        if(iret)
        {
            mr_print_error("repair failed!\n");
        }
        else
        {
            mr_print_error("repair ok!\n");
        }
    }
}
int bsp_modem_memrepair_test(void)
{
    int iret = 0;

    irmbbp_memrepair_test();
    bbe_memrepair_test();
    mr_print_error("test finsh.\n");

    return iret;
}

#else

int bsp_modem_memrepair_test(u32 addr,u32 len,u32 value)
{
	return 0;
}
#endif

