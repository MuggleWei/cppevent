#include "peer.h"
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

NS_CPPEVENT_BEGIN

Peer::Peer()
	: extra_(nullptr)
	, owner_thread_(nullptr)
	, bev_(nullptr)
	, remote_ss_len_(0)
	, flag_(0)
{}
Peer::~Peer()
{
	if (bev_)
	{
		bufferevent_free((struct bufferevent*)bev_);
		bev_ = nullptr;
	}
}

void Peer::Close()
{
	flag_ |= PEER_FLAG_CLOSE;
}

size_t Peer::GetByteLength()
{
	if (bev_ == nullptr)
	{
		return 0;
	}
	struct evbuffer *input = bufferevent_get_input((struct bufferevent*)bev_);
	return evbuffer_get_length(input);
}

size_t Peer::PeekBytes(void *data_out, size_t datalen)
{
	if (bev_ == nullptr)
	{
		return -1;
	}
	struct evbuffer *input = bufferevent_get_input((struct bufferevent*)bev_);
	return (size_t)evbuffer_copyout(input, data_out, datalen);
}

size_t Peer::ReadBytes(void *data_out, size_t datalen)
{
	if (bev_ == nullptr)
	{
		return -1;
	}

	struct evbuffer *input = bufferevent_get_input((struct bufferevent*)bev_);
	return (size_t)evbuffer_remove(input, data_out, datalen);
}

int Peer::Write(void *data_out, size_t datalen)
{
	if (bev_ == nullptr)
	{
		return -1;
	}

	return bufferevent_write((struct bufferevent*)bev_, data_out, datalen);
}
int Peer::WriteAndFlush(void *data_out, size_t datalen)
{
	if (bev_ == nullptr)
	{
		return -1;
	}

	int ret = bufferevent_write((struct bufferevent*)bev_, data_out, datalen);
	if (ret != 0)
	{
		return ret;
	}
	return bufferevent_flush((struct bufferevent*)bev_, EV_WRITE, BEV_FLUSH);
}

NS_CPPEVENT_END