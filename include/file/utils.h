#ifndef VEST_FILE_UTILS_H
#define VEST_FILE_UTILS_H

#include <string>
#include <vector>
#include <file/types.h>

namespace VestFileUtils {

    std::vector<std::string> listSubEntries(std::vector<std::string>& entries, std::string rootPath);

    std::string byteToHex(uint8_t byte);
    std::string byteToBinary(uint8_t byte);
    void printHexAndBinary(uint8_t byte);

    std::string constructFileLine(
        VestTypes::FileType& fileType,
        std::string& sha1Hex,
        std::string& fileName
    );

    std::string computeSHA1(std::string& inputData);
    std::string computeSHA1(const std::vector<unsigned char>& data);

    std::string constructfPath(std::string fileID, std::string root = ".git/objects/");
    std::string hexToBytes(const std::string& hex);
}

#endif // VEST_FILE_UTILS_H