#include "event_loop.h"
#include <signal.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <new>
#include <stdexcept>
#include <event2/event.h>
#include <event2/thread.h>
#include "muggle/muggle_cc.h"
#include "tunnel_msg.h"

NS_CPPEVENT_BEGIN

static std::mutex s_event_global_mtx;
static bool s_event_global_already_init = false;
static void EventGlobalInit()
{
	std::lock_guard<std::mutex> lock(s_event_global_mtx);

	if (s_event_global_already_init)
	{
		return;
	}

	char err_buf[128];
	int err = 0;

#if WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		snprintf(err_buf, sizeof(err_buf), "WSAStartup failed with error: %d\n", err);
		throw std::runtime_error(err_buf);
	}

	err = evthread_use_windows_threads();
	if (err != 0)
	{
		snprintf(err_buf, sizeof(err_buf), "evthread failed use windows threads: %d\n", err);
		throw std::runtime_error(err_buf);
	}
#else
	signal(SIGPIPE, SIG_IGN);

	err = evthread_use_pthreads();
	if (err != 0)
	{
		snprintf(err_buf, sizeof(err_buf), "evthread failed use pthreads: %d\n", err);
		throw std::runtime_error(err_buf);
	}
#endif

	s_event_global_already_init = true;
}

static void timerCallback(evutil_socket_t fd, short events, void *args)
{
	if (fd == -1 && events == EV_TIMEOUT)
	{
		Timer *timer = (Timer*)args;
		timer->fn();
	}
}

EventLoop::EventLoop()
	: base_(nullptr)
	, thread_id_(std::this_thread::get_id())
	, tunnel_(nullptr)
{
	EventGlobalInit();

	base_ = (void*)event_base_new();
	if (base_ == nullptr)
	{
		throw std::bad_alloc();
	}

	tunnel_ = new Tunnel(this);
	if (tunnel_ == nullptr)
	{
		throw std::bad_alloc();
	}
}
EventLoop::~EventLoop()
{
	if (base_)
	{
		event_base_free((struct event_base*)base_);
		base_ = nullptr;
	}
}

void EventLoop::run()
{
	thread_id_ = std::this_thread::get_id();
	event_base_loop((struct event_base*)base_, EVLOOP_NO_EXIT_ON_EMPTY);

	clean();
}
void EventLoop::stop()
{
	if (std::this_thread::get_id() == thread_id_)
	{
		stopSync();
	}
	else
	{
		TunnelMsgStop *msg = new TunnelMsgStop();
		tunnelWrite((void*)msg);
	}
}

void EventLoop::profileTunnel()
{
	cppevent::TunnelMsgProfile *msg = new cppevent::TunnelMsgProfile(0);
	auto t = std::chrono::system_clock::now().time_since_epoch();
	msg->microsec = (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t).count();
	this->tunnelWrite((void*)msg);
}

std::future<Timer*> EventLoop::addTimer(long mill_seconds, std::function<void()> &&fn)
{
	return internalAddTimer(mill_seconds, std::move(fn), false);
}
std::future<Timer*> EventLoop::addTimerOnce(long mill_seconds, std::function<void()> &&fn)
{
	return internalAddTimer(mill_seconds, std::move(fn), true);
}

void EventLoop::stopTimer(Timer *timer)
{
	if (std::this_thread::get_id() == thread_id_)
	{
		stopTimerSync(timer);
	}
	else
	{
		TunnelMsgStopTimer *msg = new TunnelMsgStopTimer(timer);
		tunnelWrite((void*)msg);
	}
}

void* EventLoop::getBase()
{
	return base_;
}

void EventLoop::tunnelWrite(void *arg)
{
	tunnel_->write(arg);
}
void EventLoop::tunnelRead(void *arg)
{
	TunnelMsg *message = (TunnelMsg*)arg;
	switch (message->type)
	{
	case TunnelMsgType_Profile:
	{
		std::shared_ptr<TunnelMsgProfile> msg((TunnelMsgProfile*)message);
		auto t = std::chrono::system_clock::now().time_since_epoch();
		int64_t diff = (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t).count() - msg->microsec;
		std::cout << "use " << diff << " micro seconds" << std::endl;
	}break;
	case TunnelMsgType_Stop:
	{
		std::shared_ptr<TunnelMsgStop> msg((TunnelMsgStop*)message);
		stopSync();
	}break;
	case TunnelMsgType_Timer:
	{
		std::shared_ptr<TunnelMsgAddTimer> msg((TunnelMsgAddTimer*)message);
		Timer *timer = addTimerSync(msg->mill_seconds, std::move(msg->fn), msg->is_once);
		msg->promise.set_value(timer);
	}break;
	case TunnelMsgType_StopTimer:
	{
		std::shared_ptr<TunnelMsgStopTimer> msg((TunnelMsgStopTimer*)message);
		stopTimerSync(msg->timer);
	}break;
	}
}

void EventLoop::clean()
{
	if (tunnel_)
	{
		delete tunnel_;
		tunnel_ = nullptr;
	}

	for (auto &p_timer : timers_)
	{
		stopTimer(p_timer);
	}
	timers_.clear();
}

void EventLoop::stopSync()
{
	event_base_loopbreak((struct event_base*)base_);
}

std::future<Timer*> EventLoop::internalAddTimer(long mill_seconds, std::function<void()> &&fn, bool is_once)
{
	std::promise<Timer*> promise;
	std::future<Timer*> future = promise.get_future();
	if (std::this_thread::get_id() == thread_id_)
	{
		Timer *timer = addTimerSync(mill_seconds, std::move(fn), is_once);
		promise.set_value(timer);
	}
	else
	{
		TunnelMsgAddTimer *msg = new TunnelMsgAddTimer(
			mill_seconds,
			std::move(fn),
			is_once,
			std::move(promise)
		);
		tunnelWrite((void*)msg);
	}

	return future;
}
Timer* EventLoop::addTimerSync(long mill_seconds, std::function<void()> &&fn, bool is_once)
{
	short events = is_once ? EV_TIMEOUT : EV_TIMEOUT | EV_PERSIST;

	struct timeval tv = { 0, mill_seconds * 1000 };
	Timer *timer = new Timer;
	timer->fn = std::move(fn);
	timer->ev = (void*)event_new((struct event_base*)base_, -1, events, timerCallback, timer);
	if (timer->ev)
	{
		event_add((struct event*)timer->ev, &tv);
	}

	timers_.push_back(timer);

	return timer;
}
void EventLoop::stopTimerSync(Timer *timer)
{
	if (timer)
	{
		event_free((struct event*)timer->ev);
		delete timer;
	}
}

NS_CPPEVENT_END