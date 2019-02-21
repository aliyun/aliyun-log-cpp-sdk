#ifndef __LOGV6_RESTFULAPI_COMMON_H__
#define __LOGV6_RESTFULAPI_COMMON_H__
#include <string>
#include <vector>
#include <map>
#include "stdint.h"

namespace aliyun_log_sdk_v6
{
extern const char * const DATE_FORMAT_RFC822;// "%a, %d %b %Y %H:%M:%S GMT" ///<RFC822 date formate, GMT time.
//typedef enum {REQUEST_GET= 1, REQUEST_POST } LOG_Request_Mode;///<Http Request Method.
typedef enum {CURSOR_MODE_BEGIN = 1, CURSOR_MODE_END} LOG_Cursor_Mode;
const uint32_t LOG_REQUEST_TIMEOUT = 20; //second
extern const char* const LOG_HEADSIGNATURE_PREFIX;///< "";

extern const char* const LOGE_REQUEST_ERROR;//= "RequestError";
extern const char* const LOGE_INVALID_HOST;//= "InvalidHost"
extern const char* const LOGE_UNKNOWN_ERROR;//= "UnknownError";
extern const char* const LOGE_NOT_IMPLEMENTED;//= "NotImplemented";
extern const char* const LOGE_SERVER_BUSY;//= "ServerBusy";
extern const char* const LOGE_INTERNAL_SERVER_ERROR;//= "InternalServerError";
extern const char* const LOGE_RESPONSE_SIG_ERROR;//= "ResponseSignatureError";
extern const char* const LOGE_PARAMETER_INVALID;//= "ParameterInvalid";
extern const char* const LOGE_MISSING_PARAMETER;//= "MissingParameter";
extern const char* const LOGE_INVALID_METHOD;//= "InvalidMethod";
extern const char* const LOGE_INVALID_CONTENTTYPE;//= "InvalidContentType";
extern const char* const LOGE_INVALID_CONTENTLENGTH;//= "InvalidContentLength";
extern const char* const LOGE_BAD_RESPONSE;//= "BadResponse";
extern const char* const LOGE_UNAUTHORIZED;//= "Unauthorized";
extern const char* const LOGE_QUOTA_EXCEED;//= "ExceedQuota";
extern const char* const LOGE_REQUEST_TIMEOUT;//= "RequestTimeout";
extern const char* const LOGE_CLIENT_OPERATION_TIMEOUT;//= "ClientOpertaionTimeout";
extern const char* const LOGE_CLIENT_NETWORK_ERROR;//= "ClientNetworkError";
extern const char* const LOGE_USER_NOT_EXIST;//= "UserNotExist";
extern const char* const LOGE_CATEGORY_NOT_EXIST;//= "CategoryNotExist";
extern const char* const LOGE_TOPIC_NOT_EXIST;//= "TopicNotExist";
extern const char* const LOGE_POST_BODY_INVALID;//= "PostBodyInvalid";
extern const char* const LOGE_INVALID_HOST;//= "InvalidHost";
extern const char* const LOGE_INVALID_APIVERSION;//="InvalidAPIVersion";
extern const char* const LOGE_PROJECT_NOT_EXIST;//="ProjectNotExist";
extern const char* const LOGE_LOGSTORE_NOT_EXIST;//="LogStoreNotExist";
extern const char* const LOGE_INVALID_ACCESSKEYID;//="InvalidAccessKeyId";
extern const char* const LOGE_SIGNATURE_NOT_MATCH;//="SignatureNotMatch";
extern const char* const LOGE_PROJECT_FORBIDDEN;//="ProjectForbidden";
extern const char* const LOGE_WRITE_QUOTA_EXCEED;//="WriteQuotaExceed";
extern const char* const LOGE_READ_QUOTA_EXCEED;//="ReadQuotaExceed";
extern const char* const LOGE_REQUEST_TIME_EXPIRED;//="RequestTimeExpired";
extern const char* const LOGE_INVALID_REQUEST_TIME;//="InvalidRequestTime";
extern const char* const LOGE_POST_BODY_TOO_LARGE;//="PostBodyTooLarge";
extern const char* const LOGE_INVALID_TIME_RANGE;//="InvalidTimeRange";
extern const char* const LOGE_INVALID_REVERSE;//="InvalidReverse";
extern const char* const LOGE_LOGSTORE_WITHOUT_SHARD;//="LogStoreWithoutShard";

extern const char* const LOGSTORES;//= "/logstores"
extern const char* const SHARDS;//= "/shards"
extern const char* const INDEX;//= "/index"
extern const char* const CONFIGS;//= "/configs"
extern const char* const MACHINES;//= "/machines"
extern const char* const MACHINEGROUPS;//= "/machinegroups"
extern const char* const ACLS;//= "/acls"

extern const char* const HTTP_GET;//= "GET";
extern const char* const HTTP_POST;//= "POST";
extern const char* const HTTP_PUT;//= "PUT";
extern const char* const HTTP_DELETE;//= "DELETE";

extern const char* const HOST;//= "Host";
extern const char* const DATE;//= "Date";
extern const char* const USER_AGENT;//= "User-Agent";
extern const char* const LOG_HEADER_PREFIX;//= "x-log-";
extern const char* const LOG_OLD_HEADER_PREFIX;//= "x-sls-";
extern const char* const X_LOG_KEYPROVIDER;// = "x-log-keyprovider";
extern const char* const X_LOG_APIVERSION;// = "x-log-apiversion";
extern const char* const X_LOG_COMPRESSTYPE;// = "x-log-compresstype";
extern const char* const X_LOG_BODYRAWSIZE;// = "x-log-bodyrawsize";
extern const char* const X_LOG_SIGNATUREMETHOD;// = "x-log-signaturemethod";
extern const char* const X_ACS_SECURITY_TOKEN;// = "x-acs-security-token";
extern const char* const X_LOG_CURSOR;// = "cursor";
extern const char* const X_LOG_REQUEST_ID;// = "x-log-requestid";

extern const char* const X_LOG_PROGRESS;// = "x-log-progress";
extern const char* const X_LOG_COUNT;// = "x-log-count";

extern const char* const HTTP_ACCEPT;// = "accept";
extern const char* const DEFLATE;//= "deflate";
extern const char* const HMAC_SHA1;//= "hmac-sha1";
extern const char* const CONTENT_LENGTH;//= "Content-Length";
extern const char* const CONTENT_TYPE;//= "Content-Type";
extern const char* const CONTENT_MD5;//= "Content-MD5";
extern const char* const AUTHORIZATION;//= "Authorization";
extern const char* const SIGNATURE;//= "Signature";
extern const char* const ACCEPT_ENCODING;// = "Accept-Encoding";
extern const char* const ENCONDING_GZIP;// = "gzip";
extern const char* const TYPE_LOG_PROTOBUF;//="application/x-protobuf";
extern const char* const TYPE_LOG_JSON;//="application/json";
extern const char* const LOGITEM_TIME_STAMP_LABEL;//="__time__";
extern const char* const LOGITEM_SOURCE_LABEL;//="__source__";
extern const char* const LOG_API_VERSION; // = "0.4.0";
extern const char* const LOGTAIL_USER_AGENT; // = "ali-sls-logtail";
extern const char* const MD5_SHA1_SALT_KEYPROVIDER; // = "md5-shal-salt";
extern const char* const LOG_TYPE_CURSOR;// = "cursor";
extern const char* const LOG_TYPE;// = "type";
extern const char* const LOGE_NOT_SUPPORTED_ACCEPT_CONTENT_TYPE;
extern const char* const LOGE_NOT_SUPPORTED_ACCEPT_ENCODING;
extern const char* const LOGE_SHARD_NOT_EXIST;
extern const char* const LOGE_INVALID_CURSOR;
extern const char* const LOGE_SHARD_WRITE_QUOTA_EXCEED;
extern const char* const LOGE_SHARD_READ_QUOTA_EXCEED;
extern const char* const LOG_LZ4;//= "lz4";

extern const char* const LOG_ERROR_CODE;//= "errorCode";
extern const char* const LOG_ERROR_MESSAGE;//= "errorMessage";

extern const char* const LOG_SHARD_STATUS_READWRITE;// "readwrite";
extern const char* const LOG_SHARD_STATUS_READONLY ;// "readonly";
/**
 *This class enumerates LOG ErrorCode with HTTP status.
 */
class Status
{
private:
    Status(int32_t para_statusCode, const std::string& para_errorCode)
        :statusCode(para_statusCode), errorCode(para_errorCode)
    {
    }
public:
    const int32_t statusCode;///<Normally this is the HTTP status code.
    const std::string errorCode;///<This is the LOG error code.

    bool IsSuccess() const
    {
        return statusCode == 200;
    }
    bool IsTimeOutError() const
    {
        return statusCode == 1001;
    }
    bool IsServerError() const
    {
        return statusCode == 500;
    }
    bool IsNotFound() const
    {
        return statusCode == 404;
    }

    static Status Ok()
    {
        return Status(200 /* HTTP_OK */, "");
    }
    static Status NotSupportedAcceptEncoding()
    {
        return Status(400, LOGE_NOT_SUPPORTED_ACCEPT_ENCODING);
    }
    static Status NotSupportedAcceptContentType()
    {
        return Status(400, LOGE_NOT_SUPPORTED_ACCEPT_CONTENT_TYPE);
    }
    static Status LogStoreWithoutShard()
    {
        return Status(400, LOGE_LOGSTORE_WITHOUT_SHARD);
    }
    static Status ShardNotExist()
    {
        return Status(404, LOGE_SHARD_NOT_EXIST);
    }
    static Status InvalidCursor()
    {
        return Status(400, LOGE_INVALID_CURSOR);
    }
/** Function that get UnknownError definition. 
* @param void None. 
* @return Get UnknownError error code with http status code.
*/
    static Status UnknownError()
    {
        return Status(500  /* HTTP_INTERNAL_SERVER_ERROR */,  LOGE_UNKNOWN_ERROR);
    }
/** Function that get InternalServerError definition. 
* @param void None. 
* @return Get InternalServerError error code with http status code.
*/
    static Status InternalServerError()
    {
        return Status(500  /* HTTP_INTERNAL_SERVER_ERROR */,  LOGE_INTERNAL_SERVER_ERROR); 
    }
/** Function that get NotImplemented definition. 
* @param void None. 
* @return Get NotImptemented error code with http status code.
*/
    static Status NotImplemented()
    {
        return Status(501  /* HTTP_NOT_IMPLEMENTED */,        LOGE_NOT_IMPLEMENTED); 
    }
/** Function that get PostBodyInvalid definition. 
* @param void None. 
* @return Get PostBodyInvalid error code with http status code.
*/
    static Status PostBodyInvalid()
    {
        return Status(400 /* HTTP_BAD_REQUEST */,             LOGE_POST_BODY_INVALID);
    }
/** Function that get ParameterInvalid definition. 
* @param void None. 
* @return Get ParameterInvalid error code with http status code.
*/
    static Status ParameterInvalid()
    {
        return Status(400  /* HTTP_BAD_REQUEST */,            LOGE_PARAMETER_INVALID); 
    }
/** Function that get MissingParameter definition. 
* @param void None. 
* @return Get MissingParameter error code with http status code.
*/
    static Status MissingParameter()
    {
        return Status(400  /* HTTP_BAD_REQUEST */,            LOGE_MISSING_PARAMETER); 
    }
/** Function that get InvalidContentType definition. 
* @param void None. 
* @return Get InvalidContentType error code with http status code.
*/
    static Status InvalidContentType()
    {   
        return Status(400 /* HTTP_BAD_REQUEST */,             LOGE_INVALID_CONTENTTYPE);
    }   
/** Function that get Invalid definition. 
* @param void None. 
* @return Get InvalidMethod error code with http status code.
*/
    static Status InvalidMethod()
    {
        return Status(405  /* METHOD_NOT_ALLOWED */,           LOGE_INVALID_METHOD); 
    }
/** Function that get ServerBusy definition. 
* @param void None. 
* @return Get ServerBusy error code with http status code.
*/
    static Status ServerBusy()
    {
        return Status(500  /* HTTP_INTERNAL_SERVER_ERROR */,   LOGE_SERVER_BUSY); 
    }
/** Function that get UnAuthorized definition. 
* @param void None. 
* @return Get UnAuthorized error code with http status code.
*/
    static Status UnAuthorized()
    {
        return Status(401  /* HTTP_UNAUTHORIZED */,            LOGE_UNAUTHORIZED); 
    }
/** Function that get ExceedQuota definition. 
* @param void None. 
* @return Get ExceedQuota error code with http status code.
*/
    static Status QuotaExceed()
    {
        return Status(403  /* HTTP_FORBIDDEN */,               LOGE_QUOTA_EXCEED); 
    }
/** Function that get UserNotExist definition. 
* @param void None. 
* @return Get UserNotExist code with http status code.
*/
    static Status UserNotExist()
    {
        return Status(404  /* HTTP_NOT_FOUND */,              LOGE_USER_NOT_EXIST); 
    }
/** Function that get ProjectNotExist definition.                                                                                                                      
 * * @param void None. 
 * * @return Get LogStoreNotExist code with http status code.
 * */
    static Status ProjectNotExist()
    {   
        return Status(404  /* HTTP_NOT_FOUND */,              LOGE_PROJECT_NOT_EXIST); 
    }
/** Function that get LogStoreNotExist definition.                                                                                                                      
 * * @param void None. 
 * * @return Get LogStoreNotExist code with http status code.
 * */
    static Status LogStoreNotExist()
    {   
        return Status(404  /* HTTP_NOT_FOUND */,              LOGE_LOGSTORE_NOT_EXIST); 
    }

/** Function that get CategoryNotExist definition. 
* @param void None. 
* @return Get CategoryNotExist code with http status code.
*/
    static Status CategoryNotExist()
    {
        return Status(404  /* HTTP_NOT_FOUND */,              LOGE_CATEGORY_NOT_EXIST); 
    }
/** Function that get TopicNotExist definition. 
* @param void None. 
* @return Get TopicNotExist error code with http status code.
*/
    static Status TopicNotExist()
    {
        return Status(404  /* HTTP_NOT_FOUND */,              LOGE_TOPIC_NOT_EXIST); 
    }
/** Function that get ClientOperationTimeout definition. 
* @param void None. 
* @return Get ClientOperationTimeout error code with http status code.
*/
    static Status ClientOperationTimeout()
    {
        return Status(1001 /* CLIENT OPERATION TIMEOUOT */,   LOGE_CLIENT_OPERATION_TIMEOUT); 
    }
/** Function that get ProjectForbidden definition. 
* @param void None. 
* @return Get ProjectForbidden code with http status code.
*/
    static Status ProjectForbidden()
    {
        return Status(403  /* HTTP_FORBIDDEN */,              LOGE_PROJECT_FORBIDDEN); 
    }
    static Status ShardWriteQuotaExceed()
    {
        return Status(403  /* HTTP_FORBIDDEN */,              LOGE_SHARD_WRITE_QUOTA_EXCEED); 
    }
    static Status ShardReadQuotaExceed()
    {
        return Status(403  /* HTTP_FORBIDDEN */,              LOGE_SHARD_READ_QUOTA_EXCEED); 
    }
/** Function that get WriteQuotaExceed definition. 
* @param void None. 
* @return Get WriteQuotaExceed code with http status code.
*/
    static Status WriteQuotaExceed()
    {
        return Status(403  /* HTTP_FORBIDDEN */,              LOGE_WRITE_QUOTA_EXCEED); 
    }
/** Function that get ReadQuotaExceed definition. 
* @param void None. 
* @return Get ReadQuotaExceed code with http status code.
*/
    static Status ReadQuotaExceed()
    {
        return Status(403  /* HTTP_FORBIDDEN */,              LOGE_READ_QUOTA_EXCEED); 
    }
/** Function that get InvalidHost definition. 
* @param void None. 
* @return Get InvalidHost code with http status code.
*/
    static Status InvalidHost()
    {
        return Status(400  /* HTTP_BAD_REQUEST */,            LOGE_INVALID_HOST); 
    }
/** Function that get InvalidContentLength definition. 
* @param void None. 
* @return Get InvalidContentLength code with http status code.
*/
    static Status InvalidContentLength()
    {
        return Status(400  /* HTTP_BAD_REQUEST */,            LOGE_INVALID_CONTENTLENGTH); 
    }
/** Function that get InvalidAPIVersion definition. 
* @param void None. 
* @return Get InvalidAPIVersion code with http status code.
*/
    static Status InvalidAPIVersion()
    {
        return Status(400  /* HTTP_BAD_REQUEST */,            LOGE_INVALID_APIVERSION); 
    }
/** Function that get InvalidAccessKeyId definition. 
* @param void None. 
* @return Get InvalidAccessKeyId code with http status code.
*/
    static Status InvalidAccessKeyId()
    {
        return Status(401  /* HTTP_UNAUTHORIZED */,           LOGE_INVALID_ACCESSKEYID); 
    }
/** Function that get RequestTimeExpired definition. 
* @param void None. 
* @return Get RequestTimeExpired code with http status code.
*/
    static Status RequestTimeExpired()
    {
        return Status(400  /* HTTP_BAD_REQUEST */,            LOGE_REQUEST_TIME_EXPIRED); 
    }
/** Function that get InvalidRequestTime definition. 
* @param void None. 
* @return Get InvalidRequestTime code with http status code.
*/
    static Status InvalidRequestTime()
    {
        return Status(400  /* HTTP_BAD_REQUEST */,            LOGE_INVALID_REQUEST_TIME); 
    }
/** Function that get PostBodyTooLarge definition. 
* @param void None. 
* @return Get PostBodyTooLarge code with http status code.
*/
    static Status PostBodyTooLarge()
    {
        return Status(413  /* HTTP_REQUEST_ENTITY_TOO_LARGE */, LOGE_POST_BODY_TOO_LARGE); 
    }
/** Function that get InvalidTimeRange definition. 
* @param void None. 
* @return Get InvalidTimeRange code with http status code.
*/
    static Status InvalidTimeRange()
    {
        return Status(400  /* HTTP_BAD_REQUEST */,              LOGE_INVALID_TIME_RANGE);
    }
/** Function that get InvalidReverse definition. 
* @param void None. 
* @return Get InvalidReverse code with http status code.
*/
    static Status InvalidReverse()
    {
        return Status(400  /* HTTP_BAD_REQUEST */,              LOGE_INVALID_REVERSE);
    }
/** Function that get SignatureNotMatch definition. 
* @param void None. 
* @return Get SignatureNotMatch code with http status code.
*/
    static Status SignatureNotMatch()
    {
        return Status(401  /* HTTP_UNAUTHORIZED */,             LOGE_SIGNATURE_NOT_MATCH);
    }

};

/** 
 * HTTP message structure includes three parts: http status code, http header, and http content.
 */
typedef struct HttpMessage
{
    int32_t statusCode;///<Http status code
    std::map<std::string,std::string> header;///<Only contains the header lines which have key:value pair
    std::string content;///<Http content
/** Constructor with no parameter.
* @param void None. 
* @return The objcect pointer.
*/
    HttpMessage()
    {
    }
/** Constructor with header and content. 
* @param para_header A map structure which contains the key:value pair of http header lines. 
   Those header lines which do not contains key:value pair are not included.
* @param para_content A string which contains the content of http request.
* @return The objcect pointer.
*/
    HttpMessage(const std::map<std::string, std::string>& para_header, const std::string& para_content)
        :header(para_header), content(para_content)
    {
    }
/** Constructor with status code, header and content. 
* @param para_statusCode Http status code. 
* @param para_header A map structure which contains the key:value pair of http header lines. 
   Those header lines which do not contains key:value pair are not included.
* @param para_content A string which contains the http content of http content.
* @return The objcect pointer.
*/
    HttpMessage(const int32_t para_statusCode, const std::map<std::string, std::string>& para_header, const std::string& para_content)
        :statusCode(para_statusCode), header(para_header), content(para_content)
    {
    }

    void FillResponse(const int32_t para_statusCode, const std::string& para_content)
    {
        statusCode = para_statusCode;
        content = para_content;
    }
}HttpMessage;

/**
 *LOG Exception includes an error code and a detail message.
 */
class LOGException : public std::exception
{
/** Constructor with error code and message. 
* @param errorCode LOG error code. 
* @param message Detailed information for the exception.
* @return The objcect pointer.
*/
public:
    LOGException()
    {
        mHttpCode = 0;
    }
    LOGException(const std::string& errorCode, const std::string& message, const std::string& requestId="", const int32_t httpCode=0)
        :mErrorCode(errorCode),mMessage(message),mRequestId(requestId),mHttpCode(httpCode)
    {
    }
    ~LOGException() throw()
    {
    }
    /** Function that return error code. 
    * @param void None. 
    * @return Error code string.
    */
    std::string GetErrorCode(void) const
    {
        return mErrorCode;
    }
    /** Function that return error message. 
    * @param void None. 
    * @return Error message string.
    */
    std::string GetMessage(void) const
    {
        return mMessage;
    }
    /** Function that return request id. 
    * @param void None. 
    * @return request id string.
    */
    std::string GetRequestId(void) const
    {
        return mRequestId;
    }
    /** Function that return http response code. 
    * @param void None.
    * @return http response code int32_t, if client error, return 0.
    */
    int32_t GetHttpCode(void) const
    {
        return mHttpCode;
    }
private:
    std::string mErrorCode;
    std::string mMessage;
    std::string mRequestId;
    int32_t mHttpCode;
};

}
#endif
