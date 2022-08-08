#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "RestfulApiCommon.h"
#include "adapter.h"


// Return debugVal if not empty, else returns func(__args__)
#ifndef SLS_STRING_VALUE_IF_NOT_EMPTY
#define SLS_STRING_VALUE_IF_NOT_EMPTY(debugVal, func, ...) \
        (debugVal.empty() ? func(##__VA_ARGS__) : debugVal) 
#endif

namespace aliyun_log_sdk_v6
{
namespace auth
{
struct Credential
{
    std::string accessKeySecret, accessKeyId;
    Credential(const std::string& accessKeyId,
               const std::string& accessKeySecret)
        : accessKeyId(accessKeyId), accessKeySecret(accessKeySecret)
    {
    }
};

// virtual class, as base class
class Signer
{
   public:
    // for sign version v4, param 'region' is required to be non-empty
    static void Sign(const std::string& httpMethod,
                     const std::string& resourceUri,
                     std::map<std::string, std::string>& httpHeaders,
                     const std::map<std::string, std::string>& urlParams,
                     const std::string& payload, const std::string& accessKeyId,
                     const std::string& accessKeySecret,
                     const LOGSignVersion version,
                     const std::string& region = DEFAULT_EMPTY_REGION);

   public:
    Signer(const Credential& crendential) : mCredential(crendential) {}
    virtual void Sign(const std::string& httpMethod,
                      const std::string& resourceUri,
                      std::map<std::string, std::string>& httpHeaders,
                      const std::map<std::string, std::string>& urlParameters,
                      const std::string& payload) = 0;

   protected:
    Credential mCredential;

   private:
    static const std::string
        DEFAULT_EMPTY_REGION;  // = '', use when not passing param 'region'
};

// sign version v1
class SignerV1 : public Signer
{
   public:
    SignerV1(const Credential& crendential) : Signer(crendential) {}
    virtual void Sign(const std::string& httpMethod,
                      const std::string& resourceUri,
                      std::map<std::string, std::string>& httpHeaders,
                      const std::map<std::string, std::string>& urlParameters,
                      const std::string& content);

   private:
    static std::string GetDateTimeString();
    std::string mDebugDateTime; // for unittest

#ifdef __ALIYUN_LOG_UNITTEST__
    friend class SignerUnittestHelper;
#endif
};

// sign version v4
class SignerV4 : public Signer
{
   private:
    using StringTreeMap = std::map<std::string, std::string>;
    using StringHashMap = std::unordered_map<std::string, std::string>;

   public:
    SignerV4(const Credential& crendential, const std::string& region)
        : Signer(crendential), mRegion(region)
    {
    }
    virtual void Sign(const std::string& httpMethod,
                      const std::string& resourceUri,
                      StringTreeMap& httpHeaders,
                      const StringTreeMap& urlParams,
                      const std::string& payload);

   private:
    std::string BuildScope(const std::string& date) const;

   private:
    static std::string GetDateTimeString();
    static std::string GetDateString();
    static std::string UrlEncode(const std::string& s, bool ignoreSlash);

    static StringTreeMap BuildCanonicalHeaders(
        const StringTreeMap& httpHeaders);
    static std::string BuildSignedHeaderStr(
        const StringTreeMap& canonicalHeaders);
    static std::string BuildCanonicalRequest(
        const std::string& method, const std::string& resourceUri,
        const std::string& signedHeaderStr, const std::string& sha256Payload,
        const StringTreeMap& urlParams, const StringTreeMap& canonicalHeaders);

    static std::string BuildSignMessage(const std::string& canonicalRequest,
                                        const std::string& dateTime,
                                        const std::string& scope);
    static std::string BuildSignKey(const std::string& accessKeySecret,
                                    const std::string& region,
                                    const std::string& date);
    static std::string BuildAuthorization(const std::string& accessKeyId,
                                          const std::string& signHeaderStr,
                                          const std::string& signature,
                                          const std::string& scope);

   private: // constants
    static const std::string SECRET_KEY_PREFIX;  // = "aliyun_v4";
    static const std::string SLS_PRODUCT_NAME;   // = "sls";
    static const std::string TERMINATOR;         // = "aliyun_v4_request";
    static const std::string SLS4_HMAC_SHA256;   // = "SLS4-HMAC-SHA256";
    static const std::string
        EMPTY_STRING_SHA256;  // =
                              // "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
    static const StringHashMap CHARACTER_WITHOUT_SLASH, CHARACTER_WITH_SLASH;
    static const std::unordered_set<std::string>
        DEFAULT_SIGNED_HEADERS;  // = {"content-type", "host"}

   private:
    const std::string mRegion;
    std::string mDebugDateTime, mDebugDate; // for unittest

#ifdef __ALIYUN_LOG_UNITTEST__
    friend class SignerUnittestHelper;
#endif
};

}  // end of namespace auth
}  // end of namespace aliyun_log_sdk_v6