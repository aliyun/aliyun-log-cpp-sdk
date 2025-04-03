#include "util.h"

#include <cstring>
#include <ctime>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#include <iphlpapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <net/if.h>
#endif

using namespace std;
using namespace aliyun_log_sdk_v6::internal;

namespace aliyun_log_sdk_v6
{
namespace internal
{

static std::string GetHostIpByHostName()
{
    char hostname[255];
    gethostname(hostname, 255);
    struct hostent* entry = gethostbyname(hostname);
    if (entry == NULL)
    {
        return string();
    }
    struct in_addr* addr = (struct in_addr*)entry->h_addr_list[0];
    if (addr == NULL)
    {
        return string();
    }
    char* ipaddr = inet_ntoa(*addr);
    if (ipaddr == NULL)
    {
        return string();
    }
    return string(ipaddr);
}


static std::string GetHostIpByETHName()
{
#ifdef _WIN32
    PMIB_IPADDRTABLE pIPAddrTable = (MIB_IPADDRTABLE*)malloc(sizeof(MIB_IPADDRTABLE));
    if (!pIPAddrTable)
    {
        return "";
    }
    DWORD dwSize = 0, dwRetVal = 0;
    if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)
    {
        free(pIPAddrTable);
        pIPAddrTable = (MIB_IPADDRTABLE*)malloc(dwSize); // re-allocate
    }
    // second call
    if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR)
    {
        for (int i = 0; i < (int)pIPAddrTable->dwNumEntries; ++i)
        {
            // ignore disconnected / deleted / transient ip addresses
            if (pIPAddrTable->table[i].wType & (MIB_IPADDR_DELETED | MIB_IPADDR_DISCONNECTED | MIB_IPADDR_TRANSIENT))
            {
                continue;
            }
            struct in_addr addr;
            addr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwAddr;
            string ip = inet_ntoa(addr);
            if (ip == "127.0.0.1") // rm loopback
            {
                continue;
            }
            free(pIPAddrTable);
            return ip;
        }
    }
    if (pIPAddrTable)
    {
        free(pIPAddrTable);
        pIPAddrTable = NULL;
    }
    return "";
#else
    int sock;
    struct sockaddr_in sin;
    struct ifreq ifr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        return string();
    }

    // use eth0 as the default ETH name
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
    {
        close(sock); // added by gaolei 13-10-09
        return string();
    }

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));

    char* ipaddr = inet_ntoa(sin.sin_addr);
    close(sock); // added by gaolei 13-10-09
    if (ipaddr == NULL)
    {
        return string();
    }
    return string(ipaddr);
#endif
}

std::string GetHostIp()
{
    string ip = GetHostIpByHostName();
    if (ip.empty() || ip.find("127.0.0.1") != string::npos)
    {
        return GetHostIpByETHName();
    }
    return ip;
}


bool InitNetWork()
{
#ifdef _WIN32
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
    {
        return false;
    }
#endif
    return true;
}

void CleanNetWork()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

} // namespace internal
} // namespace aliyun_log_sdk_v6