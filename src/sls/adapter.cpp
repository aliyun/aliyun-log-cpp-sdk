#include "adapter.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

#define NOMINMAX

#include <curl/curl.h>
#include <algorithm>

#if defined(_MSC_VER)
#pragma comment(lib,"ws2_32.lib")
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define SLS_MSVC_CLEANUP WSACleanup()
#else // only linux 
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#define SLS_MSVC_CLEANUP
#endif

using namespace std;

namespace aliyun_log_sdk_v6
{
extern const char * const DATE_FORMAT_RFC822  =  "%a, %d %b %Y %H:%M:%S GMT"; ///<RFC822 date formate, GMT time.
static const uint32_t MD5_BYTES=16;
extern const char* const LOG_HEADSIGNATURE_PREFIX = "LOG ";

extern const char* const LOGE_REQUEST_ERROR            = "RequestError";
extern const char* const LOGE_INVALID_HOST             = "InvalidHost";
extern const char* const LOGE_UNKNOWN_ERROR            = "UnknownError";
extern const char* const LOGE_NOT_IMPLEMENTED          = "NotImplemented";
extern const char* const LOGE_SERVER_BUSY              = "ServerBusy";
extern const char* const LOGE_INTERNAL_SERVER_ERROR    = "InternalServerError";
extern const char* const LOGE_RESPONSE_SIG_ERROR       = "ResponseSignatureError";
extern const char* const LOGE_PARAMETER_INVALID        = "ParameterInvalid";
extern const char* const LOGE_MISSING_PARAMETER        = "MissingParameter";
extern const char* const LOGE_INVALID_METHOD           = "InvalidMethod";
extern const char* const LOGE_BAD_RESPONSE             = "BadResponse";
extern const char* const LOGE_UNAUTHORIZED             = "Unauthorized";
extern const char* const LOGE_QUOTA_EXCEED             = "ExceedQuota";
extern const char* const LOGE_REQUEST_TIMEOUT          = "RequestTimeout";
extern const char* const LOGE_CLIENT_OPERATION_TIMEOUT = "ClientOpertaionTimeout";
extern const char* const LOGE_CLIENT_NETWORK_ERROR     = "ClientNetworkError";
extern const char* const LOGE_USER_NOT_EXIST           = "UserNotExist";
extern const char* const LOGE_CATEGORY_NOT_EXIST       = "CategoryNotExist";
extern const char* const LOGE_TOPIC_NOT_EXIST          = "TopicNotExist";
extern const char* const LOGE_POST_BODY_INVALID        = "PostBodyInvalid";
extern const char* const LOGE_INVALID_CONTENTTYPE      = "InvalidContentType";
extern const char* const LOGE_INVALID_CONTENLENGTH     = "InvalidContentLength";
extern const char* const LOGE_INVALID_APIVERSION       = "InvalidAPIVersion";
extern const char* const LOGE_PROJECT_NOT_EXIST        = "ProjectNotExist";
extern const char* const LOGE_LOGSTORE_NOT_EXIST       = "LogStoreNotExist";
extern const char* const LOGE_INVALID_ACCESSKEYID      = "InvalidAccessKeyId";
extern const char* const LOGE_SIGNATURE_NOT_MATCH      = "SignatureNotMatch";
extern const char* const LOGE_PROJECT_FORBIDDEN        = "ProjectForbidden";
extern const char* const LOGE_WRITE_QUOTA_EXCEED       = "WriteQuotaExceed";
extern const char* const LOGE_READ_QUOTA_EXCEED        = "ReadQuotaExceed";
extern const char* const LOGE_REQUEST_TIME_EXPIRED     = "RequestTimeExpired";
extern const char* const LOGE_INVALID_REQUEST_TIME     = "InvalidRequestTime";
extern const char* const LOGE_POST_BODY_TOO_LARGE      = "PostBodyTooLarge";
extern const char* const LOGE_INVALID_TIME_RANGE       = "InvalidTimeRange";
extern const char* const LOGE_INVALID_REVERSE          = "InvalidReverse";
extern const char* const LOGE_LOGSTORE_WITHOUT_SHARD   = "LogStoreWithoutShard";
extern const char* const LOGE_SHARD_WRITE_QUOTA_EXCEED = "ShardWriteQuotaExceed";
extern const char* const LOGE_SHARD_READ_QUOTA_EXCEED  = "ShardReadQuotaExceed";

extern const char* const LOGSTORES = "/logstores";
extern const char* const SHARDS = "/shards";
extern const char* const INDEX = "/index";
extern const char* const CONFIGS = "/configs";
extern const char* const MACHINES = "/machines";
extern const char* const MACHINEGROUPS = "/machinegroups";
extern const char* const ACLS = "/acls";

extern const char* const HTTP_GET = "GET";
extern const char* const HTTP_POST = "POST";
extern const char* const HTTP_PUT = "PUT";
extern const char* const HTTP_DELETE = "DELETE";

extern const char* const HOST = "Host";
extern const char* const DATE = "Date";
extern const char* const USER_AGENT = "User-Agent";
extern const char* const LOG_OLD_HEADER_PREFIX = "x-sls-";
extern const char* const LOG_HEADER_PREFIX = "x-log-";
extern const char* const ACS_HEADER_PREFIX = "x-acs-";
extern const char* const X_LOG_KEYPROVIDER = "x-log-keyprovider";
extern const char* const X_LOG_APIVERSION = "x-log-apiversion";
extern const char* const X_LOG_COMPRESSTYPE = "x-log-compresstype";
extern const char* const X_LOG_BODYRAWSIZE = "x-log-bodyrawsize";
extern const char* const X_LOG_SIGNATUREMETHOD = "x-log-signaturemethod";
extern const char* const X_ACS_SECURITY_TOKEN = "x-acs-security-token";
extern const char* const X_LOG_CURSOR = "x-log-cursor";
extern const char* const X_LOG_REQUEST_ID = "x-log-requestid";

extern const char* const X_LOG_PROGRESS = "x-log-progress";
extern const char* const X_LOG_COUNT = "x-log-count";
extern const char *const X_LOG_PROCESSED_ROWS = "x-log-processoed-rows";
extern const char *const X_LOG_ELASPED_MILLISECOND = "x-log-elapsed-millisecond";
extern const char *const X_LOG_CPU_SEC = "x-log-cpu-sec";
extern const char *const X_LOG_CPU_CORES = "x-log-cpu-cores";

extern const char* const HTTP_ACCEPT = "accept";
extern const char* const DEFLATE = "deflate";
extern const char* const HMAC_SHA1 = "hmac-sha1";
extern const char* const CONTENT_TYPE = "Content-Type";
extern const char* const CONTENT_LENGTH = "Content-Length";
extern const char* const CONTENT_MD5 = "Content-MD5";
extern const char* const AUTHORIZATION = "Authorization";
extern const char* const SIGNATURE = "Signature";
extern const char* const ACCEPT_ENCODING = "Accept-Encoding";
extern const char* const ENCONDING_GZIP = "gzip";
extern const char* const TYPE_LOG_PROTOBUF ="application/x-protobuf";
extern const char* const TYPE_LOG_JSON ="application/json";
extern const char* const LOGITEM_TIME_STAMP_LABEL="__time__";
extern const char* const LOGITEM_SOURCE_LABEL="__source__";
extern const char* const LOG_API_VERSION = "0.6.0";
extern const char* const LOGTAIL_USER_AGENT = "ali-log-logtail";
extern const char* const MD5_SHA1_SALT_KEYPROVIDER = "md5-sha1-salt";
extern const char* const LOG_TYPE_CURSOR = "cursor";
extern const char* const LOG_TYPE = "type";
extern const char* const LOGE_NOT_SUPPORTED_ACCEPT_CONTENT_TYPE = "InvalidAcceptContentType";
extern const char* const LOGE_NOT_SUPPORTED_ACCEPT_ENCODING = "InvalidAcceptEncoding";
extern const char* const LOGE_SHARD_NOT_EXIST = "ShardNotExist";
extern const char* const LOGE_INVALID_CURSOR = "InvalidCursor";
extern const char* const LOG_LZ4 = "lz4";

extern const char* const LOG_ERROR_CODE = "errorCode";
extern const char* const LOG_ERROR_MESSAGE = "errorMessage";

extern const char* const LOG_SHARD_STATUS_READWRITE= "readwrite";
extern const char* const LOG_SHARD_STATUS_READONLY = "readonly";

static std::string HexToString(const uint8_t md5[16])
{
    static const char* table = "0123456789ABCDEF";
    std::string ss(32, 'a');
    for (int i = 0; i < 16 ; ++i) 
    {    
        ss[i * 2] = table[md5[i] >> 4];
        ss[i * 2 + 1] = table[md5[i] & 0x0F];
    }    
    return ss;
}

std::string CodecTool::CalcMD5(const std::string& message)
{
    uint8_t md5[MD5_BYTES];
    DoMd5((const uint8_t*)message.data(), message.length(), md5);
    return HexToString(md5);
}
std::string CodecTool::CalcSHA1(const std::string& message, const std::string& key)
{
    HMAC hmac(reinterpret_cast<const uint8_t*>(key.data()), key.size()); 
    hmac.add(reinterpret_cast<const uint8_t*>(message.data()), message.size());
    return string(reinterpret_cast<const char*>(hmac.result()),SHA1_DIGEST_BYTES);
}
std::string CodecTool::Base64Enconde(const std::string& message)
{
    std::istringstream iss(message);
    std::ostringstream oss;
    Base64Encoding(iss, oss);
    return oss.str();
}
static constexpr size_t DATE_BUFFER_SIZE = 128;

std::string CodecTool::GetDateString(const std::string& dateFormat)
{
    std::time_t time = std::time({});
    char buffer[DATE_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    std::strftime(buffer, DATE_BUFFER_SIZE, dateFormat.c_str(), std::gmtime(&time));
    return string(buffer);
}
std::string CodecTool::GetDateString()
{
    return GetDateString(DATE_FORMAT_RFC822);
}

#if defined(_MSC_VER)
// for windows on msvc
time_t CodecTool::DecodeDateString(const std::string& dateString, const std::string& dateFormat)
{
    std::tm tm = {};
    std::istringstream input(dateString);
    input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
    input >> std::get_time(&tm, dateFormat.c_str());
    if (input.fail())
    {
        throw LOGException(LOGE_PARAMETER_INVALID, string("Invalid date string:") +
                                                       dateString + ",format:" + dateFormat);
    }
    return _mkgmtime(&tm);
}
#else
// for linux on gcc
time_t CodecTool::DecodeDateString(const std::string& dateString, const std::string& dateFormat)
{
    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_sec = -1;
    strptime(dateString.c_str(), dateFormat.c_str(), &t);
    if (t.tm_sec == -1)
    {
        throw LOGException(LOGE_PARAMETER_INVALID, string("Invalid date string:") + dateString + ",format:" + dateFormat);
    }
    struct timezone tz;
    struct timeval tv;
    gettimeofday(&tv, &tz);
    return mktime(&t) - tz.tz_minuteswest * 60;
}
#endif

bool CodecTool::StartWith(const std::string& input, const std::string& pattern)
{
    if (input.length() < pattern.length())
    {
        return false;
    }

    size_t i = 0;
    while (i < pattern.length()
        && input[i] == pattern[i])
    {
        i++;
    }

    return i == pattern.length();
}
string CodecTool::UrlEncode(const string& url)
{   
    char *szEncoded;
    szEncoded = curl_escape(url.c_str(), url.size());
    string tmpStr = szEncoded;
    curl_free(szEncoded);
    return tmpStr;
}

void LOGAdapter::GetQueryString(const map<string, string>& parameterList, string &queryString)
{
    queryString.clear();
    for(map<string, string>::const_iterator iter=parameterList.begin(); iter!=parameterList.end(); ++iter)
    {
        if (iter != parameterList.begin())
        {
            queryString.append("&");
        }
        queryString.append(iter->first);
        queryString.append("=");
        queryString.append(CodecTool::UrlEncode(iter->second));
    }
}
static size_t data_write_callback(char* buffer, size_t size, size_t nmemb, string* write_buf)
{
    size_t sizes = size * nmemb;

    if (buffer == NULL)
    {   
        return 0;
    }   
    write_buf->append(buffer, sizes);
    return sizes;
}

static size_t header_write_callback(char* buffer, size_t size, size_t nmemb, map<string,string>* write_buf)
{   
    size_t sizes = size * nmemb;

    if (buffer == NULL)
    {   
        return 0;
    }
    size_t colonIndex;
    for(colonIndex=1; colonIndex<sizes-2; colonIndex++)
    {   
        if(buffer[colonIndex] == ':')break;
    }
    if(colonIndex<sizes-2)
    {   
        unsigned long leftSpaceNum, rightSpaceNum;
        for(leftSpaceNum=0; leftSpaceNum<colonIndex-1; leftSpaceNum++)
        {   
            if(buffer[colonIndex-leftSpaceNum-1] != ' ')
                break;
        }
        for(rightSpaceNum=0; rightSpaceNum<sizes-colonIndex-1-2; rightSpaceNum++)
        {   
            if(buffer[colonIndex+rightSpaceNum+1] != ' ')
                break;
        }
        (*write_buf)[string(buffer,0,colonIndex-leftSpaceNum)] = string(buffer, colonIndex+1+rightSpaceNum, sizes-colonIndex-1-2-rightSpaceNum);
    }
    return sizes;
}

struct BodyTransfer
{
    size_t transfered = 0;
    const std::string &data;

public:
    BodyTransfer(const std::string &data) : data(data) {}

    size_t read(char *ptr, size_t wanted)
    {
        size_t remains = std::max((size_t)0, data.size() - transfered);
        if (remains == 0)
        {
            return 0;
        }
        size_t readBytes = std::min(remains, wanted);
        std::memcpy(ptr, data.c_str() + transfered, readBytes);
        transfered += readBytes;
        return readBytes;
    }
};

static size_t sendBody(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    BodyTransfer *state = static_cast<BodyTransfer *>(userdata);
    const size_t wanted = size * nmemb;
    return state->read(ptr, wanted);
}

void LOGAdapter::Send(const string& httpMethod, const string& host, const int32_t port, const string& url, const string& queryString, const map<string, string>& header, const string& body, const int32_t timeout, HttpMessage& httpMessage, const curl_off_t maxspeed) 
{
    /*
    for(map<string, string>::const_iterator iter = header.begin();iter != header.end();iter++)
    {
    }
    cout << endl;
    cout << "HOST:" << host << "\n" << endl;
    //cout << "URI:" << url << "\n" << endl;
    //cout << "BODY:" << body << "\n" << endl;
    //cout << "QUERYSTRING:" << queryString << "\n" << endl;
*/
    CURLcode res;
    std::string response;
    int64_t statusCode;
    map<std::string,std::string> responseHeader;
    struct curl_slist *headers = NULL;
    for(map<std::string, std::string>::const_iterator iter=header.begin(); iter!=header.end(); iter++)
    {
        headers = curl_slist_append(headers, (iter->first + ":" + iter->second).c_str());
    }
    string queryUrl = host + url;
    if(queryString.empty() == false)
        queryUrl += "?"+queryString;

    CURL* curl = curl_easy_init();
    
    if (curl)
    {
        if(headers)
        {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
        curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &responseHeader);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_write_callback);

        curl_easy_setopt(curl, CURLOPT_URL, queryUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, data_write_callback);

        curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CONNECT_TIMEOUT);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

        BodyTransfer transfer(body);
        curl_easy_setopt(curl, CURLOPT_READDATA, &transfer);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, sendBody);

        curl_easy_setopt(curl, CURLOPT_MAX_SEND_SPEED_LARGE, maxspeed);
        if(httpMethod   == HTTP_POST)
        {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            // curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, body.size());
            // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }
        else if(httpMethod == HTTP_DELETE)
        {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,HTTP_DELETE);
        }
        else if(httpMethod == HTTP_PUT){
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
            // curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, body.size());
            // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }

        res = curl_easy_perform(curl);
#define CURL_CLEAN_UP         curl_easy_cleanup(curl);\
        if(headers)\
        {\
            curl_slist_free_all(headers);\
        }

        switch(res)
        {
            case CURLE_OK:
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &statusCode);
            CURL_CLEAN_UP
                break;
            case CURLE_OPERATION_TIMEDOUT:
            CURL_CLEAN_UP
                throw LOGException(LOGE_REQUEST_TIMEOUT, "Request operation timeout.");
                break;
            case CURLE_COULDNT_CONNECT:
            CURL_CLEAN_UP
                throw LOGException(LOGE_CLIENT_NETWORK_ERROR, "Can not connect to server.");
                break;
            default:
            CURL_CLEAN_UP
                throw LOGException(LOGE_CLIENT_NETWORK_ERROR, std::string("Request operation failed.")+"CURL_ERROR_CODE:"+ToString(res));
                break;
        }
    }
    else
    {
        throw LOGException(LOGE_UNKNOWN_ERROR, "Initailizing request failed.");
    }

    httpMessage = HttpMessage(statusCode,responseHeader, response);
}


string LOGAdapter::GetUrlSignature(const string& httpMethod, const string& operationType, map<string, string>& httpHeader, const map<string, string>& parameterList, const string& content, const string& signKey, const LOGSigType sigType)
{
    string contentMd5;
    string signature;
    string osstream;
    if(! content.empty())
    {
        contentMd5=CodecTool::CalcMD5(content);
    }
    string contentType;
    {
        auto iter = httpHeader.find(CONTENT_TYPE);
        if (iter != httpHeader.end())
        {
            contentType = iter->second;
        }
    }

    std::map<string,string> endingMap;
    switch(sigType)
    {
    case BASE64_SHA1_MD5:
        osstream.append(httpMethod);
        osstream.append("\n");
        osstream.append(contentMd5);
        osstream.append("\n");
        osstream.append(contentType);
        osstream.append("\n");
        osstream.append(httpHeader[DATE]);
        osstream.append("\n");
        for(map<string,string>::const_iterator iter=httpHeader.begin(); iter!=httpHeader.end(); ++iter)
        {
            if(CodecTool::StartWith(iter->first, LOG_OLD_HEADER_PREFIX))
            {
                std::string key = iter->first;
                endingMap.insert(std::make_pair(key.replace(0, std::strlen(LOG_OLD_HEADER_PREFIX), LOG_HEADER_PREFIX), iter->second));
            }
            else if(CodecTool::StartWith(iter->first, LOG_HEADER_PREFIX) || CodecTool::StartWith(iter->first, ACS_HEADER_PREFIX))
            {
                endingMap.insert(std::make_pair(iter->first, iter->second));
            }
        }
        for(map<string,string>::const_iterator it=endingMap.begin(); it!=endingMap.end(); ++it)
        {
                osstream.append(it->first);
                osstream.append(":");
                osstream.append(it->second);
                osstream.append("\n");
        }
        osstream.append(operationType);
        if (parameterList.size() > 0) osstream.append("?");
        for(map<string,string>::const_iterator iter=parameterList.begin(); iter!=parameterList.end(); ++iter)
        {
            if(iter != parameterList.begin())
            {
                osstream.append("&");
            }
            osstream.append(iter->first);
            osstream.append("=");
            osstream.append(iter->second);
        }
        signature=CodecTool::Base64Enconde(CodecTool::CalcSHA1(osstream, signKey));
        break;
    default:
        throw LOGException(LOGE_NOT_IMPLEMENTED, "Signature Version does not support.");
    }
    return signature;
}

}

