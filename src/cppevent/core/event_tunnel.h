#ifndef CPP_EVENT_EVENT_TUNNEL_H_
#define CPP_EVENT_EVENT_TUNNEL_H_

#include "cppevent/core/cppevent_def.h"
#include "cppevent/core/tunnel_msg.h"

NS_CPPEVENT_BEGIN

class EventLoop;

class EventTunnel
{
public:
	cppevent_core_EXPORT EventTunnel(EventLoop *event_loop, unsigned tunnel_buf_size=0);
	cppevent_core_EXPORT ~EventTunnel();

	cppevent_core_EXPORT int write(cppevent::TunnelMsg *arg);
	cppevent_core_EXPORT void onRead();

	cppevent_core_EXPORT void open();
	cppevent_core_EXPORT void close();

private:
	EventLoop *event_loop_;
	void *event_;
	void *tunnel_;
};

NS_CPPEVENT_END

#endif
