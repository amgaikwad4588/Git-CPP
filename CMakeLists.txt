cmake_minimum_required(VERSION 3.13)

project(git-starter-cpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

# Find all source files
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Find libcurl
find_package(CURL REQUIRED)

# Add the executable
add_executable(git ${SOURCE_FILES})

# Link libraries (zlib, OpenSSL, and libcurl)
target_link_libraries(git OpenSSL::Crypto -lz CURL::libcurl)
