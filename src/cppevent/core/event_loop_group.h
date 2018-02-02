#ifndef CPP_EVENT_EVENT_LOOP_GROUP_H_
#define CPP_EVENT_EVENT_LOOP_GROUP_H_

#include "cppevent/core/cppevent_def.h"
#include <thread>
#include <future>
#include <vector>
#include <string>
#include "cppevent/core/conn.h"
#include "cppevent/core/event_loop.h"

NS_CPPEVENT_BEGIN

enum eEventLoopOption
{
	CPPEVENT_BACKLOG = 0x01,
};

class EventLoopGroup
{
public:
	cppevent_core_EXPORT EventLoopGroup();
	cppevent_core_EXPORT ~EventLoopGroup();

	cppevent_core_EXPORT void run();
	cppevent_core_EXPORT void stop();

	cppevent_core_EXPORT EventLoopGroup& setHandler(bool shared, EventHandlerFactoryFunc* func);

	cppevent_core_EXPORT EventLoopGroup& acceptNumber(int number);
	cppevent_core_EXPORT EventLoopGroup& workerNumber(int number);

	cppevent_core_EXPORT EventLoopGroup& bind(const char *addr);

	cppevent_core_EXPORT EventLoopGroup& idleTimeout(long second);

	cppevent_core_EXPORT EventLoopGroup& option(int flag, int val);

	cppevent_core_EXPORT std::future<std::shared_ptr<Conn>> connect(const char *addr);

private:
	EventLoop* getWorker();

private:
	std::vector<std::thread> threads_;
	std::vector<EventLoop*> workers_;
	std::vector<EventLoop*> acceptors_;

	int backlog_;
	long idle_second_;

	int accept_thread_num_;
	int worker_thread_num_;
	bool is_shared_handler_;
	EventHandlerFactoryFunc *handler_factory_func_;
	std::string bind_addr_;

	int next_worker_;
};

NS_CPPEVENT_END

#endif