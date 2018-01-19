#include "tunnel.h"
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include "event_loop.h"

NS_CPPEVENT_BEGIN

static void tunnelcb(struct bufferevent *bev, void *ctx)
{
	Tunnel *tunnel = (Tunnel*)ctx;
	tunnel->onTunnelRead();
}

Tunnel::Tunnel(EventLoop *event_loop)
	: event_loop_(event_loop)
	, tunnel_{nullptr, nullptr}
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
	return bufferevent_write((struct bufferevent*)tunnel_[TUNNEL_IN], &arg, sizeof(arg));
}

void Tunnel::onTunnelRead()
{
	struct bufferevent* bev = (struct bufferevent*)tunnel_[TUNNEL_OUT];
	struct evbuffer *input = bufferevent_get_input(bev);
	while (evbuffer_get_length(input) > 0)
	{
		void *data;
		bufferevent_read(bev, &data, sizeof(data));
		event_loop_->tunnelRead(data);
	}
}

NS_CPPEVENT_END