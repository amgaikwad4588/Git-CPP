#ifndef VEST_REQUEST_H
#define VEST_REQUEST_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <curl/curl.h>
#include <functional>

namespace VestRequest {

    size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
    size_t vectorWriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    class RequestManager {
        public:

            RequestManager();
            ~RequestManager();

            uint8_t getResponseStatus();
            std::string& getSReturnData();
            std::vector<uint8_t>& getVReturnData();

            void post(
                const char* url,
                const char* header,
                std::stringstream& payload
            );
            void post(
                const char* url,
                const char* header,
                std::stringstream& payload,
                size_t(*func)(void*, size_t, size_t, void*)
            );
            void get(const char* url);

        private:
            CURL* curl;

            std::string rSData;
            std::vector<uint8_t> rVData;

            uint8_t rStatus;
            uint32_t httpCode;

            char* url;

            void initCurl(
                const char* url,
                size_t(*func)(void*, size_t, size_t, void*)
            );
            void makeCurlRequest();

            void freeUrl();
    };

}


#endif // VEST_REQUEST_H