#ifdef __ALIYUN_LOG_UNITTEST__
#define __ALIYUN_LOG_CLIENT_UNITTEST__

#include "client.h"

#include <iostream>
#include <regex>
#include <string>

#include "RestfulApiCommon.h"
#include "adapter.h"
#include "doctest.h"
#include "resource.h"

using namespace std;
using namespace aliyun_log_sdk_v6;

TEST_CASE("Client")
{
    SUBCASE("parseRegion")
    {
      try{
        CHECK_EQ(CodecTool::ParseRegionFromHost(
                     "http://cn-hangzhou-devcommon-intranet.sls.aliyuncs.com"),
                 "cn-hangzhou-devcommon");
        CHECK_EQ(CodecTool::ParseRegionFromHost(
                     "cn-hangzhou-share.log.aliyuncs.com"),
                 "cn-hangzhou");
        CHECK_EQ(CodecTool::ParseRegionFromHost(
                     "https://cn-shanghai.log.aliyuncs.com"),
                 "cn-shanghai");
        CHECK_EQ(
            CodecTool::ParseRegionFromHost("cn-hangzhou-stg.log.aliyuncs.com"),
            "cn-hangzhou-stg");
        CHECK_EQ(CodecTool::ParseRegionFromHost(
                     "http://cn-zhangjiakou-stg-intranet.log.aliyuncs.com"),
                 "cn-zhangjiakou-stg");
        CHECK_EQ(CodecTool::ParseRegionFromHost(""), "");
        CHECK_EQ(CodecTool::ParseRegionFromHost("192.168.1.1"), "");
        CHECK_EQ(CodecTool::ParseRegionFromHost("http://192.168.1.1"), "");
      }catch(std::regex_error& e){
          cout << e.what() << endl;
      }
    }
    SUBCASE("clientParseRegion")
    {
        string host = "https://cn-shanghai.log.aliyuncs.com";
        auto client = unique_ptr<LOGClient>(
            new LOGClient(host, "testAccessId", "testAccessKey",
                          LOG_REQUEST_TIMEOUT, "127.0.0.1", false));
        CHECK_EQ(client->GetRegion(), "cn-shanghai");
    }
}

#endif