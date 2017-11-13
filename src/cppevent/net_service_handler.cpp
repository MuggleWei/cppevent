#include "net_service_handler.h"
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include "net_service.h"

NS_CPPEVENT_BEGIN

int NetServiceHandler::OnNewSocket(cppevent_socket_t fd, struct sockaddr_storage *ss,cppevent_socklen_t slen, FuncConnectCallback connect_cb)
{
	int ret = -1;

	if (!owner_)
	{
		return -1;
	}

	if (owner_->worker_threads_.empty())
	{
		if (!owner_->boss_thread_)
		{
			return -1;
		}

		ret = owner_->boss_thread_->AddSocket(fd, ss, slen, connect_cb);
	}
	else
	{
		static int s_idx = 0;
		ret = owner_->worker_threads_[s_idx++]->AddSocket(fd, ss, slen, connect_cb);
		if (s_idx >= owner_->worker_threads_.size())
		{
			s_idx = 0;
		}
	}

	return ret;
}
void NetServiceHandler::OnConnected(cppevent::Peer *peer)
{}
void NetServiceHandler::OnRead(cppevent::Peer *peer)
{}
void NetServiceHandler::OnWrite(cppevent::Peer *peer)
{}
void NetServiceHandler::OnEvent(cppevent::Peer *peer, short events)
{}

NS_CPPEVENT_END