#include <string>
#include <vector>

#include <utils.h>

#include <file/file.h>
#include <file/utils.h>

#include <objects/readers.h>

namespace VestObjects {

    ObjectRead readObject(std::string& sha1) {
        std::string root = "";
        return readObject(sha1, root);
    }

    ObjectRead readObject(std::string& sha1, std::string& dir) {

        ObjectRead objRead {};

        std::string root = dir + ".git/objects/";
        std::string fPath = VestFileUtils::constructfPath(sha1, root);

        std::vector<uint8_t> vContent = VestFile::readFile(fPath);
        VestTypes::DecompressedData dData = VestFile::decompressData(vContent);

        objRead.fContent = std::string(dData.data.begin(), dData.data.end());

        std::string type = objRead.fContent.substr(0, objRead.fContent.find(' '));
        objRead.fContent = objRead.fContent.substr(objRead.fContent.find('\x00') + 1);

        // Determine and return the file type
        if (type == "tree") objRead.setType(VestTypes::TREE);
        else if (type == "blob") objRead.setType(VestTypes::BLOB);
        else {
            PRINT_ERROR("WRONG FILE TYPE");
            throw std::runtime_error("");
        }

        return objRead;
    }

}