
#include <bsp_om.h>

#define  mr_print_error(fmt, ...)   (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_MEMREPAIR, "[memrepair]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define  mr_print_info(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MODU_MEMREPAIR, "[memrepair]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

