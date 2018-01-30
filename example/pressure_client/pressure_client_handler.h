#ifndef PRESSURE_CLIENT_HANDLER_H_
#define PRESSURE_CLIENT_HANDLER_H_

#include "cppevent/cppevent.h"
#include <vector>

class PressureClientHandler : public cppevent::EventHandler
{
public:
	static cppevent::EventHandler* getHandler();

	virtual void connActive(std::shared_ptr<cppevent::Conn> &connptr) override;
	virtual void connInactive(std::shared_ptr<cppevent::Conn> &connptr) override;
	virtual void connRead(std::shared_ptr<cppevent::Conn> &connptr) override;

	void writeMessage(std::shared_ptr<cppevent::Conn> &connptr);

private:
	static std::vector<int64_t> elapseds;
};

#endif