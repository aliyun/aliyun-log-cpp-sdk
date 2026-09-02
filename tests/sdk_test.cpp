#include "adapter.h"
#include "client.h"
#include "common.h"
#include "pb.h"
#include "resource.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

using namespace aliyun_log_sdk_v6;

namespace
{
struct CapturedRequest
{
    std::string method;
    std::string host;
    int32_t port = 0;
    bool usingHttps = false;
    std::string url;
    std::string queryString;
    std::map<std::string, std::string> headers;
    std::string body;
    int32_t timeout = 0;
    int64_t maxSpeed = 0;
};

CapturedRequest capturedRequest;
HttpMessage nextResponse;

void CaptureSend(const std::string& httpMethod,
                 const std::string& host,
                 const int32_t port,
                 const bool usingHttps,
                 const std::string& url,
                 const std::string& queryString,
                 const std::map<std::string, std::string>& header,
                 const std::string& body,
                 const int32_t timeout,
                 HttpMessage& httpMessage,
                 const int64_t maxSpeed)
{
    capturedRequest.method = httpMethod;
    capturedRequest.host = host;
    capturedRequest.port = port;
    capturedRequest.usingHttps = usingHttps;
    capturedRequest.url = url;
    capturedRequest.queryString = queryString;
    capturedRequest.headers = header;
    capturedRequest.body = body;
    capturedRequest.timeout = timeout;
    capturedRequest.maxSpeed = maxSpeed;
    httpMessage = nextResponse;
}

void ResetTransport(const std::string& content = std::string())
{
    capturedRequest = CapturedRequest();
    nextResponse = HttpMessage(200, {{X_LOG_REQUEST_ID, "request-for-unit-test"}}, content);
}

class TestClient : public LOGClient
{
public:
    explicit TestClient(const std::string& host, bool compress = false)
        : LOGClient(host,
                    "access-id-for-unit-tests",
                    "access-key-for-unit-tests",
                    7,
                    "source-for-unit-tests",
                    compress)
    {
        mLOGSend = CaptureSend;
    }
};
}

TEST_CASE("query strings are ordered and URL encoded")
{
    CHECK(CodecTool::StartWith("x-log-requestid", "x-log-"));
    CHECK(CodecTool::StartWith("anything", ""));
    CHECK_FALSE(CodecTool::StartWith("x-acs-", "x-log-"));
    CHECK_FALSE(CodecTool::StartWith("short", "longer-pattern"));
    CHECK(CodecTool::UrlEncode("a b+c/x") == "a%20b%2Bc%2Fx");

    const std::map<std::string, std::string> parameters{
        {"slash", "/x"},
        {"empty", ""},
        {"query", "a b+c"},
    };
    std::string queryString = "stale";
    LOGAdapter::GetQueryString(parameters, queryString);

    CHECK(queryString == "empty=&query=a%20b%2Bc&slash=%2Fx");

    LOGAdapter::GetQueryString(std::map<std::string, std::string>(), queryString);
    CHECK(queryString.empty());
}

TEST_CASE("default HTTP messages have no successful status")
{
    const HttpMessage message;
    CHECK(message.statusCode == -1);
    CHECK(message.header.empty());
    CHECK(message.content.empty());
}

TEST_CASE("LZ4 compression preserves binary payloads")
{
    std::string source;
    for (int i = 0; i < 128; ++i)
    {
        source.append("repeated-value", 14);
        source.push_back('\0');
        source.push_back(static_cast<char>(i));
    }

    std::string compressed;
    REQUIRE(CompressAlgorithm::CompressLz4(source, compressed));
    CHECK(compressed.size() < source.size());

    std::string restored;
    REQUIRE(CompressAlgorithm::UncompressLz4(compressed, static_cast<uint32_t>(source.size()), restored));
    CHECK(restored == source);

    CHECK_FALSE(CompressAlgorithm::UncompressLz4("invalid", static_cast<uint32_t>(source.size()), restored));
}

TEST_CASE("client passes normalized host and protocol to the transport")
{
    SUBCASE("HTTPS endpoint prefixes the project")
    {
        ResetTransport("[]");
        TestClient client("https://logs.example.com/path");

        const ListSqlInstanceResponse response = client.ListSqlInstance("demo-project");

        CHECK(response.statusCode == 200);
        CHECK(response.requestId == "request-for-unit-test");
        CHECK(capturedRequest.method == HTTP_GET);
        CHECK(capturedRequest.host == "demo-project.logs.example.com");
        CHECK(capturedRequest.port == 80);
        CHECK(capturedRequest.usingHttps);
        CHECK(capturedRequest.url == "/sqlinstance");
        CHECK(capturedRequest.queryString.empty());
        CHECK(capturedRequest.headers.at(HOST) == capturedRequest.host);
        CHECK(capturedRequest.headers.at(CONTENT_LENGTH) == "0");
        CHECK(capturedRequest.timeout == 7);
    }

    SUBCASE("raw IP endpoint is not prefixed")
    {
        ResetTransport("[]");
        TestClient client("http://127.0.0.1:8080/path");

        client.ListSqlInstance("ignored-project");

        CHECK(capturedRequest.host == "127.0.0.1:8080");
        CHECK_FALSE(capturedRequest.usingHttps);
        CHECK(capturedRequest.headers.at(HOST) == "127.0.0.1:8080");
    }
}

TEST_CASE("SQL Instance requests use the expected method and JSON body")
{
    TestClient client("https://logs.example.com");

    SUBCASE("create")
    {
        ResetTransport();
        const CreateSqlInstanceResponse response = client.CreateSqlInstance("demo-project", 8);

        CHECK(response.statusCode == 200);
        CHECK(capturedRequest.method == HTTP_POST);
        CHECK(capturedRequest.url == "/sqlinstance");
        CHECK(capturedRequest.headers.at(CONTENT_TYPE) == TYPE_LOG_JSON);
        CHECK(capturedRequest.headers.at(CONTENT_LENGTH) == std::to_string(capturedRequest.body.size()));

        rapidjson::Document body;
        body.Parse(capturedRequest.body.c_str());
        REQUIRE(body.IsObject());
        REQUIRE(body.HasMember("cu"));
        CHECK(body["cu"].GetInt() == 8);
        CHECK_FALSE(body.HasMember("useAsDefault"));
    }

    SUBCASE("update with explicit default selection")
    {
        ResetTransport();
        const UpdateSqlInstanceResponse response = client.UpdateSqlInstance("demo-project", 16, true);

        CHECK(response.requestId == "request-for-unit-test");
        CHECK(capturedRequest.method == HTTP_PUT);
        CHECK(capturedRequest.url == "/sqlinstance");
        CHECK(capturedRequest.headers.at(CONTENT_TYPE) == TYPE_LOG_JSON);

        rapidjson::Document body;
        body.Parse(capturedRequest.body.c_str());
        REQUIRE(body.IsObject());
        CHECK(body["cu"].GetInt() == 16);
        CHECK(body["useAsDefault"].GetBool());
    }
}

TEST_CASE("SQL Instance responses preserve all fields")
{
    ResetTransport(
        "[{\"name\":\"primary\",\"cu\":12,\"createTime\":100,\"updateTime\":200,\"useAsDefault\":true},"
        "{\"name\":\"secondary\",\"cu\":4,\"createTime\":300,\"updateTime\":400}]");
    TestClient client("logs.example.com");

    const ListSqlInstanceResponse response = client.ListSqlInstance("demo-project");

    REQUIRE(response.sqlInstances.size() == 2);
    CHECK(response.sqlInstances[0].name == "primary");
    CHECK(response.sqlInstances[0].cu == 12);
    CHECK(response.sqlInstances[0].createTime == static_cast<time_t>(100));
    CHECK(response.sqlInstances[0].updateTime == static_cast<time_t>(200));
    CHECK(response.sqlInstances[0].useAsDefault);
    CHECK(response.sqlInstances[1].name == "secondary");
    CHECK(response.sqlInstances[1].cu == 4);
    CHECK_FALSE(response.sqlInstances[1].useAsDefault);
}

TEST_CASE("invalid SQL Instance responses are reported as bad responses")
{
    ResetTransport("{\"name\":\"not-an-array\"}");
    TestClient client("logs.example.com");
    bool caught = false;

    try
    {
        client.ListSqlInstance("demo-project");
    }
    catch (const LOGException& exception)
    {
        caught = true;
        CHECK(exception.GetErrorCode() == LOGE_BAD_RESPONSE);
        CHECK(exception.GetRequestId() == "request-for-unit-test");
        CHECK(exception.GetHttpCode() == 200);
    }

    CHECK(caught);
}

TEST_CASE("project SQL parses metrics and log records")
{
    ResetTransport("[{\"__time__\":\"123\",\"__source__\":\"worker\",\"level\":\"error\"}]");
    nextResponse.header[X_LOG_PROGRESS] = "Complete";
    nextResponse.header[X_LOG_COUNT] = "1";
    nextResponse.header[X_LOG_PROCESSED_ROWS] = "9876543210";
    nextResponse.header[X_LOG_ELASPED_MILLISECOND] = "42";
    nextResponse.header[X_LOG_CPU_SEC] = "1.25";
    nextResponse.header[X_LOG_CPU_CORES] = "3";
    TestClient client("https://logs.example.com");

    const ProjectSqlResponse response =
        client.ExecuteProjectSql("demo-project", "status:500 | select count(1)", true);

    CHECK(capturedRequest.method == HTTP_GET);
    CHECK(capturedRequest.url == "/logs");
    CHECK(capturedRequest.queryString ==
          "powerSql=true&query=status%3A500%20%7C%20select%20count%281%29");
    CHECK(response.processedRows == 9876543210LL);
    CHECK(response.elapsedMilli == 42);
    CHECK(response.cpuSec == doctest::Approx(1.25));
    CHECK(response.cpuCore == 3);
    CHECK(response.result.progress == "Complete");
    CHECK(response.result.logline == 1);
    REQUIRE(response.result.logdatas.size() == 1);
    CHECK(response.result.logdatas[0].timestamp == 123);
    CHECK(response.result.logdatas[0].source == "worker");
    REQUIRE(response.result.logdatas[0].data.size() == 1);
    CHECK(response.result.logdatas[0].data[0].first == "level");
    CHECK(response.result.logdatas[0].data[0].second == "error");
}

TEST_CASE("posting log items serializes and compresses the expected protobuf")
{
    ResetTransport();
    TestClient client("https://logs.example.com", true);
    LogItem item;
    item.timestamp = 12345;
    item.data.push_back({"level", "info"});
    item.data.push_back({"message", "hello"});

    const PostLogStoreLogsResponse response =
        client.PostLogStoreLogs("demo-project", "demo-store", "demo-topic", {item});

    CHECK(response.statusCode == 200);
    CHECK(response.bodyBytes == static_cast<int32_t>(capturedRequest.body.size()));
    CHECK(capturedRequest.method == HTTP_POST);
    CHECK(capturedRequest.url == "/logstores/demo-store");
    CHECK(capturedRequest.headers.at(CONTENT_TYPE) == TYPE_LOG_PROTOBUF);
    CHECK(capturedRequest.headers.at(X_LOG_COMPRESSTYPE) == LOG_LZ4);

    const uint32_t rawSize = static_cast<uint32_t>(std::stoul(capturedRequest.headers.at(X_LOG_BODYRAWSIZE)));
    std::string rawBody;
    REQUIRE(CompressAlgorithm::UncompressLz4(capturedRequest.body, rawSize, rawBody));

    pb::LogGroup expected(
        {pb::Log(12345, {pb::LogContent("level", "info"), pb::LogContent("message", "hello")})},
        {},
        "demo-topic",
        "source-for-unit-tests");
    std::string expectedBody;
    REQUIRE(expected.SerializeToString(expectedBody));
    CHECK(rawBody == expectedBody);
}

TEST_CASE("resource parsers preserve string collections")
{
    ConfigInputDetail detail;
    detail.FromJsonString(
        "{\"logType\":\"common_reg_log\",\"logPath\":\"/var/log/app\","
        "\"filePattern\":\"*.log\",\"localStorage\":true,\"timeFormat\":\"%Y-%m-%d\","
        "\"logBeginRegex\":\"^\\\\d\",\"regex\":\"(.*)\","
        "\"key\":[\"time\",\"message\"],\"filterKey\":[\"level\"],"
        "\"filterRegex\":[\"DEBUG\",\"TRACE\"],\"topicFormat\":\"default\"}");

    CHECK(detail.GetKey() == std::vector<std::string>({"time", "message"}));
    CHECK(detail.GetFilterKey() == std::vector<std::string>({"level"}));
    CHECK(detail.GetFilterRegex() == std::vector<std::string>({"DEBUG", "TRACE"}));

    MachineGroup group;
    group.FromJsonString(
        "{\"groupName\":\"workers\",\"groupType\":\"\","
        "\"groupAttribute\":{\"externalName\":\"\",\"groupTopic\":\"prod\"},"
        "\"machineIdentifyType\":\"ip\",\"machineList\":[\"10.0.0.1\",\"10.0.0.2\"],"
        "\"createTime\":11,\"lastModifyTime\":22}");

    CHECK(group.GetMachineList() == std::vector<std::string>({"10.0.0.1", "10.0.0.2"}));
    CHECK(group.GetGroupAttribute().GetGroupTopic() == "prod");
    CHECK(group.GetCreateTime() == 11);
    CHECK(group.GetLastModifyTime() == 22);
}
