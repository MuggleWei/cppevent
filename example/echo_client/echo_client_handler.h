#ifndef ECHO_SERVER_HANDLER_H_
#define ECHO_SERVER_HANDLER_H_

#include "cppevent/cppevent.h"

class EchoClientHandler : public cppevent::NetServiceHandler
{
public:
	virtual void OnConnected(cppevent::Peer *peer) override;
	virtual void OnRead(cppevent::Peer *peer) override;
	virtual void OnEvent(cppevent::Peer *peer, short events) override;
};

#endif