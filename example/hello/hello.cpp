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

	unsigned int n = std::thread::hardware_concurrency();
	n = n < 2 ? 2 : n;
	std::cout << "start 1 timer with 500ms interval" << std::endl;
	std::cout << "start " << n << " threads" << std::endl;
	std::cout << "sleep for 2 seconds..." << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(2));

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

	std::vector<std::thread> threads;
	for (unsigned int i = 0; i < n; i++)
	{
		threads.push_back(std::thread([&event_loop]{
			while (true)
			{
				if (is_done)
				{
					break;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				event_loop.profileTunnel();
			}
		}));
	}

	event_loop.run();
	th.join();

	for (unsigned int i = 0; i < n; i++)
	{
		threads[i].join();
	}

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
