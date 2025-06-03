#pragma once

#include <vector>
#include <string>
#include <optional>
#include <functional>

namespace nandroid::util
{
    int run_command(const std::string& cmd, std::optional<std::reference_wrapper<std::string>> stdout = std::nullopt, std::optional<std::reference_wrapper<std::string>> stderr = std::nullopt);

    int run_command_stream_output(const std::string& cmd, const std::function<void(const std::string&)>& consume_output);

    int run_adb_command(const std::string& cmd, std::optional<std::reference_wrapper<std::string>> stdout = std::nullopt, std::optional<std::reference_wrapper<std::string>> stderr = std::nullopt);

    int run_adb_command_with_device(const std::string& cmd, const std::string& device, std::optional<std::reference_wrapper<std::string>> stdout = std::nullopt, std::optional<std::reference_wrapper<std::string>> stderr = std::nullopt);

    int run_adb_command_stream_output(const std::string& cmd, const std::function<void(const std::string&)>& consume_output);

    std::vector<std::string> split_string(const std::string& str, char sep);

    const std::string& find_adb_path();
}