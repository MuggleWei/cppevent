#ifndef CPP_EVENT_EVENT_LOOP_H_
#define CPP_EVENT_EVENT_LOOP_H_

#include "cppevent/cppevent_def.h"
#include <list>
#include <thread>
#include <future>
#include "cppevent/timer.h"
#include "cppevent/event_tunnel.h"

NS_CPPEVENT_BEGIN

class EventLoop
{
public:
	cppevent_EXPORT static void GlobalClean();

public:
	cppevent_EXPORT EventLoop(unsigned int tunnel_buf_size = 0);
	cppevent_EXPORT virtual ~EventLoop();

	cppevent_EXPORT void run();
	cppevent_EXPORT void stop();

	// just for test tunnel performance 
	cppevent_EXPORT int profileTunnel();

	cppevent_EXPORT std::future<Timer*> addTimer(long mill_seconds, std::function<void()> &&fn);
	cppevent_EXPORT std::future<Timer*> addTimerOnce(long mill_seconds, std::function<void()> &&fn);

	cppevent_EXPORT void stopTimer(Timer *timer);

	cppevent_EXPORT std::future<int> bindAndListen(const char *addr, int backlog = -1);

public:
	void* getBase();

	int tunnelWrite(cppevent::TunnelMsg *message);
	void tunnelRead(cppevent::TunnelMsg *message);

private:
	void stopSync();

	std::future<Timer*> internalAddTimer(long mill_seconds, std::function<void()> &&fn, bool is_once);
	Timer* addTimerSync(long mill_seconds, std::function<void()> &&fn, bool is_once);

	void stopTimerSync(Timer *timer);

	int bindAndListenSync(const char *addr, int backlog = -1);

private:
	void *base_;
	std::thread::id thread_id_;

	EventTunnel *event_tunnel_;
	std::list<Timer*> timers_;
	std::list<void*> listeners_;
};

NS_CPPEVENT_END


#endif
