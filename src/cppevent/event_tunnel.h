#ifndef CPP_EVENT_EVENT_TUNNEL_H_
#define CPP_EVENT_EVENT_TUNNEL_H_

#include "cppevent/cppevent_def.h"
#include "tunnel_msg.h"

NS_CPPEVENT_BEGIN

class EventLoop;

class EventTunnel
{
public:
	cppevent_EXPORT EventTunnel(EventLoop *event_loop, unsigned tunnel_buf_size=0);
	cppevent_EXPORT ~EventTunnel();

	cppevent_EXPORT int write(cppevent::TunnelMsg *arg);
	cppevent_EXPORT void onRead();

	cppevent_EXPORT void open();
	cppevent_EXPORT void close();

private:
	EventLoop *event_loop_;
	void *event_;
	void *tunnel_;
};

NS_CPPEVENT_END

#endif
