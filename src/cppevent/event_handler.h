#ifndef CPP_EVENT_EVENT_HANDLER_H_
#define CPP_EVENT_EVENT_HANDLER_H_

#include "cppevent/cppevent_def.h"
#include <memory>

NS_CPPEVENT_BEGIN

class Conn;

class EventHandler
{
public:
	virtual ~EventHandler() {}

	cppevent_EXPORT virtual void connActive(std::shared_ptr<Conn>&) {}
	cppevent_EXPORT virtual void connInactive(std::shared_ptr<Conn>&) {}

	cppevent_EXPORT virtual void connRead(std::shared_ptr<Conn>&) {}
	cppevent_EXPORT virtual void connWrite(std::shared_ptr<Conn>&) {}

	cppevent_EXPORT virtual void connEvent(std::shared_ptr<Conn>&) {}

	cppevent_EXPORT virtual void connIdleTimeout(std::shared_ptr<Conn>&) {}
};

typedef EventHandler* EventHandlerFactoryFunc();

NS_CPPEVENT_END

#endif