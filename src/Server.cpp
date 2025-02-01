#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include <commands/manager.h>

int main(int argc, char *argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    Vest::CommandManager commandManager {};
    return commandManager.processCommand(argc, argv);
}
