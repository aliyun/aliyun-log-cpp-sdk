#include "adapter.h"

#include <iomanip>
#include <mutex>
#include <sstream>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <curl/curl.h>

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
extern const char* const LOG_HEADER_PREFIX = "x-log-";
extern const char* const ACS_HEADER_PREFIX = "x-acs-";
extern const char* const X_LOG_APIVERSION = "x-log-apiversion";
extern const char* const X_LOG_COMPRESSTYPE = "x-log-compresstype";
extern const char* const X_LOG_BODYRAWSIZE = "x-log-bodyrawsize";
extern const char* const X_LOG_SIGNATUREMETHOD = "x-log-signaturemethod";
extern const char* const X_ACS_SECURITY_TOKEN = "x-acs-security-token";
extern const char* const X_LOG_CURSOR = "x-log-cursor";
extern const char* const X_LOG_REQUEST_ID = "x-log-requestid";

extern const char* const X_LOG_PROGRESS = "x-log-progress";
extern const char* const X_LOG_COUNT = "x-log-count";
extern const char *const X_LOG_PROCESSED_ROWS = "x-log-processed-rows";
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

static void EnsureCurlGlobalInitialized()
{
    // Keep libcurl initialized for the process lifetime. The SDK cannot know
    // when other libcurl users in the process have finished their work.
    static std::once_flag initFlag;
    std::call_once(initFlag, []()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    });
}

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

std::string CodecTool::ToGmtTime(std::time_t &t, const std::string& format)
{
    std::stringstream date;
    std::tm tm;
#ifdef _MSC_VER
    ::gmtime_s(&tm, &t);
#else
    ::gmtime_r(&t, &tm);
#endif

#if defined(__GNUG__) && __GNUC__ < 5
    static const char wday_name[][4] = {
      "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
    static const char mon_name[][4] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    char tmbuff[26];
    snprintf(tmbuff, sizeof(tmbuff), "%.3s, %.2d %.3s %d %.2d:%.2d:%.2d",
        wday_name[tm.tm_wday], tm.tm_mday, mon_name[tm.tm_mon],
        1900 + tm.tm_year,
        tm.tm_hour, tm.tm_min, tm.tm_sec);
    date << tmbuff << " GMT";
#else
    date.imbue(std::locale::classic());
    date << std::put_time(&tm, format.c_str());
#endif
    return date.str();    
}


std::string CodecTool::GetDateString(const std::string& dateFormat)
{
    std::time_t t = std::time(nullptr);
    return ToGmtTime(t, dateFormat);
}
std::string CodecTool::GetDateString()
{
    return GetDateString(DATE_FORMAT_RFC822);
}

bool CodecTool::StartWith(const std::string& input, const std::string& pattern)
{
    if (input.length() < pattern.length())
    {
        return false;
    }
    return input.compare(0, pattern.length(), pattern) == 0;
}
string CodecTool::UrlEncode(const string& url)
{
    EnsureCurlGlobalInitialized();
    char* encoded = curl_escape(url.c_str(), static_cast<int>(url.size()));
    string result(encoded);
    curl_free(encoded);
    return result;
}

void LOGAdapter::GetQueryString(const map<string, string>& parameterList, string &queryString)
{
    queryString.clear();
    size_t querySize = parameterList.empty() ? 0 : parameterList.size() - 1;
    for (map<string, string>::const_iterator iter = parameterList.begin(); iter != parameterList.end(); ++iter)
    {
        querySize += iter->first.size() + 1 + iter->second.size();
    }
    queryString.reserve(querySize);
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
static size_t data_write_callback(char* buffer, size_t size, size_t nmemb, void* userdata)
{
    const size_t sizes = size * nmemb;
    if (buffer == NULL)
    {
        return 0;
    }
    static_cast<string*>(userdata)->append(buffer, sizes);
    return sizes;
}

static size_t header_write_callback(char* buffer, size_t size, size_t nmemb, void* userdata)
{
    const size_t sizes = size * nmemb;
    if (buffer == NULL)
    {
        return 0;
    }
    map<string, string>* headers = static_cast<map<string, string>*>(userdata);
    string line(buffer, sizes);
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
    {
        line.pop_back();
    }
    const size_t colonIndex = line.find(':');
    if (colonIndex != string::npos)
    {
        size_t valueBegin = colonIndex + 1;
        while (valueBegin < line.size() && (line[valueBegin] == ' ' || line[valueBegin] == '\t'))
        {
            ++valueBegin;
        }
        (*headers)[line.substr(0, colonIndex)] = line.substr(valueBegin);
    }
    return sizes;
}

namespace
{
class CurlRequest
{
public:
    CurlRequest() = default;

    bool Init()
    {
        EnsureCurlGlobalInitialized();
        mCurl = curl_easy_init();
        return mCurl != nullptr;
    }

    ~CurlRequest()
    {
        if (mCurl != nullptr)
        {
            curl_easy_cleanup(mCurl);
        }
        if (mHeaders != nullptr)
        {
            curl_slist_free_all(mHeaders);
        }
    }

    void Perform(const string& httpMethod,
                 const string& requestUrl,
                 const map<string, string>& header,
                 const string& body,
                 int32_t timeout,
                 int64_t maxspeed,
                 HttpMessage& httpMessage)
    {
        string response;
        map<string, string> responseHeaders;
        for (const auto& item : header)
        {
            mHeaders = curl_slist_append(mHeaders, (item.first + ":" + item.second).c_str());
        }
        curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, mHeaders);
        curl_easy_setopt(mCurl, CURLOPT_URL, requestUrl.c_str());
        curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, static_cast<void*>(&response));
        curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, data_write_callback);
        curl_easy_setopt(mCurl, CURLOPT_WRITEHEADER, static_cast<void*>(&responseHeaders));
        curl_easy_setopt(mCurl, CURLOPT_HEADERFUNCTION, header_write_callback);
        curl_easy_setopt(mCurl, CURLOPT_TCP_NODELAY, 1L);
        curl_easy_setopt(mCurl, CURLOPT_TIMEOUT, static_cast<long>(timeout));
        curl_easy_setopt(mCurl, CURLOPT_CONNECTTIMEOUT, static_cast<long>(CONNECT_TIMEOUT));
        curl_easy_setopt(mCurl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(mCurl, CURLOPT_MAX_SEND_SPEED_LARGE, static_cast<curl_off_t>(maxspeed));
        if (httpMethod == HTTP_POST)
        {
            curl_easy_setopt(mCurl, CURLOPT_POST, 1L);
            curl_easy_setopt(mCurl, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(body.size()));
            curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, body.c_str());
        }
        else if (httpMethod == HTTP_PUT)
        {
            curl_easy_setopt(mCurl, CURLOPT_CUSTOMREQUEST, HTTP_PUT);
            curl_easy_setopt(mCurl, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(body.size()));
            curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, body.c_str());
        }
        else if (httpMethod == HTTP_DELETE)
        {
            curl_easy_setopt(mCurl, CURLOPT_CUSTOMREQUEST, HTTP_DELETE);
        }

        const CURLcode result = curl_easy_perform(mCurl);

        long statusCode = 0;
        switch (result)
        {
            case CURLE_OK:
                curl_easy_getinfo(mCurl, CURLINFO_RESPONSE_CODE, &statusCode);
                break;
            case CURLE_OPERATION_TIMEDOUT:
                throw LOGException(LOGE_REQUEST_TIMEOUT, "Request operation timeout.");
            case CURLE_COULDNT_CONNECT:
                throw LOGException(LOGE_CLIENT_NETWORK_ERROR, "Can not connect to server.");
            default:
                throw LOGException(LOGE_CLIENT_NETWORK_ERROR,
                                   std::string("Request operation failed. CURL_ERROR_CODE:") +
                                       ToString(result) + ", CURL_ERROR_MESSAGE:" +
                                       curl_easy_strerror(result));
        }

        httpMessage = HttpMessage(static_cast<int32_t>(statusCode), responseHeaders, response);
    }

private:
    curl_slist* mHeaders = nullptr;
    CURL* mCurl = nullptr;
};
}

void LOGAdapter::Send(const string& httpMethod, const string& host, const int32_t port, const bool usingHttps, const string& url, const string& queryString, const map<string, string>& header, const string& body, const int32_t timeout, HttpMessage& httpMessage, const int64_t maxspeed)
{
    string queryUrl = usingHttps ? "https://" : "http://";
    queryUrl.reserve(queryUrl.size() + host.size() + url.size() + (queryString.empty() ? 0 : queryString.size() + 1));
    queryUrl.append(host);
    queryUrl.append(url);
    if (!queryString.empty())
    {
        queryUrl.append("?");
        queryUrl.append(queryString);
    }

    CurlRequest request;
    if (!request.Init())
    {
        throw LOGException(LOGE_UNKNOWN_ERROR, "Initializing request failed.");
    }
    request.Perform(httpMethod, queryUrl, header, body, timeout, maxspeed, httpMessage);
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
    map<string, string>::iterator iter = httpHeader.find(CONTENT_TYPE);
    if (iter != httpHeader.end())
    {
        contentType = iter->second;
    }
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
            if(CodecTool::StartWith(iter->first, LOG_HEADER_PREFIX) || CodecTool::StartWith(iter->first, ACS_HEADER_PREFIX))
            {
                osstream.append(iter->first);
                osstream.append(":");
                osstream.append(iter->second);
                osstream.append("\n");
            }
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
