#include "bcrypt.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <random>

namespace utils {

std::string generateSalt(int length) {
    unsigned char buffer[32];
    if (RAND_bytes(buffer, length) != 1) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        for (int i = 0; i < length; ++i) {
            buffer[i] = static_cast<unsigned char>(dis(gen));
        }
    }
    std::ostringstream oss;
    for (int i = 0; i < length; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]);
    }
    return oss.str();
}

std::string sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final(hash, &sha256);

    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return oss.str();
}

std::string hashPassword(const std::string& password) {
    std::string salt = generateSalt();
    std::string hash = sha256(salt + password);
    return salt + ":" + hash;
}

bool verifyPassword(const std::string& password, const std::string& stored) {
    size_t pos = stored.find(':');
    if (pos == std::string::npos) return false;
    std::string salt = stored.substr(0, pos);
    std::string expectedHash = stored.substr(pos + 1);
    std::string computedHash = sha256(salt + password);
    return computedHash == expectedHash;
}

} // namespace utils
