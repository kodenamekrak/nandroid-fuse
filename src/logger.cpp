#include "logger.hpp"

#include <fstream>
#include <cstdio>

static std::fstream log_stream;

namespace nandroid
{
    void Logger::init(const std::filesystem::path& logfile)
    {
        log_stream = std::fstream(logfile, std::ios::out | std::ios::trunc);
    }

    void Logger::log(const std::string& msg)
    {
        std::printf("%s\n", msg.c_str());

        if(!log_stream) return;
        log_stream << msg;
        log_stream << '\n';
    }

    void Logger::logerr(const std::string& msg)
    {
        std::fprintf(stderr, "%s\n", msg.c_str());

        if(!log_stream) return;
        log_stream << msg;
        log_stream << '\n';
    }
}