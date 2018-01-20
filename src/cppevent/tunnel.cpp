#include "tunnel.h"
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include "event_loop.h"

NS_CPPEVENT_BEGIN

static void tunnelcb(struct bufferevent*, void *ctx)
{
	Tunnel *tunnel = (Tunnel*)ctx;
	tunnel->onTunnelRead();
}

Tunnel::Tunnel(EventLoop *event_loop)
	: event_loop_(event_loop)
	, tunnel_{nullptr, nullptr}
	, enable_write_(true)
{
	struct bufferevent *tunnel[2];
	if (bufferevent_pair_new((struct event_base*)event_loop->getBase(), BEV_OPT_CLOSE_ON_FREE, tunnel) == 0)
	{
		bufferevent_setcb(tunnel[TUNNEL_OUT], tunnelcb, nullptr, nullptr, (void*)this);
		bufferevent_enable(tunnel[TUNNEL_OUT], EV_READ);
		bufferevent_enable(tunnel[TUNNEL_IN], EV_WRITE);

		tunnel_[TUNNEL_IN] = (void*)tunnel[TUNNEL_IN];
		tunnel_[TUNNEL_OUT] = (void*)tunnel[TUNNEL_OUT];
	}
}
Tunnel::~Tunnel()
{
	for (int i = 0; i < 2; ++i)
	{
		if (tunnel_[i])
		{
			bufferevent_free((struct bufferevent*)tunnel_[i]);
		}
	}
}

int Tunnel::write(void* arg)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!enable_write_)
	{
		return -1;
	}
	return bufferevent_write((struct bufferevent*)tunnel_[TUNNEL_IN], &arg, sizeof(arg));
}

void Tunnel::onTunnelRead()
{
	// without this lock, may lead error in evbuffer_free_trailing_empty_chains
	std::unique_lock<std::mutex> lock(mtx_);

	struct bufferevent* bev = (struct bufferevent*)tunnel_[TUNNEL_OUT];
	struct evbuffer *input = bufferevent_get_input(bev);
	while (evbuffer_get_length(input) > 0)
	{
		void *data;
		bufferevent_read(bev, &data, sizeof(data));
		event_loop_->tunnelRead((TunnelMsg*)data);
	}
}

void Tunnel::openInput()
{
	std::unique_lock<std::mutex> lock(mtx_);
	enable_write_ = true;
}
void Tunnel::closeInput()
{
	std::unique_lock<std::mutex> lock(mtx_);
	enable_write_ = false;
}

NS_CPPEVENT_END
