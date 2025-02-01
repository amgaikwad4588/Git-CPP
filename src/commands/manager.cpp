#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <zlib.h>
#include <cstring>

#include <utils.h>

#include <file/file.h>
#include <file/types.h>
#include <file/utils.h>
#include <file/pack.h>

#include <request/utils.h>

#include <objects/initializers.h>

#include <commands/manager.h>

namespace Vest {

    CommandManager::CommandManager() :
        actions {
            {INIT, [this](int argc, char *argv[]) { return actionForInit(argc, argv); }},
            {CLONE, [this](int argc, char *argv[]) { return actionForClone(argc, argv); }},
            {LS_TREE, [this](int argc, char *argv[]) { return actionForLsTree(argc, argv); }},
            {CAT_FILE, [this](int argc, char *argv[]) { return actionForCatFile(argc, argv); }},
            {WRITE_TREE, [this](int argc, char *argv[]) { return actionForWriteTree(argc, argv); }},
            {HASH_OBJECT, [this](int argc, char *argv[]) { return actionForHashObject(argc, argv); }},
            {COMMIT_TREE, [this](int argc, char *argv[]) { return actionForCommitTree(argc, argv); }},
        } {}
    CommandManager::~CommandManager() {}

    uint8_t CommandManager::processCommand(int& argc, char *argv[]) {

        std::string command {argv[1]};

        if (argc < 2) return EXIT_FAILURE;

        if (!actions.count(command)) {
            PRINT_ERROR("COMMAND NOT FOUND: " + command);
            return EXIT_FAILURE;
        }

        uint8_t r {};
        try {
            r = actions[command](argc, argv);
        } catch (const std::filesystem::filesystem_error& e) {
            PRINT_ERROR("ERROR WHILE PROCESSING COMMAND: " + command);
            PRINT_ERROR("ERROR: " + std::string(e.what()));
        }

        return r;
    }

    uint8_t CommandManager::actionForInit(int argc, char* argv[]) {
        return VestObjects::initializeVest();
    }

    uint8_t CommandManager::actionForCatFile(int argc, char* argv[]) {

        std::string parameter = argv[2];
        std::string fileID = argv[3];
        std::string fPath {VestFileUtils::constructfPath(fileID)};
        std::vector<uint8_t> compressedData {VestFile::readFile(fPath)};

        if (compressedData.empty()) {
            PRINT_ERROR("THE FILE IS EMPTY");
            return EXIT_FAILURE;
        }

        // PRINT_WARNING("TO READ: " + std::to_string(compressedData.size()) + " BYTES FROM FILE");

        VestTypes::DecompressedData data {VestFile::decompressData(compressedData)}; if (data.isEmpty()) return EXIT_FAILURE;
        unsigned char* bufferData = data.data.data() + 5;  // Start 5 bytes into the buffer this to avoid blob

        // Find the null character '\x00'
        while (*bufferData != '\x00') {
            bufferData++;
        }
        // Move past the null character
        bufferData++;

        uint8_t offset = bufferData - data.data.data();
        std::cout.write(reinterpret_cast<char*>(bufferData), data.stream.total_out - offset);

        inflateEnd(&data.stream);
        return EXIT_SUCCESS;

    }

    uint8_t CommandManager::actionForHashObject(int argc, char* argv[]) {

        std::string var {argv[2]};
        std::string fPath {argv[3]};
        std::string sha1 = VestObjects::createBlob(fPath);
        std::cout << sha1;

        return EXIT_SUCCESS;
    }

    uint8_t CommandManager::actionForLsTree(int argc, char* argv[]) {
        std::string parameter = argv[2];
        std::string fSha1 = argv[3];
        std::string fPath {VestFileUtils::constructfPath(fSha1)};
        std::vector<uint8_t> compressedData {VestFile::readFile(fPath)};

        VestTypes::DecompressedData data {VestFile::decompressData(compressedData)};

        unsigned char* bufferData = data.data.data();
        unsigned char* completeString {bufferData}; // This modifies the original data but is more efficient
        unsigned char* toOut {completeString};

        bool isListening {};

        uLong& totalOut = data.stream.total_out;

        while (totalOut-- > 0) {
            unsigned char c {*bufferData};

            if (c == '\x00' && !isListening) {
                isListening = true;
            }
            else if (c == '\x00' && isListening) {
                *completeString++ = '\n';
                bufferData += VestTypes::SHA_BYTES_SIZE;
                totalOut -= VestTypes::SHA_BYTES_SIZE;
            }

            bufferData++;
            if (!isListening) continue;

            if (std::isalpha(c)) {
                *completeString++ = c;
            }

        }

        std::cout.write(reinterpret_cast<char*>(toOut), completeString - toOut);

        inflateEnd(&data.stream);
        return EXIT_SUCCESS;

    }

    uint8_t CommandManager::actionForWriteTree(int argc, char* argv[]) {

        std::filesystem::path rootPath {std::filesystem::current_path()};

        std::string sha1 = VestObjects::createTree(rootPath);
        std::cout << sha1;

        return EXIT_SUCCESS;
    }

    uint8_t CommandManager::actionForCommitTree(int argc, char* argv[]) {

        std::string tSha1 = argv[2];
        std::string parent {};
        std::string commitMsg {};

        // check if we have a parent
        for (uint8_t i {}; i < argc; i++) {
            if (std::string(argv[i]) == "-p") {parent = argv[i + 1]; i++; continue;}
            if (std::string(argv[i]) == "-m") {commitMsg = argv[i + 1]; i++; continue;}
        }

        std::string sha1 = VestObjects::createCommit(tSha1, parent, commitMsg);
        std::cout << sha1;

        return EXIT_SUCCESS;
    }

    uint8_t CommandManager::actionForClone(int argc, char* argv[]) {

        std::string bUrl = argv[2];
        std::string dir = argv[3];

        // Error on git-sample-2

        // bUrl = "https://github.com/codecrafters-io/git-sample-1";
        PRINT_HIGHLIGHT("USING : " + bUrl);

        if (dir[dir.size() - 1] != '/') dir += '/';
        if (VestObjects::initializeVest(dir) == EXIT_FAILURE) return EXIT_FAILURE;

        std::string sha1Head {};
        VestRequest::getSha1Head(
            std::string(bUrl + "/info/refs?service=git-upload-pack").c_str(),
            sha1Head
        );

        std::vector<uint8_t> rData {};
        std::vector<std::string> wSha1 {sha1Head};
        uint8_t r = VestRequest::requestFilesToGit(
            std::string(bUrl + ".git/git-upload-pack").c_str(),
            rData,
            wSha1
        );

        VestPack::processPack(rData, 12, dir);

        return EXIT_SUCCESS;
    }

}
