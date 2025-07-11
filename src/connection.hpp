#pragma once

#include "socket.hpp"

#include "nandroid_shared/serialization.hpp"
#include "nandroid_shared/responses.hpp"

#include <string>
#include <functional>

namespace nandroid
{
    using nandroidfs::ResponseStatus;
    using nandroidfs::RequestType;
    using nandroidfs::FileStat;

    class Connection
    {
    public:
        
        Connection(const std::string& address, uint16_t port);

        ResponseStatus req_readdir(const std::string& path, const std::function<void(const std::string&, const FileStat&)>& consume);
        ResponseStatus req_getattr(const std::string& path, FileStat& out);
        ResponseStatus req_open(const std::string& path, nandroidfs::OpenMode mode, bool read_access, bool write_access, int& out_handle);
        ResponseStatus req_release(int handle);
        ResponseStatus req_read(int handle, uint8_t* buffer, size_t size, off_t offset, int& bytes_read);
        ResponseStatus req_write(int handle, uint8_t* buffer, size_t size, off_t offset);

    private:

        void perform_handshake();

        nandroidfs::DataReader reader;
        nandroidfs::DataWriter writer;
        Socket socket;
    };
}