#include <signal.h>
#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include "cppevent/cppevent.h"

#if WIN32
#include <muggle/cpp/mem_detect/mem_detect.h>
#endif

cppevent::EventLoop *p_event_loop = nullptr;
bool is_done = false;
void sighandler(int signum)
{
	switch (signum)
	{
		case SIGINT:
		{
			std::cout << "\nRecieve signal: SIGINT" << std::endl;
			if (p_event_loop)
			{
				p_event_loop->stop();
				is_done = true;
			}
		}break;
	}
}

void run()
{
	cppevent::EventLoop event_loop;

	p_event_loop = &event_loop;
	signal(SIGINT, sighandler);

	std::future<cppevent::Timer*> future = event_loop.addTimer(500, [] {
		std::cout << "hello" << std::endl;
	});
	cppevent::Timer *timer = future.get();

	std::thread th([&event_loop, &timer] {
		int cnt = 0;
		while(true)
		{
			event_loop.profileTunnel();
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			if (cnt == 3)
			{
				event_loop.stopTimer(timer);
				std::cout << "stop hello" << std::endl;
			}
			++cnt;

			if (is_done)
			{
				break;
			}
		}
	});

	event_loop.run();
	th.join();

	std::cout << "bye byte" << std::endl;

	cppevent::EventLoop::GlobalClean();
}

int main(int , char **)
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
