#ifdef __ALIYUN_LOG_UNITTEST__
#define __ALIYUN_LOG_SIGNER_UNITTEST__
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "signer.h"

#include <iostream>

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

        REQUIRE_EQ(authorization, authorization2);
    }
    SUBCASE("macro SLS_DEFINE_DEBUGABLE_STRING")
    {
        auto headerCopy = httpHeaders;
        auth::SignerV4 v4({accessKeyId, accessKeySecret}, "cn-hangzhou");
        v4.SetDebugDateTime("hello");
        v4.SetDebugDate("hello-date");
        v4.Sign(httpMethod, resourceUri, headerCopy, urlParams, payload);
        auto date = headerCopy[X_LOG_DATE];
        REQUIRE_EQ(date, "hello");
    }

    SUBCASE("v4")
    {
        SignerV4 v4({accessKeyId, accessKeySecret}, "cn-hangzhou");
        string debugDate = "20220808";
        string debugDateTime = "20220808T032330Z";
        v4.SetDebugDateTime(debugDateTime);
        v4.SetDebugDate(debugDate);
        SUBCASE("empty header and url params and region shanghai")
        {
            map<string, string> emptyHeader, emptyUrlParams;
            SignerV4 v4Shanghai({accessKeyId, accessKeySecret}, "cn-shanghai");
            v4Shanghai.SetDebugDateTime(debugDateTime);
            v4Shanghai.SetDebugDate(debugDate);
            v4Shanghai.Sign(httpMethod, resourceUri, emptyHeader,
                            emptyUrlParams, payload);
            string authorization = emptyHeader[AUTHORIZATION];
            COUT_EX << "signer v4:" << authorization << endl;
            string expected =
                "SLS4-HMAC-SHA256 Credential=acsddda21dsd/20220808/cn-shanghai/sls/aliyun_v4_request,SignedHeaders=x-log-content-sha256;x-log-date,Signature=8a10a5e723cb2e75964816de660b2c16a58af8bc0261f7f0722d832468c76ce8";
            REQUIRE_EQ(authorization, expected);
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
                "SLS4-HMAC-SHA256 Credential=acsddda21dsd/20220808/cn-hangzhou/sls/aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-sha256;x-log-date;x-log-test,Signature=17277e433834a91c193f2dd6f237fc9b33c653f13f4c87e9e73a5f7fcabc6631";
            REQUIRE_EQ(authorization, expected);
        }

        SUBCASE("method get")
        {
            auto headerCopy = httpHeaders;
            auto emptyPayload = "";
            v4.Sign(HTTP_GET, resourceUri, headerCopy, urlParams, emptyPayload);
            string authorization = headerCopy[AUTHORIZATION];
            COUT_EX << "signer v4:" << authorization << endl;
            string expected =
                "SLS4-HMAC-SHA256 Credential=acsddda21dsd/20220808/cn-hangzhou/sls/aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-sha256;x-log-date;x-log-test,Signature=ef98c6596c88b80d12366ec42f4fab6d82037d961d84f2e8c52ab10908406470";
            REQUIRE_EQ(authorization, expected);
        }

        SUBCASE("variable header and url params")
        {
            auto headerCopy = httpHeaders;
            v4.Sign(httpMethod, resourceUri, headerCopy, urlParams, payload);
            string authorization = headerCopy[AUTHORIZATION];
            COUT_EX << "signer v4:" << authorization << endl;
            string expected =
                "SLS4-HMAC-SHA256 Credential=acsddda21dsd/20220808/cn-hangzhou/sls/aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-sha256;x-log-date;x-log-test,Signature=348d28cb4aa259a5302105b52d7d0ecde7ab415b3c0eb3a452f2a2fd38468991";
            REQUIRE_EQ(authorization, expected);
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
                "SLS4-HMAC-SHA256 Credential=acsddda21dsd/20220808/cn-hangzhou/sls/aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-sha256;x-log-date;x-log-test,Signature=d79c9358725537e03e3e0ff6d375853f36e2a7f853a2960053a498eefbbb42f5";
            REQUIRE_EQ(authorization, expected);
        }
    }
}
#endif