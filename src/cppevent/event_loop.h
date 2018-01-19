#ifndef CPP_EVENT_EVENT_LOOP_H_
#define CPP_EVENT_EVENT_LOOP_H_

#include "cppevent/cppevent_def.h"
#include <vector>
#include <thread>
#include <future>
#include "cppevent/tunnel.h"
#include "cppevent/timer.h"

NS_CPPEVENT_BEGIN

class EventLoop
{
public:
	cppevent_EXPORT EventLoop();
	cppevent_EXPORT virtual ~EventLoop();

	cppevent_EXPORT void run();
	cppevent_EXPORT void stop();

	// just for test tunnel profile
	cppevent_EXPORT void profileTunnel();

	cppevent_EXPORT std::future<Timer*> addTimer(long mill_seconds, std::function<void()> &&fn);
	cppevent_EXPORT std::future<Timer*> addTimerOnce(long mill_seconds, std::function<void()> &&fn);

	cppevent_EXPORT void stopTimer(Timer *timer);

public:
	void* getBase();

	void tunnelWrite(void *arg);
	void tunnelRead(void *arg);

private:
	void clean();

	void stopSync();

	std::future<Timer*> internalAddTimer(long mill_seconds, std::function<void()> &&fn, bool is_once);
	Timer* addTimerSync(long mill_seconds, std::function<void()> &&fn, bool is_once);

	void stopTimerSync(Timer *timer);

private:
	void *base_;
	std::thread::id thread_id_;

	Tunnel *tunnel_;
	std::vector<Timer*> timers_;
};

NS_CPPEVENT_END


#endif