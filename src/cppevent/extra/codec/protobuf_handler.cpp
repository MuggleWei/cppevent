#include "protobuf_handler.h"

NS_CPPEVENT_BEGIN

void ProtobufHandler::connRead(std::shared_ptr<cppevent::Conn> &connptr)
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

void ProtobufHandler::write(std::shared_ptr<cppevent::Conn> &connptr, const google::protobuf::Message *message)
{
	int32_t len;
	char *bytes = codec_.code(message, len);
	connptr->write(bytes, (size_t)len);
	free(bytes);
}


NS_CPPEVENT_END