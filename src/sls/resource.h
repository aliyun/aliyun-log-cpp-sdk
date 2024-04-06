/*
 *  Trade secret of Alibaba Group R&D.
 *  Copyright (c) 2010 Alibaba Group R&D. (unpublished)
 *
 *  All rights reserved.  This notice is intended as a precaution against
 *  inadvertent publication and does not imply publication or any waiver
 *  of confidentiality.  The year included in the foregoing notice is the
 *  year of creation of the work.
 *
 */

#ifndef __RESOURCE_H_V6__
#define __RESOURCE_H_V6__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <sls/rapidjson/writer.h>
#include <sls/rapidjson/document.h>

namespace aliyun_log_sdk_v6
{
class Resource
{
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const = 0;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const = 0;
    std::string ToJsonString() const;
    std::string ToRequestJsonString(const bool array = false) const;

    virtual void FromJson(const rapidjson::Value& value) = 0;
    void FromJsonString(const std::string& jsonStr);

    virtual ~Resource() {}
};

class ConsumerGroup: public Resource
{
private:
    std::string mConsumerGroupName;
    uint32_t mTimeoutInSec;
    bool mInOrder;
public:
    ConsumerGroup(const std::string& consumergroup, const uint32_t timeoutInSec, const bool inOrder):
        mConsumerGroupName(consumergroup), mTimeoutInSec(timeoutInSec), mInOrder(inOrder)
    {}
public:
    const std::string& GetConsumerGroupName() const { return mConsumerGroupName; }
    uint32_t GetTimeoutInSec() const { return mTimeoutInSec; }
    bool GetInOrder() const { return mInOrder; }
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);
};

class ConsumerGroupCheckpoint: public Resource
{
private:
    int mShard;
    std::string mCheckpoint;
    uint64_t mUpdateTime;
public:
    ConsumerGroupCheckpoint(const int shard, const std::string& checkpoint):
        mShard(shard), mCheckpoint(checkpoint)
    {}
    ConsumerGroupCheckpoint(const int shard, const std::string& checkpoint, const uint64_t updateTime):
        mShard(shard), mCheckpoint(checkpoint), mUpdateTime(updateTime)
    {}
    int GetShard() const{ return mShard; }
    const std::string& GetCheckpoint() const { return mCheckpoint; }
    uint64_t GetUpdateTime() const { return mUpdateTime; }
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);
};
class Heartbeat: public Resource
{
private:
    std::vector<uint32_t> mShards;
public:
    void Add(const uint32_t shard){ mShards.push_back(shard); }
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);
};
class LogStore : public Resource
{
private:
    std::string mLogStoreName;
    int64_t mTtl;
    int64_t mShardCount;

    int64_t mCreateTime;
    int64_t mLastModifyTime;
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    LogStore();
    LogStore(const std::string& logStoreName, const int64_t ttl, const int64_t shardCount);
    ~LogStore() {}

    void SetLogStoreName(const std::string& logStoreName) { mLogStoreName = logStoreName; }
    void SetTTL(const int64_t ttl) { mTtl = ttl; }
    void SetShardCount(const int64_t shardCount) { mShardCount = shardCount; }

    std::string GetLogStoreName() const { return mLogStoreName; }
    int64_t GetTTL() const { return mTtl; }
    int64_t GetShardCount() const { return mShardCount; }

    int64_t GetCreateTime() const { return mCreateTime; }
    int64_t GetLastModifyTime() const { return mLastModifyTime; }
};

class ConfigInputDetail : public Resource
{
private:
    std::string mLogType;
    std::string mLogPath;
    std::string mFilePattern;
    bool mLocalStorage;
    std::string mTimeFormat;
    std::string mLogBeginRegex;
    std::string mRegex;
    std::vector<std::string> mKey;
    std::vector<std::string> mFilterKey;
    std::vector<std::string> mFilterRegex;
    std::string mTopicFormat;
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    ConfigInputDetail();
    ConfigInputDetail(const std::string& logType, const std::string& logPath, const std::string& filePattern, const bool localStorage, const std::string& timeFormat, const std::string& logBeginRegex, const std::string& regex, const std::vector<std::string>& key, const std::vector<std::string>& filterKey, const std::vector<std::string>& filterRegex, const std::string& topicFormat);
    ~ConfigInputDetail() {}

    void SetLogType(const std::string& logType) { mLogType = logType; }
    void SetLogPath(const std::string& logPath) { mLogPath = logPath; }
    void SetFilePattern(const std::string& filePattern) { mFilePattern = filePattern; }
    void SetLocalStorage(const bool localStorage) { mLocalStorage = localStorage; }
    void SetTimeFormat(const std::string& timeFormat) { mTimeFormat = timeFormat; }
    void SetLogBeginRegex(const std::string& logBeginRegex) { mLogBeginRegex = logBeginRegex; }
    void SetRegex(const std::string& regex) { mRegex = regex; }
    void SetKey(const std::vector<std::string>& key) { mKey = key; }
    void SetFilterKey(const std::vector<std::string>& filterKey) { mFilterKey = filterKey; }
    void SetFilterRegex(const std::vector<std::string>& filterRegex) { mFilterRegex = filterRegex; }
    void SetTopicFormat(const std::string& topicFormat) { mTopicFormat = topicFormat; }
    
    std::string GetLogType() const { return mLogType; }
    std::string GetLogPath() const { return mLogPath; }
    std::string GetFilePattern() const { return mFilePattern; }
    bool GetLocalStorage() const { return mLocalStorage; }
    std::string GetTimeFormat() const { return mTimeFormat; }
    std::string GetLogBeginRegex() const { return mLogBeginRegex; }
    std::string GetRegex() const { return mRegex; }
    std::vector<std::string> GetKey() const { return mKey; }
    std::vector<std::string> GetFilterKey() const { return mFilterKey; }
    std::vector<std::string> GetFilterRegex() const { return mFilterRegex; }
    std::string GetTopicFormat() const  { return mTopicFormat; }
};

class ConfigOutputDetail : public Resource
{
private:
    std::string mProjectName;
    std::string mLogStoreName;
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    ConfigOutputDetail();
    ConfigOutputDetail(const std::string& projectName, const std::string& logStoreName);

    ~ConfigOutputDetail() {}

    void SetProjectName(const std::string& projectName) { mProjectName = projectName; }
    void SetLogStoreName(const std::string& logStoreName) { mLogStoreName = logStoreName; }

    std::string GetProjectName() const { return mProjectName; }
    std::string GetLogStoreName() const { return mLogStoreName; }
};

class Config : public Resource
{
private:
    std::string mConfigName;
    std::string mInputType;
    ConfigInputDetail mInputDetail;
    std::string mOutputType;
    ConfigOutputDetail mOutputDetail;

    std::string mLogSample;
    int64_t mCreateTime;
    int64_t mLastModifyTime;
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    Config();
    Config(const std::string& configName, const std::string& inputType, const ConfigInputDetail& inputDetail, const std::string& outputType, const ConfigOutputDetail& outputDetail, const std::string& logSample = "");
    ~Config() {}

    void SetConfigName(const std::string& configName) { mConfigName = configName; }
    void SetInputType(const std::string& inputType) { mInputType = inputType; }
    void SetInputDetail(const ConfigInputDetail& inputDetail) { mInputDetail = inputDetail; }
    void SetOutputType(const std::string& outputType) { mOutputType = outputType; }
    void SetOutputDetail(const ConfigOutputDetail& outputDetail) { mOutputDetail = outputDetail; }
    void SetLogSample(const std::string& logSample) { mLogSample = logSample; }

    std::string GetConfigName() const { return mConfigName; }
    std::string GetInputType() const { return mInputType; }
    ConfigInputDetail GetInputDetail() const { return mInputDetail; }
    std::string GetOutputType() const { return mOutputType; }
    ConfigOutputDetail GetOutputDetail() const { return mOutputDetail; }
    std::string GetLogSample() const { return mLogSample; }
    int64_t GetCreateTime() const { return mCreateTime; }
    int64_t GetLastModifyTime() const { return mLastModifyTime; }
};

class Line : public Resource
{
private:
    bool mIncludeKeysSet;
    bool mExcludeKeysSet;

    std::vector<std::string> mToken;
    std::vector<std::string> mIncludeKeys;
    std::vector<std::string> mExcludeKeys;
    bool mCaseSensitive;
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    Line() { mIncludeKeysSet = false; mExcludeKeysSet = false; }
    ~Line() {}

    void SetToken(const std::vector<std::string>& token) { mToken = token; }
    void SetIncludeKeys(const std::vector<std::string>& includeKeys) { mIncludeKeysSet = true; mIncludeKeys = includeKeys; }
    void SetExcludeKeys(const std::vector<std::string>& excludeKeys) { mExcludeKeysSet = true; mExcludeKeys = excludeKeys; }
    void SetCaseSensitive(const bool caseSensitive) { mCaseSensitive = caseSensitive; }

    std::vector<std::string> GetToken() const { return mToken; }
    std::vector<std::string> GetIncludeKeys() const { return mIncludeKeys; }
    std::vector<std::string> GetExcludeKeys() const { return mExcludeKeys; }
    bool GetCaseSensitive() const { return mCaseSensitive; }
};

class KeyContent : public Resource
{
private:
    std::vector<std::string> mToken;
    bool mCaseSensitive;
    std::string mAlias;
    bool mChn;
    std::string mIndexType;
    bool mDocValue;
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    KeyContent() {mChn = false;mDocValue = false;mIndexType = "text";}
    ~KeyContent() {}

    void SetToken(const std::vector<std::string>& token) { mToken = token; }
    void SetCaseSensitive(const bool caseSensitive) { mCaseSensitive = caseSensitive; }
    void SetChnToken(const bool chn) { mChn = chn; }
    void SetDocValue(const bool docValue) { mDocValue = docValue; }
    void SetIndexType(const std::string &indexType) { mIndexType = indexType; }

    std::vector<std::string> GetToken() const { return mToken; }
    bool GetCaseSensitive() const { return mCaseSensitive; }
    const std::string& GetIndexType() const { return mIndexType; }
};

class Keys : public Resource
{
private:
    std::map<std::string, std::shared_ptr<KeyContent>> mKeys;
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    Keys() {}
    ~Keys() {}

    void AddKey(const std::string &key, const std::shared_ptr<KeyContent> &keyContent);
    std::map<std::string, std::shared_ptr<KeyContent>> GetKeys() const { return mKeys; }
};
class JsonKeyContent : public KeyContent
{
private:
    bool mIndexAll;
    int mMaxDepth;
    Keys mJsonKeys;
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer> &writer) const;
    virtual void FromJson(const rapidjson::Value &value);

    JsonKeyContent() : mIndexAll(true), mMaxDepth(-1) { SetIndexType("json"); }
    ~JsonKeyContent() {}

    void SetIndexAll(const bool indexAll) { mIndexAll = indexAll; }
    void SetMaxDepth(const int maxDepth) { mMaxDepth = maxDepth; }
    void SetJsonKeys(const Keys &jsonKeys) { mJsonKeys = jsonKeys; }
};

class AllKeys : public Resource
{
private:
    std::vector<std::string> mToken;
    bool mCaseSensitive;
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    AllKeys() {}
    ~AllKeys() {}

    std::vector<std::string> GetToken() const { return mToken; }
    bool GetCaseSensitive() const { return mCaseSensitive; }
};

class Index : public Resource
{
private:
    bool mLineSet;
    bool mKeysSet;
    bool mAllKeysSet;

    uint32_t mTtl;
    AllKeys mAllKeys;
    Line mLine;
    Keys mKeys;
    bool mLogReduceEnable;
    std::vector<std::string> mLogReduceWhiteList;
    std::vector<std::string> mLogReduceBlackList;
    int mMaxTextLen;

    int64_t mLastModifyTime;
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    Index() { mLineSet = false; mKeysSet = false; mAllKeysSet = false; mLogReduceEnable = false; mMaxTextLen = 0;}
    Index(uint32_t ttl) { mLineSet = false; mKeysSet = false; mAllKeysSet = false; mTtl = ttl; mLogReduceEnable = false; mMaxTextLen = 0;}
    ~Index() {}

    AllKeys GetAllKeys() const { return mAllKeys; }
    Line GetLine() const { return mLine; }
    Keys GetKeys() const { return mKeys; }
    uint32_t GetTTL() const { return mTtl; }
    int64_t GetLastModifyTime() const { return mLastModifyTime; }


    void SetTTL(const uint32_t ttl) { mTtl = ttl; }
    void SetLine(const Line& line) { mLineSet = true; mLine = line; }
    void SetKeys(const Keys& keys) { mKeysSet = true; mKeys = keys; }
    void SetLogReduceEnable(const bool logReduceEnalbe) { mLogReduceEnable = logReduceEnalbe; }
    void SetLogReduceWhiteList(const std::vector<std::string> &logReduceWhiteList) { mLogReduceWhiteList = logReduceWhiteList; }
    void SetLogReduceBlackList(const std::vector<std::string> &logReduceBlackList) { mLogReduceBlackList = logReduceBlackList; }
};

typedef enum ACLPrivilege
{
    ACL_WRITE = 0,
    ACL_ADMIN,
    ACL_READ,
    ACL_LIST,
    ACL_OWNER
}ACLPrivilege;

class ACL : public Resource
{
private:
    std::string mAction;
    std::string mPrinciple;
    std::set<std::string> mPrivilege;

    int64_t mCreateTime;
    int64_t mLastModifyTime;

    void PrivilegeToString(const ACLPrivilege& privilegeValue, std::string& privilege);
public:
    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    ACL();
    ~ACL() {}

    void SetAction(const std::string& action) { mAction = action; }
    void SetPrinciple(const std::string& principle) { mPrinciple = principle; }
    void AddPrivilege(const ACLPrivilege& privilege);
    void RemovePrivilege(const ACLPrivilege& privilege);

    std::string GetAction() const { return mAction; }
    std::string GetPrinciple() const { return mPrinciple; }
    std::vector<std::string> GetPrivilege() const;

    int64_t GetCreateTime() const { return mCreateTime; }
    int64_t GetLastModifyTime() const { return mLastModifyTime; }
};

class GroupAttribute : public Resource
{
private:
    std::string mExternalName;
    std::string mGroupTopic;
public:
    GroupAttribute();
    GroupAttribute(const std::string& externalName, const std::string& groupTopic);
    ~GroupAttribute() {}

    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    void SetExternalName(const std::string& externalName) { mExternalName = externalName; }
    void SetGroupTopic(const std::string& groupTopic) { mGroupTopic = groupTopic; }

    std::string GetExternalName() const { return mExternalName; }
    std::string GetGroupTopic() const { return mGroupTopic; }
};

class MachineGroup : public Resource
{
private:
    std::string mGroupName;
    std::string mGroupType;
    GroupAttribute mGroupAttribute;
    std::string mMachineIdentifyType;
    std::vector<std::string> mMachineList;

    int64_t mCreateTime;
    int64_t mLastModifyTime;

public:
    MachineGroup();
    MachineGroup(const std::string& groupName, const std::string& machineIdentifyType, const std::vector<std::string>& machineList);
    ~MachineGroup() {}

    virtual void SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
    virtual void FromJson(const rapidjson::Value& value);

    void SetGroupName(const std::string& groupName) { mGroupName = groupName; }
    void SetGroupType(const std::string& groupType) { mGroupType = groupType; }
    void SetGroupAttribute(const GroupAttribute& groupAttribute) { mGroupAttribute = groupAttribute; }
    void SetMachineIdentifyType(const std::string& machineIdentifyType) { mMachineIdentifyType = machineIdentifyType; }
    void SetMachineList(const std::vector<std::string>& machineList) { mMachineList = machineList; }

    std::string GetGroupName() const { return mGroupName; }
    std::string GetGroupType() const { return mGroupType; }
    GroupAttribute GetGroupAttribute() const { return mGroupAttribute; }
    std::string GetMachineIdentifyType() const { return mMachineIdentifyType; }
    std::vector<std::string> GetMachineList() const { return mMachineList; }
    int64_t GetCreateTime() const { return mCreateTime; }
    int64_t GetLastModifyTime() const { return mLastModifyTime; }
};

}
#endif
