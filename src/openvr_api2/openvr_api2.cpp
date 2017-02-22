// openvr_api2.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "imp_interface.h"
#include "dprintf.h"
#include <openvr.h>
#include <openvr_broker.h>
#include <memory.h>
#include <windows.h>


extern ImpInterface *GetCppStubInterfaces();
extern ImpInterface *GetFnStubInterfaces();
extern ImpInterface *GetFnPassalongInterfaces();
extern ImpInterface *GetCppPassalongInterfaces();

static ImpInterface *m_fn_passalong_interfaces;
static ImpInterface *m_cpp_passalong_interfaces;


bool thread_zombie = false;
DWORD WINAPI ThreadProcGuitest(_In_ LPVOID lpParameter)
{
	extern int profiler_main();
	thread_zombie = false;
	profiler_main();
	thread_zombie = true;
	return 0;
}

static HANDLE h_profiler;
static int init_count;


VR_INTERFACE void VR_CALLTYPE VR_ShowProfiler()
{
	if (h_profiler && thread_zombie)
	{
		CloseHandle(h_profiler);
		h_profiler = 0;
	}

	if (h_profiler == 0)
	{
		h_profiler = CreateThread(0, 0, &ThreadProcGuitest, 0, 0, 0);
	}
}

extern int boing;

VR_INTERFACE uint32_t VR_CALLTYPE VR_InitInternal(vr::EVRInitError *peError, vr::EVRApplicationType eApplicationType)
{
	LOG_ENTRY_1("VR_InitInternal", eApplicationType);

	if (!m_cpp_passalong_interfaces)
	{
		m_cpp_passalong_interfaces = GetCppPassalongInterfaces();
	}

	m_cpp_passalong_interfaces->Init();

	init_count++;
	VR_ShowProfiler();
	

	if (peError)
	{ 
		*peError = vr::VRInitError_None;
	}

	LOG_EXIT_RC(0, "VR_InitInternal");
}

VR_INTERFACE void VR_CALLTYPE VR_ShutdownInternal()
{
	LOG_ENTRY("VR_ShutdownInternal");

	m_cpp_passalong_interfaces->Shutdown();

	LOG_EXIT("VR_ShutdownInternal");
}
VR_INTERFACE bool VR_CALLTYPE VR_IsHmdPresent()
{
	LOG_ENTRY("VR_IsHmdPresent");
	
	if (!m_cpp_passalong_interfaces)
	{
		m_cpp_passalong_interfaces = GetCppPassalongInterfaces();
	}
	bool rc = m_cpp_passalong_interfaces->IsHmdPresent();

	LOG_EXIT_RC(rc, "VR_IsHmdPresent");
}

VR_INTERFACE bool VR_CALLTYPE VR_IsRuntimeInstalled()
{
	LOG_ENTRY("VR_IsRuntimeInstalled");
	if (!m_cpp_passalong_interfaces)
	{
		m_cpp_passalong_interfaces = GetCppPassalongInterfaces();
	}
	bool rc = m_cpp_passalong_interfaces->IsRuntimeInstalled();

	LOG_EXIT_RC(rc, "VR_IsRuntimeInstalled");
}

VR_INTERFACE const char *VR_CALLTYPE VR_RuntimePath()
{
	LOG_ENTRY("VR_RuntimePath");
	if (!m_cpp_passalong_interfaces)
	{
		m_cpp_passalong_interfaces = GetCppPassalongInterfaces();
	}
	const char *rc = m_cpp_passalong_interfaces->RuntimePath();

	LOG_EXIT_RC(rc, "VR_RuntimePath");
}
VR_INTERFACE const char *VR_CALLTYPE VR_GetVRInitErrorAsSymbol(vr::EVRInitError error)
{
	LOG_ENTRY("VR_GetVRInitErrorAsSymbol");
	const char *rc = openvr_string::EVRInitErrorToString(error);
	LOG_EXIT_RC(rc, "VR_GetVRInitErrorAsSymbol");
}
VR_INTERFACE const char *VR_GetStringForHmdError(vr::EVRInitError error)
{
	LOG_ENTRY("VR_GetStringForHmdError");
	const char *rc = openvr_string::EVRInitErrorToString(error);
	LOG_EXIT_RC(rc, "VR_GetStringForHmdError");
}

VR_INTERFACE const char *VR_CALLTYPE VR_GetVRInitErrorAsEnglishDescription(vr::EVRInitError error)
{
	LOG_ENTRY("VR_GetVRInitErrorAsEnglishDescription");
	const char *rc = openvr_string::EVRInitErrorToString(error);
	LOG_EXIT_RC(rc, "VR_GetVRInitErrorAsEnglishDescription");
}

static void *GetInterface(const char *pchInterfaceVersion, openvr_broker::open_vr_interfaces_void *interfaces)
{
	if (!pchInterfaceVersion)
	{
		return nullptr;
	}

	int underscore_pos = -1;
	int i = 0;
	while (pchInterfaceVersion[i])
	{
		if (pchInterfaceVersion[i] == '_')
		{
			underscore_pos = i;
			break;
		}
		i++;
	}

	void * ret = nullptr;
	switch (underscore_pos)
	{
	case 9:		if (memcmp(pchInterfaceVersion, "IVRSystem_", 9) == 0) { ret = interfaces->sysi; break; }
	case 10:	if (memcmp(pchInterfaceVersion, "IVROverlay_", 10) == 0) { ret = interfaces->ovi; break; }
	case 11:	if (memcmp(pchInterfaceVersion, "IVRSettings_", 11) == 0) { ret = interfaces->seti; break; }
	case 12:	if (memcmp(pchInterfaceVersion, "IVRResources_", 12) == 0) { ret = interfaces->resi; break; }
				if (memcmp(pchInterfaceVersion, "IVRChaperone_", 12) == 0) { ret = interfaces->chapi; break; }
	case 13:	if (memcmp(pchInterfaceVersion, "IVRCompositor_", 13) == 0) { ret = interfaces->compi; break; }
	case 14:	if (memcmp(pchInterfaceVersion, "IVRScreenshots_", 14) == 0) { ret = interfaces->screeni; break; }
	case 15:	if (memcmp(pchInterfaceVersion, "IVRApplications_", 15) == 0) { ret = interfaces->appi; break; }
				if (memcmp(pchInterfaceVersion, "IVRRenderModels_", 15) == 0) { ret = interfaces->remi; break; }
	case 16:	if (memcmp(pchInterfaceVersion, "IVRTrackedCamera_", 16) == 0) { ret = interfaces->taci; break; }
				if (memcmp(pchInterfaceVersion, "IVRNotifications_", 16) == 0) { ret = interfaces->noti; break; }
	case 17:	if (memcmp(pchInterfaceVersion, "IVRChaperoneSetup_", 17) == 0) { ret = interfaces->chapsi; break; }
	case 18:	if (memcmp(pchInterfaceVersion, "IVRExtendedDisplay_", 18) == 0) { ret = interfaces->exdi; break; }
	}
	return ret;
}

static void *GetInterface(const char *pchInterfaceVersion)
{
	void *ret;
	if (strncmp(pchInterfaceVersion, "FnTable:", 8) == 0)
	{
		// it's a c interface
		//ret = GetInterface(pchInterfaceVersion + 8, (openvr_broker::open_vr_interfaces_void *) GetFnStubInterfaces());

		if (!m_fn_passalong_interfaces)
		{
			m_fn_passalong_interfaces = GetFnPassalongInterfaces();
		}
		ret = GetInterface(pchInterfaceVersion + 8, (openvr_broker::open_vr_interfaces_void *)
								m_fn_passalong_interfaces->GetAPIInterfaceHandles());
	}
	else
	{

		if (!m_cpp_passalong_interfaces)
		{
			m_cpp_passalong_interfaces = GetCppPassalongInterfaces();
		}

		//ret = GetInterface(pchInterfaceVersion, (openvr_broker::open_vr_interfaces_void *) GetCppStubInterfaces());
		ret = GetInterface(pchInterfaceVersion, (openvr_broker::open_vr_interfaces_void *) 
									m_cpp_passalong_interfaces->GetAPIInterfaceHandles());
	}
	return ret;
}

VR_INTERFACE void *VR_CALLTYPE VR_GetGenericInterface(const char *pchInterfaceVersion, vr::EVRInitError *peError)
{
	LOG_ENTRY_1("VR_GetGenericInterface", pchInterfaceVersion);

	void *ret = GetInterface(pchInterfaceVersion);

	if (peError)
	{
		if (ret == nullptr)
		{
			*peError = vr::VRInitError_Unknown;
		}
		else
		{
			*peError = vr::VRInitError_None;
		}
	}

	LOG_EXIT_RC(ret, "VR_GetGenericInterface");
}

VR_INTERFACE bool VR_CALLTYPE VR_IsInterfaceVersionValid(const char *pchInterfaceVersion)
{
	LOG_ENTRY_1("VR_IsInterfaceVersionValid", pchInterfaceVersion);
	bool rc = (GetInterface(pchInterfaceVersion) != nullptr);
	LOG_EXIT_RC(rc, "VR_IsInterfaceVersionValid");
}

VR_INTERFACE uint32_t VR_CALLTYPE VR_GetInitToken()
{
	LOG_ENTRY("VR_GetInitToken");
	uint32_t rc = 1000;
	LOG_EXIT_RC(rc, "VR_GetInitToken");
}

