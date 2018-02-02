#ifndef CPPEVENT_EXTRA_DEF_H_
#define CPPEVENT_EXTRA_DEF_H_

#include "cppevent/core/cppevent_core.h"

// windows export macro
#if WIN32 && defined(cppevent_extra_USE_DLL)
	#ifdef cppevent_extra_EXPORTS
		#define cppevent_extra_EXPORT __declspec(dllexport)
	#else
		#define cppevent_extra_EXPORT __declspec(dllimport)
	#endif
#else
	#define cppevent_extra_EXPORT
#endif

#endif