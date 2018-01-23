#ifndef CPP_EVENT_BYTE_BUF_H_
#define CPP_EVENT_BYTE_BUF_H_

#include "cppevent/cppevent_def.h"


NS_CPPEVENT_BEGIN

class ByteBuffer
{
public:
	cppevent_EXPORT ByteBuffer();
	cppevent_EXPORT virtual ~ByteBuffer();

	cppevent_EXPORT void* getEvbuf();
	cppevent_EXPORT void setEvbuf(void *evbuf);
	
	/*
	 * Returns the total number of bytes stored in the buffer
	 * @return the number of bytes stored in the peer's buffer
	 */
	cppevent_EXPORT size_t GetByteLength();

	/*
	 * Read data from peer, and leave the buffer unchanged
	 * @return the number of bytes read, or -1 if we can't drain the buffer.
	 */
	cppevent_EXPORT size_t PeekBytes(void *data_out, size_t datalen);

	/*
	 * Read data from peer, draining the bytes from the source buffer
	 * @return the number of bytes read, or -1 if we can't drain the buffer.
	 */
	cppevent_EXPORT size_t ReadBytes(void *data_out, size_t datalen);

	/*
	 * Write data into peer, if invoke thread is the same thread
	 * as the peer owner thread (e.g. in the handler callback),
	 * then it's safe to write. otherwise, user need ensure thread
	 * safe by user self
	 * @return 0 if successful, or -1 if an error occurred
	 */
	cppevent_EXPORT int Write(void *data_out, size_t datalen);

	/*
	 * Move all data from anthoer ByteBuffer into this
	 * @return 0 if successful, or -1 if an error occurred
	 */
	cppevent_EXPORT int Append(ByteBuffer& buf);

	/*
	 * Move data from anthoer ByteBuffer into this
	 * @return the number of bytes read
	 */
	cppevent_EXPORT int Append(ByteBuffer& buf, size_t datalen);

	/*
	 * Copy data from anthoer ByteBuffer into this
	 * @return 0 if successful, or -1 if an error occurred
	 */
	cppevent_EXPORT int AppendRef(ByteBuffer& buf);

private:
	void *ev_buf_;
};

NS_CPPEVENT_END


#endif