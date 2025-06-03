#include "connection.hpp"

#include <format>

static const int BUFFER_SIZE = 2048;
static const uint32_t HANDSHAKE_DATA = 0xFAFE5ABE;

namespace nandroid
{
    Connection::Connection(const std::string& address, uint16_t port): socket(address, port), reader(&socket, BUFFER_SIZE), writer(&socket, BUFFER_SIZE)
    {
        perform_handshake();
    }

    void Connection::perform_handshake()
    {
        writer.write_u32(HANDSHAKE_DATA);
        writer.flush();

        uint32_t received_data = reader.read_u32();
        if(received_data != HANDSHAKE_DATA)
        {
            throw std::runtime_error(std::format("Handshake failed (expected {:X}, received {:X})", HANDSHAKE_DATA, received_data));
        }
    }
}