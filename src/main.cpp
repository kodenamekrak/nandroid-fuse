#include "device_tracker.hpp"
#include "logger.hpp"

#include <csignal>
#include <atomic>
#include <thread>

static std::atomic<bool> quit = false;

void signal_handler(int sig)
{
	quit = true;
}

int main(int argc, char *argv[])
{
	struct sigaction action{};
	sigfillset(&action.sa_mask);
	action.sa_handler = signal_handler;
	sigaction(SIGINT, &action, nullptr);
	
	nandroid::DeviceTracker tracker;
	try
	{
		while(!quit)
		{
			tracker.update_connected_devices();
			std::this_thread::sleep_for(std::chrono::seconds(2));
		}
	} 
	catch(const std::exception& e)
	{
		nandroid::Logger::error("Caught exception!\n\t{}", e.what());
	}
	catch(...)
	{
		nandroid::Logger::error("Caught exception with no what() method!");
	}

	return 0;
}