#include "client.h"
#include "adapter.h"
#include "common.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <iostream>
#include <curl/curl.h>
#include <unistd.h> 
#include <netdb.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/ioctl.h> 
#include <net/if.h> 
#include <net/if_arp.h>
#include <string>
#include <cstdlib>

#include <iostream>
#include <cstdio>

#define ETH_NAME "eth0"

using namespace std;
using namespace sls_logs;
using namespace rapidjson;
extern const char* const aliyun_log_sdk_v6::LOG_SDK_IDENTIFICATION = "sls-cpp-sdk v0.6";
static string GetHostIpByHostName()
{
    char hostname[255];
    gethostname(hostname, 255);
    struct hostent* entry = gethostbyname(hostname);
    if (entry == NULL)
    {
        return string();
    }
    struct in_addr* addr = (struct in_addr*)entry -> h_addr_list[0];
    if (addr == NULL)
    {
        return string();
    }
    char* ipaddr = inet_ntoa(*addr);
    if (ipaddr == NULL)
    {
        return string();
    }
    return string(ipaddr);
}


static string GetHostIpByETHName()
{
    int   sock;    
    struct   sockaddr_in   sin;    
    struct   ifreq   ifr;    

    sock =  socket(AF_INET,   SOCK_DGRAM,   0);    
    if (sock == -1) 
    { 
       return string();
    }    

    // use eth0 as the default ETH name 
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);    
    ifr.ifr_name[IFNAMSIZ-1] = 0;    

    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)    
    {    
        close(sock);//added by gaolei 13-10-09
        return string();
    }          

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));    

    char* ipaddr = inet_ntoa(sin.sin_addr);
    close(sock); //added by gaolei 13-10-09
    if ( ipaddr == NULL)
    {
        return string();
    }
    return string(ipaddr);
}

static string GetHostIp()
{
    string ip = GetHostIpByHostName();
    if (ip.empty() || ip.find("127.0.0.1") != string::npos)
    {
        return GetHostIpByETHName();
    }
    return ip;
}


namespace aliyun_log_sdk_v6
{

/************************ common method ***********************/
/************************ json method *************************/
void ExtractJsonResult(const string& response, rapidjson::Document& document)
{
    document.Parse(response.c_str());
    if (document.HasParseError())
    {
        throw JsonException("ParseException", "Fail to parse from json string");
    }
}

void JsonMemberCheck(const rapidjson::Value& value, const char* name)
{
    if (!value.IsObject())
    {
        throw JsonException("InvalidObjectException", "response is not valid JSON object");
    }
    if (!value.HasMember(name))
    {
        throw JsonException("NoMemberException", string("Member ") + name + " does not exist");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, int32_t& number)
{
    JsonMemberCheck(value, name);
    if (value[name].IsInt())
    {
        number = value[name].GetInt();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not int type");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, uint32_t& number)
{
    JsonMemberCheck(value, name);
    if (value[name].IsUint())
    {
        number = value[name].GetUint();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not uint type");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, int64_t& number)
{
    JsonMemberCheck(value, name);
    if (value[name].IsInt64())
    {
        number = value[name].GetInt64();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not int type");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, uint64_t& number)
{
    JsonMemberCheck(value, name);
    if (value[name].IsUint64())
    {
        number = value[name].GetUint64();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not uint type");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, bool& boolean)
{
    JsonMemberCheck(value, name);
    if (value[name].IsBool())
    {
        boolean = value[name].GetBool();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not boolean type");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, string& dst)
{
    JsonMemberCheck(value, name);
    if (value[name].IsString())
    {
        dst = value[name].GetString();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not string type");
    }
}

const rapidjson::Value& GetJsonValue(const rapidjson::Value& value, const char* name)
{
    JsonMemberCheck(value, name);
    if (value[name].IsObject() || value[name].IsArray())
    {
        return value[name];
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not json value type");
    }
}


static void ErrorCheck(const string& response, const string& requestId, const int32_t httpCode)
{
    rapidjson::Document document;
    try
    {
        ExtractJsonResult(response, document);
            
        string errorCode;
        ExtractJsonResult(document, LOG_ERROR_CODE, errorCode);

        string errorMessage;
        ExtractJsonResult(document, LOG_ERROR_MESSAGE, errorMessage);

        throw LOGException(errorCode, errorMessage, requestId, httpCode);
    }
    catch (JsonException& e)
    {
        if (httpCode == 500)
        {
            throw LOGException(LOGE_INTERNAL_SERVER_ERROR, response, requestId, httpCode);
        }
        else
        {
            throw LOGException(LOGE_BAD_RESPONSE, string("Unextractable error:") + response, requestId, httpCode);
        }
    }
}

static int32_t ParseLogGroupList(const int32_t logGroupCount, const uint32_t uncompressedSize, const string& content, sls_logs::LogGroupList& logGroupList)
{
    string uncompressed = "";
    if (! aliyun_log_sdk_v6::CompressAlgorithm::UncompressLz4(content, uncompressedSize, uncompressed))
    {
        return 1;
    }

    if (logGroupCount > 0)
    {
        if (!logGroupList.ParseFromString(uncompressed) || logGroupList.loggrouplist_size() != logGroupCount)
        {
            logGroupList.Clear();
            return 2;
        }
    }

    return 0;
}

bool ShardItem::isReadOnly()
{
    return status == LOG_SHARD_STATUS_READONLY;
}

static void ParseBatchLogData(const string& nextCursor, const sls_logs::LogGroupList& logGroupList, BatchLogData& batchLogData)
{
    batchLogData.nextCursor = nextCursor;
    batchLogData.logGroupCount = logGroupList.loggrouplist_size();
    for (uint32_t index = 0; index < batchLogData.logGroupCount; ++index)
    {
        batchLogData.logGroups.push_back(vector<LogItem>());
        LogGroup logGroup = logGroupList.loggrouplist(index);
        for (int32_t logIdx = 0; logIdx < logGroup.logs_size(); ++logIdx)
        {
            LogItem li;
            const Log& log = logGroup.logs(logIdx);
            li.timestamp = log.time();
            li.topic = logGroup.topic();
            li.source = logGroup.source();
            for (int contentIdx = 0; contentIdx < log.contents_size(); ++contentIdx)
            {
                const Log_Content& lc = log.contents(contentIdx);
                li.data.push_back(pair<string, string>(lc.key(), lc.value()));
            }
            batchLogData.logGroups.back().push_back(li);
        }
    }
    
}


LOGClient::LOGClient(const string& slsHost, const string& accessKeyId, const string& accessKey, int32_t timeout, const string& source, bool compressFlag):
    mSlsHost(slsHost),
    mAccessKeyId(accessKeyId),
    mAccessKey(accessKey),
    mSource(source),
    mCompressFlag(compressFlag),
    mTimeout(timeout),
    mUserAgent(LOG_SDK_IDENTIFICATION),
    mKeyProvider(""),
    mHostFieldSuffix(""),
    mIsHostRawIp(false),    
    mGetDateString(CodecTool::GetDateString),
    mLOGSend(LOGAdapter::Send)
{
    pthread_spin_init(&mSpinLock, PTHREAD_PROCESS_PRIVATE);
    SetSlsHost(slsHost);
    if(mSource=="")
    {
        mSource = GetHostIp();
    }
    if(mTimeout<=0)
    {
        mTimeout = LOG_REQUEST_TIMEOUT;
    }
    mMaxSendSpeedInBytePerSec = 1024 * 1024 * 1024;
}
LOGClient::LOGClient(const string& slsHost, const string& accessKeyId, const string& accessKey, const std::string& securityToken, int32_t timeout, const string& source, bool compressFlag):
    mSlsHost(slsHost),
    mAccessKeyId(accessKeyId),
    mAccessKey(accessKey),
    mSecurityToken(securityToken),
    mSource(source),
    mCompressFlag(compressFlag),
    mTimeout(timeout),
    mUserAgent(LOG_SDK_IDENTIFICATION),
    mKeyProvider(""),
    mHostFieldSuffix(""),
    mIsHostRawIp(false),    
    mGetDateString(CodecTool::GetDateString),
    mLOGSend(LOGAdapter::Send)
{
    pthread_spin_init(&mSpinLock, PTHREAD_PROCESS_PRIVATE);
    SetSlsHost(slsHost);
    if(mSource=="")
    {
        mSource = GetHostIp();
    }
    if(mTimeout<=0)
    {
        mTimeout = LOG_REQUEST_TIMEOUT;
    }
    mMaxSendSpeedInBytePerSec = 1024 * 1024 * 1024;
}

LOGClient::~LOGClient() throw()
{
    pthread_spin_destroy(&mSpinLock);
}

static void ConvertLogGroup(const vector<LogItem>& logGroup, LogGroup& pbLogGroup)
{
    if(logGroup.size()==0)
    {
        throw LOGException(LOGE_PARAMETER_INVALID, "Empty LogItem.");
    }
    for(vector<LogItem>::const_iterator iter=logGroup.begin(); iter!=logGroup.end(); iter++)
    {
        Log* logPtr = pbLogGroup.add_logs();
        logPtr->set_time(iter->timestamp);
        for(vector<pair<string, string> >::const_iterator it=iter->data.begin(); it!=iter->data.end(); it++)
        {
            Log_Content* contentPtr = logPtr->add_contents();
            contentPtr->set_key(it->first);
            contentPtr->set_value(it->second);
        }
    }
}

void LOGClient::SetAccessKey(const string& accessKey)
{
    pthread_spin_lock(&mSpinLock);
    mAccessKey = accessKey;
    pthread_spin_unlock(&mSpinLock);
}

string LOGClient::GetAccessKey()
{
    pthread_spin_lock(&mSpinLock);
    string accessKey = mAccessKey;
    pthread_spin_unlock(&mSpinLock);
    return accessKey;
}

void LOGClient::SetAccessKeyId(const string& accessKeyId)
{
    pthread_spin_lock(&mSpinLock);
    mAccessKeyId = accessKeyId;
    pthread_spin_unlock(&mSpinLock);
}

string LOGClient::GetAccessKeyId()
{
    pthread_spin_lock(&mSpinLock);
    string accessKeyId = mAccessKeyId;
    pthread_spin_unlock(&mSpinLock);
    return accessKeyId;
}

string LOGClient::GetSlsHost()
{
    pthread_spin_lock(&mSpinLock);
    string slsHost = mSlsHost;
    pthread_spin_unlock(&mSpinLock);
    return slsHost;
}

string LOGClient::GetHostFieldSuffix()
{
    pthread_spin_lock(&mSpinLock);
    string hostFieldSuffix = mHostFieldSuffix;
    pthread_spin_unlock(&mSpinLock);
    return hostFieldSuffix;
}
void LOGClient::SetSlsHost(const string& slsHost)
{
    pthread_spin_lock(&mSpinLock);
    //mSlsHost = slsHost;
    size_t  bpos = slsHost.find("://");
    if(bpos == string::npos)
        bpos = 0;
    else
        bpos += 3;
    string tmpstr = slsHost.substr(bpos);
    size_t  epos = tmpstr.find_first_of("/");
    if(epos == string::npos)
        epos = tmpstr.length();
    string host = tmpstr.substr(0,epos);
    
    mSlsHost = host;

    mHostFieldSuffix = "." + host;
    size_t i = 0;
    for(; i < host.length(); ++i)
    {
        if((host[i] >= 'a' && host[i] <= 'z') || (host[i] >= 'A' && host[i] <= 'Z'))
            break;
    }
    if(i == host.length())
        mIsHostRawIp = true;
    else
        mIsHostRawIp = false;
    pthread_spin_unlock(&mSpinLock);
}

void LOGClient::SetCommonHeader(map<string, string>& httpHeader, int32_t contentLength, const string& project)
{
    if (project != "")
    {
        httpHeader[HOST] = project + GetHostFieldSuffix();
    }
    else
    {
        httpHeader[HOST] = GetSlsHost();
    }

    httpHeader[USER_AGENT] = mUserAgent;
    httpHeader[X_LOG_APIVERSION] = LOG_API_VERSION;
    httpHeader[X_LOG_SIGNATUREMETHOD] = HMAC_SHA1;
    httpHeader[DATE] = CodecTool::GetDateString();
    httpHeader[CONTENT_LENGTH] = ToString(contentLength);
    if(!mSecurityToken.empty())
    {
        httpHeader[X_ACS_SECURITY_TOKEN] = mSecurityToken;
    }
}

void LOGClient::SetCommonParameter(map<string, string>& parameterList)
{
}

string LOGClient::GetHost(const string& project)
{
    if(mIsHostRawIp || project == "")
    {
        return GetSlsHost();
    }
    else
    {
        return project + GetHostFieldSuffix();
    }
}

void LOGClient::SendRequest(const string& project, const string& httpMethod, const string& url, const string& body, const map<string, string>& parameterList, map<string, string>& header, HttpMessage& httpMessage)
{
    string host = GetHost(project);
    SetCommonHeader(header, body.length(), project);
    string signature = LOGAdapter::GetUrlSignature(httpMethod, url, header, parameterList, body, GetAccessKey());
    header[AUTHORIZATION] = LOG_HEADSIGNATURE_PREFIX + GetAccessKeyId() + ':' + signature;
    
    string queryString;
    LOGAdapter::GetQueryString(parameterList, queryString);
   
    mLOGSend(httpMethod, host, 80, url, queryString, header, body, mTimeout, httpMessage, mMaxSendSpeedInBytePerSec);
    
    if (httpMessage.statusCode != 200)
    {
        ErrorCheck(httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);
    }
}
PostLogStoreLogsResponse LOGClient::PostLogStoreLogs(const string& project, const string& logstore, const string& topic, const vector<LogItem>& logGroup,const std::string& hashKey)
{
    LogGroup pbLogGroup;
    ConvertLogGroup(logGroup, pbLogGroup);
    pbLogGroup.set_topic(topic);
    pbLogGroup.set_source(mSource);
    pbLogGroup.set_category(logstore);
    return PostLogStoreLogs(project, logstore, pbLogGroup,hashKey);
}

PostLogStoreLogsResponse LOGClient::PostLogStoreLogs(const string& project, const string& logstore, const LogGroup& logGroup,const std::string& hashKey)
{
    string body;
    string serializeData;
    if ((logGroup.source()).empty())
    {
        LogGroup pbLogGroup;
        pbLogGroup = logGroup;
        pbLogGroup.set_source(mSource);
        pbLogGroup.SerializeToString(&serializeData);
    }
    else
    {
        logGroup.SerializeToString(&serializeData);
    }

    string operation = LOGSTORES;
    operation.append("/").append(logstore);
    if(hashKey.empty() )
        operation.append("");
    else
        operation.append("/shards/route");
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_PROTOBUF;
    if(mKeyProvider.empty()==false)
        httpHeader[X_LOG_KEYPROVIDER] = mKeyProvider;

    if(mCompressFlag)
    {
        if(! aliyun_log_sdk_v6::CompressAlgorithm::CompressLz4(serializeData, body))
        {
            throw LOGException(LOGE_UNKNOWN_ERROR, "Data compress failed.");
        }
        httpHeader[X_LOG_COMPRESSTYPE] = LOG_LZ4;
    }
    else
    {
        body = serializeData;
    }
    
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(serializeData.size());
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    
    map<string, string> parameterList;
    if(hashKey.empty() == false)
        parameterList["key"] = hashKey;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
   
    PostLogStoreLogsResponse ret;
    ret.bodyBytes = body.size();
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];
    return ret;
}

static void ExtractLogMeta(HttpMessage& httpMessage, LogMeta& logMeta)
{
    rapidjson::Document document;
    try
    {   
        ExtractJsonResult(httpMessage.content, document);    

        logMeta.progress = httpMessage.header[X_LOG_PROGRESS];
        logMeta.count = atoi(httpMessage.header[X_LOG_COUNT].c_str());

        //const rapidjson::Value& histograms = GetJsonValue(document, "histograms");
        for (rapidjson::Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
        {
            logMeta.metaItems.push_back(LogMetaItem());
            LogMetaItem& metaItem = logMeta.metaItems.back();
            ExtractJsonResult(*itr, "from", metaItem.from);
            ExtractJsonResult(*itr, "to", metaItem.to);
            ExtractJsonResult(*itr, "count", metaItem.count);
            if ((*itr).HasMember("progress"))
            {
                ExtractJsonResult(*itr, "progress", metaItem.progress);
            }
        }
    }
    catch(JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}

GetLogStoreHistogramResponse LOGClient::GetLogStoreHistogram(const string& project, const string& logstore, const string& topic, time_t beginTime, time_t endTime, const string& query)
{
    string operation = LOGSTORES;
    operation.append("/").append(logstore).append(INDEX);
    string body = "";

    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    parameterList["type"] = "histogram";
    parameterList["topic"] = topic;
    parameterList["from"] = ToString(beginTime);
    parameterList["to"] = ToString(endTime);
    parameterList["query"] = query;

    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";

    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);

    GetLogStoreHistogramResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractLogMeta(httpResponse, ret.result);

    return ret;
}

static void ExtractLogs(HttpMessage& httpMessage, LogResult& logResult)
{
    rapidjson::Document document;
    try
    {
        ExtractJsonResult(httpMessage.content, document);    

        logResult.progress = httpMessage.header[X_LOG_PROGRESS];
        logResult.logline = atoi(httpMessage.header[X_LOG_COUNT].c_str());

        //const rapidjson::Value& logs = GetJsonValue(document, "logs");
        for (rapidjson::Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
        {
            logResult.logdatas.push_back(LogItem());
            LogItem& logItem = logResult.logdatas.back();
                
            string logTimeStamp = "";
            ExtractJsonResult(*itr, LOGITEM_TIME_STAMP_LABEL, logTimeStamp);
            logItem.timestamp = atoi(logTimeStamp.c_str());

            ExtractJsonResult(*itr, LOGITEM_SOURCE_LABEL, logItem.source);
            for (rapidjson::Value::ConstMemberIterator mItr = itr->MemberBegin(); mItr != itr->MemberEnd(); ++mItr)
            {
                if (string(mItr->name.GetString()) != string(LOGITEM_TIME_STAMP_LABEL) && string(mItr->name.GetString()) != string(LOGITEM_SOURCE_LABEL))
                {
                    if (mItr->value.IsString())
                    {
                        logItem.data.push_back(pair<string,string>(mItr->name.GetString(), mItr->value.GetString()));
                    }
                    else
                    {
                        throw LOGException(LOGE_BAD_RESPONSE, string("Invalid json format, value is not string type\tbad json format:") + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);
                    }
                }
            }
        }
    }
    catch(JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}

GetLogStoreLogsResponse LOGClient::GetLogStoreLogs(const string& project, const string& logstore, const string& topic, time_t beginTime, time_t endTime, bool reverseFlag, int32_t lines, int32_t offset, const string& query)
{
    string operation = LOGSTORES;
    operation.append("/").append(logstore).append(INDEX);

    string body = "";

    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    parameterList["type"]="log";
    parameterList["topic"]=topic;
    parameterList["from"]=ToString(beginTime);
    parameterList["to"]=ToString(endTime);
    parameterList["reverse"]=ToString(reverseFlag);
    parameterList["line"]=ToString(lines);
    parameterList["offset"]=ToString(offset);
    parameterList["query"]=query;

    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";

    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);

    GetLogStoreLogsResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractLogs(httpResponse, ret.result);

    return ret;
}
CreateConsumerGroupResponse LOGClient::CreateConsumerGroup(const std::string& project , const std::string& logstore, const std::string& consumergroup, const uint32_t timeoutInSec, const bool inOrder)
{
    ConsumerGroup group(consumergroup, timeoutInSec, inOrder);
    string operation = "/logstores/" + logstore + "/consumergroups";
    string body = group.ToRequestJsonString();
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    
    CreateConsumerGroupResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

DeleteConsumerGroupResponse LOGClient::DeleteConsumerGroup(const std::string& project , const std::string& logstore, const std::string& consumergroup)
{
    string operation = "/logstores/" + logstore + "/consumergroups/" + consumergroup;
    
    map<string, string> httpHeader;
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_DELETE, operation, "", parameterList, httpHeader, httpResponse);
    
    DeleteConsumerGroupResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

Response LOGClient::CreateLogStore(const string& project, const LogStore& logStore)
{
    string body = logStore.ToRequestJsonString();
    
    string operation = LOGSTORES;
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

Response LOGClient::UpdateLogStore(const string& project, const LogStore& logStore)
{
    string body = logStore.ToRequestJsonString();
    
    string operation = LOGSTORES;
    operation.append("/").append(logStore.GetLogStoreName());
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_PUT, operation, body, parameterList, httpHeader, httpResponse);
    
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

Response LOGClient::DeleteLogStore(const string& project, const string& logStore)
{
    string body = "";
    
    string operation = LOGSTORES;
    operation.append("/").append(logStore);
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = "";
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_DELETE, operation, body, parameterList, httpHeader, httpResponse);

    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

static void ExtractLogStore(HttpMessage& httpMessage, LogStore& logStore)
{
    try
    {
        logStore.FromJsonString(httpMessage.content);
    }
    catch (JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}

GetLogStoreResponse LOGClient::GetLogStore(const string& project, const string& logStore)
{
    string body = "";
    
    string operation = LOGSTORES;
    operation.append("/").append(logStore);
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = "";
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);

    GetLogStoreResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractLogStore(httpResponse, ret.result);

    return ret;
}
static void ExtractHeartbeat(HttpMessage& httpMessage, std::vector<uint32_t>& shards)
{
    rapidjson::Document doc;
    try
    {
        ExtractJsonResult(httpMessage.content, doc);
        typeof(doc.GetArray()) array = doc.GetArray();
        for (Value::ConstValueIterator itr = array.Begin(); itr != array.End(); ++itr)
        {
            shards.push_back(itr->GetUint());
        }
    }
    catch(JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}
static void ExtractConsumerGroupCheckpoints(HttpMessage& httpMessage, vector<ConsumerGroupCheckpoint>& cps)
{
    rapidjson::Document doc;
    try
    {
        ExtractJsonResult(httpMessage.content, doc);
        typeof(doc.GetArray()) array = doc.GetArray();
        for (Value::ConstValueIterator itr = array.Begin(); itr != array.End(); ++itr)
        {
            ConsumerGroupCheckpoint cp((*itr)["shard"].GetUint(), (*itr)["checkpoint"].GetString(), (*itr)["updateTime"].GetUint64());
            cps.push_back(cp);
        }
    }
    catch(JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}
static void ExtractConsumerGroups(HttpMessage& httpMessage, vector<ConsumerGroup>& consumerGroups)
{
    rapidjson::Document doc;
    try
    {
        ExtractJsonResult(httpMessage.content, doc);
        typeof(doc.GetArray()) array = doc.GetArray();
        for (Value::ConstValueIterator itr = array.Begin(); itr != array.End(); ++itr)
        {
            ConsumerGroup group((*itr)["name"].GetString(), (*itr)["timeout"].GetUint(), (*itr)["order"].GetBool());
            consumerGroups.push_back(group);
        }
    }
    catch(JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}
static void ExtractLogStores(HttpMessage& httpMessage, vector<string>& logStoresResult)
{
    rapidjson::Document document;
    try
    {
        ExtractJsonResult(httpMessage.content, document);    
        const rapidjson::Value& logStores = GetJsonValue(document, "logstores");
        for (rapidjson::Value::ConstValueIterator itr = logStores.Begin(); itr != logStores.End(); ++itr)
        {
            if (itr->IsString())
            {
                logStoresResult.push_back(itr->GetString());
            }
            else
            {
                throw LOGException(LOGE_BAD_RESPONSE, string("Invalid json format, value is not string type\tbad json format:") + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);
            }
        }
    }
    catch(JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}
ListConsumerGroupResponse LOGClient::ListConsumerGroup(const string& project, const std::string& logstore)
{
    string operation = "/logstores/" + logstore + "/consumergroups";
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    map<string, string> httpHeader;
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, "", parameterList, httpHeader, httpResponse);
    ListConsumerGroupResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractConsumerGroups(httpResponse, ret.consumerGroups);
    return ret;
}
HeartbeatResponse LOGClient::ConsumerGroupHeartbeat(const std::string& project, const std::string& logstore, const std::string& consumergroup, const std::string& consumer, const std::vector<uint32_t>& shards)
{
    string operation = "/logstores/" + logstore + "/consumergroups/" + consumergroup;
    Heartbeat hb;
    for(std::vector<uint32_t>::const_iterator it = shards.begin(); it != shards.end(); ++it)
        hb.Add(*it);
    string body = hb.ToRequestJsonString(true);
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    parameterList["type"] = "heartbeat";
    parameterList["consumer"] = consumer;

    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    HeartbeatResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];
    ExtractHeartbeat(httpResponse, ret.shards);

    return ret;

}
UpdateCheckpointResponse LOGClient::UpdateCheckpoint(const std::string& project, const std::string& logstore, const std::string& consumergroup, const int shard, const std::string& checkpoint)
{
    string operation = "/logstores/" + logstore + "/consumergroups/" + consumergroup;
    ConsumerGroupCheckpoint cp(shard, checkpoint);
    string body = cp.ToRequestJsonString();
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    parameterList["type"] = "checkpoint";
    parameterList["forceSuccess"] = "true";

    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    UpdateCheckpointResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}
ListCheckpointResponse LOGClient::ListCheckpoint(const std::string& project, const std::string& logstore, const std::string& consumergroup)
{
    string operation = "/logstores/" + logstore + "/consumergroups/" + consumergroup;
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    map<string, string> httpHeader;
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, "", parameterList, httpHeader, httpResponse);
    ListCheckpointResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractConsumerGroupCheckpoints(httpResponse, ret.consumerGroupCheckpoints);
    return ret;

}
ListLogStoresResponse LOGClient::ListLogStores(const string& project, const string& logstorename, const int32_t& offset, const int32_t& size)
{
    const string operation = LOGSTORES;
    
    string body = "";

    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    if (logstorename != "")
    {
        parameterList["logstorename"] = logstorename;
    }
    if (offset >= 0)
    {
        parameterList["offset"] = ToString(offset);
    }
    if (size >= 0)
    {
        parameterList["size"] = ToString(size);
    }
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = "";
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);
    
    ListLogStoresResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractLogStores(httpResponse, ret.result);

    return ret;
}

Response LOGClient::CreateIndex(const string& project, const string& logStore, const Index& index)
{
    string body = index.ToRequestJsonString();
    
    string operation = LOGSTORES;
    operation.append("/").append(logStore).append(INDEX);
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

Response LOGClient::UpdateIndex(const string& project, const string& logStore, const Index& index)
{
    string body = index.ToRequestJsonString();
    
    string operation = LOGSTORES;
    operation.append("/").append(logStore).append(INDEX);
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_PUT, operation, body, parameterList, httpHeader, httpResponse);
    
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

static void ExtractIndex(HttpMessage& httpMessage, Index& index)
{
    try
    {
        index.FromJsonString(httpMessage.content);
    }
    catch (JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}

GetIndexResponse LOGClient::GetIndex(const string& project, const string& logStore)
{
    string body = "";
    
    string operation = LOGSTORES;
    operation.append("/").append(logStore).append(INDEX);
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = "";
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);

    GetIndexResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractIndex(httpResponse, ret.result);

    return ret;
}

Response LOGClient::DeleteIndex(const string& project, const string& logStore)
{
    string body = "";
    
    string operation = LOGSTORES;
    operation.append("/").append(logStore).append(INDEX);
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = "";
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_DELETE, operation, body, parameterList, httpHeader, httpResponse);

    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}


Response LOGClient::CreateConfig(const string& project, const Config& config)
{
    string body = config.ToRequestJsonString();
    
    string operation = CONFIGS;
    
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);

    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

Response LOGClient::UpdateConfig(const string& project, const Config& config)
{
    string body = config.ToRequestJsonString();
    
    string operation = CONFIGS;
    operation.append("/").append(config.GetConfigName());
    
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_PUT, operation, body, parameterList, httpHeader, httpResponse);
    
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

Response LOGClient::DeleteConfig(const string& project, const string& config)
{
    string body = "";
    
    string operation = CONFIGS;
    operation.append("/").append(config);
    
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_DELETE, operation, body, parameterList, httpHeader, httpResponse);
    
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

static void ExtractConfig(HttpMessage& httpMessage, Config& config)
{
    try
    {
        config.FromJsonString(httpMessage.content);
    }
    catch (JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}

GetConfigResponse LOGClient::GetConfig(const string& project, const string& config)
{
    string body = "";
    
    string operation = CONFIGS;
    operation.append("/").append(config);
    
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);
    
    GetConfigResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractConfig(httpResponse, ret.result);

    return ret;
}

static void ExtractConfigs(HttpMessage& httpMessage, vector<string>& configsResult)
{
    rapidjson::Document document;
    try
    {
        ExtractJsonResult(httpMessage.content, document);    
        
        const rapidjson::Value& configs = GetJsonValue(document, "configs");
        for (rapidjson::Value::ConstValueIterator itr = configs.Begin(); itr != configs.End(); ++itr)
        {
            if (itr->IsString())
            {
                configsResult.push_back(itr->GetString());
            }
            else
            {
                throw LOGException(LOGE_BAD_RESPONSE, string("Invalid json format, value is not string type\tbad json format:") + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);
            }
        }
    }
    catch(JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}

ListConfigsResponse LOGClient::ListConfigs(const string& project, const string& configName, const int32_t& offset, const int32_t& size)
{
    const string operation = CONFIGS;
    
    string body;

    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    if (configName != "")
    {
        parameterList["configname"] = configName;
    }
    if (offset >= 0)
    {
        parameterList["offset"] = ToString(offset);
    }
    if (size >= 0)
    {
        parameterList["size"] = ToString(size);
    }
    
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);
    
    ListConfigsResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractConfigs(httpResponse, ret.result);
    
    return ret;
}

Response LOGClient::CreateMachineGroup(const string& project, const MachineGroup& machineGroup)
{
    string body = machineGroup.ToRequestJsonString();
    
    string operation = MACHINEGROUPS;
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

Response LOGClient::UpdateMachineGroup(const string& project, const MachineGroup& machineGroup)
{
    string body = machineGroup.ToRequestJsonString();
    
    string operation = MACHINEGROUPS;
    operation.append("/").append(machineGroup.GetGroupName());
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_PUT, operation, body, parameterList, httpHeader, httpResponse);
    
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

Response LOGClient::DeleteMachineGroup(const string& project, const string& machineGroup)
{
    string body = "";
    
    string operation = MACHINEGROUPS;
    operation.append("/").append(machineGroup);
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_DELETE, operation, body, parameterList, httpHeader, httpResponse);
    
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

static void ExtractMachineGroup(HttpMessage& httpMessage, MachineGroup& machineGroup)
{
    try
    {
        machineGroup.FromJsonString(httpMessage.content);
    }
    catch (JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}

GetMachineGroupResponse LOGClient::GetMachineGroup(const string& project, const string& machineGroup)
{
    string body = "";
    
    string operation = MACHINEGROUPS;
    operation.append("/").append(machineGroup);
    
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);
    
    GetMachineGroupResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractMachineGroup(httpResponse, ret.result);

    return ret;
}

static void ExtractMachineGroups(HttpMessage& httpMessage, vector<string>& machineGroupsResult)
{
    rapidjson::Document document;
    try
    {
        ExtractJsonResult(httpMessage.content, document);    
        
        const rapidjson::Value& machineGroups = GetJsonValue(document, "machinegroups");
        for (rapidjson::Value::ConstValueIterator itr = machineGroups.Begin(); itr != machineGroups.End(); ++itr)
        {
            if (itr->IsString())
            {
                machineGroupsResult.push_back(itr->GetString());
            }
            else
            {
                throw LOGException(LOGE_BAD_RESPONSE, string("Invalid json format, value is not string type\tbad json format:") + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);
            }
        }
    }
    catch(JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}

ListMachineGroupsResponse LOGClient::ListMachineGroups(const string& project, const int32_t& offset, const int32_t& size)
{
    const string operation = MACHINEGROUPS;
    
    string body = "";

    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    if (offset >= 0)
    {
        parameterList["offset"] = ToString(offset);
    }
    if (size >= 0)
    {
        parameterList["size"] = ToString(size);
    }
    
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);
    
    ListMachineGroupsResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractMachineGroups(httpResponse, ret.result);
    
    return ret;
}

Response LOGClient::ApplyConfigToMachineGroup(const string& project, const string& machineGroup, const string& config)
{
    string body = "";
    
    string operation = MACHINEGROUPS;
    operation.append("/").append(machineGroup).append(CONFIGS)
        .append("/").append(config);
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_PUT, operation, body, parameterList, httpHeader, httpResponse);
    
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

Response LOGClient::RemoveConfigFromMachineGroup(const string& project, const string& machineGroup, const string& config)
{
    string body = "";
    
    string operation = MACHINEGROUPS;
    operation.append("/").append(machineGroup).append(CONFIGS)
        .append("/").append(config);
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_DELETE, operation, body, parameterList, httpHeader, httpResponse);
    
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}

static void ExtractApplyConfigs(HttpMessage& httpMessage, vector<string>& configsResult)
{
    rapidjson::Document document;
    try
    {
        ExtractJsonResult(httpMessage.content, document);    
        
        const rapidjson::Value& configs = GetJsonValue(document, "configs");
        for (rapidjson::Value::ConstValueIterator itr = configs.Begin(); itr != configs.End(); ++itr)
        {
            if (itr->IsString())
            {
                configsResult.push_back(itr->GetString());
            }
            else
            {
                throw LOGException(LOGE_BAD_RESPONSE, string("Invalid json format, value is not string type\tbad json format:") + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);
            }
        }
    }
    catch(JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}

GetAppliedConfigsResponse LOGClient::GetAppliedConfigs(const string& project, const string& machineGroup)
{
    string body = "";
    
    string operation = MACHINEGROUPS;
    operation.append("/").append(machineGroup).append(CONFIGS);
    
    map<string, string> httpHeader;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    httpHeader[CONTENT_MD5] = CodecTool::CalcMD5(body);
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);
    
    GetAppliedConfigsResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractApplyConfigs(httpResponse, ret.result);

    return ret;
}

LogStoreSqlResponse LOGClient::ExecuteLogStoreSql(const std::string &project, const std::string &logstore, time_t beginTime, time_t endTime, const std::string &query, bool powerSql)
{
    string operation = LOGSTORES;
    operation.append("/").append(logstore).append(INDEX);
    string body = "";
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    parameterList["type"] = "log";
    parameterList["from"] = apsara::ToString(beginTime);
    parameterList["to"] = apsara::ToString(endTime);
    parameterList["query"] = query;
    parameterList["powerSql"] = apsara::ToString(powerSql);
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = apsara::ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);
    LogStoreSqlResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];
    if (httpResponse.header.count(X_LOG_PROCESSED_ROWS) > 0)
        ret.processedRows = atol(httpResponse.header.at(X_LOG_PROCESSED_ROWS).c_str());
    if (httpResponse.header.count(X_LOG_ELASPED_MILLISECOND) > 0)
        ret.elapsedMilli = atol(httpResponse.header.at(X_LOG_ELASPED_MILLISECOND).c_str());
    if (httpResponse.header.count(X_LOG_CPU_SEC) > 0)
        ret.cpuSec = atof(httpResponse.header.at(X_LOG_CPU_SEC).c_str());
    if (httpResponse.header.count(X_LOG_CPU_CORES) > 0)
        ret.cpuCore = atol(httpResponse.header.at(X_LOG_CPU_CORES).c_str());
    ExtractLogs(httpResponse, ret.result);
    return ret;
}
ProjectSqlResponse LOGClient::ExecuteProjectSql(const std::string &project, const std::string &query, bool powerSql)
{
    string operation = "/logs";
    string body = "";
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    parameterList["query"] = query;
    parameterList["powerSql"] = apsara::ToString(powerSql);
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = apsara::ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);
    ProjectSqlResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];
    if (httpResponse.header.count(X_LOG_PROCESSED_ROWS) > 0)
        ret.processedRows = atol(httpResponse.header.at(X_LOG_PROCESSED_ROWS).c_str());
    if (httpResponse.header.count(X_LOG_ELASPED_MILLISECOND) > 0)
        ret.elapsedMilli = atol(httpResponse.header.at(X_LOG_ELASPED_MILLISECOND).c_str());
    if (httpResponse.header.count(X_LOG_CPU_SEC) > 0)
        ret.cpuSec = atof(httpResponse.header.at(X_LOG_CPU_SEC).c_str());
    if (httpResponse.header.count(X_LOG_CPU_CORES) > 0)
        ret.cpuCore = atol(httpResponse.header.at(X_LOG_CPU_CORES).c_str());
    ExtractLogs(httpResponse, ret.result);
    return ret;
}
CreateSqlInstanceResponse LOGClient::CreateSqlInstance(const std::string &project, int cu)
{
    std::string operation = "/sqlinstance";
    std::string body;
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("cu");
    writer.Int(cu);
    writer.EndObject();
    body = stringBuffer.GetString();
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = apsara::ToString(body.length());
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    CreateSqlInstanceResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];
    return ret;
}
UpdateSqlInstanceResponse LOGClient::UpdateSqlInstance(const std::string &project, int cu)
{
    std::string operation = "/sqlinstance";
    std::string body;
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("cu");
    writer.Int(cu);
    writer.EndObject();
    body = stringBuffer.GetString();
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = apsara::ToString(body.length());
    httpHeader[CONTENT_TYPE] = TYPE_LOG_JSON;
    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    UpdateSqlInstanceResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];
    return ret;
}
ListSqlInstanceResponse LOGClient::ListSqlInstance(const std::string &project)
{
    std::string operation = "/sqlinstance";
    std::string body;
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = apsara::ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    ListSqlInstanceResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];
    rapidjson::Document document;
    try
    {
        ExtractJsonResult(httpResponse.content, document);
        for (rapidjson::Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
        {
            SqlInstance sqlInstance;
            ExtractJsonResult(*itr, "name", sqlInstance.name);
            string value;
            ExtractJsonResult(*itr, "cu", value);
            sqlInstance.cu = atoi(value.c_str());
            ExtractJsonResult(*itr, "updateTime", value);
            sqlInstance.updateTime = atoi(value.c_str());
            ExtractJsonResult(*itr, "createTime", value);
            sqlInstance.createTime = atoi(value.c_str());
            ret.sqlInstances.push_back(sqlInstance);
        }
    }
    catch (JsonException &e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpResponse.content, httpResponse.header[X_LOG_REQUEST_ID], httpResponse.statusCode);
    }
    return ret;
}

static void ExtractTopics(HttpMessage& httpMessage, vector<string>& result)
{
    rapidjson::Document document;
    try
    {
        ExtractJsonResult(httpMessage.content, document);    
        
        for (rapidjson::Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
        {
            if (itr->IsString())
            {
                result.push_back(itr->GetString());
            }
            else
            {
                throw LOGException(LOGE_BAD_RESPONSE, string("Invalid json format, value is not string type\tbad json format:") + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);
            }
        }
    }
    catch(JsonException& e)
    {
        throw LOGException(LOGE_BAD_RESPONSE, e.GetMessage() + "\tbad json format:" + httpMessage.content, httpMessage.header[X_LOG_REQUEST_ID], httpMessage.statusCode);    
    }
}

ListLogStoreTopicsResponse LOGClient::ListLogStoreTopics(const string& project, const string& logstore, const int64_t& line, const string& nextToken)
{
    string operation = LOGSTORES;
    operation.append("/").append(logstore).append(INDEX);
    string body;
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    
    parameterList["type"] = "topic";
    parameterList["line"] = ToString(line);
    parameterList["token"] = nextToken;
    
    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);
    
    ListLogStoreTopicsResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    ExtractTopics(httpResponse, ret.result);
    
    return ret;
}

GetCursorResponse LOGClient::GetCursor(const string& project, const string& logstore, uint32_t shardId, uint32_t fromTime)
{
    return GetCursor(project, logstore, shardId, "from", ToString(fromTime));
}

GetCursorResponse LOGClient::GetCursor(const string& project, const string& logstore, uint32_t shardId, LOG_Cursor_Mode cursorMode)
{
    string cursorModePara;
    switch (cursorMode)
    {
    case CURSOR_MODE_BEGIN:
        cursorModePara = "begin";
        break;
    case CURSOR_MODE_END:
        cursorModePara = "end";
        break;
    default:
        cursorModePara = "";
    }        
    return GetCursor(project, logstore, shardId, "from", cursorModePara);
}

GetCursorResponse LOGClient::GetCursor(const string& project, const string& logstore, uint32_t shardId, const string& paraKey, const string& paraValue)
{
    if (project.empty() || logstore.empty())
        throw LOGException(LOGE_PARAMETER_INVALID, "project or logstore invalid.");

    string operation = LOGSTORES + string("/") + logstore + SHARDS + string("/") + ToString(shardId);

    string body;
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    parameterList[LOG_TYPE] = LOG_TYPE_CURSOR;
    parameterList[paraKey] = paraValue;
    map<string, string> httpHeader;
    
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);

    GetCursorResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    rapidjson::Document document;
    ExtractJsonResult(httpResponse.content, document);    
    ExtractJsonResult(document, "cursor", ret.result);

    return ret;
}

ListShardsResponse LOGClient::ListShards(const string& project, const string& logstore)
{
    if (project.empty() || logstore.empty())
        throw LOGException(LOGE_PARAMETER_INVALID, "project or logstore invalid.");
    string operation = LOGSTORES + string("/") + logstore + SHARDS;
    string body = "";

    map<string, string> parameterList;
    SetCommonParameter(parameterList);

    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);
    
    rapidjson::Document document;
    ExtractJsonResult(httpResponse.content, document);    
    
    ListShardsResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    if (document.IsArray())
    {
        for (rapidjson::Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
        {
            if (itr->IsObject())
            {
                ShardItem shardItem;
                ExtractJsonResult(*itr, "shardID", shardItem.shardId);
                ExtractJsonResult(*itr,"status",shardItem.status);
                ExtractJsonResult(*itr,"inclusiveBeginKey",shardItem.inclusiveBeginKey);
                ExtractJsonResult(*itr,"exclusiveEndKey",shardItem.exclusiveEndKey);
                ExtractJsonResult(*itr, "createTime", shardItem.createTime);
                ret.result.push_back(shardItem);
            }
            else
            {
                throw LOGException(LOGE_BAD_RESPONSE, string("Invalid json format, value is not json object type\tbad json format:") + httpResponse.content, httpResponse.header[X_LOG_REQUEST_ID], httpResponse.statusCode);
            }
        }
    }
    
    return ret;
}
SplitShardResponse LOGClient::SplitShard(const string& project, const string& logstore,const int shardId,const std::string& middleHash)
{
    if (project.empty() || logstore.empty())
        throw LOGException(LOGE_PARAMETER_INVALID, "project or logstore invalid.");
    string operation = LOGSTORES + string("/") + logstore + SHARDS+"/"+ ToString(shardId);
;
    string body = "";

    map<string, string> parameterList;
    parameterList["key"] = middleHash;
    parameterList["action"] = "split";
    SetCommonParameter(parameterList);

    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    
    rapidjson::Document document;
    ExtractJsonResult(httpResponse.content, document);    
    
    ListShardsResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    if (document.IsArray())
    {
        for (rapidjson::Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
        {
            if (itr->IsObject())
            {
                ShardItem shardItem;
                ExtractJsonResult(*itr, "shardID", shardItem.shardId);
                ExtractJsonResult(*itr,"status",shardItem.status);
                ExtractJsonResult(*itr,"inclusiveBeginKey",shardItem.inclusiveBeginKey);
                ExtractJsonResult(*itr,"exclusiveEndKey",shardItem.exclusiveEndKey);
                ret.result.push_back(shardItem);
            }
            else
            {
                throw LOGException(LOGE_BAD_RESPONSE, string("Invalid json format, value is not json object type\tbad json format:") + httpResponse.content, httpResponse.header[X_LOG_REQUEST_ID], httpResponse.statusCode);
            }
        }
    }
    return ret;
}
MergeShardsResponse LOGClient::MergeShard(const string& project, const string& logstore,const int shardId)
{
    if (project.empty() || logstore.empty())
        throw LOGException(LOGE_PARAMETER_INVALID, "project or logstore invalid.");
    string operation = LOGSTORES + string("/") + logstore + SHARDS+string("/") + ToString(shardId);;

    string body = "";

    map<string, string> parameterList;
    parameterList["action"] = "merge";
    SetCommonParameter(parameterList);

    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    
    rapidjson::Document document;
    ExtractJsonResult(httpResponse.content, document);    
    
    ListShardsResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    if (document.IsArray())
    {
        for (rapidjson::Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
        {
            if (itr->IsObject())
            {
                ShardItem shardItem;
                ExtractJsonResult(*itr, "shardID", shardItem.shardId);
                ExtractJsonResult(*itr,"status",shardItem.status);
                ExtractJsonResult(*itr,"inclusiveBeginKey",shardItem.inclusiveBeginKey);
                ExtractJsonResult(*itr,"exclusiveEndKey",shardItem.exclusiveEndKey);
                ExtractJsonResult(*itr,"createTime",shardItem.createTime);
                ret.result.push_back(shardItem);
            }
            else
            {
                throw LOGException(LOGE_BAD_RESPONSE, string("Invalid json format, value is not json object type\tbad json format:") + httpResponse.content, httpResponse.header[X_LOG_REQUEST_ID], httpResponse.statusCode);
            }
        }
    }
    return ret;
}
Response LOGClient::DeleteShard( const std::string& project,const std::string& logstore,const int shardId)
{
    if (project.empty() || logstore.empty())
        throw LOGException(LOGE_PARAMETER_INVALID, "project or logstore invalid.");
    string operation = LOGSTORES + string("/") + logstore + SHARDS+string("/") + ToString(shardId);;
    string body = "";

    map<string, string> parameterList;
    SetCommonParameter(parameterList);

    map<string, string> httpHeader;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_DELETE, operation, body, parameterList, httpHeader, httpResponse);
    Response ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];

    return ret;
}
GetPbBatchLogResponse LOGClient::GetPbBatchLog(const string& project, const string& logstore, uint32_t shardId, int count, const string& cursor, const std::string& endCursor)
{
    GetPbBatchLogResponse ret;

    PullDataResponse res = GetLogGroupList(project, logstore, shardId, count, cursor, endCursor, ret.result.logGroupList);
    ret.statusCode = res.statusCode;
    ret.requestId = res.requestId;
    ret.result.nextCursor = res.cursor;
    ret.result.logGroupCount = ret.result.logGroupList.loggrouplist_size();
    return ret;
}

GetBatchLogResponse LOGClient::GetBatchLog(const string& project, const string& logstore, uint32_t shardId, int count, const string& cursor, const std::string& endCursor)
{
    GetBatchLogResponse ret;

    LogGroupList logGroupList;
    
    PullDataResponse res = GetLogGroupList(project, logstore, shardId, count, cursor, endCursor, logGroupList);
    ret.statusCode = res.statusCode;
    ret.requestId = res.requestId;

    ParseBatchLogData(res.cursor, logGroupList, ret.result);

    return ret;
}

PullDataResponse LOGClient::GetLogGroupList(const string& project, const string& logstore, uint32_t shardId, int count, const string& cursor, const std::string& endCursor, sls_logs::LogGroupList& logGroupList)
{
    if (project.empty() || logstore.empty())
        throw LOGException(LOGE_PARAMETER_INVALID, "project or logstore invalid.");
    string operation = LOGSTORES + string("/") + logstore + SHARDS + string("/") + ToString(shardId);
    string body;
    map<string, string> parameterList;
    SetCommonParameter(parameterList);
    parameterList["type"] = "log";
    parameterList["cursor"] = cursor;
    if (!endCursor.empty())
        parameterList["end_cursor"] = endCursor;
    parameterList["count"] = ToString(count);
    map<string, string> httpHeader;
    
    httpHeader[ACCEPT_ENCODING] = LOG_LZ4; 
    httpHeader[HTTP_ACCEPT] = TYPE_LOG_PROTOBUF;
    httpHeader[X_LOG_BODYRAWSIZE] = ToString(body.length());
    httpHeader[CONTENT_TYPE] = "";
    
    HttpMessage httpResponse;
    SendRequest(project, HTTP_GET, operation, body, parameterList, httpHeader, httpResponse);
    
    int32_t retCode = ParseLogGroupList(atoi(httpResponse.header["x-log-count"].c_str()), atoi(httpResponse.header[X_LOG_BODYRAWSIZE].c_str()), httpResponse.content, logGroupList);
    if (retCode == 1)
    {
        throw LOGException(LOGE_BAD_RESPONSE, "Fail to uncompress data", httpResponse.header[X_LOG_REQUEST_ID], httpResponse.statusCode);
    }
    else if (retCode == 2)
    {
        throw LOGException(LOGE_BAD_RESPONSE, "Parse LogGroupLddist from string fail", httpResponse.header[X_LOG_REQUEST_ID], httpResponse.statusCode);
    }
    
    PullDataResponse ret;
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[X_LOG_REQUEST_ID];
    ret.cursor = httpResponse.header[X_LOG_CURSOR];

    return ret;
}
}
