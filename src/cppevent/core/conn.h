#ifndef CPP_EVENT_CONN_H_
#define CPP_EVENT_CONN_H_

#include "cppevent/core/cppevent_def.h"
#include <memory>
#include "cppevent/core/byte_buf.h"

NS_CPPEVENT_BEGIN

class EventLoop;
class Conn;
struct ConnContainer;
class EventHandler;

class Conn
{
public:
	friend class EventLoop;

public:
	cppevent_core_EXPORT Conn();
	cppevent_core_EXPORT ~Conn();

	cppevent_core_EXPORT const char* getLocalAddr() { return local_addr_; }
	cppevent_core_EXPORT const char* getRemoteAddr() { return remote_addr_; }

	cppevent_core_EXPORT ByteBuffer& getInputByteBuf() { return byte_buf_in_; }
	cppevent_core_EXPORT ByteBuffer& getOutputByteBuf() { return byte_buf_out_; }

	cppevent_core_EXPORT EventHandler* getHandler();
	cppevent_core_EXPORT EventLoop* getLoop();

	cppevent_core_EXPORT void close();

	/*
	 * get readable byte length in input byte buffer
	 */
	cppevent_core_EXPORT size_t getReadableLength();
	/*
	 * get datalen bytes in input byte buffer and leave the buffer unchanged
	 * @data_out: the destination byte array to store the result
	 * @data_len: the maximum size of the destination byte array
	 * @RETURN: the number of bytes read, or -1 represent failed
	 */
	cppevent_core_EXPORT size_t peekBytes(void *data_out, size_t datalen);
	/*
	 * get datalen bytes in input byte buffer and draining the bytes from the source buffer
	 * @data_out: the destination byte array to store the result
	 * @data_len: the maximum size of the destination byte array
	 * @RETURN: the number of bytes read, or -1 represent failed
	 */
	cppevent_core_EXPORT size_t readBytes(void *data_out, size_t datalen);
	
	/*
	 * write data into output byte buffer
	 * @data_out: the byte array
	 * @datalen: the number of bytes to be copied into output byte buffer
	 * @RETURN: 0 if successful, or -1 if an error occurred
	 */
	cppevent_core_EXPORT int write(void *data_out, size_t datalen);
	/*
	 * Move data from anthoer ByteBuffer into output byte buffer of this conn
	 * @buf: the source byte buffer
	 * @RETURN: 0 if successful, or -1 if an error occurred
	 */
	cppevent_core_EXPORT int write(ByteBuffer& buf);
	/*
	 * Move datalen number of bytes from anthoer ByteBuffer into output byte buffer of this conn
	 * @buf: the source byte buffer
	 * @datalen: the number of bytes want to move
	 * @RETURN: 0 if successful, or -1 if an error occurred
	 */
	cppevent_core_EXPORT int write(ByteBuffer& buf, size_t datalen);
	cppevent_core_EXPORT int writeAndClose(void *data_out, size_t datalen);
	cppevent_core_EXPORT int writeAndClose(ByteBuffer& buf);
	cppevent_core_EXPORT int writeAndClose(ByteBuffer& buf, size_t datalen);

	cppevent_core_EXPORT int16_t peekInt16();
	cppevent_core_EXPORT int32_t peekInt32();
	cppevent_core_EXPORT int64_t peekInt64();
	cppevent_core_EXPORT float peekFloat();
	cppevent_core_EXPORT double peekDouble();
	cppevent_core_EXPORT int16_t readInt16();
	cppevent_core_EXPORT int32_t readInt32();
	cppevent_core_EXPORT int64_t readInt64();
	cppevent_core_EXPORT float readFloat();
	cppevent_core_EXPORT double readDouble();

	/*
	 *  @RETURN: number of byte that write into bytebuf
	 */
	cppevent_core_EXPORT int writeInt16(int16_t val);
	cppevent_core_EXPORT int writeInt32(int32_t val);
	cppevent_core_EXPORT int writeInt64(int64_t val);
	cppevent_core_EXPORT int writeFloat(float val);
	cppevent_core_EXPORT int writeDouble(double val);

	cppevent_core_EXPORT void afterRead();
	cppevent_core_EXPORT void afterWrite();

	cppevent_core_EXPORT void updateLastInTime();
	cppevent_core_EXPORT void updateLastOutTime();
	cppevent_core_EXPORT long getLastInTime();
	cppevent_core_EXPORT long getLastOutTime();
	cppevent_core_EXPORT long getLastActiveTime();

protected:
	void setBev(void *bev);

protected:
	ConnContainer *container_;

	EventLoop *event_loop_;
	void *bev_;

	ByteBuffer byte_buf_in_;
	ByteBuffer byte_buf_out_;

	char local_addr_[128];
	char remote_addr_[128];

	EventHandler *handler_;
	bool shared_handler_;

	bool wait_close_;
	long last_in_time_;
	long last_out_time_;
};

// for shared ptr Conn compatible with c callback function
struct ConnContainer
{
	std::shared_ptr<Conn> connptr;
};

NS_CPPEVENT_END

#endif
