#ifndef CPP_EVENT_TUNNEL_MSG_H_
#define CPP_EVENT_TUNNEL_MSG_H_

#include "cppevent/core/cppevent_def.h"
#include <string.h>
#include <iostream>
#include <future>
#ifdef WIN32
#include <WinSock2.h>
#endif
#include "cppevent/core/timer.h"

NS_CPPEVENT_BEGIN

class Conn;

enum TunnelMsgType
{
	TunnelMsgType_Profile = 0,
	TunnelMsgType_Stop,
	TunnelMsgType_Timer,
	TunnelMsgType_StopTimer,
	TunnelMsgType_BindAndListen,
	TunnelMsgType_AcceptConn,
	TunnelMsgType_AddConn,
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

class TunnelMsgBindAndListen : public TunnelMsg
{
public:
	TunnelMsgBindAndListen(
		const char *addr_in, 
		int backlog_in, 
		std::promise<int> &&promise_in)
		: TunnelMsg(TunnelMsgType_Stop)
		, backlog(backlog_in)
		, promise(std::move(promise_in))
	{
		strncpy(addr, addr_in, sizeof(addr));
	}

	char addr[512];
	int backlog;
	std::promise<int> promise;
};

class TunnelMsgAcceptConn : public TunnelMsg
{
public:
	TunnelMsgAcceptConn(
		cppevent_socket_t fd_in,
		struct sockaddr *addr_in,
		int socklen_in
		)
		: TunnelMsg(TunnelMsgType_AcceptConn)
		, fd(fd_in)
		, socklen(socklen_in)
	{
		memcpy(&addr, addr_in, socklen);
	}

	cppevent_socket_t fd;
	struct sockaddr_storage addr;
	int socklen;
};

class TunnelMsgAddConn : public TunnelMsg
{
public:
	TunnelMsgAddConn(
		const char *addr_in, 
		std::promise<std::shared_ptr<Conn>> promise_in)
		: TunnelMsg(TunnelMsgType_AddConn)
		, promise(std::move(promise_in))
	{
		strncpy(addr, addr_in, sizeof(addr));
	}

	char addr[512];
	std::promise<std::shared_ptr<Conn>> promise;
};

NS_CPPEVENT_END

#endif
