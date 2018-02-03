#ifndef CPPEVENT_PROTOBUF_DISPATCH_H_
#define CPPEVENT_PROTOBUF_DISPATCH_H_

#include "cppevent/core/cppevent_def.h"
#include <map>
#include <memory>
#include <functional>
#include <google/protobuf/message.h>
#include "cppevent/core/conn.h"
#include "cppevent/extra/cppevent_extra_def.h"

NS_CPPEVENT_BEGIN

class ProtobufDispatch
{
	class MessageCallback
	{
	public:
		virtual void OnMessage(std::shared_ptr<cppevent::Conn> &connptr, std::shared_ptr<google::protobuf::Message> &msg) {}
	};

	template<typename T>
	class TMessageCallback : public MessageCallback
	{
		typedef std::function<void(std::shared_ptr<cppevent::Conn>&, std::shared_ptr<T>&)> callbackFunc;

	public:
		TMessageCallback(const callbackFunc& callback)
			: MessageCallback()
			, callback_(callback)
		{}

		virtual void OnMessage(std::shared_ptr<cppevent::Conn> &connptr, std::shared_ptr<google::protobuf::Message> &msg)
		{
			std::shared_ptr<T> derived_msg = std::dynamic_pointer_cast<T>(msg);
			if (derived_msg && callback_)
			{
				callback_(connptr, derived_msg);
			}
		}

	private:
		callbackFunc callback_;
	};

public:
	ProtobufDispatch()
		: default_callback_(nullptr)
	{}
	~ProtobufDispatch()
	{
		if (default_callback_)
		{
			delete default_callback_;
		}

		for (auto it = callbacks_.begin(); it != callbacks_.end(); ++it)
		{
			delete it->second;
		}
	}

	template<typename T>
	void RegisterDefault(const std::function<void(std::shared_ptr<cppevent::Conn> &connptr, std::shared_ptr<T>&)>& callback)
	{
		if (default_callback_)
		{
			delete(default_callback_);
		}
		default_callback_ = new TMessageCallback<T>(callback);
	}

	template<typename T>
	void Register(const std::function<void(std::shared_ptr<cppevent::Conn> &connptr, std::shared_ptr<T>&)>& callback)
	{
		TMessageCallback<T> *tcallback = new TMessageCallback<T>(callback);
		std::string msg_type_name = T::descriptor()->full_name();
		callbacks_.insert(std::pair<std::string, MessageCallback*>(msg_type_name, tcallback));
	}

	template<typename T>
	void OnMessage(std::shared_ptr<cppevent::Conn> &connptr, std::shared_ptr<T>& msg)
	{
		auto it = callbacks_.find(msg->GetTypeName());
		if (it != callbacks_.end())
		{
			it->second->OnMessage(connptr, msg);
		}
		else
		{
			if (default_callback_)
			{
				default_callback_->OnMessage(connptr, msg);
			}
		}
	}

private:
	std::map<std::string, MessageCallback*> callbacks_;
	MessageCallback *default_callback_;
};

NS_CPPEVENT_END

#endif
