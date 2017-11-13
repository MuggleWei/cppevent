#ifndef CPP_EVENT_TCP_PEER_H_
#define CPP_EVENT_TCP_PEER_H_

#if WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include "cppevent/cppevent_def.h"

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
	cppevent_EXPORT int WriteAndFlush(void *data_out, size_t datalen);
	
	cppevent_EXPORT const char* getLocalAddr() { return local_addr_; }
	cppevent_EXPORT const char* getRemoteAddr() { return remote_addr_; }

	cppevent_EXPORT WorkerThread* getOwnerThread() { return owner_thread_; }

public:
	void *extra_;

private:
	WorkerThread *owner_thread_;
	void *bev_;

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