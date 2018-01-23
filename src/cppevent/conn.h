#ifndef CPP_EVENT_CONN_H_
#define CPP_EVENT_CONN_H_

#include "cppevent/cppevent_def.h"
#include <memory>
#include "cppevent/byte_buf.h"

NS_CPPEVENT_BEGIN

class Conn;
struct ConnContainer;

class Conn
{
public:
	friend class EventLoop;

public:
	static std::shared_ptr<Conn> connect(const char *addr);

public:
	cppevent_EXPORT Conn();
	cppevent_EXPORT ~Conn();

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
};

// for shared ptr Conn compatible with c callback function
struct ConnContainer
{
	std::shared_ptr<Conn> connptr;
};

NS_CPPEVENT_END

#endif