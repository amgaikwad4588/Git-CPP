#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <zlib.h>
#include <cstring>
using namespace std;
string read_hash_object(const string &filepath) {
    gzFile filez = gzopen(filepath.c_str(), "rb");
    if (filez == NULL) throw(runtime_error("The blob does not exist..."));
    unsigned char buffer[1024];
    string compressed_content;
    while (true)
    {
        int content_read = gzread(filez, buffer, 1024);
        if (content_read > 0)
        {
            compressed_content.insert(compressed_content.end(), buffer, buffer + content_read);
        }
        else{
            gzclose(filez);
            break;
        }
    }
    return compressed_content;
}
string decompress(const string &compressed_content) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    if (inflateInit(&zs) != Z_OK) {
        throw(runtime_error("Could not initialize z_stream"));
    }
    zs.next_in = (Bytef *)compressed_content.data();
    zs.avail_in = compressed_content.size();
    int result;
    char buffer[10240];
    string uncompressed_content;
    do {
        zs.next_out = reinterpret_cast<Bytef *>(buffer);
        zs.avail_out = sizeof(buffer);
        result = inflate(&zs, 0);
        if (zs.total_out != 0) {
            uncompressed_content.append(buffer, zs.total_out);
        }
    } while (zs.avail_in != 0);
    inflateEnd(&zs);
    if (result != Z_STREAM_END) {
        ostringstream err;
        err << "Excpetion during decompression ( " << result << " )" << zs.msg ;
        throw(runtime_error(err.str()));
    }
    int start = 0;
    for (int i = 0; i < uncompressed_content.size(); i++) {
        if (uncompressed_content[i] == '\0') {
            start = i + 1;
            break;
        }
    }
    return uncompressed_content.substr(start);
}
int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cerr << "Logs from your program will appear here!\n";
    // Uncomment this block to pass the first stage
    if (argc < 2)
    {
        std::cerr << "No command provided.\n";
        return EXIT_FAILURE;
    }
    std::string command = argv[1];
    if (command == "init")
    {
        try
        {
            std::filesystem::create_directory(".git");
            std::filesystem::create_directory(".git/objects");
            std::filesystem::create_directory(".git/refs");
            std::ofstream headFile(".git/HEAD");
            if (headFile.is_open())
            {
                headFile << "ref: refs/heads/main\n";
                headFile.close();
            }
            else
            {
                std::cerr << "Failed to create .git/HEAD file.\n";
                return EXIT_FAILURE;
            }
            std::cout << "Initialized git directory\n";
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cerr << e.what() << '\n';
            return EXIT_FAILURE;
        }
    }
    else if (command == "cat-file")
    {
        std::string option = argv[2];
        if (option == "-p")
        {
            const string blob_hash = argv[3];
            const string filepath = ".git/objects/" + blob_hash.substr(0, 2) + "/" + blob_hash.substr(2);
            const string compressed_content = read_hash_object(filepath);
            // cout << "Compressed content - " << compressed_content << endl;
            const string uncompressed_content = decompress(compressed_content);
            cout << uncompressed_content;
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
    else
    {
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}