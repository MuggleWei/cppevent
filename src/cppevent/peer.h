#ifndef CPP_EVENT_TCP_PEER_H_
#define CPP_EVENT_TCP_PEER_H_

#if WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include "cppevent/cppevent_def.h"
#include "cppevent/byte_buf.h"

NS_CPPEVENT_BEGIN

enum ePeerFlag
{
	PEER_FLAG_CLOSE = 0x01,
	PEER_FLAG_DONT_ERASE = 0x02,
};

class Peer
{
	friend class WorkerThread;

public:
	cppevent_EXPORT Peer();
	cppevent_EXPORT virtual ~Peer();

	cppevent_EXPORT void Close();

	/*
	 * flush output data out
	 * @return 0 if successful, or -1 if an error occurred
	 */
	cppevent_EXPORT int FlushOutput();
	
	cppevent_EXPORT const char* getLocalAddr() { return local_addr_; }
	cppevent_EXPORT const char* getRemoteAddr() { return remote_addr_; }

	cppevent_EXPORT WorkerThread* getOwnerThread() { return owner_thread_; }

	cppevent_EXPORT ByteBuffer& getInputByteBuf() { return byte_buf_in_; }
	cppevent_EXPORT ByteBuffer& getOutputByteBuf() { return byte_buf_out_; }

private:
	void setBev(void *bev);

public:
	void *extra_;

private:
	WorkerThread *owner_thread_;
	void *bev_;
	ByteBuffer byte_buf_in_;
	ByteBuffer byte_buf_out_;

	char local_addr_[128];
	char remote_addr_[128];
	struct sockaddr_storage remote_ss_;
	int remote_ss_len_;
	struct timeval first_connect_time_;

	int flag_;
};

typedef std::function<void(Peer*)> FuncConnectCallback;

NS_CPPEVENT_END

#endif