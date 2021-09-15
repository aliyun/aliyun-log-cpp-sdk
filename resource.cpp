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

#include "resource.h"
#include "client.h"
#include <iostream>

using namespace std;
using namespace rapidjson;

namespace aliyun_log_sdk_v6
{
/***************Resource**************/
string Resource::ToJsonString() const
{
    StringBuffer stringBuffer;
    Writer<StringBuffer> writer(stringBuffer);

    writer.StartObject();
    SetFullValues(writer); 
    writer.EndObject();

    return stringBuffer.GetString();
}

string Resource::ToRequestJsonString(const bool array) const
{
    StringBuffer stringBuffer;
    Writer<StringBuffer> writer(stringBuffer);
    if(!array) writer.StartObject();
    SetRequestValues(writer); 
    if(!array) writer.EndObject();

    return stringBuffer.GetString();
}

void Resource::FromJsonString(const string& jsonStr)
{
    Document document;
    ExtractJsonResult(jsonStr, document);
    FromJson(document);
}

/***************LogStore**************/
LogStore::LogStore():
    mLogStoreName(""),
    mTtl(-1),
    mShardCount(-1),
    mCreateTime(-1),
    mLastModifyTime(-1)
{
}

LogStore::LogStore(const string& logStoreName, const int64_t ttl, const int64_t shardCount):
    mLogStoreName(logStoreName),
    mTtl(ttl),
    mShardCount(shardCount),
    mCreateTime(-1),
    mLastModifyTime(-1)
{
}
void ConsumerGroup::SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    writer.Key("consumerGroup");
    writer.String(mConsumerGroupName.c_str());
    writer.Key("timeout");
    writer.Uint(mTimeoutInSec);
    writer.Key("order");
    writer.Bool(mInOrder);
}
void ConsumerGroup::SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    SetRequestValues(writer);
}
void ConsumerGroup::FromJson(const rapidjson::Value& value)
{
    ExtractJsonResult(value, "consumerGroup", mConsumerGroupName);
    ExtractJsonResult(value, "timeout", mTimeoutInSec);
    ExtractJsonResult(value, "order", mInOrder);
}
void ConsumerGroupCheckpoint::SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    writer.Key("shard");
    writer.Uint(mShard);
    writer.Key("checkpoint");
    writer.String(mCheckpoint.c_str());
}
void ConsumerGroupCheckpoint::SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    SetRequestValues(writer);
}
void ConsumerGroupCheckpoint::FromJson(const rapidjson::Value& value)
{
    ExtractJsonResult(value, "shard", mShard);
    ExtractJsonResult(value, "checkpoint", mCheckpoint);
    ExtractJsonResult(value, "updateTime", mUpdateTime);
}
void Heartbeat::SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    writer.StartArray();
    for(std::vector<uint32_t>::const_iterator it = mShards.begin(); it != mShards.end(); ++it)
        writer.Uint(*it);
    writer.EndArray();
}
void Heartbeat::SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    SetRequestValues(writer);
}
void Heartbeat::FromJson(const rapidjson::Value& value)
{
}

void LogStore::FromJson(const Value& value)
{
    ExtractJsonResult(value, "logstoreName", mLogStoreName);
    ExtractJsonResult(value, "ttl", mTtl);
    ExtractJsonResult(value, "shardCount", mShardCount);
    if (value.HasMember("createTime"))
    {
        ExtractJsonResult(value, "createTime", mCreateTime);
    }
    if (value.HasMember("lastModifyTime"))
    {
        ExtractJsonResult(value, "lastModifyTime", mLastModifyTime);
    }
}

void LogStore::SetRequestValues(Writer<StringBuffer>& writer) const
{
    writer.Key("logstoreName");
    writer.String(mLogStoreName.c_str());
        
    writer.Key("ttl");
    writer.Int64(mTtl);
        
    writer.Key("shardCount");
    writer.Int64(mShardCount);    
}

void LogStore::SetFullValues(Writer<StringBuffer>& writer) const
{
    SetRequestValues(writer);

    writer.Key("createTime");
    writer.Int64(mCreateTime);
        
    writer.Key("lastModifyTime");
    writer.Int64(mLastModifyTime);
}

/*****************ConfigInputDetail******************/

ConfigInputDetail::ConfigInputDetail():
    mLogType(""),
    mLogPath(""),
    mFilePattern(""),
    mLocalStorage(false),
    mTimeFormat(""),
    mLogBeginRegex(""),
    mRegex(""),
    mTopicFormat("")
{
}

ConfigInputDetail::ConfigInputDetail(const string& logType, const string& logPath, const string& filePattern, const bool localStorage, const string& timeFormat, const string& logBeginRegex, const string& regex, const vector<string>& key, const vector<string>& filterKey, const vector<string>& filterRegex, const string& topicFormat):
    mLogType(logType),
    mLogPath(logPath),
    mFilePattern(filePattern),
    mLocalStorage(localStorage),
    mTimeFormat(timeFormat),
    mLogBeginRegex(logBeginRegex),
    mRegex(regex),
    mKey(key),
    mFilterKey(filterKey),
    mFilterRegex(filterRegex),
    mTopicFormat(topicFormat)
{
}

void ConfigInputDetail::FromJson(const Value& value)
{
    ExtractJsonResult(value, "logType", mLogType);
    ExtractJsonResult(value, "logPath", mLogPath);
    ExtractJsonResult(value, "filePattern", mFilePattern);
    ExtractJsonResult(value, "localStorage", mLocalStorage);
    ExtractJsonResult(value, "timeFormat", mTimeFormat);
    ExtractJsonResult(value, "logBeginRegex", mLogBeginRegex);
    ExtractJsonResult(value, "regex", mRegex);

    const Value& key = GetJsonValue(value, "key");
    for (Value::ConstValueIterator itr = key.Begin(); itr != key.End(); ++itr)
    {
        if (itr->IsString())
        {
            mKey.push_back(itr->GetString());
        }
        else
        {
            throw JsonException("ValueTypeException", "One value of member key is not string type");
        }
    }

    const Value& filterKey = GetJsonValue(value, "filterKey");
    for (Value::ConstValueIterator itr = filterKey.Begin(); itr != filterKey.End(); ++itr)
    {
        if (itr->IsString())
        {
            mFilterKey.push_back(itr->GetString());
        }
        else
        {
            throw JsonException("ValueTypeException", "One value of member filterKey is not string type");
        }
    }

    const Value& filterRegex = GetJsonValue(value, "filterRegex");
    for (Value::ConstValueIterator itr = filterRegex.Begin(); itr != filterRegex.End(); ++itr)
    {
        if (itr->IsString())
        {
            mFilterRegex.push_back(itr->GetString());
        }
        else
        {
            throw JsonException("ValueTypeException", "One value of member key is not string type");
        }
    }

    ExtractJsonResult(value, "topicFormat", mTopicFormat);
}

void ConfigInputDetail::SetRequestValues(Writer<StringBuffer>& writer) const
{
    writer.Key("logType");
    writer.String(mLogType.c_str());
        
    writer.Key("logPath");
    writer.String(mLogPath.c_str());
        
    writer.Key("filePattern");
    writer.String(mFilePattern.c_str());
        
    writer.Key("localStorage");
    writer.Bool(mLocalStorage);
        
    writer.Key("timeFormat");
    writer.String(mTimeFormat.c_str());
        
    writer.Key("logBeginRegex");
    writer.String(mLogBeginRegex.c_str());
        
    writer.Key("regex");
    writer.String(mRegex.c_str());
        
    writer.Key("key");
    writer.StartArray();
    for (vector<string>::size_type i = 0;i < mKey.size();++i)
    {
        writer.String(mKey[i].c_str());
    }
    writer.EndArray();
        
    writer.Key("filterKey");
    writer.StartArray();
    for (vector<string>::size_type i = 0;i < mFilterKey.size();++i)
    {
        writer.String(mFilterKey[i].c_str());
    }
    writer.EndArray();
        
    writer.Key("filterRegex");
    writer.StartArray();
    for (vector<string>::size_type i = 0;i < mFilterRegex.size();++i)
    {
        writer.String(mFilterRegex[i].c_str());
    }
    writer.EndArray();
        
    writer.Key("topicFormat");
    writer.String(mTopicFormat.c_str());
}

void ConfigInputDetail::SetFullValues(Writer<StringBuffer>& writer) const
{
    SetRequestValues(writer);
}

/***************ConfigOutputDetail****************/
ConfigOutputDetail::ConfigOutputDetail():
    mProjectName(""),
    mLogStoreName("")
{
}

ConfigOutputDetail::ConfigOutputDetail(const string& projectName, const string& logStoreName):
    mProjectName(projectName),
    mLogStoreName(logStoreName)
{
}

void ConfigOutputDetail::FromJson(const Value& value)
{
    ExtractJsonResult(value, "projectName", mProjectName);
    ExtractJsonResult(value, "logstoreName", mLogStoreName);
}


void ConfigOutputDetail::SetRequestValues(Writer<StringBuffer>& writer) const
{
    writer.Key("projectName");
    writer.String(mProjectName.c_str());
        
    writer.Key("logstoreName");
    writer.String(mLogStoreName.c_str());
}

void ConfigOutputDetail::SetFullValues(Writer<StringBuffer>& writer) const
{
    SetRequestValues(writer);
}

/**************Config*************/
Config::Config():
    mConfigName(""),
    mInputType(""),
    mOutputType(""),
    mLogSample(""),
    mCreateTime(-1),
    mLastModifyTime(-1)
{
}

Config::Config(const string& configName, const string& inputType, const ConfigInputDetail& inputDetail, const string& outputType, const ConfigOutputDetail& outputDetail, const string& logSample):
    mConfigName(configName),
    mInputType(inputType),
    mInputDetail(inputDetail),
    mOutputType(outputType),
    mOutputDetail(outputDetail),
    mLogSample(logSample),
    mCreateTime(-1),
    mLastModifyTime(-1)
{
}

void Config::FromJson(const Value& value)
{
    ExtractJsonResult(value, "configName", mConfigName);
    ExtractJsonResult(value, "inputType", mInputType);
    const Value& inputDetail = GetJsonValue(value, "inputDetail");
    mInputDetail.FromJson(inputDetail);
    ExtractJsonResult(value, "outputType", mOutputType);
    const Value& outputDetail = GetJsonValue(value, "outputDetail");
    mOutputDetail.FromJson(outputDetail);
    if (value.HasMember("logSample"))
    {
        ExtractJsonResult(value, "logSample", mLogSample);
    }
    if (value.HasMember("createTime"))
    {
        ExtractJsonResult(value, "createTime", mCreateTime);
    }
    if (value.HasMember("lastModifyTime"))
    {
        ExtractJsonResult(value, "lastModifyTime", mLastModifyTime);
    }
}

void Config::SetRequestValues(Writer<StringBuffer>& writer) const
{
    writer.Key("configName");
    writer.String(mConfigName.c_str());
        
    writer.Key("inputType");
    writer.String(mInputType.c_str());
       
    writer.Key("inputDetail");
    writer.StartObject();
    mInputDetail.SetRequestValues(writer);
    writer.EndObject();
        
    writer.Key("outputType");
    writer.String(mOutputType.c_str());
       
    writer.Key("outputDetail");
    writer.StartObject();
    mOutputDetail.SetRequestValues(writer);
    writer.EndObject();

    writer.Key("logSample");
    writer.String(mLogSample.c_str());
}

void Config::SetFullValues(Writer<StringBuffer>& writer) const
{
    SetRequestValues(writer);

    writer.Key("createTime");
    writer.Int64(mCreateTime);
        
    writer.Key("lastModifyTime");
    writer.Int64(mLastModifyTime);
}

/**************Index*************/
void Line::SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    writer.Key("token");
    writer.StartArray();
    for (vector<string>::size_type i = 0;i < mToken.size();i++)
    {
        writer.String(mToken[i].c_str());
    }
    writer.EndArray();

    if (mIncludeKeysSet)
    {
        writer.Key("include_keys");
        writer.StartArray();
        for (vector<string>::size_type i = 0;i < mIncludeKeys.size();i++)
        {
            writer.String(mIncludeKeys[i].c_str());
        }
        writer.EndArray();
    }

    if (mExcludeKeysSet)
    {
        writer.Key("exclude_keys");
        writer.StartArray();
        for (vector<string>::size_type i = 0;i < mExcludeKeys.size();i++)
        {
            writer.String(mExcludeKeys[i].c_str());
        }
        writer.EndArray();
    }

    writer.Key("caseSensitive");
    writer.Bool(mCaseSensitive);
}

void Line::SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    SetRequestValues(writer);
}

void Line::FromJson(const rapidjson::Value& value)
{
    const Value& token = GetJsonValue(value, "token");
    for (Value::ConstValueIterator itr = token.Begin(); itr != token.End(); ++itr)
    {
        if (itr->IsString())
        {
           mToken.push_back(itr->GetString());
        }
        else
        {
           throw JsonException("ValueTypeException", "One value of member token is not string type");
        }
    }

    if (value.HasMember("include_keys"))
    {
        mIncludeKeysSet = true;
        const Value& includeKeys = GetJsonValue(value, "include_keys");
        for (Value::ConstValueIterator itr = includeKeys.Begin(); itr != includeKeys.End(); ++itr)
        {
            if (itr->IsString())
            {
                mIncludeKeys.push_back(itr->GetString());
            }
            else
            {
                throw JsonException("ValueTypeException", "One value of member includeKeys is not string type");
            }
        }
    }
    
    if (value.HasMember("exclude_keys"))
    {
        mExcludeKeysSet = true;
        const Value& excludeKeys = GetJsonValue(value, "exclude_keys");
        for (Value::ConstValueIterator itr = excludeKeys.Begin(); itr != excludeKeys.End(); ++itr)
        {
            if (itr->IsString())
            {
                mExcludeKeys.push_back(itr->GetString());
            }
            else
            {
                throw JsonException("ValueTypeException", "One value of member excludeKeys is not string type");
            }
        }
    }

    ExtractJsonResult(value, "caseSensitive", mCaseSensitive);
}

void KeyContent::SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    writer.Key("type");
    writer.String(mIndexType.c_str());
    if (mIndexType == "text")
    {
        writer.Key("token");
        writer.StartArray();
        for (vector<string>::size_type i = 0; i < mToken.size(); i++)
        {
            writer.String(mToken[i].c_str());
        }
        writer.EndArray();

        writer.Key("caseSensitive");
        writer.Bool(mCaseSensitive);
        writer.Key("chn");
        writer.Bool(mChn);
    }

    if (!mAlias.empty())
    {
        writer.Key("alias");
        writer.String(mAlias.c_str());
    }

    if (mDocValue)
    {
        writer.Key("doc_value");
        writer.Bool(mDocValue);
    }
}

void KeyContent::SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    SetRequestValues(writer);
}

void KeyContent::FromJson(const rapidjson::Value& value)
{
    const Value &token = GetJsonValue(value, "token");
    for (Value::ConstValueIterator itr = token.Begin(); itr != token.End(); ++itr)
    {
        if (itr->IsString())
        {
            //cout << "key's token:" << itr->GetString() << endl;
            mToken.push_back(itr->GetString());
        }
        else
        {
            throw JsonException("ValueTypeException", "One value of member token is not string type");
        }
    }

    ExtractJsonResult(value, "caseSensitive", mCaseSensitive);

    if (value.HasMember("chn"))
    {
        ExtractJsonResult(value, "chn", mChn);
    }
    if (value.HasMember("alias"))
    {
        ExtractJsonResult(value, "alias", mAlias);
    }
    if (value.HasMember("type"))
    {
        ExtractJsonResult(value, "type", mIndexType);
    }
    if (value.HasMember("doc_value"))
    {
        ExtractJsonResult(value, "doc_value", mDocValue);
    }
}

void Keys::SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    for (map<string, std::shared_ptr<KeyContent>>::const_iterator iter = mKeys.begin(); iter != mKeys.end(); ++iter)
    {
        writer.Key(iter->first.c_str());
        writer.StartObject();
        iter->second->SetRequestValues(writer);
        writer.EndObject();
    }
}

void Keys::SetFullValues(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
    SetRequestValues(writer);
}

void Keys::FromJson(const rapidjson::Value& value)
{
    for (rapidjson::Value::ConstMemberIterator mItr = value.MemberBegin(); mItr != value.MemberEnd(); ++mItr)
    {
        std::shared_ptr<KeyContent> key;
        const rapidjson::Value &v = mItr->value;
        if (v.HasMember("type") && v["type"].IsString() && std::string(v["type"].GetString()) == "json")
        {
            key.reset(new JsonKeyContent());
        }
        else
        {
            key.reset(new KeyContent());
        }
        key->FromJson(mItr->value);
        //cout << "KeyName:" << mItr->name.GetString() << endl;
        mKeys.insert(map<string, std::shared_ptr<KeyContent>>::value_type(mItr->name.GetString(), key));
    }
}

void Keys::AddKey(const string &key, const std::shared_ptr<KeyContent> &keyContent)
{
    mKeys.insert(map<string, std::shared_ptr<KeyContent>>::value_type(key, keyContent));
}

void JsonKeyContent::SetRequestValues(rapidjson::Writer<rapidjson::StringBuffer> &writer) const
{
    KeyContent::SetRequestValues(writer);
    writer.Key("index_all");
    writer.Bool(mIndexAll);
    writer.Key("max_depth");
    writer.Int(mMaxDepth);
    writer.Key("json_keys");
    writer.StartObject();
    mJsonKeys.SetRequestValues(writer);
    writer.EndObject();
}

void JsonKeyContent::FromJson(const rapidjson::Value &value)
{
    KeyContent::FromJson(value);
    if (value.HasMember("index_all"))
    {
        ExtractJsonResult(value, "index_all", mIndexAll);
    }
    if (value.HasMember("max_depth"))
    {
        ExtractJsonResult(value, "max_depth", mMaxDepth);
    }
    if (value.HasMember("json_keys"))
    {
        mJsonKeys.FromJson(value["json_keys"]);
    }
}

void AllKeys::SetRequestValues(Writer<StringBuffer>& writer) const
{
    writer.Key("token");
    writer.StartArray();
    for (vector<string>::size_type i = 0;i < mToken.size();i++)
    {
        writer.String(mToken[i].c_str());
    }
    writer.EndArray();

    writer.Key("caseSensitive");
    writer.Bool(mCaseSensitive);
}

void AllKeys::SetFullValues(Writer<StringBuffer>& writer) const
{
    SetRequestValues(writer);
}

void AllKeys::FromJson(const Value& value)
{
    const Value& token = GetJsonValue(value, "token");
    for (Value::ConstValueIterator itr = token.Begin(); itr != token.End(); ++itr)
    {
        if (itr->IsString())
        {
            mToken.push_back(itr->GetString());
        }
        else
        {
            throw JsonException("ValueTypeException", "One value of member token is not string type");
        }
    }

    ExtractJsonResult(value, "caseSensitive", mCaseSensitive);
}

void Index::SetRequestValues(Writer<StringBuffer>& writer) const
{
    writer.Key("ttl");
    writer.Uint(mTtl);
    if (mLogReduceEnable)
    {
        writer.Key("log_reduce");
        writer.Bool(mLogReduceEnable);
    }
    if (mLineSet)
    {
        writer.Key("line");
        writer.StartObject();
        mLine.SetRequestValues(writer);
        writer.EndObject();
    }
    if (mKeysSet)
    {
        writer.Key("keys");
        writer.StartObject();
        mKeys.SetRequestValues(writer);
        writer.EndObject();
    }
    if (mMaxTextLen > 0)
    {
        writer.Key("max_text_len");
        writer.Uint(mMaxTextLen);
    }
    if (!mLogReduceWhiteList.empty())
    {
        writer.Key("log_reduce_white_list");
        writer.StartArray();
        for (vector<string>::const_iterator iter = mLogReduceWhiteList.begin(); iter != mLogReduceWhiteList.end(); iter++)
        {
            writer.String((*iter).c_str());
        }
        writer.EndArray();
    }
    if (!mLogReduceBlackList.empty())
    {
        writer.Key("log_reduce_black_list");
        writer.StartArray();
        for (vector<string>::const_iterator iter = mLogReduceBlackList.begin(); iter != mLogReduceBlackList.end(); iter++)
        {
            writer.String((*iter).c_str());
        }
        writer.EndArray();
    }
}

void Index::SetFullValues(Writer<StringBuffer>& writer) const
{
    SetRequestValues(writer);
    if (mAllKeysSet)
    {
        writer.Key("all_keys");
        writer.StartObject();
        mAllKeys.SetRequestValues(writer);
        writer.EndObject();
    }
    writer.Key("lastModifyTime");
    writer.Uint64(mLastModifyTime);
}

void Index::FromJson(const Value& value)
{
    ExtractJsonResult(value, "ttl", mTtl);

    if (value.HasMember("line"))
    {
        mLineSet = true;
        const Value& line = GetJsonValue(value, "line");
        mLine.FromJson(line);
    }
    if (value.HasMember("keys"))
    {
        mKeysSet = true;
        const Value& keys = GetJsonValue(value, "keys");
        mKeys.FromJson(keys);
    }
    if (value.HasMember("all_keys"))
    {
        mAllKeysSet = true;
        const Value& allKeys = GetJsonValue(value, "all_keys");
        mAllKeys.FromJson(allKeys);
    }
    if (value.HasMember("lastModifyTime"))
    {
        ExtractJsonResult(value, "lastModifyTime", mLastModifyTime);
    }
    if (value.HasMember("log_reduce"))
    {
        ExtractJsonResult(value, "log_reduce", mLogReduceEnable);
    }
    if (value.HasMember("max_text_len"))
    {
        ExtractJsonResult(value, "max_text_len", mMaxTextLen);
    }
    if (value.HasMember("log_reduce_white_list"))
    {
        const Value &logReduceWhiteList = GetJsonValue(value, "log_reduce_white_list");
        for (Value::ConstValueIterator itr = logReduceWhiteList.Begin(); itr != logReduceWhiteList.End(); ++itr)
        {
            if (itr->IsString())
            {
                mLogReduceWhiteList.push_back(itr->GetString());
            }
            else
            {
                throw JsonException("ValueTypeException", "One value of member logReduceWhiteList is not string type");
            }
        }
    }
    if (value.HasMember("log_reduce_black_list"))
    {
        const Value &logReduceBlackList = GetJsonValue(value, "log_reduce_black_list");
        for (Value::ConstValueIterator itr = logReduceBlackList.Begin(); itr != logReduceBlackList.End(); ++itr)
        {
            if (itr->IsString())
            {
                mLogReduceBlackList.push_back(itr->GetString());
            }
            else
            {
                throw JsonException("ValueTypeException", "One value of member logReduceBlackList is not string type");
            }
        }
    }
}

/***************ACL********************/
void ACL::PrivilegeToString(const ACLPrivilege& privilegeValue, string& privilege)
{
    switch(privilegeValue)
    {
    case ACL_ADMIN:
        privilege = "ADMIN";
        break;
    case ACL_WRITE:
        privilege = "WRITE";
        break;
    case ACL_READ:
        privilege = "READ";
        break;
    case ACL_LIST:
        privilege = "LIST";
        break;
    case ACL_OWNER:
        privilege = "OWNER";
        break;
    }
}

ACL::ACL():
    mAction(""),
    mPrinciple(""),
    mCreateTime(-1),
    mLastModifyTime(-1)
{
}

void ACL::AddPrivilege(const ACLPrivilege& privilege)
{
    if (privilege != ACL_OWNER)
    {
        string privilegeStr;
        PrivilegeToString(privilege, privilegeStr);
        mPrivilege.insert(privilegeStr);
    }
}

void ACL::RemovePrivilege(const ACLPrivilege& privilege)
{
    string privilegeStr;
    PrivilegeToString(privilege, privilegeStr);
    mPrivilege.erase(privilegeStr);
}

vector<string> ACL::GetPrivilege() const
{
    vector<string> privilege;
    for (set<string>::const_iterator iter = mPrivilege.begin();iter != mPrivilege.end();iter++)
    {
        privilege.push_back(*iter);
    }

    return privilege;
}

void ACL::SetRequestValues(Writer<StringBuffer>& writer) const
{
    writer.Key("action");
    writer.String(mAction.c_str());
    
    writer.Key("principle");
    writer.String(mPrinciple.c_str());

    writer.Key("privilege");
    writer.StartArray();
    for (set<string>::const_iterator iter = mPrivilege.begin();iter != mPrivilege.end();iter++)
    {
        writer.String((*iter).c_str());
    }
    writer.EndArray();
}

void ACL::SetFullValues(Writer<StringBuffer>& writer) const
{
    writer.Key("principle");
    writer.String(mPrinciple.c_str());

    writer.Key("privilege");
    writer.StartArray();
    for (set<string>::const_iterator iter = mPrivilege.begin();iter != mPrivilege.end();iter++)
    {
        writer.String((*iter).c_str());
    }
    writer.EndArray();

    writer.Key("createTime");
    writer.Int64(mCreateTime);

    writer.Key("lastModifyTime");
    writer.Int64(mLastModifyTime);
}

void ACL::FromJson(const Value& value)
{
    if (value.HasMember("action"))
    {
        ExtractJsonResult(value, "action", mAction);
    }
    ExtractJsonResult(value, "principle", mPrinciple);

    const Value& privilege = GetJsonValue(value, "privilege");
    for (Value::ConstValueIterator itr = privilege.Begin(); itr != privilege.End(); ++itr)
    {
        if (itr->IsString())
        {
            mPrivilege.insert(itr->GetString());
        }
        else
        {
            throw JsonException("ValueTypeException", "One value of member privilege is not string type");
        }
    }

    if (value.HasMember("createTime"))
    {
        ExtractJsonResult(value, "createTime", mCreateTime);
    }
    if (value.HasMember("lastModifyTime"))
    {
        ExtractJsonResult(value, "lastModifyTime", mLastModifyTime);
    }
}

GroupAttribute::GroupAttribute() :
    mExternalName(""),
    mGroupTopic("")
{
}

GroupAttribute::GroupAttribute(const string& externalName, const string& groupTopic) :
    mExternalName(externalName),
    mGroupTopic(groupTopic)
{
}

void GroupAttribute::FromJson(const Value& value)
{
    ExtractJsonResult(value, "externalName", mExternalName);
    ExtractJsonResult(value, "groupTopic", mGroupTopic);
}

void GroupAttribute::SetRequestValues(Writer<StringBuffer>& writer) const
{
    writer.Key("externalName");
    writer.String(mExternalName.c_str());
        
    writer.Key("groupTopic");
    writer.String(mGroupTopic.c_str());
}

void GroupAttribute::SetFullValues(Writer<StringBuffer>& writer) const
{
    SetRequestValues(writer);
}


MachineGroup::MachineGroup() :
    mGroupName(""),
    mGroupType(""),
    mMachineIdentifyType(""),
    mCreateTime(-1),
    mLastModifyTime(-1)
{
}

MachineGroup::MachineGroup(const string& groupName, const string& machineIdentifyType, const vector<string>& machineList) :
    mGroupName(groupName),
    mGroupType(""),
    mMachineIdentifyType(machineIdentifyType),
    mMachineList(machineList),
    mCreateTime(-1),
    mLastModifyTime(-1)
{
}

void MachineGroup::SetRequestValues(Writer<StringBuffer>& writer) const
{
    writer.Key("groupName");
    writer.String(mGroupName.c_str());

    writer.Key("groupType");
    writer.String(mGroupType.c_str());

    writer.Key("groupAttribute");
    writer.StartObject();
    mGroupAttribute.SetRequestValues(writer);
    writer.EndObject();

    writer.Key("machineIdentifyType");
    writer.String(mMachineIdentifyType.c_str());

    writer.Key("machineList");
    writer.StartArray();
    for (vector<string>::size_type i = 0;i < mMachineList.size();i++)
    {
        writer.String(mMachineList[i].c_str());
    }
    writer.EndArray();
}

void MachineGroup::SetFullValues(Writer<StringBuffer>& writer) const
{
    SetRequestValues(writer);

    writer.Key("createTime");
    writer.Int64(mCreateTime);

    writer.Key("lastModifyTime");
    writer.Int64(mLastModifyTime);
}

void MachineGroup::FromJson(const Value& value)
{
    ExtractJsonResult(value, "groupName", mGroupName);
    ExtractJsonResult(value, "groupType", mGroupType);
    
    const Value& groupAttribute = GetJsonValue(value, "groupAttribute");
    mGroupAttribute.FromJson(groupAttribute);
    
    ExtractJsonResult(value, "machineIdentifyType", mMachineIdentifyType);

    const Value& machineList = GetJsonValue(value, "machineList");
    for (Value::ConstValueIterator itr = machineList.Begin(); itr != machineList.End(); ++itr)
    {
        if (itr->IsString())
        {
            mMachineList.push_back(itr->GetString());
        }
        else
        {
            throw JsonException("ValueTypeException", "One value of member machineList is not string type");
        }
    }

    if (value.HasMember("createTime"))
    {
        ExtractJsonResult(value, "createTime", mCreateTime);
    }
    if (value.HasMember("lastModifyTime"))
    {
        ExtractJsonResult(value, "lastModifyTime", mLastModifyTime);
    }
}

}
