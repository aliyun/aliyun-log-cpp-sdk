#pragma once
#include <map>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <vector>

namespace aliyun_log_sdk_v6 {
namespace auth {
extern const char* const EMPTY_STRING_SHA256;  // = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
typedef enum { SIGN_V1, SIGN_V4 } LOGSignType;

struct Credential {
    std::string accessKeySecret, accessKeyId;
    Credential(const std::string& accessKeyId,
               const std::string& accessKeySecret)
        : accessKeyId(accessKeyId), accessKeySecret(accessKeySecret) {}
};

class Signer
{
  public:
   static const LOGSignType DEFAULT_SIGN_VERSION = SIGN_V1; // default sign version
   static const std::string DEFAULT_EMPTY_REGION;  // use when not passing param 'region'
   static void Sign(const std::string& httpMethod,
                    const std::string& resourceUri,
                    std::map<std::string, std::string>& httpHeaders,
                    const std::map<std::string, std::string>& urlParams,
                    const std::string& payload,
                    const std::string& accessKeySecret,
                    const std::string& accessKeyId,
                    const LOGSignType signType = DEFAULT_SIGN_VERSION,
                    const std::string& region = DEFAULT_EMPTY_REGION);
};


class BaseSigner {
   protected:
    Credential mCredential;

   public:
    BaseSigner(const Credential& crendential) : mCredential(crendential) {}
    virtual void Sign(const std::string& httpMethod,
                      const std::string& resourceUri,
                      std::map<std::string, std::string>& httpHeaders,
                      const std::map<std::string, std::string>& urlParameters,
                      const std::string& content) = 0;
};

// sign version v1
class SignerV1 : public BaseSigner {
   public:
    SignerV1(const Credential& crendential) : BaseSigner(crendential) {}
    virtual void Sign(const std::string& httpMethod,
                      const std::string& resourceUri,
                      std::map<std::string, std::string>& httpHeaders,
                      const std::map<std::string, std::string>& urlParameters,
                      const std::string& content);

   private:
    std::string GetDateTimeString();
};

// sign version v4
class SignerV4 : public BaseSigner {
   public:
    using StringTreeMap = std::map<std::string, std::string>;
    using StringHashMap = std::unordered_map<std::string, std::string>;
    SignerV4(const Credential& crendential, const std::string& region)
        : BaseSigner(crendential), mRegion(region) {}
    virtual void Sign(const std::string& httpMethod,
                      const std::string& resourceUri,
                      StringTreeMap& httpHeaders,
                      const StringTreeMap& urlParams,
                      const std::string& payload);

   private:
    std::string BuildScope(const std::string& date);

   private:
    static std::string GetDateTimeString();
    static std::string GetDateString();
    static std::string UrlEncode(const std::string& s, bool ignoreSlash);


    static StringTreeMap BuildCanonicalHeaders(
        const StringTreeMap& httpHeaders);
    static std::string BuildSignedHeaderStr(
        const StringTreeMap& canonicalHeaders);
    static std::string BuildCanonicalRequest(
        const std::string& method,
        const std::string& resourceUri,
        const std::string& signedHeaderStr,
        const std::string& sha256Payload,
        const StringTreeMap& urlParams,
        const StringTreeMap& canonicalHeaders);

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

   public:
    static const std::string SECRET_KEY_PREFIX;  // = "aliyun_v4";
    static const std::string SLS_PRODUCT_NAME;   // = "sls";
    static const std::string TERMINATOR;         // = "aliyun_v4_request";
    static const std::string SLS4_HMAC_SHA256;   // = "SLS4-HMAC-SHA256";
    static const StringHashMap CHARACTER_WITHOUT_SLASH, CHARACTER_WITH_SLASH;
    static const std::unordered_set<std::string> DEFAULT_SIGNED_HEADERS;  // = {"content-type", "host"}
    

   private:
    const std::string mRegion;
};



}  // end of namespace auth
}  // end of namespace aliyun_log_sdk_v6