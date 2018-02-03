#ifndef GAME_CLIENT_HANDLER_H_
#define GAME_CLIENT_HANDLER_H_

#include <google/protobuf/message.h>
#include "cppevent/cppevent.h"

#include "gen/gameobject.pb.h"

typedef std::shared_ptr<cppevent::Conn> ConnPtr;

using namespace google::protobuf;
using namespace gen::proto;

class ProtobufClientHandler : public cppevent::ProtobufHandler
{
public:
	static EventHandler* getHandler()
	{
		return new ProtobufClientHandler();
	}

public:
	ProtobufClientHandler();

	virtual void connActive(ConnPtr &connptr) override;
	virtual void connInactive(ConnPtr &connptr) override;

	void OnTransform(ConnPtr &connptr, std::shared_ptr<Transform> &msg);
};



#endif