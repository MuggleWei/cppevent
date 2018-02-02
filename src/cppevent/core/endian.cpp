#include "endian.h"

NS_CPPEVENT_BEGIN

union endianUnion
{
	int a;
	char b;
};

bool isLittleEndian()
{
	endianUnion eu;
	eu.a = 1;
	return eu.b == 1;
}

NS_CPPEVENT_END