#include "common.h"
#include "lz4/lz4.h"
#include <asm/byteorder.h>


namespace aliyun_log_sdk_v6
{
/////////////////////////////////////////////// MACRO //////////////////////////////////////////////////
#define SHIFT_LEFT(a, b) ((a) << (b) | (a) >> (32 - b))

/**
 * each operation
 */
#define F(b, c, d) (((b) & (c)) | ( (~(b)) & (d)))
#define G(b, c, d) (((d) & (b)) | ( (~(d)) & (c)))
#define H(b, c, d) ((b) ^ (c) ^ (d))
#define I(b, c, d) ((c) ^ ((b) | (~(d))))

/**
 * each round
 */
#define FF(a, b, c, d, word, shift, k) \
{ \
    (a) += F((b), (c), (d)) + (word) + (k); \
    (a) = SHIFT_LEFT((a), (shift)); \
    (a) += (b); \
}
#define GG(a, b, c, d, word, shift, k) \
{ \
    (a) += G((b), (c), (d)) + (word) + (k); \
    (a) = SHIFT_LEFT((a), (shift)); \
    (a) += (b); \
}
#define HH(a, b, c, d, word, shift, k) \
{ \
    (a) += H((b), (c), (d)) + (word) + (k); \
    (a) = SHIFT_LEFT((a), (shift)); \
    (a) += (b); \
}
#define II(a, b, c, d, word, shift, k) \
{ \
    (a) += I((b), (c), (d)) + (word) + (k); \
    (a) = SHIFT_LEFT((a), (shift)); \
    (a) += (b); \
}
////////////////////////////////////////////////////////// GLOBAL VARIABLE /////////////////////////////////////////////////////////
const uint8_t gPadding[64] =
{
    0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};



//////////////////////////////////////////////////////// LOCAL DECLEARATION //////////////////////////////////////////////////////////
struct Md5Block
{
    uint32_t word[16];
};
/**
 * copy a pool into a block, using little endian
 */
void CopyBytesToBlock(const uint8_t* poolIn, struct Md5Block& block)
{
    uint32_t j = 0;
    for (uint32_t i = 0; i < 32; ++i, j += 4)
    {
        block.word[i] = ((uint32_t) poolIn[j])
                    | (((uint32_t) poolIn[j+1]) << 8)
                    | (((uint32_t) poolIn[j+2]) << 16)
                    | (((uint32_t) poolIn[j+3]) << 24);
    }
}

/**
 * calculate md5 hash value from a block
 */
void CalMd5(struct Md5Block block, uint32_t h[4])
{
    uint32_t a = h[0];
    uint32_t b = h[1];
    uint32_t c = h[2];
    uint32_t d = h[3];

    // Round 1
    FF (a, b, c, d, block.word[0], 7, 0xd76aa478);
    FF (d, a, b, c, block.word[1], 12, 0xe8c7b756);
    FF (c, d, a, b, block.word[2], 17, 0x242070db);
    FF (b, c, d, a, block.word[3], 22, 0xc1bdceee);
    FF (a, b, c, d, block.word[4], 7, 0xf57c0faf);
    FF (d, a, b, c, block.word[5], 12, 0x4787c62a);
    FF (c, d, a, b, block.word[6], 17, 0xa8304613);
    FF (b, c, d, a, block.word[7], 22, 0xfd469501);
    FF (a, b, c, d, block.word[8], 7, 0x698098d8);
    FF (d, a, b, c, block.word[9], 12, 0x8b44f7af);
    FF (c, d, a, b, block.word[10], 17, 0xffff5bb1);
    FF (b, c, d, a, block.word[11], 22, 0x895cd7be);
    FF (a, b, c, d, block.word[12], 7, 0x6b901122);
    FF (d, a, b, c, block.word[13], 12, 0xfd987193);
    FF (c, d, a, b, block.word[14], 17, 0xa679438e);
    FF (b, c, d, a, block.word[15], 22, 0x49b40821);

    // Round 2
    GG (a, b, c, d, block.word[1], 5, 0xf61e2562);
    GG (d, a, b, c, block.word[6], 9, 0xc040b340);
    GG (c, d, a, b, block.word[11], 14, 0x265e5a51);
    GG (b, c, d, a, block.word[0], 20, 0xe9b6c7aa);
    GG (a, b, c, d, block.word[5], 5, 0xd62f105d);
    GG (d, a, b, c, block.word[10], 9,  0x2441453);
    GG (c, d, a, b, block.word[15], 14, 0xd8a1e681);
    GG (b, c, d, a, block.word[4], 20, 0xe7d3fbc8);
    GG (a, b, c, d, block.word[9], 5, 0x21e1cde6);
    GG (d, a, b, c, block.word[14], 9, 0xc33707d6);
    GG (c, d, a, b, block.word[3], 14, 0xf4d50d87);
    GG (b, c, d, a, block.word[8], 20, 0x455a14ed);
    GG (a, b, c, d, block.word[13], 5, 0xa9e3e905);
    GG (d, a, b, c, block.word[2], 9, 0xfcefa3f8);
    GG (c, d, a, b, block.word[7], 14, 0x676f02d9);
    GG (b, c, d, a, block.word[12], 20, 0x8d2a4c8a);

    // Round 3
    HH (a, b, c, d, block.word[5], 4, 0xfffa3942);
    HH (d, a, b, c, block.word[8], 11, 0x8771f681);
    HH (c, d, a, b, block.word[11], 16, 0x6d9d6122);
    HH (b, c, d, a, block.word[14], 23, 0xfde5380c);
    HH (a, b, c, d, block.word[1], 4, 0xa4beea44);
    HH (d, a, b, c, block.word[4], 11, 0x4bdecfa9);
    HH (c, d, a, b, block.word[7], 16, 0xf6bb4b60);
    HH (b, c, d, a, block.word[10], 23, 0xbebfbc70);
    HH (a, b, c, d, block.word[13], 4, 0x289b7ec6);
    HH (d, a, b, c, block.word[0], 11, 0xeaa127fa);
    HH (c, d, a, b, block.word[3], 16, 0xd4ef3085);
    HH (b, c, d, a, block.word[6], 23,  0x4881d05);
    HH (a, b, c, d, block.word[9], 4, 0xd9d4d039);
    HH (d, a, b, c, block.word[12], 11, 0xe6db99e5);
    HH (c, d, a, b, block.word[15], 16, 0x1fa27cf8);
    HH (b, c, d, a, block.word[2], 23, 0xc4ac5665);

    // Round 4
    II (a, b, c, d, block.word[0], 6, 0xf4292244);
    II (d, a, b, c, block.word[7], 10, 0x432aff97);
    II (c, d, a, b, block.word[14], 15, 0xab9423a7);
    II (b, c, d, a, block.word[5], 21, 0xfc93a039);
    II (a, b, c, d, block.word[12], 6, 0x655b59c3);
    II (d, a, b, c, block.word[3], 10, 0x8f0ccc92);
    II (c, d, a, b, block.word[10], 15, 0xffeff47d);
    II (b, c, d, a, block.word[1], 21, 0x85845dd1);
    II (a, b, c, d, block.word[8], 6, 0x6fa87e4f);
    II (d, a, b, c, block.word[15], 10, 0xfe2ce6e0);
    II (c, d, a, b, block.word[6], 15, 0xa3014314);
    II (b, c, d, a, block.word[13], 21, 0x4e0811a1);
    II (a, b, c, d, block.word[4], 6, 0xf7537e82);
    II (d, a, b, c, block.word[11], 10, 0xbd3af235);
    II (c, d, a, b, block.word[2], 15, 0x2ad7d2bb);
    II (b, c, d, a, block.word[9], 21, 0xeb86d391);

    // Add to hash value
    h[0] += a;
    h[1] += b;
    h[2] += c;
    h[3] += d;
}


void DoMd5Little(const uint8_t* poolIn, const uint64_t inputBytesNum, uint8_t hash[16])
{
    struct Md5Block block;

    ///initialize hash value
    uint32_t h[4];
    h[0] = 0x67452301;
    h[1] = 0xEFCDAB89;
    h[2] = 0x98BADCFE;
    h[3] = 0x10325476;

    ///padding and divide input data into blocks
    uint64_t fullLen = (inputBytesNum >> 6) << 6;    ///complete blocked length
    uint64_t partLen = inputBytesNum & 0x3F;        ///length remained

    uint32_t i;
    for (i = 0; i < fullLen; i += 64)
    {
        ///copy input data into block
        memcpy(block.word, &(poolIn[i]), 64);

        ///calculate Md5
        CalMd5(block, h);
    }


    if (partLen > 55)        ///append two more blocks
    {
        ///copy input data into block and pad
        memcpy(block.word, &(poolIn[i]), partLen);
        memcpy( ((uint8_t*)&(block.word[partLen >> 2])) + (partLen & 0x3), gPadding, (64 - partLen));

        ///calculate Md5
        CalMd5(block, h);

        ///set rest byte to 0x0
        memset(block.word, 0x0, 64);
    }
    else        ///append one more block
    {
        ///copy input data into block and pad
        memcpy(block.word, &(poolIn[i]), partLen);
        memcpy( ((uint8_t*)&(block.word[partLen >> 2])) + (partLen & 0x3), gPadding, (64 - partLen));
    }

    ///append length (bits)
    uint64_t bitsNum = inputBytesNum * 8;
    memcpy(&(block.word[14]), &bitsNum, 8);

    ///calculate Md5
    CalMd5(block, h);

    ///clear sensitive information
    memset(block.word, 0, 64);

    ///fill hash value
    memcpy(&(hash[0]), &(h[0]), 16);
}///DoMd5Little

void DoMd5Big(const uint8_t* poolIn, const uint64_t inputBytesNum, uint8_t hash[16])
{
    struct Md5Block block;
    uint8_t tempBlock[64];

    ///initialize hash value
    uint32_t h[4];
    h[0] = 0x67452301;
    h[1] = 0xEFCDAB89;
    h[2] = 0x98BADCFE;
    h[3] = 0x10325476;

    ///padding and divide input data into blocks
    uint64_t fullLen = (inputBytesNum >> 6) << 6;
    uint64_t partLen = inputBytesNum & 0x3F;

    uint32_t i;
    for (i = 0; i < fullLen; i += 64)
    {
        ///copy input data into block, in little endian
        CopyBytesToBlock(&(poolIn[i]), block);

        ///calculate Md5
        CalMd5(block, h);
    }

    ///append two more blocks
    if (partLen > 55)
    {
        ///put input data into a temporary block
        memcpy(tempBlock, &(poolIn[i]), partLen);
        memcpy(&(tempBlock[partLen]), gPadding, (64 - partLen));

        ///copy temporary data into block, in little endian
        CopyBytesToBlock(tempBlock, block);

        ///calculate Md5
        CalMd5(block, h);

        memset(tempBlock, 0x0, 64);
    }
    ///append one more block
    else
    {
        memcpy(tempBlock, &(poolIn[i]), partLen);
        memcpy( &(tempBlock[partLen]), gPadding, (64 - partLen));
    }
    ///append length (bits)
    uint64_t bitsNum = inputBytesNum * 8;
    memcpy(&(tempBlock[56]), &bitsNum, 8);

    ///copy temporary data into block, in little endian
    CopyBytesToBlock(tempBlock, block);

    ///calculate Md5
    CalMd5(block, h);

    ///clear sensitive information
    memset(block.word, 0, 64);
    memset(tempBlock, 0, 64);

    ///fill hash value
    memcpy(&(hash[0]), &(h[0]), 16);
}///DoMd5Big

void DoMd5(const uint8_t* poolIn, const uint64_t inputBytesNum, uint8_t md5[16])
{
    ///detect big or little endian
    union
    {
        uint32_t a;
        uint8_t b;
    } symbol;

    symbol.a = 1;

    ///for little endian
    if (symbol.b == 1)
    {
        DoMd5Little(poolIn, inputBytesNum, md5);
    }
    ///for big endian
    else
    {
        DoMd5Big(poolIn, inputBytesNum, md5);
    }
}///DoMd5

/*
* define the rotate left (circular left shift) operation
*/
#define rotl(v,b) (((v)<<(b))|((v)>>(32-(b))))

/*
* Define the basic SHA-1 functions F1 ~ F4. Note that the exclusive-OR
* operation (^) in F1 and F3 may be replaced by a bitwise OR operation
* (|), which produce identical results. 
*
* F1 is used in ROUND  0~19, F2 is used in ROUND 20~39
* F3 is used in ROUND 40~59, F4 is used in ROUND 60~79
*/
#define F1(B,C,D) (((B)&(C))^(~(B)&(D)))
#define F2(B,C,D) ((B)^(C)^(D))
#define F3(B,C,D) (((B)&(C))^((B)&(D))^((C)&(D)))
#define F4(B,C,D) ((B)^(C)^(D))

/*
* Use different K in different ROUND
*/
#define K00_19 0x5A827999
#define K20_39 0x6ED9EBA1
#define K40_59 0x8F1BBCDC
#define K60_79 0xCA62C1D6

/*
* Another implementation of the ROUND transformation:
* (here the T is a temp variable)
* For t=0 to 79:
* {
*     T=rotl(A,5)+Func(B,C,D)+K+W[t]+E;
*     E=D; D=C; C=rotl(B,30); B=A; A=T;
* }
*/
#define ROUND(t,A,B,C,D,E,Func,K) E+=rotl(A,5)+Func(B,C,D)+W[t]+K; B=rotl(B,30);

#define ROUND5(t,Func,K)  ROUND(t,A,B,C,D,E,Func,K);\
                          ROUND(t+1,E,A,B,C,D,Func,K);\
                          ROUND(t+2,D,E,A,B,C,Func,K );\
                          ROUND(t+3,C,D,E,A,B,Func,K);\
                          ROUND(t+4,B,C,D,E,A,Func,K)

#define ROUND20(t,Func,K) ROUND5(t,Func,K);\
                          ROUND5(t+5,Func,K);\
                          ROUND5(t+10,Func,K);\
                          ROUND5(t+15,Func,K)

/*
* Define constant of the initial vector
*/
const uint32_t SHA1::IV[SHA1_DIGEST_WORDS] = {
    0x67452301, 
    0xEFCDAB89, 
    0x98BADCFE, 
    0x10325476, 
    0xC3D2E1F0
};

/*
* the message must be the big-endian32 (or left-most word) 
* before calling the transform() function.
*/
const static uint32_t iii=1;
const static bool littleEndian = *(uint8_t*)&iii!=0;
inline void	make_big_endian32(uint32_t *data, unsigned n) 
{
    if (littleEndian) for (; n>0; ++data,--n) *data = __cpu_to_be32(*data);
}

inline size_t min(size_t a,size_t b) {return a<b ? a : b;}

void SHA1::transform()
{   
    uint32_t W[80];
    memcpy(W, M, SHA1_INPUT_BYTES);
    memset((uint8_t*)W+SHA1_INPUT_BYTES, 0, sizeof(W)-SHA1_INPUT_BYTES);
    for (unsigned t=16; t<80; t++) {
        W[t] = rotl(W[t-16]^W[t-14]^W[t-8]^W[t-3],1);
    }

    uint32_t A = H[0];
    uint32_t B = H[1];
    uint32_t C = H[2];
    uint32_t D = H[3];
    uint32_t E = H[4];

    ROUND20( 0,F1,K00_19);
    ROUND20(20,F2,K20_39);
    ROUND20(40,F3,K40_59);
    ROUND20(60,F4,K60_79);
  
    H[0] += A;
    H[1] += B;
    H[2] += C;
    H[3] += D;
    H[4] += E;
}

void SHA1::add(const uint8_t* data, size_t data_len)
{
    unsigned mlen=(unsigned)((bits>>3)%SHA1_INPUT_BYTES);
    bits += (uint64_t)data_len<<3;
    unsigned use = (unsigned)min((size_t)(SHA1_INPUT_BYTES - mlen), data_len);
    memcpy(M+mlen, data, use); 
    mlen += use;

    while (mlen==SHA1_INPUT_BYTES) {
        data_len -= use; 
        data += use;
        make_big_endian32((uint32_t*)M, SHA1_INPUT_WORDS);
        transform(); 
        use=(unsigned)min((size_t)SHA1_INPUT_BYTES, data_len);
        memcpy(M,data,use); 
        mlen=use;
    }
}

uint8_t *SHA1::result()
{
    unsigned mlen = (unsigned)((bits>>3)%SHA1_INPUT_BYTES), padding = SHA1_INPUT_BYTES - mlen;
    M[mlen++] = 0x80;
    if (padding > BIT_COUNT_BYTES) {
        memset(M+mlen, 0x00, padding-BIT_COUNT_BYTES);
        make_big_endian32((uint32_t*)M, SHA1_INPUT_WORDS-BIT_COUNT_WORDS);
    } 
    else {
        memset(M+mlen, 0x00, SHA1_INPUT_BYTES - mlen);
        make_big_endian32((uint32_t*)M, SHA1_INPUT_WORDS);
        transform();
        memset(M, 0x00, SHA1_INPUT_BYTES-BIT_COUNT_BYTES);
    }

    uint64_t temp = littleEndian ? bits<<32 | bits>>32 : bits;
    memcpy(M + SHA1_INPUT_BYTES - BIT_COUNT_BYTES, &temp, BIT_COUNT_BYTES);
    transform();
    make_big_endian32(H, SHA1_DIGEST_WORDS);
    return (uint8_t*)H;
}

template<typename T>
inline void axor(T *p1, const T *p2, size_t len) 
{
    for (; len != 0; --len) *p1++ ^= *p2++;
}

HMAC::HMAC(const uint8_t *key, size_t lkey)
{
    init(key, lkey);
}

void HMAC::init(const uint8_t *key, size_t lkey)
{
    in.init();
    out.init();
    
    uint8_t ipad[SHA1_INPUT_BYTES];
    uint8_t opad[SHA1_INPUT_BYTES];
    memset(ipad, 0x36, sizeof(ipad)); 
    memset(opad, 0x5c, sizeof(opad));
    
    if (lkey <= SHA1_INPUT_BYTES) {
        axor(ipad, key, lkey); 
        axor(opad, key, lkey);
    }
    else {
		SHA1 tmp; 
        tmp.add(key, lkey); 
		const uint8_t *key2 = tmp.result();
		axor(ipad, key2, SHA1_DIGEST_BYTES); 
		axor(opad, key2, SHA1_DIGEST_BYTES);
    }
    
    in.add((uint8_t*)ipad, sizeof(ipad)); 
    out.add((uint8_t*)opad, sizeof(opad));
}

void Base64Encoding(std::istream& is, std::ostream& os, char makeupChar, const char *alphabet)
{
    int out[4];
    int remain = 0;
    while (!is.eof())
    {
        int byte1 = is.get();
        if (byte1 < 0)
        {
            break;
        }
        int byte2 = is.get();
        int byte3;
        if (byte2 < 0)
        {
            byte2 = 0;
            byte3 = 0;
            remain = 1;
        }
        else
        {
            byte3 = is.get();
            if (byte3 < 0)
            {
                byte3 = 0;
                remain = 2;
            }
        }
        out[0] = static_cast<unsigned>(byte1) >> 2;
        out[1] = ((byte1 & 0x03) << 4) + (static_cast<unsigned>(byte2) >> 4);
        out[2] = ((byte2 & 0x0F) << 2) + (static_cast<unsigned>(byte3) >> 6);
        out[3] = byte3 & 0x3F;

        if (remain == 1)
        {
            os.put(out[0] = alphabet[out[0]]);
            os.put(out[1] = alphabet[out[1]]);
            os.put(makeupChar);
            os.put(makeupChar);
        }
        else if (remain == 2)
        {
            os.put(out[0] = alphabet[out[0]]);
            os.put(out[1] = alphabet[out[1]]);
            os.put(out[2] = alphabet[out[2]]);
            os.put(makeupChar);
        }
        else
        {
            os.put(out[0] = alphabet[out[0]]);
            os.put(out[1] = alphabet[out[1]]);
            os.put(out[2] = alphabet[out[2]]);
            os.put(out[3] = alphabet[out[3]]);
        }
    }
}

bool CompressAlgorithm::CompressLz4(const char*  srcPtr,  const uint32_t srcSize, std::string& dst)
{
    uint32_t encodingSize = LZ4_compressBound(srcSize);
    dst.resize(encodingSize);
    char* compressed = const_cast<char*>(dst.c_str());
    try
    {   
        encodingSize = LZ4_compress(srcPtr, compressed, srcSize);
        if (encodingSize)
        {  
            dst.resize(encodingSize);
            return true;
        }
    }
    catch (...)
    {   
    }
    return false;
}

bool CompressAlgorithm::CompressLz4(const std::string& src, std::string& dst)
{
    return CompressLz4(src.c_str(), src.length(), dst);
}

bool CompressAlgorithm::UncompressLz4(const char*  srcPtr, const uint32_t srcSize, const uint32_t rawSize, std::string& dst)
{
    dst.resize(rawSize);
    char* unCompressed = const_cast<char*>(dst.c_str());
    uint32_t length = 0;
    try
    {
        length = LZ4_decompress_safe(srcPtr, unCompressed, srcSize, rawSize);
    }
    catch(...)
    {
        return false;
    }
    if (length != rawSize)
    {   
        return false;
    }
    return true;
}

bool CompressAlgorithm::UncompressLz4(const std::string& src, const uint32_t rawSize, std::string& dst)
{
    return UncompressLz4(src.c_str(), src.length(), rawSize, dst);
}
template<class T>
inline std::string SignedToString(const T& n)
{
    long long ll = static_cast<long long>(n);
    char buf[64];
    snprintf(buf, sizeof(buf), "%lld", ll);
    return std::string(buf);
}
std::string ToString(const int32_t& n)
{
    return SignedToString(n);
}

template<class T>
inline std::string UnsignedToString(const T& n)
{
    unsigned long long llu = static_cast<unsigned long long>(n);
    char buf[64];
    snprintf(buf, sizeof(buf), "%llu", llu);
    return std::string(buf);
}


std::string ToString(const uint32_t& n)
{
    return UnsignedToString(n);
}

std::string ToString(const size_t&n)
{
    return UnsignedToString(n);
}
std::string ToString(const int64_t& n)
{
    return SignedToString(n);
}
std::string ToString(const bool& n)
{
    if(n)
        return "true";
    else
        return "false";
}
}
