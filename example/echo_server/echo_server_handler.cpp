#include "echo_server_handler.h"

void EchoServerHandler::OnConnected(cppevent::Peer *peer)
{
	fprintf(stdout, "connected: %s <--> %s\n", peer->getLocalAddr(), peer->getRemoteAddr());
}
void EchoServerHandler::OnRead(cppevent::Peer *peer)
{
	char buf[1025];
	while (true)
	{
		memset(buf, 0, sizeof(buf));
		size_t len = peer->GetByteLength();
		if (len == 0)
		{
			return;
		}

		len = len > 1024 ? 1024 : len;
		size_t read_bytes = peer->ReadBytes(buf, len);
		if (read_bytes == -1)
		{
			fprintf(stderr, "error read!\n");
			return;
		}
		fprintf(stdout, buf);

		peer->Write(buf, read_bytes);
	}
}
void EchoServerHandler::OnEvent(cppevent::Peer *peer, short events)
{
	if (events & CPPEVENT_ERROR)
	{
		fprintf(stdout, "disconnect: %s <--> %s\n", peer->getLocalAddr(), peer->getRemoteAddr());
	}
}