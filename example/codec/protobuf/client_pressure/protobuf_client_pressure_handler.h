#ifndef PROTOBUF_CLIENT_PRESSURE_HANDLER_H_
#define PROTOBUF_CLIENT_PRESSURE_HANDLER_H_

#include <vector>
#include <google/protobuf/message.h>
#include "cppevent/cppevent.h"

#include "gen/timerecord.pb.h"

typedef std::shared_ptr<cppevent::Conn> ConnPtr;

using namespace google::protobuf;
using namespace gen::proto;

class ProtobufClientPressureHandler : public cppevent::ProtobufHandler
{
public:
	static EventHandler* getHandler()
	{
		return new ProtobufClientPressureHandler();
	}

private:
	ProtobufClientPressureHandler();

	virtual void connActive(ConnPtr &connptr) override;
	virtual void connInactive(ConnPtr &connptr) override;

	void OnTimeRecord(ConnPtr &connptr, std::shared_ptr<TimeRecord> &msg);

private:
	std::vector<int64_t> elapseds;
};

#endif