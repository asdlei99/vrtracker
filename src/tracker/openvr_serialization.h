#pragma once
#include "openvr.h"
#include <memory.h>
#include <assert.h>

struct EncodeStream
{
	EncodeStream(char *buf, uint64_t buf_size, bool count_only_in)
		:
		buf_size(buf_size),
		encoded_buf(buf),
		buf_pos(0),
		count_only(count_only_in)
	{}

	EncodeStream(EncodeStream &) = delete;

	void reset_buf_pos()
	{
		buf_pos = 0;
	}

	// write value to buf and advance pointer
	void memcpy_out_to_stream(const void *src, size_t s)
	{
		if (!count_only)
		{
			assert(buf_pos + (int)s < buf_size + 1);
			::memcpy(&encoded_buf[buf_pos], src, (int)s);
		}
		buf_pos += (int) s;
	}

	// write internal value out to stream and advance pointer
	void memcpy_from_stream(void *dest, size_t s)
	{
		assert(buf_pos + (int)s < buf_size + 1);	 // buf_pos can refer to the element after the last one
		::memcpy(dest, &encoded_buf[buf_pos], (int)s);
		buf_pos += (int)s;
	}
	uint64_t buf_size;
	char *encoded_buf;
	uint64_t buf_pos;
	bool count_only;
	
};



// don't define this or pay the price of crazy (implicit) encodings!
template <typename T>
static void encode(T v, EncodeStream &e);

#define ENCODE(type_name) template <> static void encode<type_name>(type_name v, EncodeStream &e) { e.memcpy_out_to_stream(&v, sizeof(v)); }

ENCODE(unsigned int);
ENCODE(int);
ENCODE(vr::EVRApplicationError);
ENCODE(vr::EVRRenderModelError); 
ENCODE(bool);
ENCODE(vr::EVRTrackedCameraError);
ENCODE(vr::EVRCompositorError);
ENCODE(vr::ETrackedPropertyError);
ENCODE(vr::EVRSettingsError);
ENCODE(uint64_t);



template <>
static void encode<const char *>(const char *v, EncodeStream &e)
{
	int size = (int)strlen(v) + 1;
	encode(size, e);
	e.memcpy_out_to_stream(v, size);
}

template <typename T>
static void decode(T &v, EncodeStream &e)
{
	e.memcpy_from_stream(&v, sizeof(v));
}

template <typename U>
static void decode(const char *&str, EncodeStream &e, U allocator)
{
	// read the size of the string
	int size;
	decode(size, e);

	// make space for it
	char *buf = allocator.allocate(size);
	// read it into the string
	e.memcpy_from_stream(buf, size);
	str = buf;
}

static void decode_str(char *str, EncodeStream &e)
{
	// read the size of the string
	int size;
	decode(size, e);

	// read it into the string
	e.memcpy_from_stream(str, size);
}


static void decode(vr::HmdMatrix34_t &v, EncodeStream &e)
{
	e.memcpy_from_stream(&v, sizeof(v));
}


