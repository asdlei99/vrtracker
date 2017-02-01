#include <windows.h>
#include <dprintf.h>
#include "InterfacesIntoOpenVR.h"

static const char *proc_names[] =
{
	//"LiquidVR",								// {openvr_api.dll!LiquidVR(void)} int(__stdcall*)()
	//"VRCompositorSystemInternal",             //openvr_api.dll!VRCompositorSystemInternal(void)}	int(__stdcall*)()
	//"VRControlPanel",							//openvr_api.dll!VRControlPanel(void)}	int(__stdcall*)()
	//"VRDashboardManager",						//openvr_api.dll!VRDashboardManager(void)}	int(__stdcall*)()
	//"VROculusDirect",							//openvr_api.dll!VROculusDirect(void)}	int(__stdcall*)()
	//"VRRenderModelsInternal",					//openvr_api.dll!VRRenderModelsInternal(void)}	int(__stdcall*)()
	//"VRTrackedCameraInternal",					//openvr_api.dll!VRTrackedCameraInternal(void)}	int(__stdcall*)()
	/*0*/"VR_GetGenericInterface",					//openvr_api.dll!VR_GetGenericInterface(const char *, vr::EVRInitError *)}	int(__stdcall*)()
	/*1*/"VR_GetInitToken",							//openvr_api.dll!VR_GetInitToken(void)}	int(__stdcall*)()
	/*2*/"VR_GetStringForHmdError",					//openvr_api.dll!VR_GetStringForHmdError(vr::EVRInitError)}	int(__stdcall*)()
	/*3*/"VR_GetVRInitErrorAsEnglishDescription",	//openvr_api.dll!VR_GetStringForHmdError(vr::EVRInitError)}	int(__stdcall*)()
	/*4*/"VR_GetVRInitErrorAsSymbol",				//openvr_api.dll!VR_GetVRInitErrorAsSymbol(vr::EVRInitError)}	int(__stdcall*)()
	/*5*/"VR_InitInternal",							//openvr_api.dll!VR_InitInternal(vr::EVRInitError *, vr::EVRApplicationType)}	int(__stdcall*)()
	/*6*/"VR_IsHmdPresent",							//openvr_api.dll!VR_IsHmdPresent(void)}	int(__stdcall*)()
	/*7*/"VR_IsInterfaceVersionValid",				//openvr_api.dll!VR_IsInterfaceVersionValid(const char *)}	int(__stdcall*)()
	/*8*/"VR_IsRuntimeInstalled",					//openvr_api.dll!VR_IsRuntimeInstalled(void)}	int(__stdcall*)()
	/*9*/"VR_RuntimePath",							//openvr_api.dll!VR_RuntimePath(void)}	int(__stdcall*)()
	/*10*/"VR_ShutdownInternal"						//openvr_api.dll!VR_ShutdownInternal(void)}	int(__stdcall*)()
};

enum
{
	/*0*/indexVR_GetGenericInterface,					//openvr_api.dll!VR_GetGenericInterface(const char *, vr::EVRInitError *)}	int(__stdcall*)()
	/*1*/indexVR_GetInitToken,							//openvr_api.dll!VR_GetInitToken(void)}	int(__stdcall*)()
	/*2*/indexVR_GetStringForHmdError,					//openvr_api.dll!VR_GetStringForHmdError(vr::EVRInitError)}	int(__stdcall*)()
	/*3*/indexVR_GetVRInitErrorAsEnglishDescription,	//openvr_api.dll!VR_GetStringForHmdError(vr::EVRInitError)}	int(__stdcall*)()
	/*4*/indexVR_GetVRInitErrorAsSymbol,				//openvr_api.dll!VR_GetVRInitErrorAsSymbol(vr::EVRInitError)}	int(__stdcall*)()
	/*5*/indexVR_InitInternal,							//openvr_api.dll!VR_InitInternal(vr::EVRInitError *, vr::EVRApplicationType)}	int(__stdcall*)()
	/*6*/indexVR_IsHmdPresent,							//openvr_api.dll!VR_IsHmdPresent(void)}	int(__stdcall*)()
	/*7*/indexVR_IsInterfaceVersionValid,				//openvr_api.dll!VR_IsInterfaceVersionValid(const char *)}	int(__stdcall*)()
	/*8*/indexVR_IsRuntimeInstalled,					//openvr_api.dll!VR_IsRuntimeInstalled(void)}	int(__stdcall*)()
	/*9*/indexVR_RuntimePath,							//openvr_api.dll!VR_RuntimePath(void)}	int(__stdcall*)()
	/*10*/indexVR_ShutdownInternal						//openvr_api.dll!VR_ShutdownInternal(void)}	int(__stdcall*)()
};

static const int NUM_PROCS = sizeof(proc_names) / sizeof(proc_names[0]);

typedef int(*voidReturnsVoid)();
typedef int(*voidReturnsBool)();
typedef int(*voidReturnsInt)();
typedef char *(intReturnsChar)();

static const char *cpp_interface_names[] =
{
	"IVRSystem_015",
	"IVRApplications_006",
	"IVRSettings_002",
	"IVRChaperone_003",
	"IVRChaperoneSetup_005",
	"IVRCompositor_019",
	"IVRNotifications_002",
	"IVROverlay_014",
	"IVRRenderModels_005",
	"IVRExtendedDisplay_001",
	"IVRTrackedCamera_003",
	"IVRScreenshots_001",
	"IVRResources_001"
};
static const char *c_interface_names[] =
{
	"FnTable:IVRSystem_015",
	"FnTable:IVRApplications_006",
	"FnTable:IVRSettings_002",
	"FnTable:IVRChaperone_003",
	"FnTable:IVRChaperoneSetup_005",
	"FnTable:IVRCompositor_019",
	"FnTable:IVRNotifications_002",
	"FnTable:IVROverlay_014",
	"FnTable:IVRRenderModels_005",
	"FnTable:IVRExtendedDisplay_001",
	"FnTable:IVRTrackedCamera_003",
	"FnTable:IVRScreenshots_001",
	"FnTable:IVRResources_001"
};

enum
{
	index_IVRSystem,
	index_IVRApplications,
	index_IVRSettings,
	index_IVRChaperone,
	index_IVRChaperoneSetup,
	index_IVRCompositor,
	index_IVRNotifications,
	index_IVROverlay,
	index_IVRRenderModels,
	index_IVRExtendedDisplay,
	index_IVRTrackedCamera,
	index_IVRScreenshots,
	index_IVRResources
};

static const int NUM_INTERFACES = sizeof(cpp_interface_names) / sizeof(cpp_interface_names[0]);

struct InterfacesIntoOpenVRInternal
{
	HMODULE module;
	FARPROC proc_table[NUM_PROCS];
	void *cpp_interface_table[NUM_INTERFACES];
	void *c_interface_table[NUM_INTERFACES];
};

static void call_ShutdownInternal(InterfacesIntoOpenVRInternal *openvr)
{
	FARPROC p = *openvr->proc_table[indexVR_ShutdownInternal];
	(*(voidReturnsVoid)p)();
}

static bool QueryInterfaces(FARPROC p, void **interface_table, const char **interface_names)
{
	typedef void* (*VR_GetGenericInterface)(const char *, int *);
	bool rc = true;
	for (int i = 0; i < NUM_INTERFACES; i++)
	{
		const char *expected_valid_version = interface_names[i];
		int error_code;
		void *handle = (*(VR_GetGenericInterface)p)(expected_valid_version, &error_code);
		if (handle)
		{
			// interface version is valid
			dprintf("%p %s\n", handle, expected_valid_version);
			interface_table[i] = handle;
		}
		else
		{
			printf("got failed handle for version: %s error code: %d\n", expected_valid_version, error_code);
			interface_table[i] = nullptr;
			rc = false;
		}
	}
	return rc;
}

static bool GetGenericInterfaces(InterfacesIntoOpenVRInternal *openvr)
{
	FARPROC p = *openvr->proc_table[indexVR_GetGenericInterface];
	bool rc = true;
	dprintf("handles\n");
	bool rc1 = QueryInterfaces(p, &openvr->cpp_interface_table[0], &cpp_interface_names[0]);
	bool rc2 = QueryInterfaces(p, &openvr->c_interface_table[0], &c_interface_names[0]);
	return rc1 && rc2;
}

static bool call_IsInterfaceVersionValid(InterfacesIntoOpenVRInternal *openvr, const char *interface_name)
{
	typedef int(*VR_IsInterfaceVersionValid)(const char *);
	FARPROC p = *openvr->proc_table[indexVR_IsInterfaceVersionValid];
	int valid = (*(VR_IsInterfaceVersionValid)p)(interface_name);
	if (valid)
		return true;
	else
		return false;
}

static bool call_VRInit(InterfacesIntoOpenVRInternal *openvr)
{
	typedef int(*VR_InitInternal)(int *, int);
	int error;
	FARPROC p = *openvr->proc_table[indexVR_InitInternal];
	int token = (*(VR_InitInternal)p)(&error, 1);
	if (error == 0)
	{
		dprintf("VR_Init success\n");
		return true;
	}
	else
	{
		dprintf("VR_Init fail %d\n", error);
		return false;
	}
}

static bool call_IsRuntimeInstalled(InterfacesIntoOpenVRInternal *openvr)
{
	FARPROC p = *openvr->proc_table[indexVR_IsRuntimeInstalled];
	int runtime_installed = (*(voidReturnsBool)p)();
	if (runtime_installed)
	{
		// interface version is valid
		dprintf("runtime_installed: true\n");
	}
	else
	{
		dprintf("runtime_installed: false\n");
	}
	return (runtime_installed != 0);
}

static bool call_IsHmdPresent(InterfacesIntoOpenVRInternal *openvr)
{
	FARPROC p = *openvr->proc_table[indexVR_IsHmdPresent];
	int hmd = (*(voidReturnsBool)p)();
	if (hmd)
	{
		// interface version is valid
		dprintf("hmd_present: true\n");
	}
	else
	{
		dprintf("hmd_present: false\n");
	}
	return (hmd != 0);
}

static const char *call_RuntimePath(InterfacesIntoOpenVRInternal *openvr)
{
	typedef char *(*typedefRuntimePath)();
	FARPROC p = *openvr->proc_table[indexVR_RuntimePath];
	char *runtime_path = (*(typedefRuntimePath)p)();
	if (runtime_path)
	{
		// interface version is valid
		printf("runtime_path: %s\n", runtime_path);
	}
	else
	{
		printf("runtime_path: null\n");
	}
	return runtime_path;
}

static int call_GetInitToken(InterfacesIntoOpenVRInternal *openvr)
{
	FARPROC p = *openvr->proc_table[indexVR_RuntimePath];
	int init_token = (*(voidReturnsInt)p)();
	printf("init_token: %d\n", init_token);
	return init_token;
}

static bool close_lib(InterfacesIntoOpenVRInternal *openvr)
{
	call_ShutdownInternal(openvr);
	BOOL free_rc = FreeLibrary(openvr->module);
	if (free_rc)
	{
		dprintf("freelibrary: success\n");
		return true;
	}
	else
	{
		dprintf("freelibary: fail %d\n", GetLastError());
		return false;
	}
}

static bool open_lib(LPCTSTR library_name, LPCTSTR extra_path, InterfacesIntoOpenVRInternal *openvr)
{
	openvr->module = LoadLibraryEx(library_name, NULL, 0);
	if (!openvr->module)
	{
		wprintf(L"could not find %s in default path\n", library_name);
		DLL_DIRECTORY_COOKIE c = AddDllDirectory(extra_path);
		if (!c)
		{
			wprintf(L"could not add %s\n", extra_path);
		}
		else
		{
			openvr->module = LoadLibraryEx(library_name, NULL, LOAD_LIBRARY_SEARCH_USER_DIRS);
			if (!openvr->module)
			{
				wprintf(L"could not find (or load!) %s in extra path %s\n", library_name, extra_path);
			}
			else
			{
				wprintf(L"found it after adding extra path %s\n", extra_path);
			}
		}
	}
	if (openvr->module)
	{
		printf("module %p\n", openvr->module);
		bool got_procs = true;
		for (int i = 0; i < NUM_PROCS; i++)
		{
			openvr->proc_table[i] = GetProcAddress(openvr->module, proc_names[i]);
			if (!openvr->proc_table[i])
			{
				printf("failed to GetProcAddress for %s\n", proc_names[i]);
				got_procs = false;
			}
		}
		if (got_procs)
		{
			bool rc = call_VRInit(openvr);
			if (rc)
			{
				call_IsRuntimeInstalled(openvr);
				call_RuntimePath(openvr);
				call_IsHmdPresent(openvr);
				call_GetInitToken(openvr);
				rc = GetGenericInterfaces(openvr);
			}
			return rc;
		}
	}
	return false;
}

bool InterfacesIntoOpenVR::open_lib(LPCTSTR library_name, LPCTSTR extra_path)
{
	InterfacesIntoOpenVRInternal *imp = new InterfacesIntoOpenVRInternal;
	internals = imp;
	memset(internals, 0, sizeof(InterfacesIntoOpenVRInternal));
	bool open_rc = ::open_lib(library_name, extra_path, imp);
	if (!open_rc)
	{
		delete internals;
		internals = nullptr;
	}
	return open_rc;
}

openvr_broker::open_vr_interfaces *InterfacesIntoOpenVR::cpp_interfaces()
{ 
	InterfacesIntoOpenVRInternal *imp = (InterfacesIntoOpenVRInternal *)internals;
	if (imp)
	{
		openvr_broker::open_vr_interfaces *ret = (openvr_broker::open_vr_interfaces *)imp->cpp_interface_table;
		return ret;
	}
	else
		return nullptr;
}

openvr_broker::open_vr_interfaces_void *InterfacesIntoOpenVR::c_interfaces()
{
	if (internals)
	{
		openvr_broker::open_vr_interfaces_void *ret = (openvr_broker::open_vr_interfaces_void *)
								((InterfacesIntoOpenVRInternal *)internals)->c_interface_table;
		return ret;
	}
	else
		return nullptr;
}

bool InterfacesIntoOpenVR::close_lib()
{
	bool rc = ::close_lib((InterfacesIntoOpenVRInternal *)internals);
	delete internals;
	internals = nullptr;
	return rc;
}
