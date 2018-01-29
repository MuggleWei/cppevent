#include "event_loop.h"
#include <signal.h>
#include <assert.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <new>
#include <algorithm>
#include <stdexcept>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include <event2/listener.h>
#include "muggle/muggle_cc.h"
#include "tunnel_msg.h"
#include "byte_buf.h"

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

static void readcb(struct bufferevent* /*bev*/, void *ctx)
{
	ConnContainer *conn_container = (ConnContainer*)ctx;
	EventHandler *handler = conn_container->connptr->getHandler();
	handler->connRead(conn_container->connptr);
}
static void writecb(struct bufferevent* /*bev*/, void *ctx)
{
	ConnContainer *conn_container = (ConnContainer*)ctx;
	EventHandler *handler = conn_container->connptr->getHandler();
	if (handler)
	{
		handler->connWrite(conn_container->connptr);
		conn_container->connptr->afterWrite();
	}
}
static void eventcb(struct bufferevent* /*bev*/, short events, void *ctx)
{
	ConnContainer *conn_container = (ConnContainer*)ctx;
	EventHandler *handler = conn_container->connptr->getHandler();
	handler->connEvent(conn_container->connptr);

	if ((events & CPPEVENT_EOF) || (events & CPPEVENT_ERROR))
	{
		EventLoop *event_loop = conn_container->connptr->getLoop();
		event_loop->delConn(conn_container->connptr);
	}
}
static void on_accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg)
{
	EventLoop *event_loop = (EventLoop*)arg;
	event_loop->onAccept((void*)listener, fd, addr, socklen);
}

// copy from evutil_format_sockaddr_port_
static const char* get_sockaddr_port(const struct sockaddr *sa, char *out, size_t outlen)
{
	char b[128];
	const char *res = NULL;
	int port;
	if (sa->sa_family == AF_INET) {
		const struct sockaddr_in *sin = (const struct sockaddr_in*)sa;
		res = evutil_inet_ntop(AF_INET, &sin->sin_addr, b, sizeof(b));
		port = ntohs(sin->sin_port);
		if (res) {
			evutil_snprintf(out, outlen, "%s:%d", b, port);
			return out;
		}
	}
	else if (sa->sa_family == AF_INET6) {
		const struct sockaddr_in6 *sin6 = (const struct sockaddr_in6*)sa;
		res = evutil_inet_ntop(AF_INET6, &sin6->sin6_addr, b, sizeof(b));
		port = ntohs(sin6->sin6_port);
		if (res) {
			evutil_snprintf(out, outlen, "[%s]:%d", b, port);
			return out;
		}
	}

	evutil_snprintf(out, outlen, "<addr with socktype %d>",
		(int)sa->sa_family);
	return out;
}

void EventLoop::GlobalClean()
{
	libevent_global_shutdown();
}

EventLoop::EventLoop(unsigned int tunnel_buf_size)
	: base_(nullptr)
	, thread_id_(std::this_thread::get_id())
	, handler_(nullptr)
	, handler_factory_(nullptr)
	, event_tunnel_(nullptr)
	, get_accept_func_(nullptr)
{
	EventGlobalInit();

	base_ = (void*)event_base_new();
	if (base_ == nullptr)
	{
		throw std::bad_alloc();
	}

	event_tunnel_ = new EventTunnel(this, tunnel_buf_size);
	if (event_tunnel_ == nullptr)
	{
		throw std::bad_alloc();
	}
}
EventLoop::~EventLoop()
{
	if (event_tunnel_)
	{
		delete event_tunnel_;
		event_tunnel_ = nullptr;
	}

	if (base_)
	{
		event_base_free((struct event_base*)base_);
		base_ = nullptr;
	}
}

void EventLoop::run()
{
	thread_id_ = std::this_thread::get_id();

	// in default, after constructor, tunnel open input automaticly, this
	// invoke for the situation that run after stop
	event_tunnel_->open();
	event_base_loop((struct event_base*)base_, EVLOOP_NO_EXIT_ON_EMPTY);

	while (timers_.size() > 0)
	{
		Timer *p_timer = timers_.front();
		// Don't use the code below, cause stopTimer will
		// erase the timer
		// timers_.pop_front();
		stopTimer(p_timer);
	}

	while (listeners_.size() > 0)
	{
		struct evconnlistener *listener = (struct evconnlistener*)listeners_.front();
		evconnlistener_free(listener);
		listeners_.pop_front();
	}

	while (conns_.size() > 0)
	{
		auto it = conns_.begin();
		delConn(it->second);
	}
}
void EventLoop::stop()
{
	if (std::this_thread::get_id() == thread_id_)
	{
		syncStop();
	}
	else
	{
		TunnelMsgStop *msg = new TunnelMsgStop();
		tunnelWrite(msg);
	}
}

void EventLoop::setHandler(bool shared, EventHandlerFactoryFunc* func)
{
	if (shared)
	{
		handler_ = func();
	}
	else
	{
		handler_factory_ = func;
	}
}

int EventLoop::profileTunnel()
{
	cppevent::TunnelMsgProfile *msg = new cppevent::TunnelMsgProfile(0);
	auto t = std::chrono::system_clock::now().time_since_epoch();
	msg->microsec = (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t).count();
	msg->thread_id = std::this_thread::get_id();
	static thread_local int s_msg_id = 0;
	msg->message_id = s_msg_id++;
	return tunnelWrite(msg);
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
		syncStopTimer(timer);
	}
	else
	{
		TunnelMsgStopTimer *msg = new TunnelMsgStopTimer(timer);
		tunnelWrite(msg);
	}
}

std::future<int> EventLoop::bindAndListen(const char *addr, int backlog)
{
	std::promise<int> promise;
	std::future<int> future = promise.get_future();
	if (std::this_thread::get_id() == thread_id_)
	{
		int ret = syncBindAndListen(addr, backlog);
		promise.set_value(ret);
	}
	else
	{
		TunnelMsgBindAndListen *msg = new TunnelMsgBindAndListen(addr, backlog, std::move(promise));
		tunnelWrite(msg);
	}
	
	return future;
}

void EventLoop::onAccept(void * /*listener*/, cppevent_socket_t fd, struct sockaddr *addr, int socklen)
{
	EventLoop *acceptor = this;
	if (get_accept_func_)
	{
		acceptor = get_accept_func_();
	}

	if (acceptor->thread_id_ == std::this_thread::get_id())
	{
		acceptor->syncOnAccept(fd, addr, socklen);
	}
	else
	{
		TunnelMsgAcceptConn *msg = new TunnelMsgAcceptConn(fd, addr, socklen);
		tunnelWrite(msg);
	}
}

void EventLoop::setAcceptFunc(GetAcceptEventLoopFunc &func)
{
	get_accept_func_ = func;
}

std::future<std::shared_ptr<Conn>> EventLoop::addConn(const char *addr)
{
	std::promise<std::shared_ptr<Conn>> promise;
	std::future<std::shared_ptr<Conn>> future = promise.get_future();
	if (std::this_thread::get_id() == thread_id_)
	{
		std::shared_ptr<Conn> connptr = syncAddConn(addr);
		promise.set_value(connptr);
	}
	else
	{
		TunnelMsgAddConn *msg = new TunnelMsgAddConn(addr, std::move(promise));
		tunnelWrite(msg);
	}

	return future;
}

void EventLoop::delConn(std::shared_ptr<Conn> &connptr)
{
	connptr->handler_->connInactive(connptr);
	if (!connptr->shared_handler_)
	{
		delete connptr->handler_;
	}

	auto it = conns_.find(connptr->container_);
	assert(it != conns_.end());
	bufferevent_setcb((struct bufferevent*)it->second->bev_, nullptr, nullptr, nullptr, nullptr);
	delete it->first;
	conns_.erase(it);
}

void* EventLoop::getBase()
{
	return base_;
}

int EventLoop::tunnelWrite(cppevent::TunnelMsg *message)
{
	int ret = event_tunnel_->write(message);
	if (ret != 0)
	{
		delete message;
	}
	return ret;
}
void EventLoop::tunnelRead(cppevent::TunnelMsg *message)
{
	switch (message->type)
	{
	case TunnelMsgType_Profile:
	{
		std::shared_ptr<TunnelMsgProfile> msg((TunnelMsgProfile*)message);
		auto t = std::chrono::system_clock::now().time_since_epoch();
		int64_t diff = (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t).count() - msg->microsec;

		std::cout << "message(" 
			<< msg->thread_id << "," 
			<< msg->message_id << ") through tunnel use " << diff << " micro seconds" << std::endl;
	}break;
	case TunnelMsgType_Stop:
	{
		std::shared_ptr<TunnelMsgStop> msg((TunnelMsgStop*)message);
		syncStop();
	}break;
	case TunnelMsgType_Timer:
	{
		std::shared_ptr<TunnelMsgAddTimer> msg((TunnelMsgAddTimer*)message);
		Timer *timer = syncAddTimer(msg->mill_seconds, std::move(msg->fn), msg->is_once);
		msg->promise.set_value(timer);
	}break;
	case TunnelMsgType_StopTimer:
	{
		std::shared_ptr<TunnelMsgStopTimer> msg((TunnelMsgStopTimer*)message);
		syncStopTimer(msg->timer);
	}break;
	case TunnelMsgType_BindAndListen:
	{
		std::shared_ptr<TunnelMsgBindAndListen> msg((TunnelMsgBindAndListen*)message);
		int ret = syncBindAndListen(msg->addr, msg->backlog);
		msg->promise.set_value(ret);
	}break;
	case TunnelMsgType_AcceptConn:
	{
		std::shared_ptr<TunnelMsgAcceptConn> msg((TunnelMsgAcceptConn*)message);
		syncOnAccept(msg->fd, (struct sockaddr*)&msg->addr, msg->socklen);
	}break;
	case TunnelMsgType_AddConn:
	{
		std::shared_ptr<TunnelMsgAddConn> msg((TunnelMsgAddConn*)message);
		std::shared_ptr<Conn> connptr = syncAddConn(msg->addr);
		msg->promise.set_value(connptr);
	}break;
	}
}

std::future<Timer*> EventLoop::internalAddTimer(long mill_seconds, std::function<void()> &&fn, bool is_once)
{
	std::promise<Timer*> promise;
	std::future<Timer*> future = promise.get_future();
	if (std::this_thread::get_id() == thread_id_)
	{
		Timer *timer = syncAddTimer(mill_seconds, std::move(fn), is_once);
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
		tunnelWrite(msg);
	}

	return future;
}

void EventLoop::onNewConn(std::shared_ptr<Conn> &connptr)
{
	conns_[connptr->container_] = connptr;

	if (handler_)
	{
		connptr->handler_ = handler_;
		connptr->shared_handler_ = true;
	}
	else if (handler_factory_)
	{
		connptr->handler_ = handler_factory_();
		connptr->shared_handler_ = false;
	}

	if (connptr->handler_)
	{
		connptr->handler_->connActive(connptr);
	}
}

void EventLoop::syncStop()
{
	event_tunnel_->close();

	// NOTE: don't use event_base_break, if still have message in 
	// tunnel, will lead memory leak
	event_base_loopexit((struct event_base*)base_, nullptr);
}

Timer* EventLoop::syncAddTimer(long mill_seconds, std::function<void()> &&fn, bool is_once)
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
void EventLoop::syncStopTimer(Timer *timer)
{
	if (timer)
	{
		auto it = std::find(timers_.begin(), timers_.end(), timer);
		if (it != timers_.end())
		{
			timers_.erase(it);

			event_free((struct event*)timer->ev);
			delete timer;
		}
		else
		{
			// maybe want delete timer twice
			assert(false);
		}
	}
}

int EventLoop::syncBindAndListen(const char *addr, int backlog)
{
	struct sockaddr_storage ss;
	int slen = (int)sizeof(ss);
	memset(&ss, 0, sizeof(ss));

	if (evutil_parse_sockaddr_port(addr, (struct sockaddr*)&ss, &slen) < 0)
	{
		return -1;
	}

	struct evconnlistener *listener = evconnlistener_new_bind((struct event_base*)base_, on_accept, (void*)this,
		LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, backlog,
		(struct sockaddr*)&ss, slen);
	if (!listener)
	{
		return -1;
	}
	listeners_.push_back((void*)listener);

	return 0;
}

void EventLoop::syncOnAccept(cppevent_socket_t fd, struct sockaddr *addr, int /*socklen*/)
{
	Conn *new_conn = new Conn();
	new_conn->container_ = new ConnContainer;
	new_conn->container_->connptr = std::shared_ptr<Conn>(new_conn);

	struct bufferevent *bev = bufferevent_socket_new((struct event_base*)base_, fd, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, readcb, writecb, eventcb, new_conn->container_);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	new_conn->event_loop_ = this;
	new_conn->setBev((void*)bev);

	struct sockaddr_storage local_ss;
	cppevent_socklen_t local_ss_len = sizeof(local_ss);
	getsockname(fd, (struct sockaddr*)&local_ss, &local_ss_len);
	get_sockaddr_port((struct sockaddr*)&local_ss, new_conn->local_addr_, sizeof(new_conn->local_addr_));
	get_sockaddr_port(addr, new_conn->remote_addr_, sizeof(new_conn->remote_addr_));

	onNewConn(new_conn->container_->connptr);
}

std::shared_ptr<Conn> EventLoop::syncAddConn(const char *addr)
{
	struct sockaddr_storage remote_ss;
	int remote_ss_len = (int)sizeof(remote_ss);
	if (evutil_parse_sockaddr_port(addr, (struct sockaddr*)&remote_ss, &remote_ss_len) < 0)
	{
		return nullptr;
	}

	Conn *new_conn = new Conn();
	new_conn->container_ = new ConnContainer;
	new_conn->container_->connptr = std::shared_ptr<Conn>(new_conn);

	struct bufferevent *bev = bufferevent_socket_new((struct event_base*)base_, -1, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, readcb, writecb, eventcb, new_conn->container_);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	if (bufferevent_socket_connect(bev, (struct sockaddr *)&remote_ss, sizeof(remote_ss)) < 0)
	{
		delete new_conn->container_;
		return nullptr;
	}

	new_conn->event_loop_ = this;
	new_conn->setBev((void*)bev);

	struct sockaddr_storage local_ss;
	cppevent_socklen_t local_ss_len = sizeof(local_ss);
	getsockname(bufferevent_getfd(bev), (struct sockaddr*)&local_ss, &local_ss_len);
	get_sockaddr_port((struct sockaddr*)&local_ss, new_conn->local_addr_, sizeof(new_conn->local_addr_));
	get_sockaddr_port((struct sockaddr *)&remote_ss, new_conn->remote_addr_, sizeof(new_conn->remote_addr_));

	onNewConn(new_conn->container_->connptr);

	return new_conn->container_->connptr;
}

NS_CPPEVENT_END
