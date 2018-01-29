#ifndef SIMPLE_CLIENT_HANDLER_H_
#define SIMPLE_CLIENT_HANDLER_H_

#include "cppevent/cppevent.h"

class SimpleClientHandler : public cppevent::EventHandler
{
public:
	virtual void connActive(std::shared_ptr<cppevent::Conn> &connptr) override;
	virtual void connInactive(std::shared_ptr<cppevent::Conn> &connptr) override;
	virtual void connRead(std::shared_ptr<cppevent::Conn> &connptr) override;

	static cppevent::EventHandler* newClientHandler();

private:
	cppevent::Timer *timer_;
	int receive_msg_cnt_;
};

#endif