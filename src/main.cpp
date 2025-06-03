#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "device_tracker.hpp"
int main(int argc, char *argv[])
{
	nandroid::DeviceTracker tracker;
	tracker.update_connected_devices();
}