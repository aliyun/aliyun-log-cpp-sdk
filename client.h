#ifndef __LOGV6_CLIENT_H__
#define __LOGV6_CLIENT_H__
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "RestfulApiCommon.h"
#include "sls_logs.pb.h"
#include "resource.h"
#include <mutex>

namespace ehttp
{
class HttpController;
class MemHttpRequest;
class MemHttpResponse;
}

namespace google
{
namespace protobuf
{
class Closure;
}
}
typedef class google::protobuf::Closure RPCClosure;

namespace aliyun_log_sdk_v6
{

void ExtractJsonResult(const std::string& response, rapidjson::Document& document);

void JsonMemberCheck(const rapidjson::Value& value, const char* name);

void ExtractJsonResult(const rapidjson::Value& value, const char* name, int32_t& number);

void ExtractJsonResult(const rapidjson::Value& value, const char* name, uint32_t& number);

void ExtractJsonResult(const rapidjson::Value& value, const char* name, int64_t& number);

void ExtractJsonResult(const rapidjson::Value& value, const char* name, uint64_t& number);

void ExtractJsonResult(const rapidjson::Value& value, const char* name, bool& boolean);

void ExtractJsonResult(const rapidjson::Value& value, const char* name, std::string& dst);

const rapidjson::Value& GetJsonValue(const rapidjson::Value& value, const char* name);

/** Struct which is the definition of the log item.
*/
extern const char* const LOG_SDK_IDENTIFICATION;
typedef struct
{
    uint32_t timestamp;///<The timestamp of the log item.
    std::string source;///<The source of the log item.
    std::string topic;
    std::vector<std::pair<std::string, std::string> > data;///< The data of the log item, including many key-value pairs. 
}LogItem;


typedef struct
{
    std::string progress;  // "Complete" if the request is full success, other wise the [logdatas] means part of the result 
    int32_t logline; // the logs lines returned 
    std::vector<LogItem> logdatas; //the result data returned
}LogResult;


/** struct store the log meta info for every specific time range [beginTime , endTime)
 */
typedef struct
{
    uint32_t from;  /// the log meta begin time (include)
    uint32_t to;  /// the log meta end time(exclude)
    uint32_t count;  /// total log count in [beginTime, endTime)
    std::string progress;  /// a flag if there is more data not read, if it is set to true, the user can reduce the 
                    /// read time range to get more precise result
}LogMetaItem;

/** total log meta info, 
 */
typedef struct
{
    uint32_t count;   /// total  log count found
    std::string progress;  /// a flag if there is more data not read, if it is set to true, the user can reduce the 
                    /// read time range to get more precise result
    std::vector<LogMetaItem> metaItems; /// the log meta for every small time range piece
}LogMeta;

typedef struct BatchLogData
{
    std::vector<std::vector<LogItem> > logGroups;
    uint32_t logGroupCount;
    std::string nextCursor;
}BatchLogData;

typedef struct PbBatchLogData
{
    sls_logs::LogGroupList logGroupList;
    uint32_t logGroupCount;
    std::string nextCursor;
}PbBatchLogData;
typedef struct ShardItem
{
    uint32_t shardId;
    std::string status;
    std::string inclusiveBeginKey ;
    std::string exclusiveEndKey;
    int createTime;
    bool isReadOnly();
} ShardItem;
/**
 *LOG Json Exception includes an error code and a detail message.
 */
class JsonException : public std::exception
{
/** Constructor with error code and message. 
* @param errorCode LOG error code. 
* @param message Detailed information for the exception.
* @return The objcect pointer.
*/
public:
    JsonException(const std::string& errorCode, const std::string& message)
        :mErrorCode(errorCode),mMessage(message)
    {
    }
    ~JsonException() throw()
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
private:
    std::string mErrorCode;
    std::string mMessage;
};

/*
 * Responses
 */
struct Response
{
    int32_t statusCode;
    std::string requestId;
};

struct CreateConsumerGroupResponse: public Response
{
};
struct DeleteConsumerGroupResponse: public Response
{
};

struct ListConsumerGroupResponse: public Response
{
    std::vector<ConsumerGroup> consumerGroups;
};
struct UpdateCheckpointResponse: public Response
{};
struct ListCheckpointResponse: public Response
{
    std::vector<ConsumerGroupCheckpoint> consumerGroupCheckpoints;
};
struct HeartbeatResponse: public Response
{
    std::vector<uint32_t> shards;
};
struct PostLogStoreLogsResponse : public Response
{
    int32_t bodyBytes;
};

struct GetLogStoreLogsResponse : public Response
{
    LogResult result;
};

struct GetLogStoreHistogramResponse : public Response
{
    LogMeta result;
};

struct GetLogStoreResponse : public Response
{
    LogStore result;
};

struct ListLogStoresResponse : public Response
{
    std::vector<std::string> result;
};

struct GetCursorResponse : public Response
{
    std::string result;
};

struct ListLogStoreTopicsResponse : public Response
{
    std::vector<std::string> result;
};

struct ListShardsResponse : public Response
{
    std::vector<ShardItem> result;
};

struct GetBatchLogResponse : public Response
{
    BatchLogData result;
}; 

struct GetPbBatchLogResponse : public Response
{
    PbBatchLogData result;
};

struct GetIndexResponse : public Response
{
    Index result;
};

struct GetConfigResponse : public Response
{
    Config result;
};

struct ListConfigsResponse : public Response
{
    std::vector<std::string> result;
};

struct GetMachineGroupResponse : public Response
{
    MachineGroup result;
};

struct ListMachineGroupsResponse : public Response
{
    std::vector<std::string> result;
};

struct GetAppliedConfigsResponse : public Response
{
    std::vector<std::string> result;
};

struct PullDataResponse : public Response
{
    std::string cursor;
};
struct LogStoreSqlResponse : public Response
{
    int64_t processedRows;
    int64_t elapsedMilli;
    double cpuSec;
    int64_t cpuCore;
    LogResult result;
    LogStoreSqlResponse() : processedRows(0), elapsedMilli(0), cpuSec(0.0), cpuCore(0) {}
};
typedef LogStoreSqlResponse ProjectSqlResponse;
struct CreateSqlInstanceResponse : public Response
{
};
struct UpdateSqlInstanceResponse : public Response
{
};
struct SqlInstance
{
    std::string name;
    int cu;
    time_t createTime;
    time_t updateTime;
    SqlInstance() : cu(0), createTime(0), updateTime(0) {}
};
struct ListSqlInstanceResponse : public Response
{
    std::vector<SqlInstance> sqlInstances;
};

typedef ListShardsResponse  SplitShardResponse ;
typedef ListShardsResponse  MergeShardsResponse ;
/** A client which is used to access the LOG service.
*/
class LOGClient
{
public:
/** Constructor needs at least three parameters.
* @param LOGHost LOG service address, for example:http://LOG.aliyun-inc.com. 
* @param accessKeyId Aliyun AccessKeyId. 
* @param accessKey Aliyun AccessKey Secret. 
* @param timeout Timeout time of one operation. 
* @param source Source identifier used to differentiate data from different machines. If it is empty, constructor will use machine ip as its source.
* @param compressFlag The flag decides whether compresses the data or not when put data to LOG.
* @return The objcect pointer.
*/
    LOGClient(const std::string& slsHost, const std::string& accessKeyId, const std::string& accessKey, int32_t timeout=LOG_REQUEST_TIMEOUT, const std::string& source="", bool compressFlag=true);
    LOGClient(const std::string& slsHost, const std::string& accessKeyId, const std::string& accessKey, const std::string& securityToken, int32_t timeout=LOG_REQUEST_TIMEOUT, const std::string& source="", bool compressFlag=true);
    ~LOGClient() throw();
    
    using curl_off_t = long long;
    CreateConsumerGroupResponse CreateConsumerGroup(const std::string& project , 
            const std::string& logstore, 
            const std::string& consumergroup, 
            const uint32_t timeoutInSec, 
            const bool inOrder);
    DeleteConsumerGroupResponse DeleteConsumerGroup(const std::string& project,
            const std::string& logstore,
            const std::string& consumergroup);
    ListConsumerGroupResponse ListConsumerGroup(const std::string& project,
            const std::string& logStore);
    UpdateCheckpointResponse UpdateCheckpoint(const std::string& project, const std::string& logstore, const std::string& consumergroup, const int shard, const std::string& checkpoint);
    ListCheckpointResponse ListCheckpoint(const std::string& project, const std::string& logstore, const std::string& consumergroup);
    HeartbeatResponse ConsumerGroupHeartbeat(const std::string& project, const std::string& logstore, const std::string& consumergroup, const std::string& consumer, const std::vector<uint32_t>& shards);
    /** Put data to LOG service. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param logstore The logstore of the data. 
* @param topic The topic of the data. 
* @param logGroup Log data. 
* @return request_id.
*/
    PostLogStoreLogsResponse  PostLogStoreLogs(const std::string& project , const std::string& logstore, const std::string& topic, const std::vector<LogItem>& logGroup,const std::string& hashKey="");
/** Put data to LOG service. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param logGroup Log data. 
* @return request_id.
*/
    PostLogStoreLogsResponse  PostLogStoreLogs(const std::string& project, const std::string& logstore, const sls_logs::LogGroup& logGroup,const std::string& hashKey="");
/** Get data from LOG service. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param logstore The logstore of the data. 
* @param topic The topic of the data. 
* @param beginTime Start time for the search. 
* @param endTime End time for the search. 
* @param reverseFlag Direction of results returned, false:time sequence, true:reversed time sequence. 
* @param lines Lines of results returned, if the number of all results is litter than the parameter, actual lines of results will be returned. 
* @param offset Start offset of all results, return result will be [offset, offset+lines) of them. 
* @param query Filter string to filter the results. 
* @return The data specified by the parameters.
*/
    GetLogStoreLogsResponse GetLogStoreLogs(const std::string& project, const std::string& logstore, const std::string& topic, time_t beginTime, time_t endTime, bool reverseFlag=false, int32_t lines=10, int32_t offset=0, const std::string& query="");

    /** Get the data meta info for the query. It is only used for data indexed in sls. 
     * @param project The project name
     * Unsuccessful opertaion will cause an LOGException
     * @param logstore The logstore of the data. 
     * @param topic The topic of the data. 
     * @param beginTime Start time for the search. 
     * @param endTime End time for the search. 
     * @param query Filter string to filter the results. 
     * @return The data meta by the parameters.
     */
    
    GetLogStoreHistogramResponse GetLogStoreHistogram(const std::string& project, const std::string& logstore, const std::string& topic, time_t beginTime, time_t endTime, const std::string& query="");

    Response CreateLogStore(const std::string& project, const LogStore& logStore);
    Response UpdateLogStore(const std::string& project, const LogStore& logStore);
    Response DeleteLogStore(const std::string& project, const std::string& logStore);

    GetLogStoreResponse GetLogStore(const std::string& project, const std::string& logStore);

/** List all categories of the user. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param offset The list offset
* @param size The list size
* @return All categories of the user. 
*/
    ListLogStoresResponse ListLogStores(const std::string& project, const std::string& logstorename = "", const int32_t& offset = -1, const int32_t& size = -1);
/** List all topics in a logstore. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param logstore The logstore of the topic. 
* @return All topics in a logstore. 
*/
    ListLogStoreTopicsResponse ListLogStoreTopics(const std::string& project, const std::string& logstore, const int64_t& line, const std::string& nextToken = "");

/** get cursor in a logstore. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param logstore. 
* @param shardId
* @param fromTime, unixtimestamp 
* @return cursor. 
*/
    GetCursorResponse GetCursor(const std::string& project, const std::string& logstore, uint32_t shardId, uint32_t fromTime);

/** get cursor in a logstore. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param logstore. 
* @param shardId
* @param cursorMode, "begin" or "end" 
* @return cursor. 
*/
    GetCursorResponse GetCursor(const std::string& project, const std::string& logstore, uint32_t shardId, LOG_Cursor_Mode cursorMode);

/** List all shards in a logstore. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param logstore The logstore of the topic. 
* @return All shards in a logstore. 
*/
    ListShardsResponse ListShards(const std::string& project, const std::string& logstore);

/** split a shard in logstore. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param logstore The logstore of the topic. 
* @param shardId  the shard id 
* @param middleHash mmiddle hash
* @return originShard and result two splited shard
*/
    SplitShardResponse  SplitShard(const std::string& project,const std::string& logstore,const int shardId,const std::string& middleHash);

/** merge two shard in to one shard in logstore. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param logstore The logstore of the topic. 
* @param shardId  the left shard id 
* @return origin tow shards and result merged shard
*/
    MergeShardsResponse  MergeShard(const std::string& project,const std::string& logstore,const int shardId);

/**
 * delete a readonly shard 
* @param project The project name
* @param logstore The logstore of the topic. 
* @param shardId  the left shard id 
* @return request_id.
*/
    Response DeleteShard( const std::string& project,const std::string& logstore,const int shardId);

/** get package list in a shard. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param logstore
* @param shardId
* @param count, max loggroup count returned 
* @param cursor/endCursor, [cursor, endCursor), endCursor set "" means no endCursor defined
* @return All package list in a shard 
*/
    GetBatchLogResponse GetBatchLog(const std::string& project, const std::string& logstore, uint32_t shardId, int count, const std::string& cursor, const std::string& endCursor = "");

/** get package list in a shard. Unsuccessful opertaion will cause an LOGException.
* @param project The project name
* @param logstore
* @param shardId
* @param count, max loggroup count returned 
* @param cursor/endCursor, [cursor, endCursor), endCursor set "" means no endCursor defined
* @return All pb package list in a shard 
*/
    GetPbBatchLogResponse GetPbBatchLog(const std::string& project, const std::string& logstore, uint32_t shardId, int count, const std::string& cursor, const std::string& endCursor = "");

    Response CreateIndex(const std::string& project, const std::string& logStore, const Index& index);
    Response UpdateIndex(const std::string& project, const std::string& logStore, const Index& index);
    GetIndexResponse GetIndex(const std::string& project, const std::string& logStore);
    Response DeleteIndex(const std::string& project, const std::string& logStore);

    Response CreateConfig(const std::string& project, const Config& config);
    Response UpdateConfig(const std::string& project, const Config& config);
    Response DeleteConfig(const std::string& project, const std::string& config);
    GetConfigResponse GetConfig(const std::string& project, const std::string& config);
    ListConfigsResponse ListConfigs(const std::string& project, const std::string& configName = "", const int32_t& offset = -1, const int32_t& size = -1);

    Response CreateMachineGroup(const std::string& project, const MachineGroup& machineGroup);
    Response UpdateMachineGroup(const std::string& project, const MachineGroup& machineGroup);
    Response DeleteMachineGroup(const std::string& project, const std::string& machineGroup);
    GetMachineGroupResponse GetMachineGroup(const std::string& project, const std::string& machineGroup);
    ListMachineGroupsResponse ListMachineGroups(const std::string& project, const int32_t& offset = -1, const int32_t& size = -1);
    Response ApplyConfigToMachineGroup(const std::string& project, const std::string& machineGroup, const std::string& config);
    Response RemoveConfigFromMachineGroup(const std::string& project, const std::string& machineGroup, const std::string& config);
    GetAppliedConfigsResponse GetAppliedConfigs(const std::string& project, const std::string& machineGroup);

    LogStoreSqlResponse ExecuteLogStoreSql(const std::string &project, const std::string &logstore, time_t beginTime, time_t endTime, const std::string &query, bool powerSql = false);
    ProjectSqlResponse ExecuteProjectSql(const std::string &project, const std::string &query, bool powerSql);
    CreateSqlInstanceResponse CreateSqlInstance(const std::string &project, int cu);
    UpdateSqlInstanceResponse UpdateSqlInstance(const std::string &project, int cu);
    ListSqlInstanceResponse ListSqlInstance(const std::string &project);

    void SetUserAgent(const std::string& userAgent) {mUserAgent = userAgent;}
    void SetKeyProvider(const std::string& keyProvider) {mKeyProvider = keyProvider;}

    void SetAccessKey(const std::string& accessKey);
    std::string GetAccessKey();
    void SetAccessKeyId(const std::string& accessKeyId);
    std::string GetAccessKeyId();
    void SetSlsHost(const std::string& slsHost);
    std::string GetSlsHost();
    std::string GetHostFieldSuffix();

    const std::string& GetSecurityToken()
    {
        return mSecurityToken;
    }
    void SetSecurityToken(const std::string& securityToken)
    {
        mSecurityToken = securityToken;
    }
    void RemoveSecurityToken()
    {
        SetSecurityToken("");
    }
    void SetMaxSendSpeed(const curl_off_t speed){ mMaxSendSpeedInBytePerSec = speed; }
protected:
    std::string mSlsHost;
    std::string mAccessKeyId;
    std::string mAccessKey;
    std::string mSecurityToken;
    std::string mSource;
    bool mCompressFlag;
    int32_t mTimeout;
    std::string mUserAgent;
    std::string mKeyProvider;
    std::string mHostFieldSuffix;
    bool mIsHostRawIp;
    std::mutex mMutex;
    //HttpMessage (*mLOGSend)(const std::string& url, const std::map<std::string, std::string>& header, const std::string& body, const LOG_Request_Mode requestMode, int32_t timeout); 
    std::string (*mGetDateString)();
    void (*mLOGSend)(const std::string& httpMethod, const std::string& host, const int32_t port, const std::string& url, const std::string& queryString, const std::map<std::string, std::string>& header, const std::string& body, const int32_t timeout, HttpMessage& httpMessage, const curl_off_t maxspeed); 
    curl_off_t mMaxSendSpeedInBytePerSec;
    void SendRequest(const std::string& project, const std::string& httpMethod, const std::string& url, const std::string& body, const std::map<std::string, std::string>& parameterList, std::map<std::string, std::string>& header, HttpMessage& httpMessage);
private:
    GetCursorResponse GetCursor(const std::string& project, const std::string& logstore, uint32_t shardId, const std::string& paraKey, const std::string& paraValue);
    PullDataResponse GetLogGroupList(const std::string& project, const std::string& logstore, uint32_t shardId, int count, const std::string& cursor, const std::string& endCursor, sls_logs::LogGroupList& logGroupList);
    void SetCommonHeader(std::map<std::string, std::string>& httpHeader, int32_t contentLength, const std::string& project="");
    void SetCommonParameter(std::map<std::string, std::string>& parameterList);
    std::string GetHost(const std::string& project);
};
}
#endif
