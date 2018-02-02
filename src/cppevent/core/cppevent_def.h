#ifndef CPP_EVENT_DEF_H_
#define CPP_EVENT_DEF_H_

#include <functional>
#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#endif

// windows export macro
#if WIN32 && defined(cppevent_core_USE_DLL)
	#ifdef cppevent_core_EXPORTS
		#define cppevent_core_EXPORT __declspec(dllexport)
	#else
		#define cppevent_core_EXPORT __declspec(dllimport)
	#endif
#else
	#define cppevent_core_EXPORT
#endif

// namespace
#ifdef __cplusplus
	#define NS_CPPEVENT_BEGIN namespace cppevent {
	#define NS_CPPEVENT_END   }
	#define USING_NS_CPPEVENT using namespace cppevent
#else
	#define NS_CPPEVENT_BEGIN
	#define NS_CPPEVENT_END  
	#define USING_NS_CPPEVENT
#endif

#if WIN32
	#define cppevent_socket_t intptr_t
	#define cppevent_socklen_t int
#else
	#define cppevent_socket_t int
	#define cppevent_socklen_t socklen_t
#endif

enum
{
	CPPEVENT_READING   = 0x01, // BEV_EVENT_READING
	CPPEVENT_WRITING   = 0x02, // BEV_EVENT_WRITING
	CPPEVENT_EOF       = 0x10, // BEV_EVENT_EOF
	CPPEVENT_ERROR     = 0x20, // BEV_EVENT_ERROR
	CPPEVENT_TIMEOUT   = 0x40, // BEV_EVENT_TIMEOUT
	CPPEVENT_CONNECTED = 0x80, // BEV_EVENT_CONNECTED
};

typedef std::function<void(cppevent_socket_t, struct sockaddr_storage *, cppevent_socklen_t)> FuncAcceptCb;

#endif
