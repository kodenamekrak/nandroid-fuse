#include "operations.hpp"
#include "logger.hpp"
#include "connection.hpp"

#include <fuse.h>

namespace nandroid::operations
{
    static std::string get_remote_path(const std::string& local_path)
    {
        if(local_path == "/")
        {
            return "/sdcard";
        }

        return std::filesystem::path("/sdcard") / local_path.substr(1);
    }

    void* op_init(fuse_conn_info *conn, fuse_config *cfg)
    {
        return fuse_get_context()->private_data;
    }

    int op_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
    {
        filler(buf, ".", NULL, 0, (fuse_fill_dir_flags)0);
	    filler(buf, "..", NULL, 0, (fuse_fill_dir_flags)0);

        Connection* connection = reinterpret_cast<Connection*>(fuse_get_context()->private_data);
        
        const std::function consume_func = [buf, filler](const std::string& file_name, const FileStat& stat)
        {
            struct stat stbuf;
            stbuf.st_atim.tv_sec = stat.access_time;
            stbuf.st_mtim.tv_sec = stat.write_time;
            stbuf.st_mode = stat.mode;
            stbuf.st_size = stat.size;
            stbuf.st_nlink = 1;
            filler(buf, file_name.c_str(), &stbuf, 0, (fuse_fill_dir_flags)0);
        };

        ResponseStatus status = connection->req_readdir(get_remote_path(path), consume_func);
        if(status != ResponseStatus::Success)
        {
            return -1;
        }

        return 0;
    }

    int op_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)
    {
        Connection* connection = reinterpret_cast<Connection*>(fuse_get_context()->private_data);

        FileStat stat;
        ResponseStatus status = connection->req_getattr(get_remote_path(path), stat);
        if(status != ResponseStatus::Success)
        {
            return -1;
        }

        stbuf->st_atim.tv_sec = stat.access_time;
        stbuf->st_mtim.tv_sec = stat.write_time;
        stbuf->st_mode = stat.mode;
        stbuf->st_size = stat.size;
        stbuf->st_nlink = 1;

        return 0;
    }

    static fuse_operations operations{
        .getattr = op_getattr,
        .readdir = op_readdir,
        .init = op_init,
    };

    fuse_operations* get_operations()
    {
        return &operations;
    }
}