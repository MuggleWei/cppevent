#include "simple_client_handler.h"
#include <iostream>

void SimpleClientHandler::connActive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn active: "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;

	connptr->getLoop()->addTimer(1000, [this, connptr]() mutable {
		char buf[] = "hello";
		connptr->getOutputByteBuf().Write(buf, sizeof(buf));
	});
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
			fprintf(stderr, "error read!\n");
			return;
		}
		fprintf(stdout, "receieve message: %s\n", buf);
	}
}

cppevent::EventHandler* SimpleClientHandler::newClientHandler()
{
	return new SimpleClientHandler();
}