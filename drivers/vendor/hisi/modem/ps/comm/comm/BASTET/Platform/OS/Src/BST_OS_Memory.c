

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_OS_Memory.h"
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_OS_MEMORY_C
/*lint +e767*/
/******************************************************************************
   2 宏定义
******************************************************************************/
/*****************************************************************************
  3 函数声明
*****************************************************************************/
/******************************************************************************
   4 私有定义
******************************************************************************/
/******************************************************************************
   5 全局变量定义
******************************************************************************/
/******************************************************************************
   6 函数实现
******************************************************************************/

BST_VOID BST_OS_MemInit( BST_VOID )
{
    BST_OS_PalMemInit();
}


/*lint -sem(BST_OS_PalMemset,custodial(1))*/
BST_VOID BST_OS_Memset(
    BST_VOID           *pData,
    BST_INT32           lValue,
    BST_OS_MEMSIZE_T    ulSize )
{
    BST_OS_PalMemset( pData, lValue, ulSize );
}
BST_INT8 BST_OS_Memcmp(
    const BST_VOID     *pData1,
    const BST_VOID     *pData2,
    BST_OS_MEMSIZE_T    ulSize )
{
    return( ( BST_INT8 )BST_OS_PalMemcmp( pData1, pData2, ulSize ) );
}


/*lint -sem(BST_OS_PalMemcpy,custodial(1))*/
BST_VOID BST_OS_Memcpy(
    BST_VOID           *pDst,
    const BST_VOID     *pSrc,
    BST_OS_MEMSIZE_T    ulSize )
{
    BST_OS_PalMemcpy( pDst, pSrc, ulSize );
}
BST_VOID    BST_OS_Memmove (
    BST_VOID           *pDst,
    BST_VOID           *pSrc,
    BST_OS_MEMSIZE_T    ulSize )
{
    BST_OS_PalMemmove( pDst, pSrc, ulSize );
}

