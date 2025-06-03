#include "socket.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <format>
#include <cstring>

namespace nandroid
{
    Socket::Socket(const std::string& address, uint16_t port): address(address), port(port)
    {
        sockaddr_in socket_address;
        socket_address.sin_family = AF_INET;
        socket_address.sin_addr.s_addr = inet_addr(address.c_str());
        socket_address.sin_port = htons(port);
        
        socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
        if(int res = connect(socket_descriptor, (sockaddr*)&socket_address, sizeof(socket_address)); res != 0)
        {
            throw std::runtime_error(std::format("Failed to connect socket! [errno {}] {}", errno, std::strerror(errno)));
        }
    }

    Socket::~Socket()
    {
        close(socket_descriptor);
    }

    int Socket::read(uint8_t* buffer, int length)
    {
        return ::read(socket_descriptor, buffer, length);
    }

    void Socket::write(const uint8_t* buffer, int length)
    {
        ::write(socket_descriptor, buffer, length);
    }
}