#ifdef __ALIYUN_LOG_UNITTEST__
#define __ALIYUN_LOG_SIGNER_UNITTEST__

#include <iostream>

#include "RestfulApiCommon.h"
#include "adapter.h"
#include "doctest.h"
#include "resource.h"
#include "signer.h"

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

TEST_CASE("codec tool")
{
    SUBCASE("ToHex")
    {
        auto vec2str = [](const vector<uint8_t>& i)
        { return string(i.begin(), i.end()); };
        CHECK_EQ(CodecTool::ToHex(vec2str(vector<uint8_t>{})), "");
        CHECK_EQ(CodecTool::ToHex(vec2str(vector<uint8_t>{15})), "0f");
        CHECK_EQ(CodecTool::ToHex(vec2str(vector<uint8_t>{255})), "ff");
        CHECK_EQ(CodecTool::ToHex(vec2str(vector<uint8_t>{0})), "00");
        CHECK_EQ(CodecTool::ToHex(vec2str(vector<uint8_t>{254, 16, 8, 161})),
                 "fe1008a1");
        CHECK_EQ(CodecTool::ToHex(vec2str(vector<uint8_t>{9, 91})), "095b");
        CHECK_EQ(CodecTool::ToHex(vec2str(vector<uint8_t>{9, 180, 200, 17})),
                 "09b4c811");

        CHECK_NE(CodecTool::ToHex(vec2str(vector<uint8_t>{9, 180, 200, 17})),
                 "");
    }
    SUBCASE("LowerCase")
    {
        CHECK_EQ(CodecTool::LowerCase("avaasdas"), "avaasdas");
        CHECK_EQ(CodecTool::LowerCase("avAAas"), "avaaas");
        CHECK_EQ(CodecTool::LowerCase(""), "");
        CHECK_EQ(CodecTool::LowerCase("ABC"), "abc");
        CHECK_EQ(CodecTool::LowerCase("ava---Asdas"), "ava---asdas");
        CHECK_EQ(CodecTool::LowerCase("cZxGHJ9821-2Kl+-()?.,adsdas"),
                 "czxghj9821-2kl+-()?.,adsdas");
        CHECK_EQ(CodecTool::LowerCase("--/.,"), "--/.,");
    }
    SUBCASE("ReplaceAll")
    {
        CHECK_EQ(CodecTool::ReplaceAll("", "", ""), "");
        CHECK_EQ(CodecTool::ReplaceAll("asdasd", "aa", "a"), "asdasd");
        CHECK_EQ(CodecTool::ReplaceAll("a 1adda ", "a", "b"), "b 1bddb ");
        CHECK_EQ(CodecTool::ReplaceAll("abcabcabcabc", "abc", "cba"),
                 "cbacbacbacba");
        CHECK_EQ(CodecTool::ReplaceAll("abcabcabcabc", "a", "ab"),
                 "abbcabbcabbcabbc");
        CHECK_EQ(CodecTool::ReplaceAll("abcabcabcabc", "abc", "a"), "aaaa");
    }
    SUBCASE("Trim")
    {
        CHECK_EQ(CodecTool::Trim(""), "");
        CHECK_EQ(CodecTool::Trim("abs123;"), "abs123;");
        CHECK_EQ(CodecTool::Trim("dd abv ad"), "dd abv ad");
        CHECK_EQ(CodecTool::Trim(" ad2 23"), "ad2 23");
        CHECK_EQ(CodecTool::Trim("a3asa   "), "a3asa");
        CHECK_EQ(CodecTool::Trim(" a a "), "a a");
        CHECK_EQ(CodecTool::Trim(" a a "), "a a");
        CHECK_EQ(CodecTool::Trim("\t  aad \t a123 \t "), "aad \t a123");
    }
}

TEST_CASE("Hasher")
{
    SUBCASE("Sha256")
    {
        string message = "hello", message2 = "world";
        SUBCASE("sha256 add once")
        {
            const auto v = CodecTool::CalcSHA256("world");
            CHECK_EQ(toHex(v),
                       "486ea46224d1bb4fb680f34f7c9ad96a8f24ec88be73ea8e5a6c652"
                       "60e9cb8a7");
            CHECK_EQ(toHex(CodecTool::CalcSHA256("hello, world")),
                       "09ca7e4eaa6e8ae9c7d261167129184883644d07dfba7cbfbc4c8a2"
                       "e08360d5b");
            CHECK_EQ(toHex(CodecTool::CalcSHA256("")),
                       "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991"
                       "b7852b855");
            CHECK_EQ(toHex(CodecTool::CalcSHA256(
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
            CHECK_EQ(res, res2);
            CHECK_EQ(m, "helloworld");
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
                CHECK_EQ(toHex(CodecTool::CalcHMACSHA1("world", key)),
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
                vector<uint8_t> hash = hmac.getHash();
                string res(hash.begin(), hash.end());
                string res2 = CodecTool::CalcHMACSHA1(message + message2, key);
                CHECK_EQ(res, res2);
            }

            SUBCASE("hmac-sha256")
            {
                SUBCASE("hmac-sha256 add once")
                {
                    CHECK_EQ(toHex(CodecTool::CalcHMACSHA256("world", key)),
                               "be705bbb4879669206f3ff4863b30aa90ef36b4f1b0adf9"
                               "c8706d6a91f05218d");
                    CHECK_EQ(
                        toHex(CodecTool::CalcHMACSHA256("hello, world", key)),
                        "68492565a04121909ff0757b000b032435e0ca5bc43b8307f8a83d"
                        "3bd0fdb387");
                    CHECK_EQ(toHex(CodecTool::CalcHMACSHA256("aa", key)),
                               "5ddc9210595d8bf35d4c488c21ced173beacc2ac774666a"
                               "323f2c86414c611b1");
                    CHECK_EQ(
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
                    vector<uint8_t> hash = hmac.getHash();
                    string res(hash.begin(), hash.end());
                    string res2 =
                        CodecTool::CalcHMACSHA256(message + message2, key);
                    CHECK_EQ(res, res2);
                }
            }
        }
    }
}

#endif