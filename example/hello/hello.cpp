#include "cppevent/cppevent.h"
#include <iostream>
#include <thread>
#include <future>
#include <chrono>

int main(int argc, char *argv[])
{
	cppevent::EventLoop event_loop;

	std::future<cppevent::Timer*> future = event_loop.addTimer(500, [] {
		std::cout << "hello" << std::endl;
	});
	cppevent::Timer *timer = future.get();

	std::thread th([&event_loop, &timer] {
		for (int i = 0; i < 10; ++i)
		{
			event_loop.profileTunnel();
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		event_loop.stopTimer(timer);
	});

	th.detach();
	event_loop.run();

	return 0;
}