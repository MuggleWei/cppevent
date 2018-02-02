#ifndef CPPEVENT_ENDIAN_H_
#define CPPEVENT_ENDIAN_H_

#include "cppevent_def.h"

// static union
// {
// 	char c[4];
// 	unsigned long l;
// }endian_test = { { 1, 0, 0, 0 } };
// #define CPPEVENT_IS_LITTLE_ENDIAN ((char)endian_test.l)

#define CPPEVENT_IS_LITTLE_ENDIAN 1

#define CPPEVENT_REVERSE_ENDIAN_16(val) \
((((val) & 0x00FF) << 8) | \
(((val) & 0xFF00) >> 8))

#define CPPEVENT_REVERSE_ENDIAN_32(val) \
((((val) & 0x000000FF) << 24) | \
(((val) & 0x0000FF00) << 8)  | \
(((val) & 0x00FF0000) >> 8)  | \
(((val) & 0xFF000000) >> 24))

#define CPPEVENT_REVERSE_ENDIAN_64(val) \
((((val) & 0x00000000000000FF) << 56) | \
(((val) & 0x000000000000FF00) << 40) | \
(((val) & 0x0000000000FF0000) << 24) | \
(((val) & 0x00000000FF000000) << 8)  | \
(((val) & 0x000000FF00000000) >> 8)  | \
(((val) & 0x0000FF0000000000) >> 24) | \
(((val) & 0x00FF000000000000) >> 40) | \
(((val) & 0xFF00000000000000) >> 56))

#define CPPEVENT_HTON_16(val) \
(CPPEVENT_IS_LITTLE_ENDIAN ? CPPEVENT_REVERSE_ENDIAN_16(val) : (val))

#define CPPEVENT_HTON_32(val) \
(CPPEVENT_IS_LITTLE_ENDIAN ? CPPEVENT_REVERSE_ENDIAN_32(val) : (val))

#define CPPEVENT_HTON_64(val) \
(CPPEVENT_IS_LITTLE_ENDIAN ? CPPEVENT_REVERSE_ENDIAN_64(val) : (val))

#define CPPEVENT_NTOH_16(val) CPPEVENT_HTON_16(val)
#define CPPEVENT_NTOH_32(val) CPPEVENT_HTON_32(val)
#define CPPEVENT_NTOH_64(val) CPPEVENT_HTON_64(val)

#endif