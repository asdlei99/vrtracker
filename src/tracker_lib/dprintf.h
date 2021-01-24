#pragma once
#include <openvr_string_std.h>
#include <openvr_operators.h>

void dprintf(const char *fmt, ...);

inline void log_entry(const char *fn)
{
	dprintf("> %s\n", fn);
}

inline void log_entry(const char *fn, const char *param)
{
	dprintf("> %s %s\n", fn, param);
}

inline void log_entry(const char *fn, vr::EVRApplicationType app_type)
{
	dprintf("> %s %s\n", fn, openvr_string::EVRApplicationTypeToString(app_type));
}

inline void log_entry(const char *fn, int d)
{
	dprintf("> %s %d\n", fn, d);
}


inline std::string to_string(const char *s)
{
	return std::string(s);
};

inline std::string to_string(void * const p)
{
	uint64_t x = (uint64_t)p;
	return std::to_string(x);
};

template <typename T>
inline void log_exit_rc(const char *fn, const T &ret)
{
	using namespace std;
	using namespace openvr_string;
	std::string s  = to_string(ret);
	dprintf("< %s\n\n", s.c_str());
}
#define NO_LOG
#ifdef NO_LOG
#define LOG_ENTRY(x)
#define LOG_ENTRY_1(x,y)
#define LOG_EXIT(x)  
#define LOG_EXIT_RC(x,y) return x
#else
#define LOG_ENTRY(x)		log_entry(x);
#define LOG_ENTRY_1(x,y)	log_entry(x,y);
#define LOG_EXIT(x)  dprintf("< %s\n\n", x);
#define LOG_EXIT_RC(x,y) log_exit_rc(y,x); return x
#endif
