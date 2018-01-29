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
	, wait_close_(false)
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

size_t Conn::PeekBytes(void *data_out, size_t datalen)
{
	return byte_buf_in_.PeekBytes(data_out, datalen);
}
size_t Conn::ReadBytes(void *data_out, size_t datalen)
{
	return byte_buf_in_.ReadBytes(data_out, datalen);
}

int Conn::write(void *data_out, size_t datalen)
{
	return byte_buf_out_.Write(data_out, datalen);
}
int Conn::write(ByteBuffer& buf)
{
	return byte_buf_out_.Append(buf);
}
int Conn::write(ByteBuffer& buf, size_t datalen)
{
	return byte_buf_out_.Append(buf, datalen);
}
int Conn::writeAndClose(void *data_out, size_t datalen)
{
	wait_close_ = true;
	return byte_buf_out_.Write(data_out, datalen);
}
int Conn::writeAndClose(ByteBuffer& buf)
{
	wait_close_ = true;
	return byte_buf_out_.Append(buf);
}
int Conn::writeAndClose(ByteBuffer& buf, size_t datalen)
{
	wait_close_ = true;
	return byte_buf_out_.Append(buf, datalen);
}

void Conn::afterWrite()
{
	if (wait_close_ && byte_buf_out_.GetByteLength() == 0)
	{
		close();
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