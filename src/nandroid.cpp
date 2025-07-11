#include "nandroid.hpp"
#include "operations.hpp"
#include "util.hpp"
#include "logger.hpp"

#include "nandroid_shared/requests.hpp"

#include <format>
#include <thread>
#include <filesystem>

#include <fuse.h>

constexpr std::string_view DAEMON_REMOTE_PATH("/data/local/tmp/nandroid-daemon");

namespace nandroid
{
    Nandroid::Nandroid(const std::string& device, uint16_t port): device(device), port(port)
    {}

    Nandroid::~Nandroid()
    {
        unmount();
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
        if(int ex = util::run_adb_command_with_device(std::format("push \"{}\" {}", util::get_daemon_path(), DAEMON_REMOTE_PATH), device); ex != 0)
        {
            throw std::runtime_error("Failed to push daemon to device");
        }
        
        if(int ex = util::run_adb_command_with_device(std::format("shell chmod +x {}", DAEMON_REMOTE_PATH), device); ex != 0)
        {
            throw std::runtime_error("Failed to chmod daemon");
        }
        
        if(int ex = util::run_adb_command_with_device(std::format("forward tcp:{} tcp:{}", port, nandroidfs::AGENT_PORT), device); ex != 0)
        {
            throw std::runtime_error("Failed to forward device port");
        }
    }

    void Nandroid::mount()
    {
        Logger::info("[{}] Mounting device", device.c_str());

        std::string mp = get_mountpoint();
        if(!std::filesystem::exists(mp))
        {
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
        util::run_adb_command_with_device("shell killall nandroid-daemon", device);
    }

    void Nandroid::invoke_daemon_thread()
    {
        util::run_adb_command_stream_output(std::format("-s {} shell {}", device, DAEMON_REMOTE_PATH),
        [this](const std::string& line)
        {
            Logger::info("[{}][daemon] {}", device, line);
            if(line.starts_with(nandroidfs::AGENT_READY_MSG))
            {
                agent_ready = true;
            }
        });
    }

    const std::string& Nandroid::get_device()
    {
        return device;
    }

    std::string Nandroid::get_mountpoint()
    {
        return std::format("/run/user/{}/nandroid/{}", getuid(), device);
    }
}