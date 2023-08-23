#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <iostream>
#include <string>
#include "adapter.h"
#include "doctest.h"

// #include <iphlpapi.h>
// #include <winsock2.h>
// #include <ws2tcpip.h>
// #include <vector>
// #pragma comment(lib, "ws2_32.lib")
// #pragma comment(lib, "iphlpapi.lib")

using namespace std;
using namespace aliyun_log_sdk_v6;

TEST_CASE("parseDate")
{
    string dateString = "Mon, 07 Aug 2023 06:50:42 GMT";
    auto date1 = CodecTool::GetDateString();
    auto t = CodecTool::DecodeDateString(date1, DATE_FORMAT_RFC822);
    std::time_t time = std::time({});
    CHECK_EQ(time, t);
}

TEST_CASE("host")
{
    //   string ip;
    //   cout << DnsCache::ParseHost("", ip) << " " << ip << endl;
    //   ip = "";
    //   cout << DnsCache::ParseHost(nullptr, ip) << " " << ip << endl;
    //   ip = "";
    //   cout << DnsCache::ParseHost("www.baidu.com", ip) << " " << ip << endl;
    //   ip = "";
    //   cout << DnsCache::ParseHost("http://www.baidu.com", ip) << " " << ip << endl;
    //   ip = "";
    //   cout << DnsCache::ParseHost("1.2.3.4", ip) << " " << ip << endl;
    //   ip = "";
    //   cout << DnsCache::ParseHost("1232.2.3.4", ip) << " " << ip << endl;
}

TEST_CASE("GetDateString")
{
}