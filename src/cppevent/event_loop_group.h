#ifndef CPP_EVENT_EVENT_LOOP_GROUP_H_
#define CPP_EVENT_EVENT_LOOP_GROUP_H_

#include "cppevent/cppevent_def.h"
#include <thread>
#include <future>
#include <vector>
#include <string>
#include "cppevent/conn.h"
#include "cppevent/event_loop.h"

NS_CPPEVENT_BEGIN

enum eEventLoopOption
{
	CPPEVENT_BACKLOG = 0x01,
};

class EventLoopGroup
{
public:
	cppevent_EXPORT EventLoopGroup();
	cppevent_EXPORT ~EventLoopGroup();

	cppevent_EXPORT void run();
	cppevent_EXPORT void stop();

	cppevent_EXPORT EventLoopGroup& setHandler(bool shared, EventHandlerFactoryFunc* func);

	cppevent_EXPORT EventLoopGroup& acceptNumber(int number);
	cppevent_EXPORT EventLoopGroup& workerNumber(int number);

	cppevent_EXPORT EventLoopGroup& bind(const char *addr);

	cppevent_EXPORT EventLoopGroup& option(int flag, int val);

	cppevent_EXPORT std::future<std::shared_ptr<Conn>> connect(const char *addr);

private:
	EventLoop* getWorker();

private:
	std::vector<std::thread> threads_;
	std::vector<EventLoop*> workers_;
	std::vector<EventLoop*> acceptors_;

	int backlog_;

	int accept_thread_num_;
	int worker_thread_num_;
	bool is_shared_handler_;
	EventHandlerFactoryFunc *handler_factory_func_;
	std::string bind_addr_;

	int next_worker_;
};

NS_CPPEVENT_END

#endif