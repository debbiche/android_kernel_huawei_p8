
#include <stdio.h>
#include "pmom_cat.h"
#include "tcxo.h"
#include "adppower.h"

char *g_adppower_func_obj[] = {
    "PWRUP",
    "PWRDOWN",
    "BBPCLKENABLE",
    "BBPCLKDISABLE",
    "PLLENABLE",
    "PLLDISABLE",
    "OTHERS",
};
char *g_adppower_ops_obj[] = {
    "power_on",
    "power_off",
    "clk_on",
    "clk_off",
    "pll_on",
    "pll_off",
    "others",
};
char *g_err_obj[] = {
    "para_err",
    "ops_err",
    "dev_err",
    "other_err",
};
/*与枚举保持一致*/
char *g_module_obj[] = {
    "module_pa",
    "module_rf",
    "module_bbe",
    "module_bbpdrx",
    "module_bbpdpa",
    "module_bbpmem",
    "module_abb",
    "module_bbppwr",
    "module_dsp",
    "module_bbpsoc",
    "module_butt",
};
char *g_mode_obj[] = {
    "mode_gsm",
    "mode_wcdma",
    "mode_lte",
    "mode_tds",
    "mode_lcipher",
    "mode_butt",
};
char *g_modem_obj[] = {
    "modem0",
    "modem1",
    "modem_butt",
};
char *g_channel_obj[] = {
    "channel0",
    "channel1",
    "channel_butt",
};

char *g_adppower_dev_obj[] = {
    "pa",
    "rfic",
    "irmbbp",
    "g1bbp",
    "g2bbp",
    "tbbp",
    "wbbp",
    "twbbp",
    "bbppll",
    "bbepll",
    "abbch0g",
    "abbch0lwt",
    "abbch1g",
    "abbch1lwt",
    "dev_butt"
};

int adppower_msg_parse(unsigned int typeid, char *in_buf, unsigned int len, char *out_buf, unsigned int *out_ptr)
{
    int data_len = 0;
    struct adp_power_errlog *err_log_msg = (struct adp_power_errlog *)in_buf;
    struct adp_power_actlog  *act_log_msg = (struct adp_power_actlog *)in_buf;

    if(ADPPOWER_LOG_ACTUAL== typeid)
    {
        /* step1: 消息类型检查 */
        if(len != sizeof(*act_log_msg))
        {
            return PMOM_ERR;
        }
#if 0
        /* step2: 消息参数检查*/
        if(((unsigned int)act_log_msg->dev_id < sizeof(g_adppower_dev_obj)/sizeof(g_adppower_dev_obj[0])) \
            ||((unsigned int)act_log_msg->ops_id < sizeof(g_adppower_ops_obj)/sizeof(g_adppower_ops_obj[0]))\
            ||(act_log_msg->act_mode < sizeof(g_mode_obj)/sizeof(g_mode_obj[0]))\
            ||(act_log_msg->act_module < sizeof(g_module_obj)/sizeof(g_module_obj[0]))\
            ||(act_log_msg->act_modem < sizeof(g_modem_obj)/sizeof(g_modem_obj[0])))
        {
            data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ,"act_log para error");
            return PMOM_ERR;
        }
#endif
        /* step3: 格式化消息到out_buf: 以回车结束 */
        data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ,
                "task:0x%x, [do]%s, %s(%d-%s, %d-%s, %d-%s), vote:0x%x, ", act_log_msg->task_id,
                g_adppower_dev_obj[act_log_msg->dev_id],g_adppower_ops_obj[act_log_msg->ops_id],
                act_log_msg->act_mode,g_mode_obj[act_log_msg->act_mode],
                act_log_msg->act_module,g_module_obj[act_log_msg->act_module],
                act_log_msg->act_modem,g_modem_obj[act_log_msg->act_modem],
                act_log_msg->vote_lock);
        *out_ptr = *out_ptr + data_len;
        /* step4: 解析投票*/
        if(act_log_msg->vote_lock&((unsigned int)0x1 << PWC_COMM_MODE_GSM))
		{
			data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ,"GSM, ");
			*out_ptr = *out_ptr + data_len;
		}
		if(act_log_msg->vote_lock&((unsigned int)0x1 << PWC_COMM_MODE_WCDMA))
		{
			data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ,"WCDMA, ");
			*out_ptr = *out_ptr + data_len;
		}
		if(act_log_msg->vote_lock&((unsigned int)0x1 << PWC_COMM_MODE_LTE))
		{
			data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ, "LTE, ");
			*out_ptr = *out_ptr + data_len;
		}
		if(act_log_msg->vote_lock&((unsigned int)0x1 << PWC_COMM_MODE_TDS))
		{
			data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ, "TDS, ");
			*out_ptr = *out_ptr + data_len;
		}
        if(act_log_msg->vote_lock&((unsigned int)0x1 << PWC_COMM_MODE_LCIPHER))
		{
			data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ, "LCIPHER, ");
			*out_ptr = *out_ptr + data_len;
		}

        data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ, "\n");
		*out_ptr = *out_ptr + data_len;
    }
    else if (ADPPOWER_LOG_ERROR == typeid)
    {
        /* step1: 消息类型检查 */
        if(len != sizeof(*err_log_msg))
        {
            return PMOM_ERR;
        }
#if 0
        /* step2: 消息参数检查*/
        if(((unsigned int)err_log_msg->ops_id < sizeof(g_adppower_func_obj)/sizeof(g_adppower_func_obj[0]))\
            ||(err_log_msg->info_mode < sizeof(g_mode_obj)/sizeof(g_mode_obj[0]))\
            ||(err_log_msg->info_module < sizeof(g_module_obj)/sizeof(g_module_obj[0]))\
            ||(err_log_msg->info_modem < sizeof(g_modem_obj)/sizeof(g_modem_obj[0]))\
            ||(err_log_msg->err_id < sizeof(g_err_obj)/sizeof(g_err_obj)))
        {
            data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ,"err_log para error");
            return PMOM_ERR;
        }
#endif
        /*参数错误，则直接打印id，否则才打印字符串*/
        if(err_log_msg->err_id == ADPPOWER_PARAERR)
        {
            /* step3: 格式化消息到out_buf: 以回车结束 */
            data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ,
                    "task:0x%x, [err]:%s(%d, %d, %d),%d-%s\n", err_log_msg->task_id,g_adppower_func_obj[err_log_msg->ops_id],
                    err_log_msg->info_mode,err_log_msg->info_module,err_log_msg->info_modem,
                    err_log_msg->err_id,g_err_obj[err_log_msg->err_id]);

            *out_ptr = *out_ptr + data_len;
        }
        else
        {
            /* step3: 格式化消息到out_buf: 以回车结束 */
            data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ,
                    "task:0x%x, [err]:%s(%d-%s, %d-%s, %d-%s),%d-%s\n", err_log_msg->task_id,g_adppower_func_obj[err_log_msg->ops_id],
                    err_log_msg->info_mode,g_mode_obj[err_log_msg->info_mode],
                    err_log_msg->info_module,g_module_obj[err_log_msg->info_module],
                    err_log_msg->info_modem,g_modem_obj[err_log_msg->info_modem],
                    err_log_msg->err_id,g_err_obj[err_log_msg->err_id]);

            *out_ptr = *out_ptr + data_len;
        }
    }
    else
    {
        return PMOM_ERR;
    }

    return PMOM_OK;
}

