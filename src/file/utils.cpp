#include <string>
#include <iostream>
#include <openssl/sha.h>

#include <utils.h>

#include <file/types.h>
#include <file/utils.h>

namespace VestFileUtils {

    std::string byteToHex(uint8_t byte) {
        std::ostringstream oss;
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        return oss.str();
    }

    std::string byteToBinary(uint8_t byte) {
        std::string binary;
        for (int i = 7; i >= 0; --i) { // Loop through each bit
            binary += (byte & (1 << i)) ? '1' : '0';
        }
        return binary;
    }

    void printHexAndBinary(uint8_t byte) {
        byteToHex(byte);
        std::cout << " | ";
        byteToBinary(byte);
        std::cout << "\x0A";
    }

    std::string constructFileLine(
        VestTypes::FileType& fileType,
        std::string& sha1Hex,
        std::string& fileName
    ) {

        std::string sha1Bytes = hexToBytes(sha1Hex);
        return std::string(fileType.mode) + " " + fileName + '\x00' + sha1Bytes;

    }

    std::string hexToBytes(const std::string& hex) {
        std::string bytes;
        for (size_t i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            char byte = static_cast<char>(strtol(byteString.c_str(), nullptr, 16));
            bytes.push_back(byte);
        }
        return bytes;
    }

    std::string computeSHA1(std::string& inputData) {
        std::vector<unsigned char> data(inputData.begin(), inputData.end());
        return computeSHA1(data);
    }

    std::string computeSHA1(const std::vector<unsigned char>& data) {
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1(data.data(), data.size(), hash);

        // Convert to hexadecimal string
        std::stringstream ss;
        for (uint8_t i {}; i < SHA_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }

        return ss.str();
    }

    std::string constructfPath(std::string fileID, std::string root) {
        std::ostringstream fPath {};
        fPath << root << fileID[0] << fileID[1] << '/' + fileID.substr(2);

        return fPath.str();
    }

    std::vector<std::string> listSubEntries(std::vector<std::string>& entries, std::string rootPath) {
        std::filesystem::path absPath = std::filesystem::absolute(rootPath);

        for (const auto& entry : std::filesystem::directory_iterator(absPath)) {

            std::string str = entry.path().string();

            if (entry.path().filename().string() == ".git") continue;
            entries.push_back(str);

            if (std::filesystem::is_directory(entry)) listSubEntries(entries, str);
        }

        if (entries.empty()) PRINT_WARNING("ROOT_PATH: " + absPath.string() + " IS EMTPY");

        return entries;
    }

}
