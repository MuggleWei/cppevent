#include "cppevent_endian.h"

static union
{
	char c[4];
	unsigned long l;
}endian_test = { { 1, 0, 0, 0 } };

#define CPPEVENT_IS_LITTLE_ENDIAN (endian_test.l == 1)

NS_CPPEVENT_BEGIN

bool isLittleEndian()
{
	return CPPEVENT_IS_LITTLE_ENDIAN;
}

NS_CPPEVENT_END