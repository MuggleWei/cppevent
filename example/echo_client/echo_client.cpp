#include <iostream>
#include <thread>
#include "cppevent/cppevent.h"
#include "echo_client_handler.h"

int GetTimeAndHelloWorld(char *p, int size)
{
	using namespace std;
	using namespace std::chrono;

	system_clock::time_point now = system_clock::now();
	system_clock::duration tp = now.time_since_epoch();

	tp -= duration_cast<seconds>(tp);
	time_t tt = system_clock::to_time_t(now);

	tm t = *localtime(&tt);
	// tm t = *gmtime(&tt);

	return snprintf(p, size, "[%04u-%02u-%02u %02u:%02u:%02u.%03u] hello world\n", t.tm_year + 1900,
		t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,
		static_cast<unsigned>(tp / milliseconds(1)));
}

int main(int argc, char *argv[])
{
	cppevent::NetService service;
	std::thread th([&service]() {
		service.Handler(new EchoClientHandler())
			.Option(cppevent::BACKLOG_NUM, 128);
		service.Run();
	});
	if (!service.InRunning())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	cppevent::Peer *peer = service.SyncAddConnectTo("127.0.0.1:10102");
	if (peer == nullptr)
	{
		exit(EXIT_FAILURE);
	}

	char buf[128];
	while (true)
	{
		int len = GetTimeAndHelloWorld(buf, sizeof(buf)-1);
		peer->Write(buf, len);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return 0;
}