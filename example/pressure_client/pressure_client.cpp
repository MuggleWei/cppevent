#include <signal.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include "pressure_client_handler.h"

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

void run(int argc, char *argv[])
{
	cppevent::EventLoop event_loop;

	p_event_loop = &event_loop;
	signal(SIGINT, sighandler);

	event_loop.setHandler(true, PressureClientHandler::getHandler);

	if (argc == 2)
	{
		event_loop.addConn(argv[1]);
	}
	else
	{
		event_loop.addConn("127.0.0.1:10102");
	}
	
	event_loop.run();

	std::cout << "bye byte" << std::endl;

	cppevent::EventLoop::GlobalClean();
}

int main(int argc, char *argv[])
{
	run(argc, argv);

	return 0;
}