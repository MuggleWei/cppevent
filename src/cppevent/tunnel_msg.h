#ifndef CPP_EVENT_TUNNEL_MSG_H_
#define CPP_EVENT_TUNNEL_MSG_H_

#include "cppevent/cppevent_def.h"
#include <iostream>
#include <future>
#include "cppevent/timer.h"

NS_CPPEVENT_BEGIN

enum TunnelMsgType
{
	TunnelMsgType_Profile = 0,
	TunnelMsgType_Stop,
	TunnelMsgType_Timer,
	TunnelMsgType_StopTimer,
};

class TunnelMsg
{
public:
	TunnelMsg(TunnelMsgType t) : type(t) {}
	virtual ~TunnelMsg() {}

	TunnelMsgType type;
};

class TunnelMsgProfile : public TunnelMsg
{
public:
	TunnelMsgProfile(int64_t m)
		: TunnelMsg(TunnelMsgType_Profile)
		, microsec(m)
	{}

	std::thread::id thread_id;
	int message_id;
	int64_t microsec;
};

class TunnelMsgStop : public TunnelMsg
{
public:
	TunnelMsgStop()
		: TunnelMsg(TunnelMsgType_Stop)
	{}
};

class TunnelMsgAddTimer : public TunnelMsg
{
public:
	TunnelMsgAddTimer(
		long mill_seconds_in,
		std::function<void()> &&fn_in,
		bool is_once_in,
		std::promise<cppevent::Timer*> &&promise_in)
		: TunnelMsg(TunnelMsgType_Timer)
		, mill_seconds(mill_seconds_in)
		, fn(std::move(fn_in))
		, is_once(is_once_in)
		, promise(std::move(promise_in))
	{}

	long mill_seconds;
	std::function<void()> fn;
	bool is_once;
	std::promise<cppevent::Timer*> promise;
};

class TunnelMsgStopTimer : public TunnelMsg
{
public:
	TunnelMsgStopTimer(cppevent::Timer* timer_in)
		: TunnelMsg(TunnelMsgType_StopTimer)
		, timer(timer_in)
	{}

	cppevent::Timer *timer;
};

NS_CPPEVENT_END

#endif
