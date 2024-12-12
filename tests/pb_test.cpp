#include "pb.h"
#include "coding.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;
using namespace aliyun_log_sdk_v6;
using namespace aliyun_log_sdk_v6::pb;

static std::string toHexSpaced(const std::string& data)
{
    stringstream ss;
    for (unsigned char c : data)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    ss << std::dec;
    return ss.str();
}

static std::string toHex(const std::string& data)
{
    stringstream ss;
    for (unsigned char c : data)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return ss.str();
}

static std::string fromHex(const std::string& data)
{
    std::string result;
    for (size_t i = 0; i < data.size(); i += 2)
    {
        int value = std::stoi(data.substr(i, 2), nullptr, 16);
        result.push_back(static_cast<char>(value));
    }
    return result;
}

static std::string ToLogGroupList(const std::string& data)
{
    string result;
    result.resize(1 + GetEncodeVarint32Len(data.size()));
    result[0] = (char)((1 << 3) | 2);
    EncodeVarint32(&result[1], data.size());
    result += data;
    return result;
}

TEST_CASE("pb")
{
    SUBCASE("ParseFromString should handle empty string correctly")
    {
        std::string emptyString;
        LogGroupList logGroupList;
        REQUIRE(PbEncoder::ParseFromString(emptyString, logGroupList));
    }

    SUBCASE("Empty LogGroup should serialize to empty string")
    {
        LogGroup logGroup;
        string data;
        REQUIRE(PbEncoder::SerializeToString(logGroup, data));
        REQUIRE(data.empty());
    }

    SUBCASE("Topic and Source are non-empty, Logs and Tags are non-empty")
    {
        LogGroup logGroup{{Log{12345, {LogContent("key1", "val1")}}}, {LogTag("key2", "val2")}, "topic1", "source1"};
        string data;
        REQUIRE(PbEncoder::SerializeToString(logGroup, data));

        LogGroupList logGroupList;
        INFO(toHexSpaced(ToLogGroupList(data)));
        REQUIRE(PbEncoder::ParseFromString(ToLogGroupList(data), logGroupList));
        REQUIRE(logGroupList.logGroupList.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].topic == "topic1");
        REQUIRE(logGroupList.logGroupList[0].source == "source1");
        REQUIRE(logGroupList.logGroupList[0].logs.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].key == "key1");
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].value == "val1");
        REQUIRE(logGroupList.logGroupList[0].logs[0].time == 12345);
        REQUIRE(logGroupList.logGroupList[0].logs[0].hasTimeNs == false);
        REQUIRE(logGroupList.logGroupList[0].logTags.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].logTags[0].key == "key2");
        REQUIRE(logGroupList.logGroupList[0].logTags[0].value == "val2");
    }

    SUBCASE("Logs contain timeNs")
    {
        LogGroup logGroup{{Log{1733935091, {LogContent("key1", "val1")}, 999999999}}, {}, "", ""};
        string data;
        REQUIRE(PbEncoder::SerializeToString(logGroup, data));

        LogGroupList logGroupList;
        INFO(toHexSpaced(ToLogGroupList(data)));
        REQUIRE(PbEncoder::ParseFromString(ToLogGroupList(data), logGroupList));
        REQUIRE(logGroupList.logGroupList.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].logs[0].time == 1733935091);
        REQUIRE(logGroupList.logGroupList[0].logs[0].hasTimeNs == true);
        REQUIRE(logGroupList.logGroupList[0].logs[0].timeNs == 999999999);
    }

    SUBCASE("Topic is empty, Source is non-empty")
    {
        LogGroup logGroup{{}, {}, "", "source1"};
        string data;
        REQUIRE(PbEncoder::SerializeToString(logGroup, data));

        LogGroupList logGroupList;
        INFO(toHexSpaced(ToLogGroupList(data)));
        REQUIRE(PbEncoder::ParseFromString(ToLogGroupList(data), logGroupList));
        REQUIRE(logGroupList.logGroupList.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].topic.empty());
        REQUIRE(logGroupList.logGroupList[0].source == "source1");
    }

    SUBCASE("Source is empty, Topic is non-empty")
    {
        LogGroup logGroup{{}, {}, "topic1", ""};
        string data;
        REQUIRE(PbEncoder::SerializeToString(logGroup, data));

        LogGroupList logGroupList;
        INFO(toHexSpaced(ToLogGroupList(data)));
        REQUIRE(PbEncoder::ParseFromString(ToLogGroupList(data), logGroupList));
        REQUIRE(logGroupList.logGroupList.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].topic == "topic1");
        REQUIRE(logGroupList.logGroupList[0].source.empty());
    }

    SUBCASE("Logs are empty")
    {
        LogGroup logGroup{{}, {}, "topic1", "source1"};
        string data;
        REQUIRE(PbEncoder::SerializeToString(logGroup, data));

        LogGroupList logGroupList;
        INFO(toHexSpaced(ToLogGroupList(data)));
        REQUIRE(PbEncoder::ParseFromString(ToLogGroupList(data), logGroupList));
        REQUIRE(logGroupList.logGroupList.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].logs.empty());
    }

    SUBCASE("Tags are empty")
    {
        LogGroup logGroup{{Log{1733935091, {LogContent("key1", "val1")}}}, {}, "topic1", "source1"};
        string data;
        REQUIRE(PbEncoder::SerializeToString(logGroup, data));

        LogGroupList logGroupList;
        INFO(toHexSpaced(ToLogGroupList(data)));
        REQUIRE(PbEncoder::ParseFromString(ToLogGroupList(data), logGroupList));
        REQUIRE(logGroupList.logGroupList.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].logTags.empty());
    }

    SUBCASE("Multiple logs and tags")
    {
        LogGroup logGroup{{Log{1733935091, {LogContent("key1", "val1"), LogContent("你好", "世界")}},
                           Log{1733935092, {LogContent("key1", "val1")}, 2}},
                          {
                              LogTag("key2", "val2"),
                              LogTag("测试tag", "val3"),
                          },
                          "topic测试",
                          "source1》》？？"};
        string data;
        REQUIRE(PbEncoder::SerializeToString(logGroup, data));
        INFO(toHexSpaced(ToLogGroupList(data)));
        LogGroupList logGroupList;
        REQUIRE(PbEncoder::ParseFromString(ToLogGroupList(data), logGroupList));
        REQUIRE(logGroupList.logGroupList.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].topic == "topic测试");
        REQUIRE(logGroupList.logGroupList[0].source == "source1》》？？");
        REQUIRE(logGroupList.logGroupList[0].logs.size() == 2);
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents.size() == 2);
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].key == "key1");
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].value == "val1");
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[1].key == "你好");
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[1].value == "世界");
        REQUIRE(logGroupList.logGroupList[0].logs[0].time == 1733935091);
        REQUIRE(logGroupList.logGroupList[0].logs[0].hasTimeNs == false);
        REQUIRE(logGroupList.logGroupList[0].logs[1].contents.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].logs[1].contents[0].key == "key1");
        REQUIRE(logGroupList.logGroupList[0].logs[1].contents[0].value == "val1");
        REQUIRE(logGroupList.logGroupList[0].logs[1].time == 1733935092);
        REQUIRE(logGroupList.logGroupList[0].logs[1].hasTimeNs == true);
        REQUIRE(logGroupList.logGroupList[0].logs[1].timeNs == 2);
        REQUIRE(logGroupList.logGroupList[0].logTags.size() == 2);
        REQUIRE(logGroupList.logGroupList[0].logTags[0].key == "key2");
        REQUIRE(logGroupList.logGroupList[0].logTags[0].value == "val2");
        REQUIRE(logGroupList.logGroupList[0].logTags[1].key == "测试tag");
        REQUIRE(logGroupList.logGroupList[0].logTags[1].value == "val3");
    }

    SUBCASE("Logs and tags value are empty string")
    {
        LogGroup logGroup{{Log{1733935091, {LogContent("key1", ""), LogContent("你好", "")}},
                           Log{1733935092, {LogContent("key1", "")}, 2}},
                          {
                              LogTag("key2", ""),
                              LogTag("测试tag", "val3"),
                          },
                          "",
                          ""};
        string data;
        REQUIRE(PbEncoder::SerializeToString(logGroup, data));
        INFO(toHexSpaced(ToLogGroupList(data)));
        LogGroupList logGroupList;
        REQUIRE(PbEncoder::ParseFromString(ToLogGroupList(data), logGroupList));
        REQUIRE(logGroupList.logGroupList.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].topic.empty());
        REQUIRE(logGroupList.logGroupList[0].source.empty());
        REQUIRE(logGroupList.logGroupList[0].logs.size() == 2);
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents.size() == 2);
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].key == "key1");
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].value.empty());
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[1].key == "你好");
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[1].value.empty());
        REQUIRE(logGroupList.logGroupList[0].logs[0].time == 1733935091);
        REQUIRE(logGroupList.logGroupList[0].logs[0].hasTimeNs == false);
        REQUIRE(logGroupList.logGroupList[0].logs[1].contents.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].logs[1].contents[0].key == "key1");
        REQUIRE(logGroupList.logGroupList[0].logs[1].contents[0].value.empty());
        REQUIRE(logGroupList.logGroupList[0].logs[1].time == 1733935092);
        REQUIRE(logGroupList.logGroupList[0].logs[1].hasTimeNs == true);
        REQUIRE(logGroupList.logGroupList[0].logs[1].timeNs == 2);
        REQUIRE(logGroupList.logGroupList[0].logTags.size() == 2);
        REQUIRE(logGroupList.logGroupList[0].logTags[0].key == "key2");
        REQUIRE(logGroupList.logGroupList[0].logTags[0].value.empty());
        REQUIRE(logGroupList.logGroupList[0].logTags[1].key == "测试tag");
        REQUIRE(logGroupList.logGroupList[0].logTags[1].value == "val3");
    }

    SUBCASE("ParseFromString should fail on invalid data")
    {
        std::string invalid_data =
            "\x0a\x0c\x0a\x0a\x08\x01\x12\x04key1\x1a\x04val1\x0a\x0c\x0a\x0a\x08\x01\x12\x04key1\x1a\x04val1"
            "\x0a\x0c"
            "\x0a"
            "\x0a\x08\x01\x12\x04key1\x1a\x04val1";
        LogGroupList logGroupList;
        REQUIRE(!PbEncoder::ParseFromString(invalid_data, logGroupList));
    }

    SUBCASE("ParseFromString cases 1")
    {
        const char*
            hex = "0a130894c3d4f905259373c92912060a01611201621a0022093132382e392e392e3132060a013412013532060a013612013"
                  "7";
        LogGroupList logGroupList;
        std::string data = ToLogGroupList(fromHex(hex));
        
        for (int i = 0; i < 2; i++)
        {
            logGroupList.logGroupList.clear();
            INFO(toHexSpaced(data));
            REQUIRE(PbEncoder::ParseFromString(data, logGroupList));
            REQUIRE(logGroupList.logGroupList.size() == 1);
            REQUIRE(logGroupList.logGroupList[0].topic == "");
            REQUIRE(logGroupList.logGroupList[0].source == "128.9.9.1");
            REQUIRE(logGroupList.logGroupList[0].logs.size() == 1);
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents.size() == 1);
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].key == "a");
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].value == "b");
            REQUIRE(logGroupList.logGroupList[0].logs[0].time == 1597317524);
            REQUIRE(logGroupList.logGroupList[0].logs[0].hasTimeNs == true);
            REQUIRE(logGroupList.logGroupList[0].logs[0].timeNs == 701068179);
            REQUIRE(logGroupList.logGroupList[0].logTags.size() == 2);
            REQUIRE(logGroupList.logGroupList[0].logTags[0].key == "4");
            REQUIRE(logGroupList.logGroupList[0].logTags[0].value == "5");
            REQUIRE(logGroupList.logGroupList[0].logTags[1].key == "6");
            REQUIRE(logGroupList.logGroupList[0].logTags[1].value == "7");

            string serialized;
            REQUIRE(PbEncoder::SerializeToString(logGroupList.logGroupList[0], serialized));
            data = ToLogGroupList(serialized);
        }

    }

    SUBCASE("ParseFromString cases 2")
    {
        const char* hex =
            "0a130894c3d4f905259373c92912060a01611201620a130893fb90ab062594579a2312060a01621201611a0022093132382e392e39"
            "2e3132060a013412013532060a0136120137";
        LogGroupList logGroupList;
        std::string data = ToLogGroupList(fromHex(hex));
        for (int i = 0; i < 2; i++)
        {
            logGroupList.logGroupList.clear();
            INFO(toHexSpaced(data));
            REQUIRE(PbEncoder::ParseFromString(data, logGroupList));
            REQUIRE(logGroupList.logGroupList.size() == 1);
            REQUIRE(logGroupList.logGroupList[0].topic == "");
            REQUIRE(logGroupList.logGroupList[0].source == "128.9.9.1");
            REQUIRE(logGroupList.logGroupList[0].logs.size() == 2);
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents.size() == 1);
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].key == "a");
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].value == "b");
            REQUIRE(logGroupList.logGroupList[0].logs[0].time == 1597317524);
            REQUIRE(logGroupList.logGroupList[0].logs[0].hasTimeNs == true);
            REQUIRE(logGroupList.logGroupList[0].logs[0].timeNs == 701068179);

            REQUIRE(logGroupList.logGroupList[0].logs[1].contents.size() == 1);
            REQUIRE(logGroupList.logGroupList[0].logs[1].contents[0].key == "b");
            REQUIRE(logGroupList.logGroupList[0].logs[1].contents[0].value == "a");
            REQUIRE(logGroupList.logGroupList[0].logs[1].time == 1701068179);
            REQUIRE(logGroupList.logGroupList[0].logs[1].hasTimeNs == true);
            REQUIRE(logGroupList.logGroupList[0].logs[1].timeNs == 597317524);

            REQUIRE(logGroupList.logGroupList[0].logTags.size() == 2);
            REQUIRE(logGroupList.logGroupList[0].logTags[0].key == "4");
            REQUIRE(logGroupList.logGroupList[0].logTags[0].value == "5");
            REQUIRE(logGroupList.logGroupList[0].logTags[1].key == "6");
            REQUIRE(logGroupList.logGroupList[0].logTags[1].value == "7");
            string serialized;
            REQUIRE(PbEncoder::SerializeToString(logGroupList.logGroupList[0], serialized));
            data = ToLogGroupList(serialized);
        }
    }

    SUBCASE("ParseFromString cases 3")
    {
        const char* hex =
            "0a190894c3d4f90512060a016112016212090a02323212036162631a0568656c6c6f22093132372e302e302e3132060a0134120135"
            "32060a0136120137";
        LogGroupList logGroupList;
        std::string data = ToLogGroupList(fromHex(hex));
        for (int i = 0; i < 2; i++)
        {
            logGroupList.logGroupList.clear();
            INFO(toHexSpaced(data));
            REQUIRE(PbEncoder::ParseFromString(data, logGroupList));
            REQUIRE(logGroupList.logGroupList.size() == 1);
            REQUIRE(logGroupList.logGroupList[0].topic == "hello");
            REQUIRE(logGroupList.logGroupList[0].source == "127.0.0.1");
            REQUIRE(logGroupList.logGroupList[0].logs.size() == 1);
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents.size() == 2);
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].key == "a");
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].value == "b");
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[1].key == "22");
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[1].value == "abc");
            REQUIRE(logGroupList.logGroupList[0].logs[0].time == 1597317524);
            REQUIRE(logGroupList.logGroupList[0].logs[0].hasTimeNs == false);

            REQUIRE(logGroupList.logGroupList[0].logTags.size() == 2);
            REQUIRE(logGroupList.logGroupList[0].logTags[0].key == "4");
            REQUIRE(logGroupList.logGroupList[0].logTags[0].value == "5");
            REQUIRE(logGroupList.logGroupList[0].logTags[1].key == "6");
            REQUIRE(logGroupList.logGroupList[0].logTags[1].value == "7");

            string serialized;
            REQUIRE(PbEncoder::SerializeToString(logGroupList.logGroupList[0], serialized));
            data = ToLogGroupList(serialized);
        }
    }

    SUBCASE("ParseFromString cases 4")
    {
        const char* hex =
            "0a300894c3d4f90512060a016112016212090a023232120361626312060a0163120164120d0a01781208616263a0616266611a0568"
            "656c6c6f22093132372e302e302e31320c0a057461675f6b1203747878";
        LogGroupList logGroupList;
        std::string data = ToLogGroupList(fromHex(hex));
        for (int i = 0; i < 2; i++)
        {
            logGroupList.logGroupList.clear();
            INFO(toHexSpaced(data));
            REQUIRE(PbEncoder::ParseFromString(data, logGroupList));
            REQUIRE(logGroupList.logGroupList.size() == 1);
            REQUIRE(logGroupList.logGroupList[0].topic == "hello");
            REQUIRE(logGroupList.logGroupList[0].source == "127.0.0.1");
            REQUIRE(logGroupList.logGroupList[0].logs.size() == 1);
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents.size() == 4);
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].key == "a");
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].value == "b");
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[1].key == "22");
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[1].value == "abc");
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[2].key == "c");
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[2].value == "d");
            REQUIRE(logGroupList.logGroupList[0].logs[0].contents[3].key == "x");
            INFO(logGroupList.logGroupList[0].logs[0].contents[3].value);
            REQUIRE(toHex(logGroupList.logGroupList[0].logs[0].contents[3].value) == "616263a061626661");
            REQUIRE(logGroupList.logGroupList[0].logs[0].time == 1597317524);
            REQUIRE(logGroupList.logGroupList[0].logs[0].hasTimeNs == false);

            REQUIRE(logGroupList.logGroupList[0].logTags.size() == 1);
            REQUIRE(logGroupList.logGroupList[0].logTags[0].key == "tag_k");
            REQUIRE(logGroupList.logGroupList[0].logTags[0].value == "txx");
            string serialized;
            REQUIRE(PbEncoder::SerializeToString(logGroupList.logGroupList[0], serialized));
            data = ToLogGroupList(serialized);
        }
    }

    SUBCASE("mulitple loggroups")
    {
        LogGroup logGroup1{{Log{12345, {LogContent("key1", "val1")}}}, {LogTag("key2", "val2")}, "topic1", "source1"};
        string data1;
        REQUIRE(PbEncoder::SerializeToString(logGroup1, data1));

        LogGroup logGroup2{{Log{12345, {LogContent("key3", "val4")}}}, {}, "topic2", "source2"};
        string data2;
        REQUIRE(PbEncoder::SerializeToString(logGroup2, data2));

        string data = ToLogGroupList(data1) + ToLogGroupList(data2);
        LogGroupList logGroupList;
        INFO(toHexSpaced(ToLogGroupList(data)));
        REQUIRE(PbEncoder::ParseFromString(data, logGroupList));
        REQUIRE(logGroupList.logGroupList.size() == 2);
        REQUIRE(logGroupList.logGroupList[0].topic == "topic1");
        REQUIRE(logGroupList.logGroupList[0].source == "source1");
        REQUIRE(logGroupList.logGroupList[0].logs.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].key == "key1");
        REQUIRE(logGroupList.logGroupList[0].logs[0].contents[0].value == "val1");
        REQUIRE(logGroupList.logGroupList[0].logs[0].time == 12345);
        REQUIRE(logGroupList.logGroupList[0].logs[0].hasTimeNs == false);
        REQUIRE(logGroupList.logGroupList[0].logTags.size() == 1);
        REQUIRE(logGroupList.logGroupList[0].logTags[0].key == "key2");
        REQUIRE(logGroupList.logGroupList[0].logTags[0].value == "val2");
        REQUIRE(logGroupList.logGroupList[1].topic == "topic2");
        REQUIRE(logGroupList.logGroupList[1].source == "source2");
    }
}
