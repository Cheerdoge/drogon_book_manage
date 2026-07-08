#include "jwt_util.h"
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <json/json.h>
#include <sstream>
#include <vector>

namespace utils {

static const char BASE64_CHARS[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64UrlEncode(const std::string& input) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.data(), static_cast<int>(input.size()));
    BIO_flush(bio);
    BUF_MEM* buf;
    BIO_get_mem_ptr(bio, &buf);
    std::string result(buf->data, buf->length);
    BIO_free_all(bio);
    for (char& c : result) {
        if (c == '+') c = '-';
        if (c == '/') c = '_';
    }
    result.erase(std::find(result.begin(), result.end(), '='), result.end());
    return result;
}

std::string base64UrlDecode(const std::string& input) {
    std::string s = input;
    for (char& c : s) {
        if (c == '-') c = '+';
        if (c == '_') c = '/';
    }
    while (s.size() % 4) s += '=';

    BIO* bmem = BIO_new_mem_buf(s.data(), static_cast<int>(s.size()));
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bio = BIO_push(b64, bmem);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    std::vector<char> buf(s.size());
    int len = BIO_read(bio, buf.data(), static_cast<int>(s.size()));
    std::string result(buf.data(), len > 0 ? len : 0);
    BIO_free_all(bio);
    return result;
}

std::string hmacSha256(const std::string& key, const std::string& data) {
    unsigned char result[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    HMAC(EVP_sha256(), key.c_str(), static_cast<int>(key.size()),
         reinterpret_cast<const unsigned char*>(data.c_str()),
         data.size(), result, &len);
    return std::string(reinterpret_cast<char*>(result), len);
}

std::string generateJWT(const std::string& secret, const std::string& payloadJson) {
    Json::Value header;
    header["alg"] = "HS256";
    header["typ"] = "JWT";
    Json::FastWriter writer;
    std::string headerB64 = base64UrlEncode(writer.write(header));
    headerB64.erase(std::remove(headerB64.begin(), headerB64.end(), '\n'), headerB64.end());

    std::string payloadB64 = base64UrlEncode(payloadJson);
    payloadB64.erase(std::remove(payloadB64.begin(), payloadB64.end(), '\n'), payloadB64.end());

    std::string signInput = headerB64 + "." + payloadB64;
    std::string signature = hmacSha256(secret, signInput);
    std::string signatureB64 = base64UrlEncode(signature);

    return headerB64 + "." + payloadB64 + "." + signatureB64;
}

bool verifyJWT(const std::string& token, const std::string& secret, std::string& payload) {
    size_t firstDot = token.find('.');
    size_t secondDot = token.find('.', firstDot + 1);
    if (firstDot == std::string::npos || secondDot == std::string::npos) {
        return false;
    }

    std::string headerB64 = token.substr(0, firstDot);
    std::string payloadB64 = token.substr(firstDot + 1, secondDot - firstDot - 1);
    std::string signatureB64 = token.substr(secondDot + 1);

    std::string signInput = headerB64 + "." + payloadB64;
    std::string expectedSig = hmacSha256(secret, signInput);
    std::string expectedSigB64 = base64UrlEncode(expectedSig);

    if (signatureB64 != expectedSigB64) {
        return false;
    }

    payload = base64UrlDecode(payloadB64);
    return true;
}

} // namespace utils
