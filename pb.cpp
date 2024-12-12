#include "pb.h"

#include <cassert>
#include "coding.h"

using namespace aliyun_log_sdk_v6;
using namespace aliyun_log_sdk_v6::pb;

using KvPair = std::pair<std::string, std::string>;

#define SLS_OUT

#define IF_CONFITION_RETURN_FALSE(condition) \
    if (condition)                           \
    {                                        \
        return false;                        \
    }

#define CHECK_FIELD_TYPE(index, actual, expected) \
    if (expected != actual)                       \
    {                                             \
        return false;                             \
    }

/// for deserialize

static bool ParseFromStr(const SlsStringPiece& str, SLS_OUT KvPair& result)
{
    const char* pos = str.mPtr;
    const char* end = str.mPtr + str.mLen;
    bool hasKey = false, hasValue = false;
    while (pos < end)
    {
        uint32_t head = 0;
        pos = GetVarint32Ptr(pos, end, &head);
        IF_CONFITION_RETURN_FALSE(pos == nullptr);
        uint32_t mode = head & 0x7;
        uint32_t index = head >> 3;
        CHECK_FIELD_TYPE(index, mode, 2);
        uint32_t len = 0;
        pos = GetVarint32Ptr(pos, end, &len);
        IF_CONFITION_RETURN_FALSE(pos == nullptr || pos + len > end);

        // key = 1
        if (index == 1)
        {
            result.first = std::string(pos, len);
            pos += len;
            hasKey = true;
            continue;
        }

        // value = 2
        if (index == 2)
        {
            result.second = std::string(pos, len);
            pos += len;
            hasValue = true;
            continue;
        }

        pos = SkipProtobufField(pos, end, mode);
        IF_CONFITION_RETURN_FALSE(pos == nullptr);
    }
    return hasKey && hasValue && (pos == end);
}

static bool ParseFromStr(const SlsStringPiece& str, SLS_OUT Log& result)
{
    const char* pos = str.mPtr;
    const char* end = str.mPtr + str.mLen;
    bool hasTime = false;
    while (pos < end)
    {
        uint32_t head = 0;
        pos = GetVarint32Ptr(pos, end, &head);
        IF_CONFITION_RETURN_FALSE(pos == nullptr);
        uint32_t mode = head & 0x7;
        uint32_t index = head >> 3;
        switch (index)
        {
        case 1: // log time
        {
            CHECK_FIELD_TYPE(index, mode, 0);
            IF_CONFITION_RETURN_FALSE(hasTime);
            uint32_t data_time;
            pos = GetVarint32Ptr(pos, end, &data_time);
            IF_CONFITION_RETURN_FALSE(pos == nullptr);
            result.time = data_time;
            hasTime = true;
            break;
        }

        case 2: // log content
        {
            CHECK_FIELD_TYPE(index, mode, 2);
            uint32_t len = 0;
            pos = GetVarint32Ptr(pos, end, &len);
            IF_CONFITION_RETURN_FALSE(pos == nullptr || pos + len > end);
            KvPair pair;
            if (!ParseFromStr(SlsStringPiece(pos, len), pair))
                return false;
            result.contents.emplace_back(pair.first, pair.second);
            pos += len;
            break;
        }

        case 4: // nano time
        {
            CHECK_FIELD_TYPE(index, mode, 5);
            IF_CONFITION_RETURN_FALSE(pos + 4 > end);
            std::memcpy(&result.timeNs, pos, 4);
            result.hasTimeNs = true;
            pos += 4;
            break;
        }

        default:
        {
            pos = SkipProtobufField(pos, end, mode);
            IF_CONFITION_RETURN_FALSE(pos == nullptr);
            break;
        }
        }
    }
    return hasTime && (pos == end);
}

static bool ParseFromStr(const SlsStringPiece& str, SLS_OUT LogGroup& result)
{
    const char* pos = str.mPtr;
    const char* end = str.mPtr + str.mLen;
    while (pos < end)
    {
        uint32_t head = 0;
        pos = GetVarint32Ptr(pos, end, &head);
        IF_CONFITION_RETURN_FALSE(pos == nullptr);
        uint32_t mode = head & 0x7;
        uint32_t index = head >> 3;

        if (index != 1 && index != 3 && index != 4 && index != 6)
        {
            pos = SkipProtobufField(pos, end, mode);
            IF_CONFITION_RETURN_FALSE(pos == nullptr);
            continue;
        }

        CHECK_FIELD_TYPE(index, mode, 2);
        uint32_t len = 0;
        pos = GetVarint32Ptr(pos, end, &len);
        IF_CONFITION_RETURN_FALSE(pos == nullptr || pos + len > end);

        switch (index)
        {
        case 1: // logs
        {
            Log log;
            if (!ParseFromStr(SlsStringPiece(pos, len), log))
                return false;
            result.logs.push_back(std::move(log));
            pos += len;
            break;
        }

        case 3: // topic
        {
            result.topic = std::string(pos, len);
            pos += len;
            break;
        }

        case 4: // source
        {
            result.source = std::string(pos, len);
            pos += len;
            break;
        }

        case 6: // tags
        {
            KvPair pair;
            if (!ParseFromStr(SlsStringPiece(pos, len), pair))
                return false;
            result.logTags.emplace_back(pair.first, pair.second);
            pos += len;
            break;
        }

        default:
        {
            // unreachable
            break;
        }
        }
    }
    return pos == end;
}

static bool ParseFromStr(const SlsStringPiece& str, SLS_OUT LogGroupList& result)
{
    const char* pos = str.mPtr;
    const char* end = str.mPtr + str.mLen;
    while (pos < end)
    {
        uint32_t head = 0;
        pos = GetVarint32Ptr(pos, end, &head);
        IF_CONFITION_RETURN_FALSE(pos == nullptr);
        uint32_t mode = head & 0x7;
        uint32_t index = head >> 3;

        if (index != 1)
        {
            pos = SkipProtobufField(pos, end, mode);
            IF_CONFITION_RETURN_FALSE(pos == nullptr);
            continue;
        }

        CHECK_FIELD_TYPE(index, mode, 2);
        uint32_t len = 0;
        pos = GetVarint32Ptr(pos, end, &len);
        IF_CONFITION_RETURN_FALSE(pos == nullptr || pos + len > end);
        LogGroup logGroup;
        if (!ParseFromStr(SlsStringPiece(pos, len), logGroup))
            return false;
        result.logGroupList.push_back(std::move(logGroup));
        pos += len;
    }
    return pos == end;
}

bool PbEncoder::ParseFromString(const std::string &str, SLS_OUT LogGroupList &out)
{
    out.Clear();
    if (!ParseFromStr(SlsStringPiece(str.c_str(), str.size()), out))
    {
        out.Clear();
        return false;
    }
    return true;
}

/// for serialize

inline static size_t WithFieldHeader(size_t size)
{
    return 1 + size + GetEncodeVarint32Len(size);
}

inline static size_t PbLength(const std::string& key, const std::string& value)
{
    return WithFieldHeader(key.size()) + WithFieldHeader(value.size());
}

inline static size_t PbLength(const Log& log)
{
    size_t size = 1 + GetEncodeVarint32Len(log.time);
    for (const auto& content : log.contents)
    {
        size += WithFieldHeader(PbLength(content.key, content.value));
    }
    if (log.hasTimeNs)
    {
        size += 1 + 4;
    }
    return size;
}

inline static size_t PbLength(const LogGroup& logGroup)
{
    size_t size = 0;
    // topic
    if (!logGroup.topic.empty())
    {
        size += WithFieldHeader(logGroup.topic.size());
    }
    // source
    if (!logGroup.source.empty())
    {
        size += WithFieldHeader(logGroup.source.size());
    }
    // logs
    for (const auto& log : logGroup.logs)
    {
        size += WithFieldHeader(PbLength(log));
    }
    // tags
    for (const auto& logTag : logGroup.logTags)
    {
        size += WithFieldHeader(PbLength(logTag.key, logTag.value));
    }
    return size;
}

inline static char* WriteStrWithHead(const std::string& str, char head, SLS_OUT char* dst)
{
    *dst++ = head;
    dst = EncodeVarint32(dst, str.size());
    std::memcpy(dst, str.c_str(), str.size());
    return dst + str.size();
}

static char* WriteStrPairWithHead(char head,
                                  size_t msgSize,
                                  const std::string& key,
                                  const std::string& value,
                                  SLS_OUT char* dst)
{
    // head | EncodeVarint32(msgSize) | msg
    *dst++ = head;
    dst = EncodeVarint32(dst, msgSize);
    dst = WriteStrWithHead(key, (char)((1 << 3) | 2), dst);
    return WriteStrWithHead(value, (char)((2 << 3) | 2), dst);
}

static char* WriteLogWithHead(size_t msgSize, const Log& log, SLS_OUT char* dst)
{
    // write in order
    *dst++ = (char)((1 << 3) | 2);
    dst = EncodeVarint32(dst, msgSize);
    // time = 1
    *dst++ = (char)((1 << 3) | 0);
    dst = EncodeVarint32(dst, log.time);
    // timeNs = 4
    if (log.hasTimeNs)
    {
        *dst++ = (char)((4 << 3) | 5);
        dst = PutFixedUint32(dst, log.timeNs);
    }
    // contents = 2
    for (const auto& content : log.contents)
    {
        size_t contentMsgSize = PbLength(content.key, content.value);
        dst = WriteStrPairWithHead((char)((2 << 3) | 2), contentMsgSize, content.key, content.value, dst);
    }
    return dst;
}

// todo: cache PbLength
bool PbEncoder::SerializeToString(const LogGroup& logGroup, SLS_OUT std::string& out)
{
    size_t totalMsgSize = PbLength(logGroup);
    out.resize(totalMsgSize);

    // write in order
    char* begin = &out[0];
    char* pos = begin + totalMsgSize;

    // topic = 3
    if (!logGroup.topic.empty())
    {
        char* writeBeginPos = pos - WithFieldHeader(logGroup.topic.size());
        WriteStrWithHead(logGroup.topic, (char)((3 << 3) | 2), writeBeginPos);
        pos = writeBeginPos;
    }

    // source = 4
    if (!logGroup.source.empty())
    {
        char* writeBeginPos = pos - WithFieldHeader(logGroup.source.size());
        WriteStrWithHead(logGroup.source, (char)((4 << 3) | 2), writeBeginPos);
        pos = writeBeginPos;
    }

    // tags = 6
    for (auto it = logGroup.logTags.rbegin(); it != logGroup.logTags.rend(); ++it)
    {
        auto& logTag = *it;
        size_t msgSize = PbLength(logTag.key, logTag.value);
        char* writeBeginPos = pos - WithFieldHeader(msgSize);
        WriteStrPairWithHead((char)((6 << 3) | 2), msgSize, logTag.key, logTag.value, writeBeginPos);
        pos = writeBeginPos;
    }

    // logs = 1
    for (auto it = logGroup.logs.rbegin(); it != logGroup.logs.rend(); ++it)
    {
        auto& log = *it;
        size_t msgSize = PbLength(log);
        char* writeBeginPos = pos - WithFieldHeader(msgSize);
        WriteLogWithHead(msgSize, log, writeBeginPos);
        pos = writeBeginPos;
    }

    if (pos != begin)
    {
        out.clear();
        return false;
    }
    return true;
}

bool LogGroupList::ParseFromString(const std::string& str)
{
    return PbEncoder::ParseFromString(str, *this);
}

bool LogGroup::SerializeToString(std::string& out) const
{
    return PbEncoder::SerializeToString(*this, out);
}