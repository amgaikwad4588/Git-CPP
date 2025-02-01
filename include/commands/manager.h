#ifndef VEST_COMMAND_MANAGER_H
#define VEST_COMMAND_MANAGER_H

#include <string>
#include <unordered_map>
#include <functional>
#include <cstdint>


namespace Vest {

    constexpr const char* INIT = "init";
    constexpr const char* CAT_FILE = "cat-file";
    constexpr const char* HASH_OBJECT = "hash-object";
    constexpr const char* LS_TREE = "ls-tree";
    constexpr const char* WRITE_TREE = "write-tree";
    constexpr const char* COMMIT_TREE = "commit-tree";
    constexpr const char* CLONE = "clone";

    class CommandManager {

        public:
            CommandManager();
            ~CommandManager();

            uint8_t processCommand(int& argc, char* argv[]);

        private:

            uint8_t actionForInit       (int argc, char* argv[]);
            uint8_t actionForCatFile    (int argc, char* argv[]);
            uint8_t actionForHashObject (int argc, char* argv[]);
            uint8_t actionForLsTree     (int argc, char* argv[]);
            uint8_t actionForWriteTree  (int argc, char* argv[]);
            uint8_t actionForCommitTree (int argc, char* argv[]);
            uint8_t actionForClone      (int argc, char* argv[]);

            std::unordered_map<
                std::string,
                std::function<uint8_t(int argc, char* argv[])>
            > actions;

    };
}


#endif // VEST_COMMAND_MANAGER_H