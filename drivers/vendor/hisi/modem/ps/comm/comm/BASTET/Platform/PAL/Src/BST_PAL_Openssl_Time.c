
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "v_timer.h"
#include "BST_OS_Log.h"
/*lint -e767*/
#define THIS_FILE_ID PS_FILE_ID_BST_PAL_OPENSSL_TIME_C
/*lint +e767*/
/*****************************************************************************
  2 结构体声明
*****************************************************************************/
struct timeval {
  long    tv_sec;         /* seconds */
  long    tv_usec;        /* microseconds */
};

/******************************************************************************
   2 函数实现
******************************************************************************/
int gettimeofday( struct timeval *tp, void *tzp )
{
    BST_UINT32             ulRet;
    SYS_T                  stSysTime;
    DATE_T                 stDate;
    TIME_T                 stTime;
    BST_UINT32             ulRetSecHigh;
    BST_UINT32             ulRetSecLow;

    if ( BST_NULL_PTR == tp )
    {
        return 0;
    }

    stSysTime.uwYear       = 1970;
    stSysTime.ucMonth      = 1;
    stSysTime.ucDate       = 1;
    stSysTime.ucHour       = 0;
    stSysTime.ucMinute     = 0;
    stSysTime.ucSecond     = 0;
    stSysTime.ucWeek       = 1;
    /*lint -e718 -e732 -e746*/
    ulRet                  = VOS_GetSysTime( &stSysTime );
    /*lint +e718 +e732 +e746*/
    if ( VOS_OK != ulRet )
    {
        BST_DBG_LOG1( "VOS_GetSysTime ulRet: failed", ulRet );
        return 0;
    }

    stDate.uwYear          = stSysTime.uwYear;
    stDate.ucMonth         = stSysTime.ucMonth;
    stDate.ucDate          = stSysTime.ucDate;

    stTime.ucHour          = stSysTime.ucHour;
    stTime.ucMinute        = stSysTime.ucMinute;
    stTime.ucSecond        = stSysTime.ucSecond;

    ulRet                  = VOS_SysSecsSince1970( &stDate, &stTime, 0, &ulRetSecHigh, &ulRetSecLow );
    if ( VOS_OK != ulRet )
    {
        BST_DBG_LOG1( "VOS_SysSecsSince1970 ulRet: failed", ulRet );
        return 0;
    }

    tp->tv_sec             = (long)ulRetSecLow;
    tp->tv_usec            = 0;

    return 0;
}
