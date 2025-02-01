#include <curl/curl.h>
#include <string>
#include <vector>
#include <request/request.h>
#include <iomanip>
#include <cstring>
#include <cstdint>

#include <utils.h>

namespace VestRequest {

    RequestManager::RequestManager() :
    curl {curl_easy_init()}, rSData {}, rVData {}, rStatus {255}, httpCode {}, url {nullptr} {}

    RequestManager::~RequestManager() {
        if (curl != nullptr) {
            curl_easy_cleanup(curl);
            curl = nullptr;
        }

        freeUrl();
    }

    std::string& RequestManager::getSReturnData() {return rSData;}
    std::vector<uint8_t>& RequestManager::getVReturnData() {return rVData;}
    uint8_t RequestManager::getResponseStatus() {return rStatus;}

    void RequestManager::freeUrl() {
        if (url == nullptr) return;

        // delete [] url;
        url = nullptr;
    }

    void RequestManager::initCurl(
        const char* url,
        size_t(*func)(void*, size_t, size_t, void*)
    ) {
        curl_easy_reset(curl);
        freeUrl();

        rSData = "";
        rVData = {};
        this->url = new char[std::strlen(url) + 1];
        std::strcpy(this->url, url);

        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, this->url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, func);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        if (func == writeCallback) {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rSData);
        } else {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rVData);
        }

    }

    void RequestManager::makeCurlRequest() {
        CURLcode res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

        rStatus = EXIT_SUCCESS;

        if (res != CURLE_OK || (httpCode >= 400 && 600 > httpCode)) {
            rStatus = EXIT_FAILURE;
        }

        if (rStatus == EXIT_FAILURE) {
            PRINT_ERROR("CALL FAILED TO URL: " + std::string(url));
            PRINT_ERROR("WITH HTTP RESPONSE CODE: " + std::to_string(httpCode));
            PRINT_ERROR("MSG: " + std::string(curl_easy_strerror(res)));
        }
    }

    void RequestManager::post(
        const char* url,
        const char* header,
        std::stringstream& payload,
        size_t(*func)(void*, size_t, size_t, void*)
    ) {
        initCurl(url, func);

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, header);

        std::string postData = payload.str();

        curl_easy_setopt(curl, CURLOPT_URL, this->url);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.size());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        makeCurlRequest();

        curl_slist_free_all(headers);
    }

    void RequestManager::post(
        const char* url,
        const char* header,
        std::stringstream& payload
    ) {
        post(url, header, payload, writeCallback);
    }

    void RequestManager::get(const char* url) {
        initCurl(url, writeCallback);
        makeCurlRequest();
    }
}