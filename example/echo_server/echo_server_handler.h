#ifndef ECHO_SERVER_HANDLER_H_
#define ECHO_SERVER_HANDLER_H_

#include "cppevent/cppevent.h"

class EchoServerHandler : public cppevent::EventHandler
{
public:
	virtual void connActive(std::shared_ptr<cppevent::Conn> &connptr) override;
	virtual void connInactive(std::shared_ptr<cppevent::Conn> &connptr) override;
	virtual void connRead(std::shared_ptr<cppevent::Conn> &connptr) override;
	virtual void connIdleTimeout(std::shared_ptr<cppevent::Conn> &connptr) override;

	static cppevent::EventHandler* getHandler();
};

#endif