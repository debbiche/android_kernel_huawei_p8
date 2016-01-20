
#ifndef __BST_APP_EXCGHTTP_H__
#define __BST_APP_EXCGHTTP_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "BST_Platform.h"
#include "BST_APP_HttpWbXmlDef.h"


#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack(4)
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define EXCG_WEB_XML_LEN                (10)

/*****************************************************************************
  3 类/结构定义
*****************************************************************************/

class BST_APP_CExcgHttp
{
BST_PUBLIC:
                    BST_APP_CExcgHttp(BST_VOID);
    virtual        ~BST_APP_CExcgHttp(BST_VOID);

    BST_BOOL        ConfigRqst(
        const BST_CHAR                     *strUserName,
        const BST_CHAR                     *strUserNameAndPass,
        const BST_CHAR                     *strDeviceId,
        const BST_EMAIL_HTTP_REQ_HEAD_STRU *pstHttpReqHead,
        const BST_EMAIL_WBXML_BODY_STRU    *pstWbxmlBody,
        const BST_APP_EXCGREQ_TYPE_ENUM_UINT8 enCmdType);

    BST_VOID        CreateRqstLine(
        const BST_CHAR                         *strUserName,
        const BST_CHAR                         *strDeviceId,
        const BST_APP_EXCGREQ_TYPE_ENUM_UINT8   enCmdType );

    BST_VOID        CreateRqstHeader(
        const BST_CHAR                         *strUserNameAndPass,
        const BST_CHAR                         *strBodyLen,
        const BST_EMAIL_HTTP_REQ_HEAD_STRU     *pstHttpReqHead,
        const BST_APP_EXCGREQ_TYPE_ENUM_UINT8   enCmdType);

    BST_UINT16      CreateRqstBody(
        const BST_EMAIL_WBXML_BODY_STRU        *pstWbxmlBody,
        const BST_APP_EXCGREQ_TYPE_ENUM_UINT8   enCmdType );

    BST_EMAIL_SERVER_STATE_ENUM_UINT8   ParseRspns(
        const BST_CHAR         *pcResponse,
        BST_UINT16              Length );

    BST_EMAIL_SERVER_STATE_ENUM_UINT8   ParseSyncRspns(
        const BST_CHAR         *pstRspStr,
        const BST_UINT16        usLen );

    BST_LIB_StrStru *GetRequest( BST_VOID );

    BST_UINT16      CreatePingWbxml(
        const BST_EMAIL_WBXML_BODY_STRU    *pstWbxmlBody );

    BST_UINT16      CreateSyncWbxml(
        const BST_EMAIL_WBXML_BODY_STRU    *pstWbxmlBody );

    BST_UINT16      CreateWbxmlHeader( BST_VOID );

    BST_INT32       HexStrToInt(
        const BST_UINT8        *pHexStr,
        const BST_UINT16        usLen );

BST_PRIVATE:
    BST_VOID        CreateWbxmlTag(
        const BST_CHAR          cTag,
        const BST_BOOL          bNoContent );

    BST_VOID        CreateWbxmlData(
        const BST_CHAR* strData,
        const BST_CHAR  cTag );
    BST_LIB_StrStru            *m_pstrRequest;
    BST_LIB_StrStru            *m_pstrWbxml;
    BST_UINT8                   m_enCmdType;
};
/*****************************************************************************
  4 OTHERS定义
*****************************************************************************/

#if (BST_OS_VER == BST_HISI_VOS)
#pragma pack()
#elif(BST_OS_VER == BST_WIN32)
#pragma pack(pop)
#endif

#endif