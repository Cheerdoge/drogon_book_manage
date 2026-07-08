#pragma once
#include <string>

namespace utils {

std::string base64UrlEncode(const std::string& input);
std::string base64UrlDecode(const std::string& input);
std::string hmacSha256(const std::string& key, const std::string& data);
std::string generateJWT(const std::string& secret, const std::string& payload);
bool verifyJWT(const std::string& token, const std::string& secret, std::string& payload);

} // namespace utils
