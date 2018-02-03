#include <signal.h>
#include <memory>
#include <google/protobuf/message.h>
#include "cppevent/cppevent.h"
#include "gen/gameobject.pb.h"

using namespace google::protobuf;
using namespace gen::proto;

typedef std::shared_ptr<cppevent::Conn> ConnPtr;

cppevent::EventLoopGroup *p_event_loop_group = nullptr;
void sighandler(int signum)
{
	switch (signum)
	{
	case SIGINT:
	{
		std::cout << "\nRecieve signal: SIGINT" << std::endl;
		if (p_event_loop_group)
		{
			p_event_loop_group->stop();
		}
	}break;
	}
}

static cppevent::ProtobufHandler handler;
cppevent::EventHandler* getProtobufHandler()
{
	return &handler;
}

void OnMessage(ConnPtr &connptr, std::shared_ptr<Message> &msg)
{
	std::cout << std::endl << "========" << std::endl;
	std::cout << "Receive unregistered message: " << msg->GetTypeName() << std::endl;
}
void OnPosition(ConnPtr &connptr, std::shared_ptr<Position> &msg)
{
	std::cout << std::endl << "========" << std::endl;
	std::cout << "OnMessagePosition: " << std::endl
		<< msg->x() << ", "
		<< msg->y() << ", "
		<< msg->z() << std::endl;
}
void OnRotation(ConnPtr &connptr, std::shared_ptr<Rotation> &msg)
{
	std::cout << std::endl << "========" << std::endl;
	std::cout << "OnMessageRotation: " << std::endl
		<< msg->x_axis() << ", "
		<< msg->y_axis() << ", "
		<< msg->z_axis() << std::endl;
}
void OnTransform(ConnPtr &connptr, std::shared_ptr<Transform> &msg)
{
	std::cout << std::endl << "========" << std::endl;
	std::cout << "OnMessageTransform: " << std::endl
		<< msg->position().x() << ", "
		<< msg->position().y() << ", "
		<< msg->position().z() << std::endl
		<< msg->rotation().x_axis() << ", "
		<< msg->rotation().y_axis() << ", "
		<< msg->rotation().z_axis() << std::endl
		<< msg->scale().x_scale() << ", "
		<< msg->scale().y_scale() << ", "
		<< msg->scale().z_scale() << std::endl;

	handler.write(connptr, msg.get());
}

int main(int argc, char *argv[])
{
	cppevent::EventLoopGroup event_loop_group;

	p_event_loop_group = &event_loop_group;
	signal(SIGINT, sighandler);

	handler.defaultHandleFunc<Message>(CPPEVENT_PROTOBUF_HANDLE_FUNC(OnMessage));
	handler.handleFunc<Position>(CPPEVENT_PROTOBUF_HANDLE_FUNC(OnPosition));
	handler.handleFunc<Rotation>(CPPEVENT_PROTOBUF_HANDLE_FUNC(OnRotation));
	handler.handleFunc<Transform>(CPPEVENT_PROTOBUF_HANDLE_FUNC(OnTransform));

	event_loop_group
		.acceptNumber(1)
		.workerNumber(4)
		.setHandler(true, getProtobufHandler)
		.idleTimeout(5)
		.option(cppevent::CPPEVENT_BACKLOG, 512);

	if (argc == 2)
	{
		event_loop_group.bind(argv[1]);
	}
	else
	{
		event_loop_group.bind("127.0.0.1:10102");
	}

	event_loop_group.run();

	cppevent::EventLoop::GlobalClean();

	return 0;
}