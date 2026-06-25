#include "util.hpp"
#include "assets.hpp"

#include <filesystem>
#include <fstream>

static std::string daemon_path;

namespace nandroid::util
{
    const std::string& get_daemon_path()
    {
        if(daemon_path.empty())
        {
            std::filesystem::path temp_dir = std::filesystem::temp_directory_path() / "nandroid";
            if(!std::filesystem::exists(temp_dir))
            {
                std::filesystem::create_directories(temp_dir);
            }

            daemon_path = temp_dir / "nandroid-daemon";
            std::ofstream stream(daemon_path, std::ios::out | std::ios::binary);
            std::span<uint8_t> bytes = assets::get_daemon_bytes();
            stream.write(reinterpret_cast<const char*>(bytes.data()), bytes.size_bytes());
            stream.close();
        }

        return daemon_path;
    }
}
