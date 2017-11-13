#include "net_service.h"
#ifndef WIN32
#include <signal.h>
#endif
#include <string.h>
#include <assert.h>
#include <mutex>
#include <condition_variable>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

NS_CPPEVENT_BEGIN

cppevent::NetServiceHandler default_handler;

NetService::NetService()
	: boss_thread_(nullptr)
	, handler_(&default_handler)
	, bind_addr_{0}
	, bind_port_(0)
	, worker_thread_num_(0)
	, backlog_(128)
	, running_(false)
	, accept_cb_(std::bind(&NetService::OnNewSocket, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, nullptr))
{
	handler_->owner_ = this;
}
NetService::~NetService()
{
	// TODO: 
}

void NetService::Run()
{
	int ret = 0;

	ret = InitNetAndSingal();
	if (ret != 0)
	{
		return;
	}
	
	boss_thread_ = cppevent::WorkerThread::Create(this, 0, false);
	if (boss_thread_ == nullptr)
	{
		return;
	}

	CreateWorkerThread();

	if (BindAndListen() != 0)
	{
		fprintf(stderr, "Failed Bind and listen\n");
		assert(false);
	}

	running_ = true;
	boss_thread_->Run();
	running_ = false;
}
bool NetService::InRunning()
{
	return running_;
}

NetService& NetService::WorkerThread(int num)
{
	if (running_)
	{
		return *this;
	}

	worker_thread_num_ = num;
	return *this;
}
NetService& NetService::Handler(NetServiceHandler *handler)
{
	if (running_)
	{
		return *this;
	}

	handler_ = handler;
	if (handler_)
	{
		handler_->owner_ = this;
	}
	return *this;
}
NetService& NetService::Bind(const char *addr_ip_port)
{
	if (running_)
	{
		return *this;
	}

	memset(bind_addr_, 0, sizeof(bind_addr_));
	strncpy(bind_addr_, addr_ip_port, sizeof(bind_addr_) - 1);
	bind_port_ = 0;
	return *this;
}
NetService& NetService::Bind(int port)
{
	if (running_)
	{
		return *this;
	}

	memset(bind_addr_, 0, sizeof(bind_addr_));
	bind_port_ = port;
	return *this;
}
NetService& NetService::Option(int32_t option, int64_t value)
{
	if (running_)
	{
		return *this;
	}

	switch (option)
	{
	case BACKLOG_NUM:
	{
		backlog_ = (decltype(backlog_))value;
	}break;
	default:
		break;
	}

	return *this;
}

NetServiceHandler* NetService::getHandler()
{
	return handler_;
}

int NetService::AddConnectTo(const char *addr_ip_port, FuncConnectCallback connect_cb)
{
	struct sockaddr_storage remote_ss;
	int remote_ss_len = (int)sizeof(remote_ss);
	if (evutil_parse_sockaddr_port(addr_ip_port, (struct sockaddr*)&remote_ss, &remote_ss_len) < 0)
	{
		return -1;
	}

	evutil_socket_t fd = socket(remote_ss.ss_family, SOCK_STREAM, 0);
	if (fd < 0)
	{
		return -1;
	}

	if (connect(fd, (struct sockaddr*)&remote_ss, remote_ss_len) < 0)
	{
		evutil_closesocket(fd);
		return -1;
	}

	int ret = OnNewSocket(fd, &remote_ss, remote_ss_len, connect_cb);
	return ret;
}
Peer* NetService::SyncAddConnectTo(const char *addr_ip_port)
{
	std::mutex mtx;
	std::condition_variable cv;
	Peer *peer = nullptr;
	bool is_same_thread = false;
	std::thread::id thread_id = std::this_thread::get_id();

	std::unique_lock<std::mutex> lock(mtx);
	int ret = AddConnectTo(addr_ip_port, [&peer, &thread_id, &cv, &mtx](Peer *conn_peer) {
		peer = conn_peer;
		if (thread_id != std::this_thread::get_id())
		{
			mtx.lock();
			mtx.unlock();
			cv.notify_all();
		}
	});

	if (ret == 0)
	{
		// lambda is the same thread to this function thread, so peer already set
		return peer;
	}
	else if (ret < 0)
	{
		// failed create peer
		return nullptr;
	}

	// add socket message to other thread, wait for lambda return
	assert(ret == 1);
	cv.wait(lock);

	return peer;
}

int NetService::InitNetAndSingal()
{
	int err = 0;

#if WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) 
	{
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		fprintf(stderr, "WSAStartup failed with error: %d\n", err);
		assert(err == 0);
		return 1;
	}

	err = evthread_use_windows_threads();
	if (err != 0)
	{
		fprintf(stderr, "evthread failed use windows threads: %d\n", err);
		assert(err == 0);
		return 2;
	}
#else
	signal(SIGPIPE, SIG_IGN);

	err = evthread_use_pthreads();
	if (err != 0)
	{
		fprintf(stderr, "evthread failed use pthreads: %d\n", err);
		assert(err == 0);
		return 2;
	}
#endif

	return 0;
}
int NetService::CreateWorkerThread()
{
	if (worker_thread_num_ < 0)
	{
		worker_thread_num_ = 0;
	}

	for (int i = 0; i < worker_thread_num_; ++i)
	{
		cppevent::WorkerThread *worker = cppevent::WorkerThread::Create(this, i);
		if (worker)
		{
			worker_threads_.push_back(worker);
		}
	}
	
	if (worker_thread_num_ != worker_threads_.size())
	{
		assert(worker_thread_num_ == worker_threads_.size());
		fprintf(stderr, "Failed create worker thread\n");
		return 1;
	}

	return 0;
}
int NetService::BindAndListen()
{
	if (boss_thread_ == nullptr)
	{
		assert(boss_thread_ != nullptr);
		return -1;
	}

	if (bind_addr_[0] != '\0')
	{
		return boss_thread_->BindAndListen(bind_addr_, backlog_, &accept_cb_);
	}
	else
	{
		return boss_thread_->BindAndListen(bind_port_, backlog_, &accept_cb_);
	}
}

int NetService::OnNewSocket(cppevent_socket_t fd, struct sockaddr_storage *ss, cppevent_socklen_t slen, FuncConnectCallback connect_cb)
{
	int ret = -1;
	if (handler_)
	{
		ret = handler_->OnNewSocket(fd, ss, slen, connect_cb);
	}

	return ret;
}

NS_CPPEVENT_END
