#ifndef VEST_OBJECTS_STRUCTS_H
#define VEST_OBJECTS_STRUCTS_H

#include <map>
#include <file/types.h>

namespace VestObjects {

    class CommitNode {

        public:

        VestTypes::CommitFile* commit;
        CommitNode* prev;
        CommitNode* next;

        CommitNode(VestTypes::CommitFile* commit);
        ~CommitNode();

        void addNext(CommitNode* node);
        void addNext(VestTypes::CommitFile* commit);

        void addPrev(CommitNode* node);
        void addPrev(VestTypes::CommitFile* commit);
    };

    class CommitLinkedList {

        public:

        CommitLinkedList();
        ~CommitLinkedList();

        CommitNode* getCurrent();

        bool isHead();

        void addNode(CommitNode* node);
        void addNode(VestTypes::CommitFile* commit);
        void incrementIndex();

        void printCommits();

        private:

        void setHead(CommitNode* node);
        void setTail(CommitNode* node);

        CommitNode* head;
        CommitNode* current;
        CommitNode* tail;

    };

    class TreeNode {
        public:

        TreeNode(VestTypes::TreeFile* treeFile, std::string folderName);
        TreeNode(VestTypes::TreeFile* treeFile, std::string folderName, TreeNode* parent);

        void addChild(TreeNode* node);
        void addChild(VestTypes::TreeFile* treeFile, std::string folderName);

        void incrementIndex();

        bool isCompleted();

        std::string getFolderName();
        std::string getPath();

        VestTypes::TreeFileLine* getCurrentLine();
        VestTypes::TreeFileLine* getPreviousLine();

        VestTypes::TreeFile* treeFile;
        TreeNode* parent;
        std::vector<TreeNode*> children;

        private:

        std::string folderName;

        bool completed;
        uint8_t index;
    };

    class Tree {
        public:

        Tree();
        ~Tree();

        TreeNode* getRoot();
        TreeNode* getIndex();

        void setRoot(TreeNode* node);
        void setIndex(TreeNode* node);
        void setRoot(VestTypes::TreeFile* treeFile, std::string folderName);

        private:

        TreeNode* index;
        TreeNode* root;
    };

    class PackIndex {
        public:

        PackIndex();
        ~PackIndex();

        bool exists(std::string sha1);

        std::string getFile(std::string sha1);

        void addSha1(std::string sha1);
        void write();

        private:

        bool written;
        std::map<std::string, uint32_t> packIdx;


    };


    struct ObjectRead {

        public:
            ObjectRead();
            ObjectRead(std::string fContent, uint8_t type);

            bool setType(uint8_t t);

            uint8_t getType();
            std::string getStrType();
            std::string fContent;

        private:
            bool validateType(uint8_t& t);
            uint8_t type;

    };

}

#endif