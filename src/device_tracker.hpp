#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace nandroid
{
    class DeviceTracker
    {
    public:

        DeviceTracker() = default;

        void update_connected_devices();
        std::vector<std::string> get_adb_devices() const;
        
    private:

        void connect_device(const std::string& device);
        
        std::vector<std::string> connected_devices;
        uint16_t current_port = 25989;
    };
}