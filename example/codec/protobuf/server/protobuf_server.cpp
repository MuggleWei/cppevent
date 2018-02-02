#include <signal.h>
#include "protobuf_server_handler.h"

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


int main(int argc, char *argv[])
{
	cppevent::EventLoopGroup event_loop_group;

	p_event_loop_group = &event_loop_group;
	signal(SIGINT, sighandler);

	event_loop_group
		.acceptNumber(1)
		.workerNumber(4)
		.setHandler(true, ProtobufServerHandler::getHandler)
		.idleTimeout(5)
		.option(cppevent::CPPEVENT_BACKLOG, 512);

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

	return 0;
}