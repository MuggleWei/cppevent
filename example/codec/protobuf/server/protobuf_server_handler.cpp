#include "protobuf_server_handler.h"

ProtobufServerHandler ProtobufServerHandler::s_game_server;

ProtobufServerHandler::ProtobufServerHandler()
{
	registers();
}

void ProtobufServerHandler::registers()
{
	dispatch_.RegisterDefault<google::protobuf::Message>(std::bind(&ProtobufServerHandler::OnMessage, this, std::placeholders::_1, std::placeholders::_2));

	dispatch_.Register<Position>(std::bind(&ProtobufServerHandler::OnMessagePosition, this, std::placeholders::_1, std::placeholders::_2));
	dispatch_.Register<Rotation>(std::bind(&ProtobufServerHandler::OnMessageRotation, this, std::placeholders::_1, std::placeholders::_2));
	dispatch_.Register<Transform>(std::bind(&ProtobufServerHandler::OnMessageTransform, this, std::placeholders::_1, std::placeholders::_2));
}

void ProtobufServerHandler::connRead(std::shared_ptr<cppevent::Conn> &connptr)
{
	while (true)
	{
		int32_t total_len = connptr->peekInt32();
		if (total_len > connptr->getReadableLength())
		{
			break;
		}

		char *byte = (char*)malloc(total_len);
		connptr->readBytes(byte, total_len);
		cppevent::ProtobufCodec::eDecodeError err;
		auto message = codec_.decode(byte, total_len, err);
		free(byte);
		if (message == nullptr)
		{
			std::cerr << "error decode: " << err << std::endl;
		}

		std::shared_ptr<google::protobuf::Message> msg(message);
		dispatch_.OnMessage(connptr, msg);
	}
}

void ProtobufServerHandler::OnMessage(ConnPtr &connptr, std::shared_ptr<Message> &msg)
{
	std::cout << std::endl << "========" << std::endl;
	std::cout << "Receive unregistered message: " << msg->GetTypeName() << std::endl;
}

void ProtobufServerHandler::OnMessagePosition(ConnPtr &connptr, std::shared_ptr<Position> &msg)
{
	std::cout << std::endl << "========" << std::endl;
	std::cout << "OnMessagePosition: " << std::endl
		<< msg->x() << ", "
		<< msg->y() << ", "
		<< msg->z() << std::endl;
}
void ProtobufServerHandler::OnMessageRotation(ConnPtr &connptr, std::shared_ptr<Rotation> &msg)
{
	std::cout << std::endl << "========" << std::endl;
	std::cout << "OnMessageRotation: " << std::endl
		<< msg->x_axis() << ", "
		<< msg->y_axis() << ", "
		<< msg->z_axis() << std::endl;
}
void ProtobufServerHandler::OnMessageTransform(ConnPtr &connptr, std::shared_ptr<Transform> &msg)
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
}
