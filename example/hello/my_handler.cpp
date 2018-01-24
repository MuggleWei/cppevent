#include "my_handler.h"

void MyHandler::connActive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn active: "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;
}

void MyHandler::connInactive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn Inactive: "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;
}
void MyHandler::connRead(std::shared_ptr<cppevent::Conn> &connptr)
{
	char buf[1024];
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
			std::cerr << "error read" << std::endl;
			return;
		}
		std::cout << "["
			<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
			<< "]conn read: " << buf << std::endl;
	}
}
void MyHandler::connWrite(std::shared_ptr<cppevent::Conn> &connptr)
{}
void MyHandler::connEvent(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn Event: "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;
}

static MyHandler s_handler;
cppevent::EventHandler* MyHandler::getMyHandler()
{
	return &s_handler;
}