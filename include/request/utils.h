#ifndef VEST_REQUEST_UTILS_H
#define VEST_REQUEST_UTILS_H

#include <cstdint>
#include <string>
#include <vector>


namespace VestRequest {

    size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);

    uint8_t getSha1Head(const char* url, std::string& rSha1);
    uint8_t requestFilesToGit(
        const char* url,
        std::vector<uint8_t>& rData,
        std::vector<std::string>& wSha1
    );
    uint8_t requestFilesToGit(
        const char* url,
        std::vector<uint8_t>& rData,
        std::vector<std::string>& wSha1,
        std::vector<std::string>& hSha1
    );

}

#endif // VEST_REQUEST_UTILS_H