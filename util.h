
#include <string>
#include "sls_namespace.h"

namespace ALIYUN_LOG_SDK_NAMESPACE
{
    namespace internal
    {
        std::string GetHostIp();
        bool InitNetWork();
        void CleanNetWork();
    }
}
