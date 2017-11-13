#include "echo_client_handler.h"
#include <string.h>

void EchoClientHandler::OnConnected(cppevent::Peer *peer)
{
	fprintf(stdout, "connected: %s <--> %s\n", peer->getLocalAddr(), peer->getRemoteAddr());
}
void EchoClientHandler::OnRead(cppevent::Peer *peer)
{
	char buf[1025];
	while (true)
	{
		memset(buf, 0, sizeof(buf));
		size_t len = peer->getInputByteBuf().GetByteLength();
		if (len == 0)
		{
			return;
		}

		len = len > 1024 ? 1024 : len;
		size_t read_bytes = peer->getInputByteBuf().ReadBytes(buf, len);
		if (read_bytes == -1)
		{
			fprintf(stderr, "error read!\n");
			return;
		}
		fprintf(stdout, "receieve echo: %s", buf);
	}
}
void EchoClientHandler::OnEvent(cppevent::Peer *peer, short events)
{
	if (events & CPPEVENT_ERROR)
	{
		fprintf(stdout, "disconnect: %s <--> %s\n", peer->getLocalAddr(), peer->getRemoteAddr());
		exit(EXIT_SUCCESS);
	}
}
