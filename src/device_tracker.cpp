#include "device_tracker.hpp"
#include "logger.hpp"
#include "adb.hpp"

#include "nandroid.hpp"

#include <algorithm>
#include <memory>

namespace nandroid
{
    void DeviceTracker::update_connected_devices()
    {
        for(const std::string& device : adb::list_devices())
        {
            const auto iter = std::ranges::find_if(connected_devices, 
                [&device](std::unique_ptr<Nandroid>& connected_device)
                {
                    return connected_device->get_device() == device;
                });

            // if device is already mounted then ignore
            if(iter != connected_devices.end() && iter->get()->get_is_mounted()) continue;

            connect_device(device);
        }

        std::ignore =  std::erase_if(connected_devices, 
            [](std::unique_ptr<Nandroid>& device)
            {
                bool remove = !device->get_is_mounted();
                if(remove) Logger::info("Removing device {} as it is not mounted", device->get_device());
                return remove;
            });
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