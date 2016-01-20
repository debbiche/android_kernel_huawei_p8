

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "BST_LIB_String.h"
#include "BST_OS_Memory.h"
#include "BST_DBG_MemLeak.h"
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_LIB_STRINT8U_CPP
/*lint +e767*/

BST_UINT16 BST_LIB_StrLen(
    const BST_CHAR          *pcData )
{
    BST_UINT16      usLen;
    usLen           = 0;
    if ( BST_NULL_PTR == pcData )
    {
        return 0;
    }
    /*ÕÒµ½'\0'½áÊø*/
    while( pcData[usLen] != '\0' )
    {
        usLen++;
    }
    return usLen;
}
BST_VOID   BST_LIB_StrAddByte(
    BST_LIB_StrStru        *pStr,
    const BST_CHAR          cData )
{
    BST_CHAR   *pucNewAddr;
    if ( BST_NULL_PTR == pStr )
    {
        return;
    }
    if( pStr->usUsed + 1 > pStr->usSize )
    {
        pucNewAddr  = (BST_CHAR *)BST_OS_MALLOC( pStr->usUsed + 1 + 1 );
        BST_OS_MEMCPY( pucNewAddr, pStr->pData, pStr->usUsed + 1 );
        BST_OS_FREE( pStr->pData );
        pStr->pData    = pucNewAddr;
        pStr->usSize ++;
    }
    pStr->pData[ pStr->usUsed ] = cData;
    pStr->usUsed++;
    pStr->pData[ pStr->usUsed ] = 0;
}

BST_VOID   BST_LIB_StrClear(
    BST_LIB_StrStru        *pStr )
{
    if ( BST_NULL_PTR == pStr )
    {
        return;
    }
    pStr->usUsed     = 0;
    if ( BST_NULL_PTR != pStr->pData )
    {
        pStr->pData[0]= 0;
    }
}

BST_VOID   BST_LIB_StrAddStr(
    BST_LIB_StrStru        *pStr,
    const BST_CHAR         *pcData )
{
    BST_CHAR   *pucNewAddr;
    BST_UINT16  usLen   = BST_LIB_StrLen( pcData );
    if ( BST_NULL_PTR == pStr )
    {
        return;
    }
    if( pStr->usUsed + usLen > pStr->usSize )
    {
        pucNewAddr      = (BST_CHAR *)BST_OS_MALLOC( pStr->usUsed + usLen + 1 );
        BST_OS_MEMCPY( pucNewAddr, pStr->pData, pStr->usUsed );
        BST_OS_FREE( pStr->pData );
        pStr->pData        = pucNewAddr;
        pStr->usSize    += usLen;
    }
    BST_OS_MEMCPY( pStr->pData + pStr->usUsed, pcData, usLen );
    pStr->usUsed += usLen;
    pStr->pData[ pStr->usUsed ] = 0;
}

BST_VOID   BST_LIB_StrAddBytes(
    BST_LIB_StrStru        *pStr,
    BST_CHAR               *pcData,
    BST_UINT16              usBufLen )
{
    BST_CHAR   *pucNewAddr;
    if ( BST_NULL_PTR == pStr )
    {
        return;
    }
    if( pStr->usUsed + usBufLen > pStr->usSize )
    {
        pucNewAddr      = (BST_CHAR *)BST_OS_MALLOC( pStr->usUsed + usBufLen + 1 );
        BST_OS_MEMCPY( pucNewAddr, pStr->pData, pStr->usUsed );
        BST_OS_FREE( pStr->pData );
        pStr->pData        = pucNewAddr;
        pStr->usSize    += usBufLen;
    }
    BST_OS_MEMCPY( pStr->pData + pStr->usUsed, pcData, usBufLen );
    pStr->usUsed += usBufLen;
    pStr->pData[ pStr->usUsed ] = 0;
}

BST_BOOL   BST_LIB_StrInsert(
    BST_UINT16              usOfset,
    BST_LIB_StrStru        *pStr,
    BST_CHAR               *pcData )
{
    BST_CHAR   *pucNewAddr;
    BST_UINT16  usDstLen;

    if ( BST_NULL_PTR == pStr )
    {
        return BST_FALSE;
    }
    if ( usOfset > pStr->usUsed )
    {
        return BST_FALSE;
    }
    usDstLen    = BST_LIB_StrLen( pcData );
    if( pStr->usUsed + usDstLen > pStr->usSize )
    {
        pucNewAddr      = (BST_CHAR *)BST_OS_MALLOC( pStr->usUsed + usDstLen + 1 );
        BST_ASSERT_NULL_RTN( pucNewAddr, BST_FALSE );
        BST_OS_MEMCPY( pucNewAddr, pStr->pData, usOfset );
        BST_OS_MEMCPY( pucNewAddr + usOfset, pcData, usDstLen );
        BST_OS_MEMCPY( pucNewAddr + usOfset + usDstLen, pStr->pData + usOfset, pStr->usUsed - usOfset );
        BST_OS_FREE( pStr->pData );
        pStr->pData        = pucNewAddr;
        pStr->usSize    += usDstLen;
    }
    else
    {
        BST_OS_MEMMOVE( pStr->pData + usOfset + usDstLen, pStr->pData + usOfset, pStr->usUsed - usOfset );
        BST_OS_MEMCPY( pStr->pData + usOfset, pcData, pStr->usUsed );
    }
    pStr->usUsed += usDstLen;
    pStr->pData[ pStr->usUsed ] = 0;
    return BST_TRUE;
}

BST_UINT16 BST_LIB_StrCmp(
    BST_UINT16              usOfset,
    const BST_CHAR         *pStr,
    BST_CHAR               *pcData,
    BST_UINT16              usBufLen )
{
    if ( BST_NULL_PTR == pStr )
    {
        return 1;
    }
    if ( BST_LIB_StrLen( pcData ) < usBufLen )
    {
        return 1;
    }
    /*lint -e571*/
    return( (BST_UINT16)BST_OS_MEMCMP( pStr + usOfset, pcData, usBufLen ) );
    /*lint +e571*/
}

BST_UINT16 BST_LIB_StrFind(
    const BST_CHAR         *pStr,
    const BST_UINT16        usSrcLen,
    const BST_CHAR         *pcData,
    const BST_UINT16        usDstLen)
{
    BST_UINT16  usPst, usLooper;

    if ( BST_NULL_PTR == pStr )
    {
        return BST_LIB_STR_NPO;
    }
    if ( BST_NULL_PTR == pcData )
    {
        return BST_LIB_STR_NPO;
    }
    if ( usDstLen > usSrcLen )
    {
        return BST_LIB_STR_NPO;
    }
    for ( usPst = 0; usPst <= usSrcLen - usDstLen; usPst++ )
    {
        for ( usLooper=0 ; usLooper < usDstLen; usLooper ++ )
        {
            if( *( pStr + usPst + usLooper ) != *( pcData + usLooper ) )
            {
                break;
            }
        }
        if( usLooper >= usDstLen )
        {
            return usPst;
        }
    }
    return BST_LIB_STR_NPO;
}

BST_ERR_ENUM_UINT8 BST_LIB_StrFill(
    BST_LIB_StrStru    *pStr,
    const BST_UINT8    *pucSrc,
    const BST_UINT16    ulLen )
{
    BST_ERR_ENUM_UINT8      enErrMsg;
    
    if ( BST_NULL_PTR == pStr )
    {
        return BST_ERR_INVALID_PTR;
    }
    if ( BST_NULL_PTR == pucSrc )
    {
        return BST_ERR_INVALID_PTR;
    }
    if ( 0 == ulLen )
    {
        return BST_ERR_PAR_LEN;
    }
    enErrMsg                = BST_NO_ERROR_MSG;

    if ( pStr->usSize < ulLen )
    {
        enErrMsg            = BST_LIB_StrReSize( pStr, ulLen );

        if ( BST_NO_ERROR_MSG != enErrMsg )
        {
            return enErrMsg;
        }
    }
    BST_OS_MEMCPY( pStr->pData, pucSrc, ulLen );
    pStr->usUsed            = ulLen;
    pStr->pData[ulLen]      = 0;
    return enErrMsg;
}


BST_ERR_ENUM_UINT8  BST_LIB_StrReSize(
    BST_LIB_StrStru        *pStr,
    BST_UINT16              usLen )
{
    BST_ASSERT_NULL_RTN( pStr, BST_ERR_INVALID_PTR );

    if ( usLen == pStr->usSize )
    {
        pStr->usUsed        = 0;
        if ( BST_NULL_PTR != pStr->pData )
        {
            pStr->pData[0]  = 0;
        }
        return BST_NO_ERROR_MSG;
    }
    if ( 0 != pStr->usSize )
    {
        BST_OS_FREE( pStr->pData );
        pStr->usUsed        = 0;
        pStr->usSize        = 0;
    }

    if( usLen == 0 )
    {
        return BST_NO_ERROR_MSG;
    }
    pStr->pData             = ( BST_CHAR * )BST_OS_MALLOC( usLen + 1 );

    BST_ASSERT_NULL_RTN( pStr->pData, BST_ERR_NO_MEMORY );
    pStr->usUsed            = 0;
    pStr->usSize            = usLen;
    pStr->pData[0]          = 0;
    return BST_NO_ERROR_MSG;
}

/*lint -e438*/
BST_LIB_StrStru* BST_LIB_StrCreat(
    BST_UINT16              usLength )
{
    BST_LIB_StrStru        *pstNewStr;
    pstNewStr               = ( BST_LIB_StrStru *)BST_OS_MALLOC
                              ( BST_OS_SIZEOF( BST_LIB_StrStru ) );
    BST_ASSERT_NULL_RTN( pstNewStr, BST_NULL_PTR );

    if ( 0 != usLength )
    {
        pstNewStr->pData    = ( BST_CHAR *)BST_OS_MALLOC( usLength + 1 );

        if ( BST_NULL_PTR == pstNewStr->pData )
        {
            BST_OS_FREE( pstNewStr );
            return BST_NULL_PTR;
        }
        pstNewStr->pData[0] = 0;
    }
    else
    {
        pstNewStr->pData    = BST_NULL_PTR;
    }
    pstNewStr->usUsed       = 0;
    pstNewStr->usSize       = usLength;
    return pstNewStr;
}
/*lint +e438*/

/*lint -e438*/
BST_VOID BST_LIB_StrDelete(
    BST_LIB_StrStru         *pStr )
{
    BST_ASSERT_NULL( pStr );
    if ( BST_NULL_PTR != pStr->pData )
    {
        BST_OS_FREE( pStr->pData );
    }
    BST_OS_FREE( pStr );
}
/*lint +e438*/
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


