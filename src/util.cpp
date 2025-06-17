#include "util.hpp"
#include "assets.hpp"

#include <boost/process.hpp>

#include <filesystem>
#include <fstream>

static std::string adb_path;
static std::string daemon_path;

namespace nandroid::util
{
    int run_command(const std::string& cmd, std::optional<std::reference_wrapper<std::string>> stdout, std::optional<std::reference_wrapper<std::string>> stderr)
    {
        using namespace boost::process;
    
        ipstream outstream;
        ipstream errstream;
        child child(cmd, std_out > outstream, std_err > errstream);
    
        if(stdout)
        {
            std::string tmp;
            while(outstream && std::getline(outstream, tmp) && !tmp.empty())
            {
                stdout->get().append(tmp).push_back('\n');
            }
        }
    
        if(stderr)
        {
            std::string tmp;
            while(errstream && std::getline(errstream, tmp) && !tmp.empty())
            {
                stderr->get().append(tmp).push_back('\n');
            }
        }
    
        child.wait();
        return child.exit_code();
    }

    int run_command_stream_output(const std::string& cmd, const std::function<void(const std::string&)>& consume_output)
    {
        using namespace boost::process;
    
        ipstream outstream;
        child child(cmd, std_out > outstream);
    
        if(stdout)
        {
            std::string line;
            while(outstream && std::getline(outstream, line) && !line.empty())
            {
                consume_output(line);
            }
        }
    
        child.wait();
        return child.exit_code();
    }

    int run_adb_command(const std::string& cmd, std::optional<std::reference_wrapper<std::string>> stdout, std::optional<std::reference_wrapper<std::string>> stderr)
    {
        return run_command(std::format("{} {}", find_adb_path(), cmd), stdout, stderr);
    }

    int run_adb_command_with_device(const std::string& cmd, const std::string& device, std::optional<std::reference_wrapper<std::string>> stdout, std::optional<std::reference_wrapper<std::string>> stderr)
    {
        return run_command(std::format("{} -s {} {}", find_adb_path(), device, cmd), stdout, stderr);
    }

    int run_adb_command_stream_output(const std::string& cmd, const std::function<void(const std::string&)>& consume_output)
    {
        return run_command_stream_output(std::format("{} {}", find_adb_path(), cmd), consume_output);
    }
    
    std::vector<std::string> split_string(const std::string& str, char sep)
    {
        std::vector<std::string> ret;
        boost::split(ret, str, [sep](char c){return c == sep;});
        return ret;
    }

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

    const std::string& find_adb_path()
    {
        if(adb_path.empty())
        {
            adb_path = boost::process::search_path("adb").string();
        }


        if(adb_path.empty())
        {
            throw std::runtime_error("Could not locate ADB on path!");
        }

        return adb_path;
    }
}
