#ifndef CPP_EVENT_CONN_H_
#define CPP_EVENT_CONN_H_

#include "cppevent/cppevent_def.h"
#include <memory>
#include "cppevent/byte_buf.h"

NS_CPPEVENT_BEGIN

class EventLoop;
class Conn;
struct ConnContainer;
class EventHandler;

class Conn
{
public:
	friend class EventLoop;

public:
	cppevent_EXPORT Conn();
	cppevent_EXPORT ~Conn();

	cppevent_EXPORT const char* getLocalAddr() { return local_addr_; }
	cppevent_EXPORT const char* getRemoteAddr() { return remote_addr_; }

	cppevent_EXPORT ByteBuffer& getInputByteBuf() { return byte_buf_in_; }
	cppevent_EXPORT ByteBuffer& getOutputByteBuf() { return byte_buf_out_; }

	cppevent_EXPORT EventHandler* getHandler();
	cppevent_EXPORT EventLoop* getLoop();

	cppevent_EXPORT void close();

	cppevent_EXPORT size_t getReadableLength();
	cppevent_EXPORT size_t peekBytes(void *data_out, size_t datalen);
	cppevent_EXPORT size_t readBytes(void *data_out, size_t datalen);

	cppevent_EXPORT int write(void *data_out, size_t datalen);
	cppevent_EXPORT int write(ByteBuffer& buf);
	cppevent_EXPORT int write(ByteBuffer& buf, size_t datalen);
	cppevent_EXPORT int writeAndClose(void *data_out, size_t datalen);
	cppevent_EXPORT int writeAndClose(ByteBuffer& buf);
	cppevent_EXPORT int writeAndClose(ByteBuffer& buf, size_t datalen);

	cppevent_EXPORT void afterRead();
	cppevent_EXPORT void afterWrite();

	cppevent_EXPORT void updateLastInTime();
	cppevent_EXPORT void updateLastOutTime();
	cppevent_EXPORT long getLastInTime();
	cppevent_EXPORT long getLastOutTime();
	cppevent_EXPORT long getLastActiveTime();

protected:
	void setBev(void *bev);

protected:
	ConnContainer *container_;

	EventLoop *event_loop_;
	void *bev_;

	ByteBuffer byte_buf_in_;
	ByteBuffer byte_buf_out_;

	char local_addr_[128];
	char remote_addr_[128];

	EventHandler *handler_;
	bool shared_handler_;

	bool wait_close_;
	long last_in_time_;
	long last_out_time_;
};

// for shared ptr Conn compatible with c callback function
struct ConnContainer
{
	std::shared_ptr<Conn> connptr;
};

NS_CPPEVENT_END

#endif
