#include "device_tracker.hpp"
#include "util.hpp"

#include <boost/process/search_path.hpp>

#include "nandroid.hpp"

namespace nandroid
{
    void DeviceTracker::update_connected_devices()
    {
        for(const std::string& device : get_adb_devices())
        {
            if(std::ranges::contains(connected_devices, device))
            {
                continue;
            }

            connect_device(device);
        }
    }

    std::vector<std::string> DeviceTracker::get_adb_devices() const
    {
        std::string adbout;
        if(int ex = util::run_adb_command("devices", adbout); ex != 0)
        {
            std::fprintf(stderr, "Failed to execute adb: Error code %i\n", ex);
            return {};
        }

        std::vector<std::string> lines = util::split_string(adbout, '\n');

        std::vector<std::string> devices;
        for(const std::string& line : lines)
        {
            if(line == "List of devices attached")
            {
                continue;
            }

            if(line.find("device") == std::string::npos)
            {
                continue;
            }

            devices.push_back(util::split_string(line, '\t')[0]);
        }

        return devices;
    }

    void DeviceTracker::connect_device(const std::string& device)
    {
        std::printf("Connecting device %s\n", device.c_str());
        
        Nandroid* nandroid = new Nandroid(device, current_port);
        nandroid->connect();
        current_port++;
        
        connected_devices.push_back(device);
    }
}