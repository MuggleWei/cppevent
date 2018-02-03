#include "protobuf_handler.h"

NS_CPPEVENT_BEGIN

void ProtobufHandler::connActive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "connected: " 
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr() << std::endl;
}
void ProtobufHandler::connInactive(std::shared_ptr<cppevent::Conn> &connptr)
{
	std::cout << "disconnected: "
		<< connptr->getLocalAddr() << " <--> " << connptr->getRemoteAddr() << std::endl;
}
void ProtobufHandler::connRead(std::shared_ptr<cppevent::Conn> &connptr)
{
	while (true)
	{
		size_t readable_len = connptr->getReadableLength();
		if (readable_len < 4)
		{
			break;
		}

		int32_t total_len = connptr->peekInt32();
		int32_t limited_len = codec_.getMaxTotalLen();
		if (limited_len != 0 && total_len > limited_len)
		{
			connptr->close();
			break;
		}
		if (total_len > (int32_t)readable_len)
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
			connptr->close();
			break;
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
