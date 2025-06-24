#include "device_tracker.hpp"
#include "logger.hpp"
#include "config.hpp"

#include <csignal>
#include <thread>
#include <vector>

using namespace nandroid;

DeviceTracker* device_tracker;

void signal_handler(int sig)
{
	device_tracker->disconnect_all();
	delete device_tracker;
	exit(0);
}

int main(int argc, char *argv[])
{
	std::vector<std::string_view> args;
	for(auto p = argv; p != argv + argc; p++)
	{
		args.emplace_back(*p);
	}

	Config::parse(args);

	struct sigaction action{};
	sigfillset(&action.sa_mask);
	action.sa_handler = signal_handler;
	sigaction(SIGINT, &action, nullptr);
	sigaction(SIGTERM, &action, nullptr);
	
	device_tracker = new DeviceTracker();
	try
	{
		while(true)
		{
			device_tracker->update_connected_devices();
			std::this_thread::sleep_for(std::chrono::seconds(2));
		}
	} 
	catch(const std::exception& e)
	{
		Logger::error("Caught exception!\n\t{}", e.what());
	}
	catch(...)
	{
		Logger::error("Caught exception with no what() method!");
	}

	delete device_tracker;
	return 0;
}