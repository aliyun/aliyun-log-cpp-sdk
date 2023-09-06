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
#include <curl/curl.h>
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
    static void Send(const std::string& httpMethod, const std::string& host, const int32_t port, const std::string& url, const std::string& queryString, const std::map<std::string, std::string>& header, const std::string& body, const int32_t timeout, HttpMessage& httpMessage, const curl_off_t maxspeed = 0);
    static void AsynSend(const std::string& httpMethod, const std::string& host, const int32_t port, const std::string& url, const std::string& queryString, const std::map<std::string, std::string>& header, const std::string& body, const int32_t timeout, RequestClosure* callBack);
    static std::string GetUrlSignature(const std::string& httpMethod, const std::string& operationType, std::map<std::string, std::string>& httpHeader, const std::map<std::string, std::string>& parameterList, const std::string& content, const std::string& signKey, const LOGSigType sigType = BASE64_SHA1_MD5);
};


class DnsCache
{
public:
    DnsCache(const int32_t dnsTTL=600) // 10 min
    {
        mUpdateTime = time(NULL);
        mDnsTTL = dnsTTL;
        pthread_mutex_init(& mDnsCacheLock, NULL);
    }
    ~DnsCache()
    {
        pthread_mutex_destroy(& mDnsCacheLock);
    }
    
    static DnsCache* GetInstance()
    {
        static DnsCache* dnsCache = new DnsCache();
        return dnsCache;
    }
    
    bool UpdateHostInDnsCache(const std::string& host, std::string& address)
    {
        int32_t currentTime = time(NULL);
        bool status = false;
        
        pthread_mutex_lock(& mDnsCacheLock);
        
        std::map<std::string, std::pair<std::string, int32_t> >::iterator itr = mDnsCacheData.find(host);
        if (itr==mDnsCacheData.end() || currentTime-(itr->second).second>=3)
        {
            if (ParseHost(host.c_str(), address))
            {
                status = true;
                mDnsCacheData[host] = std::make_pair(address, currentTime);
            }
            else
            {
                if (itr==mDnsCacheData.end())
                {
                    mDnsCacheData[host] = std::make_pair(host, currentTime);
                }
                else
                {
                    mDnsCacheData[host] = std::make_pair((itr->second).first, currentTime);
                }
            }
        }
        
        pthread_mutex_unlock(& mDnsCacheLock);
        return status;
    }
    
    bool GetIPFromDnsCache(const std::string& host, std::string& address)
    {
        bool found = false;
        
        pthread_mutex_lock(& mDnsCacheLock);
        
        if (! RemoveTimeOutDnsCache()) // not time out
        {
            std::map<std::string, std::pair<std::string, int32_t> >::iterator itr = mDnsCacheData.find(host);
            if (itr != mDnsCacheData.end())
            {
                address = (itr->second).first;
                found = true;
            } // else found=false
        }
        
        pthread_mutex_unlock(& mDnsCacheLock);
        
        if (! found) // time out or not found
            return UpdateHostInDnsCache(host, address);
        return found;
    }
    
private:
    static bool IsRawIp(const char *host) {
        unsigned char c, *p;
        p = (unsigned char *)host;
        while ((c = (*p++)) != '\0') {
            if ((c != '.') && (c < '0' || c > '9')) {
                return false;
            }
        }
        return true;
    }

    static bool ParseHost(const char* host, std::string& ip) {
        struct sockaddr_in  addr;

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;

        if (host && host[0]) {
            if (IsRawIp(host)) {
                if ((addr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
                    return false;
            } else {
                // FIXME: gethostbyname will block
                char    buffer[1024];
                struct  hostent h;
                struct  hostent *hp=NULL;
                int     rc;

                if (gethostbyname_r(host, &h, buffer, 1024, &hp, &rc) || hp == NULL)
                    return false;

                addr.sin_addr.s_addr = *((in_addr_t *) (hp->h_addr));
            }
        } else {
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
        }
        ip = inet_ntoa(addr.sin_addr);
        return true;
    }
    
    bool RemoveTimeOutDnsCache()
    {
        int32_t currentTime = time(NULL);
        bool isTimeOut = false;
        if (currentTime - mUpdateTime >= mDnsTTL)
        {
            isTimeOut = true;
            mDnsCacheData.clear();
            mUpdateTime = currentTime;
        }
        return isTimeOut;
    }
    
    int32_t mUpdateTime;
    int32_t mDnsTTL;
    pthread_mutex_t mDnsCacheLock;
    std::map<std::string, std::pair<std::string, int32_t> > mDnsCacheData;
};

}
#endif

