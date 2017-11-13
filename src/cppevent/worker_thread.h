#ifndef CPPEVENT_WORKER_THREAD_H_
#define CPPEVENT_WORKER_THREAD_H_

#include <set>
#include <thread>
#include <memory>
#include "cppevent/cppevent_def.h"
#include "cppevent/peer.h"

NS_CPPEVENT_BEGIN

class NetService;

class WorkerThread
{
public:
	cppevent_EXPORT static WorkerThread* Create(NetService *service, int id, bool run = true);

	cppevent_EXPORT WorkerThread(NetService *service, int id);
	cppevent_EXPORT virtual ~WorkerThread();

	cppevent_EXPORT void Run();

	cppevent_EXPORT int BindAndListen(const char *addr_ip_port, int backlog, FuncAcceptCb *accept_cb);
	cppevent_EXPORT int BindAndListen(int port, int backlog, FuncAcceptCb *accept_cb);

	cppevent_EXPORT int AddSocket(cppevent_socket_t fd, struct sockaddr_storage *ss,cppevent_socklen_t slen, FuncConnectCallback cb = nullptr);

	cppevent_EXPORT size_t getPeerCount();

	cppevent_EXPORT void OnRead(Peer *peer);
	cppevent_EXPORT void OnWrite(Peer *peer);
	cppevent_EXPORT void OnEvent(Peer *peer, short events);

	cppevent_EXPORT std::thread::id getThreadId() { return system_thread_id_; }
	
private:
	int BindListen(cppevent_socket_t listener, struct sockaddr* sa, int slen, int backlog, FuncAcceptCb *accept_cb);

	Peer* AddSocketImmediately(cppevent_socket_t fd, struct sockaddr_storage *ss, cppevent_socklen_t slen);

	void FreePeer(Peer *peer);

private:
	NetService *owner_;
	std::thread::id system_thread_id_;
	int worker_id_;
	void *base_;
	void *tunnel_[2];
	std::set<Peer*> peers_;

	FuncAcceptCb accept_cb_;
};

NS_CPPEVENT_END

#endif