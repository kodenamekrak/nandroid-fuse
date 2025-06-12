#include "connection.hpp"
#include "logger.hpp"

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

    ResponseStatus Connection::req_readdir(const std::string& path, const std::function<void(const std::string&, const nandroidfs::FileStat&)>& consume)
    {
        try
        {
            writer.write_byte((uint8_t)nandroidfs::RequestType::ListDirectory);
            writer.write_utf8_string(path);
            writer.flush();
    
            ResponseStatus status = (ResponseStatus)reader.read_byte();
            if(status != ResponseStatus::Success)
            {
                return status;
            }
    
            while((status = (ResponseStatus)reader.read_byte()) != ResponseStatus::NoMoreEntries)
            {
                if(status == ResponseStatus::Success)
                {
                    std::string filename = reader.read_utf8_string();
                    FileStat stat(reader);
    
                    consume(filename, stat);
                }
            }
        }
        catch(const std::exception& e)
        {
            nandroid::Logger::error("Caught exception!\t{}", e.what());
            return ResponseStatus::GenericFailure;
        }

        return ResponseStatus::Success;
    }

    ResponseStatus Connection::req_getattr(const std::string& path, FileStat& out)
    {
        try 
        {
            writer.write_byte((uint8_t)RequestType::StatFile);
            writer.write_utf8_string(path);
            writer.flush();
    
            ResponseStatus status = (ResponseStatus)reader.read_byte();
            if(status != ResponseStatus::Success)
            {
                return status;
            }
    
            out = FileStat(reader);
        }
        catch(const std::exception& e)
        {
            nandroid::Logger::error("Caught exception!\t{}", e.what());
            return ResponseStatus::GenericFailure;
        }

        return ResponseStatus::Success;
    }
}