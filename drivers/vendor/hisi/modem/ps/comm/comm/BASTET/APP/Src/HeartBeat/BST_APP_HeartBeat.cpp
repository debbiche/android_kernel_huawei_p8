
#include "BST_APP_HeartBeat.h"
#include "BST_DBG_MemLeak.h"
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_BST_APP_HEARTBEAT_CPP
/*lint +e767*/

HeartBeat::HeartBeat( BST_PROCID_T usInProcID, BST_TASKID_T usInTaskID )
    : BST_CORE_CNPTask( usInProcID, usInTaskID )
{
    m_pstHbPktContent                       = BST_NULL_PTR;
    m_ulHbSerNum                            = 0;
    m_stHbTxRetry.ulTimerId                 = BST_OS_INVALID_TIMER_ID;
    m_stHbTxRetry.usRetryNum                = 0;
    m_stHbTxRetry.stRetryInfo.ulMaxReTimes  = BST_HB_DEFAULT_RETRY_TIMES;
    m_stHbTxRetry.stRetryInfo.ulIntervals   = BST_APP_RX_TIME_OUT;
}


HeartBeat::~HeartBeat( BST_VOID )
{
    if ( BST_OS_IsTimerValid( m_stHbTxRetry.ulTimerId ) )
    {
        BST_OS_TimerRemove( m_stHbTxRetry.ulTimerId );
        m_stHbTxRetry.ulTimerId         = BST_OS_INVALID_TIMER_ID;
    }

    if ( BST_NULL_PTR == m_pstHbPktContent )
    {
        BST_RLS_LOG( "Memory leak occured" );
        return;
    }
#if ( BST_OS_VER != BST_QCOM_ROS )
    try
    {
#endif
        if ( BST_NULL_PTR != m_pSocket )
        {
            m_pSocket->FreePcb();
        }
    BST_LIB_StrDelete( m_pstHbPktContent->pFixedContent );
    BST_LIB_StrDelete( m_pstHbPktContent->pRespContent );
    BST_LIB_StrDelete( m_pstHbPktContent->pCipherKey );
#if ( BST_OS_VER != BST_QCOM_ROS )
    }
    catch (...)
    {
        BST_RLS_LOG( "HeartBeat destruction exception!" );
    }
#endif
    if ( BST_NULL_PTR != m_pstHbPktContent->pstSnInfoPkt )
    {
        BST_OS_FREE( m_pstHbPktContent->pstSnInfoPkt );
    }

    BST_OS_FREE( m_pstHbPktContent );
}


BST_VOID HeartBeat::Entry( BST_VOID )
{
    BST_IP_ERR_T                        lSockRtnVal;
    BST_ERR_ENUM_UINT8                  enRtnVal;
    BST_BOOL                            bRtnVal;

    lSockRtnVal                         = BST_IP_ERR_OK;
    bRtnVal                             = BST_FALSE;

    if ( BST_FALSE == IsConfigOk() )
    {
        Finished();
        return;
    }

    switch ( m_ProtolType )
    {
        case BST_PRTC_TYPE_TCP_LONG:

#if ( BST_OS_VER == BST_WIN32 && BST_VER_TYPE == BST_DBG_VER )
            {
                static int ini_flag     = 0;
                if ( 0== ini_flag )
                {
                    ini_flag            = 1;
                    m_pSocket->Bind();
                    m_pSocket->Connect();
                    Finished();
                    return;
                }
            }
#endif
            bRtnVal                     = SendHbPkt();
            if ( ( BST_TRUE == bRtnVal ) && ( BST_OS_IsTimerValid( m_stHbTxRetry.ulTimerId ) ) )
            {
                BST_OS_TimerStart( m_stHbTxRetry.ulTimerId, m_stHbTxRetry.stRetryInfo.ulIntervals );
                m_stHbTxRetry.usRetryNum
                                        = 0;
            }
            else
            {
                BST_RLS_LOG2( "Heartbeat::Entry ProcId=%d, TaskId=%d, Send Heartbeat packet failed",
                              m_usProcId, m_usTaskId );
                Finished();
                Suspend();
                enRtnVal                = Report( BST_ERR_CLIENT_EXCEPTION );
                if ( BST_NO_ERROR_MSG != enRtnVal )
                {
                    BST_RLS_LOG3( "Heartbeat::Entry ProcId=%d, TaskId=%d, Entry Report Error: %u",
                                  m_usProcId, m_usTaskId, enRtnVal );
                }
                return;
            }
            break;

        case BST_PRTC_TYPE_TCP_SHORT:
            BST_ASSERT_NULL( m_pSocket );
            lSockRtnVal                 = m_pSocket->Bind();
            BST_ASSERT_NORM( BST_IP_ERR_OK != lSockRtnVal );
            lSockRtnVal                 = m_pSocket->Connect();
            BST_ASSERT_NORM( BST_IP_ERR_OK != lSockRtnVal );
            break;

        default:
            break;
    }
#if ( BST_OS_VER == BST_WIN32 && BST_VER_TYPE == BST_DBG_VER )
    Finished();
#endif
}

/*lint -e438*/
BST_BOOL HeartBeat::SendHbPkt( BST_VOID )
{
    BST_UINT8               *pucSndData;
    BST_UINT16              usSndLength;
    BST_UINT16              usPktLength;
    BST_UINT8               *pucRtnVal;
    BST_IP_ERR_T            lRtnVal;

    pucSndData              = BST_NULL_PTR;
    lRtnVal                 = BST_IP_ERR_OK;

    usSndLength             = 0;

    if ( ( BST_NULL_PTR == m_pstHbPktContent ) || ( BST_NULL_PTR == m_pSocket ) )
    {
        return BST_FALSE;
    }

    usPktLength             = m_pstHbPktContent->usHbPktLength;
    pucSndData              = ( BST_UINT8 * )BST_OS_MALLOC( usPktLength );
    if ( BST_NULL_PTR == pucSndData )
    {
        return BST_FALSE;
    }

    pucRtnVal               = ComposeHbPkt( pucSndData, &usSndLength, usPktLength );
    if ( BST_NULL_PTR == pucRtnVal )
    {
        BST_DBG_LOG2( "Heartbeat::SendHbPkt ProcId=%d, TaskId=%d, Composing packet failed",
                      m_usProcId, m_usTaskId );
        BST_OS_FREE( pucSndData );
        return BST_FALSE;
    }

    lRtnVal                 = m_pSocket->Write( pucSndData, usSndLength );
    if ( BST_IP_ERR_OK != lRtnVal )
    {
        BST_DBG_LOG2( "Heartbeat::SendHbPkt ProcId=%d, TaskId=%d, socket writing failed",
                      m_usProcId, m_usTaskId );
        BST_OS_FREE( pucSndData );
        return BST_FALSE;
    }

#if ( BST_VER_TYPE == BST_RLS_VER )
    {
        BST_UINT16          i;
        for ( i = 0; i < usSndLength; i++ )
        {
            BST_DBG_LOG4( "Heartbeat::SendHbPkt ProcId=%d, TaskId=%d, Write Data:pucSndData[%d] = 0x%02x",
                          m_usProcId, m_usTaskId, i, pucSndData[i] );
        }
    }
#endif

    BST_DBG_LOG2( "Heartbeat::SendHbPkt ProcId=%d, TaskId=%d, heartbeat packet is sent",
                  m_usProcId, m_usTaskId );
    BST_OS_FREE( pucSndData );
    return BST_TRUE;
}
/*lint +e438*/

BST_IP_ERR_T HeartBeat::Connectd( BST_FD_T fd )
{
    BST_BOOL                            bRtnVal;
    bRtnVal                             = BST_FALSE;

    BST_DBG_LOG2( "Heartbeat::Connectd ProcId=%d, TaskId=%d, Connection is built",
                  m_usProcId, m_usTaskId );
    if ( BST_PRTC_TYPE_TCP_SHORT != m_ProtolType )
    {
        return BST_IP_ERR_OK;
    }
    bRtnVal                             = SendHbPkt();
    if ( BST_TRUE != bRtnVal )
    {
        BST_RLS_LOG2( "Heartbeat::Connectd ProcId=%d, TaskId=%d, Send heartbeat packet Error",
                      m_usProcId, m_usTaskId );
    }

    return BST_IP_ERR_OK;
}


BST_IP_PKTPROC_MODE_ENUM HeartBeat::Received (
    BST_FD_T         fd,
    const BST_UINT8 *const pucData,
    const BST_UINT16 usLength )
{
    BST_ERR_ENUM_UINT8              ucRtnVal;
    ucRtnVal                        = BST_NO_ERROR_MSG;

    Finished();

    if ( BST_OS_IsTimerValid( m_stHbTxRetry.ulTimerId ) )
    {
        BST_OS_TimerStop( m_stHbTxRetry.ulTimerId );
        m_stHbTxRetry.usRetryNum    = 0;
    }

    if ( ( 0 == usLength ) || ( BST_NULL_PTR == pucData ) )
    {
        return BST_IP_PKT_REMOVE_PROC;
    }

    ucRtnVal                        = ParsePkt( pucData, usLength );
    if ( BST_NO_ERROR_MSG == ucRtnVal )           /*说明是心跳包回复*/
    {
        return BST_IP_PKT_REMOVE_PROC;
    }
    return BST_IP_PKT_FORWARD_PROC;
}
BST_VOID HeartBeat::Closed ( BST_FD_T fd )
{
    BST_ERR_ENUM_UINT8              ucRtnVal;
    BST_TASK_STATE_ENUM_UINT8       enTaskState;
    BST_FD_T                        unFd;
    BST_IP_ERR_T                    lSockRtnVal;

    unFd                            = fd;
    lSockRtnVal                     = BST_IP_ERR_OK;
    ucRtnVal                        = BST_NO_ERROR_MSG;
    enTaskState                     = GetState();

    Finished();
    if ( BST_TASK_STATE_START != enTaskState )
    {
        BST_RLS_LOG3( "Heartbeat::Closed ProcId=%d, TaskId=%d, Unexpected task state: %d",
                      m_usProcId, m_usTaskId, enTaskState );
        return;
    }
    Suspend();

    if ( BST_OS_IsTimerValid( m_stHbTxRetry.ulTimerId ) )
    {
        BST_OS_TimerStop( m_stHbTxRetry.ulTimerId );
        m_stHbTxRetry.usRetryNum    = 0;
    }

    if ( BST_NULL_PTR == m_pSocket)
    {
        return;
    }

    /* 收到对端的断开消息，本地关闭套接字 */
    lSockRtnVal                     = m_pSocket->IoCtrl( BST_SCKT_CMD_CHK_FD, &unFd );
    if ( BST_IP_ERR_OK != lSockRtnVal )
    {
        BST_RLS_LOG2( "Heartbeat::Closed ProcId=%d, TaskId=%d, Check local socket failed",
                      m_usProcId, m_usTaskId );
        return;
    }
    m_pSocket->Close();

    /* 通知AP清除该Socket记录 */
    ucRtnVal                        = Report( BST_ERR_SOCKET_CLSD );
    if ( BST_NO_ERROR_MSG != ucRtnVal )
    {
        BST_RLS_LOG3( "Heartbeat::Closed ProcId=%d, TaskId=%d, Closed Report Error: %u",
                      m_usProcId, m_usTaskId, ucRtnVal );
    }
    BST_DBG_LOG2( "HeartBeat::Closed ProcId=%d, TaskId=%d, Connection is closed",
                  m_usProcId, m_usTaskId );
}

BST_VOID HeartBeat::ErrClosed(
    BST_FD_T fd,
    BST_IP_ERR_T enIpErr )
{
    BST_ERR_ENUM_UINT8                  ucRtnVal;
    ucRtnVal                            = BST_NO_ERROR_MSG;

    BST_RLS_LOG3( "Heartbeat::ErrClosed ProcId=%d, TaskId=%d, Socket enIpErr: %d",
                      m_usProcId, m_usTaskId, enIpErr );

    Finished();
    Suspend();

    if ( BST_OS_IsTimerValid( m_stHbTxRetry.ulTimerId ) )
    {
        BST_OS_TimerStop( m_stHbTxRetry.ulTimerId );
        m_stHbTxRetry.usRetryNum        = 0;
    }
    BST_DBG_UPDATE_ERR_NUM( m_usProcId, m_usTaskId );
    ucRtnVal                            = Report( BST_ERR_SOCKET_CLSD );
    if ( BST_NO_ERROR_MSG != ucRtnVal )
    {
        BST_RLS_LOG3( "Heartbeat::ErrClosed ProcId=%d, TaskId=%d, ErrClosed Report Error: %u",
                      m_usProcId, m_usTaskId, ucRtnVal );
    }
}


BST_ERR_ENUM_UINT8 HeartBeat::ParsePkt(
    const BST_UINT8    *pucRcvData,
    const BST_UINT16    usLength )
{
    BST_UINT16                  usRespLength;
    BST_UINT8                   *pucRespTmp;
    BST_BOOL                    bRtnVal;
    BST_ERR_ENUM_UINT8          ucRtnVal;
    usRespLength                = 0;
    bRtnVal                     = BST_FALSE;
    ucRtnVal                    = BST_NO_ERROR_MSG;
    pucRespTmp                  = BST_NULL_PTR;

    if ( ( BST_NULL_PTR == pucRcvData ) || ( 0 == usLength ) )
    {
        return BST_ERR_ILLEGAL_PARAM;
    }
    if ( BST_NULL_PTR == m_pstHbPktContent )
    {
        return BST_ERR_INVALID_PTR;
    }
    if ( BST_NULL_PTR == m_pstHbPktContent->pRespContent )
    {
        return BST_ERR_INVALID_PTR;
    }
    usRespLength                = m_pstHbPktContent->pRespContent->usUsed;
    pucRespTmp                  = ( BST_UINT8 * )m_pstHbPktContent->pRespContent->pData;

    bRtnVal                     = IsRespPkt( pucRcvData, pucRespTmp, usLength, usRespLength );
    if ( BST_FALSE == bRtnVal )
    {
        BST_RLS_LOG2( "HeartBeat::ParsePkt ProcId=%d,TaskId=%d, This is not a heartbeat reply",
                       m_usProcId, m_usTaskId );
        Stop();

        ucRtnVal                = Report( BST_NOT_HEARTBEAT_PACK );
        if ( BST_NO_ERROR_MSG != ucRtnVal )
        {
            BST_RLS_LOG3( "HeartBeat::ParsePkt ProcId=%d,TaskId=%d, ParsePkt Report Error: %u",
                          m_usProcId, m_usTaskId, ucRtnVal );
        }

        return BST_NOT_HEARTBEAT_PACK;
    }
    else
    {
        BST_RLS_LOG2( "HeartBeat::ParsePkt ProcId=%d, TaskId=%d, This is a heartbeat reply.",
                      m_usProcId, m_usTaskId );
        if ( BST_HB_IsHbSnUsed( m_pstHbPktContent ) )
        {
            m_ulHbSerNum       += m_pstHbPktContent->pstSnInfoPkt->ulSnStep;
        }

        BST_DBG_UPDATE_SUC_NUM( m_usProcId, m_usTaskId );
        return BST_NO_ERROR_MSG;
    }
}


/*lint -e438 -e429*/
BST_UINT8 *HeartBeat::ComposeHbPkt(
    BST_UINT8  *pucCmpsData,
    BST_UINT16 *pusSndLen,
    BST_UINT16  usCmpsLen )
{
    BST_UINT16                      usCopyedLen;
    BST_UINT16                      usFixedLen;
    SERNO_HB_INFO_STRU             *pstHbSnInfo;
    BST_UINT8                      *pucFixedContent;
    BST_UINT8                      *ucCharTmp;
    BST_UINT32                      ulHbSnNum;

    pstHbSnInfo                     = BST_NULL_PTR;
    pucFixedContent                 = BST_NULL_PTR;
    ucCharTmp                       = (BST_UINT8 *)(&ulHbSnNum);
    usCopyedLen                     = 0;

    BST_ASSERT_0_RTN( usCmpsLen, BST_NULL_PTR );
    BST_ASSERT_NULL_RTN( pucCmpsData, BST_NULL_PTR );
    BST_ASSERT_NULL_RTN( pusSndLen, BST_NULL_PTR );
    BST_ASSERT_NULL_RTN( m_pstHbPktContent, BST_NULL_PTR );
    BST_ASSERT_NULL_RTN( m_pstHbPktContent->pFixedContent, BST_NULL_PTR );
    BST_ASSERT_NULL_RTN( m_pstHbPktContent->pstSnInfoPkt, BST_NULL_PTR );

    usFixedLen                      = m_pstHbPktContent->pFixedContent->usUsed;
    pucFixedContent                 = ( BST_UINT8 *)( m_pstHbPktContent->pFixedContent->pData);

    if ( usCmpsLen < usFixedLen )            /*内存拷贝越界检查*/
    {
        BST_RLS_LOG2( "HeartBeat::ComposeHbPkt ProcId=%d, TaskId=%d, out of memory",
                      m_usProcId, m_usTaskId );
        return BST_NULL_PTR;
    }
    BST_OS_MEMCPY( pucCmpsData, pucFixedContent, usFixedLen );
    usCopyedLen                    += usFixedLen;

    if( BST_HB_IsHbSnUsed( m_pstHbPktContent ) )
    {
        if ( usCmpsLen < ( usCopyedLen + BST_OS_SIZEOF( m_ulHbSerNum ) ) )
        {
            BST_RLS_LOG2( "HeartBeat::ComposeHbPkt ProcId=%d, TaskId=%d, out of memory",
                          m_usProcId, m_usTaskId );
            return BST_NULL_PTR;
        }

        pstHbSnInfo                 = m_pstHbPktContent->pstSnInfoPkt;
        if ( !BST_HB_IsHbSeqValid( m_ulHbSerNum, pstHbSnInfo ) )
        {
            m_ulHbSerNum            = pstHbSnInfo->ulSnMin;
        }

        BST_HB_U32TOU8( ucCharTmp[3], ucCharTmp[2], ucCharTmp[1], ucCharTmp[0], m_ulHbSerNum );
        BST_OS_MEMCPY( pucCmpsData + usCopyedLen, &ucCharTmp[0], BST_OS_SIZEOF( m_ulHbSerNum ) );
        usCopyedLen                += BST_OS_SIZEOF( m_ulHbSerNum );
    }

   *pusSndLen                       = usCopyedLen;
    BST_DBG_LOG2( "Heartbeat::ComposeHbPkt ProcId=%d, TaskId=%d, Composing Packet finished",
                  m_usProcId, m_usTaskId );
    return pucCmpsData;
}
/*lint +e438 +e429*/

BST_BOOL HeartBeat::IsRespPkt(
    const BST_UINT8 *pucData,
    BST_UINT8 *pucRespData,
    BST_UINT16 usDataLen,
    BST_UINT16 usRespLen ) const
{
    if ( ( 0 == usDataLen )
      || ( 0 == usRespLen ) )
    {
        return BST_FALSE;
    }
    if ( ( BST_NULL_PTR == pucData )
      || ( BST_NULL_PTR == pucRespData ) )
    {
        return BST_FALSE;
    }
    if ( usDataLen != usRespLen )
    {
        return BST_FALSE;
    }

    if ( 0 != BST_OS_MEMCMP( pucData, pucRespData, usDataLen ) )
    {
        return BST_FALSE;
    }

    return BST_TRUE;
}


BST_BOOL HeartBeat::IsConfigOk( BST_VOID ) const
{
    BST_DRV_STRU                       *pNetDrvHandle;
    BST_BOOL                            bNetState;

    pNetDrvHandle                       = BST_DRV_NetGetDvcHandle();
    pNetDrvHandle->pfIoCtrl( BST_DRV_CMD_GET_NET_STATE, &bNetState );

    if( ( BST_TRUE != pNetDrvHandle->ulIsInited )
     || ( BST_TRUE != bNetState ) )
    {
        BST_RLS_LOG2( "HeartBeat::IsConfigOk ProcId=%d, TaskId=%d, config is not OK: Net Device",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }
    if ( BST_NULL_PTR == m_pstHbPktContent )
    {
        BST_RLS_LOG2( "HeartBeat::IsConfigOk ProcId=%d, TaskId=%d, config is not OK: m_pstHbPktContent",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }

    if ( !BST_LIB_IsStringInit(m_pstHbPktContent->pFixedContent) )
    {
        BST_RLS_LOG2( "HeartBeat::IsConfigOk ProcId=%d, TaskId=%d, config is not OK: pFixedContent",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }

    if ( !BST_LIB_IsStringInit( m_pstHbPktContent->pRespContent ) )
    {
        BST_RLS_LOG2( "HeartBeat::IsConfigOk ProcId=%d, TaskId=%d, config is not OK: pRespContent",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }

    return BST_TRUE;
}


BST_VOID HeartBeat::TimerExpired(
    BST_OS_TIMERID_T ulId,
    BST_VOID        *pvPara )
{
    BST_ERR_ENUM_UINT8                  ucRtnVal;
    ucRtnVal                            = BST_NO_ERROR_MSG;

    if( BST_HB_INVALID_RETRY_TIMES == m_stHbTxRetry.stRetryInfo.ulMaxReTimes )
    {
        return;
    }

    m_stHbTxRetry.usRetryNum++;

    if ( m_stHbTxRetry.stRetryInfo.ulMaxReTimes <= m_stHbTxRetry.usRetryNum )
    {
        BST_RLS_LOG2( "HeartBeat::TimerExpired ProcId=%d, TaskId=%d, Tx retry is finished",
                      m_usProcId, m_usTaskId );
        if ( BST_OS_IsTimerValid( m_stHbTxRetry.ulTimerId ) )
        {
            BST_OS_TimerStop( m_stHbTxRetry.ulTimerId );
            m_stHbTxRetry.usRetryNum    = 0;
        }
        Finished();
        Suspend();
        ucRtnVal                        = Report( BST_APP_SERVER_TIMEOUT );
        if ( BST_NO_ERROR_MSG != ucRtnVal )
        {
            BST_RLS_LOG3( "HeartBeat::TimerExpired ProcId=%d, TaskId=%d, TimerExpired Report Error: %u",
                          m_usProcId, m_usTaskId, ucRtnVal );
        }
    }
    else
    {
        BST_DBG_LOG4( "HeartBeat::TimerExpired ProcId=%d, TaskId=%d, Tx retry, timerId:%u, usRetryNum:%u",
                      m_usProcId, m_usTaskId, m_stHbTxRetry.ulTimerId, m_stHbTxRetry.usRetryNum );
        SendHbPkt();
        BST_OS_TimerStart( m_stHbTxRetry.ulTimerId, m_stHbTxRetry.stRetryInfo.ulIntervals );
        BST_DBG_UPDATE_ERR_NUM( m_usProcId, m_usTaskId );
    }
}


BST_UINT16 HeartBeat::PidInquired(
    BST_CORE_PID_ENUM_UINT16  enParamId,
    BST_UINT16                usDataSize,
    BST_VOID                 *const pData )
{
    switch ( enParamId )
    {
        case BST_PID_SOCKET_PROPERTY:
            /* 当AP要查询当前TCP属性时，暂停当前的心跳发送及其定时器*/
            if ( BST_TRUE == m_ulBusyFlag )
            {
                Finished();
                if ( BST_OS_IsTimerValid( m_stHbTxRetry.ulTimerId ) )
                {
                    BST_OS_TimerStop( m_stHbTxRetry.ulTimerId );
                    m_stHbTxRetry.usRetryNum
                                        = 0;
                }
            }
            return ( BST_OS_SIZEOF ( BST_IP_SKT_PROPTY_STRU ) );

        default:
            return 0;
    }
}
BST_ERR_ENUM_UINT8 HeartBeat::PidConfiged(
    BST_CORE_PID_ENUM_UINT16     enParamId,
    BST_UINT16                   usLen,
    const BST_VOID              *const pData,
    BST_UINT32                   ulBaseCfged,
    BST_UINT8                  **pucNewAddr )
{
    BST_UINT8                          *pucRptr;
#if ( BST_OS_VER != BST_WIN32 || BST_VER_TYPE != BST_DBG_VER )
    BST_IP_ERR_T                        enIpErrMsg;
#endif
    pucRptr                             = BST_NULL_PTR;

    BST_ASSERT_NULL_RTN( pData, BST_ERR_ILLEGAL_PARAM );

    switch ( enParamId )
    {
        case BST_PID_HB_RESP_CONTENT:
            pucRptr                     = SetRespContent( ( BST_UINT8 * )pData, usLen );
            break;

        case BST_PID_HB_SEND_CONTENT:
            pucRptr                     = SetFixedContent( ( BST_UINT8 * )pData, usLen );
            break;

        case BST_PID_HB_CHECK_TYPE:
            pucRptr                     = SetCheckType( ( BST_UINT8 * )pData, usLen );
            break;

        case BST_PID_HB_SERIAL_NUM:
            pucRptr                     = SetHbSnInfo( pData, usLen );
            break;

        case BST_PID_CIPHER_CODE:
            pucRptr                     = SetCipherKey( ( BST_UINT8 * ) pData, usLen );
            break;

        case BST_PID_CIPHER_MODE:
            pucRptr                     = SetEncrypType( ( BST_UINT8 * ) pData, usLen );
            break;

        case BST_PID_PROTOCOL_TYPE:
            pucRptr                     = ConfigType( pData, usLen );
            if ( BST_NULL_PTR != pucRptr )
            {
                BST_SetConfigFlag( BST_CFGFLG_TYPE );
            }
#if ( BST_OS_VER != BST_WIN32 || BST_VER_TYPE != BST_DBG_VER )
            BST_HB_CLONE_SOCKET( enIpErrMsg, pucRptr );
#endif
            break;
        case BST_PID_SOCKET_INFO:
#if ( BST_OS_VER != BST_WIN32 || BST_VER_TYPE != BST_DBG_VER )
            BST_HB_CLONE_SOCKET( enIpErrMsg, pucRptr );
#endif
            break;

        case BST_PID_HB_TX_RETRY_MAX:
            pucRptr                     = SetAppTxRetry( pData, usLen );
            break;

        default:
            pucRptr                     = BST_NULL_PTR;
            break;
    }
    /*此处包含了property的下发判断，若想要测试无seq同步的，修改宏*/
    if ( BST_HB_IsBaseCfged( ulBaseCfged ) )
    {
        Start();
    }
   *pucNewAddr                          = pucRptr;

    if ( BST_NULL_PTR == pucRptr )
    {
       return BST_ERR_ILLEGAL_PARAM;
    }
    return BST_NO_ERROR_MSG;
}
BST_BOOL HeartBeat::SetSocketInfo( BST_SCKT_TYPE_ENUM_UINT32 enSocketType )
{
    BST_IP_ERR_T                enIpErrMsg;
    BST_IP_CNetRcver           *pNetRcver;

    pNetRcver                   = this;
    if ( BST_NULL_PTR == m_pSocket )
    {
        return BST_FALSE;
    }

    enIpErrMsg                  = m_pSocket->IoCtrl( BST_SCKT_CMD_DEF_TYPE,
                                                    ( BST_VOID * )&enSocketType );
    if ( BST_IP_ERR_OK != enIpErrMsg )
    {
        BST_RLS_LOG2( "HeartBeat::SetSocketInfo ProcId=%d, TaskId=%d, DEF_TYPE Err",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }

    if ( !BST_IsInfoConfiged( BST_CFGFLG_TYPE ) )
    {
        enIpErrMsg              = m_pSocket->IoCtrl( BST_SCKT_CMD_REG_CALLBACK,
                                                     pNetRcver );
        if( BST_IP_ERR_OK != enIpErrMsg )
        {
            BST_RLS_LOG2( "HeartBeat::SetSocketInfo ProcId=%d, TaskId=%d, REG_CALLBACK Err",
                          m_usProcId, m_usTaskId );
            return BST_FALSE;
        }
    }

    enIpErrMsg                  = m_pSocket->Socket();
    if( BST_IP_ERR_OK != enIpErrMsg )
    {
        return BST_FALSE;
    }
    return BST_TRUE;
}

BST_UINT8 *HeartBeat::ConfigType(
    const BST_VOID *const pData,
    BST_UINT16            usLen )
{
    BST_PRTC_TYPE_ENUM_UINT32           enPrtcType;
    BST_SCKT_TYPE_ENUM_UINT32           enSocketType;
    BST_UINT8                          *pucRtnPtr;
    BST_BOOL                            bRtnVal;
    if ( BST_IP_IsTypeInited ( m_ProtolType ) )
    {
        return BST_NULL_PTR;
    }
    if ( BST_FALSE == InitHeartBeat() )
    {
        return BST_NULL_PTR;
    }
    BST_ASSERT_NULL_RTN( pData, BST_NULL_PTR );
    BST_ASSERT_0_RTN( usLen, BST_NULL_PTR );
    BST_OS_MEMCPY( &enPrtcType, pData, usLen );

    pucRtnPtr                           = BST_NULL_PTR;

    switch ( enPrtcType )
    {
        case BST_PRTC_TYPE_TCP_LONG:
            enSocketType                = BST_SCKT_TYPE_RAW_TCP;
            break;

        case BST_PRTC_TYPE_TCP_SHORT:
            enSocketType                = BST_SCKT_TYPE_RAW_TCP;
            break;

        case BST_PRTC_TYPE_UDP:
            enSocketType                = BST_SCKT_TYPE_RAW_UDP;
            break;

        case BST_PRTC_TYPE_SSL:
            enSocketType                = BST_SCKT_TYPE_SSL;
            break;

        default:
            return BST_NULL_PTR;
    }

    if ( BST_IP_IsBsdSocketType( enSocketType ) )
    {
        SetRunMode( BST_TASK_RUN_BLOCKED );
    }
    else
    {
        SetRunMode( BST_TASK_RUN_UNBLOCKED );
    }

    bRtnVal                             = SetSocketInfo( enSocketType );
    if ( BST_FALSE == bRtnVal )
    {
        BST_RLS_LOG2( "HeartBeat::ConfigType ProcId=%d, TaskId=%d, SetSocketInfo Err",
                      m_usProcId, m_usTaskId );
        return BST_NULL_PTR;
    }
    m_ProtolType                        = enPrtcType;
    pucRtnPtr                           = (BST_UINT8 *)&m_ProtolType;
    return pucRtnPtr;
}


BST_UINT8 *HeartBeat::SetHbSnInfo(
    const BST_VOID *const pData,
    BST_UINT16            usLen )
{
    SERNO_HB_INFO_STRU                  stHbSnPkt;
    BST_UINT8                          *pucRtnPtr;

    pucRtnPtr                           = BST_NULL_PTR;

    if ( usLen != BST_OS_SIZEOF( SERNO_HB_INFO_STRU ) )
    {
        return BST_NULL_PTR;
    }
    if ( BST_NULL_PTR == pData )
    {
        return BST_NULL_PTR;
    }
    if ( BST_NULL_PTR == m_pstHbPktContent )
    {
        return BST_NULL_PTR;
    }
    if ( BST_NULL_PTR == m_pstHbPktContent->pstSnInfoPkt )
    {
        return BST_NULL_PTR;
    }

    BST_OS_MEMCPY( &stHbSnPkt, pData, usLen );

    m_pstHbPktContent->usHbPktLength   += usLen;
    m_pstHbPktContent->bSnUsed          = BST_TRUE;
   *(m_pstHbPktContent->pstSnInfoPkt)   = stHbSnPkt;
    m_ulHbSerNum                        = stHbSnPkt.ulSnBegin;/*给心跳包序列号赋值*/
    pucRtnPtr                           = (BST_UINT8 *)m_pstHbPktContent->pstSnInfoPkt;

#if ( BST_VER_TYPE == BST_DBG_VER )

    BST_DBG_LOG4( "HeartBeat::SetHbSnInfo ulSnMin:%u, ulSnMax:%u, ulSnBegin:%u, ulSnStep:%u",
                   stHbSnPkt.ulSnMin, stHbSnPkt.ulSnMax,
                   stHbSnPkt.ulSnBegin, stHbSnPkt.ulSnStep );

#endif
    return pucRtnPtr;
}
BST_BOOL HeartBeat::InitHeartBeat()
{
    if ( BST_NULL_PTR == m_pSocket )
    {
        BST_RLS_LOG2( "HeartBeat::InitHeartBeat ProcId=%d, TaskId=%d, m_pSocket=NULL",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }

    m_stHbTxRetry.ulTimerId             = BST_OS_TimerCreateCpp( this, BST_NULL_PTR );
    if ( !BST_OS_IsTimerValid( m_stHbTxRetry.ulTimerId ) )
    {
        BST_RLS_LOG2( "HeartBeat::InitHeartBeat ProcId=%d, TaskId=%d, create timer failed",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }

    m_pstHbPktContent                   = ( HB_PCKT_CONTENT_STRU * )BST_OS_MALLOC
                                            ( BST_OS_SIZEOF( HB_PCKT_CONTENT_STRU ) );
    if ( BST_NULL_PTR == m_pstHbPktContent )
    {
        BST_RLS_LOG2( "HeartBeat::InitHeartBeat ProcId=%d, TaskId=%d, m_pstHbPktContent=NULL",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }

    m_pstHbPktContent->pstSnInfoPkt     = ( SERNO_HB_INFO_STRU * )BST_OS_MALLOC
                                            ( BST_OS_SIZEOF( SERNO_HB_INFO_STRU ) );
    if ( BST_NULL_PTR == m_pstHbPktContent->pstSnInfoPkt )
    {
        BST_RLS_LOG2( "HeartBeat::InitHeartBeat ProcId=%d, TaskId=%d, pstSnInfoPkt=NULL",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }
    BST_OS_MEMSET( m_pstHbPktContent->pstSnInfoPkt, 0, BST_OS_SIZEOF( SERNO_HB_INFO_STRU ) );

    m_pstHbPktContent->pRespContent     = BST_LIB_StrCreat( BST_LIB_SHORT_STR_LEN );
    m_pstHbPktContent->pFixedContent    = BST_LIB_StrCreat( BST_LIB_SHORT_STR_LEN );
    m_pstHbPktContent->pCipherKey       = BST_LIB_StrCreat( BST_LIB_SHORT_STR_LEN );
    if ( BST_NULL_PTR == m_pstHbPktContent->pRespContent )
    {
        return BST_FALSE;
    }
    if ( BST_NULL_PTR == m_pstHbPktContent->pFixedContent )
    {
        return BST_FALSE;
    }
    if ( BST_NULL_PTR == m_pstHbPktContent->pCipherKey )
    {
        return BST_FALSE;
    }

    m_pstHbPktContent->usHbPktLength    = 0;
    m_pstHbPktContent->enSecMode        = BST_APP_ENCRP_INVALID_TYPE;
    m_pstHbPktContent->enCheckType      = BST_APP_HB_INVALID_CHK_TYPE;
    m_pstHbPktContent->bSnUsed          = BST_FALSE;

    if ( BST_FALSE == InitRegedit() )
    {
        BST_RLS_LOG2( "HeartBeat::InitHeartBeat ProcId=%d, TaskId=%d, Regist failed",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }

    return BST_TRUE;
}


BST_BOOL HeartBeat::InitRegedit( BST_VOID )
{
    BST_CORE_CRegedit                  *pcRegedit;
    BST_ERR_ENUM_UINT8                  enRtnVal;
    pcRegedit                           = BST_CORE_CRegedit::GetInstance();
    if ( BST_NULL_PTR == pcRegedit )
    {
        BST_RLS_LOG2( "HeartBeat::InitRegedit ProcId=%d, TaskId=%d, pcRegedit=NULL",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }

    enRtnVal                            = pcRegedit->Regist( this,
                                                             this,
                                                             BST_PID_HB_RESP_CONTENT,
                                                             m_pstHbPktContent->pRespContent->usUsed,
                                                             m_pstHbPktContent->pRespContent->pData );
    if ( BST_NO_ERROR_MSG != enRtnVal )
    {
        BST_DBG_LOG2( "Heartbeat::InitRegedit ProcId=%d, TaskId=%d, Regist BST_PID_HB_RESP_CONTENT error",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }
    enRtnVal                            = pcRegedit->Regist( this,
                                                             this,
                                                             BST_PID_HB_SEND_CONTENT,
                                                             m_pstHbPktContent->pFixedContent->usUsed,
                                                             m_pstHbPktContent->pFixedContent->pData );
    if ( BST_NO_ERROR_MSG != enRtnVal )
    {
        BST_DBG_LOG2( "Heartbeat::InitRegedit ProcId=%d, TaskId=%d, Regist BST_PID_HB_SEND_CONTENT error",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }
    enRtnVal                            = pcRegedit->Regist( this,
                                                             this,
                                                             BST_PID_CIPHER_CODE,
                                                             m_pstHbPktContent->pCipherKey->usUsed,
                                                             m_pstHbPktContent->pCipherKey->pData );
    if ( BST_NO_ERROR_MSG != enRtnVal )
    {
        BST_DBG_LOG2( "Heartbeat::InitRegedit ProcId=%d, TaskId=%d, Regist BST_PID_CIPHER_CODE error",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }
    enRtnVal                            = pcRegedit->Regist( this,
                                                             this,
                                                             BST_PID_HB_SERIAL_NUM,
                                                             BST_OS_SIZEOF( SERNO_HB_INFO_STRU ),
                                                             m_pstHbPktContent->pstSnInfoPkt );
    if ( BST_NO_ERROR_MSG != enRtnVal )
    {
        BST_DBG_LOG2( "Heartbeat::InitRegedit ProcId=%d, TaskId=%d, Regist BST_PID_HB_SERIAL_NUM error",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }
    enRtnVal                            = pcRegedit->Regist( this,
                                                             this,
                                                             BST_PID_CIPHER_MODE,
                                                             BST_OS_SIZEOF( m_pstHbPktContent->enSecMode ),
                                                             &m_pstHbPktContent->enSecMode );
    if ( BST_NO_ERROR_MSG != enRtnVal )
    {
        BST_DBG_LOG2( "Heartbeat::InitRegedit ProcId=%d, TaskId=%d, Regist BST_PID_CIPHER_MODE error",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }
    enRtnVal                            = pcRegedit->Regist( this,
                                                             this,
                                                             BST_PID_HB_TX_RETRY_MAX,
                                                             BST_OS_SIZEOF( BST_HB_RETRY_INFO ),
                                                             &m_stHbTxRetry.stRetryInfo );
    if ( BST_NO_ERROR_MSG != enRtnVal )
    {
        BST_DBG_LOG2( "Heartbeat::InitRegedit ProcId=%d, TaskId=%d, Regist BST_PID_HB_TX_RETRY_MAX error",
                      m_usProcId, m_usTaskId );
        return BST_FALSE;
    }
    return BST_TRUE;
}


BST_UINT8 *HeartBeat::SetCipherKey(
    BST_UINT8 *pucCipherKey,
    BST_UINT16 usLength )
{
    BST_DBG_LOG( "HeartBeat::SetCipherKey is not implemented" );
    return BST_NULL_PTR;
}


BST_UINT8 *HeartBeat::SetEncrypType(
    BST_UINT8 *pData,
    BST_UINT16 usLen )
{
    BST_APP_ENCRP_MODE_ENUM_UINT32      enEncryptType;
    BST_OS_MEMCPY( &enEncryptType, pData, BST_OS_SIZEOF( BST_APP_ENCRP_MODE_ENUM_UINT32 ) );

    BST_DBG_LOG( "HeartBeat::SetEncrypType is not implemented" );
    return BST_NULL_PTR;
}


BST_UINT8 *HeartBeat::SetCheckType(
    BST_UINT8 *pData,
    BST_UINT16 usLen )
{
    BST_APP_HB_CHECK_TYPE_ENUM_UINT32 enCheckType;
    BST_OS_MEMCPY( &enCheckType, pData, usLen );

    BST_DBG_LOG( "HeartBeat::SetCheckType is not implemented" );
    return BST_NULL_PTR;
}


BST_UINT8 *HeartBeat::SetFixedContent(
    BST_UINT8  *pucFixContent,
    BST_UINT16  usFixLength )
{
    BST_UINT8                          *pucRtnPtr;

    pucRtnPtr                           = BST_NULL_PTR;
    if ( BST_HB_MAX_CONFIG_BYTES < usFixLength )
    {
        return BST_NULL_PTR;
    }
    if ( BST_NULL_PTR == pucFixContent )
    {
        return BST_NULL_PTR;
    }
    if ( BST_NULL_PTR == m_pstHbPktContent->pFixedContent )
    {
        return BST_NULL_PTR;
    }

    BST_DBG_LOG3( "Heartbeat::SetFixedContent ProcId=%d, TaskId=%d, SetFixedContent length: %d",
                  m_usProcId, m_usTaskId, usFixLength );
    BST_LIB_StrFill( m_pstHbPktContent->pFixedContent, pucFixContent, usFixLength );
    m_pstHbPktContent->usHbPktLength   += usFixLength;
    pucRtnPtr                           = (BST_UINT8 *)m_pstHbPktContent->pFixedContent->pData;

    return pucRtnPtr;
}


BST_UINT8 *HeartBeat::SetRespContent(
    BST_UINT8  *pucRespContent,
    BST_UINT16  usRespLength )
{
    BST_UINT8                          *pucRtnPtr;

    pucRtnPtr                           = BST_NULL_PTR;

    if( BST_HB_MAX_CONFIG_BYTES < usRespLength )
    {
        return BST_NULL_PTR;
    }
    if( BST_NULL_PTR == pucRespContent )
    {
        return BST_NULL_PTR;
    }
    if( BST_NULL_PTR == m_pstHbPktContent->pRespContent )
    {
        return BST_NULL_PTR;
    }

    BST_DBG_LOG3( "Heartbeat::SetRespContent ProcId=%d, TaskId=%d, SetRespContent length: %d",
                  m_usProcId, m_usTaskId, usRespLength );
    BST_LIB_StrFill( m_pstHbPktContent->pRespContent, pucRespContent, usRespLength );

    pucRtnPtr                           = (BST_UINT8 *)m_pstHbPktContent->pRespContent->pData;

    return pucRtnPtr;
}


BST_UINT8 *HeartBeat::SetAppTxRetry(
    const BST_VOID *const pData,
    BST_UINT16            usLen )
{
    BST_HB_RETRY_INFO                   stHbRetryInfo;
    BST_UINT8                          *pucRtnPtr;

    pucRtnPtr                           = BST_NULL_PTR;

    if ( usLen != BST_OS_SIZEOF( BST_HB_RETRY_INFO ) )
    {
        return BST_NULL_PTR;
    }
    if ( BST_NULL_PTR == pData )
    {
        return BST_NULL_PTR;
    }

    BST_OS_MEMCPY( &stHbRetryInfo, pData, usLen );
    m_stHbTxRetry.stRetryInfo           = stHbRetryInfo;
    pucRtnPtr                           = (BST_UINT8 *)&m_stHbTxRetry.stRetryInfo;
#if ( BST_VER_TYPE == BST_DBG_VER )

    BST_DBG_LOG4( "Heartbeat::SetAppTxRetry ProcId=%d, TaskId=%d, Set retry info, ulIntervals:%u, ulMaxReTimes:%u",
                  m_usProcId, m_usTaskId, stHbRetryInfo.ulIntervals, stHbRetryInfo.ulMaxReTimes );

#endif
    return pucRtnPtr;
}
