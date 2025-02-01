#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <zlib.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdexcept>

#include <file/pack.h>
#include <file/file.h>
#include <file/types.h>
#include <file/utils.h>

#include <objects/initializers.h>
#include <objects/helpers.h>
#include <objects/structs.h>
#include <objects/readers.h>

#include <utils.h>

namespace VestPack {

    ObjectHeader parseObjectHeader(const std::vector<uint8_t>& rData, size_t& offset) {
        if (offset >= rData.size()) {
            throw std::runtime_error("Invalid PACK file: Offset exceeds data size");
        }

        uint32_t size = 0;
        uint8_t type = 0;
        int shift = 0;

        // Read the first byte
        uint8_t currentByte = rData[offset++];
        type = (currentByte >> 4) & 0x07; // (1) Extract type (bits 6-4)
        size = currentByte & 0x0F;        // (2) Extract size (bits 3-0)

        // Process continuation bytes if MSB is set
        while (currentByte & 0x80) { // MSB (bit 7) is 1 => more bytes follow

            if (offset >= rData.size()) {
                throw std::runtime_error("Invalid PACK file: Header is incomplete");
            }
            currentByte = rData[offset++];
            // printHexAndBinary(currentByte);
            size |= (currentByte & 0x7F) << shift; // (3) Append 7 bits to size
            shift += 7;
        }

        ObjectHeader obj {type, offset, size};
        return obj;
    }

    uint32_t parsePackHeader(const std::vector<uint8_t>& rData, size_t& offset) {

        // Read version and object count
        uint32_t version =  (static_cast<uint8_t>(rData[offset + 0]) << 24) |
                            (static_cast<uint8_t>(rData[offset + 1]) << 16) |
                            (static_cast<uint8_t>(rData[offset + 2]) << 8)  |
                             static_cast<uint8_t>(rData[offset + 3]);

        uint32_t nObjects = (static_cast<uint8_t>(rData[offset + 4]) << 24) |
                            (static_cast<uint8_t>(rData[offset + 5]) << 16) |
                            (static_cast<uint8_t>(rData[offset + 6]) << 8)  |
                             static_cast<uint8_t>(rData[offset + 7]);

        // std::cout << "PACK version: " << version << ", Object count: " << nObjects << std::endl;

        offset += 8;
        return nObjects;
    }

    void setFileContent(
        const size_t& startAt,
        std::vector<uint8_t>& rData,
        size_t& offset,
        std::string& fContent
    ) {
        std::vector<uint8_t> nextObject(rData.begin() + startAt, rData.end());
        VestTypes::DecompressedData dData = VestFile::decompressData(
            nextObject, VestTypes::EXPAND_AS_NEEDED
        );

        offset += dData.compressedUsed;
        fContent = std::string(dData.data.begin(), dData.data.end());
    }

    void copyDelta(
        std::string& baseBlob,
        std::string& objBlob,
        std::string& fContent,
        size_t& offset
    ) {

        // verify that in fact the first bit is a 1
        uint8_t cByte = fContent[offset];
        if ((cByte & 0x80) == 0) return; // MSB is 0, not a copy command.
        // PRINT_WARNING("IN COPY_DELTA");

        uint32_t startCopyAt = 0;
        uint32_t bytesToCopy = 0;

        // We'll consume bytes from fContent after cByte:
        offset++;

        // Calculating the offset - bits : 0 - 3
        {
            // If bit0 is set, read one byte for offset’s low 8 bits
            if (cByte & 0x01) {
                startCopyAt |= (unsigned char)fContent.at(offset++) << 0;
            }
            // If bit1 is set, read one byte for offset’s next 8 bits
            if (cByte & 0x02) {
                startCopyAt |= (unsigned char)fContent.at(offset++) << 8;
            }
            if (cByte & 0x04) {
                startCopyAt |= (unsigned char)fContent.at(offset++) << 16;
            }
            if (cByte & 0x08) {
                startCopyAt |= (unsigned char)fContent.at(offset++) << 24;
            }
        }

        // Calculating the size - bits : 4 - 6
        {
            if (cByte & 0x10) {
                bytesToCopy |= (unsigned char)fContent.at(offset++) << 0;
            }
            if (cByte & 0x20) {
                bytesToCopy |= (unsigned char)fContent.at(offset++) << 8;
            }
            if (cByte & 0x40) {
                bytesToCopy |= (unsigned char)fContent.at(offset++) << 16;
            }
            if (bytesToCopy == 0) {
                bytesToCopy = 0x10000;
            }
        }

        // At this point we have everything to start the copy into objBlob
        std::string toCopy {};
        for (uint32_t i = 0; i < bytesToCopy; i++) toCopy += baseBlob[startCopyAt + i];
        // std::cout << toCopy << "\x0A";

        // PRINT_DELTA("RESULT OF COPY: ");
        objBlob += toCopy;
        // std::cout << objBlob << "\x0A";
    }

    void addDelta(
        std::string& blob,
        std::string& fContent,
        size_t& offset
    ) {

        if ((fContent[offset] & 0x80) != 0) return; // MSB is 1, not an add command.

        std::string toAdd = fContent.substr(offset + 1, static_cast<int>(fContent[offset]));
        blob += toAdd; offset += toAdd.size() + 1;

        // PRINT_SML_SEPARATION;
        // PRINT_DELTA("TO ADD: ");
        // std::cout << toAdd << "\x0A";

        // PRINT_DELTA("RESULT: ");
        // std::cout << blob << "\x0A";
        // PRINT_SML_SEPARATION;

    }

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
    ) {

        size_t originalOffset = offset;

        // Let's get the SHA1 for the ref object
        std::string refDeltaSha1 {};
        for (uint8_t _ {}; VestTypes::SHA_BYTES_SIZE > _; _++) {
            refDeltaSha1 += VestFileUtils::byteToHex(rData.at(offset));
            offset++;
        }

        // PRINT_DELTA("SHA1_PARENT: " + refDeltaSha1);
        VestObjects::ObjectRead objRead = VestObjects::readObject(refDeltaSha1, dir);

        std::string fContent {};
        (void)setFileContent(offset, rData, offset, fContent);

        auto parseRefDeltaSizes = [](std::string& rData, size_t& offset) -> uint64_t {

            uint64_t value {};
            uint16_t shift {};

            while (rData.size() > offset) {
                uint8_t cByte = rData[offset++];

                value |= static_cast<uint64_t>(cByte & 0x7F) << shift;

                if ((cByte & 0x80) == 0) break; // If true MSB == 0, so we can terminate
                shift += 7;
            }

            return value;
        };

        size_t internalOffset {};

        // Let's grab the base size from the decompressedFcontent
        uint64_t bSize = parseRefDeltaSizes(fContent, internalOffset);
        uint64_t rSize = parseRefDeltaSizes(fContent, internalOffset);

        std::string newFile {};
        bool inRef {true};

        // Now, we have to start copying from the blob or add the new values
        while (inRef && rSize > newFile.size()) {

            uint8_t cByte = fContent[internalOffset];

            switch (cByte & 0x80) {
                case 0x80:
                    (void)copyDelta(objRead.fContent, newFile, fContent, internalOffset);
                    break;

                case 0x00:
                    (void)addDelta(newFile, fContent, internalOffset);
                    break;

                default:
                    inRef = false;
                    break;
            }

        }

        // At this point our file object should be completed, so we need to compute the SHA1
        // Of it, write the REF_DELTA under the same SHA1 with the newBlob content, not with the
        // content of the REF_DELTA

        switch (objRead.getType()) {
            case VestTypes::TREE:
                processTree(commitList, treeClass, parent, packIndex, newFile, dir, writeOnFile);
                return;
        default:
            break;
        }
        // Compute hash
        std::string dataToComputeSha1 = objRead.getStrType() + " " + std::to_string(newFile.size()) + '\x00';
        dataToComputeSha1 += newFile;
        std::string sha1 = VestFileUtils::computeSHA1(dataToComputeSha1);
        packIndex.addSha1(sha1);

        (void)VestObjects::writeObject(
            dataToComputeSha1, dir, sha1
        );

        // PRINT_DELTA("SHA1 WRITTEN: " + sha1);

        // Check if the sha1 is the same as the one we have on queue
        // The tree must be reset.
        VestTypes::TreeFileLine* treeLine = parent->getCurrentLine();

        if (sha1 != treeLine->sha1()) return; // This indicates that there must be another delta to parse
        // If we reach here, we have found the correct sha1, so we can increment the line
        parent->incrementIndex();
        if (parent->isCompleted()) treeClass->setIndex(parent->parent);

        checkDelta = false;

        if (!writeOnFile) return;

        std::string path = parent->getPath() + treeLine->fName;
        VestFile::saveToFile(path, {fContent.begin(), fContent.end()});
    }

    void processCommit(
        VestObjects::CommitLinkedList* commitList,
        VestObjects::PackIndex& packIndex,
        std::string& fContent,
        std::string& dir
    ) {
        VestTypes::CommitFile* commitFile = VestFile::readCommit(fContent);
        commitList->addNode(commitFile);

        std::string sha1 = VestObjects::createCommit(fContent, dir);
        packIndex.addSha1(sha1);

        // PRINT_COMMIT("COMMIT SHA1 WRITTEN: " + sha1);
    }

    void processTree(
        VestObjects::CommitLinkedList* commitList,
        VestObjects::Tree*& treeClass,
        VestObjects::TreeNode* parent,
        VestObjects::PackIndex& packIndex,
        std::string& fContent,
        std::string& dir,
        bool& writeOnFile
    ) {

        VestTypes::TreeFile* treeFile = VestFile::readTreeFile(fContent);

        // We write the tree on the files
        std::string fileToWrite = "tree " + std::to_string(fContent.size()) + '\x00';
        fileToWrite += fContent;
        std::string sha1 = VestObjects::writeObject(fileToWrite, dir);
        packIndex.addSha1(sha1);

        // PRINT_SML_SEPARATION;
        // for (VestTypes::TreeFileLine* t : treeFile->tLines) {
        //     PRINT_TREE("TYPE: " + std::to_string(t->fType) + " NAME: " + t->fName + " SHA1: " + t->sha1());
        // }
        // PRINT_TREE("TREE SHA1: " + sha1 + " | SIZE: " + std::to_string(treeFile->tLines.size()));
        // PRINT_SML_SEPARATION;

        if (commitList->getCurrent()->commit->tSha1 == sha1) {
            // We gotta reset the tree
            // PRINT_HIGHLIGHT("RESET TREE");
            delete treeClass;
            if (!commitList->isHead()) {
                PRINT_WARNING("CHANGING WRITING");
                writeOnFile = false;
            }

            treeClass = new VestObjects::Tree();
            commitList->incrementIndex();
        }

        if (treeClass->getRoot() == nullptr) {
            treeClass->setRoot(treeFile, dir);
            return;
        }

        bool reRun {};
        VestTypes::TreeFileLine* treeLine = parent->getCurrentLine();
        if (sha1 != treeLine->sha1()) {


            if (!packIndex.exists(treeLine->sha1())) {
                // PRINT_ERROR("TREE: NOT CURRENT SHA1: " + treeLine->sha1());
                throw std::runtime_error("");
            }

            // Modify this to add a better sha1 handleing
            // PRINT_SUCCESS("TREE: SHA1 FOUND IN PACK_INDEX: " + treeLine->sha1());
            reRun = true;
        }

        parent->incrementIndex();

        if (reRun) {

            if (parent->isCompleted()) {
                if (parent->parent->isCompleted()) {
                    treeClass->setIndex(parent->parent->parent);
                } else {
                    treeClass->setIndex(parent->parent);
                }
            };
            processTree(commitList, treeClass, treeClass->getIndex(), packIndex, fContent, dir, writeOnFile);
            return;
        }


        // We have to check the parent at this point, and mark that line as read
        VestObjects::TreeNode* currentNode = new VestObjects::TreeNode(treeFile, treeLine->fName, parent);
        parent->addChild(currentNode);

        // And we set the parent to be the current node;
        // Set here the Tree->index
        treeClass->setIndex(currentNode);

        // At this point we should create the folder if we are on the head:
        // Otherwise we should only return
        if (!writeOnFile) return;

        std::filesystem::path absPath = std::filesystem::absolute(currentNode->getPath());
        std::string path = absPath.string();

        bool isEmpty = true;

        if (std::filesystem::exists(path)) {
            isEmpty = std::filesystem::remove(path);
        }
        if (isEmpty) std::filesystem::create_directory(absPath);

        std::vector<std::string> entries {};

        // PRINT_TREE("TREE WRITTEN: " + sha1);
        // PRINT_SML_SEPARATION;
        // if (parent->isCompleted()) treeClass->setIndex(parent->parent);

    }

    void processBlob(
        VestObjects::Tree* treeClass,
        VestObjects::TreeNode* parent,
        VestObjects::PackIndex& packIndex,
        std::string& fContent,
        std::string& dir,
        bool& writeOnFile,
        bool& checkDelta
    ) {

        std::string fileToWrite = "blob " + std::to_string(fContent.size()) + '\x00';
        fileToWrite += fContent;

        std::vector<uint8_t> vContent {};
        std::string sha1 = VestObjects::writeObject(fileToWrite, dir);

        VestTypes::TreeFileLine* treeLine = parent->getCurrentLine();
        bool reRun {};
        if (sha1 != treeLine->sha1()) {

            if (!packIndex.exists(treeLine->sha1())) {
                // PRINT_ERROR("BLOB: NOT CURRENT SHA1: " + treeLine->sha1() + " | AND BLOB: " + sha1);
                checkDelta = true;
                return;
            }

            // Modify this to add a better sha1 handleing
            // PRINT_SUCCESS("BLOB: SHA1 FOUND IN PACK_INDEX: " + treeLine->sha1());
            std::string path = parent->getPath() + treeLine->fName;

            std::string fContent = treeLine->sha1();
            if (writeOnFile) VestFile::saveToFile(path, {fContent.begin(), fContent.end()});
            reRun = true;
        }

        parent->incrementIndex();
        if (reRun) {
            processBlob(treeClass, parent, packIndex, fContent, dir, writeOnFile, checkDelta);
            return;
        }

        packIndex.addSha1(sha1);

        // PRINT_SML_SEPARATION;
        if (parent->isCompleted()) {
            // PRINT_BLOB("BLOB_COMPLETED: " + parent->parent->getPreviousLine()->sha1());
            if (parent->parent->isCompleted()) {
                // PRINT_SUCCESS("YUP");
                treeClass->setIndex(parent->parent->parent);
            } else {
                treeClass->setIndex(parent->parent);
            }
        }

        if (!writeOnFile) return;
        PRINT_BLOB("BLOB WRITTEN: " + sha1);
        std::string path = parent->getPath() + treeLine->fName;
        VestFile::saveToFile(path, {fContent.begin(), fContent.end()});
    }

    void processPack(
        std::vector<uint8_t>& rData,
        size_t offset,
        std::string& dir
    ) {

        size_t _offset = offset;
        uint32_t nObjects = parsePackHeader(rData, _offset);

        VestObjects::CommitLinkedList* commitList = new VestObjects::CommitLinkedList();
        VestObjects::Tree* tree = new VestObjects::Tree();
        VestObjects::PackIndex packIndex {};
        std::string lastBlob {};

        bool isHead {true};
        bool mustBeDelta {};

        for (uint32_t i {}; i < nObjects; i++) {

            ObjectHeader objHeader = parseObjectHeader(rData, _offset);
            VestObjects::TreeNode* treeIdx = tree->getIndex();

            if (objHeader.type == VestTypes::REF_DELTA) {
                PRINT_DELTA("PROCESSING REF_DELTA");
                (void)processRefDelta(commitList, tree, treeIdx, packIndex, _offset, dir, rData, isHead, mustBeDelta);
                continue;
            }

            PRINT_HIGHLIGHT("CURRENT INDEX: " + std::to_string(i) + "/" + std::to_string(nObjects));

            if (mustBeDelta) {
                PRINT_ERROR("NEXT FILE MUST BE DELTA AND IS NOT!");
                throw std::runtime_error("");
            }

            std::string fContent {};
            (void)setFileContent(_offset, rData, _offset, fContent);

            switch (objHeader.type) {
                case VestTypes::COMMIT:
                    // PRINT_COMMIT("PROCESSING COMMIT");
                    (void)processCommit(commitList, packIndex, fContent, dir);
                    break;

                case VestTypes::TREE:
                    // PRINT_TREE("PROCESSING TREE");
                    (void)processTree(commitList, tree, treeIdx, packIndex, fContent, dir, isHead);
                    break;

                case VestTypes::BLOB:
                    // PRINT_BLOB("PROCESSING BLOB");
                    (void)processBlob(tree, treeIdx, packIndex, fContent, dir, isHead, mustBeDelta);
                    break;

                case VestTypes::OFS_DELTA:
                    PRINT_WARNING("OFS_DELTA - DOING NOTHING");
                    break;

                default:
                    PRINT_ERROR("NOT VALID TYPE FOUND");
                    break;
            }
        }

    }
}