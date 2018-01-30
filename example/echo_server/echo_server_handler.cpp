#include "echo_server_handler.h"

void EchoServerHandler::connActive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn active: [" << std::this_thread::get_id() << "] "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;
}
void EchoServerHandler::connInactive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn Inactive: [" << std::this_thread::get_id() << "] "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;
}

void EchoServerHandler::connRead(std::shared_ptr<cppevent::Conn> &connptr)
{
	connptr->write(connptr->getInputByteBuf());
}
void EchoServerHandler::connIdleTimeout(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "conn idle timeout: "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr()
		<< std::endl;

	const char *msg = "idle timeout";
	connptr->writeAndClose((void*)msg, strlen(msg));
}

static EchoServerHandler s_handler;
cppevent::EventHandler* EchoServerHandler::getHandler()
{
	return &s_handler;
}