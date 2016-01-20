#include <osl_common.h>
#include <osl_malloc.h>
#include <memLib.h>
#include "bsp_shared_ddr.h"
#include "bsp_memmap.h"
#include <bsp_hardtimer.h>
#include "gpio_balong.h"
#include "bsp_om.h"

#include "pintrl_balong.h"

#define GPIO_DIR(x)            (HI_GPIO0_REGBASE_ADDR + (x) * 0x1000 + 0x400)
#define GPIO_SYS_DIR(x)        (HI_GPIO22_REGBASE_ADDR + (x - 22) * 0x1000 + 0x400)
#define GPIO_DATA(x, y)        (HI_GPIO0_REGBASE_ADDR + (x) * 0x1000 + (1 << (2 + y)))
#define GPIO_SYS_DATA(x, y)    (HI_GPIO22_REGBASE_ADDR + (x - 22) * 0x1000 + (1 << (2 + y)))
#define GPIO_BIT(x, y)         ((x) << (y))
#define GPIO_IS_SET(x, y)      (((x) >> (y)) & 0x1)

#define ios_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IOS, "[ios]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define ios_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_INFO,  BSP_MODU_IOS, "[ios]: "fmt, ##__VA_ARGS__))
#define PINCONFIG_NF_VAL     0xffff
struct pintrl_stru{
    unsigned int ugpiog;
    unsigned int ugpio_bit;
    unsigned int uiomg_off;
    unsigned int iomg_val;
    unsigned int uiocg_off;
    unsigned int iocg_val;
    unsigned int gpio_dir;
    unsigned int gpio_val;
};

struct pin_addr{
    unsigned int low;
    unsigned int high;
};

struct modem_pintrl_cfg
{
    union 
    {
        struct pin_addr addr;
        struct pintrl_stru * pinstu;
    }pintrl_addr[MODEM_PIN_BUTT];
    unsigned int pintrl_size[MODEM_PIN_BUTT];
    unsigned int reserved;
};

/*void pintrl_data_convert(PINTRL_STRU *iocfg_stru, PINTRL_STRU *iocfg_table)
{
    iocfg_stru->ugpiog    = cpu_to_fdt32(iocfg_table->ugpiog);
    iocfg_stru->ugpio_bit = cpu_to_fdt32(iocfg_table->ugpio_bit);
    iocfg_stru->uiomg_off = cpu_to_fdt32(iocfg_table->uiomg_off);
    iocfg_stru->iomg_val  = cpu_to_fdt32(iocfg_table->iomg_val);
    iocfg_stru->uiocg_off = cpu_to_fdt32(iocfg_table->uiocg_off);
    iocfg_stru->iocg_val  = cpu_to_fdt32(iocfg_table->iocg_val);
    iocfg_stru->gpio_dir  = cpu_to_fdt32(iocfg_table->gpio_dir);
    iocfg_stru->gpio_val  = cpu_to_fdt32(iocfg_table->gpio_val);
}*/

/* config modem pintrl status */
struct modem_pintrl_cfg *g_pin_cfg = NULL;
void pintrl_config(struct pintrl_stru *iocfg_stru, unsigned int length)
{
    unsigned int i;
    unsigned int uregv, value, addr;
    unsigned int gpionum = 0;
    if(NULL == iocfg_stru || (0 == length))
    {
        return;
    }
    
    for (i = 0; i < length; i++) {
        
        //pintrl_data_convert(&iocfg_stru, &(iocfg_table[i]));
        
        /* gpio controller register */
        if ((!iocfg_stru[i].gpio_dir) && (PINCONFIG_NF_VAL != iocfg_stru[i].gpio_dir) ) {    /* direction is in */

            gpionum = 8*iocfg_stru[i].ugpiog + iocfg_stru[i].ugpio_bit;
            bsp_gpio_direction_input(gpionum);
            
        } else {    /* direction is out */
            if(PINCONFIG_NF_VAL!=iocfg_stru[i].gpio_val){                
                gpionum = 8*iocfg_stru[i].ugpiog + iocfg_stru[i].ugpio_bit;
                bsp_gpio_direction_output(gpionum , iocfg_stru[i].gpio_val);

            }
        }

        uregv = ((iocfg_stru[i].ugpiog << 3) + iocfg_stru[i].ugpio_bit);

        /* set iomg register */
        if ((0xfff != iocfg_stru[i].uiomg_off) &&
            (PINCONFIG_NF_VAL != iocfg_stru[i].iomg_val)) {
            value = iocfg_stru[i].iomg_val;

            if (uregv <= 164) {
                addr = HI_PINTRL_REG_ADDR + (iocfg_stru[i].uiomg_off);
            } else {
                addr = HI_PINTRL_SYS_REG_ADDR + (iocfg_stru[i].uiomg_off);
            }

            writel(value, addr);

        }

        /* set iocg register */
        if(PINCONFIG_NF_VAL != iocfg_stru[i].iocg_val){
            if (uregv <= 164) {
                addr = HI_PINTRL_REG_ADDR + 0x800 + (iocfg_stru[i].uiocg_off);
            } else {
                addr = HI_PINTRL_SYS_REG_ADDR + 0x800 + (iocfg_stru[i].uiocg_off);
            }
            value = readl(addr) & (~0x03);
            writel(value | iocfg_stru[i].iocg_val, addr);
        }

    }

}


struct pintrl_stru * get_pintrl(unsigned int mode, unsigned int ugpiog, unsigned int ugpio_bit)
{
    unsigned int begin = 0;
    unsigned int end   = 0;
    unsigned int i     = 0;
    unsigned int gpio  = 0;
    unsigned int gpio_tmp = 0;
    struct pintrl_stru *pincfg = NULL;
    struct pintrl_stru *root   = NULL;
    struct modem_pintrl_cfg *pintrl_cfg = (struct modem_pintrl_cfg *)g_pin_cfg;

    if(NULL == (void *)pintrl_cfg->pintrl_addr[mode].addr.low)
    {
        return NULL;
    }

    root = (struct pintrl_stru *)(pintrl_cfg->pintrl_addr[mode].addr.low);

    end  = pintrl_cfg->pintrl_size[mode] - 1;
    gpio = ugpiog * 8 + ugpio_bit;
    for(begin = 0; begin <= end ; )
    {
        i        = begin + ((end - begin)/2);
        pincfg   = &root[i];
        gpio_tmp = pincfg->ugpiog*8 + pincfg->ugpio_bit;
        if(gpio < gpio_tmp)
        {
            end = i - 1;
        }
        else if(gpio > gpio_tmp)
        {
            begin = i + 1;
        }
        else{
            return pincfg;
        }
    }

    return NULL;

}

int is_in_modem_pintrl(unsigned int gpio)
{
    unsigned int group = gpio / GPIO_MAX_NUMBER;
    unsigned int pin = gpio % GPIO_MAX_NUMBER;

    if(NULL != get_pintrl(MODEM_PIN_NORMAL,group,pin))
        return 0;

    return -1;
    
}
void bsp_pinctrl_modify_dts(u32 step ,unsigned int type, unsigned int gpio, unsigned int value)
{
    unsigned int group = 0;
    unsigned int pin   = 0;
    
    struct pintrl_stru *pin_cfg = NULL;
    
    if(gpio > GPIO_TOTAL || (type >= MODEM_IO_BUTT) || (step >= MODEM_PIN_BUTT))
    {
        ios_print_error("para is error, step = %d, gpio = %d, type = %d, value = %d.\n", step, gpio, type, value);        
        return ;
    }

    group = gpio / GPIO_MAX_NUMBER;
    pin   = gpio % GPIO_MAX_NUMBER;
    
    pin_cfg = get_pintrl(step, group, pin);
    if(NULL == pin_cfg)
    {
        ios_print_error("get_pintrl is fail, pin_cfg = 0x%x.\n", pin_cfg);  
        return ;
    }
    
    switch(type)
    {
        case MODEM_IO_MUX:
            if(PINCONFIG_NF_VAL  != pin_cfg->iomg_val){
                pin_cfg->iomg_val = value;
            }
            break;
            
        case MODEM_IO_CFG:
            if(PINCONFIG_NF_VAL  != pin_cfg->iocg_val){
                pin_cfg->iocg_val = value;
            }
            break;
            
        case MODEM_GPIO_INPUT:
            if(PINCONFIG_NF_VAL  != pin_cfg->iocg_val){
                pin_cfg->iomg_val = 0;            
            }

            if(PINCONFIG_NF_VAL  != pin_cfg->gpio_dir){
                pin_cfg->gpio_dir = 0;
            }
            break;
            
        case MODEM_GPIO_OUTPUT:
            if(PINCONFIG_NF_VAL  != pin_cfg->iomg_val){
                pin_cfg->iomg_val = 0;
            }

            if(PINCONFIG_NF_VAL  != pin_cfg->gpio_dir){
                pin_cfg->gpio_dir = 1;            
            }

            if(PINCONFIG_NF_VAL  != pin_cfg->gpio_val){
                pin_cfg->gpio_val = !!value;            
            }
            break;
            
        default:
            ios_print_error("type is error, type = %d.\n", type);
            return ;
        
    }

}


void bsp_set_pintrl(unsigned int type, unsigned int gpio, unsigned int value)
{
    unsigned int group = 0;
    unsigned int pin = 0;
    
    struct pintrl_stru *normal_stru = NULL;

    if(gpio > GPIO_TOTAL || (type >= MODEM_IO_BUTT))
    {
        ios_print_error("para is error, gpio = %d, type = %d, value = %d.\n", gpio, type, value);        
        return ;
    }

    group = gpio / GPIO_MAX_NUMBER;
    pin   = gpio % GPIO_MAX_NUMBER;
    
    normal_stru = get_pintrl(MODEM_PIN_NORMAL, group, pin);
    if(NULL == normal_stru)
    {
        ios_print_error("get_pintrl is fail, normal_stru = 0x%x.\n", normal_stru);  
        return ;
    }

    bsp_pinctrl_modify_dts(MODEM_PIN_NORMAL, type, gpio, value);
    bsp_pinctrl_modify_dts(MODEM_PIN_POWERDOWN, type, gpio, value);

    pintrl_config(normal_stru, 1);

}


int bsp_get_pintrl(unsigned int type, unsigned int gpio, unsigned int *value)
{
    int ret = 0;
    unsigned int group = 0;
    unsigned int pin = 0;
    struct pintrl_stru *normal_stru = NULL;

    if(gpio > GPIO_TOTAL || (type >= MODEM_IO_BUTT) || (NULL == value))
    {
        ios_print_error("para is error, gpio = %d, type = %d, value = %d.\n", gpio, type, value);        
        return -1;
    }

    group = gpio / GPIO_MAX_NUMBER;
    pin = gpio % GPIO_MAX_NUMBER;

    normal_stru = get_pintrl(MODEM_PIN_NORMAL, group, pin);
    if(NULL == normal_stru)
    {
        ios_print_error("get_pintrl is fail, normal_stru = 0x%x.\n", normal_stru);  
        return -1;
    }

    switch(type)
    {
        case MODEM_IO_MUX:
            *value   = normal_stru->iomg_val;
            ret = 0;
            break;
            
        case MODEM_IO_CFG:
            *value   = normal_stru->iocg_val;
            ret = 0;
            break;
            
        default:
            ios_print_error("type is error, type = %d.\n", type);
            ret = -1;
    }

    return ret;
}

void modem_pintrl_debug(int mode)
{
    unsigned int i = 0;    
    unsigned int length = 0;
    struct pintrl_stru *pintrl_stru = NULL;
    struct modem_pintrl_cfg *pintrl_cfg = g_pin_cfg;

    if((MODEM_PIN_INIT != mode) && (MODEM_PIN_NORMAL != mode) && (MODEM_PIN_POWERDOWN != mode))
    {
        ios_print_error("modem pintrl mode is error, mode=%d.\n", mode);
        return ;
    }
    
    length = pintrl_cfg->pintrl_size[mode];
    pintrl_stru = (struct pintrl_stru *)pintrl_cfg->pintrl_addr[mode].addr.low;
    
    ios_print_info("iocfg table length is %d\n", length);
    ios_print_info("  group  pin  iomg  iocg  dir  value\n");
    
    for(i = 0;i < length;i++)
    {
        //pintrl_data_convert(&iocfg_stru, &(iocfg_table[i]));

        ios_print_info("%5d %5d %5d %5d %5d %5d\n", pintrl_stru[i].ugpiog,pintrl_stru[i].ugpio_bit,\
                                                            pintrl_stru[i].iomg_val,pintrl_stru[i].iocg_val,\
                                                            pintrl_stru[i].gpio_dir,pintrl_stru[i].gpio_val);
                                                          

    }

}

void modem_pinctrl_init(void)
{
    struct modem_pintrl_cfg *pin_source = SHM_MEM_MODEM_PINTRL_ADDR;
    u32 addr_temp = 0;
    u32 pin_num   = pin_source->pintrl_size[MODEM_PIN_INIT] + pin_source->pintrl_size[MODEM_PIN_NORMAL]\
                  + pin_source->pintrl_size[MODEM_PIN_POWERDOWN];

    u32 size      = sizeof(struct modem_pintrl_cfg) + pin_num * sizeof(struct pintrl_stru);
    g_pin_cfg = osl_malloc(size);
    if(NULL == g_pin_cfg){
        ios_print_error("modem pinctrl malloc fail!size:0x%x\n",SHM_MEM_MODEM_PINTRL_SIZE);
        g_pin_cfg = SHM_MEM_MODEM_PINTRL_ADDR;
        return ;
    }
    memcpy(g_pin_cfg,SHM_MEM_MODEM_PINTRL_ADDR,size);
    addr_temp = (u32)g_pin_cfg + sizeof(struct modem_pintrl_cfg);
    g_pin_cfg->pintrl_addr[MODEM_PIN_INIT].addr.low = addr_temp;
    addr_temp = addr_temp + (g_pin_cfg->pintrl_size[MODEM_PIN_INIT])*sizeof(struct pintrl_stru);
    g_pin_cfg->pintrl_addr[MODEM_PIN_NORMAL].addr.low = addr_temp;
    addr_temp = addr_temp + (g_pin_cfg->pintrl_size[MODEM_PIN_NORMAL])*sizeof(struct pintrl_stru);
    g_pin_cfg->pintrl_addr[MODEM_PIN_POWERDOWN].addr.low = addr_temp;
    if(size > SHM_MEM_MODEM_PINTRL_SIZE){
        ios_print_error("modem pinctrl memory expend 6k!\n");
    }
    ios_print_error("modem pinctrl init ok!\n");
}
void modem_pintrl_config(int mode)
{

    if((MODEM_PIN_INIT != mode) && (MODEM_PIN_NORMAL != mode) && (MODEM_PIN_POWERDOWN != mode))
    {
        ios_print_error("modem pintrl mode is error, mode=%d.\n", mode);
        return ;
    }

    pintrl_config((struct pintrl_stru *)(g_pin_cfg->pintrl_addr[mode].addr.low), g_pin_cfg->pintrl_size[mode]);

}

