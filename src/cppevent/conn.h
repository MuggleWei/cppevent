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
	static std::shared_ptr<Conn> connect(const char *addr);

public:
	cppevent_EXPORT Conn();
	cppevent_EXPORT ~Conn();

	cppevent_EXPORT const char* getLocalAddr() { return local_addr_; }
	cppevent_EXPORT const char* getRemoteAddr() { return remote_addr_; }

	cppevent_EXPORT ByteBuffer& getInputByteBuf() { return byte_buf_in_; }
	cppevent_EXPORT ByteBuffer& getOutputByteBuf() { return byte_buf_out_; }

	cppevent_EXPORT EventHandler* getHandler();
	cppevent_EXPORT EventLoop* getLoop();

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
};

// for shared ptr Conn compatible with c callback function
struct ConnContainer
{
	std::shared_ptr<Conn> connptr;
};

NS_CPPEVENT_END

#endif
