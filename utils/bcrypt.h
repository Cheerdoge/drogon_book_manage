#pragma once
#include <string>

namespace utils {

std::string generateSalt(int length = 16);
std::string sha256(const std::string& input);
std::string hashPassword(const std::string& password);
bool verifyPassword(const std::string& password, const std::string& stored);

} // namespace utils
