#pragma once

#include <span>
#include <string_view>

namespace nandroid
{
    class Config
    {
    public:

        Config() = delete;

        static void parse(const std::span<std::string_view> args);

        static bool verbose();

    private:

        static bool verbose_logs;
    };
}