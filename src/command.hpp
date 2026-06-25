#pragma once

#include <string>
#include <vector>
#include <functional>

namespace nandroid::command
{
    int run_with_output(const std::string& executable_path, std::vector<std::string> parameters, std::function<void(const std::string& line)> line_read_callback);
    int run(const std::string& executable_path, std::vector<std::string> parameters);
}