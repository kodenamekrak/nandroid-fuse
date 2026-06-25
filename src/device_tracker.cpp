#include "device_tracker.hpp"
#include "logger.hpp"
#include "adb.hpp"

#include "nandroid.hpp"

#include <algorithm>

namespace nandroid
{
    void DeviceTracker::update_connected_devices()
    {
        for(const std::string& device : adb::list_devices())
        {
            if(std::ranges::any_of(connected_devices, 
               [&](const auto& p){return p->get_device() == device;}))
            {
                continue;
            }

            connect_device(device);
        }
    }

    void DeviceTracker::connect_device(const std::string& device)
    {
        Logger::info("[{}] Connecting device", device.c_str());
        
        try
        {
            std::unique_ptr nandroid = std::make_unique<Nandroid>(device, current_port);
            nandroid->connect();
            
            connected_devices.emplace_back(std::move(nandroid));
        }
        catch(const std::exception& e)
        {
            nandroid::Logger::error("[{}] Caught exception while mounting device!\t{}", device, e.what());
        }

        current_port++;
    }

    void DeviceTracker::disconnect_all()
    {
        // Destructors will unmount the devices
        connected_devices.clear();
    }
}