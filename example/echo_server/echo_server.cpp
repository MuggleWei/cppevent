#include <signal.h>
#include "echo_server_handler.h"

#if WIN32
#include <muggle/cpp/mem_detect/mem_detect.h>
#endif

cppevent::EventLoopGroup *p_event_loop_group = nullptr;
void sighandler(int signum)
{
	switch (signum)
	{
	case SIGINT:
	{
		std::cout << "\nRecieve signal: SIGINT" << std::endl;
		if (p_event_loop_group)
		{
			p_event_loop_group->stop();
		}
	}break;
	}
}

void run(int argc, char *argv[])
{
	cppevent::EventLoopGroup event_loop_group;

	p_event_loop_group = &event_loop_group;
	signal(SIGINT, sighandler);

	event_loop_group
		.acceptNumber(1)
		.workerNumber(4)
		.setHandler(true, EchoServerHandler::getHandler)
		.idleTimeout(5)
		.option(cppevent::CPPEVENT_OPTION_BACKLOG, 512)
		.flag(cppevent::CPPEVENT_FLAG_REUSEPORT | cppevent::CPPEVENT_FLAG_REUSEADDR);

	if (argc == 2)
	{
		event_loop_group.bind(argv[1]);
	}
	else
	{
		event_loop_group.bind("127.0.0.1:10102");
	}

	event_loop_group.run();

	cppevent::EventLoop::GlobalClean();
}

int main(int argc, char *argv[])
{
#if WIN32
	muggle::DebugMemoryLeakDetect detect;
	detect.Start();
#endif

	run(argc, argv);

#if WIN32
	detect.End();
#endif

	return 0;
}