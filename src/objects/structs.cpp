#include <cstdint>
#include <algorithm>

#include <file/types.h>
#include <file/file.h>
#include <file/utils.h>

#include <objects/structs.h>

#include <utils.h>

namespace VestObjects {

    CommitNode::CommitNode(VestTypes::CommitFile* commit) :
        commit {commit}, prev {nullptr}, next {nullptr} {}

    CommitNode::~CommitNode() {
        if (prev != nullptr) prev->next = next;
        if (next != nullptr) next->prev = prev;

        prev = nullptr;
        next = nullptr;
        commit = nullptr;
    }

    void CommitNode::addNext(CommitNode* node) {
        next = node;
        node->addPrev(this);
    }

    void CommitNode::addNext(VestTypes::CommitFile* commit) {
        addNext(new CommitNode(commit));
    }

    void CommitNode::addPrev(CommitNode* node) {
        prev = node;
        node->addNext(this);
    }

    void CommitNode::addPrev(VestTypes::CommitFile* commit) {
        addPrev(new CommitNode(commit));
    }

    /**
     * ===================================================================
     *                             LinkedList
     * ===================================================================
    */

    CommitLinkedList::CommitLinkedList() :
        head {nullptr}, current {nullptr}, tail {nullptr} {}

    CommitLinkedList::~CommitLinkedList() {}

    CommitNode* CommitLinkedList::getCurrent() { return current; }

    bool CommitLinkedList::isHead() { return current == head; }

    void CommitLinkedList::incrementIndex() {

        if (current->next == nullptr) {
            PRINT_WARNING("LINKED LIST COMPLETED");
            return;
        }

        current = current->next;

        if (current->next == nullptr) {
            PRINT_WARNING("LINKED LIST COMPLETED");
        }
    }

    void CommitLinkedList::addNode(CommitNode* node) {

        if (head == nullptr) setHead(node);

        setTail(node);
        if (current == nullptr) current = node;
    }

    void CommitLinkedList::addNode(VestTypes::CommitFile* commit) {
        addNode(new CommitNode(commit));
    }

    void CommitLinkedList::setHead(CommitNode* node) {

        if (head == nullptr) {head = node; return;}

        node->next = head;
        head->prev = node;

        head = node;
    }

    void CommitLinkedList::setTail(CommitNode* node) {
        if (tail == nullptr) {tail = node; return;}

        tail->next = node;
        node->prev = tail;

        tail = node;
    }

    void CommitLinkedList::printCommits() {

        CommitNode* currentNode = head;

        while (currentNode != nullptr) {
            currentNode->commit->printCommitFile();
            PRINT_SML_SEPARATION;
            currentNode = currentNode->next;
        }

    }

    /**
     * ===================================================================
     *                           Tree Node
     * ===================================================================
    */

    TreeNode::TreeNode(VestTypes::TreeFile* treeFile, std::string folderName)
    : TreeNode(treeFile, folderName, nullptr) {}

    TreeNode::TreeNode(VestTypes::TreeFile* treeFile, std::string folderName, TreeNode* parent)
    : treeFile {treeFile}, parent {parent}, index {}, completed {}, folderName {folderName} {}

    void TreeNode::addChild(TreeNode* node) {
        node->parent = this;
        children.push_back(node);
    }

    void TreeNode::addChild(VestTypes::TreeFile* treeFile, std::string folderName) {
        addChild(new TreeNode(treeFile, folderName));
    }

    void TreeNode::incrementIndex() {
        if (treeFile->tLines.size() > index) {
            index++;
        }

        if (index == treeFile->tLines.size()) {
            if (parent != nullptr) {
                PRINT_WARNING("CURRENT TREE NODE COMPLETED: " + parent->getPreviousLine()->sha1());
            } else {
                PRINT_WARNING("ROOT COMPLETED");
            }
            completed = true;
        }
    };

    bool TreeNode::isCompleted() {return completed;}

    std::string TreeNode::getFolderName() {return folderName;}

    std::string TreeNode::getPath() {

        TreeNode* currentNode {this};
        std::string path {};

        while (currentNode != nullptr) {
            path = currentNode->getFolderName() + "/" + path;
            currentNode = currentNode->parent;
        }

        path.erase(std::remove(path.begin(), path.end(), '\0'), path.end());
        return path;
    }

    VestTypes::TreeFileLine* TreeNode::getCurrentLine() {
        if (index >= treeFile->tLines.size()) {
            PRINT_ERROR(
                "INDEX OUT OF RANGE | INDEX: " + std::to_string(index)
                + " |  PARENT: " + parent->getPreviousLine()->sha1()
            );
            throw std::runtime_error("");
            return nullptr;
        }
        // PRINT_HIGHLIGHT("INDEX: " + std::to_string(index) + " : " + treeFile->tLines[index]->sha1());
        return treeFile->tLines[index];
    }

    VestTypes::TreeFileLine* TreeNode::getPreviousLine() {
        return treeFile->tLines[index - 1];
    }

    /**
     * ===================================================================
     *                           Tree
     * ===================================================================
    */

    Tree::Tree() : root {nullptr} {};
    Tree::~Tree() {};

    TreeNode* Tree::getIndex() {return index;}
    TreeNode* Tree::getRoot() {return root;}

    void Tree::setIndex(TreeNode* node) {index = node;}

    void Tree::setRoot(TreeNode* node) {
        if (root == nullptr) setIndex(node);
        root = node;
    }

    void Tree::setRoot(VestTypes::TreeFile* treeFile, std::string folderName) {
        if (folderName[folderName.size() - 1] == '/') {
            folderName = folderName.substr(0, folderName.size() - 1);
        }
        setRoot(new TreeNode(treeFile, folderName));
    }


    /**
     * ===================================================================
     *                           PACK INDEX
     * ===================================================================
    */

    PackIndex::PackIndex() : written {}, packIdx {} {}
    PackIndex::~PackIndex() {}

    bool PackIndex::exists(std::string sha1) { return packIdx.count(sha1) == 1; }

    std::string PackIndex::getFile(std::string sha1) {

        if (!exists(sha1)) return "";

        std::vector<uint8_t> fAsVector = VestFile::readFile(
            VestFileUtils::constructfPath(sha1)
        );

        return std::string(fAsVector.begin(), fAsVector.end());
    }

    // Change this to get the offset of the sha1 in the packfile.
    void PackIndex::addSha1(std::string sha1) { packIdx[sha1] = 0; }

    void PackIndex::write() {
        if (written) return;

        std::vector<uint8_t> fContent {};

        for (std::map<std::string, uint32_t>::iterator it = packIdx.begin(); it != packIdx.end(); it++) {

            std::string row = std::to_string(it->second) + " " + it->first;
            fContent.insert(fContent.end(), row.begin(), row.end());
        }

        std::string sha1 = VestFileUtils::computeSHA1(fContent);
        std::string fPath = VestFileUtils::constructfPath(sha1);

        std::vector<uint8_t> compressedData = VestFile::compressData(fContent);
        VestFile::saveToFile(fPath, compressedData);
    }

    /**
     * ===================================================================
     *                           STRUCTS
     * ===================================================================
    */

    ObjectRead::ObjectRead() {}

    ObjectRead::ObjectRead(std::string fContent, uint8_t type) : fContent{fContent}, type{} {
        setType(type);
    }

    bool ObjectRead::setType(uint8_t t) {
        if (!validateType(t)) {
            PRINT_ERROR("INVALID TYPE; TYPE MUST BE BETWEEN 1 AND 8, SEE FILE/TYPES.H");
            return false;
        }

        type = t;
        return true;
    }

    bool ObjectRead::validateType(uint8_t& t) {
        return (t >= 1 && 8 >= t);
    }

    uint8_t ObjectRead::getType() { return type; }

    std::string ObjectRead::getStrType() { return type == VestTypes::TREE ? "tree" : "blob"; }

}