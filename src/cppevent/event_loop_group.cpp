#include "event_loop_group.h"
#include <functional>

NS_CPPEVENT_BEGIN

EventLoopGroup::EventLoopGroup()
	: backlog_(128)
	, accept_thread_num_(1)
	, worker_thread_num_(0)
	, is_shared_handler_(true)
	, handler_factory_func_(nullptr)
	, next_worker_(0)
{}
EventLoopGroup::~EventLoopGroup()
{
	for (EventLoop* &event_loop : workers_)
	{
		delete event_loop;
	}

	for (EventLoop* &event_loop : acceptors_)
	{
		delete event_loop;
	}
}

void EventLoopGroup::run()
{
	if (accept_thread_num_ <= 0 && worker_thread_num_ <= 0)
	{
		accept_thread_num_ = 1;
		worker_thread_num_ = 0;
	}

	for (int i = 0; i < accept_thread_num_; ++i)
	{
		cppevent::GetAcceptEventLoopFunc func = [this]() mutable -> cppevent::EventLoop* {
			return getWorker();
		};
		EventLoop *event_loop = new EventLoop();
		event_loop->bindAndListen(bind_addr_.c_str(), backlog_);
		event_loop->setHandler(is_shared_handler_, handler_factory_func_);
		event_loop->setAcceptFunc(func);

		acceptors_.push_back(event_loop);
		threads_.push_back(std::thread([event_loop, this, func]() mutable {
			event_loop->run();
		}));
	}

	for (int i = 0; i < worker_thread_num_; ++i)
	{
		EventLoop *event_loop = new EventLoop();
		event_loop->setHandler(is_shared_handler_, handler_factory_func_);

		workers_.push_back(event_loop);
		threads_.push_back(std::thread([event_loop]() mutable {
			event_loop->run();
		}));
	}

	for (size_t i = 0; i < threads_.size(); ++i)
	{
		threads_[i].join();
	}
}
void EventLoopGroup::stop()
{
	for (int i = 0; i < accept_thread_num_; ++i)
	{
		acceptors_[i]->stop();
	}
	for (int i = 0; i < worker_thread_num_; ++i)
	{
		workers_[i]->stop();
	}
}

EventLoopGroup& EventLoopGroup::setHandler(bool shared, EventHandlerFactoryFunc* func)
{
	is_shared_handler_ = shared;
	handler_factory_func_ = func;
	return *this;
}

EventLoopGroup& EventLoopGroup::acceptNumber(int number)
{
	accept_thread_num_ = number;
	return *this;
}
EventLoopGroup& EventLoopGroup::workerNumber(int number)
{
	worker_thread_num_ = number;
	return *this;
}

EventLoopGroup& EventLoopGroup::bind(const char *addr)
{
	bind_addr_ = addr;
	return *this;
}

EventLoopGroup& EventLoopGroup::option(int flag, int val)
{
	switch (flag)
	{
	case CPPEVENT_BACKLOG:
	{
		backlog_ = val;
	}break;
	default:
		break;
	}
	return *this;
}

std::future<std::shared_ptr<Conn>> EventLoopGroup::connect(const char *addr)
{
	EventLoop *event_loop = getWorker();
	if (event_loop)
	{
		return event_loop->addConn(addr);
	}

	std::promise<std::shared_ptr<Conn>> promise;
	std::future<std::shared_ptr<Conn>> future = promise.get_future();
	promise.set_value(nullptr);
	return future;
}

EventLoop* EventLoopGroup::getWorker()
{
	EventLoop *event_loop = nullptr;
	if (worker_thread_num_ > 0)
	{
		int pos = next_worker_++;
		if (next_worker_ >= worker_thread_num_)
		{
			next_worker_ %= worker_thread_num_;
		}

		if (pos >= worker_thread_num_ || pos < 0)
		{
			pos = 0;
		}
		event_loop = workers_[pos];
	}
	else if (accept_thread_num_ > 0)
	{
		int pos = next_worker_++;
		if (next_worker_ >= accept_thread_num_)
		{
			next_worker_ %= accept_thread_num_;
		}

		if (pos >= accept_thread_num_ || pos < 0)
		{
			pos = 0;
		}
		event_loop = workers_[pos];
	}

	return event_loop;
}

NS_CPPEVENT_END