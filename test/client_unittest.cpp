#ifdef __ALIYUN_LOG_UNITTEST__
#define __ALIYUN_LOG_SIGNER_UNITTEST__
#include "signer.h"

#include <iostream>
#include <regex>

#include "RestfulApiCommon.h"
#include "adapter.h"
#include "doctest.h"
#include "resource.h"
#include "client.h"
#include <thread>
#include <chrono>

using namespace std;
using namespace aliyun_log_sdk_v6;
using namespace aliyun_log_sdk_v6::auth;

#ifndef COUT_EX
#define COUT_EX std::cout << (__FILE__) << ":" << (__LINE__) << " "
#endif

// todo delete this
TEST_CASE("client-stg")
{
    string endpoint = "cn-hangzhou-staging-intranet.sls.aliyuncs.com";
    string accessKeyId = "";
    string accessKeySecret = "";
    string region = "cn-hangzhou-stg";
    auto clientv4 =
        unique_ptr<LOGClient>(new LOGClient(endpoint, accessKeyId, accessKeySecret,
                               LOG_REQUEST_TIMEOUT, "127.0.0.1", false));
    string project = "sls-sdk-testp-8471-1658392184";
    clientv4->SetRegion(region);
    clientv4->SetSignVersion(LOGSignVersion::V4);
    SUBCASE("Logstore")
    {
        string logstoreName = "cpp-v4-test-logstore";
        try
        {   
            SUBCASE("CreateLogstore")   
            {
                LogStore logstore(logstoreName, 30, 2);
                auto resp = clientv4->CreateLogStore(project, logstore);
                REQUIRE_EQ(resp.statusCode, 200);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            
            SUBCASE("ListLogstore")
            {
                auto resp = clientv4->ListLogStores(project, logstoreName);
                REQUIRE_EQ(resp.statusCode, 200);
                REQUIRE_EQ(resp.result.size(), 1);
            }
             
             SUBCASE("GetLogstore")
            {
                auto resp = clientv4->GetLogStore(project, logstoreName);
                REQUIRE_EQ(resp.statusCode, 200);
                REQUIRE_EQ(resp.result.GetLogStoreName(), logstoreName);
                REQUIRE_EQ(resp.result.GetTTL(), 30);
                REQUIRE_EQ(resp.result.GetShardCount(), 2);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            
             SUBCASE("UpdateLogstore")
            {
                LogStore logstore(logstoreName, 30, 2);
                auto resp = clientv4->UpdateLogStore(project, logstore);
                REQUIRE_EQ(resp.statusCode, 200);
                 std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            
            SUBCASE("DeleteLogstore")
            // delete
            {
                auto resp = clientv4->DeleteLogStore(project, logstoreName);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

        }
        catch (LOGException& e)
        {
            COUT_EX << e.GetErrorCode() << ": " << e.GetMessage() << endl;
            FAIL(e.GetErrorCode(), string(" "), e.GetMessage());
        }
    }
    SUBCASE("MachineGroup")
    {
        string name = "test-cpp-v4-sdk-mg";
        try
        {
          SUBCASE("Create")
          {
              MachineGroup machineGroup(name, "ip", {});
              auto resp = clientv4->CreateMachineGroup(project, machineGroup);
              REQUIRE_EQ(resp.statusCode, 200);
              std::this_thread::sleep_for(std::chrono::seconds(1));
          }
          SUBCASE("List")
          {
              auto resp = clientv4->ListMachineGroups(project, 0, 100);
              REQUIRE_GE(resp.result.size(), 1);
              COUT_EX << "list machine group: ";
              for (auto& mg : resp.result)
              {
                  cout << mg << ", ";
              }
              cout << endl;
              std::this_thread::sleep_for(std::chrono::seconds(1));
          }
          SUBCASE("Update")
          {
              MachineGroup machineGroup(name, "ip", {});
              auto resp = clientv4->UpdateMachineGroup(project, machineGroup);
              REQUIRE_EQ(resp.statusCode, 200);
              std::this_thread::sleep_for(std::chrono::seconds(1));
          }
          SUBCASE("Delete")
          {
              auto resp = clientv4->DeleteMachineGroup(project, name);
              std::this_thread::sleep_for(std::chrono::seconds(1));
          }
        }
        catch(LOGException& e)
        {
            COUT_EX << e.GetErrorCode() << ": " << e.GetMessage() << endl;
            FAIL(e.GetErrorCode(), string(" "), e.GetMessage());
        }
    }
}
#endif