

enum
{
    NV_ICC_REQ      = 63,
    NV_ICC_REQ_SYS  = 64,
    NV_ICC_REQ_FLUSH= 65,
    NV_ICC_CNF      = 127,
    NV_ICC_RESUME   = 0xFF  /*请求恢复DDR中的nv数据*/
};
#define NV_ICC_MSG_TYPE_SUM (5)


struct nv_icc_stru
{
    unsigned int msg_type;          /*msg type*/
    unsigned int data_off;          /*data off */
    unsigned int data_len;          /*data len*/
    unsigned int ret;
    unsigned int itemid;
    unsigned int slice;
};

struct nv_icc_msg_name
{
    unsigned int msg_type;
    char         cpu[5];
    char         work[32];
};

int nvim_msg_parse(unsigned int typeid, char *in_buf, unsigned int len, char *out_buf, unsigned int *out_ptr);

