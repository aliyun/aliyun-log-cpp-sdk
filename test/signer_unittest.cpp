#ifdef __ALIYUN_LOG_UNITTEST__
#define __ALIYUN_LOG_SIGNER_UNITTEST__
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "signer.h"

#include <iostream>
#include <regex>

#include "RestfulApiCommon.h"
#include "adapter.h"
#include "doctest.h"
#include "resource.h"

using namespace std;
using namespace aliyun_log_sdk_v6;
using namespace aliyun_log_sdk_v6::auth;

#ifndef COUT_EX
#define COUT_EX std::cout << (__FILE__) << ":" << (__LINE__) << " "
#endif

static string toHex(const string& m)
{
    return CodecTool::ToHex(m);
}

namespace aliyun_log_sdk_v6
{
namespace auth
{
class SignerUnittestHelper
{
   public:
    static std::string V4UrlEncode(const std::string& url, bool ignoreSlash)
    {
        return SignerV4::UrlEncode(url, ignoreSlash);
    }
    static void SetV4DateTime(SignerV4& v4, const std::string& s)
    {
        v4.mDebugDateTime = s;
    }
    static void SetV4Date(SignerV4& v4, const std::string& s)
    {
        v4.mDebugDate = s;
    }
    static void SetV1DateTime(SignerV1& v1, const std::string& s)
    {
        v1.mDebugDateTime = s;
    }
};
}  // namespace auth
}  // namespace aliyun_log_sdk_v6

TEST_CASE("Signer")
{
    const string httpMethod = HTTP_POST;
    const string resourceUri = "/logstores";
    const string payload = "adasd= -asd zcas";
    const string accessKeyId = "acsddda21dsd";
    const string accessKeySecret = "zxasdasdasw2";
    const string region = "";
    const map<string, string> httpHeaders = {{"hello", "world"},
                                             {"hello-Text", "a12X- "},
                                             {" Ko ", ""},
                                             {"", "AA"},
                                             {"x-log-test", "het123"},
                                             {"x-acs-ppp", "dds"}},
                              urlParams = {
                                  {" abc", "efg"},
                                  {" agc ", ""},
                                  {"", "efg"},
                                  {"A-bc", "eFg"},
                              };
    SUBCASE("v1")
    {
        string date = CodecTool::GetDateString();
        auto headerCopy = httpHeaders;
        headerCopy[DATE] = date;
        string signature =
            LOGAdapter::GetUrlSignature(httpMethod, resourceUri, headerCopy,
                                        urlParams, payload, accessKeySecret);
        string authorization =
            LOG_HEADSIGNATURE_PREFIX + accessKeyId + ':' + signature;
        COUT_EX << "adapter v1:" << authorization << endl;

        auto headerCopy2 = httpHeaders;
        headerCopy2[DATE] = date;
        auth::Signer::Sign(httpMethod, resourceUri, headerCopy2, urlParams,
                           payload, accessKeyId, accessKeySecret, V1);
        string authorization2 = headerCopy2[AUTHORIZATION];
        COUT_EX << "signer v1:" << authorization << endl;

        CHECK_EQ(authorization, authorization2);
    }
    SUBCASE("set debug date")
    {
        auto headerCopy = httpHeaders;
        auth::SignerV4 v4({accessKeyId, accessKeySecret}, "cn-hangzhou");
        SignerUnittestHelper::SetV4DateTime(v4,"hello");
        SignerUnittestHelper::SetV4Date(v4,"hello-date");
        v4.Sign(httpMethod, resourceUri, headerCopy, urlParams, payload);
        auto date = headerCopy[X_LOG_DATE];
        CHECK_EQ(date, "hello");
    }

    SUBCASE("v4")
    {
        SignerV4 v4({accessKeyId, accessKeySecret}, "cn-hangzhou");
        string debugDate = "20220808";
        string debugDateTime = "20220808T032330Z";
        SignerUnittestHelper::SetV4DateTime(v4,debugDateTime);
        SignerUnittestHelper::SetV4Date(v4,debugDate);
        SUBCASE("empty header and url params and region shanghai")
        {
            map<string, string> emptyHeader, emptyUrlParams;
            SignerV4 v4Shanghai({accessKeyId, accessKeySecret}, "cn-shanghai");
            SignerUnittestHelper::SetV4DateTime(v4Shanghai, debugDateTime);
            SignerUnittestHelper::SetV4Date(v4Shanghai, debugDate);
            v4Shanghai.Sign(httpMethod, resourceUri, emptyHeader,
                            emptyUrlParams, payload);
            string authorization = emptyHeader[AUTHORIZATION];
            COUT_EX << "signer v4:" << authorization << endl;
            string expected =
                "SLS4-HMAC-SHA256 "
                "Credential=acsddda21dsd/20220808/cn-shanghai/sls/"
                "aliyun_v4_request,SignedHeaders=x-log-content-sha256;x-log-"
                "date,Signature="
                "8a10a5e723cb2e75964816de660b2c16a58af8bc0261f7f0722d832468c76c"
                "e8";
            CHECK_EQ(authorization, expected);
        }

        SUBCASE("empty payload")
        {
            auto headerCopy = httpHeaders;
            auto emptyPayload = "";
            v4.Sign(httpMethod, resourceUri, headerCopy, urlParams,
                    emptyPayload);
            string authorization = headerCopy[AUTHORIZATION];
            COUT_EX << "signer v4:" << authorization << endl;
            string expected =
                "SLS4-HMAC-SHA256 "
                "Credential=acsddda21dsd/20220808/cn-hangzhou/sls/"
                "aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-"
                "sha256;x-log-date;x-log-test,Signature="
                "17277e433834a91c193f2dd6f237fc9b33c653f13f4c87e9e73a5f7fcabc66"
                "31";
            CHECK_EQ(authorization, expected);
        }

        SUBCASE("method get")
        {
            auto headerCopy = httpHeaders;
            auto emptyPayload = "";
            v4.Sign(HTTP_GET, resourceUri, headerCopy, urlParams, emptyPayload);
            string authorization = headerCopy[AUTHORIZATION];
            COUT_EX << "signer v4:" << authorization << endl;
            string expected =
                "SLS4-HMAC-SHA256 "
                "Credential=acsddda21dsd/20220808/cn-hangzhou/sls/"
                "aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-"
                "sha256;x-log-date;x-log-test,Signature="
                "ef98c6596c88b80d12366ec42f4fab6d82037d961d84f2e8c52ab109084064"
                "70";
            REQUIRE_EQ(authorization, expected);
        }

        SUBCASE("variable header and url params")
        {
            auto headerCopy = httpHeaders;
            v4.Sign(httpMethod, resourceUri, headerCopy, urlParams, payload);
            string authorization = headerCopy[AUTHORIZATION];
            COUT_EX << "signer v4:" << authorization << endl;
            string expected =
                "SLS4-HMAC-SHA256 "
                "Credential=acsddda21dsd/20220808/cn-hangzhou/sls/"
                "aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-"
                "sha256;x-log-date;x-log-test,Signature="
                "348d28cb4aa259a5302105b52d7d0ecde7ab415b3c0eb3a452f2a2fd384689"
                "91";
            CHECK_EQ(authorization, expected);
        }

        SUBCASE("uri and url params with / + * ~")
        {
            auto headerCopy = httpHeaders;
            string uri = "/logstores/hello/a+*~bb/cc";
            auto urlParamsCopy = urlParams;
            urlParamsCopy["abs-ij*asd/vc"] = "a~js+d ada";
            urlParamsCopy["a abAas123/vc"] = "a~jdad a2ADFs+d ada";
            v4.Sign(httpMethod, uri, headerCopy, urlParamsCopy, payload);
            string authorization = headerCopy[AUTHORIZATION];
            COUT_EX << "signer v4:" << authorization << endl;
            string expected =
                "SLS4-HMAC-SHA256 "
                "Credential=acsddda21dsd/20220808/cn-hangzhou/sls/"
                "aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-"
                "sha256;x-log-date;x-log-test,Signature="
                "d79c9358725537e03e3e0ff6d375853f36e2a7f853a2960053a498eefbbb42"
                "f5";
            CHECK_EQ(authorization, expected);
        }

        SUBCASE("ISO8601 time format")
        {
            SUBCASE("dateTime")
            {
                auto dateTime =
                    CodecTool::GetDateString(DATETIME_FORMAT_ISO8601);
                std::regex re(
                    "^\\s*((((1[6-9]|[2-9]\\d)\\d{2})(0[13578]|1[02])(0[1-9]|["
                    "12]"
                    "\\d|3[01]))|(((1[6-9]|[2-9]\\d)\\d{2})(0[469]|11)(0?[1-9]|"
                    "[12]"
                    "\\d|30))|(((1[6-9]|[2-9]\\d)\\d{2})02(0[1-9]|1\\d|2[0-8]))"
                    "|((("
                    "1[6-9]|[2-9]\\d)[13579][26])0229)|(((1[6-9]|[2-9]\\d)["
                    "2468]["
                    "048])0229)|(((1[6-9]|[2-9]\\d)0[48])0229)|(([13579]6)"
                    "000229)|("
                    "([2468][048])000229)|(([3579]2)000229))(T(2[0-3]|[0-1][0-"
                    "9])(["
                    "0-5][0-9])([0-5][0-9])Z$)");
                CHECK(std::regex_match(dateTime, re));
            }
            SUBCASE("date")
            {
                auto date = CodecTool::GetDateString(DATE_FORMAT_ISO8601);

                std::regex re2(
                    "^\\s*((((1[6-9]|[2-9]\\d)\\d{2})(0[13578]|1[02])(0[1-9]|["
                    "12]"
                    "\\d|3[01]))|(((1[6-9]|[2-9]\\d)\\d{2})(0[469]|11)(0?[1-9]|"
                    "[12]"
                    "\\d|30))|(((1[6-9]|[2-9]\\d)\\d{2})02(0[1-9]|1\\d|2[0-8]))"
                    "|((("
                    "1[6-9]|[2-9]\\d)[13579][26])0229)|(((1[6-9]|[2-9]\\d)["
                    "2468]["
                    "048])0229)|(((1[6-9]|[2-9]\\d)0[48])0229)|(([13579]6)"
                    "000229)|("
                    "([2468][048])000229)|(([3579]2)000229))$");
                CHECK(std::regex_match(date, re2));
            }
        }

        SUBCASE("no region should throws an exception")
        {
            SignerV4 v4({accessKeyId, accessKeySecret}, "");
            auto headerCopy = httpHeaders;
            CHECK_THROWS(v4.Sign(httpMethod, resourceUri, headerCopy, urlParams,
                                 payload));
        }

        SUBCASE("url encode")
        {
            SUBCASE("ignore slash")
            {
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode("", true), "");
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode("/logstores/hello", true),
                         "/logstores/hello");
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode(
                             "/logstores/hello?-*~/test", true),
                         "/logstores/hello%3F-%2A~/test");
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode(
                             "/loa871_-+dres/hel lo?-*~/test", true),
                         "/loa871_-%2Bdres/hel%20lo%3F-%2A~/test");
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode("-+*", true), "-%2B%2A");
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode("/", true), "/");
            }
            SUBCASE("with slash")
            {
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode("", false), "");
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode("/logstores/hello", false),
                         "%2Flogstores%2Fhello");
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode(
                             "/logstores/hel lo?-*~/test", false),
                         "%2Flogstores%2Fhel%20lo%3F-%2A~%2Ftest");
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode(
                             "/loa871_-+dres/hello?-*~/test", false),
                         "%2Floa871_-%2Bdres%2Fhello%3F-%2A~%2Ftest");
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode("-+*", false), "-%2B%2A");
                CHECK_EQ(SignerUnittestHelper::V4UrlEncode("/", false), "%2F");
            }
        }
    }
}
#endif