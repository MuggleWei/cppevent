#include "worker_thread.h"
#include <assert.h>
#include <mutex>
#include <condition_variable>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include "cppevent/net_service.h"

NS_CPPEVENT_BEGIN

enum eTunnelDirection
{
	TUNNEL_IN = 0,
	TUNNEL_OUT = 1
};
enum eTunnelInputType
{
	TUNNEL_ADD_SOCKET = 0
};

struct TunnelInputArgs_AddSocket
{
	evutil_socket_t fd;
	struct sockaddr_storage remote_ss;
	socklen_t remote_ss_len;
	FuncConnectCallback cb;
};

struct TunnelInput_AddSocket
{
	eTunnelInputType type;
	TunnelInputArgs_AddSocket *args;
};

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

static int tunnelcb_addsocktask(struct evbuffer *input, WorkerThread *worker_thread)
{
	struct TunnelInput_AddSocket arg;
	if (evbuffer_get_length(input) >= sizeof(arg))
	{
		evbuffer_remove(input, &arg, sizeof(arg));
		worker_thread->AddSocket(arg.args->fd, &arg.args->remote_ss, arg.args->remote_ss_len, arg.args->cb);
	}
	else
	{
		return -1;
	}

	delete arg.args;

	return 0;
}
static void tunnelcb(struct bufferevent *bev, void *ctx)
{
	WorkerThread *worker_thread = (WorkerThread*)ctx;
	struct evbuffer *input = bufferevent_get_input(bev);
	while (evbuffer_get_length(input) >= sizeof(eTunnelInputType))
	{
		eTunnelInputType task_type;
		if (evbuffer_copyout(input, &task_type, sizeof(eTunnelInputType)) < 0)
		{
			return;
		}

		int ret = 0;
		switch (task_type)
		{
		case TUNNEL_ADD_SOCKET:
		{
			ret = tunnelcb_addsocktask(input, worker_thread);
		}break;
		}

		if (ret != 0)
		{
			return;
		}
	}
}

static void do_accept(evutil_socket_t listener, short events, void *arg)
{
	struct sockaddr_storage ss;
	socklen_t slen = sizeof(ss);
	evutil_socket_t fd = accept(listener, (struct sockaddr*)&ss, &slen);
	if (fd < 0)
	{
		assert(fd >= 0);
		fprintf(stderr, "Failed accept socket\n");
		return;
	}
	else
	{
		FuncAcceptCb &on_accept = *(FuncAcceptCb*)arg;
		on_accept(fd, &ss, slen);
	}
}
static void readcb(struct bufferevent *bev, void *ctx)
{
	Peer *peer = (Peer*)ctx;
	WorkerThread *worker_thread = peer->getOwnerThread();
	worker_thread->OnRead(peer);
}
static void writecb(struct bufferevent *bev, void *ctx)
{
	Peer *peer = (Peer*)ctx;
	WorkerThread *worker_thread = peer->getOwnerThread();
	worker_thread->OnWrite(peer);
}
static void eventcb(struct bufferevent *bev, short events, void *ctx)
{
	Peer *peer = (Peer*)ctx;
	WorkerThread *worker_thread = peer->getOwnerThread();
	worker_thread->OnEvent(peer, events);
}

WorkerThread* WorkerThread::Create(NetService *service, int id, bool run)
{
	std::mutex mtx;
	std::condition_variable cv;

	WorkerThread *thread = new WorkerThread(service, id);
	if (thread)
	{

		std::unique_lock<std::mutex> lock(mtx);
		std::thread th([&thread, &cv, run]() {
			struct event_base *base = event_base_new();
			if (base)
			{
				struct bufferevent *tunnel[2];
				if (bufferevent_pair_new(base, BEV_OPT_CLOSE_ON_FREE, tunnel) == 0)
				{
					bufferevent_setcb(tunnel[TUNNEL_OUT], tunnelcb, NULL, NULL, (void*)thread);
					bufferevent_enable(tunnel[TUNNEL_OUT], EV_READ);
					bufferevent_enable(tunnel[TUNNEL_IN], EV_WRITE);

					thread->tunnel_[TUNNEL_IN] = tunnel[TUNNEL_IN];
					thread->tunnel_[TUNNEL_OUT] = tunnel[TUNNEL_OUT];
				}
				thread->base_ = base;

				cv.notify_all();

				if (run)
				{
					thread->system_thread_id_ = std::this_thread::get_id();
					event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);
				}
			}
		});
		th.detach();
		cv.wait(lock);
	}
	else
	{
		fprintf(stderr, "Failed create worker thread!\n");
		assert(thread != nullptr);
	}

	return thread;
}

WorkerThread::WorkerThread(NetService *service, int id)
	: owner_(service)
	, worker_id_(id)
	, base_(nullptr)
	, tunnel_{nullptr}
{}
WorkerThread::~WorkerThread()
{}

void WorkerThread::Run()
{
	assert(owner_ != nullptr);
	assert(base_ != nullptr);
	assert(tunnel_[TUNNEL_IN] != nullptr && tunnel_[TUNNEL_OUT] != nullptr);

	if (owner_ == nullptr ||
		base_ == nullptr ||
		tunnel_[TUNNEL_IN] == nullptr ||
		tunnel_[TUNNEL_OUT] == nullptr)
	{
		return;
	}

	system_thread_id_ = std::this_thread::get_id();
	event_base_loop((struct event_base*)base_, EVLOOP_NO_EXIT_ON_EMPTY);
}

int WorkerThread::BindAndListen(const char *addr_ip_port, int backlog, FuncAcceptCb *accept_cb)
{
	assert(base_ != nullptr);
	if (base_ == nullptr)
	{
		return -1;
	}

	evutil_socket_t listener;
	struct sockaddr* sa = nullptr;
	struct sockaddr_storage ss;
	int slen = (int)sizeof(ss);
	memset(&ss, 0, sizeof(ss));

	if (evutil_parse_sockaddr_port(addr_ip_port, (struct sockaddr*)&ss, &slen) < 0)
	{
		return -1;
	}
	sa = (struct sockaddr*)&ss;

	listener = socket(sa->sa_family, SOCK_STREAM, 0);

	return BindListen(listener, sa, slen, backlog, accept_cb);
}
int WorkerThread::BindAndListen(int port, int backlog, FuncAcceptCb *accept_cb)
{
	assert(base_ != nullptr);
	if (base_ == nullptr)
	{
		return -1;
	}

	evutil_socket_t listener;
	struct sockaddr* sa = nullptr;
	struct sockaddr_in sin;
	int slen = 0;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(port);
	sa = (struct sockaddr*)&sin;
	slen = (int)sizeof(sin);

	listener = socket(sin.sin_family, SOCK_STREAM, 0);
	
	return BindListen(listener, sa, slen, backlog, accept_cb);
}

int WorkerThread::AddSocket(cppevent_socket_t fd, struct sockaddr_storage *ss, cppevent_socklen_t slen, FuncConnectCallback cb)
{
	if (std::this_thread::get_id() == system_thread_id_)
	{
		Peer *peer = AddSocketImmediately(fd, ss, slen);
		if (cb)
		{
			cb(peer);
		}

		if (peer == nullptr)
		{
			return -1;
		}
		return 0;
	}
	else
	{
		TunnelInputArgs_AddSocket *args = new TunnelInputArgs_AddSocket{ fd, *ss, slen, cb };
		TunnelInput_AddSocket arg = { TUNNEL_ADD_SOCKET, args };
		bufferevent_write((struct bufferevent*)tunnel_[TUNNEL_IN], (void*)&arg, sizeof(arg));
		return 1;
	}
}

size_t WorkerThread::getPeerCount()
{
	return peers_.size();
}

void WorkerThread::OnRead(Peer *peer)
{
	if (!owner_)
	{
		return;
	}

	auto handler = owner_->getHandler();
	if (handler)
	{
		handler->OnRead(peer);
	}

	if (peer->flag_ & PEER_FLAG_CLOSE)
	{
		FreePeer(peer);
	}
}
void WorkerThread::OnWrite(Peer *peer)
{
	if (!owner_)
	{
		return;
	}

	auto handler = owner_->getHandler();
	if (handler)
	{
		handler->OnWrite(peer);
	}

	if (peer->flag_ & PEER_FLAG_CLOSE)
	{
		FreePeer(peer);
	}
}
void WorkerThread::OnEvent(Peer *peer, short events)
{
	if (!owner_)
	{
		return;
	}

	auto handler = owner_->getHandler();
	if (handler)
	{
		handler->OnEvent(peer, events);
	}

	if (events & CPPEVENT_ERROR)
	{
		if (!(peer->flag_ & PEER_FLAG_DONT_ERASE))
		{
			peer->flag_ |= PEER_FLAG_CLOSE;
		}
	}

	if (peer->flag_ & PEER_FLAG_CLOSE)
	{
		FreePeer(peer);
	}
}

int WorkerThread::BindListen(cppevent_socket_t listener, struct sockaddr* sa, int slen, int backlog, FuncAcceptCb *accept_cb)
{
	evutil_make_socket_nonblocking(listener);
#ifndef WIN32
	{
		int one = 1;
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	}
#endif

	if (bind(listener, sa, slen) < 0)
	{
		evutil_closesocket(listener);
		assert(false);
		return 1;
	}

	if (listen(listener, backlog) < 0)
	{
		evutil_closesocket(listener);
		assert(false);
		return 2;
	}

	struct event *listener_event = event_new(
		(struct event_base*)base_, listener,
		EV_READ | EV_PERSIST, do_accept, (void*)accept_cb);
	if (listener_event == nullptr)
	{
		evutil_closesocket(listener);
		assert(false);
		return 3;
	}

	if (event_add(listener_event, NULL) != 0)
	{
		evutil_closesocket(listener);
		event_del(listener_event);
		assert(false);
		return 4;
	}

	return 0;
}

Peer* WorkerThread::AddSocketImmediately(cppevent_socket_t fd, struct sockaddr_storage *ss, cppevent_socklen_t slen)
{
	Peer *peer = new Peer();
	if (peer == nullptr)
	{
		return nullptr;
	}

	evutil_make_socket_nonblocking(fd);
	struct bufferevent *bev = bufferevent_socket_new((struct event_base*)base_, fd, BEV_OPT_CLOSE_ON_FREE);
	if (bev == NULL)
	{
		bufferevent_free(bev);
		return nullptr;
	}
	bufferevent_setcb(bev, readcb, writecb, eventcb, (void*)peer);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	// fill out base info
	peer->owner_thread_ = this;
	peer->setBev((void*)bev);

	struct sockaddr_storage local_ss;
	int local_ss_len = sizeof(local_ss);
	getsockname(fd, (struct sockaddr*)&local_ss, &local_ss_len);
	get_sockaddr_port((struct sockaddr*)&local_ss, peer->local_addr_, sizeof(peer->local_addr_));
	get_sockaddr_port((struct sockaddr*)ss, peer->remote_addr_, sizeof(peer->remote_addr_));
	peer->remote_ss_ = *ss;
	peer->remote_ss_len_ = slen;
	event_base_gettimeofday_cached((struct event_base*)base_, &peer->first_connect_time_);

	peers_.insert(peer);

	auto handler = owner_->getHandler();
	if (handler)
	{
		handler->OnConnected(peer);
	}

	return peer;
}

void WorkerThread::FreePeer(Peer *peer)
{
	auto it = peers_.find(peer);
	assert(it != peers_.end());
	if (it != peers_.end())
	{
		peers_.erase(it);
	}

	delete peer;
}

NS_CPPEVENT_END