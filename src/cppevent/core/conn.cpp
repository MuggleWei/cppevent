#include "conn.h"
#include <event2/event.h>
#include <event2/bufferevent.h>
#include "event_loop.h"
#include "cppevent_endian.h"

NS_CPPEVENT_BEGIN

Conn::Conn()
	: container_(nullptr)
	, event_loop_(nullptr)
	, bev_(nullptr)
	, handler_(nullptr)
	, shared_handler_(true)
	, wait_close_(false)
	, wait_write_close_(false)
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
	wait_close_ = true;
}
void Conn::shutdown()
{
	if (event_loop_ && container_)
	{
		event_loop_->delConn(container_->connptr);
	}
}

size_t Conn::getReadableLength()
{
	return byte_buf_in_.GetByteLength();
}
size_t Conn::peekBytes(void *data_out, size_t datalen)
{
	return byte_buf_in_.PeekBytes(data_out, datalen);
}
size_t Conn::readBytes(void *data_out, size_t datalen)
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
	wait_write_close_ = true;
	return byte_buf_out_.Write(data_out, datalen);
}
int Conn::writeAndClose(ByteBuffer& buf)
{
	wait_write_close_ = true;
	return byte_buf_out_.Append(buf);
}
int Conn::writeAndClose(ByteBuffer& buf, size_t datalen)
{
	wait_write_close_ = true;
	return byte_buf_out_.Append(buf, datalen);
}

int16_t Conn::peekInt16()
{
	int16_t val;
	byte_buf_in_.PeekBytes(&val, sizeof(val));
	return CPPEVENT_NTOH_16(val);
}
int32_t Conn::peekInt32()
{
	int32_t val;
	byte_buf_in_.PeekBytes(&val, sizeof(val));
	return CPPEVENT_NTOH_32(val);
}
int64_t Conn::peekInt64()
{
	int64_t val;
	byte_buf_in_.PeekBytes(&val, sizeof(val));
	return CPPEVENT_NTOH_64(val);
}
float Conn::peekFloat()
{
	float val;
	byte_buf_in_.PeekBytes(&val, sizeof(val));
	return val;
}
double Conn::peekDouble()
{
	double val;
	byte_buf_in_.PeekBytes(&val, sizeof(val));
	return val;
}
int16_t Conn::readInt16()
{
	int16_t val;
	byte_buf_in_.ReadBytes(&val, sizeof(val));
	return CPPEVENT_NTOH_16(val);
}
int32_t Conn::readInt32()
{
	int32_t val;
	byte_buf_in_.ReadBytes(&val, sizeof(val));
	return CPPEVENT_NTOH_32(val);
}
int64_t Conn::readInt64()
{
	int64_t val;
	byte_buf_in_.ReadBytes(&val, sizeof(val));
	return CPPEVENT_NTOH_64(val);
}
float Conn::readFloat()
{
	float val;
	byte_buf_in_.ReadBytes(&val, sizeof(val));
	return val;
}
double Conn::readDouble()
{
	double val;
	byte_buf_in_.ReadBytes(&val, sizeof(val));
	return val;
}
int Conn::writeInt16(int16_t val)
{
	val = CPPEVENT_HTON_16(val);
	return byte_buf_out_.Write(&val, sizeof(val));
}
int Conn::writeInt32(int32_t val)
{
	val = CPPEVENT_HTON_32(val);
	return byte_buf_out_.Write(&val, sizeof(val));
}
int Conn::writeInt64(int64_t val)
{
	val = CPPEVENT_HTON_64(val);
	return byte_buf_out_.Write(&val, sizeof(val));
}
int Conn::writeFloat(float val)
{
	return byte_buf_out_.Write(&val, sizeof(val));
}
int Conn::writeDouble(double val)
{
	return byte_buf_out_.Write(&val, sizeof(val));
}


void Conn::afterRead()
{
	if (wait_close_)
	{
		shutdown();
	}
	else
	{
		updateLastInTime();
	}
}
void Conn::afterWrite()
{
	if ((wait_write_close_ || wait_close_) && byte_buf_out_.GetByteLength() == 0)
	{
		shutdown();
	}
	else
	{
		updateLastOutTime();
	}
}

void Conn::updateLastInTime()
{
	struct event_base *base = (struct event_base*)event_loop_->getBase();
	struct timeval tv;
	event_base_gettimeofday_cached(base, &tv);
	last_in_time_ = tv.tv_sec;
}
void Conn::updateLastOutTime()
{
	struct event_base *base = (struct event_base*)event_loop_->getBase();
	struct timeval tv;
	event_base_gettimeofday_cached(base, &tv);
	last_out_time_ = tv.tv_sec;
}

long Conn::getLastInTime()
{
	return last_in_time_;
}
long Conn::getLastOutTime()
{
	return last_out_time_;
}
long Conn::getLastActiveTime()
{
	return last_out_time_ > last_in_time_ ? last_out_time_ : last_in_time_;
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