#ifndef CPPEVENT_NET_SERVICE_H_
#define CPPEVENT_NET_SERVICE_H_

#include <vector>
#include "cppevent/cppevent_def.h"
#include "cppevent/worker_thread.h"
#include "cppevent/net_service_handler.h"

NS_CPPEVENT_BEGIN

enum eNetServiceOption
{
	BACKLOG_NUM,
};


class NetService
{
	friend class NetServiceHandler;

public:
	cppevent_EXPORT NetService();
	cppevent_EXPORT virtual ~NetService();

	NetService(const NetService &rhs) = delete;
	NetService(const NetService &&rhs) = delete;
	NetService& operator=(const NetService &rhs) = delete;
	NetService& operator=(const NetService &&rhs) = delete;

	cppevent_EXPORT void Run();
	cppevent_EXPORT bool InRunning();

	cppevent_EXPORT NetService& WorkerThread(int num);
	cppevent_EXPORT NetService& Handler(NetServiceHandler *handler);
	cppevent_EXPORT NetService& Bind(const char *addr_ip_port);
	cppevent_EXPORT NetService& Bind(int port);
	cppevent_EXPORT NetService& Option(int32_t option, int64_t value);

	cppevent_EXPORT NetServiceHandler* getHandler();

	/*
	* @return On success add new peer, return 0. Failed return -1, success
	*         add to other thread return 1
	*/
	cppevent_EXPORT int AddConnectTo(const char *addr_ip_port, FuncConnectCallback connect_cb = nullptr);
	cppevent_EXPORT Peer* SyncAddConnectTo(const char *addr_ip_port);

private:
	int InitNetAndSingal();
	int CreateWorkerThread();
	int BindAndListen();

	/*
	 * @return On success add new peer, return 0. Failed return -1, success
	 *         add to other thread return 1
	 */
	int OnNewSocket(
		cppevent_socket_t fd, struct sockaddr_storage *ss, 
		cppevent_socklen_t slen, FuncConnectCallback connect_cb = nullptr);

private:
	cppevent::WorkerThread *boss_thread_;
	std::vector<cppevent::WorkerThread*> worker_threads_;
	cppevent::NetServiceHandler *handler_;
	char bind_addr_[64];
	int bind_port_;
	unsigned short worker_thread_num_;
	int backlog_;

	bool running_;

	FuncAcceptCb accept_cb_;
};

NS_CPPEVENT_END

#endif