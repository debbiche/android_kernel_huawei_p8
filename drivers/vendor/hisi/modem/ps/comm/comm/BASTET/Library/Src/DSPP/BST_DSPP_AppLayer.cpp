

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_DSPP_AppLayer.h"
#include "BST_DSPP_CtrLayer.h"
#include "BST_DSPP_TrsLayer.h"
#include "BST_DBG_MemLeak.h"
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_DSPP_APPLAYER_CPP
/*lint +e767*/

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BST_DSPP_HEAD_LENGTH            (  BST_DSPP_TPA_HEAD_LEN        \
                                         + BST_DSPP_TPA_FLAG_LEN        \
                                         + BST_DSPP_CP_HEAD_LEN )
#define BST_DSPP_AP_OFFSET_ADDR         (  BST_DSPP_TPA_HEAD_LEN        \
                                         + BST_DSPP_TPA_BEGIN_FLAG_LEN  \
                                         + BST_DSPP_CP_HEAD_LEN )

/*****************************************************************************
  3 函数实现
******************************************************************************/


BST_DSPP_CAppDlVa::BST_DSPP_CAppDlVa( BST_VOID )
    : BST_DSPP_CDlLayerBase( BST_DSPP_LAYER_APP, BST_DSPP_VER_A )
{
    m_pcTaskManager = BST_SRV_CTaskMng::GetInstance ();
}


BST_DSPP_CAppDlVa::~BST_DSPP_CAppDlVa( BST_VOID )
{
    m_pcTaskManager                     = BST_NULL_PTR;
}


BST_ERR_ENUM_UINT8  BST_DSPP_CAppDlVa::ParseHeadInfo(
    BST_DSPP_HEAD_STRU   *const pstHead,
    BST_UINT8 const      *pucData,
    BST_UINT16            usLength )
{
    return BST_NO_ERROR_MSG;
}


BST_UINT16 BST_DSPP_CAppDlVa::ReadOut(
    BST_DSPP_OBJTADDR_T     usObjtAddr,
    BST_DSPP_OFSTADDR_T     usOfstAddr,
    BST_UINT8              *pucSrcBuff,
    BST_UINT8              *pucDstBuff,
    BST_UINT16              usSrcLen,
    BST_UINT16              usDstLen )
{

    BST_CORE_PID_LEN_T                  usParamLen;
    BST_CORE_PID_ENUM_UINT16            usParamId;
    BST_UINT16                          usLooper;
    BST_UINT16                          usResBufCnt;

    usResBufCnt                         = 0;
    for ( usLooper = 0;
          usLooper < usSrcLen;
          usLooper += BST_CORE_PID_LEN )
    {

        /*
         * 读取PID编号
         */
        usParamId                       = BST_DSPP_AP_BYTE2PID
                                         ( pucSrcBuff[ usLooper ], pucSrcBuff[ usLooper + 1 ] );
        /*
         * 读取参数保存长度
         * |--PL(2 BYTE)--|--PID(2 BYTE)--|--Content( usParamLen，函数获得)--|
         * 因此，读入缓冲长度至少为BST_OS_SIZEOF ( BST_CORE_PID_LEN_T )+BST_CORE_PID_LEN
         */
        usParamLen                      = m_pcTaskManager->Inquire
                                         ( usObjtAddr, usOfstAddr, usParamId,
                                         ( BST_UINT16 )( usDstLen - usResBufCnt
                                                       - BST_OS_SIZEOF ( BST_CORE_PID_LEN_T )
                                                       - BST_CORE_PID_LEN ),
                                                        pucDstBuff + usResBufCnt
                                                        + BST_OS_SIZEOF ( BST_CORE_PID_LEN_T )
                                                        + BST_CORE_PID_LEN );
        BST_DBG_LOG4( "DSPP Inquire: procid=%u, taskid=%u, pid=%u, plen=%u",
                      usObjtAddr, usOfstAddr, usParamId, usParamLen );

        /*
         * 参数长度获取成功后，复制缓冲区内容
         */
        if ( 0 != usParamLen )
        {
            usParamLen                 += ( BST_OS_SIZEOF ( BST_CORE_PID_LEN_T ) + BST_CORE_PID_LEN );
            pucDstBuff[usResBufCnt]    = BST_DSPP_U16_TO_U8L ( usParamLen );
            pucDstBuff[usResBufCnt+1]  = BST_DSPP_U16_TO_U8H ( usParamLen );
            pucDstBuff[usResBufCnt+2]  = BST_DSPP_U16_TO_U8L ( usParamId );
            pucDstBuff[usResBufCnt+3]  = BST_DSPP_U16_TO_U8H ( usParamId );
            usResBufCnt               += usParamLen;
        }

        /*
         * 如果继续读取长度超限，那么将结束本次参数读取操作
         */
        if ( usResBufCnt + BST_OS_SIZEOF ( BST_CORE_PID_LEN_T ) + BST_CORE_PID_LEN > usDstLen )
        {
            break;
        }
    }
    return usResBufCnt;
}


BST_ERR_ENUM_UINT8  BST_DSPP_CAppDlVa::Inquire(
    BST_DSPP_OBJTADDR_T    usObjtAddr,
    BST_DSPP_OFSTADDR_T    usOfstAddr,
    BST_DSPP_HEAD_STRU    *const pstRxHeadInfo )
{
    BST_CORE_PID_LEN_T          usParamLen;
    BST_CORE_PID_ENUM_UINT16    usParamId;
    BST_UINT16                  usLooper;
    BST_UINT16                  usResBufCnt;
    BST_UINT8                  *pucData;
    BST_UINT8                  *pcResBuffer;
    BST_UINT16                  usPktTotLen;

    usLooper                    = 0;
    usParamLen                  = 0;
    usPktTotLen                 = 0;
    usResBufCnt                 = 0;
    usParamId                   = BST_PID_INVALID;
    pcResBuffer                 = BST_NULL_PTR;
    pucData                     = pstRxHeadInfo->pucData;

    /*
     * 入口条件检查
     */
    if ( ( BST_NULL_PTR == m_pcTaskManager )
     || ( BST_NULL_PTR == pstRxHeadInfo ) )
    {
        return BST_ERR_INVALID_PTR;
    }

    if ( !BST_DSPP_IsObjIdValid (usObjtAddr) )
    {
        BST_RLS_LOG1( "BST_DSPP_CAppDlVa::Inquire usObjtAddr=%u",
                      usObjtAddr );
        return BST_ERR_UNKNOW_OBJ;
    }

    if ( ( BST_NULL_PTR == pucData )
     || ( BST_CORE_PID_LEN > pstRxHeadInfo->usLength ) )
    {
        return BST_ERR_PAR_LEN;
    }

    if ( ( pstRxHeadInfo->usLength % BST_CORE_PID_LEN ) != 0 )
    {
        BST_RLS_LOG1( "BST_DSPP_CAppDlVa::Inquire pstRxHeadInfo->usLength=%u",
                      pstRxHeadInfo->usLength );
        return BST_ERR_PAR_LEN;
    }

    /*
     * 首先获取预查询参数的总长度
     */
    for ( usLooper = 0;
          usLooper < pstRxHeadInfo->usLength;
          usLooper+= BST_CORE_PID_LEN )
    {
        usParamId               = BST_DSPP_AP_BYTE2PID
                                ( pucData[usLooper], pucData[usLooper + 1] );
        usParamLen              = m_pcTaskManager->GetLen
                                ( usObjtAddr, usOfstAddr, usParamId );

        if ( 0 != usParamLen )
        {
            usPktTotLen        += ( BST_OS_SIZEOF ( BST_CORE_PID_LEN_T )
                                  + BST_CORE_PID_LEN
                                  + usParamLen );
        }
    }
    if ( 0 == usPktTotLen )
    {
        return BST_ERR_PAR_UNKNOW;
    }
    /*
     * 申请目标缓冲资源
     */
    pcResBuffer                 = ( BST_UINT8 * )BST_OS_MALLOC ( usPktTotLen );
    if ( BST_NULL_PTR == pcResBuffer )
    {
        return BST_ERR_NO_MEMORY;
    }

    /*
     * 从PID数据库或者应用程序获得参数内容
     */
    usResBufCnt                 = ReadOut( usObjtAddr, usOfstAddr, pucData, pcResBuffer,
                                           pstRxHeadInfo->usLength, usPktTotLen );
    /*
     * Memory Leak! If we malloc the space but copy none, We shoud release it here;
     */
    if ( 0 == usResBufCnt )
    {
        BST_OS_FREE ( pcResBuffer );
        pcResBuffer             = BST_NULL_PTR;
    }
    /*
     * 从新配置缓冲区内容，供应用使用
     */
    pstRxHeadInfo->pucData      = pcResBuffer;
    pstRxHeadInfo->usLength     = usResBufCnt;
    return BST_NO_ERROR_MSG;
}


BST_ERR_ENUM_UINT8  BST_DSPP_CAppDlVa::Config(
    BST_DSPP_OBJTADDR_T usObjtAddr,
    BST_DSPP_OFSTADDR_T usOfstAddr,
    BST_DSPP_HEAD_STRU *const pstRxHeadInfo )
{
    BST_CORE_PID_LEN_T          usParamLen;
    BST_CORE_PID_ENUM_UINT16    usParamId;
    BST_UINT8                  *pcData;
    BST_UINT16                  usBufCnt;
    BST_ERR_ENUM_UINT8          ucRtnVal;

    if ( BST_NULL_PTR == pstRxHeadInfo )
    {
        return BST_ERR_INVALID_PTR;
    }
    usParamLen                  = 0;
    usBufCnt                    = 0;
    usParamId                   = BST_PID_INVALID;
    ucRtnVal                    = BST_NO_ERROR_MSG;
    pcData                      = pstRxHeadInfo->pucData;

    /*
     * 入口检查
     */
    if ( BST_NULL_PTR == m_pcTaskManager )
    {
        return ( BST_ERR_INVALID_PTR );
    }

    if ( !BST_DSPP_IsObjIdValid (usObjtAddr) )
    {
        BST_RLS_LOG1( "BST_DSPP_CAppDlVa::Config usObjtAddr=%u",
                      usObjtAddr );
        return ( BST_ERR_UNKNOW_OBJ );
    }

    if( BST_CORE_PID_LEN > pstRxHeadInfo->usLength )
    {
        return ( BST_ERR_PAR_LEN );
    }

    for (;;)
    {
        /*
         * 如果剩余数据长度无法获取有效PID信息，那么直接退出或返回
         */
        if ( pstRxHeadInfo->usLength - usBufCnt <= BST_DSPP_AP_CMD_HEAD_LEN )
        {
            if ( pstRxHeadInfo->usLength == usBufCnt )
            {
                ucRtnVal        = BST_NO_ERROR_MSG;
            }
            else
            {
                ucRtnVal        = BST_ERR_PAR_LEN;
            }
            break;
        }
        /*
         * 读取PID长度，PID名称等信息
         */
        usParamLen              = ( BST_CORE_PID_LEN_T )BST_DSPP_U8_TO_U16
                                  ( pcData[BST_DSPP_AP_CMD_LEN_L],
                                    pcData[BST_DSPP_AP_CMD_LEN_H] )
                                   -BST_DSPP_AP_CMD_HEAD_LEN;
        usParamId               = BST_DSPP_AP_BYTE2PID
                                ( pcData[BST_DSPP_AP_CMD_ID_L],
                                  pcData[BST_DSPP_AP_CMD_ID_H] );
        pcData                 += BST_DSPP_AP_CMD_HEAD_LEN;
        usBufCnt               += BST_DSPP_AP_CMD_HEAD_LEN;

        /*
         * 如果剩余内容长度与解析出的结果不符合，则读取识别，无法配置内容
         */
        if ( ( pstRxHeadInfo->usLength - usBufCnt ) < usParamLen )
        {
            BST_RLS_LOG3( "BST_DSPP_CAppDlVa::Config usLength=%u,usBufCnt=%u,usParamLen=%u",
                          pstRxHeadInfo->usLength, usBufCnt, usParamLen );
            ucRtnVal            = BST_ERR_PAR_LEN;
            break;
        }
        /*
         * 写入参数值
         */
        ucRtnVal                = m_pcTaskManager->Config
                                 ( usObjtAddr, usOfstAddr, usParamId,
                                   usParamLen, pcData );
        if ( BST_NO_ERROR_MSG == ucRtnVal )
        {
            pcData             += usParamLen;
            usBufCnt           += usParamLen;
        }
        else
        {
            break;
        }
    }
    return ( ucRtnVal );
}


BST_ERR_ENUM_UINT8  BST_DSPP_CAppDlVa::Add(
    BST_DSPP_HEAD_STRU     *const pstRxHeadInfo,
    BST_DSPP_OFSTADDR_T    *const pusOfstAddr,
    BST_DSPP_OBJTADDR_T     usObjtAddr )
{
    BST_UINT16              usOfstAddr;
    BST_ERR_ENUM_UINT8      ucRtnVal;

    /*
     * 入口检查
     */
    if ( ( BST_NULL_PTR == m_pcTaskManager )
     || ( BST_NULL_PTR == pstRxHeadInfo )
     || ( BST_NULL_PTR == pusOfstAddr ) )
    {
        return ( BST_ERR_INVALID_PTR );
    }
   *pusOfstAddr             = BST_DSPP_INVALID_ADDR;
    /*
     * 校验PID是否有效
     */
    if ( !BST_DSPP_IsObjIdValid (usObjtAddr) )
    {
        return ( BST_ERR_UNKNOW_OBJ );
    }

    /*
     * 从任务管理器获取新任务的ID编号
     */
    usOfstAddr              = m_pcTaskManager->Add ( usObjtAddr );
    BST_DBG_LOG2( "DSPP Add: procid=%u, taskid=%u", usObjtAddr, usOfstAddr );
    if ( BST_DSPP_INVALID_ADDR == usOfstAddr )
    {
        return ( BST_ERR_NO_MEMORY );
    }
    /*
     * 保持参数值到入口
     */
   *pusOfstAddr             = usOfstAddr;
    pstRxHeadInfo->enApVer  = BST_DSPP_VER_VOID;
    switch ( pstRxHeadInfo->enCpVer )
    {
        case BST_DSPP_VER_A:
            pstRxHeadInfo->stCpVerA.usOfstAddr = usOfstAddr;
            break;

        case BST_DSPP_VER_B:
            pstRxHeadInfo->stCpVerA.usOfstAddr = usOfstAddr;
            break;

        default:
            break;
    }
    /*
     * 在添加完成后，如果有数据，那么先假设该参数需要配置
     */
    if ( ( 0 != pstRxHeadInfo->usLength )
      && ( BST_NULL_PTR != pstRxHeadInfo->pucData ) )
    {
        ucRtnVal            = Config ( usObjtAddr, usOfstAddr, pstRxHeadInfo );
    }
    else
    {
        ucRtnVal            = BST_NO_ERROR_MSG;
    }
    return ( ucRtnVal );
}

BST_ERR_ENUM_UINT8  BST_DSPP_CAppDlVa::Remove(
    BST_DSPP_OBJTADDR_T usObjtAddr,
    BST_DSPP_OFSTADDR_T usOfstAddr )
{
    if ( BST_NULL_PTR == m_pcTaskManager )
    {
        return BST_ERR_INVALID_PTR;
    }
    if ( BST_DSPP_INVALID_ADDR == usOfstAddr )
    {
        return BST_ERR_UNKNOW_OBJ;
    }
    BST_DBG_LOG2( "DSPP Remove: procid=%u, taskid=%u", usObjtAddr, usOfstAddr );
    return ( m_pcTaskManager->Remove ( usObjtAddr, usOfstAddr ) );
}


BST_ERR_ENUM_UINT8  BST_DSPP_CAppDlVa::ProcAction(
    BST_DSPP_HEAD_STRU     *const pstRxHeadInfo,
    BST_DSPP_OFSTADDR_T    *pusOfstAddr,
    BST_DSPP_OBJTADDR_T     usObjtAddr )
{
    BST_ERR_ENUM_UINT8      enRtnVal;

    enRtnVal                = BST_ERR_UNKNOW_ACT;
    switch ( pstRxHeadInfo->stCpVerA.ucActType )
    {
        /*
         * 添加新任务
         */
        case BST_ACTION_ADD:
            enRtnVal        = Add ( pstRxHeadInfo, pusOfstAddr, usObjtAddr );
            break;
        /*
         * 删除任务
         */
        case BST_ACTION_REMOVE:
            enRtnVal        = Remove ( usObjtAddr, *pusOfstAddr ) ;
            break;
        /*
         * 开始任务，如果有参数需要先配置，而后启动任务运行
         */
        case BST_ACTION_START:
            if( ( 0 != pstRxHeadInfo->usLength )
             && ( BST_NULL_PTR != pstRxHeadInfo->pucData ) )
            {
                /*
                 * 如果配置失败，则Start动作可能引起系统异常，因此不可启动
                 */
                if ( Config( usObjtAddr, *pusOfstAddr, pstRxHeadInfo )
                  != BST_NO_ERROR_MSG )
                {
                    break;
                }
            }
            enRtnVal        = m_pcTaskManager->Start ( usObjtAddr, *pusOfstAddr );
            break;
        /*
         * 停止任务运行
         */
        case BST_ACTION_STOP:
            enRtnVal        = m_pcTaskManager->Stop ( usObjtAddr, *pusOfstAddr );
            break;

        /*
         * 查询参数
         */
        case BST_ACTION_INQUIRE:
            if ( BST_NULL_PTR == pstRxHeadInfo->pucData )
            {
                enRtnVal    = BST_ERR_LAYER_VER;
                break;
            }
            enRtnVal        = Inquire ( usObjtAddr, *pusOfstAddr, pstRxHeadInfo );
            break;
        /*
         * 设置参数
         */
        case BST_ACTION_CONFIG:
            if ( BST_NULL_PTR == pstRxHeadInfo->pucData )
            {
                enRtnVal    = BST_ERR_LAYER_VER;
                break;
            }
            enRtnVal        = Config ( usObjtAddr, *pusOfstAddr, pstRxHeadInfo ) ;
            break;

        default:
            enRtnVal        = BST_ERR_PAR_UNKNOW;
            BST_DBG_LOG ( "BST_DSPP_CAppDlVa::ProcAction Error, Unknow Action" );
            break;
    }
    return enRtnVal;
}

/*lint -e438*/
BST_ERR_ENUM_UINT8  BST_DSPP_CAppDlVa::UnpackInternal(
    BST_DSPP_HEAD_STRU         *const pstRxHeadInfo,
    BST_DSPP_VERION_ENUM_UINT8 *const pstNextVer )
{
    BST_UINT8              *pucBackRx;
    BST_DSPP_CAppUlVa      *pcUlProc;
    BST_ERR_ENUM_UINT8      enRtnVal;

    pcUlProc                = BST_DSPP_CAppUlVa::GetInstance ();
    enRtnVal                = BST_ERR_UNKNOW_ACT;

    /*
     * 入口检查
     */
    BST_ASSERT_NULL_RTN ( pstRxHeadInfo, BST_ERR_INVALID_PTR);
    BST_ASSERT_NULL_RTN ( pcUlProc, BST_ERR_INVALID_PTR);
    if ( BST_NULL_PTR == m_pcTaskManager )
    {
        return ( pcUlProc->Response ( pstRxHeadInfo, BST_ERR_INVALID_PTR ) );
    }
    if ( BST_DSPP_VER_A != pstRxHeadInfo->enCpVer )
    {
        return ( pcUlProc->Response ( pstRxHeadInfo, BST_ERR_LAYER_VER ) );
    }
    /*
     * 如果运行在0拷贝模式，那么不可FREE，数据来自ACOMM，因此指针赋值为空
     */
    if ( BST_FALSE == pstRxHeadInfo->ulZeroCopy )
    {
        pucBackRx           = pstRxHeadInfo->pucData;
    }
    else
    {
        pucBackRx           = BST_NULL_PTR;
    }
    /*
     * 根据不同ACITON类型，处理具体事务
     */
    enRtnVal                = ProcAction( pstRxHeadInfo,
                             &pstRxHeadInfo->stCpVerA.usOfstAddr,
                              pstRxHeadInfo->stCpVerA.usObjtAddr );

    /*
     * 在INQUIRE-ACTION下，因为有回应值，所以PID内容不空，不可以FREE，只有出错可以
     */
    if ( BST_ACTION_INQUIRE == pstRxHeadInfo->stCpVerA.ucActType )
    {
        if ( BST_NO_ERROR_MSG != enRtnVal )
        {
            pstRxHeadInfo->usLength = 0;
        }
    }
    else
    {
        pstRxHeadInfo->usLength     = 0;
    }
    /*
     * 如果之前有备份输入指针，则FREE掉空间，避免内存泄漏
     */
    if ( BST_NULL_PTR != pucBackRx )
    {
        BST_OS_FREE( pucBackRx );
    }
    /*
     * 应答S端ACTION结果
     */
    enRtnVal                        = pcUlProc->Response
                                    ( pstRxHeadInfo, enRtnVal );
    return enRtnVal;
}
/*lint +e438*/

BST_DSPP_CDlLayerBase *BST_DSPP_CAppDlVa::GetNextStrategy(
    BST_DSPP_VERION_ENUM_UINT8 enNextVer )
{
    return BST_NULL_PTR;
}


BST_DSPP_CAppDlVa *BST_DSPP_CAppDlVa::GetInstance( BST_VOID )
{
    static BST_DSPP_CAppDlVa   *s_pInstance = BST_NULL_PTR;

    if ( BST_NULL_PTR == s_pInstance )
    {
        s_pInstance             = new BST_DSPP_CAppDlVa ();
    }
    return s_pInstance;
}


BST_ERR_ENUM_UINT8  BST_DSPP_CAppUlVa::Response(
    BST_DSPP_HEAD_STRU   *const pstOrigHeadInfo,
    BST_ERR_ENUM_UINT8    enErrMsg )
{
    BST_UINT8          *pucSdu;
    pucSdu             = BST_NULL_PTR;

    /*
     * 入库检查
     */
    if ( BST_NULL_PTR == pstOrigHeadInfo )
    {
        return BST_ERR_UNREACHABLE;
    }

    /*
     * 校验协议版本类型是否合法
     */
    if ( !BST_DSPP_IsVersionValid ( pstOrigHeadInfo->enCpVer ) )
    {
        return BST_ERR_UNREACHABLE;
    }

    /*
     * 填写DSPP结构体内容，类型为RESPONSE
     */
    pstOrigHeadInfo->enTpVer        = BST_DSPP_VER_VOID;
    pstOrigHeadInfo->enPrmvType     = BST_DSPP_PRMTV_RESPONSE;

    /*
     * 如果源应答缓冲区有数据，那么申请空间也涵盖
     */
    if ( ( BST_NULL_PTR != pstOrigHeadInfo->pucData )
      && ( 0 != pstOrigHeadInfo->usLength ) )
    {
        pstOrigHeadInfo->enApVer= BST_DSPP_VER_A;
        pucSdu                  = ( BST_UINT8 * )BST_OS_MALLOC
                                  ( pstOrigHeadInfo->usLength+BST_DSPP_HEAD_LENGTH );
        if ( BST_NULL_PTR == pucSdu )
        {
            BST_OS_FREE( pstOrigHeadInfo->pucData );
            return BST_ERR_NO_MEMORY;
        }
        BST_OS_MEMCPY( &pucSdu[BST_DSPP_AP_OFFSET_ADDR],
                        pstOrigHeadInfo->pucData,
                        pstOrigHeadInfo->usLength );
        BST_OS_FREE( pstOrigHeadInfo->pucData );
        pstOrigHeadInfo->pucData    = pucSdu;
    }
    /*
     * 如果应对为空白，直接申请DSPP头长度即可
     */
    else
    {
        pstOrigHeadInfo->enApVer    = BST_DSPP_VER_VOID;
        pstOrigHeadInfo->usLength   = 0;
        pstOrigHeadInfo->pucData    = ( BST_UINT8 * )
                                      BST_OS_MALLOC ( BST_DSPP_HEAD_LENGTH );
        if ( BST_NULL_PTR == pstOrigHeadInfo->pucData )
        {
            return BST_ERR_NO_MEMORY;
        }
    }

    switch ( pstOrigHeadInfo->enCpVer )
    {
        case BST_DSPP_VER_A:
            pstOrigHeadInfo->stCpVerA.ucActType
                                    = enErrMsg;
            break;

        case BST_DSPP_VER_B:
            pstOrigHeadInfo->stCpVerA.ucActType
                                    = enErrMsg;
            break;

        default:
            break;
    }
    /*
     * 传入下一层进行封包
     */
    return ( Pack ( pstOrigHeadInfo ) );
}


BST_ERR_ENUM_UINT8  BST_DSPP_CAppUlVa::Report(
    BST_DSPP_OBJTADDR_T      usObjtAddr,
    BST_DSPP_OFSTADDR_T      usOfstAddr,
    BST_DSPP_CMDLEN_T        usCmdLen,
    BST_CORE_PID_ENUM_UINT16 enCmdId,
    BST_UINT8               *pucCmdData )
{
    BST_DSPP_HEAD_STRU                  stTxHeadInfo;
    BST_UINT8                          *pucSdu;

    pucSdu                             = BST_NULL_PTR;
    /*
     * 入口检查
     */
    if ( ( 0 == usCmdLen )
      || ( BST_NULL_PTR == pucCmdData ) )
    {
        return BST_ERR_INVALID_PTR;
    }
    /*
     * 校验类型、地址值是否有效
     */
    if ( (!BST_DSPP_IsObjIdValid ( usObjtAddr ) )
      || ( BST_DSPP_INVALID_ADDR == usOfstAddr ) )
    {
        return BST_ERR_UNREACHABLE;
    }

    /*
     * 获取上报缓冲
     */
    pucSdu                              = ( BST_UINT8 * )BST_OS_MALLOC ( usCmdLen
                                        + BST_DSPP_AP_CMD_HEAD_LEN + BST_DSPP_HEAD_LENGTH );
    if ( BST_NULL_PTR == pucSdu )
    {
        return BST_ERR_NO_MEMORY;
    }
    /*
     * 填写SDU-AP层信息
     */
    stTxHeadInfo.pucData                = pucSdu;
    pucSdu                             += BST_DSPP_AP_OFFSET_ADDR;
    pucSdu[BST_DSPP_AP_CMD_LEN_L]       = BST_DSPP_U16_TO_U8L
                                        ( usCmdLen + BST_DSPP_AP_CMD_HEAD_LEN );
    pucSdu[BST_DSPP_AP_CMD_LEN_H]       = BST_DSPP_U16_TO_U8H
                                        ( usCmdLen + BST_DSPP_AP_CMD_HEAD_LEN );
    pucSdu[BST_DSPP_AP_CMD_ID_L]        = BST_DSPP_U16_TO_U8L ( enCmdId );
    pucSdu[BST_DSPP_AP_CMD_ID_H]        = BST_DSPP_U16_TO_U8H ( enCmdId );
    BST_OS_MEMCPY ( pucSdu + BST_DSPP_AP_CMD_HEAD_LEN, pucCmdData, usCmdLen );

    /*
     * 填写DSPP结构体，选择协议版本，供下层使用
     */
    stTxHeadInfo.enCpVer                = BST_DSPP_VER_A;
    stTxHeadInfo.enApVer                = BST_DSPP_VER_A;
    stTxHeadInfo.enTpVer                = BST_DSPP_VER_VOID;
    stTxHeadInfo.enPrmvType             = BST_DSPP_PRMTV_REPORT;
    stTxHeadInfo.usPktNum               = BST_DSPP_TP_INVALID_PKTNUM;
    stTxHeadInfo.usLength               = ( usCmdLen + BST_DSPP_AP_CMD_HEAD_LEN );
    stTxHeadInfo.stCpVerA.usObjtAddr    = usObjtAddr;
    stTxHeadInfo.stCpVerA.usOfstAddr    = usOfstAddr;
    stTxHeadInfo.stCpVerA.ucActType     = BST_ACTION_REPORT;

    return ( Pack ( &stTxHeadInfo ) );
}

BST_ERR_ENUM_UINT8  BST_DSPP_CAppUlVa::Report(
    BST_DSPP_OBJTADDR_T usObjtAddr,
    BST_DSPP_OFSTADDR_T usOfstAddr,
    BST_ERR_ENUM_UINT8  enErr )
{
    BST_DSPP_HEAD_STRU                  stTxHeadInfo;

    /*
     * 入口检查
     */
    if ( (!BST_DSPP_IsObjIdValid ( usObjtAddr ) )
      || ( BST_DSPP_INVALID_ADDR == usOfstAddr ) )
    {
        return BST_ERR_UNREACHABLE;
    }
    stTxHeadInfo.pucData                = ( BST_UINT8 * )
                                          BST_OS_MALLOC ( BST_DSPP_HEAD_LENGTH );
    if ( BST_NULL_PTR == stTxHeadInfo.pucData )
    {
        return BST_ERR_NO_MEMORY;
    }
    /*
     * 填写DSPP结构体，选择协议版本，供下层使用
     */
    stTxHeadInfo.enCpVer                = BST_DSPP_VER_A;
    stTxHeadInfo.enApVer                = BST_DSPP_VER_A;
    stTxHeadInfo.enTpVer                = BST_DSPP_VER_VOID;
    stTxHeadInfo.enPrmvType             = BST_DSPP_PRMTV_REPORT;
    stTxHeadInfo.usPktNum               = BST_DSPP_TP_INVALID_PKTNUM;
    stTxHeadInfo.usLength               = 0;
    stTxHeadInfo.stCpVerA.usObjtAddr    = usObjtAddr;
    stTxHeadInfo.stCpVerA.usOfstAddr    = usOfstAddr;
    stTxHeadInfo.stCpVerA.ucActType     = enErr;
    return ( Pack ( &stTxHeadInfo ) );
}


BST_DSPP_CAppUlVa *BST_DSPP_CAppUlVa::GetInstance ( BST_VOID )
{
    static BST_DSPP_CAppUlVa           *s_pInstance = BST_NULL_PTR;

    if ( BST_NULL_PTR == s_pInstance )
    {
        s_pInstance                     = new BST_DSPP_CAppUlVa ();
    }

    return s_pInstance;
}


BST_DSPP_CAppUlVa::BST_DSPP_CAppUlVa ( BST_VOID )
    : BST_DSPP_CUlLayerBase ( BST_DSPP_LAYER_APP, BST_DSPP_VER_A )
{

}

BST_DSPP_CAppUlVa::~BST_DSPP_CAppUlVa ( BST_VOID )
{

}

BST_ERR_ENUM_UINT8  BST_DSPP_CAppUlVa::PackInternal(
    BST_DSPP_HEAD_STRU         *const pstTxHeadInfo,
    BST_DSPP_VERION_ENUM_UINT8 *const penNextVer )
{
    if ( BST_NULL_PTR == pstTxHeadInfo )
    {
        return BST_ERR_UNKNOW_OBJ;
    }
   *penNextVer                          = pstTxHeadInfo->enCpVer;
    return BST_NO_ERROR_MSG;
}


BST_DSPP_CUlLayerBase *BST_DSPP_CAppUlVa::GetNextStrategy(
    BST_DSPP_VERION_ENUM_UINT8 enNextVer )
{
    switch ( enNextVer )
    {
        case BST_DSPP_VER_A:
            return ( new BST_DSPP_CCtrUlVa () );

        default:
            break;
    }
    return BST_NULL_PTR;
}

