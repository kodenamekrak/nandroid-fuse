#include "nandroid.hpp"
#include "util.hpp"

#include "nandroid_shared/requests.hpp"

#include <format>
#include <thread>

constexpr std::string_view DAEMON_LOCAL_PATH("/home/kodenamekrak/dev/C++/nandroid-fuse/nandroidfs/nandroid_daemon/libs/arm64-v8a/nandroid-daemon");
constexpr std::string_view DAEMON_REMOTE_PATH("/data/local/tmp/nandroid-daemon");

namespace nandroid
{
    Nandroid::Nandroid(const std::string& device, uint16_t port): device(device), port(port)
    {}

    Nandroid::~Nandroid()
    {
        daemon_process_thread.join();
    }

    void Nandroid::connect()
    {
        push_daemon();

        agent_ready = false;
        daemon_process_thread = std::thread(std::bind(&Nandroid::invoke_daemon_thread, this));
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
        if(int ex = util::run_adb_command_with_device(std::format("push \"{}\" {}", DAEMON_LOCAL_PATH, DAEMON_REMOTE_PATH), device); ex != 0)
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
        std::printf("Mounting device %s\n", device.c_str());
    }

    void Nandroid::invoke_daemon_thread()
    {
        util::run_adb_command_stream_output(std::format("-s {} shell {}", device, DAEMON_REMOTE_PATH),
        [this](const std::string& line)
        {
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
}