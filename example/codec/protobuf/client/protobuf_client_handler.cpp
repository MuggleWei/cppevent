#include "protobuf_client_handler.h"

using namespace google::protobuf;
using namespace gen::proto;

ProtobufClientHandler::ProtobufClientHandler()
{}

void ProtobufClientHandler::connActive(ConnPtr &connptr)
{
	connptr->getLoop()->addTimer(2000, [this, connptr]() mutable {
		int32_t len;
		char *bytes = nullptr;

		Transform transform;
		Position &position = *transform.mutable_position();
		Rotation &rotation = *transform.mutable_rotation();
		Scale &scale = *transform.mutable_scale();

		position.set_x(1.0f);
		position.set_y(0.0f);
		position.set_z(0.0f);
		bytes = codec_.code(&position, len);
		connptr->write(bytes, (size_t)len);
		free(bytes);

		rotation.set_x_axis(0.0f);
		rotation.set_y_axis(1.57f);
		rotation.set_z_axis(0.0f);
		bytes = codec_.code(&rotation, len);
		connptr->write(bytes, (size_t)len);
		free(bytes);

		scale.set_x_scale(1.0f);
		scale.set_y_scale(1.0f);
		scale.set_z_scale(1.0f);
		bytes = codec_.code(&scale, len);
		connptr->write(bytes, (size_t)len);
		free(bytes);

		bytes = codec_.code(&transform, len);
		connptr->write(bytes, (size_t)len);
		free(bytes);
	});
}
void ProtobufClientHandler::connInactive(ConnPtr &connptr)
{}