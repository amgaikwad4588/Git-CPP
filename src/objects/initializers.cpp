#include <string>
#include <map>

#include <utils.h>

#include <file/types.h>
#include <file/file.h>
#include <file/utils.h>

#include <objects/helpers.h>

namespace VestObjects {

    uint8_t initializeVest(std::string dir) {

        if (!dir.empty()) {
            if (dir[dir.size() - 1] != '/') dir += '/';
            std::filesystem::create_directories(dir);
        }

        std::filesystem::create_directory(dir + ".git");
        std::filesystem::create_directory(dir + ".git/objects");
        std::filesystem::create_directory(dir + ".git/refs");

        std::ofstream headFile(dir + ".git/HEAD");
        if (headFile.is_open()) {
            headFile << "ref: refs/heads/main\n";
            headFile.close();
        } else {
            PRINT_ERROR("FAILED TO CREATE .git/HEAD file.");
            return EXIT_FAILURE;
        }

        PRINT_SUCCESS("INITIALIZED VEST DIRECTORY");
        return EXIT_SUCCESS;
    }

    std::string createBlob(std::string& fPath) {
        std::vector<unsigned char> fContent = VestFile::readFile(fPath);
        std::string blob = prepareBlob(fContent);
        std::string sha1 = writeObject(blob);

        return sha1;
    }

    std::string createCommit(std::string& fContent, std::string dir) {
        std::string commit = prepareCommit(fContent);
        std::string sha1 = writeObject(commit, dir);

        return sha1;
    }

    std::string createCommit(
        std::string& tSha1,
        std::string& parent,
        std::string commitMsg,
        std::string dir
    ) {
        std::ostringstream fContent {};
        fContent << "tree " << tSha1 << "\x0A";

        if (!parent.empty()) fContent << "parent " << parent + "\x0A";

        fContent << "author root <root@Destiny.> 1735071151 +0100 \x0A";
        fContent << "committer root <root@Destiny.> 1735071151 +0100 \x0A";
        fContent << "\x0A";
        fContent << commitMsg << "\x0A";

        // Prepend the header (fContent type and size) to the fContent body
        std::string fContentStr = fContent.str();

        return createCommit(fContentStr, dir);
    }

    std::string createTree(std::filesystem::path& root) {

        size_t cSize {};
        std::map<std::string, std::string> objs {};

        VestTypes::FileType fType {};

        for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(root)) {
            std::string cSha1 {};
            std::string cPath {entry.path().string()};
            std::string fileName {entry.path().filename().string()};

            if (fileName == ".git") continue;

            if (entry.is_directory()) {
                std::filesystem::path p {entry.path()};
                cSha1 = createTree(p);
                fType = VestTypes::TREE_F;
            }

            if (entry.is_regular_file()) {
                cSha1 = createBlob(cPath);
                fType = VestTypes::BLOB_F;
            }

            objs[fileName] = VestFileUtils::constructFileLine(fType, cSha1, fileName);
            cSize += VestTypes::SHA_BYTES_SIZE  // 20 bytes for SHA-1
            + fileName.size()            // size of the filename
            + 1                          // null terminator (\0)
            + (fType.mode == VestTypes::TREE_FILE_STR ? 5 : 6) // mode length
            + 1;                         // space separator

        }

        std::string fContent = "tree " + std::to_string(cSize) + '\x00';
        for (std::map<std::string, std::string>::iterator it {objs.begin()}; it != objs.end(); ++it) {
            fContent += it->second;
        }
        std::cerr << fContent << "\x0A";
        std::string sha1 = writeObject(fContent);

        return sha1;
    }

}