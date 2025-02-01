#include <iostream>

#include <file/types.h>
#include <file/utils.h>

namespace VestTypes {

    bool DecompressedData::isEmpty() {
        return data.empty();
    }

    void TreeFile::addLine(uint8_t fType, std::string fName, std::string bSha1) {
        tLines.push_back(new TreeFileLine(fType, fName, bSha1));
    }

    void TreeFile::addLine(std::string fType, std::string fName, std::string bSha1) {
        tLines.push_back(new TreeFileLine(fType, fName, bSha1));
    }

    TreeFileLine::TreeFileLine() : bSha1 {}, fType {}, fName{}, sSha1 {} {}

    TreeFileLine::TreeFileLine(std::string fType, std::string fName, std::string bSha1)
        : TreeFileLine((fType == VestTypes::BLOB_FILE_STR) ? VestTypes::BLOB
        : (fType == VestTypes::TREE_FILE_STR) ? VestTypes::TREE
        : 0, // Default or error value
        fName, bSha1
    ) {}

    TreeFileLine::TreeFileLine(uint8_t fType, std::string fName, std::string bSha1) :
        bSha1 {bSha1}, fType {fType}, fName{fName}, sSha1 {} {}

    std::string TreeFileLine::sha1() {
        if (!sSha1.empty()) return sSha1;

        std::ostringstream oss;
        for (uint8_t i {}; i < bSha1.size(); i++) {
            oss << VestFileUtils::byteToHex(bSha1[i]);
        }

        sSha1 = oss.str();
        return sSha1;
    }

    void CommitFile::printCommitFile() {
        std::cout << tSha1     << '\x0A';
        if (!pSha1.empty()) std::cout << pSha1     << '\x0A';
        std::cout << author    << '\x0A';
        std::cout << commiter  << '\x0A';
        std::cout << '\x0A';
        std::cout << commitMsg << '\x0A';
        std::cout << '\x0A';
    }
}