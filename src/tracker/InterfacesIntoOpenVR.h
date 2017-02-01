#pragma once
#include "openvr_broker.h"
#include <windows.h>

class InterfacesIntoOpenVR
{
public:
	InterfacesIntoOpenVR() : internals(nullptr)
	{}

	bool open_lib(LPCTSTR library_name, LPCTSTR extra_path);
	bool is_open() { return internals != nullptr;  }
	bool close_lib();
	openvr_broker::open_vr_interfaces *cpp_interfaces();
	openvr_broker::open_vr_interfaces_void *c_interfaces();
private:
	void *internals;
};


