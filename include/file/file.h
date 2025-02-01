#ifndef VEST_FILE_H
#define VEST_FILE_H

#include <string>
#include <fstream>
#include <cstdint>
#include <filesystem>

#include <file/types.h>
#include <file/pack.h>

namespace VestFile {

    void saveToFile(const std::string& fPath, const std::vector<unsigned char>& content);

    uint8_t getFileType(std::string& fPath);
    uint8_t getFileType(std::string&& fPath);

    std::vector<unsigned char> readFile(std::string& fPath);
    std::vector<unsigned char> readFile(std::string&& fPath);
    std::vector<unsigned char> compressData(std::string& inputData);
    std::vector<unsigned char> compressData(const std::vector<unsigned char>& inputData);

    VestTypes::DecompressedData decompressData(
        std::vector<uint8_t>& compressedData,
        size_t decompressedSize
    );

    VestTypes::DecompressedData decompressData(
        std::vector<uint8_t>& compressedData
    );

    VestTypes::CommitFile* readCommit(std::string& fContent, bool fromPack = true);
    VestTypes::TreeFile* readTreeFile(std::string& fContent);
}

#endif // VEST_FILE_H