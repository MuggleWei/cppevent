#include "protobuf_codec.h"
#include <string.h>
#include <google/protobuf/io/coded_stream.h>
#include "cppevent/core/cppevent_endian.h"

NS_CPPEVENT_BEGIN

static google::protobuf::Message* CreateProtoMessage(const std::string &message_name)
{
	auto gen_pool = google::protobuf::DescriptorPool::generated_pool();
	const google::protobuf::Descriptor *desc = gen_pool->FindMessageTypeByName(message_name);
	if (desc == nullptr)
	{
		return nullptr;
	}
	return google::protobuf::MessageFactory::generated_factory()->GetPrototype(desc)->New();
}

ProtobufCodec::ProtobufCodec(int32_t max_total_len, int32_t max_name_len)
	: max_total_len_(max_total_len)
	, max_name_len_(max_name_len)
{
	if (max_total_len_ < 0)
	{
		max_total_len_ = 0;
	}
	if (max_name_len_ < 0)
	{
		max_name_len_ = 0;
	}
}

/*
 |          int32_t               |     int32_t    |   char*   |    void*   |
 |   total_len(include itself)    |    name_len    |   name    |    data    |
 */

char* ProtobufCodec::code(const google::protobuf::Message *msg, int32_t &total_len)
{
	
	size_t size = msg->ByteSizeLong();
	const std::string &type_name = msg->GetDescriptor()->full_name();
	total_len = (int32_t)(sizeof(int32_t) + sizeof(int32_t) + type_name.size() + size);

	void* bytes = malloc(total_len);
	if (!bytes)
	{
		return nullptr;
	}
	char *p = (char*)bytes;

	// total len
	*((int32_t*)p) = CPPEVENT_HTON_32(total_len);
	p += sizeof(int32_t);

	// name len
	*((int32_t*)p) = CPPEVENT_HTON_32((int32_t)type_name.size());
	p += sizeof(int32_t);

	// name
	memcpy(p, type_name.c_str(), type_name.size());
	p += (type_name.size());

	// data
	if (!msg->SerializeToArray(p, (int)size))
	{
		free(bytes);
		bytes = nullptr;
	}
	return (char*)bytes;
}

google::protobuf::Message* ProtobufCodec::decode(const char *bytes, int32_t &total_len, eDecodeError &err)
{
	const char *p = bytes;

	// total len
	total_len = CPPEVENT_NTOH_32(*(int32_t*)p);
	if (total_len <= 0 || (max_total_len_ != 0 && total_len > max_total_len_))
	{
		err = eDecodeError::DecodeError_BeyondLenLimit;
		return nullptr;
	}
	p += (int32_t)sizeof(int32_t);

	// name len
	int32_t name_len = CPPEVENT_NTOH_32(*(int32_t*)p);
	if (name_len <= 0 || (max_name_len_ != 0 && name_len > max_name_len_))
	{
		err = eDecodeError::DecodeError_BeyondNameLenLimit;
		return nullptr;
	}
	if (name_len > total_len - sizeof(int32_t) * 2)
	{
		err = eDecodeError::DecodeError_WrongLen;
		return nullptr;
	}
	p += sizeof(int32_t);

	// name
	char *name = (char*)malloc(name_len + 1);
	memcpy(name, p, name_len);
	name[name_len] = '\0';

	google::protobuf::Message *msg = CreateProtoMessage(name);
	free(name);
	if (msg == nullptr)
	{
		err = eDecodeError::DecodeError_FailedCreateMessage;
		return nullptr;
	}
	p += name_len;

	// data
	int size = total_len - sizeof(int32_t) - sizeof(int32_t) - name_len;
	if (size > 0)
	{
		try
		{
			if (!msg->ParseFromArray(p, size))
			{
				delete msg;
				err = eDecodeError::DecodeError_WrongMessage;
				return nullptr;
			}
		}
		catch (const std::exception &)
		{
			delete msg;
			err = eDecodeError::DecodeError_WrongMessage;
			return nullptr;
		}
	}

	return msg;
}

int32_t ProtobufCodec::getMaxTotalLen()
{
	return max_total_len_;
}
int32_t ProtobufCodec::getMaxNameLen()
{
	return max_name_len_;
}

NS_CPPEVENT_END