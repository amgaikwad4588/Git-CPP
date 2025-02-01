#ifndef VEST_OBJECTS_HELPERS_H
#define VEST_OBJECTS_HELPERS_H


#include <string>
#include <vector>
#include <cstdint>

namespace VestObjects {

    std::string writeObject(std::string&& fContent, std::string dirRoot = "", std::string sha1 = "");
    std::string writeObject(std::string& fContent, std::string dirRoot = "", std::string sha1 = "");

    std::string prepareCommit(std::string& fContent);
    std::string prepareBlob(std::vector<uint8_t>& fileContent);

}

#endif // VEST_OBJECTS_HELPERS_H