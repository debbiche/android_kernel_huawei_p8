

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BST_APP_EmailExchange.h"
#include "BST_APP_EmailBox.h"
#include "BST_DBG_MemLeak.h"
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_APP_EMAILEXCHANGE_CPP
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

BST_APP_CEmailExchange::BST_APP_CEmailExchange(
    const EMAIL_ACCOUNT_STRU *pstInAccountInfo )
    : BST_APP_CEmailPacketProc( pstInAccountInfo )
{
    BST_UINT16              i;
    m_pExcgHttp             = new BST_APP_CExcgHttp();
    m_pDeviceID             = BST_LIB_StrCreat( BST_LIB_SHORT_STR_LEN );
    m_pExcgHttpInfo         = BST_LIB_StrCreat( BST_LIB_SHORT_STR_LEN );
    m_pExcgCollectionId     = BST_LIB_StrCreat( BST_LIB_SHORT_STR_LEN );
    m_pExcgSyncKey          = BST_LIB_StrCreat( BST_LIB_SHORT_STR_LEN );
    m_bCfgChgFlag           = BST_TRUE;
    for ( i = 0; i < BST_APP_SOCKET_NUM_MAX; i++ )
    {
        m_SocketArray[i]    = BST_NULL_PTR;
    }
    m_usSocketCnt           = 0;
}

BST_APP_CEmailExchange::~BST_APP_CEmailExchange( BST_VOID )
{
    BST_UINT16                  i;
#if ( BST_OS_VER != BST_QCOM_ROS )
    try
    {
#endif
        if ( BST_NULL_PTR != m_pExcgHttp )
        {
            delete m_pExcgHttp;
        }
        BST_LIB_StrDelete( m_pDeviceID );
        BST_LIB_StrDelete( m_pExcgHttpInfo );
        BST_LIB_StrDelete( m_pExcgCollectionId );
        BST_LIB_StrDelete( m_pExcgSyncKey );
        for ( i = 1; i < BST_APP_SOCKET_NUM_MAX; i++ )
        {
            if ( BST_NULL_PTR != m_SocketArray[i] )
            {
                delete m_SocketArray[i];
                m_SocketArray[i]    = BST_NULL_PTR;
            }
        }
#if ( BST_OS_VER != BST_QCOM_ROS )
    }
    catch (...)
    {
        BST_RLS_LOG("BST_APP_CEmailExchange destruction exception!");
    }
#endif
    BST_RLS_LOG("BST_APP_CEmailExchange destructor.");
}

BST_BOOL BST_APP_CEmailExchange::InitEmailProc( BST_CORE_CNPTask *pTaskProp )
{
    BST_OS_CTimerCb     *pcTimerCb;
    BST_IP_CNetRcver    *pNetReceiver;

    BST_IP_ERR_T         enIpErr;
    BST_UINT16           i;

    if ( BST_NULL_PTR == pTaskProp )
    {
        return BST_FALSE;
    }

    if ( BST_FALSE == InitRegedit( pTaskProp ) )
    {
        return BST_FALSE;
    }
    m_pcHostTask        = pTaskProp;
    m_pSyncSocket       = pTaskProp->m_pSocket;
    /*
     *  创建定时器，注册接收回调函数
     */
    pNetReceiver        = this;

    if ( BST_NULL_PTR == m_pSyncSocket )
    {
        return BST_FALSE;
    }

    enIpErr             = m_pSyncSocket->IoCtrl( BST_SCKT_CMD_REG_CALLBACK,
                                                 pNetReceiver );
    if ( BST_IP_ERR_OK != enIpErr )
    {
        return BST_FALSE;
    }

    m_SocketArray[m_usSocketCnt]        = m_pSyncSocket;


    for( i = 1; i < BST_APP_SOCKET_NUM_MAX; i++ )
    {
#if( BST_VER_TYPE == BST_DBG_VER )
        m_SocketArray[i]    = new BST_IP_CSocket( m_pcHostTask->m_usProcId,
                                                  m_pcHostTask->m_usTaskId );
#else
        m_SocketArray[i]    = new BST_IP_CSocket();
#endif

        enIpErr         = m_SocketArray[i]->IoCtrl( BST_SCKT_CMD_REG_CALLBACK,
                                                    pNetReceiver );
        if ( BST_IP_ERR_OK != enIpErr )
        {
            return BST_FALSE;
        }
    }

    pcTimerCb           = this;
    m_ulTimerId         = BST_OS_TimerCreateCpp( pcTimerCb,
                                                 BST_NULL_PTR );
    if ( !BST_OS_IsTimerValid(m_ulTimerId) )
    {
        return BST_FALSE;
    }

    return BST_TRUE;
}

BST_BOOL BST_APP_CEmailExchange::InitRegedit( BST_CORE_CNPTask *pTaskProp )
{
    BST_CORE_CRegedit  *pcRegedit;
    BST_ERR_ENUM_UINT8  enRtnVal;

    pcRegedit           = BST_CORE_CRegedit::GetInstance();

    if ( BST_NULL_PTR == pcRegedit )
    {
        return BST_FALSE;
    }

    if ( BST_NULL_PTR == m_pDeviceID )
    {
        return BST_FALSE;
    }

    if ( BST_NULL_PTR == m_pExcgHttpInfo )
    {
        return BST_FALSE;
    }

    if ( BST_NULL_PTR == m_pExcgCollectionId )
    {
        return BST_FALSE;
    }

    if ( BST_NULL_PTR== m_pExcgSyncKey )
    {
        return BST_FALSE;
    }

    enRtnVal    = pcRegedit->Regist( pTaskProp,
                                     pTaskProp,
                                     BST_PID_EMAIL_DEVICE_ID,
                                     m_pDeviceID->usUsed,
                                     m_pDeviceID->pData );
    if ( BST_NO_ERROR_MSG != enRtnVal )
    {
        return BST_FALSE;
    }
    enRtnVal    = pcRegedit->Regist( pTaskProp,
                                     pTaskProp,
                                     BST_PID_EMAIL_EXCG_HTTP_INFO,
                                     m_pExcgHttpInfo->usUsed,
                                     m_pExcgHttpInfo->pData );
    if ( BST_NO_ERROR_MSG != enRtnVal )
    {
        return BST_FALSE;
    }

    enRtnVal    = pcRegedit->Regist( pTaskProp,
                                     pTaskProp,
                                     BST_PID_EMAIL_EXCG_WXML_INFO,
                                     m_pExcgSyncKey->usUsed,
                                     m_pExcgSyncKey->pData );
    if ( BST_NO_ERROR_MSG != enRtnVal )
    {
        return BST_FALSE;
    }

    return BST_TRUE;
}

BST_VOID BST_APP_CEmailExchange::SetOtherSocketType(
    const BST_VOID *const pData,
    BST_UINT16      usLen )
{
    BST_PRTC_TYPE_ENUM_UINT32   enPrtclType;
    BST_SCKT_TYPE_ENUM_UINT32   enSocketType;
    BST_IP_ERR_T                lRtnVal;
    BST_UINT16                  i;

    BST_OS_MEMCPY( &enPrtclType, pData, usLen );
    switch ( enPrtclType )
    {
    case BST_PRTC_TYPE_TCP_SHORT:
        enSocketType            = BST_SCKT_TYPE_RAW_TCP;
        break;
#ifdef BST_SSL_SUPPORT
    case BST_PRTC_TYPE_SSL:
        enSocketType            = BST_SCKT_TYPE_SSL;
        break;
#endif
    default:
        BST_RLS_LOG("SetProtclType Error ProtocolType!");
        return;
    }
    for ( i = 1; i < BST_APP_SOCKET_NUM_MAX; i++)
    {
        lRtnVal                 = m_SocketArray[i]->IoCtrl
                                ( BST_SCKT_CMD_DEF_TYPE, &enSocketType);
        BST_ASSERT_NORM( lRtnVal != BST_IP_ERR_OK );
    }
}

BST_UINT8 *BST_APP_CEmailExchange::ConfigOtherInfo(
    BST_CORE_PID_ENUM_UINT16 enParamId,
    BST_UINT16               usLen,
    const BST_VOID          *const pData )
{
    BST_UINT8              *rtn_ptr;
    BST_BOOL                bRtnVal;
    rtn_ptr                 = BST_NULL_PTR;
    if ( BST_NULL_PTR == pData )
    {
        BST_RLS_LOG("CEmailExchange::ConfigOtherInfo pData=NULL.");
        return BST_NULL_PTR;
    }
    m_bCfgChgFlag           = BST_TRUE;

    switch ( enParamId )
    {
        case BST_PID_PROTOCOL_TYPE:
            SetOtherSocketType( (BST_UINT8 *)pData, usLen );
            break;
        case BST_PID_SOCKET_INFO:
            bRtnVal         = InitOtherSocket();
            BST_ASSERT_NORM_RTN( ( bRtnVal != BST_TRUE ), BST_NULL_PTR );
            break;
        case BST_PID_EMAIL_DEVICE_ID:
            rtn_ptr         = SetDeviceID( (BST_UINT8 *)pData, usLen );
            break;

        case BST_PID_EMAIL_EXCG_HTTP_INFO:
            rtn_ptr         = SetExcgInfo( (BST_UINT8 *)pData, usLen );
            break;

        case BST_PID_EMAIL_EXCG_WXML_INFO:
            rtn_ptr         = SetExcgWbxmlInfo( (BST_UINT8 *)pData, usLen );
            break;
        default:
            break;
    }

    return rtn_ptr;
}

BST_UINT8 *BST_APP_CEmailExchange::SetDeviceID(
    const BST_UINT8 *pucDeviceId,
    BST_UINT16       usLen )
{
    BST_ERR_ENUM_UINT8      enRtnVal;

    if ( BST_NULL_PTR == pucDeviceId )
    {
        return BST_NULL_PTR;
    }
    if ( 0 == usLen )
    {
        return BST_NULL_PTR;
    }
    if ( BST_NULL_PTR == m_pDeviceID )
    {
        BST_RLS_LOG( "BST_APP_CEmailExchange::SetDeviceID m_pDeviceID=NULL" );
        return BST_NULL_PTR;
    }

    enRtnVal        = BST_LIB_StrFill( m_pDeviceID, pucDeviceId, usLen );
    BST_ASSERT_NORM_RTN( (BST_NO_ERROR_MSG != enRtnVal), BST_NULL_PTR );
    return ( BST_UINT8 *)(m_pDeviceID->pData);
}


BST_UINT8 *BST_APP_CEmailExchange::SetExcgInfo(
    const BST_UINT8 *pucExcgInfo,
    BST_UINT16       usLen )
{
    BST_ERR_ENUM_UINT8  enRtnVal;

    if ( BST_NULL_PTR == pucExcgInfo )
    {
        return BST_NULL_PTR;
    }
    if ( 0 == usLen )
    {
        return BST_NULL_PTR;
    }
    if ( BST_NULL_PTR == m_pExcgHttpInfo)
    {
        BST_RLS_LOG( "BST_APP_CEmailExchange::SetExcgInfo m_pExcgHttpInfo=NULL" );
        return BST_NULL_PTR;
    }

    enRtnVal            = BST_LIB_StrFill ( m_pExcgHttpInfo, pucExcgInfo, usLen );
    BST_ASSERT_NORM_RTN( (BST_NO_ERROR_MSG != enRtnVal), BST_NULL_PTR );
    return ( BST_UINT8 *)(m_pExcgHttpInfo->pData);
}

BST_UINT8 *BST_APP_CEmailExchange::SetExcgWbxmlInfo(
    const BST_UINT8 *pucExcgWbxmlInfo, 
    BST_UINT16       usInfoLen )
{
    BST_UINT8          *pucRtnVal;
    BST_UINT8          *pucLast;
    BST_UINT16          usLen;
    if ( BST_NULL_PTR == pucExcgWbxmlInfo )
    {
        return BST_NULL_PTR;
    }
    if ( 0 == usInfoLen )
    {
        return BST_NULL_PTR;
    }

    pucLast             = (BST_UINT8 *)pucExcgWbxmlInfo;
    usLen               = UINT8_TO_UINT16( pucLast[BST_EMAIL_L],
                                           pucLast[BST_EMAIL_H] );
    pucLast            += BST_EMAIL_MULINFO_LEN_FLAG;
    pucRtnVal           = SetExcgCollectionId( pucLast, usLen );
    if ( BST_NULL_PTR == pucRtnVal )
    {
        return BST_NULL_PTR;
    }
    pucLast            += usLen;
    usLen               = UINT8_TO_UINT16( pucLast[BST_EMAIL_L],
                                           pucLast[BST_EMAIL_H] );
    pucLast            += BST_EMAIL_MULINFO_LEN_FLAG;
    pucRtnVal           = SetExcgSyncKey( pucLast, usLen );

    return pucRtnVal;
}

BST_UINT8 *BST_APP_CEmailExchange::SetExcgCollectionId(
    const BST_UINT8 *pucExcgCollectionId,
    BST_UINT16       usLen )
{
    BST_ERR_ENUM_UINT8          enRtnVal;
    if ( BST_NULL_PTR == pucExcgCollectionId )
    {
        return BST_NULL_PTR;
    }
    if ( 0 == usLen)
    {
        return BST_NULL_PTR;
    }
    if ( BST_NULL_PTR == m_pExcgCollectionId )
    {
        BST_RLS_LOG( "BST_APP_CEmailExchange::SetExcgCollectionId m_pExcgCollectionId=NULL" );    
        return BST_NULL_PTR;
    }

    enRtnVal                    = BST_LIB_StrFill( m_pExcgCollectionId, 
                                                   pucExcgCollectionId, 
                                                   usLen );

    BST_ASSERT_NORM_RTN( (BST_NO_ERROR_MSG != enRtnVal), BST_NULL_PTR );
    return ( BST_UINT8 *)(m_pExcgCollectionId->pData);
}
/*****************************************************************************
函 数 名  : ConfigExcgSyncKey
功能描述  : 设置exchange 协议的synckey
输入参数  : pucExcgSyncKey 同步Key
            usLen pExcgSyncKey的长度
输出参数  : 无
返 回 值  : BST_VOID
调用函数  :
被调函数    :
修改历史    :
    1.日期  :
        作者    :
        修改内容:
*****************************************************************************/
BST_UINT8 *BST_APP_CEmailExchange::SetExcgSyncKey(
    const BST_UINT8 *pucExcgSyncKey,
    BST_UINT16       usLen )
{
    BST_ERR_ENUM_UINT8  enRtnVal;

    if ( BST_NULL_PTR == pucExcgSyncKey )
    {
        return BST_NULL_PTR;
    }
    if ( 0 == usLen )
    {
        return BST_NULL_PTR;
    }
    if ( BST_NULL_PTR == m_pExcgSyncKey )
    {
        BST_RLS_LOG( "BST_APP_CEmailExchange::SetExcgSyncKey m_pExcgSyncKey=NULL" );
        return BST_NULL_PTR;
    }

    enRtnVal            = BST_LIB_StrFill( m_pExcgSyncKey, pucExcgSyncKey, usLen );

    BST_ASSERT_NORM_RTN( (BST_NO_ERROR_MSG != enRtnVal), BST_NULL_PTR );
    return ( BST_UINT8 *)(m_pExcgSyncKey->pData);
}

BST_VOID BST_APP_CEmailExchange::EntryProc( BST_VOID )
{
    if ( BST_NULL_PTR == m_pstAccountInfo )
    {
        BST_RLS_LOG("CEmailExchange::EntryProc m_pstAccountInfo = NULL.");
        m_pcHostTask->Finished();
        return;
    }
    if ( SYNC_PUSH == m_pstAccountInfo->enSyncType )
    {
        CloseConnection();
        UpdateSocket();
    }
    BST_APP_CEmailPacketProc::EntryProc();
}

BST_ERR_ENUM_UINT8 BST_APP_CEmailExchange::ParseResponse(
    BST_UINT8 *pucData,
    BST_UINT16 usLength )
{
    BST_EMAIL_SERVER_STATE_ENUM_UINT8   enRtnVal;

    BST_ASSERT_0_RTN( usLength, BST_ERR_PAR_LEN );
    BST_ASSERT_NULL_RTN( pucData, BST_ERR_INVALID_PTR );
    BST_ASSERT_NULL_RTN( m_pcHostTask, BST_ERR_NO_MEMORY );
    BST_ASSERT_NULL_RTN( m_pExcgHttp, BST_ERR_NO_MEMORY );

    if ( BST_EMAIL_UNCONNECT != m_lServerState )
    {
        m_lServerState  = BST_EMAIL_EAS_QUERYED;
        return BST_NO_ERROR_MSG;
    }
    m_lServerState      = BST_EMAIL_QUITED;
    enRtnVal            = m_pExcgHttp->ParseRspns( (BST_CHAR *)pucData, usLength );
    if ( BST_APP_SERVER_EXCRESCENT == enRtnVal )/*如果数据未接收完，直接返回*/
    {
        return BST_ERR_PAR_UNKNOW;
    }
    else if ( BST_APP_SERVER_NEW_MAIL == enRtnVal )
    {
        return BST_NEW_EMAIL;
    }
    else if ( BST_APP_SERVER_UNUSUAL == enRtnVal )
    {
        BST_RLS_LOG("BST_APP_CEmailExchange::ParseResponse exception occur then report and stop.");
        return BST_EMAIL_OTHER_EXCEPIONT;
    }
    else if ( BST_APP_SERVER_PARSE_EXCEPT == enRtnVal )
    {
        BST_RLS_LOG("BST_APP_CEmailExchange::ParseResponse donot support the type of mail.");
        return BST_EMAIL_NOT_SUPPORT;
    }
    else
    {
        BST_RLS_LOG("BST_APP_CEmailExchange::ParseResponse No new mail.");
        return BST_NO_ERROR_MSG;
    }
}
BST_ERR_ENUM_UINT8 BST_APP_CEmailExchange::FirstTrsProc( BST_VOID )
{
    BST_ERR_ENUM_UINT8  enErrMsg;

    if ( BST_TRUE == m_bCfgChgFlag )
    {
        BST_RLS_LOG("BST_APP_CEmailExchange::FirstTrsProc BuildRequest");
        if ( BST_TRUE != BuildRequest() )
        {
            BST_RLS_LOG("BST_APP_CEmailExchange::FirstTrsProc BuildRequest Failed!");
            return BST_ERR_ILLEGAL_PARAM;
        }
        m_bCfgChgFlag   = BST_FALSE;
    }
    m_lServerState      = BST_EMAIL_UNCONNECT;
    enErrMsg            = QueryServer();
    return enErrMsg;
}

BST_BOOL BST_APP_CEmailExchange::IsConfigOk( BST_VOID )
{
    if ( BST_FALSE == IsBasicConfigOk() )
    {
        return BST_FALSE;
    }
    if ( BST_NULL_PTR == m_pDeviceID )
    {
        BST_RLS_LOG("Email Box Exchange config is not OK: m_pDeviceID");
        return BST_FALSE;
    }
    if ( BST_NULL_PTR == m_pExcgHttpInfo )
    {
        BST_RLS_LOG("Email Box Exchange config is not OK: m_pExcgHttpInfo");
        return BST_FALSE;
    }
    if ( BST_NULL_PTR == m_pExcgCollectionId)
    {
        BST_RLS_LOG("Email Box Exchange config is not OK: m_pExcgCollectionId");
        return BST_FALSE;
    }
    if ( BST_NULL_PTR == m_pExcgSyncKey )
    {
        BST_RLS_LOG("Email Box Exchange config is not OK: m_pExcgSyncKey");
        return BST_FALSE;
    }
    if ( !BST_LIB_IsStringInit( m_pDeviceID ) )
    {
        BST_RLS_LOG("Email Box Exchange config is not OK: m_pDeviceID");
        return BST_FALSE;
    }
    if ( !BST_LIB_IsStringInit( m_pExcgHttpInfo ) )
    {
        BST_RLS_LOG("Email Box Exchange config is not OK: m_pExcgHttpInfo");
        return BST_FALSE;
    }
    if ( !BST_LIB_IsStringInit( m_pExcgCollectionId ) )
    {
        BST_RLS_LOG("Email Box Exchange config is not OK: m_pExcgCollectionId->IsInited");
        return BST_FALSE;
    }
    if ( !BST_LIB_IsStringInit( m_pExcgSyncKey ) )
    {
        BST_RLS_LOG("Email Box Exchange config is not OK: m_pExcgSyncKey->IsInited");
        return BST_FALSE;
    }
    return BST_TRUE;
}

BST_VOID BST_APP_CEmailExchange::FillHttpHead(
    BST_EMAIL_HTTP_REQ_HEAD_STRU *pstHttpReqHead )
{
    BST_UINT8      *pucLast;
    BST_UINT16      usLen;
    BST_UINT16      i;

    if (BST_NULL_PTR == pstHttpReqHead)
    {
        return;
    }
    if ( BST_NULL_PTR == m_pExcgHttpInfo )
    {
        return;
    }
    pucLast         = ( BST_UINT8 *)(m_pExcgHttpInfo->pData);

    for ( i = 0;
          i < BST_OS_SIZEOF(BST_EMAIL_HTTP_REQ_HEAD_STRU)/BST_OS_SIZEOF(BST_LIB_StrStru *);
          i++ )
    {
        usLen       = UINT8_TO_UINT16( pucLast[BST_EMAIL_L],
                                       pucLast[BST_EMAIL_H] );
        pucLast    += BST_EMAIL_MULINFO_LEN_FLAG;
        switch(i)
        {
            case PROTO_VER:
                BST_LIB_StrAddBytes( pstHttpReqHead->pstPrtcVer,
                                     (BST_CHAR *)pucLast, 
                                     usLen );
                break;

            case USERAGENT:
                BST_LIB_StrAddBytes( pstHttpReqHead->pstUsrAgent,
                                     (BST_CHAR *)pucLast,
                                     usLen );
                break;

            case ENCODING:
                BST_LIB_StrAddBytes( pstHttpReqHead->pstEncoding,
                                     (BST_CHAR *)pucLast,
                                     usLen );
                    break;

            case POLICYKEY:
                BST_LIB_StrAddBytes( pstHttpReqHead->pstPolicyKey,
                                    (BST_CHAR *)pucLast,
                                    usLen );
                    break;

            case HOSTNAME:
                BST_LIB_StrAddBytes( pstHttpReqHead->pstHostName,
                                    (BST_CHAR *)pucLast,
                                    usLen );
                break;

            default:
                break;
        }

        pucLast    += usLen;
    }
}

BST_UINT16 BST_APP_CEmailExchange::ComposeHttpPack(
    const BST_CHAR *strUserName,
    const BST_CHAR *strUserNameAndPass,
    const BST_CHAR *strDeviceId,
    const BST_EMAIL_HTTP_REQ_HEAD_STRU *pstHttpReqHead,
    const BST_EMAIL_WBXML_BODY_STRU *pstWbxmlBody )
{
    BST_BOOL            bRtnVal;
    BST_LIB_StrStru    *pReqString;
    if ( BST_NULL_PTR == pstHttpReqHead )
    {
        return 0;
    }
    if ( BST_NULL_PTR == pstWbxmlBody )
    {
        return 0;
    }
    if ( BST_NULL_PTR == m_pExcgHttp )
    {
        return 0;
    }
    if ( SYNC_PUSH == m_pstAccountInfo->enSyncType )/*Ping命令*/
    {
        bRtnVal     = m_pExcgHttp->ConfigRqst(
                        strUserName,
                        strUserNameAndPass,
                        strDeviceId,
                        pstHttpReqHead, 
                        pstWbxmlBody,
                        PING_CMD );
        if ( BST_FALSE == bRtnVal )
        {
            BST_RLS_LOG("BST_APP_CEmailExchange::ComposeHttpPack ConfigRqst ping failed.");
            return 0;
        }
    }
    else
    {
        bRtnVal     = m_pExcgHttp->ConfigRqst(
                        strUserName,
                        strUserNameAndPass,
                        strDeviceId,
                        pstHttpReqHead,
                        pstWbxmlBody,
                        SYNC_CMD );
        if ( BST_FALSE == bRtnVal )
        {
            BST_RLS_LOG("BST_APP_CEmailExchange::ComposeHttpPack ConfigRqst sync failed.");
            return 0;
        }
    }

    pReqString      = m_pExcgHttp->GetRequest();

    if( BST_NULL_PTR != pReqString)
    {
        return pReqString->usUsed;
    }
    return 0;
}
BST_VOID BST_APP_CEmailExchange::FillWbxmlBody(
    BST_EMAIL_WBXML_BODY_STRU *pstWbxmlBody )
{
    BST_CHAR           *pExcgCollectionId;
    BST_CHAR           *pExcgSyncKey;

    if (BST_NULL_PTR == pstWbxmlBody)
    {
        return;
    }
    if ( BST_NULL_PTR == m_pExcgCollectionId )
    {
        return;
    }
    if ( BST_NULL_PTR == m_pExcgSyncKey )
    {
        return;
    }
    pExcgCollectionId   = m_pExcgCollectionId->pData;
    pExcgSyncKey        = m_pExcgSyncKey->pData;
    BST_LIB_StrAddBytes( pstWbxmlBody->pstClctId,
                         pExcgCollectionId,
                         m_pExcgCollectionId->usUsed );
    BST_LIB_StrAddBytes( pstWbxmlBody->pstSyncKey,
                         pExcgSyncKey,
                         m_pExcgSyncKey->usUsed );
    BST_LIB_StrAddStr( pstWbxmlBody->psWinSize, "4" );
}

/*lint -e438*/
BST_BOOL BST_APP_CEmailExchange::BuildRequest( BST_VOID )
{
    BST_UINT16          usSndBufLen;
    BST_UINT8          *pcUrlUserName;
    BST_UINT8          *pEncode64;
    BST_CHAR           *strDeviceId;
    BST_EMAIL_HTTP_REQ_HEAD_STRU   *pstHttpReqHead;

    BST_EMAIL_WBXML_BODY_STRU      *pstWbxmlBody;

    BST_CHAR           *pcAuth;
    BST_UINT16          usAuthLen;

    BST_ASSERT_NULL_RTN( m_pDeviceID, BST_FALSE );
    BST_ASSERT_NULL_RTN( m_pstAccountInfo, BST_FALSE );

    BST_APP_BuildHttpHead( pstHttpReqHead );

    BST_APP_BuildWbxmlBody( pstWbxmlBody );

    /*
     * 用户名和密码总长度，加上":",格式如下: "USER:PASSWORD"
     */
    usAuthLen           = m_pstAccountInfo->pUserName->usUsed
                        + 1
                        + m_pstAccountInfo->pPassWord->usUsed;

    pcAuth              = (BST_CHAR *)BST_OS_MALLOC( BST_EXCG_URL_NAME_LEN );

    if ( BST_NULL_PTR == pcAuth )
    {
        BST_APP_DeleteHttpHead( pstHttpReqHead );
        BST_APP_DeleteWbxmlBody( pstWbxmlBody );
        return BST_FALSE;
    }
    /*
     * 密码和用户名总长度太长，组包失败
     */
    if ( usAuthLen > BST_EXCG_URL_NAME_LEN )
    {
        BST_OS_FREE( pcAuth );
        BST_APP_DeleteHttpHead( pstHttpReqHead );
        BST_APP_DeleteWbxmlBody( pstWbxmlBody );
        return BST_FALSE;
    }
    /*
     * 将"USER:PASSWORD"拼接
     */
    BST_OS_MEMCPY( pcAuth,
                   m_pstAccountInfo->pUserName->pData,
                   m_pstAccountInfo->pUserName->usUsed );
    BST_OS_MEMCPY( pcAuth+m_pstAccountInfo->pUserName->usUsed,
                   ":",
                   1 );
    BST_OS_MEMCPY( pcAuth+m_pstAccountInfo->pUserName->usUsed+1,
                   m_pstAccountInfo->pPassWord->pData,
                   m_pstAccountInfo->pPassWord->usUsed );
    /*
     * 将"USER:PASSWORD"进行base64编码
     */
    pEncode64           = EncodeBase64( (BST_UINT8 *)pcAuth, usAuthLen);

    BST_OS_FREE( pcAuth );

    if ( BST_NULL_PTR == pEncode64 )
    {
        BST_APP_DeleteHttpHead( pstHttpReqHead );
        BST_APP_DeleteWbxmlBody( pstWbxmlBody );
        return BST_FALSE;
    }

    pcUrlUserName       = (BST_UINT8 *)BST_OS_MALLOC( BST_EXCG_URL_NAME_LEN );

    if ( BST_NULL_PTR == pcUrlUserName )
    {
        BST_OS_FREE( pEncode64 );
        BST_APP_DeleteHttpHead( pstHttpReqHead );
        BST_APP_DeleteWbxmlBody( pstWbxmlBody );
        return BST_FALSE;
    }

    /*
     *  AP传下来的用户名转化为url编码时是包含上层配置的服务器，
     *  如qq填写就是把@qq.com\54445@qq.com整个进行url编码
     */
    if ( BST_NULL_PTR == EncodeUrl
                        ( (BST_UINT8 *)m_pstAccountInfo->pUserName->pData,
                        pcUrlUserName,
                        m_pstAccountInfo->pUserName->usUsed,
                        BST_EXCG_URL_NAME_LEN ) )
    {
        BST_OS_FREE( pcUrlUserName );
        BST_OS_FREE( pEncode64 );
        BST_APP_DeleteHttpHead( pstHttpReqHead );
        BST_APP_DeleteWbxmlBody( pstWbxmlBody );
        return BST_FALSE;
    }

    strDeviceId     = m_pDeviceID->pData;

    FillWbxmlBody( pstWbxmlBody );
    FillHttpHead( pstHttpReqHead );

    usSndBufLen     = ComposeHttpPack( (BST_CHAR *)pcUrlUserName,
                                       (BST_CHAR*)pEncode64, 
                                       strDeviceId,
                                       pstHttpReqHead,
                                       pstWbxmlBody );
    BST_OS_FREE( pcUrlUserName );
    BST_OS_FREE( pEncode64 );
    if ( 0 == usSndBufLen )
    {
        BST_RLS_LOG("BST_APP_CEmailExchange::BuildRequest GetRequest failed.");
        BST_APP_DeleteHttpHead( pstHttpReqHead );
        BST_APP_DeleteWbxmlBody( pstWbxmlBody );
        return BST_FALSE;
    }
    BST_APP_DeleteHttpHead( pstHttpReqHead );
    BST_APP_DeleteWbxmlBody( pstWbxmlBody );
    return BST_TRUE;
}
/*lint +e438*/

BST_ERR_ENUM_UINT8 BST_APP_CEmailExchange::QueryServer( BST_VOID )
{
    BST_LIB_StrStru    *pReqString;
    BST_ERR_ENUM_UINT8  enErrMsg;

    if ( BST_NULL_PTR == m_pExcgHttp )
    {
        BST_RLS_LOG("BST_APP_CEmailExchange::QueryServer failed: m_pExcgHttp invalid");
        return BST_ERR_INVALID_PTR;
    }
    pReqString          = m_pExcgHttp->GetRequest();
    if( BST_NULL_PTR == pReqString)
    {
        BST_RLS_LOG("BST_APP_CEmailExchange::QueryServer failed: pReqString invalid");
        return BST_ERR_ILLEGAL_PARAM;
    }
    enErrMsg            = SendCommand( (BST_UINT8*)pReqString->pData, pReqString->usUsed );
    if ( BST_NO_ERROR_MSG != enErrMsg )
    {
        BST_RLS_LOG("BST_APP_CEmailExchange::QueryServer Send failed.");
    }
    else
    {
        BST_RLS_LOG("BST_APP_CEmailExchange::QueryServer Send Success.");
    }
    return enErrMsg;
}

BST_ERR_ENUM_UINT8 BST_APP_CEmailExchange::SendCommand(
    const BST_UINT8 *pucCmd,
    BST_UINT16       usLength )
{
    BST_IP_ERR_T    lRtnVal;

    if ( ( BST_NULL_PTR == pucCmd ) || ( 0 == usLength ) )
    {
        return BST_ERR_INVALID_PTR;
    }
    if ( BST_NULL_PTR == m_pstAccountInfo )
    {
        return BST_ERR_INVALID_PTR;
    }
    if ( BST_NULL_PTR == m_pSyncSocket )
    {
        return BST_ERR_INVALID_PTR;
    }

    lRtnVal         = m_pSyncSocket->Write( pucCmd, usLength );
    if ( BST_IP_ERR_OK != lRtnVal )
    {
        BST_RLS_LOG1( "BST_APP_CEmailExchange::SendCommand Write to lwip error=%d!",
                     lRtnVal );
        return BST_ERR_UNREACHABLE;
    }
    /*是sync包就要开启定时器等待回复*/
    if ( SYNC_INTERVAL == m_pstAccountInfo->enSyncType )
    {
        if( !BST_OS_IsTimerValid(m_ulTimerId) )
        {
            return BST_ERR_ILLEGAL_PARAM;
        }
        BST_DBG_LOG1( "BST_APP_CEmailExchangehave::SendCommand start timer %u",
                      m_ulTimerId );
        BST_OS_TimerStart( m_ulTimerId, BST_APP_RX_TIME_OUT );
    }
    else/*ping包的话，直接撤票*/
    {
        if ( BST_NULL_PTR == m_pcHostTask)
        {
            return BST_ERR_TASK_REMOVED;
        }
        m_pcHostTask->Finished();
    }
    return BST_NO_ERROR_MSG;
}

BST_BOOL BST_APP_CEmailExchange::InitOtherSocket( BST_VOID )
{
    BST_IP_SOCKET_ADD_T    *pstSocketAddr;
    BST_IP_SOCKET_ADD_T     stSocketAddr;
    BST_UINT16              i;
    BST_IP_ERR_T            lIpErrMsg;
    if ( m_pSyncSocket == BST_NULL_PTR )
    {
        return BST_FALSE;
    }
    lIpErrMsg               = m_pSyncSocket->IoCtrl
                            ( BST_SCKT_CMD_GET_ADDRPTR, &pstSocketAddr );
    if ( lIpErrMsg != BST_IP_ERR_OK)
    {
        return BST_FALSE;
    }
    if ( !BST_IP_IsValidIpAddress( pstSocketAddr ) )
    {
        return BST_FALSE;
    }
    BST_OS_MEMCPY(&(stSocketAddr.aucRemoteIp[0]),
                    pstSocketAddr->aucRemoteIp,
                    BST_IP_ADDR_LENTH );

    stSocketAddr.usLocalPort    = pstSocketAddr->usLocalPort;
    stSocketAddr.usRemotePort   = pstSocketAddr->usRemotePort;
    for( i = 1; i < BST_APP_SOCKET_NUM_MAX; i++)
    {
        stSocketAddr.usLocalPort++;
        lIpErrMsg           = m_SocketArray[i]->IoCtrl
                            ( BST_SCKT_CMD_SET_ADDRESS, &stSocketAddr );
        if ( lIpErrMsg != BST_IP_ERR_OK )
        {
            BST_RLS_LOG1("m_SocketArray[%02d]->IoCtrl error", i);
            return BST_FALSE;
        }
        BST_DBG_LOG2("m_SocketArray[%02d]=%p", i, m_SocketArray[i]);
    }
    return BST_TRUE;
}


BST_VOID BST_APP_CEmailExchange::UpdateSocket( BST_VOID )
{
    BST_IP_TRAFFIC_FLOW_STRU    stTmpTrfcFlow;
    BST_IP_SOCKET_ADD_T        *pstSocketAddr;
    BST_IP_ERR_T                lIpErrMsg;
    /*赋初值*/
    stTmpTrfcFlow.ulTxByte      = 0;
    stTmpTrfcFlow.ulRxByte      = 0;
    m_usSocketCnt++;
    m_usSocketCnt              &= (BST_APP_SOCKET_NUM_MASK);
    if ( BST_NULL_PTR != m_pSyncSocket )
    {
        m_pSyncSocket->IoCtrl( BST_SCKT_CMD_GET_TRAFFIC_FLOW, &stTmpTrfcFlow );
    }
    m_pSyncSocket               = m_SocketArray[m_usSocketCnt];
    if ( BST_NULL_PTR != m_pSyncSocket )
    {
        m_pSyncSocket->IoCtrl( BST_SCKT_CMD_SET_TRAFFIC_FLOW, &stTmpTrfcFlow );
    
        lIpErrMsg                   = m_pSyncSocket->IoCtrl
                                    ( BST_SCKT_CMD_GET_ADDRPTR, &pstSocketAddr );
        if ( lIpErrMsg == BST_IP_ERR_OK)
        {
            BST_DBG_UPDATE_IP_INFO( m_pcHostTask->m_usProcId,
                                    m_pcHostTask->m_usTaskId,
                                    pstSocketAddr );
        }
        BST_DBG_UPDATE_SUC_NUM( m_pcHostTask->m_usProcId, m_pcHostTask->m_usTaskId );
    }
    BST_DBG_LOG1("CEmailExchange::UpdateSocket m_pSyncSocket = %p", m_pSyncSocket );
}
BST_UINT8 *BST_APP_CEmailExchange::EncodeBase64(
    const BST_UINT8 *pstrIn,
    BST_UINT16       usLength ) const
{
    const BST_UINT8 s_cBase64Pad    = '=';
    const BST_UINT8        *pcCurrent;
    BST_UINT8              *p;
    BST_UINT8              *pcResult;
    pcCurrent               = pstrIn;
    p                       = BST_NULL_PTR;
    pcResult                = BST_NULL_PTR;

    if ( 0 == usLength )
    {
        return BST_NULL_PTR;
    }
    if ( BST_NULL_PTR == pcCurrent )
    {
        return BST_NULL_PTR;
    }
    /*为编码后的字符分配空间，为存储'\0'多分配一个字节,每3个字节要变成4个字节，不够被3整除要补够*/
    pcResult                = ( BST_UINT8 * )BST_OS_MALLOC(
                            ( ( usLength + 2 ) / 3 ) * 4 * BST_OS_SIZEOF( BST_UINT8 ) + 1 );
    if ( BST_NULL_PTR == pcResult)
    {
        return BST_NULL_PTR;
    }
    p                       = pcResult;

    /* 将输入字符串每3个字节一组共3*8bit变成4个字节的字符4*6bit，高2位补0 */
    while ( usLength > 2 )
    {
        /* 将第一个字节的高6位转化成base64表中的字符 */
        *p++                = acBase64Table[pcCurrent[0] >> 2];
        *p++                = acBase64Table
                              [( ( pcCurrent[0] & 0x03 ) << 4 ) + ( pcCurrent[1] >> 4 )];
        *p++                = acBase64Table
                              [( ( pcCurrent[1] & 0x0f ) << 2 ) + ( pcCurrent[2] >> 6 )];
        *p++                = acBase64Table[pcCurrent[2] & 0x3f];

        pcCurrent          += 3;
        usLength           -= 3;
    }
    /*长度不能被3整除的字符串*/
    if ( usLength != 0 )
    {
        *p++                = acBase64Table[pcCurrent[0] >> 2];
        /* usLength = 2,2*8/6 = 2余4，占3个字节，最后一个字节用“=”补齐 */
        if ( usLength > 1 )
        {
            *p++            = acBase64Table
                              [( ( pcCurrent[0] & 0x03 ) << 4 ) + ( pcCurrent[1] >> 4 )];
            *p++            = acBase64Table
                              [( pcCurrent[1] & 0x0f ) << 2];
            *p++            = s_cBase64Pad;
        }
        else  /*usLength = 1,1*8/6 = 1余2，占2个字节，最后2个字节用“=”补齐*/
        {
            *p++            = acBase64Table[( pcCurrent[0] & 0x03 ) << 4];
            *p++            = s_cBase64Pad;
            *p++            = s_cBase64Pad;
        }
    }

    *p                      = '\0';
    return pcResult;
}

/*lint -e429 -e661*/
BST_UINT8 *BST_APP_CEmailExchange::EncodeUrl(
    const BST_UINT8 *cStr,
    BST_UINT8       *pstrResult,
    const BST_UINT16 usStrSize,
    const BST_UINT16 usResultSize ) const
{
    BST_UINT16      i;
    BST_UINT16      j;
    BST_UINT8       cChar;

    BST_ASSERT_NULL_RTN( cStr, BST_NULL_PTR );
    BST_ASSERT_NULL_RTN( pstrResult, BST_NULL_PTR );
    BST_ASSERT_0_RTN(usStrSize, BST_NULL_PTR);
    BST_ASSERT_0_RTN(usResultSize, BST_NULL_PTR);

    for ( i = 0, j = 0; ( i < usStrSize ) && ( j < usResultSize ); ++i )
    {
        cChar       = cStr[i];

        if ( BST_APP_IS_NUMBERORCHAR(cChar) )
        {
            pstrResult[j++] = cChar;
        }
        else if ( cChar == ' ' )
        {
            pstrResult[j++] = '+';
        }
        else if ( BST_APP_IS_SPECIALCHAR(cChar) )
        {
            pstrResult[j++] = cChar;
        }
        else
        {
            if ( j + 3 < usResultSize )
            {
                sprintf( (BST_CHAR *)(pstrResult + j), "%%%02X", cChar );
                j          += 3;
            }

            else
            {
                return BST_NULL_PTR;
            }
        }
    }

    pstrResult[j]           = '\0';
    return pstrResult;
}
/*lint +e429 +e661*/
