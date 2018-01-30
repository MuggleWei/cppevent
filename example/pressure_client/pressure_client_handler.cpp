#include "pressure_client_handler.h"
#include <chrono>

struct PressureMessage
{
	char buf[128];
	int64_t t_since_epoch;
};

std::vector<int64_t> PressureClientHandler::elapseds;

static PressureClientHandler s_handler;
cppevent::EventHandler* PressureClientHandler::getHandler()
{
	return &s_handler;
}

void PressureClientHandler::connActive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn active: "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;

	connptr->getLoop()->addTimer(10, [this, connptr] () mutable {
		writeMessage(connptr);
	});
}
void PressureClientHandler::connInactive(std::shared_ptr<cppevent::Conn> &connptr)
{
	connptr->getLoop()->stop();
}
void PressureClientHandler::connRead(std::shared_ptr<cppevent::Conn> &connptr)
{
	while (connptr->getReadableLength() >= sizeof(PressureMessage))
	{
		PressureMessage msg;
		connptr->readBytes(&msg, sizeof(msg));

		auto t = std::chrono::high_resolution_clock::now();
		int64_t cur_t = (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count();
		int64_t elapsed = cur_t - msg.t_since_epoch;

		elapseds.push_back(elapsed);

		size_t n = 100;
		if (elapseds.size() > n)
		{
			int64_t total_ms = 0;
			int64_t max_ms = 0;
			for (auto &t : elapseds)
			{
				if (t > max_ms)
				{
					max_ms = t;
				}
				total_ms += t;
			}
			int64_t avg = total_ms / elapseds.size();
			std::cout << "recently " << n << " messages avg elapsed: " << avg << " micro seconds" << std::endl;
			std::cout << "recently " << n << " messages max elapsed: " << max_ms << " micro seconds" << std::endl;
			elapseds.clear();
		}
	}
}

void PressureClientHandler::writeMessage(std::shared_ptr<cppevent::Conn> &connptr)
{
	PressureMessage msg;

	auto t = std::chrono::high_resolution_clock::now();
	msg.t_since_epoch = (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count();
	strncpy(msg.buf, connptr->getLocalAddr(), sizeof(msg.buf));

	connptr->write(&msg, sizeof(msg));
}