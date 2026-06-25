#include "adb.hpp"
#include "command.hpp"

#include <cstdlib>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>

static std::optional<std::string> adb_executable_path;

namespace nandroid::adb
{
    void locate_adb_executable()
    {
        std::string_view path_env(getenv("PATH"));
        size_t next_seperator_idx = std::string::npos;
        while(true)
        {
            next_seperator_idx = path_env.find_first_of(':');
            if(next_seperator_idx == std::string_view::npos) break;
            std::filesystem::path path_entry(path_env.substr(0, next_seperator_idx));
            path_env = path_env.substr(next_seperator_idx + 1);
            std::filesystem::path adb_path = path_entry / "adb";
            if(!std::filesystem::exists(adb_path)) {
                continue;
            }

            // ensure executable is valid
            int adb_exit_code = command::run_with_output(adb_path, {"--version"}, [](auto _){});
            if(adb_exit_code == 0)
            {
                adb_executable_path = adb_path;
                break;
            }
        }

        if(!adb_executable_path.has_value())
        {
            throw std::runtime_error("Unable to locate adb executable on path");
        }
    }

    std::vector<std::string> list_devices()
    {
        std::vector<std::string> devices;
        command::run_with_output(adb_executable_path.value(), {"devices"}, 
        [&devices](const std::string& line)
        {
            if(!line.ends_with("device")) return;
            devices.emplace_back(line.substr(0, line.find_first_of('\t')));
        });
        return devices;
    }

    int run_shell_command(const std::string& device, std::vector<std::string> args)
    {
        args.insert(args.begin(), "shell");
        args.insert(args.begin(), device);
        args.insert(args.begin(), "-s");
        return command::run(adb_executable_path.value(), args);
    }

    int run_shell_command_with_output(const std::string& device, std::vector<std::string> args, std::function<void(const std::string& line)> line_read_callback)
    {
        args.insert(args.begin(), "shell");
        args.insert(args.begin(), device);
        args.insert(args.begin(), "-s");
        return command::run_with_output(adb_executable_path.value(), args, line_read_callback);
    }

    int push_file(const std::string& device, const std::string& host_path, const std::string& remote_path)
    {
        return command::run(adb_executable_path.value(), {
            "-s",
            device,
            "push", 
            host_path, 
            remote_path
        });
    }

    int forward_ports(const std::string& device, int host_port, int remote_port)
    {
        return command::run(adb_executable_path.value(), {
            "-s",
            device,
            "forward", 
            "tcp:" + std::to_string(host_port),
            "tcp:" + std::to_string(remote_port)
        });
    }

    int remove_forward_port(const std::string& device, int host_port)
    {
        return command::run(adb_executable_path.value(), {
            "-s",
            device,
            "forward",
            "--remove",
            "tcp:" + std::to_string(host_port),
        });
    }
}