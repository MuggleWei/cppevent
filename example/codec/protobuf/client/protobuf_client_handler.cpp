#include "protobuf_client_handler.h"
#include <iostream>

ProtobufClientHandler::ProtobufClientHandler()
	: cppevent::ProtobufHandler(102400, 128)
{
	handleFunc<Transform>(CPPEVENT_PROTOBUF_HANDLE_MEMFUNC(&ProtobufClientHandler::OnTransform, this));
}

void ProtobufClientHandler::connActive(ConnPtr &connptr)
{
	std::cout << std::endl << "========" << std::endl;
	std::cout << "connected: "
		<< connptr->getLocalAddr() << " <-->" << connptr->getRemoteAddr()
		<< std::endl;

	connptr->getLoop()->addTimer(2000, [this, connptr]() mutable {
		Transform transform;
		Position &position = *transform.mutable_position();
		Rotation &rotation = *transform.mutable_rotation();
		Scale &scale = *transform.mutable_scale();

		position.set_x(1.0f);
		position.set_y(0.0f);
		position.set_z(0.0f);
		write(connptr, &position);

		rotation.set_x_axis(0.0f);
		rotation.set_y_axis(1.57f);
		rotation.set_z_axis(0.0f);
		write(connptr, &rotation);

		scale.set_x_scale(1.0f);
		scale.set_y_scale(1.0f);
		scale.set_z_scale(1.0f);
		write(connptr, &scale);

		write(connptr, &transform);
	});
}
void ProtobufClientHandler::connInactive(ConnPtr &connptr)
{
	connptr->getLoop()->stop();
}

void ProtobufClientHandler::OnTransform(ConnPtr &connptr, std::shared_ptr<Transform> &msg)
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
