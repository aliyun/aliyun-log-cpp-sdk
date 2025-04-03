
#include <string>

namespace aliyun_log_sdk_v6
{
    namespace internal
    {
        std::string GetHostIp();
        bool InitNetWork();
        void CleanNetWork();
    }
}
