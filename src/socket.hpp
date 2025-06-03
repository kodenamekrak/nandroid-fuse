#pragma once

#include "nandroid_shared/serialization.hpp"

namespace nandroid
{
    class Socket : public::nandroidfs::Readable, public nandroidfs::Writable
    {
    public:

        Socket(const std::string& address, uint16_t port);
        
        ~Socket();
    
        int read(uint8_t* buffer, int length) override;
        void write(const uint8_t* buffer, int length) override;

    private:

        int socket_descriptor;
        std::string address;
        uint16_t port;
    };
}