#ifndef GAME_SERVER_HANDLER_H_
#define GAME_SERVER_HANDLER_H_

#include <google/protobuf/message.h>
#include "cppevent/cppevent.h"

#include "gen/gameobject.pb.h"

using namespace google::protobuf;
using namespace gen::proto;

typedef std::shared_ptr<cppevent::Conn> ConnPtr;

class ProtobufServerHandler : public cppevent::EventHandler
{
public:
	static ProtobufServerHandler s_game_server;
	static EventHandler* getHandler()
	{
		return &s_game_server;
	}

public:
	ProtobufServerHandler();

	void registers();

	virtual void connRead(std::shared_ptr<cppevent::Conn> &connptr) override;

	void OnMessage(ConnPtr &connptr, std::shared_ptr<Message> &msg);
	void OnMessagePosition(ConnPtr &connptr, std::shared_ptr<Position> &msg);
	void OnMessageRotation(ConnPtr &connptr, std::shared_ptr<Rotation> &msg);
	void OnMessageTransform(ConnPtr &connptr, std::shared_ptr<Transform> &msg);

private:
	cppevent::ProtobufDispatch dispatch_;
	cppevent::ProtobufCodec codec_;
};


#endif