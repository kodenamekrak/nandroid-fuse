#pragma once

#include <vector>
#include <string>
#include <functional>

namespace nandroid::adb
{
    void locate_adb_executable();
    std::vector<std::string> list_devices();

    int run_shell_command(const std::string& device, std::vector<std::string> args);
    int run_shell_command_with_output(const std::string& device, std::vector<std::string> args, std::function<void(const std::string& line)> line_read_callback);
    int push_file(const std::string& device, const std::string& host_path, const std::string& remote_path);
    int forward_ports(const std::string& device, int host_port, int remote_port);
}