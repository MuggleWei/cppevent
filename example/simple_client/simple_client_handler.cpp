#include "simple_client_handler.h"
#include <iostream>

void SimpleClientHandler::connActive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn active: "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;

	auto future = connptr->getLoop()->addTimer(1000, [this, connptr]() mutable {
		char buf[] = "hello";
		connptr->getOutputByteBuf().Write(buf, sizeof(buf));
	});

	timer_ = future.get();
	receive_msg_cnt_ = 0;
}
void SimpleClientHandler::connInactive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn inactive: "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;

	connptr->getLoop()->stop();
}
void SimpleClientHandler::connRead(std::shared_ptr<cppevent::Conn> &connptr)
{
	char buf[1025];
	while (true)
	{
		memset(buf, 0, sizeof(buf));
		size_t len = connptr->getInputByteBuf().GetByteLength();
		if (len == 0)
		{
			return;
		}

		len = len > 1024 ? 1024 : len;
		size_t read_bytes = connptr->getInputByteBuf().ReadBytes(buf, len);
		if (read_bytes == -1)
		{
			std::cerr << "error read!" << std::endl;
			return;
		}
		std::cout << "receive message: " << buf << std::endl;

		if (receive_msg_cnt_++ > 3 && timer_)
		{
			std::cout << "stop send message..." << std::endl;
			connptr->getLoop()->stopTimer(timer_);
			timer_ = nullptr;
		}
	}
}

cppevent::EventHandler* SimpleClientHandler::newClientHandler()
{
	return new SimpleClientHandler();
}