#ifndef MY_HANDLER_H_
#define MY_HANDLER_H_

#include "cppevent/cppevent.h"

class MyHandler : public cppevent::EventHandler
{
public:
	virtual void connActive(std::shared_ptr<cppevent::Conn> &connptr) override;
	virtual void connInactive(std::shared_ptr<cppevent::Conn> &connptr) override;
	virtual void connRead(std::shared_ptr<cppevent::Conn> &connptr) override;
	virtual void connWrite(std::shared_ptr<cppevent::Conn> &connptr) override;
	virtual void connEvent(std::shared_ptr<cppevent::Conn> &connptr) override;

	static cppevent::EventHandler* getMyHandler();
};

#endif