#! /bin/sh
sed 's/#ifdef __ANDROID__/#if defined(__ANDROID__) || defined(__linux)\n#include <cstring>/' nandroidfs/nandroid_shared/serialization.cpp -i
cmake -B build -GNinja
cmake --build build