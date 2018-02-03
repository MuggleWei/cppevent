#ifndef CPP_EVENT_EVENT_HANDLER_H_
#define CPP_EVENT_EVENT_HANDLER_H_

#include "cppevent/core/cppevent_def.h"
#include <memory>

NS_CPPEVENT_BEGIN

class Conn;

class EventHandler
{
public:
	virtual ~EventHandler() {}

	/*
	 * When a new socket connected, trigger this function
	 */
	cppevent_core_EXPORT virtual void connActive(std::shared_ptr<Conn>&) {}

	/*
	 * When a socket disconnected, trigger this function
	 */
	cppevent_core_EXPORT virtual void connInactive(std::shared_ptr<Conn>&) {}

	/*
	 * Have data in conn's input byte buffer, trigger this function
	 */
	cppevent_core_EXPORT virtual void connRead(std::shared_ptr<Conn>&) {}

	/*
	 * After data write into socket buffer, trigger this function
	 */
	cppevent_core_EXPORT virtual void connWrite(std::shared_ptr<Conn>&) {}

	/*
	 * Some event happened
	 */
	cppevent_core_EXPORT virtual void connEvent(std::shared_ptr<Conn>&) {}

	/*
	 * Did not receive the message within n seconds, n is set by idleTimeout
	 */
	cppevent_core_EXPORT virtual void connIdleTimeout(std::shared_ptr<Conn>&) {}
};

typedef EventHandler* EventHandlerFactoryFunc();

NS_CPPEVENT_END

#endif