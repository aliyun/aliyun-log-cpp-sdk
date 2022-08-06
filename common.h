#ifndef __LOGV6_SDK_COMMON_H__
#define __LOGV6_SDK_COMMON_H__
#include <inttypes.h>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <vector>

#define SHA1_INPUT_WORDS    16          
#define SHA1_DIGEST_WORDS   5
#define SHA1_INPUT_BYTES    (SHA1_INPUT_WORDS*sizeof(uint32_t))
#define SHA1_DIGEST_BYTES   (SHA1_DIGEST_WORDS*sizeof(uint32_t))

#define BIT_COUNT_WORDS     2
#define BIT_COUNT_BYTES     (BIT_COUNT_WORDS*sizeof(uint32_t))

namespace aliyun_log_sdk_v6
{
/** Calculate Md5 for a byte stream,
* result is stored in md5[16]
*
* @param poolIn Input data
* @param inputBytesNum Length of input data
* @param md5[16] A 128-bit pool for storing md5
*/
void DoMd5(const uint8_t* poolIn, const uint64_t inputBytesNum, uint8_t md5[16]);
void Base64Encoding(std::istream&, std::ostream&, char makeupChar = '=',
        const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

class SHA1
{
   public:
    SHA1()
    {
        reset();
    }
    SHA1(const SHA1& s)
    {
        bits = s.bits;
        memcpy(H, s.H, sizeof(H));
        memcpy(M, s.M, sizeof(M));
    }

    void add(const uint8_t* data, size_t len);
    std::string getHash()
    {
        auto res = result();
        return std::string(reinterpret_cast<const char*>(res), DIGEST_BYTES);
    }
    static constexpr size_t DIGEST_BYTES = 20;
    static constexpr size_t HMAC_BLOCK_SIZE_BYTES = 64;

   private:
    uint64_t bits;
    uint32_t H[SHA1_DIGEST_WORDS];
    uint8_t M[SHA1_INPUT_BYTES];

    static const uint32_t IV[SHA1_DIGEST_WORDS];

   private: 
    void transform();
    uint8_t* result();
    void reset()
    {
        bits = 0;
        memcpy(H, IV, sizeof(H));
    }
};


template <typename H>
class HMAC
{
   public:
    HMAC(const uint8_t* key, size_t lkey)
    {
        init(key, lkey);
    }
    HMAC(const H& hm) : in(hm.in), out(hm.out) {}

    void init(const uint8_t* key, size_t lkey)
    {
        auto blockSize = H::HMAC_BLOCK_SIZE_BYTES;
        uint8_t ipad[blockSize];
        uint8_t opad[blockSize];
        memset(ipad, 0x36, sizeof(ipad));
        memset(opad, 0x5c, sizeof(opad));

        // get (K' ^ ipad) and (K' ^ opad)
        if (lkey <= blockSize)
        {
            axor(ipad, key, lkey);
            axor(opad, key, lkey);
        }
        else
        {
            H tmp;
            tmp.add(key, lkey);
            std::string tmpHash = tmp.getHash();
            const uint8_t* key2 =
                reinterpret_cast<const uint8_t*>(tmpHash.data());
            axor(ipad, key2, blockSize);
            axor(opad, key2, blockSize);
        }

        in.add((uint8_t*)ipad, sizeof(ipad));   // (K' ^ ipad)
        out.add((uint8_t*)opad, sizeof(opad));  // (K' ^ opad)
        // hmac.add => in.add => (K' ^ ipad) | M
        // hmac.results =>   H((K' ^ opad) | H((K' ^ ipad) | M))
    }

    void add(const uint8_t* data, size_t len)
    {
        in.add(data, len);
    }

    std::string getHash()
    {
        std::string inHash = in.getHash();
        // add to out
        out.add(reinterpret_cast<const uint8_t*>(inHash.data()),
                H::DIGEST_BYTES);
        return out.getHash();
    }

   private:
    H in, out;
    template <typename T>
    static inline void axor(T* p1, const T* p2, size_t len)
    {
        for (; len != 0; --len) *p1++ ^= *p2++;
    }
};

using HMACSHA1 = HMAC<SHA1>;

class CompressAlgorithm
{
public:
    static bool CompressLz4(const std::string& src, std::string& dst);
    static bool CompressLz4(const char*  srcPtr,  const uint32_t srcSize, std::string& dest);
    static bool UncompressLz4(const std::string& src, const uint32_t rawSize, std::string& dst);
    static bool UncompressLz4(const char*  srcPtr,  const uint32_t srcSize, const uint32_t rawSize, std::string& dst);
};
std::string ToString(const int32_t& n); 
std::string ToString(const uint32_t& n); 
std::string ToString(const size_t&n);
std::string ToString(const time_t&n);
std::string ToString(const int64_t& n);
std::string ToString(const bool& n);

// Thanks to Stephan Brumme.
// //////////////////////////////////////////////////////////
// sha256.h
// Copyright (c) 2014,2015 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//
class SHA256
{
public:
  SHA256() { reset(); };
  SHA256(const SHA256& other)
  {
      m_bufferSize = other.m_bufferSize;
      m_numBytes = other.m_numBytes;
      memcpy(m_buffer, other.m_buffer, sizeof(m_buffer));
      memcpy(m_hash, other.m_hash, sizeof(m_hash));
  }  
  void add(const uint8_t* data, size_t numBytes)
  {
      add(reinterpret_cast<const void *>(data), numBytes);
  }
  std::string getHash();

  static constexpr size_t DIGEST_BYTES = 32;
  static constexpr size_t HMAC_BLOCK_SIZE_BYTES = 64;

public:
  void reset();
  /// return latest hash as 64 hex characters
  std::string getHexHash();
  /// add arbitrary number of bytes
  void add(const void* data, size_t numBytes);
  /// compute SHA256 of a memory block
  std::string operator()(const void* data, size_t numBytes);
  /// compute SHA256 of a string, excluding final zero
  std::string operator()(const std::string& text);

private:
  /// split into 64 byte blocks (=> 512 bits), hash is 32 bytes long
  enum { BlockSize = 512 / 8, HashBytes = 32 };
  /// return latest hash as bytes
  void getHash(unsigned char buffer[HashBytes]);

  /// process 64 bytes
  void processBlock(const void* data);
  /// process everything left in the internal buffer
  void processBuffer();

  /// size of processed data in bytes
  uint64_t m_numBytes;
  /// valid bytes in m_buffer
  size_t   m_bufferSize;
  /// bytes not processed yet
  uint8_t  m_buffer[BlockSize];

  enum { HashValues = HashBytes / 4 };
  /// hash, stored as integers
  uint32_t m_hash[HashValues];
};

using HMACSHA256 = HMAC<SHA256>;


} // end of namespace aliyun_log_sdk_v6


#endif
