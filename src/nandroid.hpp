#pragma once

#include "connection.hpp"

#include <thread>
#include <atomic>
#include <memory>

struct fuse;

namespace nandroid
{
    class Nandroid
    {
    public:
        
        Nandroid(const std::string& device, uint16_t port);
        ~Nandroid();

        void connect();
        
        const std::string& get_device();
        std::string get_mountpoint();

    private:

        void push_daemon();
        void mount();
        void unmount();

        void invoke_daemon_thread();
        
        std::string device;
        uint16_t port;
        fuse* fuse_context = nullptr;
        std::unique_ptr<Connection> connection;
        std::atomic<bool> agent_ready;
        std::jthread daemon_process_thread;
        std::jthread fuse_mount_thread;
    };
}