#ifndef VEST_PACK_H
#define VEST_PACK_H

#include <fstream>
#include <cstdint>
#include <vector>
#include <unordered_map>

#include <objects/structs.h>


namespace VestPack {

    struct PackHeader {
        char magic[4]; // Should be "PACK"
        uint32_t version;
        uint32_t numObjects;
    };

    struct ObjectHeader {
        uint8_t type;
        size_t start;
        size_t size;
    };

    ObjectHeader parseObjectHeader(const std::vector<uint8_t>& rData, size_t& offset);

    uint32_t parsePackHeader(const std::vector<uint8_t>& rData, size_t& offset);

    void setFileContent(
        const size_t& startAt,
        std::vector<uint8_t>& rData,
        size_t& offset,
        std::string& fContent
    );

    void copyDelta(
        std::string& baseBlob,
        std::string& objBlob,
        std::string& rData,
        size_t& offset
    );

    void addDelta(
        std::string& blob,
        std::string& rData,
        size_t& offset
    );

    void processRefDelta(
        VestObjects::CommitLinkedList* commitList,
        VestObjects::Tree*& treeClass,
        VestObjects::TreeNode* parent,
        VestObjects::PackIndex& packIndex,
        size_t& offset,
        std::string& dir,
        std::vector<uint8_t>& rData,
        bool& writeOnFile,
        bool& checkDelta
    );

    void processCommit(
        VestObjects::CommitLinkedList* commitList,
        VestObjects::PackIndex& packIndex,
        std::string& fContent,
        std::string& dir
    );

    void processTree(
        VestObjects::CommitLinkedList* commitList,
        VestObjects::Tree*& treeClass,
        VestObjects::TreeNode* parent,
        VestObjects::PackIndex& packIndex,
        std::string& fContent,
        std::string& dir,
        bool& writeOnFile
    );

    void processBlob(
        VestObjects::Tree* treeClass,
        VestObjects::TreeNode* parent,
        VestObjects::PackIndex& packIndex,
        std::string& fContent,
        std::string& dir,
        bool& writeOnFile,
        bool& checkDelta
    );

    void processPack(
        std::vector<uint8_t>& rData,
        size_t offset,
        std::string& dir
    );

}

#endif // VEST_PACK_H