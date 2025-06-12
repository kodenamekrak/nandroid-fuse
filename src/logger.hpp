#pragma once

#include <string>
#include <format>
#include <filesystem>

namespace nandroid
{
    class Logger
    {
    public:
    
        Logger() = delete;

        void init(const std::filesystem::path& logfile);

        template<typename... _Args>
        static inline void info(std::format_string<_Args...> fmt, _Args&&... args)
        {
            log(std::format("{}: {}", "INFO", std::format(std::forward<std::format_string<_Args...>>(fmt), std::forward<_Args...>(args)...)));
        }

        template<typename... _Args>
        static inline void verbose(std::format_string<_Args...> fmt, _Args&&... args)
        {
            log(std::format("{}: {}", "VERBOSE", std::format(std::forward<std::format_string<_Args...>>(fmt), std::forward<_Args...>(args)...)));
        }

        template<typename... _Args>
        static inline void error(std::format_string<_Args...> fmt, _Args&&... args)
        {
            logerr(std::format("{}: {}", "ERROR", std::format(std::forward<std::format_string<_Args...>>(fmt), std::forward<_Args...>(args)...)));
        }

        static void log(const std::string& msg);
        static void logerr(const std::string& msg);
    };
}