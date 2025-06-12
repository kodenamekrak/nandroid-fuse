#include "operations.hpp"

#include <fuse.h>

namespace nandroid::operations
{
    static fuse_operations operations{};

    fuse_operations* get_operations()
    {
        return &operations;
    }
}