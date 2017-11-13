#ifndef CPPEVENT_NET_SERVICE_HANDLER_H_
#define CPPEVENT_NET_SERVICE_HANDLER_H_

#include "cppevent/cppevent_def.h"
#include "cppevent/peer.h"

NS_CPPEVENT_BEGIN

class NetService;

class NetServiceHandler
{
	friend class NetService;

public:
	cppevent_EXPORT virtual int OnNewSocket(cppevent_socket_t fd, struct sockaddr_storage *ss, cppevent_socklen_t slen, FuncConnectCallback connect_cb);

	cppevent_EXPORT virtual void OnConnected(cppevent::Peer *peer);
	cppevent_EXPORT virtual void OnRead(cppevent::Peer *peer);
	cppevent_EXPORT virtual void OnWrite(cppevent::Peer *peer);
	cppevent_EXPORT virtual void OnEvent(cppevent::Peer *peer, short events);

protected:
	NetService *owner_;
};

NS_CPPEVENT_END

#endif
