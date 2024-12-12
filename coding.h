#pragma once

#include <string>
#include <cstring>

namespace aliyun_log_sdk_v6
{
struct SlsStringPiece
{
    const char* mPtr;
    int32_t mLen;
    SlsStringPiece(const char* ptr, int32_t len) : mPtr(ptr), mLen(len) {}
    SlsStringPiece() : mPtr(NULL), mLen(0) {}

    std::string ToString() const
    {
        if (mPtr == NULL)
        {
            return std::string();
        }
        return std::string(mPtr, mLen);
    }
};

extern char* EncodeVarint32(char* dst, uint32_t v);
extern const char* SkipProtobufField(const char* pos, const char* limit, uint32_t wireType);

inline static char* PutFixedUint32(char* pos, uint32_t v)
{
    std::memcpy(pos, &v, sizeof(uint32_t));
    return pos + sizeof(uint32_t);
}

// Pointer-based variants of GetVarint...  These either store a value
// in *v and return a pointer just past the parsed value, or return
// NULL on error.  These routines only look at bytes in the range
// [p..limit-1]
inline static const char* GetVarint32Ptr(const char* p, const char* limit, uint32_t* v)
{
#define BITS_VALUE(ptr, offset) (((uint32_t)(*(ptr))) << offset)
#define BITS_VALUE_WITH_MASK(ptr, offset) (((uint32_t)(((uint8_t)(*(ptr))) ^ MASK)) << offset)
    if (p == NULL)
    {
        return NULL;
    }
    const static uint8_t MASK = 128;
    if (p < limit && ((*p) & MASK) == 0)
    {
        *v = *p;
        return p + 1;
    }
    else if (p + 1 < limit && !((*(p + 1)) & MASK))
    {
        *v = BITS_VALUE_WITH_MASK(p, 0) | BITS_VALUE(p + 1, 7);
        return p + 2;
    }
    else if (p + 2 < limit && !((*(p + 2)) & MASK))
    {
        *v = BITS_VALUE_WITH_MASK(p, 0) | BITS_VALUE_WITH_MASK(p + 1, 7) | BITS_VALUE(p + 2, 14);
        return p + 3;
    }
    else if (p + 3 < limit && !((*(p + 3)) & MASK))
    {
        *v = BITS_VALUE_WITH_MASK(p, 0) | BITS_VALUE_WITH_MASK(p + 1, 7) | BITS_VALUE_WITH_MASK(p + 2, 14) |
             BITS_VALUE(p + 3, 21);
        return p + 4;
    }
    else if (p + 4 < limit && !((*(p + 4)) & MASK))
    {
        *v = BITS_VALUE_WITH_MASK(p, 0) | BITS_VALUE_WITH_MASK(p + 1, 7) | BITS_VALUE_WITH_MASK(p + 2, 14) |
             BITS_VALUE_WITH_MASK(p + 3, 21) | BITS_VALUE(p + 4, 28);
        return p + 5;
    }
    *v = 0;
    return NULL;

#undef BITS_VALUE
#undef BITS_VALUE_WITH_MASK
}

inline static int32_t GetEncodeVarint32Len(uint32_t v)
{
    // Operate on characters as unsigneds
    if (v < (1 << 7))
    {
        return 1;
    }
    else if (v < (1 << 14))
    {
        return 2;
    }
    else if (v < (1 << 21))
    {
        return 3;
    }
    else if (v < (1 << 28))
    {
        return 4;
    }
    else
    {
        return 5;
    }
}

} // namespace aliyun_log_sdk_v6
