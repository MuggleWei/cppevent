#include "byte_buf.h"
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

NS_CPPEVENT_BEGIN

ByteBuffer::ByteBuffer(void *ev_buf)
	: ev_buf_(ev_buf)
{}
ByteBuffer::~ByteBuffer()
{
	ev_buf_ = nullptr;
}

size_t ByteBuffer::GetByteLength()
{
	if (ev_buf_ == nullptr)
	{
		return 0;
	}
	return evbuffer_get_length((struct evbuffer*)ev_buf_);
}
size_t ByteBuffer::PeekBytes(void *data_out, size_t datalen)
{
	if (ev_buf_ == nullptr)
	{
		return -1;
	}
	return (size_t)evbuffer_copyout((struct evbuffer*)ev_buf_, data_out, datalen);
}
size_t ByteBuffer::ReadBytes(void *data_out, size_t datalen)
{
	if (ev_buf_ == nullptr)
	{
		return -1;
	}
	return (size_t)evbuffer_remove((struct evbuffer*)ev_buf_, data_out, datalen);
}
int ByteBuffer::Write(void *data_out, size_t datalen)
{
	if (ev_buf_ == nullptr)
	{
		return -1;
	}
	return evbuffer_add((struct evbuffer*)ev_buf_, data_out, datalen);
}
int ByteBuffer::Append(ByteBuffer& buf)
{
	if (ev_buf_ == nullptr || buf.ev_buf_ == nullptr)
	{
		return -1;
	}

	return evbuffer_add_buffer((struct evbuffer*)ev_buf_, (struct evbuffer*)buf.ev_buf_);
}
int ByteBuffer::Append(ByteBuffer& buf, size_t datalen)
{
	if (ev_buf_ == nullptr || buf.ev_buf_ == nullptr)
	{
		return -1;
	}
	return evbuffer_remove_buffer((struct evbuffer*)ev_buf_, (struct evbuffer*)buf.ev_buf_, datalen);
}
int ByteBuffer::AppendRef(ByteBuffer& buf)
{
	if (ev_buf_ == nullptr || buf.ev_buf_ == nullptr)
	{
		return -1;
	}
	return evbuffer_add_buffer_reference((struct evbuffer*)ev_buf_, (struct evbuffer*)buf.ev_buf_);
}

NS_CPPEVENT_END