#include "conn.h"
#include <event2/bufferevent.h>
#include "event_loop.h"

NS_CPPEVENT_BEGIN

Conn::Conn()
	: container_(nullptr)
	, event_loop_(nullptr)
	, bev_(nullptr)
	, handler_(nullptr)
	, shared_handler_(true)
{}
Conn::~Conn()
{
	if (bev_)
	{
		struct bufferevent *bev = (struct bufferevent*)bev_;
		bufferevent_free(bev);
	}
}

EventHandler* Conn::getHandler()
{
	return handler_;
}
EventLoop* Conn::getLoop()
{
	return event_loop_;
}

void Conn::close()
{
	if (event_loop_ && container_)
	{
		event_loop_->delConn(container_->connptr);
	}
}

void Conn::setBev(void *bev)
{
	if (bev_ == nullptr)
	{
		bev_ = bev;
		byte_buf_in_.setEvbuf(bufferevent_get_input((struct bufferevent*)bev));
		byte_buf_out_.setEvbuf(bufferevent_get_output((struct bufferevent*)bev));
	}
}

NS_CPPEVENT_END