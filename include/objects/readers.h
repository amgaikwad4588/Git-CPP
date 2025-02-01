#include <string>
#include <objects/structs.h>

namespace VestObjects {

    ObjectRead readObject(std::string& sha1);
    ObjectRead readObject(std::string& sha1, std::string& dir);

}