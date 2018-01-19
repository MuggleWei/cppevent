#include "cppevent/cppevent.h"
#include <iostream>
#include <thread>
#include <future>
#include <chrono>

#if WIN32
#include <muggle/cpp/mem_detect/mem_detect.h>
#endif

void run()
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
			if (i == 5)
			{
				event_loop.stopTimer(timer);
			}
		}
		
		std::cout << "bye bye!" << std::endl;
		event_loop.stop();
	});

	th.detach();
	event_loop.run();
}

int main(int argc, char *argv[])
{
#if WIN32
	muggle::DebugMemoryLeakDetect detect;
	detect.Start();
#endif

	run();

#if WIN32
	detect.End();
#endif

	return 0;
}