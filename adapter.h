#ifndef __LOGV6_ADAPTER_H__
#define __LOGV6_ADAPTER_H__
#include <string>
#include <vector>
#include <map>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>      // inet_addr
#include <sys/ioctl.h>
#include "common.h"
#include "client.h"
#include "RestfulApiCommon.h"
#include <ctime>
namespace aliyun_log_sdk_v6
{
typedef enum{BASE64_SHA1_MD5} LOGSigType;
const uint32_t CONNECT_TIMEOUT = 5; //second

class RequestClosure;

class CodecTool
{
public:
    static std::string CalcMD5(const std::string& message);
    static std::string CalcSHA1(const std::string& message, const std::string& key);
    static std::string Base64Enconde(const std::string& message);

    static std::string GetDateString(const std::string& dateFormat);
    static std::string GetDateString();
    static time_t DecodeDateString(const std::string dateString, const std::string& dateFormat=DATE_FORMAT_RFC822);
    
    static bool StartWith(const std::string& input, const std::string& pattern);
    static std::string UrlEncode(const std::string& url);
private:
    static std::string ToGmtTime(std::time_t &t, const std::string& format);
};

class LOGAdapter
{
public:
    static void GetQueryString(const std::map<std::string, std::string>& parameterList, std::string &queryString);
    static void Send(const std::string& httpMethod, const std::string& host, const int32_t port, const std::string& url, const std::string& queryString, const std::map<std::string, std::string>& header, const std::string& body, const int32_t timeout, HttpMessage& httpMessage, const int64_t maxspeed = 0);
    static void AsynSend(const std::string& httpMethod, const std::string& host, const int32_t port, const std::string& url, const std::string& queryString, const std::map<std::string, std::string>& header, const std::string& body, const int32_t timeout, RequestClosure* callBack);
    static std::string GetUrlSignature(const std::string& httpMethod, const std::string& operationType, std::map<std::string, std::string>& httpHeader, const std::map<std::string, std::string>& parameterList, const std::string& content, const std::string& signKey, const LOGSigType sigType = BASE64_SHA1_MD5);
};


}
#endif

