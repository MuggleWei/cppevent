#ifndef CPP_EVENT_TIMER_H_
#define CPP_EVENT_TIMER_H_

#include "cppevent/core/cppevent_def.h"
#include <functional>

NS_CPPEVENT_BEGIN

struct Timer
{
	void *ev;
	std::function<void()> fn;
};

NS_CPPEVENT_END


#endif