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

TEST_CASE("Hasher")
{
    SUBCASE("Sha256")
    {
        string message = "hello", message2 = "world";
        SUBCASE("sha256 add once")
        {
            const auto v = CodecTool::CalcSHA256("world");
            REQUIRE_EQ(toHex(v),
                       "486ea46224d1bb4fb680f34f7c9ad96a8f24ec88be73ea8e5a6c652"
                       "60e9cb8a7");
            REQUIRE_EQ(toHex(CodecTool::CalcSHA256("hello, world")),
                       "09ca7e4eaa6e8ae9c7d261167129184883644d07dfba7cbfbc4c8a2"
                       "e08360d5b");
            REQUIRE_EQ(toHex(CodecTool::CalcSHA256("")),
                       "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991"
                       "b7852b855");
            REQUIRE_EQ(toHex(CodecTool::CalcSHA256(
                           "asdas,,zckoaq|+asdasmnzxk adaslv121+-@#1")),
                       "9391127e0d567ef778527c57a9f8f867c7c2c964c8706f8deb92ede"
                       "dec54aba7");
        }
        SUBCASE("sha256 add twice")
        {
            SHA256 sha256;
            sha256.add(reinterpret_cast<const uint8_t*>(message.data()),
                       message.size());
            sha256.add(reinterpret_cast<const uint8_t*>(message2.data()),
                       message2.size());
            string res = sha256.getHexHash();
            SHA256 sha256_2;
            string m = message + message2;
            sha256_2.add(reinterpret_cast<const uint8_t*>(m.data()), m.size());
            string res2 = sha256_2.getHexHash();
            REQUIRE_EQ(res, res2);
            REQUIRE_EQ(m, "helloworld");
            string res3 = toHex(CodecTool::CalcSHA256(message + message2));
            string res4 = toHex(CodecTool::CalcSHA256(m));
        }
    }
    SUBCASE("hmac")
    {
        string key = "my-test";
        string message = "hello", message2 = "world";
        SUBCASE("hmac-sha1")
        {
            SUBCASE("hmac-sha1 add once")
            {
                COUT_EX << toHex(CodecTool::CalcHMACSHA1("world", key)) << endl;
                REQUIRE_EQ(toHex(CodecTool::CalcHMACSHA1("world", key)),
                           "95c426aac427a0bd09cbd96757ec6260429fa7c0");
            }
            SUBCASE("hmac-sha1 add twice")
            {
                HMACSHA1 hmac(reinterpret_cast<const uint8_t*>(key.data()),
                              key.size());
                hmac.add(reinterpret_cast<const uint8_t*>(message.data()),
                         message.size());
                hmac.add(reinterpret_cast<const uint8_t*>(message2.data()),
                         message2.size());
                string res = hmac.getHash();
                string res2 = CodecTool::CalcHMACSHA1(message + message2, key);
                REQUIRE_EQ(res, res2);
            }

            SUBCASE("hmac-sha256")
            {
                SUBCASE("hmac-sha256 add once")
                {
                    REQUIRE_EQ(toHex(CodecTool::CalcHMACSHA256("world", key)),
                               "be705bbb4879669206f3ff4863b30aa90ef36b4f1b0adf9"
                               "c8706d6a91f05218d");
                    REQUIRE_EQ(
                        toHex(CodecTool::CalcHMACSHA256("hello, world", key)),
                        "68492565a04121909ff0757b000b032435e0ca5bc43b8307f8a83d"
                        "3bd0fdb387");
                    REQUIRE_EQ(toHex(CodecTool::CalcHMACSHA256("aa", key)),
                               "5ddc9210595d8bf35d4c488c21ced173beacc2ac774666a"
                               "323f2c86414c611b1");
                    REQUIRE_EQ(
                        toHex(CodecTool::CalcHMACSHA256(
                            "asdas,,zckoaq|+asdasmnzxk adaslv121+-@#1", key)),
                        "799d699e383a04a4e1f36fa3c0ea68bbe748de8f3ba57d9d77a8d9"
                        "2c491dabb4");
                }
                SUBCASE("hmac-sha256 add twice")
                {
                    HMACSHA256 hmac(
                        reinterpret_cast<const uint8_t*>(key.data()),
                        key.size());
                    hmac.add(reinterpret_cast<const uint8_t*>(message.data()),
                             message.size());
                    hmac.add(reinterpret_cast<const uint8_t*>(message2.data()),
                             message2.size());
                    string res = hmac.getHash();
                    string res2 =
                        CodecTool::CalcHMACSHA256(message + message2, key);
                    REQUIRE_EQ(res, res2);
                }
            }
        }
    }
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
        string debugDate = "20220808T014928Z";
        v4.SetDebugDateTime(debugDate);
        v4.SetDebugDate(debugDate);
        SUBCASE("empty header and url params and region shanghai")
        {
            map<string, string> emptyHeader, emptyUrlParams;
            SignerV4 v4Shanghai({accessKeyId, accessKeySecret}, "cn-shanghai");
            v4Shanghai.SetDebugDateTime(debugDate);
            v4Shanghai.SetDebugDate(debugDate);
            v4Shanghai.Sign(httpMethod, resourceUri, emptyHeader,
                            emptyUrlParams, payload);
            string authorization = emptyHeader[AUTHORIZATION];
            COUT_EX << "signer v4:" << authorization << endl;
            string expected =
                "SLS4-HMAC-SHA256 "
                "Credential=acsddda21dsd/20220808T014928Z/cn-shanghai/sls/"
                "aliyun_v4_request,SignedHeaders=x-log-content-sha256;x-log-"
                "date,Signature="
                "9074743df84d12aa71714c238bff987cb4d752c857237a47cae1dc55ca751c"
                "76";
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
                "SLS4-HMAC-SHA256 "
                "Credential=acsddda21dsd/20220808T014928Z/cn-hangzhou/sls/"
                "aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-"
                "sha256;x-log-date;x-log-test,Signature="
                "12edc42d33de2c9e218592fd3cdf16c1b278a45ee4278badb30d46276f1235"
                "53";
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
                "SLS4-HMAC-SHA256 "
                "Credential=acsddda21dsd/20220808T014928Z/cn-hangzhou/sls/"
                "aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-"
                "sha256;x-log-date;x-log-test,Signature="
                "74b970832224169945525a1aba87e825dee9bb806d09d27be8037d8c9064ee"
                "bb";
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
                "Credential=acsddda21dsd/20220808T014928Z/cn-hangzhou/sls/"
                "aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-"
                "sha256;x-log-date;x-log-test,Signature="
                "1d27812dd151fdfab6aaf128281686092abbed5947f3344ac3b9fa9373f9bb"
                "8d";
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
                "SLS4-HMAC-SHA256 "
                "Credential=acsddda21dsd/20220808T014928Z/cn-hangzhou/sls/"
                "aliyun_v4_request,SignedHeaders=x-acs-ppp;x-log-content-"
                "sha256;x-log-date;x-log-test,Signature="
                "0ded3f358f138d42e9eeb7381ed53f2c8c8a7495d0bf18ced57b20ba09ad10"
                "00";
            REQUIRE_EQ(authorization, expected);
        }
    }
}
#endif