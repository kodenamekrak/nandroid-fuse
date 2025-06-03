#pragma once

#include "socket.hpp"

#include "nandroid_shared/serialization.hpp"

#include <string>

namespace nandroid
{
    class Connection
    {
    public:
        
        Connection(const std::string& address, uint16_t port);
    
    private:

        void perform_handshake();

        nandroidfs::DataReader reader;
        nandroidfs::DataWriter writer;
        Socket socket;
    };
}