#ifndef CPPEVENT_PROTOBUF_HANDLER_H_
#define CPPEVENT_PROTOBUF_HANDLER_H_

#include "cppevent/core/cppevent_def.h"

#include <google/protobuf/message.h>
#include "cppevent/extra/cppevent_extra_def.h"
#include "cppevent/extra/codec/protobuf_codec.h"
#include "cppevent/extra/codec/protobuf_dispatch.h"

#define CPPEVENT_PROTOBUF_HANDLE_FUNC(func) \
std::bind(func, std::placeholders::_1, std::placeholders::_2)

#define CPPEVENT_PROTOBUF_HANDLE_MEMFUNC(func, ptr) \
std::bind(func, ptr, std::placeholders::_1, std::placeholders::_2)

NS_CPPEVENT_BEGIN

class ProtobufHandler : public cppevent::EventHandler
{
public:
	cppevent_extra_EXPORT ProtobufHandler(int32_t max_total_len = 0, int32_t max_name_len = 0)
		: codec_(max_total_len, max_name_len)
	{}
	cppevent_extra_EXPORT virtual ~ProtobufHandler() {}

	cppevent_extra_EXPORT virtual void connActive(std::shared_ptr<cppevent::Conn> &connptr) override;
	cppevent_extra_EXPORT virtual void connInactive(std::shared_ptr<cppevent::Conn> &connptr) override;
	cppevent_extra_EXPORT virtual void connRead(std::shared_ptr<cppevent::Conn> &connptr) override;

	cppevent_extra_EXPORT void write(std::shared_ptr<cppevent::Conn> &connptr, const google::protobuf::Message *message);

	template<typename T>
	void handleFunc(std::function<void(std::shared_ptr<cppevent::Conn> &connptr, std::shared_ptr<T>&)>&& callback)
	{
		dispatch_.Register<T>(std::move(callback));
	}

	template<typename T>
	void defaultHandleFunc(std::function<void(std::shared_ptr<cppevent::Conn> &connptr, std::shared_ptr<T>&)>&& callback)
	{
		dispatch_.RegisterDefault(std::move(callback));
	}

protected:
	ProtobufDispatch dispatch_;
	ProtobufCodec codec_;
};

NS_CPPEVENT_END

#endif
