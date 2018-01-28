#include <signal.h>
#include "echo_server_handler.h"

#if WIN32
#include <muggle/cpp/mem_detect/mem_detect.h>
#endif

cppevent::EventLoop *p_event_loop = nullptr;
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
		}
	}break;
	}
}

void run()
{
	cppevent::EventLoop event_loop;

	p_event_loop = &event_loop;
	signal(SIGINT, sighandler);

	event_loop.setHandler(true, EchoServerHandler::getHandler);
	std::future<int> ret1 = event_loop.bindAndListen("127.0.0.1:10102", 512);
	if (ret1.get() == 0)
	{
		std::cout << "bind and listen in 127.0.0.1:10102" << std::endl;
	}
	else
	{
		std::cout << "failed bind and listen in 127.0.0.1:10102" << std::endl;
	}

	event_loop.run();

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