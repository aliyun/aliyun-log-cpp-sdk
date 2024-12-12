#include "coding.h"

using namespace std;

namespace aliyun_log_sdk_v6
{

char* EncodeVarint32(char* dst, uint32_t v)
{
    // Operate on characters as unsigneds
    unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
    static const int B = 128;
    if (v < (1 << 7))
    {
        *(ptr++) = v;
    }
    else if (v < (1 << 14))
    {
        *(ptr++) = v | B;
        *(ptr++) = v >> 7;
    }
    else if (v < (1 << 21))
    {
        *(ptr++) = v | B;
        *(ptr++) = (v >> 7) | B;
        *(ptr++) = v >> 14;
    }
    else if (v < (1 << 28))
    {
        *(ptr++) = v | B;
        *(ptr++) = (v >> 7) | B;
        *(ptr++) = (v >> 14) | B;
        *(ptr++) = v >> 21;
    }
    else
    {
        *(ptr++) = v | B;
        *(ptr++) = (v >> 7) | B;
        *(ptr++) = (v >> 14) | B;
        *(ptr++) = (v >> 21) | B;
        *(ptr++) = v >> 28;
    }
    return reinterpret_cast<char*>(ptr);
}

const char* SkipProtobufField(const char* pos, const char* limit, uint32_t wireType)
{
    switch (wireType)
    {
    case 0: // Varint
    {
        uint32_t v;
        return GetVarint32Ptr(pos, limit, &v);
    }
    case 1: // fixed 64bit
    {
        return pos + 8 > limit ? NULL : pos + 8;
    }
    case 2: // dynamic length
    {
        uint32_t len;
        pos = GetVarint32Ptr(pos, limit, &len);
        if (pos == NULL)
        {
            return NULL;
        }
        return pos + len > limit ? NULL : pos + len;
    }
    case 5: // fixed 32bit
    {
        return pos + 4 > limit ? NULL : pos + 4;
    }
    default:
        return nullptr;
    }
}

} // namespace aliyun_log_sdk_v6
