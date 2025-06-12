cmake_minimum_required(VERSION 3.20)

project(nandroid)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

file(GLOB_RECURSE CPP_FILE_LIST ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE NANDROID_SHARED_CPP_FILE_LIST ${CMAKE_CURRENT_LIST_DIR}/nandroidfs/nandroid_shared/*.cpp)

include_directories(nandroidfs)

include_directories(/usr/include/fuse3)

add_compile_definitions(_FILE_OFFSET_BITS=64 FUSE_USE_VERSION=31)

add_executable(nandroid
    ${CPP_FILE_LIST}
    ${NANDROID_SHARED_CPP_FILE_LIST}
)

target_link_libraries(nandroid boost_filesystem boost_system fuse3)