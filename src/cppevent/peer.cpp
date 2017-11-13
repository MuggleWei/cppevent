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
	, byte_buf_in_(nullptr)
	, byte_buf_out_(nullptr)
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

int Peer::FlushOutput()
{
	if (bev_ == nullptr)
	{
		return -1;
	}
	return bufferevent_flush((struct bufferevent*)bev_, EV_WRITE, BEV_FLUSH);
}

void Peer::setBev(void *bev)
{
	if (bev_ == nullptr)
	{
		bev_ = bev;
		byte_buf_in_.ev_buf_ = bufferevent_get_input((struct bufferevent*)bev_);
		byte_buf_out_.ev_buf_ = bufferevent_get_output((struct bufferevent*)bev_);
	}
}

NS_CPPEVENT_END