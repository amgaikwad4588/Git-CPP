#include <cstdint>
#include <sstream>
#include <iomanip>

#include <request/utils.h>
#include <utils.h>
#include <request/request.h>

namespace VestRequest {

    size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        std::string* buffer = static_cast<std::string*>(userp);
        size_t tSize = size * nmemb;
        buffer->append(static_cast<char*>(contents), tSize);

        return tSize;
    }

    size_t vectorWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        std::vector<uint8_t>* buffer = static_cast<std::vector<uint8_t>*>(userp);
        size_t tSize = size * nmemb;
        buffer->insert(buffer->end(),
            static_cast<uint8_t*>(contents),
            static_cast<uint8_t*>(contents) + tSize
        );

        return tSize;
    }

    uint8_t getSha1Head(const char* url, std::string& rSha1) {
        RequestManager rManager {};
        rManager.get(url);

        if (rManager.getResponseStatus() == EXIT_FAILURE) return EXIT_FAILURE;
        PRINT_WARNING(rManager.getSReturnData());

        rSha1 = rManager.getSReturnData().substr(38, 40);

        return EXIT_SUCCESS;
    }

    uint8_t requestFilesToGit(
        const char* url,
        std::vector<uint8_t>& rData,
        std::vector<std::string>& wSha1
    ) {

        RequestManager rManager {};

        std::stringstream payload {};
        const char* header = "Content-Type: application/x-git-upload-pack-request";

        for (const std::string& w : wSha1) {
            std::string line = "want " + w + "\x0A";
            payload << std::hex << std::setw(4) << std::setfill('0') << line.size() + 4; // Include 4-character length
            payload << line;
        }

        payload << "0000";
        payload << "0009done" << "\x0A";

        rManager.post(url, header, payload, vectorWriteCallback);
        rData = rManager.getVReturnData();
        return rManager.getResponseStatus();
    }

    uint8_t requestFilesToGit(
        const char* url,
        std::vector<uint8_t>& rData,
        std::vector<std::string>& wSha1,
        std::vector<std::string>& hSha1
    ) {

        RequestManager rManager {};

        std::stringstream payload {};

        for (std::string& w : wSha1) payload << "0032want " << w << "\x0A";
        payload << "0000";

        for (std::string& h : hSha1) payload << "0032have " << h << "\x0A";

        payload << "00009done" << "\x0A";
        const char* header = "Content-Type: application/x-git-upload-pack-request";

        rManager.post(url, header, payload, vectorWriteCallback);
        rData = rManager.getVReturnData();
        return rManager.getResponseStatus();
    }

}