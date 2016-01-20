

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_DSPP_TrsLayer.h"
#include "BST_DSPP_CtrLayer.h"
#include "BST_DSPP_AppLayer.h"
#include "BST_DBG_MemLeak.h"
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_DSPP_CTRLAYER_CPP
/*lint +e767*/
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BST_DSPP_CP_OFFSET_ADDR         ( BST_DSPP_TPA_OFFSET_ADDR )
/*****************************************************************************
  3 函数实现
******************************************************************************/


BST_DSPP_CCtrDl::BST_DSPP_CCtrDl(
    BST_DSPP_LAYER_ID_ENUM_UINT8  enLayerId,
    BST_DSPP_VERION_ENUM_UINT8    enLayerVer )
    : BST_DSPP_CDlLayerBase ( enLayerId, enLayerVer )
{
}

BST_DSPP_CCtrDl::~BST_DSPP_CCtrDl( BST_VOID )
{
}

BST_DSPP_CCtrDlVa::BST_DSPP_CCtrDlVa ( BST_VOID )
    : BST_DSPP_CCtrDl( BST_DSPP_LAYER_CTR, BST_DSPP_VER_A )
{
}

BST_DSPP_CCtrDlVa::~BST_DSPP_CCtrDlVa ( BST_VOID )
{
}

BST_ERR_ENUM_UINT8  BST_DSPP_CCtrDlVa::ParseHeadInfo(
    BST_DSPP_HEAD_STRU           *const pstHead,
    BST_UINT8 const              *pucData,
    BST_UINT16                    usLength )
{
    if ( BST_NULL_PTR == pstHead )
    {
        return BST_ERR_INVALID_PTR;
    }
    if ( BST_NULL_PTR == pucData )
    {
        return BST_ERR_INVALID_PTR;
    }
    if ( usLength < BST_DSPP_CP_HEAD_LEN )
    {
        BST_RLS_LOG1( "BST_DSPP_CCtrDlVa::ParseHeadInfo usLength=%u",
                      usLength );
        return BST_ERR_PAR_LEN;
    }
    pstHead->stCpVerA.usObjtAddr    = BST_DSPP_U8_TO_U16( pucData[BST_DSPP_CP_OBJT_ADD_L],
                                                          pucData[BST_DSPP_CP_OBJT_ADD_H] );
    pstHead->stCpVerA.usOfstAddr    = BST_DSPP_U8_TO_U16( pucData[BST_DSPP_CP_OFST_ADD_L],
                                                          pucData[BST_DSPP_CP_OFST_ADD_H] );
    pstHead->stCpVerA.ucActType     = ( BST_ERR_ENUM_UINT8 )pucData[BST_DSPP_CP_OPRT_ERR];
    pstHead->enApVer                = ( BST_DSPP_VERION_ENUM_UINT8 )pucData[BST_DSPP_CP_APL_VER];
    return BST_NO_ERROR_MSG;
}

BST_ERR_ENUM_UINT8  BST_DSPP_CCtrDlVa::UnpackInternal (
    BST_DSPP_HEAD_STRU          *const pstRxHeadInfo,
    BST_DSPP_VERION_ENUM_UINT8  *const penNextVer )
{
    BST_ERR_ENUM_UINT8              ucRtnVal;

    if ( ( BST_NULL_PTR == pstRxHeadInfo )
      || ( BST_NULL_PTR == penNextVer ) )
    {
        return BST_ERR_LAYER_VER;
    }
    if ( ( BST_NULL_PTR == pstRxHeadInfo->pucData )
      || ( 0 == pstRxHeadInfo->usLength ) )
    {
        return BST_ERR_LAYER_VER;
    }

    ucRtnVal                        = ParseHeadInfo( pstRxHeadInfo,
                                      pstRxHeadInfo->pucData,
                                      pstRxHeadInfo->usLength );
    if ( BST_NO_ERROR_MSG != ucRtnVal )
    {
        BST_RLS_LOG1( "BST_DSPP_CCtrDlVa::UnpackInternal ParseHeadInfo Err=%u",
                      ucRtnVal );
        return BST_ERR_LAYER_VER;
    }
    pstRxHeadInfo->usLength        -= BST_DSPP_CP_HEAD_LEN;

    if( BST_TRUE == pstRxHeadInfo->ulZeroCopy )
    {
        if ( BST_DSPP_VER_VOID != pstRxHeadInfo->enApVer )
        {
            pstRxHeadInfo->pucData+= BST_DSPP_CP_HEAD_LEN;
           *penNextVer             = BST_DSPP_VER_A;
        }
        else
        {
            pstRxHeadInfo->pucData = BST_NULL_PTR;
           *penNextVer             = BST_DSPP_VER_A;
        }
        return BST_NO_ERROR_MSG;
    }
    if ( BST_DSPP_VER_VOID != pstRxHeadInfo->enApVer )
    {
        if ( 0 != pstRxHeadInfo->usLength )
        {
            BST_OS_MEMMOVE ( pstRxHeadInfo->pucData,
                             pstRxHeadInfo->pucData + BST_DSPP_CP_HEAD_LEN,
                             pstRxHeadInfo->usLength );
        }
        else
        {
            BST_OS_FREE ( pstRxHeadInfo->pucData );
        }
    }
    else if ( 0 != pstRxHeadInfo->usLength )
    {
        BST_OS_FREE ( pstRxHeadInfo->pucData );
    }
    *penNextVer                         = BST_DSPP_VER_A;

    return BST_NO_ERROR_MSG;
}

BST_DSPP_CDlLayerBase *BST_DSPP_CCtrDlVa::GetNextStrategy(
    BST_DSPP_VERION_ENUM_UINT8 enNextVer )
{
    delete this;

    switch ( enNextVer )
    {
        case BST_DSPP_VER_A:
            return ( BST_DSPP_CAppDlVa::GetInstance () );

        default:
            break;
    }

    return BST_NULL_PTR;
}

BST_DSPP_CCtrUlVa::BST_DSPP_CCtrUlVa ( BST_VOID )
    : BST_DSPP_CUlLayerBase ( BST_DSPP_LAYER_CTR, BST_DSPP_VER_A )
{
}

BST_DSPP_CCtrUlVa::~BST_DSPP_CCtrUlVa ( BST_VOID )
{
}

BST_ERR_ENUM_UINT8  BST_DSPP_CCtrUlVa::CheckObjtAdd(
    BST_DSPP_OBJTADDR_T  usObjtAddr )
{
    if ( BST_DSPP_IsObjIdValid ( usObjtAddr ) )
    {
        return BST_NO_ERROR_MSG;
    }
    else
    {
        return BST_ERR_UNREACHABLE;
    }
}

BST_ERR_ENUM_UINT8  BST_DSPP_CCtrUlVa::CheckOfstAdd (
    BST_DSPP_OFSTADDR_T  usOfstAddr )
{
    if ( ( BST_DSPP_BROADCASE_ADDRESS != usOfstAddr )
      && ( BST_DSPP_INVALID_ADDR != usOfstAddr ) )
    {
        return BST_NO_ERROR_MSG;
    }
    else
    {
        return BST_ERR_UNREACHABLE;
    }
}

BST_ERR_ENUM_UINT8  BST_DSPP_CCtrUlVa::PackInternal (
    BST_DSPP_HEAD_STRU         *const pstTxHeadInfo,
    BST_DSPP_VERION_ENUM_UINT8 *const pstNextVer )
{
    BST_UINT8              *pucSdu;
    BST_ERR_ENUM_UINT8      enRtnVal;

    if ( ( BST_NULL_PTR == pstTxHeadInfo )
      || ( BST_NULL_PTR == pstNextVer ) )
    {
        return BST_ERR_UNREACHABLE;
    }

    if ( BST_NULL_PTR == pstTxHeadInfo->pucData )
    {
        return BST_ERR_NO_MEMORY;
    }

    enRtnVal                = CheckObjtAdd ( pstTxHeadInfo->stCpVerA.usObjtAddr );
    if ( BST_NO_ERROR_MSG != enRtnVal )
    {
        BST_RLS_LOG1( "BST_DSPP_CCtrUlVa::PackInternal CheckObjtAdd Err=%u",
                      enRtnVal );
        return BST_ERR_UNREACHABLE;
    }

    enRtnVal                = CheckOfstAdd ( pstTxHeadInfo->stCpVerA.usOfstAddr );
    if ( BST_NO_ERROR_MSG != enRtnVal)
    {
        BST_RLS_LOG1( "BST_DSPP_CCtrUlVa::PackInternal CheckOfstAdd Err=%u",
                      enRtnVal );
        return BST_ERR_UNREACHABLE;
    }


    pucSdu                  = pstTxHeadInfo->pucData + BST_DSPP_CP_OFFSET_ADDR;

    pucSdu[ BST_DSPP_CP_OBJT_ADD_L ]    = BST_DSPP_U16_TO_U8L
                                        ( pstTxHeadInfo->stCpVerA.usObjtAddr );
    pucSdu[ BST_DSPP_CP_OBJT_ADD_H ]    = BST_DSPP_U16_TO_U8H
                                        ( pstTxHeadInfo->stCpVerA.usObjtAddr );
    pucSdu[ BST_DSPP_CP_OFST_ADD_L ]    = BST_DSPP_U16_TO_U8L
                                        ( pstTxHeadInfo->stCpVerA.usOfstAddr );
    pucSdu[ BST_DSPP_CP_OFST_ADD_H ]    = BST_DSPP_U16_TO_U8H
                                        ( pstTxHeadInfo->stCpVerA.usOfstAddr );
    pucSdu[ BST_DSPP_CP_OPRT_ERR   ]    = pstTxHeadInfo->stCpVerA.ucActType;
    pucSdu[ BST_DSPP_CP_APL_VER    ]    = pstTxHeadInfo->enApVer;
    pstTxHeadInfo->usLength            += BST_DSPP_CP_HEAD_LEN;
    pstTxHeadInfo->enTpVer              = BST_DSPP_VER_A;
    *pstNextVer                         = pstTxHeadInfo->enTpVer;
    return BST_NO_ERROR_MSG;
}

BST_DSPP_CUlLayerBase *BST_DSPP_CCtrUlVa::GetNextStrategy(
    BST_DSPP_VERION_ENUM_UINT8 enNextVer )
{
    BST_DSPP_CUlLayerBase              *pcNextStrategy;

    pcNextStrategy                      = BST_NULL_PTR;

    if ( BST_DSPP_VER_A == enNextVer )
    {
        pcNextStrategy                  = BST_DSPP_CTrsUlVa::GetInstance ();
    }
    else
    {
        pcNextStrategy                  = BST_NULL_PTR;
    }

    delete this;
    return pcNextStrategy;
}
