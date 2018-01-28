#include <signal.h>
#include "simple_client_handler.h"

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

	event_loop.setHandler(false, SimpleClientHandler::newClientHandler);

	event_loop.addConn("127.0.0.1:10102");

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