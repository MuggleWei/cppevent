#include <iostream>
#include <thread>
#include "cppevent/cppevent.h"
#include "echo_server_handler.h"

int main(int argc, char *argv[])
{
	cppevent::NetService service;
	service.Bind("0.0.0.0:10102")
		.WorkerThread(8)
		.Handler(new EchoServerHandler())
		.Option(cppevent::BACKLOG_NUM, 128);
	service.Run();

	return 0;
}