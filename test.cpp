#include <cassert>
#include <iostream>

#include "adapter.h"
#include "signer.h"

using namespace std;
using namespace aliyun_log_sdk_v6;

namespace aliyun_log_sdk_v6
{
namespace unittest
{
bool testOk = true;
class UnitTest
{
   public:
    int total = 0, failed = 0;
    string errors;

    string Report()
    {
        string res = "======= " + TestName() + " =======\n" + errors;
        res += "total: " + to_string(total) + ", failed: " + to_string(failed) +
               "\n";
        if (failed == 0)
        {
            res += "All test passed";
        }
        else
        {
            res += "Test failed";
        }
        return res;
    }
    virtual std::string TestName() { return "Base"; };
};

void AssertTrue(UnitTest* that, const string& location, bool val,
                const string& message = "expect to be true")
{
    that->total++;
    if (!val)
    {
        that->failed++;
        testOk = false;
        that->errors += location + message + "\n";
    }
}
#define ASSERT_TRUE(x)                                        \
    AssertTrue(this,                                          \
               string("") + __FILE__ + " " + __func__ + ":" + \
                   std::to_string(__LINE__) + " ",            \
               x)
#define ASSERT_TEST_EQUAL(x, y)                               \
    AssertTrue(this,                                          \
               string("") + __FILE__ + " " + __func__ + ":" + \
                   std::to_string(__LINE__),                  \
               x == y, "left = " + std::to_string(x) + ", y = ")
#define REGISTER_TEST(name) \
    std::string TestName() { return #name; }

string toHex(const string& m) { return CodecTool::ToHex(m); }

class Sha256Test : public UnitTest
{
   public:
    void RunTest()
    {
        // sha256
        const auto v = CodecTool::CalcSHA256("world");
        ASSERT_TRUE(
            toHex(v) ==
            "486ea46224d1bb4fb680f34f7c9ad96a8f24ec88be73ea8e5a6c65260e9cb8a7");
        ASSERT_TRUE(
            toHex(CodecTool::CalcSHA256("hello, world")) ==
            "09ca7e4eaa6e8ae9c7d261167129184883644d07dfba7cbfbc4c8a2e08360d5b");
        ASSERT_TRUE(
            toHex(CodecTool::CalcSHA256("")) ==
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
        ASSERT_TRUE(
            toHex(CodecTool::CalcSHA256(
                "asdas,,zckoaq|+asdasmnzxk adaslv121+-@#1")) ==
            "9391127e0d567ef778527c57a9f8f867c7c2c964c8706f8deb92ededec54aba7");

        // hmac-sha1
        string key = "my-test";
        cout << toHex(CodecTool::CalcHMACSHA1("world", key)) << endl;
        ASSERT_TRUE(toHex(CodecTool::CalcHMACSHA1("world", key)) ==
                    "95c426aac427a0bd09cbd96757ec6260429fa7c0");
        // hmac-sha1 add twice
        string message = "hello", message2 = "world";
        {
            HMAC hmac(reinterpret_cast<const uint8_t*>(key.data()), key.size());
            hmac.add(reinterpret_cast<const uint8_t*>(message.data()),
                     message.size());
            hmac.add(reinterpret_cast<const uint8_t*>(message2.data()),
                     message2.size());
            string res = string(reinterpret_cast<const char*>(hmac.result()),
                                SHA1_DIGEST_BYTES);
            string res2 = CodecTool::CalcHMACSHA1(message + message2, key);
            ASSERT_TRUE(res == res2);
        }
        // hmac-sha256 add twice
        {
            HMACSHA256 hmac(reinterpret_cast<const uint8_t*>(key.data()),
                            key.size());
            hmac.add(reinterpret_cast<const uint8_t*>(message.data()),
                     message.size());
            hmac.add(reinterpret_cast<const uint8_t*>(message2.data()),
                     message2.size());
            string res = hmac.getHash();
            string res2 = CodecTool::CalcHMACSHA256(message + message2, key);
            ASSERT_TRUE(res == res2);
        }
        // sha256 add twice
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
            ASSERT_TRUE(res == res2);
            ASSERT_TRUE(m == "helloworld");
            string res3 = toHex(CodecTool::CalcSHA256(message + message2));
            string res4 = toHex(CodecTool::CalcSHA256(m));
        }

        // hmac-sha256
        ASSERT_TRUE(
            toHex(CodecTool::CalcHMACSHA256("world", key)) ==
            "be705bbb4879669206f3ff4863b30aa90ef36b4f1b0adf9c8706d6a91f05218d");
        ASSERT_TRUE(
            toHex(CodecTool::CalcHMACSHA256("hello, world", key)) ==
            "68492565a04121909ff0757b000b032435e0ca5bc43b8307f8a83d3bd0fdb387");
        ASSERT_TRUE(
            toHex(CodecTool::CalcHMACSHA256("aa", key)) ==
            "5ddc9210595d8bf35d4c488c21ced173beacc2ac774666a323f2c86414c611b1");
        ASSERT_TRUE(
            toHex(CodecTool::CalcHMACSHA256(
                "asdas,,zckoaq|+asdasmnzxk adaslv121+-@#1", key)) ==
            "799d699e383a04a4e1f36fa3c0ea68bbe748de8f3ba57d9d77a8d92c491dabb4");
    }
    REGISTER_TEST(Sha256Test);
};
class SignTest : public UnitTest
{
   public:
    REGISTER_TEST(SignTest);
    void RunTest()
    {
        const string httpMethod = HTTP_GET;
        const string resourceUri = "/logstores";
        const string payload = "";
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
        // v1
        {
            string date = CodecTool::GetDateString();
            auto headerCopy = httpHeaders;
            headerCopy[DATE] = date;
            string signature = LOGAdapter::GetUrlSignature(
                httpMethod, resourceUri, headerCopy, urlParams, payload,
                accessKeySecret);
            string authorization =
                LOG_HEADSIGNATURE_PREFIX + accessKeyId + ':' + signature;
            cout << "adapter v1:" << authorization << endl;

            auto headerCopy2 = httpHeaders;
            headerCopy2[DATE] = date;
            auth::Signer::Sign(httpMethod, resourceUri, headerCopy2, urlParams,
                               payload,  accessKeyId , accessKeySecret, V1);
            string authorization2 = headerCopy2[AUTHORIZATION];
            cout << "signer v1:" << authorization << endl;

            ASSERT_TRUE(authorization == authorization2);
        }
        // v4
        {
            auto headerCopy = httpHeaders;
            auth::Signer::Sign(httpMethod, resourceUri, headerCopy, urlParams,
                               payload, accessKeyId, accessKeySecret,  V4,
                               "cn-hangzhou");
            string authorization = headerCopy[AUTHORIZATION];
            cout << "signer v4:" << authorization << endl;
        }
    }
};
}  // namespace unittest
}  // namespace aliyun_log_sdk_v6
using namespace aliyun_log_sdk_v6::unittest;
int main()
{
    Sha256Test test;
    test.RunTest();
    cout << test.Report() << endl;

    SignTest test2;
    test2.RunTest();
    cout << test2.Report() << endl;

    if (!testOk) return -1;
    return 0;
}