#ifndef CPP_EVENT_EVENT_LOOP_H_
#define CPP_EVENT_EVENT_LOOP_H_

#include "cppevent/cppevent_def.h"
#include <list>
#include <map>
#include <thread>
#include <future>
#include <functional>
#include "cppevent/timer.h"
#include "cppevent/event_tunnel.h"
#include "cppevent/conn.h"
#include "cppevent/event_handler.h"


NS_CPPEVENT_BEGIN

class EventLoop;

typedef std::function<EventLoop*()> GetAcceptEventLoopFunc;

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

	cppevent_EXPORT void onAccept(void *listener, cppevent_socket_t fd, struct sockaddr *addr, int socklen);
	cppevent_EXPORT void setAcceptFunc(GetAcceptEventLoopFunc &func);

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

	void onAcceptSync(cppevent_socket_t fd, struct sockaddr *addr, int socklen);

private:
	void *base_;
	std::thread::id thread_id_;

	EventTunnel *event_tunnel_;
	std::list<Timer*> timers_;
	std::list<void*> listeners_;
	std::map<ConnContainer*, std::shared_ptr<Conn>> conns_;

	GetAcceptEventLoopFunc get_accept_func_;
};

NS_CPPEVENT_END


#endif
