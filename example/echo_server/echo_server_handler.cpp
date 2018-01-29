#include "echo_server_handler.h"

void EchoServerHandler::connActive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn active: "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;
}
void EchoServerHandler::connInactive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn Inactive: "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;
}
void EchoServerHandler::connRead(std::shared_ptr<cppevent::Conn> &connptr)
{
	// connptr->write(connptr->getInputByteBuf());
	connptr->writeAndClose(connptr->getInputByteBuf());
}

static EchoServerHandler s_handler;
cppevent::EventHandler* EchoServerHandler::getHandler()
{
	return &s_handler;
}