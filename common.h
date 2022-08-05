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

#define SHA256_INPUT_WORDS    16          
#define SHA256_DIGEST_WORDS   8
#define SHA256_INPUT_BYTES    (SHA256_INPUT_WORDS*sizeof(uint32_t))
#define SHA256_DIGEST_BYTES   (SHA256_DIGEST_WORDS*sizeof(uint32_t))

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
    SHA1() : bits(0) {memcpy(H, IV, sizeof(H));}
    SHA1(const SHA1& s)  
    {   
        bits=s.bits; 
        memcpy(H,s.H,sizeof(H)); 
        memcpy(M,s.M,sizeof(M));
    }   
    void init() 
    {   
        bits = 0;  
        memcpy(H, IV, sizeof(H));
    }   
    void add(const uint8_t *data, size_t len);
    uint8_t *result();
private:
    uint64_t bits;
    uint32_t H[SHA1_DIGEST_WORDS];
    uint8_t M[SHA1_INPUT_BYTES];

    static const uint32_t IV[SHA1_DIGEST_WORDS];
    void transform();
};

// Hmac-sha1
class HMAC
{
public:
    HMAC(const uint8_t *key, size_t lkey);
    HMAC(const HMAC& hm) : in(hm.in), out(hm.out) {}
    
    void init(const uint8_t *key, size_t lkey);
  
    void add(const uint8_t *data, size_t len) 
    {
        in.add(data, len);
    }

    uint8_t *result()
    {
        out.add(in.result(), SHA1_DIGEST_BYTES);
        return out.result();
    }

private:
    SHA1 in, out;

};

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
class SHA256 //: public Hash
{
public:
  /// split into 64 byte blocks (=> 512 bits), hash is 32 bytes long
  enum { BlockSize = 512 / 8, HashBytes = 32 };

  /// same as reset()
  SHA256();

  /// compute SHA256 of a memory block
  std::string operator()(const void* data, size_t numBytes);
  /// compute SHA256 of a string, excluding final zero
  std::string operator()(const std::string& text);

  /// add arbitrary number of bytes
  void add(const void* data, size_t numBytes);

  /// return latest hash as 64 hex characters
  std::string getHexHash();
  /// return latest hash as bytes
  void        getHash(unsigned char buffer[HashBytes]);

  std::string getHash();
  /// restart
  void reset();

private:

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

//hmac-sha256
class HMACSHA256
{
public:
    HMACSHA256(const uint8_t *key, size_t lkey);
    HMACSHA256(const HMACSHA256& hm) : in(hm.in), out(hm.out) {}
    
    void init(const uint8_t *key, size_t lkey);
  
    void add(const uint8_t *data, size_t len) 
    {
        in.add(data, len);
    }

    std::string getHash()
    {
        std::string inHash = in.getHash();
        // add to out
        out.add(inHash.data(), SHA256_DIGEST_BYTES);
        return out.getHash();
    }

private:
    SHA256 in, out;

};

} // end of namespace aliyun_log_sdk_v6


#endif
