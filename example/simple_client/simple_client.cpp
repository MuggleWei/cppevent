#include <signal.h>
#include <thread>
#include <mutex>
#include <chrono>
#include "simple_client_handler.h"

#if WIN32
#include <muggle/cpp/mem_detect/mem_detect.h>
#endif

cppevent::EventLoop *p_event_loop = nullptr;
std::mutex mtx;
bool is_done = false;
void sighandler(int signum)
{
	switch (signum)
	{
	case SIGINT:
	{
		std::cout << "\nRecieve signal: SIGINT" << std::endl;
		std::unique_lock<std::mutex> lock(mtx);
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
	while (true)
	{
		signal(SIGINT, sighandler);

		cppevent::EventLoop event_loop;
		event_loop.setHandler(false, SimpleClientHandler::newClientHandler);
		event_loop.addConn("127.0.0.1:10102");

		{
			std::unique_lock<std::mutex> lock(mtx);
			if (is_done)
			{
				break;
			}
			p_event_loop = &event_loop;
		}
		event_loop.run();
		{
			std::unique_lock<std::mutex> lock(mtx);
			p_event_loop = nullptr;
		}

		if (is_done)
		{
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		std::cout << "try to reconnect..." << std::endl;
	}

	std::cout << "bye byte" << std::endl;

	cppevent::EventLoop::GlobalClean();
}

int main()
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