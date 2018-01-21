#include "event_tunnel.h"
#include <queue>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include "muggle/muggle_cc.h"
#include "event_loop.h"

NS_CPPEVENT_BEGIN

typedef muggle::Tunnel<cppevent::TunnelMsg*> EventTunnelType;

static void tunnelcb(evutil_socket_t /*sock*/, short /*which*/, void *arg)
{
	EventTunnel *tunnel = (EventTunnel*)arg;
	tunnel->onRead();
}

EventTunnel::EventTunnel(cppevent::EventLoop *event_loop, unsigned tunnel_buf_size)
	: event_loop_(event_loop)
	, event_(nullptr)
	, tunnel_(nullptr)
{
	event_ = (void*)event_new(
		(struct event_base*)event_loop->getBase(), -1, 
		EV_PERSIST | EV_READ, tunnelcb, (void*)this);
	if (event_ == nullptr)
	{
		throw std::bad_alloc();
	}
	event_add((struct event*)event_, nullptr);

	auto tunnel = new EventTunnelType(tunnel_buf_size);
	if (tunnel == nullptr)
	{
		throw std::bad_alloc();
	}
	tunnel_ = (void*)tunnel;
}
EventTunnel::~EventTunnel()
{
	if (event_ != nullptr)
	{
		event_free((struct event*)event_);
		event_ = nullptr;
	}

	if (tunnel_ != nullptr)
	{
		delete (EventTunnelType*)tunnel_;
	}
}

int EventTunnel::write(cppevent::TunnelMsg *arg)
{
	int ret = ((EventTunnelType*)tunnel_)->Write(arg);
	if (ret != muggle::TUNNEL_SUCCESS)
	{
		return ret;
	}

	event_active((struct event*)event_, EV_READ, 0);
	return 0;
}
void EventTunnel::onRead()
{
	std::queue<TunnelMsg*> queue;
	int ret = ((EventTunnelType*)tunnel_)->Read(queue, false);
	if (ret == muggle::TUNNEL_SUCCESS)
	{
		while (queue.size() > 0)
		{
			TunnelMsg *msg = queue.front();
			event_loop_->tunnelRead(msg);
			queue.pop();
		}
	}
}

void EventTunnel::open()
{
	((EventTunnelType*)tunnel_)->Open();
}
void EventTunnel::close()
{
	((EventTunnelType*)tunnel_)->Close();
}

NS_CPPEVENT_END
