#pragma once
#include "InterfaceAuditor.h"

#define WAIVE(cond, note) \
if (!(cond)) \
{ \
ReportWaive(#cond, #note, __FILE__, __LINE__); \
num_waivers++;\
} \
else\
{\
	num_passes++;\
}



#define ASSERT(cond) \
if (!(cond)) \
{ \
ReportFailure(#cond, __FILE__, __LINE__); \
num_failures++;\
} \
else\
{\
	num_passes++;\
}

template <typename T>
inline void uninit(T &ret)
{
	memset(&ret, 7, sizeof(ret));
}

template <size_t count>
inline void uninit(char(&s)[count])
{
	s[0] = 3;
	s[1] = '\0';
}

inline void uninit(char *s, uint32_t asize)
{
	s[0] = 3;
	s[1] = '\0';
}

