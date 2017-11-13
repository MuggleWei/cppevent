#include "echo_server_handler.h"

void EchoServerHandler::OnConnected(cppevent::Peer *peer)
{
	fprintf(stdout, "connected: %s <--> %s\n", peer->getLocalAddr(), peer->getRemoteAddr());
}
void EchoServerHandler::OnRead(cppevent::Peer *peer)
{
	peer->getOutputByteBuf().Append(peer->getInputByteBuf());
}
void EchoServerHandler::OnEvent(cppevent::Peer *peer, short events)
{
	if (events & CPPEVENT_ERROR)
	{
		fprintf(stdout, "disconnect: %s <--> %s\n", peer->getLocalAddr(), peer->getRemoteAddr());
	}
}