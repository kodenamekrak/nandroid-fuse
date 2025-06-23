#include "operations.hpp"
#include "logger.hpp"
#include "connection.hpp"

#include <fuse.h>

using nandroidfs::OpenMode;

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

    static int to_unix_err(ResponseStatus resp)
    {
        switch(resp)
        {
            case ResponseStatus::AccessDenied: return -EACCES;
            case ResponseStatus::NotADirectory: return -ENOTDIR;
            case ResponseStatus::NotAFile: return -EISDIR;
            case ResponseStatus::FileNotFound: return -ENOENT;
            case ResponseStatus::FileExists: return -EEXIST;
            case ResponseStatus::DirectoryNotEmpty: return -ENOTDIR;
            default: return -1;
        }
    }

    static OpenMode to_open_mode(int flags)
    {
        if      (flags & O_CREAT & O_EXCL)  return OpenMode::CreateAlways;
        else if (flags & O_CREAT & O_TRUNC) return OpenMode::CreateOrTruncate;
        else if (flags & O_TRUNC)           return OpenMode::Truncate;
        else if (flags & O_CREAT)           return OpenMode::CreateIfNotExist;

        return OpenMode::OpenOnly;
    }

    void* op_init(fuse_conn_info *conn, fuse_config *cfg)
    {
        Logger::verbose("op_init({}, {})", (void*)conn, (void*)cfg);
        return fuse_get_context()->private_data;
    }

    int op_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
    {
        Logger::verbose("op_readdir({}, {}, {}, {}, {}, {})", path, buf, (void*)filler, offset, (void*)fi, (uint8_t)flags);

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
            return to_unix_err(status);
        }

        return 0;
    }

    int op_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)
    {
        Logger::verbose("op_getattr({}, {}, {})", path, (void*)stbuf, (void*)fi);

        Connection* connection = reinterpret_cast<Connection*>(fuse_get_context()->private_data);

        FileStat stat;
        ResponseStatus status = connection->req_getattr(get_remote_path(path), stat);
        if(status != ResponseStatus::Success)
        {
            return to_unix_err(status);
        }

        stbuf->st_atim.tv_sec = stat.access_time;
        stbuf->st_mtim.tv_sec = stat.write_time;
        stbuf->st_mode = stat.mode;
        stbuf->st_size = stat.size;
        stbuf->st_nlink = 1;

        return 0;
    }

    int op_open(const char * path, struct fuse_file_info* fi)
    {
        Logger::verbose("op_open({}, {})", path, (void*)fi);

        Connection* connection = reinterpret_cast<Connection*>(fuse_get_context()->private_data);

        // TODO: Error on additional unsupported flags?
        int handle;
        bool read = (fi->flags & O_ACCMODE) == O_RDONLY || (fi->flags & O_ACCMODE) == O_RDWR;
        bool write = (fi->flags & O_ACCMODE) == O_WRONLY || (fi->flags & O_ACCMODE) == O_RDWR;
        OpenMode mode = to_open_mode(fi->flags);
        
        ResponseStatus status = connection->req_open(get_remote_path(path), mode, read, write, handle);
        if(status != ResponseStatus::Success)
        {
            return to_unix_err(status);
        }

        fi->fh = handle;
        return 0;
    }

    int op_create(const char* path, mode_t mode, struct fuse_file_info* fi)
    {
        Logger::verbose("op_create({}, {}, {})", path, mode, (void*)fi);

        Connection* connection = reinterpret_cast<Connection*>(fuse_get_context()->private_data);

        // TODO: pass mode somehow? chmod afterwords?
        int handle;
        bool read = (fi->flags & O_ACCMODE) == O_RDONLY || (fi->flags & O_ACCMODE) == O_RDWR;
        bool write = (fi->flags & O_ACCMODE) == O_WRONLY || (fi->flags & O_ACCMODE) == O_RDWR;
        OpenMode open_mode = to_open_mode(fi->flags);
        
        ResponseStatus status = connection->req_open(get_remote_path(path), open_mode, read, write, handle);
        if(status != ResponseStatus::Success)
        {
            return to_unix_err(status);
        }

        fi->fh = handle;
        return 0;
    }

    int op_release(const char* path, struct fuse_file_info* fi)
    {
        Logger::verbose("op_release({}, {})", path, (void*)fi);

        Connection* connection = reinterpret_cast<Connection*>(fuse_get_context()->private_data);

        ResponseStatus status = connection->req_release(fi->fh);
        if(status != ResponseStatus::Success)
        {
            return to_unix_err(status);
        }

        return 0;
    }

    int op_read(const char* path, char* buffer, size_t size, off_t offset, struct fuse_file_info* fi)
    {
        Logger::verbose("op_read({}, {}, {}, {}, {})", path, (void*)buffer, size, offset, (void*)fi);
        
        Connection* connection = reinterpret_cast<Connection*>(fuse_get_context()->private_data);

        int bytes_read = 0;
        ResponseStatus status = connection->req_read(fi->fh, reinterpret_cast<uint8_t*>(buffer), size, offset, bytes_read);
        if(status != ResponseStatus::Success)
        {
            return 0;
        }

        return bytes_read;
    }

    int op_write(const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info* fi)
    {
        Logger::verbose("op_write({}, {}, {}, {}, {})", path, (void*)buffer, size, offset, (void*)fi);

        Connection* connection = reinterpret_cast<Connection*>(fuse_get_context()->private_data);

        ResponseStatus status = connection->req_write(fi->fh, (uint8_t*)buffer, size, offset);
        if(status != ResponseStatus::Success)
        {
            return -1;
        }

        // This is probably technically UB
        // But the daemon doesnt report how much was written
        // So we just assume no error means everything was written
        return size;
    }

    static fuse_operations operations{
        .getattr = op_getattr,
        .open = op_open,
        .read = op_read,
        .write = op_write,
        .release = op_release,
        .readdir = op_readdir,
        .init = op_init,
        .create = op_create,
    };

    fuse_operations* get_operations()
    {
        return &operations;
    }
}