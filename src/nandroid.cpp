#include "nandroid.hpp"
#include "operations.hpp"
#include "adb.hpp"
#include "util.hpp"
#include "logger.hpp"
#include "command.hpp"

#include "nandroid_shared/requests.hpp"

#include <cerrno>
#include <format>
#include <print>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <filesystem>

#include <fuse.h>

constexpr std::string_view DAEMON_REMOTE_PATH("/data/local/tmp/nandroid-daemon");

namespace nandroid
{
    Nandroid::Nandroid(const std::string& device, uint16_t port): is_mounted(false), device(device), port(port), fuse_context(nullptr), connection(nullptr)
    {}

    Nandroid::~Nandroid()
    {
        if(is_mounted) unmount();
        if(connection) connection->close();
    }

    void Nandroid::connect()
    {
        push_daemon();

        agent_ready = false;
        daemon_process_thread = std::jthread(std::bind(&Nandroid::invoke_daemon_thread, this));
        daemon_process_thread.detach();

        while(!agent_ready)
        {
            std::this_thread::yield();
        }

        connection = std::make_unique<Connection>("127.0.0.1", port);

        mount();
    }

    void Nandroid::push_daemon()
    {
        if(int ex = adb::push_file(device, util::get_daemon_path(), std::string(DAEMON_REMOTE_PATH)); ex != 0)
        {
            throw std::runtime_error("Failed to push daemon to device");
        }
        
        if(int ex = adb::run_shell_command(device, {"chmod", "+x",  std::string(DAEMON_REMOTE_PATH)}); ex != 0)
        {
            throw std::runtime_error("Failed to chmod daemon");
        }
        
        if(int ex = adb::forward_ports(device, port, nandroidfs::AGENT_PORT); ex != 0)
        {
            throw std::runtime_error("Failed to forward device port");
        }
        std::println("Forwarding host port {} to remote port {}", port, nandroidfs::AGENT_PORT);
        std::println("Pushed daemon to device");
    }

    void Nandroid::mount()
    {
        Logger::info("[{}] Mounting device", device.c_str());

        std::string mp = get_mountpoint();
        std::error_code err;
        if(!std::filesystem::exists(mp, err))
        {
            // previous instance of program did not exit correctly and fs was not unmounted
            if(err.value() == ENOTCONN)
            {
                command::run("/usr/bin/fusermount3", {
                    "-u",
                    mp
                });
            }
            std::filesystem::create_directories(mp);
        }

        fuse_args args = FUSE_ARGS_INIT(0, nullptr);
        fuse_opt_add_arg(&args, "nandroid");
        fuse_context = fuse_new(&args, operations::get_operations(), sizeof(*operations::get_operations()), connection.get());
        fuse_opt_free_args(&args);
        fuse_mount(fuse_context, mp.c_str());
        
        fuse_mount_thread = std::jthread([fuse_context = this->fuse_context]() 
        {
            fuse_daemonize(1);
            fuse_loop(fuse_context);
        });
        fuse_mount_thread.detach();
        is_mounted = true;
    }

    void Nandroid::unmount()
    {
        Logger::info("[{}] Unmounting device", device);
        if(fuse_context)
        {
            fuse_unmount(fuse_context);
            fuse_context = nullptr;
            std::filesystem::remove(get_mountpoint());
        }
        adb::remove_forward_port(device, port);
        adb::run_shell_command(device, {
            "shell", 
            "killall", 
            "nandroid-daemon"
        });
        is_mounted = false;
    }

    void Nandroid::invoke_daemon_thread()
    {
        int exit_code = adb::run_shell_command_with_output(device, {
            std::string(DAEMON_REMOTE_PATH)
        },
            [this](const std::string& line)
            {
                Logger::info("[{}][daemon] {}", device, line);
                if(line.starts_with(nandroidfs::AGENT_READY_MSG))
                {
                    agent_ready = true;
                }
            }
        );
        if(exit_code != 0)
        {
            throw std::runtime_error("Failed to invoke daemon with exit code {}" + std::to_string(exit_code));
        }
    }

    const std::string& Nandroid::get_device()
    {
        return device;
    }

    std::string Nandroid::get_mountpoint()
    {
        return std::format("/run/user/{}/nandroid/{}", 1000, device);
    }
}