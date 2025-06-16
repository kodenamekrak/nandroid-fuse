#include "config.hpp"

#include <algorithm>

namespace nandroid
{
    bool Config::verbose_logs = false;

    void Config::parse(const std::span<std::string_view> args)
    {
        verbose_logs = std::ranges::contains(args, "-v");
    }


    bool Config::verbose()
    {
        return verbose_logs;
    }
}