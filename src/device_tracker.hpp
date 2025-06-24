#pragma once

#include "nandroid.hpp"

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

namespace nandroid
{
    class DeviceTracker
    {
    public:

        DeviceTracker() = default;

        void update_connected_devices();
        void disconnect_all();
        std::vector<std::string> get_adb_devices() const;
        
    private:

        void connect_device(const std::string& device);
        
        std::vector<std::unique_ptr<Nandroid>> connected_devices;
        uint16_t current_port = 25989;
    };
}