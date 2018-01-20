#ifndef CPP_EVENT_TUNNEL_H_
#define CPP_EVENT_TUNNEL_H_

#include "cppevent/cppevent_def.h"
#include <mutex>

NS_CPPEVENT_BEGIN

class EventLoop;

class Tunnel
{
public:
	cppevent_EXPORT Tunnel(EventLoop *event_loop);
	cppevent_EXPORT ~Tunnel();

	// Write data into tunnel
	// @return 0 if successful, or -1 if an error occurred
	cppevent_EXPORT int write(void *arg);
	cppevent_EXPORT void onTunnelRead();

	cppevent_EXPORT void openInput();
	cppevent_EXPORT void closeInput();

private:
	enum eTunnelDirection
	{
		TUNNEL_IN = 0,
		TUNNEL_OUT = 1
	};

	EventLoop *event_loop_;
	void *tunnel_[2];

	std::mutex mtx_;
	bool enable_write_;
};

NS_CPPEVENT_END

#endif
