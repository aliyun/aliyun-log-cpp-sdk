#pragma once

#include <string>
#include <vector>

namespace aliyun_log_sdk_v6
{
namespace pb
{

struct LogContent
{
    std::string key;
    std::string value;

    LogContent() {}
    LogContent(const std::string& key, const std::string& value) : key(key), value(value) {}
};
struct LogTag
{
    std::string key;
    std::string value;

    LogTag() {}
    LogTag(const std::string& key, const std::string& value) : key(key), value(value) {}
};
struct Log
{
    uint32_t time;
    std::vector<LogContent> contents;
    bool hasTimeNs;
    uint32_t timeNs;
    Log() : time(0), hasTimeNs(false), timeNs(0) {}
    Log(uint32_t time, const std::vector<LogContent>& contents)
        : time(time),
          contents(contents),
          hasTimeNs(false),
          timeNs(0)
    {
    }
    Log(uint32_t time, const std::vector<LogContent>& contents, uint32_t timeNs)
        : time(time),
          contents(contents),
          hasTimeNs(true),
          timeNs(timeNs)
    {
    }
};

struct LogGroup
{
    std::vector<Log> logs;
    std::vector<LogTag> logTags;
    std::string topic;
    std::string source;
    LogGroup() {}
    LogGroup(const std::vector<Log>& logs,
             const std::vector<LogTag>& logTags,
             const std::string& topic,
             const std::string& source)
        : logs(logs),
          logTags(logTags),
          topic(topic),
          source(source)
    {
    }
    bool SerializeToString(std::string& out) const;
};

struct LogGroupList
{
    std::vector<LogGroup> logGroupList;
    bool ParseFromString(const std::string& str);
    void Clear()
    {
        logGroupList.clear();
    }
};

class PbEncoder
{
public:
    static bool SerializeToString(const LogGroup& logGroup, std::string& out);
    static bool ParseFromString(const std::string& str, LogGroupList& out);
};

} // namespace pb
} // namespace aliyun_log_sdk_v6