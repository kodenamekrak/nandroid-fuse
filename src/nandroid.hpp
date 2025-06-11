#pragma once

#include "connection.hpp"

#include <thread>
#include <atomic>
#include <memory>

namespace nandroid
{
    class Nandroid
    {
    public:
        
        Nandroid(const std::string& device, uint16_t port);
        ~Nandroid();

        void connect();
        
        const std::string& get_device();

    private:

        void push_daemon();
        void mount();

        void invoke_daemon_thread();
        
        std::string device;
        uint16_t port;
        std::unique_ptr<Connection> connection;
        std::atomic<bool> agent_ready;
        std::thread daemon_process_thread;
    };
}