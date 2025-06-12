#pragma once

struct fuse_operations;

namespace nandroid::operations
{
    fuse_operations* get_operations();
}