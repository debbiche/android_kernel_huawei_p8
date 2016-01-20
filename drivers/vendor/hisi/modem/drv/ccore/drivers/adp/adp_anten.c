/*lint -save -e537*/
#include <product_config.h>
#include <drv_anten.h>
#include <drv_gpio.h>
#include <RfNvId.h>
#include <comm_nv_def.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <drv_anten.h>
#include <hi_anten.h>
#include <gpio_balong.h>
#include <bsp_om.h>
#include <bsp_nvim.h>
#include <bsp_hardtimer.h>
#include <bsp_mipi.h>
#include <bsp_dump.h>
#include <anten_balong.h>
#include <pintrl_balong.h>
/*lint -restore*/

#ifdef HI_K3_ANTEN
#define ANTEN_MODEM0_GPIO GPIO_158
#define ANTEN_MODEM1_GPIO GPIO_159
#else
#define ANTEN_MODEM0_GPIO             (GPIO_0_15)
#define ANTEN_MODEM1_GPIO             (GPIO_0_15)
#endif
extern int bsp_pinctrl_mux_set(unsigned int gpio,unsigned int type);

#define anten_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ANTEN, "[anten]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define anten_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_INFO,  BSP_MODU_ANTEN, "[anten]: "fmt, ##__VA_ARGS__))

DRV_DRV_ANTEN_GPIO_STRU    anten_stru;
NV_FEM_GPIO_MIPI_CTRL_STRU g_femio_info; /*gpio、mipi控制*/
NV_FEMPIN_TO_GPIO_STRU     g_pin2gpio;

typedef struct {
    u32 caller_addr;
    u32 para1;
    u32 para2;
    u32 para3;
    u32 timestamp;
    int ret;
}FEMIO_CALLINFO_S;

FEMIO_CALLINFO_S g_femio_debug[16]; 
extern void bsp_pinctrl_modify_dts(u32 step ,unsigned int type, unsigned int gpio, unsigned int value);

int anten_nv_init(void)
{
    unsigned int ret = 0;

    /* Get ANTEN NV data by id.*/
    ret = bsp_nvm_read(NV_ID_DRV_ANTEN_CFG,(u8*)&anten_stru,sizeof(DRV_DRV_ANTEN_GPIO_STRU));
    if (ret !=  0){
        anten_print_error("anten_gpio read NV=0x%x, ret = %d \n",NV_ID_DRV_ANTEN_CFG, ret);
    }

    ret = bsp_nvm_read(en_NV_Item_FEM_GPIO_MIPI_CTRL, (u8*)&g_femio_info, sizeof(NV_FEM_GPIO_MIPI_CTRL_STRU));
    if (ret !=  0){
        anten_print_error("anten_gpio read NV=0x%x, ret = %d \n",en_NV_Item_FEM_GPIO_MIPI_CTRL, ret);
    }

    ret = bsp_nvm_read(en_NV_Item_FEM_PIN_TO_GPIO, (u8*)&g_pin2gpio, sizeof(NV_FEMPIN_TO_GPIO_STRU));
    if (ret !=  0)
    {
        anten_print_error("anten_gpio read NV=0x%x, ret = %d \n",en_NV_Item_FEM_GPIO_MIPI_CTRL, ret);
    }
    return (int)ret;
}

int modem_id_to_gpio(unsigned int modem_id)
{
    unsigned int i = 0;
    unsigned int size = sizeof(DRV_DRV_ANTEN_GPIO_STRU)/sizeof(DRV_DRV_ANTEN_GPIO_CFG);

    for(i = 0;i < size;i++)
    {
        if(modem_id == anten_stru.anten_gpio[i].modem_id && (1 == anten_stru.anten_gpio[i].used))
        {
            return (int)(anten_stru.anten_gpio[i].gpio);
        }
        else if(modem_id == anten_stru.anten_gpio[i].modem_id && (1 != anten_stru.anten_gpio[i].used))
        {
            anten_print_error("anten gpio nv is not config.\n");
            return GPIO_ERROR;
        }

    }

    return GPIO_ERROR;

}


unsigned int rf_pin_to_gpio(unsigned int pin)
{

    if(pin >= FEM_PIN_MAX){        
        anten_print_error("para error, pin = %d.\n", pin);
        return GPIO_NUM_INVAILId;
    }

    return g_pin2gpio.cfg[pin];
}
/*****************************************************************************
 函 数 名  : drv_anten_lock_status_get
 功能描述  : 获取有线与无线的连接状态
 输入参数  : none
 输出参数  : 0 - 无线连接
             1 - 有线连接
 返 回 值  : 0 - 成功
             其它为失败

*****************************************************************************/
int drv_anten_lock_status_get(PWC_COMM_MODEM_E modem_id, unsigned int *status)
{
#ifdef CONFIG_ANTEN
    int value = 0;
    int gpio = 0;
#endif
    if(NULL == status || (modem_id >= PWC_COMM_MODEM_BUTT))
    {
        anten_print_error("para is error, modem id = %d, status = 0x%x.\n", modem_id, status);
        return ANTEN_ERROR;
    }
    
#ifdef CONFIG_ANTEN
    gpio = modem_id_to_gpio((unsigned int)modem_id);
    if(GPIO_ERROR == gpio)
    {
        anten_print_error("modem_id_to_gpio is fail, value = %d.\n", gpio);
        return -1;
    }   

    value = bsp_gpio_get_value((unsigned int )gpio);/* [false alarm]:屏蔽Fortify错误 */
    if(GPIO_ERROR == value)
    {
        anten_print_error("bsp_gpio_get_value is fail, value = %d.\n", value);
        return -1;
    }

    *status = value;/*lint !e732*/
#else
    *status = 1;
#endif
    return 0;
}


/*****************************************************************************
 函 数 名  : drv_anten_int_install
 功能描述  : 中断注册函数，用来获得当前的天线状态
 输入参数  : routine   - 中断处理函数
             para      - 保留字段
 输出参数  : none
 返 回 值  : void

*****************************************************************************/
void drv_anten_int_install(PWC_COMM_MODEM_E modem_id, void* routine, int para)
{
    if(NULL == routine || (modem_id >= PWC_COMM_MODEM_BUTT))
    {
        anten_print_error("para is error, modem id = %d, routine = 0x%x, para = %d.\n", modem_id, routine, para);
        return ;
    }
#ifdef CONFIG_ANTEN
    bsp_anten_int_install(modem_id, routine, para);
#endif
}

int drv_anten_ipc_reg_fun(void)
{
    return 0;
}

void fem_io_debuginfo_add(void* caller,u32 para1,u32 para2,u32 para3,int ret)
{
    u32 i = para1%FEM_IO_CFG_MAX;
    g_femio_debug[i].caller_addr = (u32) caller;
    g_femio_debug[i].para1       = para1;
    g_femio_debug[i].para2       = para2;
    g_femio_debug[i].para3       = para3;
    g_femio_debug[i].timestamp   = bsp_get_slice_value();
    g_femio_debug[i].ret         = ret;
}


static int fem_iocfg_group_mipi(u32 nv_index)
{
    int i        = 0;
    int ret      = 0;
    u8  mipi_chn = 0;
    u8  slave_id = 0;
    u8  offset   = 0;
    u8  data     = 0;

    /*发送mipi指令*/
    for(i=0; i<FEM_MIPI_CMD_MAX; i++){/*todo*/
        mipi_chn = g_femio_info.cfg[nv_index].mipi_ctrl.cmd[i].mipi_chn;
        slave_id = g_femio_info.cfg[nv_index].mipi_ctrl.cmd[i].slave_id;
        offset     = g_femio_info.cfg[nv_index].mipi_ctrl.cmd[i].reg_offset;
        data     = g_femio_info.cfg[nv_index].mipi_ctrl.cmd[i].value;
        
        ret = bsp_mipi_write(mipi_chn, slave_id, offset, data);
        if(ret){
            anten_print_error("mipi send data failed! mipi id =%d,slave_id = %d,offset = %d\n", \
                mipi_chn,slave_id,offset);
            return ret;
        }
    }

    return 0;
}


static int fem_iocfg_group_gpio(u32 nv_index, u32 mux)
{
    u32 i;
    int ret;
    u32 pin_mask  = g_femio_info.cfg[nv_index].gpio_ctrl.pin_mask.value;
    u32 pin_value = g_femio_info.cfg[nv_index].gpio_ctrl.pin_level.level;
    u32 gpio_num  = 0;

    if((nv_index > FEM_IO_CFG_MAX) || (mux > 1)){
        anten_print_error("para error ! nv index is %d,mux is %d\n", nv_index, mux);
        return -1;
    }

    for(i=0; i<FEM_PIN_MAX; i++){
        if(pin_mask & ((u32)1 << i)){
            gpio_num = rf_pin_to_gpio(i);
            if(GPIO_NUM_INVAILId == gpio_num){
                anten_print_error("get gpio num is error %d!\n",gpio_num);
                return -1;
            }
            /*set pin mux*/
            bsp_pinctrl_modify_dts(MODEM_PIN_NORMAL, MODEM_IO_MUX, gpio_num, mux);
            ret = bsp_pinctrl_mux_set(gpio_num, mux);
            if(ret){
                anten_print_error("set pin mux error! gpio num=%d,mux=%d\n",gpio_num,mux);
                return ret;
            }

            /*if pin is gpio function,set direction and pin level*/
            if(0 == mux){
                pin_value = (u32)!!(pin_value & ((u32)1<<i));
                bsp_pinctrl_modify_dts(MODEM_PIN_NORMAL, MODEM_GPIO_OUTPUT, gpio_num, pin_value);                
                bsp_gpio_direction_output(gpio_num, pin_value);
            }
        }
    }

    return 0;
}


static int fem_iocfg_group_gpio_mipi_with_nv(u32 nv_index, u32 mux)
{
    FEMIO_CTRL_MODE type = FEMIO_CTRL_TOP;
    int ret = 0;

    if(nv_index >= FEM_IO_CFG_MAX){
        anten_print_error("para is error, nv_index = %d\n", nv_index);
        return -1;
    }

    if(mux > 1){
        anten_print_error("para is error, mux = %d\n", mux);
        return -1;
    }

    type = g_femio_info.cfg[nv_index].mode;
    switch (type){
        case FEMIO_CTRL_TOP:
            ret = 0;
            break;
        case GPIO_ONLY:/*配置GPIO*/
            ret =  fem_iocfg_group_gpio(nv_index,mux);
            break;
        case MIPI_ONLY:/*配置MIPI*/
            ret = fem_iocfg_group_mipi(nv_index);
            break;
        case GPIO_MIPI:
            ret =  fem_iocfg_group_gpio(nv_index,mux);
            ret |= fem_iocfg_group_mipi(nv_index);
            break;
        default:
            anten_print_error("para is error, type = %d\n", type);
            ret = -1;
            break;
    }
    return ret;
}


int mdrv_fem_ioctrl(FEMIO_CTRL_TYPE ctrl_type, unsigned int para1, unsigned int para2, unsigned int para3)
{
    int ret = 0;
    switch(ctrl_type)
    {
        case CFG_RFFE_GROUP_GPIO_MIPI:
            ret = fem_iocfg_group_gpio_mipi_with_nv(para1, para2);
            break;

        default:
            ret = -1;
            break;
    }

    
    /*lint -save -e718 -e746 -e628 -e64*/
    fem_io_debuginfo_add(__builtin_return_address(0),para1,para2,para3,ret);    
    /*lint -save restore*/

    return ret;
}

