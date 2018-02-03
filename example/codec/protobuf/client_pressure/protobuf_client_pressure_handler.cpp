#include "protobuf_client_pressure_handler.h"
#include <chrono>

ProtobufClientPressureHandler::ProtobufClientPressureHandler()
	: cppevent::ProtobufHandler(102400, 128)
{
	handleFunc<TimeRecord>(CPPEVENT_PROTOBUF_HANDLE_MEMFUNC(&ProtobufClientPressureHandler::OnTimeRecord, this));
}

void ProtobufClientPressureHandler::connActive(ConnPtr &connptr)
{
	std::cout << std::endl << "========" << std::endl;
	std::cout << "connected: "
		<< connptr->getLocalAddr() << " <-->" << connptr->getRemoteAddr()
		<< std::endl;

	connptr->getLoop()->addTimer(10, [this, connptr]() mutable {
		auto t = std::chrono::high_resolution_clock::now();
		int64_t cur_microsecond = (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count();

		TimeRecord tr;
		tr.add_time_records(cur_microsecond);
		write(connptr, &tr);
	});
}
void ProtobufClientPressureHandler::connInactive(ConnPtr &connptr)
{
	std::cout << std::endl << "========" << std::endl;
	std::cout << "disconnected: "
		<< connptr->getLocalAddr() << " <-->" << connptr->getRemoteAddr()
		<< std::endl;

	connptr->getLoop()->stop();
}
void ProtobufClientPressureHandler::OnTimeRecord(ConnPtr &connptr, std::shared_ptr<TimeRecord> &msg)
{
	auto t = std::chrono::high_resolution_clock::now();
	int64_t cur_microsecond = (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count();

	if (msg->time_records_size() > 0)
	{
		int64_t elapsed = cur_microsecond - msg->time_records().Get(0);
		elapseds.push_back(elapsed);
	}

	size_t n = 100;
	if (elapseds.size() >= n)
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