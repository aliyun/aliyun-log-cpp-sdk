#ifndef __LOGV6_ADAPTER_H__
#define __LOGV6_ADAPTER_H__
#include <string>
#include <vector>
#include <map>

#include "common.h"
#include "client.h"
#include "RestfulApiCommon.h"
#include <mutex>

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
    static time_t DecodeDateString(const std::string& dateString, const std::string& dateFormat=DATE_FORMAT_RFC822);
    
    static bool StartWith(const std::string& input, const std::string& pattern);
    static std::string UrlEncode(const std::string& url);
};


class LOGAdapter
{
public:
    using curl_off_t = long long;
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
    }
    ~DnsCache()
    {
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

        const std::lock_guard<std::mutex> lock(mMutex);
        
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
        
        return status;
    }
    
    bool GetIPFromDnsCache(const std::string& host, std::string& address)
    {
        bool found = false;
        
        std::unique_lock<std::mutex> lock(mMutex);
        
        if (! RemoveTimeOutDnsCache()) // not time out
        {
            std::map<std::string, std::pair<std::string, int32_t> >::iterator itr = mDnsCacheData.find(host);
            if (itr != mDnsCacheData.end())
            {
                address = (itr->second).first;
                found = true;
            } // else found=false
        }

        lock.unlock();        
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
    public:
    static bool ParseHost(const char* host, std::string& ip);
    
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
    std::mutex mMutex;
    std::map<std::string, std::pair<std::string, int32_t> > mDnsCacheData;
};

}
#endif

