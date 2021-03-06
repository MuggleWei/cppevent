#ifndef CPP_EVENT_EVENT_LOOP_H_
#define CPP_EVENT_EVENT_LOOP_H_

#include "cppevent/core/cppevent_def.h"
#include <list>
#include <map>
#include <thread>
#include <future>
#include <functional>
#include "cppevent/core/timer.h"
#include "cppevent/core/event_tunnel.h"
#include "cppevent/core/conn.h"
#include "cppevent/core/event_handler.h"


NS_CPPEVENT_BEGIN

class EventLoop;

typedef std::function<EventLoop*(void)> GetAcceptEventLoopFunc;

enum eEventLoopFlag
{
	CPPEVENT_FLAG_REUSEADDR = 0x01,
	CPPEVENT_FLAG_REUSEPORT = 0x02,
};

class EventLoop
{
public:
	cppevent_core_EXPORT static void GlobalClean();

public:
	cppevent_core_EXPORT EventLoop(unsigned int tunnel_buf_size = 0);
	cppevent_core_EXPORT virtual ~EventLoop();

	cppevent_core_EXPORT void run();
	cppevent_core_EXPORT void stop();

	cppevent_core_EXPORT void setHandler(bool shared, EventHandlerFactoryFunc* func);

	cppevent_core_EXPORT void setFlag(int flag);

	cppevent_core_EXPORT void setIdleTimeout(long second);

	// just for test tunnel performance 
	cppevent_core_EXPORT int profileTunnel();

	cppevent_core_EXPORT std::future<Timer*> addTimer(long mill_seconds, std::function<void()> &&fn);
	cppevent_core_EXPORT std::future<Timer*> addTimerOnce(long mill_seconds, std::function<void()> &&fn);

	cppevent_core_EXPORT void stopTimer(Timer *timer);

	cppevent_core_EXPORT std::future<int> bindAndListen(const char *addr, int backlog = -1);

	cppevent_core_EXPORT void onAccept(void *listener, cppevent_socket_t fd, struct sockaddr *addr, int socklen);
	cppevent_core_EXPORT void setAcceptFunc(GetAcceptEventLoopFunc &func);

	cppevent_core_EXPORT std::future<std::shared_ptr<Conn>> addConn(const char *addr);

	cppevent_core_EXPORT void delConn(std::shared_ptr<Conn> &connptr);

public:
	void* getBase();

	int tunnelWrite(cppevent::TunnelMsg *message);
	void tunnelRead(cppevent::TunnelMsg *message);

	void idleCheck();

private:
	int getListenFlag();

	std::future<Timer*> internalAddTimer(long mill_seconds, std::function<void()> &&fn, bool is_once);

	void onNewConn(std::shared_ptr<Conn> &connptr);

	void syncStop();
	Timer* syncAddTimer(long mill_seconds, std::function<void()> &&fn, bool is_once);
	void syncStopTimer(Timer *timer);
	int syncBindAndListen(const char *addr, int backlog = -1);
	void syncOnAccept(cppevent_socket_t fd, struct sockaddr *addr, int socklen);
	std::shared_ptr<Conn> syncAddConn(const char *addr);

private:
	void *base_;
	std::thread::id thread_id_;

	EventHandler *handler_;
	EventHandlerFactoryFunc *handler_factory_;

	EventTunnel *event_tunnel_;
	std::list<Timer*> timers_;
	std::list<void*> listeners_;
	std::map<ConnContainer*, std::shared_ptr<Conn>> conns_;

	GetAcceptEventLoopFunc get_accept_func_;

	long idle_second_;

	int flag_;
};

NS_CPPEVENT_END


#endif
