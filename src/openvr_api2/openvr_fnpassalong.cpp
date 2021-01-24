#include "openvr_fnpassalong.h"
#include "imp_interface.h"
#include <openvr_broker.h>
#include <stdint.h>
#include "openvr_method_ids.h"
#include "dprintf.h"
#include "vrdelta.h"

#include <InterfacesIntoOpenVR.h>
static InterfacesIntoOpenVR interfaces;
vr_state_tracker_t tracker;

static void *vrapi_interfaces_fnpassalong[] =
{
VRSystemFnPassalong_table,
VRApplicationsFnPassalong_table,
VRSettingsFnPassalong_table,
VRChaperoneFnPassalong_table,
VRChaperoneSetupFnPassalong_table,
VRCompositorFnPassalong_table,
VRNotificationsFnPassalong_table,
VROverlayFnPassalong_table,
VRRenderModelsFnPassalong_table,
VRExtendedDisplayFnPassalong_table,
VRTrackedCameraFnPassalong_table,
VRScreenshotsFnPassalong_table,
VRResourcesFnPassalong_table,
};


class CppStubImpInterface : public ImpInterface
{
public:
	virtual void Init()
	{
		static LPCTSTR extra_path = L"C:\\projects\\openvr_clean\\openvr\\bin\\win32";
		static LPCTSTR server_dll = L"openvr_api.dll";

		// initialize my interfaces object by connecting to the openvr dll
		if (!interfaces.is_open())
		{
			interfaces.open_lib(server_dll, extra_path);

			char szbuf[256];
			interfaces.cpp_interfaces()->compi->GetVulkanInstanceExtensionsRequired(szbuf, sizeof(szbuf));

		}

		if (tracker == 0)
		{
			TrackerConfig c;
			c.set_default();
			tracker = create_vr_state_tracker(c);
		}
	}
	virtual void Shutdown()
	{
	}
	virtual bool IsHmdPresent()
	{
		return true;
	}
	virtual bool IsRuntimeInstalled()
	{
		return true;
	}
	virtual const char *RuntimePath()
	{
		return "";
	}

	virtual void** GetAPIInterfaceHandles()
	{
		return vrapi_interfaces_fnpassalong;
	}
};

ImpInterface *GetFnPassalongInterfaces()
{
	static CppStubImpInterface m_lib_if;
	return &m_lib_if;
}



void * VRSystemFnPassalong_table[] = 
{
 VRSystemFnPassalong_GetRecommendedRenderTargetSize,
 VRSystemFnPassalong_GetProjectionMatrix,
 VRSystemFnPassalong_GetProjectionRaw,
 VRSystemFnPassalong_ComputeDistortion,
 VRSystemFnPassalong_GetEyeToHeadTransform,
 VRSystemFnPassalong_GetTimeSinceLastVsync,
 VRSystemFnPassalong_GetD3D9AdapterIndex,
 VRSystemFnPassalong_GetDXGIOutputInfo,
 VRSystemFnPassalong_IsDisplayOnDesktop,
 VRSystemFnPassalong_SetDisplayVisibility,
 VRSystemFnPassalong_GetDeviceToAbsoluteTrackingPose,
 VRSystemFnPassalong_ResetSeatedZeroPose,
 VRSystemFnPassalong_GetSeatedZeroPoseToStandingAbsoluteTrackingPose,
 VRSystemFnPassalong_GetRawZeroPoseToStandingAbsoluteTrackingPose,
 VRSystemFnPassalong_GetSortedTrackedDeviceIndicesOfClass,
 VRSystemFnPassalong_GetTrackedDeviceActivityLevel,
 VRSystemFnPassalong_ApplyTransform,
 VRSystemFnPassalong_GetTrackedDeviceIndexForControllerRole,
 VRSystemFnPassalong_GetControllerRoleForTrackedDeviceIndex,
 VRSystemFnPassalong_GetTrackedDeviceClass,
 VRSystemFnPassalong_IsTrackedDeviceConnected,
 VRSystemFnPassalong_GetBoolTrackedDeviceProperty,
 VRSystemFnPassalong_GetFloatTrackedDeviceProperty,
 VRSystemFnPassalong_GetInt32TrackedDeviceProperty,
 VRSystemFnPassalong_GetUint64TrackedDeviceProperty,
 VRSystemFnPassalong_GetMatrix34TrackedDeviceProperty,
 VRSystemFnPassalong_GetStringTrackedDeviceProperty,
 VRSystemFnPassalong_GetPropErrorNameFromEnum,
 VRSystemFnPassalong_PollNextEvent,
 VRSystemFnPassalong_PollNextEventWithPose,
 VRSystemFnPassalong_GetEventTypeNameFromEnum,
 VRSystemFnPassalong_GetHiddenAreaMesh,
 VRSystemFnPassalong_GetControllerState,
 VRSystemFnPassalong_GetControllerStateWithPose,
 VRSystemFnPassalong_TriggerHapticPulse,
 VRSystemFnPassalong_GetButtonIdNameFromEnum,
 VRSystemFnPassalong_GetControllerAxisTypeNameFromEnum,
 VRSystemFnPassalong_CaptureInputFocus,
 VRSystemFnPassalong_ReleaseInputFocus,
 VRSystemFnPassalong_IsInputFocusCapturedByAnotherProcess,
 VRSystemFnPassalong_DriverDebugRequest,
 VRSystemFnPassalong_PerformFirmwareUpdate,
 VRSystemFnPassalong_AcknowledgeQuit_Exiting,
 VRSystemFnPassalong_AcknowledgeQuit_UserPrompt,
};
void * VRExtendedDisplayFnPassalong_table[] = 
{
 VRExtendedDisplayFnPassalong_GetWindowBounds,
 VRExtendedDisplayFnPassalong_GetEyeOutputViewport,
 VRExtendedDisplayFnPassalong_GetDXGIOutputInfo,
};
void * VRTrackedCameraFnPassalong_table[] = 
{
 VRTrackedCameraFnPassalong_GetCameraErrorNameFromEnum,
 VRTrackedCameraFnPassalong_HasCamera,
 VRTrackedCameraFnPassalong_GetCameraFrameSize,
 VRTrackedCameraFnPassalong_GetCameraIntrinsics,
 VRTrackedCameraFnPassalong_GetCameraProjection,
 VRTrackedCameraFnPassalong_AcquireVideoStreamingService,
 VRTrackedCameraFnPassalong_ReleaseVideoStreamingService,
 VRTrackedCameraFnPassalong_GetVideoStreamFrameBuffer,
 VRTrackedCameraFnPassalong_GetVideoStreamTextureSize,
 VRTrackedCameraFnPassalong_GetVideoStreamTextureD3D11,
 VRTrackedCameraFnPassalong_GetVideoStreamTextureGL,
 VRTrackedCameraFnPassalong_ReleaseVideoStreamTextureGL,
};
void * VRApplicationsFnPassalong_table[] = 
{
 VRApplicationsFnPassalong_AddApplicationManifest,
 VRApplicationsFnPassalong_RemoveApplicationManifest,
 VRApplicationsFnPassalong_IsApplicationInstalled,
 VRApplicationsFnPassalong_GetApplicationCount,
 VRApplicationsFnPassalong_GetApplicationKeyByIndex,
 VRApplicationsFnPassalong_GetApplicationKeyByProcessId,
 VRApplicationsFnPassalong_LaunchApplication,
 VRApplicationsFnPassalong_LaunchTemplateApplication,
 VRApplicationsFnPassalong_LaunchApplicationFromMimeType,
 VRApplicationsFnPassalong_LaunchDashboardOverlay,
 VRApplicationsFnPassalong_CancelApplicationLaunch,
 VRApplicationsFnPassalong_IdentifyApplication,
 VRApplicationsFnPassalong_GetApplicationProcessId,
 VRApplicationsFnPassalong_GetApplicationsErrorNameFromEnum,
 VRApplicationsFnPassalong_GetApplicationPropertyString,
 VRApplicationsFnPassalong_GetApplicationPropertyBool,
 VRApplicationsFnPassalong_GetApplicationPropertyUint64,
 VRApplicationsFnPassalong_SetApplicationAutoLaunch,
 VRApplicationsFnPassalong_GetApplicationAutoLaunch,
 VRApplicationsFnPassalong_SetDefaultApplicationForMimeType,
 VRApplicationsFnPassalong_GetDefaultApplicationForMimeType,
 VRApplicationsFnPassalong_GetApplicationSupportedMimeTypes,
 VRApplicationsFnPassalong_GetApplicationsThatSupportMimeType,
 VRApplicationsFnPassalong_GetApplicationLaunchArguments,
 VRApplicationsFnPassalong_GetStartingApplication,
 VRApplicationsFnPassalong_GetTransitionState,
 VRApplicationsFnPassalong_PerformApplicationPrelaunchCheck,
 VRApplicationsFnPassalong_GetApplicationsTransitionStateNameFromEnum,
 VRApplicationsFnPassalong_IsQuitUserPromptRequested,
 VRApplicationsFnPassalong_LaunchInternalProcess,
};
void * VRChaperoneFnPassalong_table[] = 
{
 VRChaperoneFnPassalong_GetCalibrationState,
 VRChaperoneFnPassalong_GetPlayAreaSize,
 VRChaperoneFnPassalong_GetPlayAreaRect,
 VRChaperoneFnPassalong_ReloadInfo,
 VRChaperoneFnPassalong_SetSceneColor,
 VRChaperoneFnPassalong_GetBoundsColor,
 VRChaperoneFnPassalong_AreBoundsVisible,
 VRChaperoneFnPassalong_ForceBoundsVisible,
};
void * VRChaperoneSetupFnPassalong_table[] = 
{
 VRChaperoneSetupFnPassalong_CommitWorkingCopy,
 VRChaperoneSetupFnPassalong_RevertWorkingCopy,
 VRChaperoneSetupFnPassalong_GetWorkingPlayAreaSize,
 VRChaperoneSetupFnPassalong_GetWorkingPlayAreaRect,
 VRChaperoneSetupFnPassalong_GetWorkingCollisionBoundsInfo,
 VRChaperoneSetupFnPassalong_GetLiveCollisionBoundsInfo,
 VRChaperoneSetupFnPassalong_GetWorkingSeatedZeroPoseToRawTrackingPose,
 VRChaperoneSetupFnPassalong_GetWorkingStandingZeroPoseToRawTrackingPose,
 VRChaperoneSetupFnPassalong_SetWorkingPlayAreaSize,
 VRChaperoneSetupFnPassalong_SetWorkingCollisionBoundsInfo,
 VRChaperoneSetupFnPassalong_SetWorkingSeatedZeroPoseToRawTrackingPose,
 VRChaperoneSetupFnPassalong_SetWorkingStandingZeroPoseToRawTrackingPose,
 VRChaperoneSetupFnPassalong_ReloadFromDisk,
 VRChaperoneSetupFnPassalong_GetLiveSeatedZeroPoseToRawTrackingPose,
 VRChaperoneSetupFnPassalong_SetWorkingCollisionBoundsTagsInfo,
 VRChaperoneSetupFnPassalong_GetLiveCollisionBoundsTagsInfo,
 VRChaperoneSetupFnPassalong_SetWorkingPhysicalBoundsInfo,
 VRChaperoneSetupFnPassalong_GetLivePhysicalBoundsInfo,
 VRChaperoneSetupFnPassalong_ExportLiveToBuffer,
 VRChaperoneSetupFnPassalong_ImportFromBufferToWorking,
};
void * VRCompositorFnPassalong_table[] = 
{
 VRCompositorFnPassalong_SetTrackingSpace,
 VRCompositorFnPassalong_GetTrackingSpace,
 VRCompositorFnPassalong_WaitGetPoses,
 VRCompositorFnPassalong_GetLastPoses,
 VRCompositorFnPassalong_GetLastPoseForTrackedDeviceIndex,
 VRCompositorFnPassalong_Submit,
 VRCompositorFnPassalong_ClearLastSubmittedFrame,
 VRCompositorFnPassalong_PostPresentHandoff,
 VRCompositorFnPassalong_GetFrameTiming,
 VRCompositorFnPassalong_GetFrameTimings,
 VRCompositorFnPassalong_GetFrameTimeRemaining,
 VRCompositorFnPassalong_GetCumulativeStats,
 VRCompositorFnPassalong_FadeToColor,
 VRCompositorFnPassalong_GetCurrentFadeColor,
 VRCompositorFnPassalong_FadeGrid,
 VRCompositorFnPassalong_GetCurrentGridAlpha,
 VRCompositorFnPassalong_SetSkyboxOverride,
 VRCompositorFnPassalong_ClearSkyboxOverride,
 VRCompositorFnPassalong_CompositorBringToFront,
 VRCompositorFnPassalong_CompositorGoToBack,
 VRCompositorFnPassalong_CompositorQuit,
 VRCompositorFnPassalong_IsFullscreen,
 VRCompositorFnPassalong_GetCurrentSceneFocusProcess,
 VRCompositorFnPassalong_GetLastFrameRenderer,
 VRCompositorFnPassalong_CanRenderScene,
 VRCompositorFnPassalong_ShowMirrorWindow,
 VRCompositorFnPassalong_HideMirrorWindow,
 VRCompositorFnPassalong_IsMirrorWindowVisible,
 VRCompositorFnPassalong_CompositorDumpImages,
 VRCompositorFnPassalong_ShouldAppRenderWithLowResources,
 VRCompositorFnPassalong_ForceInterleavedReprojectionOn,
 VRCompositorFnPassalong_ForceReconnectProcess,
 VRCompositorFnPassalong_SuspendRendering,
 VRCompositorFnPassalong_GetMirrorTextureD3D11,
 VRCompositorFnPassalong_GetMirrorTextureGL,
 VRCompositorFnPassalong_ReleaseSharedGLTexture,
 VRCompositorFnPassalong_LockGLSharedTextureForAccess,
 VRCompositorFnPassalong_UnlockGLSharedTextureForAccess,
 VRCompositorFnPassalong_GetVulkanInstanceExtensionsRequired,
 VRCompositorFnPassalong_GetVulkanDeviceExtensionsRequired,
};
void * VROverlayFnPassalong_table[] = 
{
 VROverlayFnPassalong_FindOverlay,
 VROverlayFnPassalong_CreateOverlay,
 VROverlayFnPassalong_DestroyOverlay,
 VROverlayFnPassalong_SetHighQualityOverlay,
 VROverlayFnPassalong_GetHighQualityOverlay,
 VROverlayFnPassalong_GetOverlayKey,
 VROverlayFnPassalong_GetOverlayName,
 VROverlayFnPassalong_GetOverlayImageData,
 VROverlayFnPassalong_GetOverlayErrorNameFromEnum,
 VROverlayFnPassalong_SetOverlayRenderingPid,
 VROverlayFnPassalong_GetOverlayRenderingPid,
 VROverlayFnPassalong_SetOverlayFlag,
 VROverlayFnPassalong_GetOverlayFlag,
 VROverlayFnPassalong_SetOverlayColor,
 VROverlayFnPassalong_GetOverlayColor,
 VROverlayFnPassalong_SetOverlayAlpha,
 VROverlayFnPassalong_GetOverlayAlpha,
 VROverlayFnPassalong_SetOverlayTexelAspect,
 VROverlayFnPassalong_GetOverlayTexelAspect,
 VROverlayFnPassalong_SetOverlaySortOrder,
 VROverlayFnPassalong_GetOverlaySortOrder,
 VROverlayFnPassalong_SetOverlayWidthInMeters,
 VROverlayFnPassalong_GetOverlayWidthInMeters,
 VROverlayFnPassalong_SetOverlayAutoCurveDistanceRangeInMeters,
 VROverlayFnPassalong_GetOverlayAutoCurveDistanceRangeInMeters,
 VROverlayFnPassalong_SetOverlayTextureColorSpace,
 VROverlayFnPassalong_GetOverlayTextureColorSpace,
 VROverlayFnPassalong_SetOverlayTextureBounds,
 VROverlayFnPassalong_GetOverlayTextureBounds,
 VROverlayFnPassalong_GetOverlayTransformType,
 VROverlayFnPassalong_SetOverlayTransformAbsolute,
 VROverlayFnPassalong_GetOverlayTransformAbsolute,
 VROverlayFnPassalong_SetOverlayTransformTrackedDeviceRelative,
 VROverlayFnPassalong_GetOverlayTransformTrackedDeviceRelative,
 VROverlayFnPassalong_SetOverlayTransformTrackedDeviceComponent,
 VROverlayFnPassalong_GetOverlayTransformTrackedDeviceComponent,
 VROverlayFnPassalong_ShowOverlay,
 VROverlayFnPassalong_HideOverlay,
 VROverlayFnPassalong_IsOverlayVisible,
 VROverlayFnPassalong_GetTransformForOverlayCoordinates,
 VROverlayFnPassalong_PollNextOverlayEvent,
 VROverlayFnPassalong_GetOverlayInputMethod,
 VROverlayFnPassalong_SetOverlayInputMethod,
 VROverlayFnPassalong_GetOverlayMouseScale,
 VROverlayFnPassalong_SetOverlayMouseScale,
 VROverlayFnPassalong_ComputeOverlayIntersection,
 VROverlayFnPassalong_HandleControllerOverlayInteractionAsMouse,
 VROverlayFnPassalong_IsHoverTargetOverlay,
 VROverlayFnPassalong_GetGamepadFocusOverlay,
 VROverlayFnPassalong_SetGamepadFocusOverlay,
 VROverlayFnPassalong_SetOverlayNeighbor,
 VROverlayFnPassalong_MoveGamepadFocusToNeighbor,
 VROverlayFnPassalong_SetOverlayTexture,
 VROverlayFnPassalong_ClearOverlayTexture,
 VROverlayFnPassalong_SetOverlayRaw,
 VROverlayFnPassalong_SetOverlayFromFile,
 VROverlayFnPassalong_GetOverlayTexture,
 VROverlayFnPassalong_ReleaseNativeOverlayHandle,
 VROverlayFnPassalong_GetOverlayTextureSize,
 VROverlayFnPassalong_CreateDashboardOverlay,
 VROverlayFnPassalong_IsDashboardVisible,
 VROverlayFnPassalong_IsActiveDashboardOverlay,
 VROverlayFnPassalong_SetDashboardOverlaySceneProcess,
 VROverlayFnPassalong_GetDashboardOverlaySceneProcess,
 VROverlayFnPassalong_ShowDashboard,
 VROverlayFnPassalong_GetPrimaryDashboardDevice,
 VROverlayFnPassalong_ShowKeyboard,
 VROverlayFnPassalong_ShowKeyboardForOverlay,
 VROverlayFnPassalong_GetKeyboardText,
 VROverlayFnPassalong_HideKeyboard,
 VROverlayFnPassalong_SetKeyboardTransformAbsolute,
 VROverlayFnPassalong_SetKeyboardPositionForOverlay,
 VROverlayFnPassalong_SetOverlayIntersectionMask,
 VROverlayFnPassalong_GetOverlayFlags,
 VROverlayFnPassalong_ShowMessageOverlay,
};
void * VRRenderModelsFnPassalong_table[] = 
{
 VRRenderModelsFnPassalong_LoadRenderModel_Async,
 VRRenderModelsFnPassalong_FreeRenderModel,
 VRRenderModelsFnPassalong_LoadTexture_Async,
 VRRenderModelsFnPassalong_FreeTexture,
 VRRenderModelsFnPassalong_LoadTextureD3D11_Async,
 VRRenderModelsFnPassalong_LoadIntoTextureD3D11_Async,
 VRRenderModelsFnPassalong_FreeTextureD3D11,
 VRRenderModelsFnPassalong_GetRenderModelName,
 VRRenderModelsFnPassalong_GetRenderModelCount,
 VRRenderModelsFnPassalong_GetComponentCount,
 VRRenderModelsFnPassalong_GetComponentName,
 VRRenderModelsFnPassalong_GetComponentButtonMask,
 VRRenderModelsFnPassalong_GetComponentRenderModelName,
 VRRenderModelsFnPassalong_GetComponentState,
 VRRenderModelsFnPassalong_RenderModelHasComponent,
 VRRenderModelsFnPassalong_GetRenderModelThumbnailURL,
 VRRenderModelsFnPassalong_GetRenderModelOriginalPath,
 VRRenderModelsFnPassalong_GetRenderModelErrorNameFromEnum,
};
void * VRNotificationsFnPassalong_table[] = 
{
 VRNotificationsFnPassalong_CreateNotification,
 VRNotificationsFnPassalong_RemoveNotification,
};
void * VRSettingsFnPassalong_table[] = 
{
 VRSettingsFnPassalong_GetSettingsErrorNameFromEnum,
 VRSettingsFnPassalong_Sync,
 VRSettingsFnPassalong_SetBool,
 VRSettingsFnPassalong_SetInt32,
 VRSettingsFnPassalong_SetFloat,
 VRSettingsFnPassalong_SetString,
 VRSettingsFnPassalong_GetBool,
 VRSettingsFnPassalong_GetInt32,
 VRSettingsFnPassalong_GetFloat,
 VRSettingsFnPassalong_GetString,
 VRSettingsFnPassalong_RemoveSection,
 VRSettingsFnPassalong_RemoveKeyInSection,
};
void * VRScreenshotsFnPassalong_table[] = 
{
 VRScreenshotsFnPassalong_RequestScreenshot,
 VRScreenshotsFnPassalong_HookScreenshot,
 VRScreenshotsFnPassalong_GetScreenshotPropertyType,
 VRScreenshotsFnPassalong_GetScreenshotPropertyFilename,
 VRScreenshotsFnPassalong_UpdateScreenshotProgress,
 VRScreenshotsFnPassalong_TakeStereoScreenshot,
 VRScreenshotsFnPassalong_SubmitScreenshot,
};
void * VRResourcesFnPassalong_table[] = 
{
 VRResourcesFnPassalong_LoadSharedResource,
 VRResourcesFnPassalong_GetResourceFullPath,
};

#if 0
// idea 1 - create a big table of interators into the vrstate
#define SDECL(var_name, presence_type, type_name)	vrtypes::history<type_name, presence_type, ALLOCATOR_TYPE> var_name
// scalar declaration
SDECL(recommended_target_size, AlwaysAndForever, Uint32Size);
// scalar reference/cursor
SCURSOR_DECL(recommended_target_size, AlwaysAndForever, Uint32Size);

// vector of values history
struct 

	// idea:
//
templetize the nodes of the vr state graph 'history graph'
instantiate a peer graph - 'iterator graph'

?instantiate a third graph - history and iterator graph binder
// visit function on the binder to hook the two together


* this has a reference to the history graph node
* this has a current iterator
* InitialState() write a bind visitor - that walks though the graphs in parallel - somehow? how do zip iterators work
  to set the initial state
* ChangeCurrentFrameTime() - walk the iterator graph and update






struct vrstate_cursor
{
	SCURSOR(recommended_target_size, AlwaysAndForever, Uint32Size);
};

void update_cursors(int frame_id)
{
	recommended_target_size.update(frame_id);
	...
}


// idea 2 - use the iterator base as I did for the
// "history_node_if" used in the gui
// typedef history_base<T, P, AllocatorT> history_node;
// typedef typename history_node::iter_type history_iter;

// this pattern:
//history_node_cos
//	-> forward_ref

// idea 2.2
// add_grid_node is able to construct object dynamically for each history node


// idea 3
// build a big vector of iterators
// build a hash table for each functionid->iterator
#endif


void __stdcall VRSystemFnPassalong_GetRecommendedRenderTargetSize(uint32_t * pnWidth, uint32_t * pnHeight)
{
   LOG_ENTRY("FnPassalongGetRecommendedRenderTargetSize");
   interfaces.cpp_interfaces()->sysi->GetRecommendedRenderTargetSize(pnWidth,pnHeight);

   // binding to the node
//   scalar<Uint32Size> &m_history_node = vr_state->system_state.recommended_target_size;

   // binding to the current position
 //  cached_iterator c_iter.start_iterator(start_frame_in, end_frame_in, m_history_node->values.begin(), m_history_node->values.end());

   //

   LOG_EXIT("FnPassalongGetRecommendedRenderTargetSize");
}

struct vr::HmdMatrix44_t __stdcall VRSystemFnPassalong_GetProjectionMatrix(vr::EVREye eEye, float fNearZ, float fFarZ)
{
   LOG_ENTRY("FnPassalongGetProjectionMatrix");
   struct vr::HmdMatrix44_t rc = interfaces.cpp_interfaces()->sysi->GetProjectionMatrix(eEye,fNearZ,fFarZ);
   LOG_EXIT_RC(rc, "FnPassalongGetProjectionMatrix");
}

void __stdcall VRSystemFnPassalong_GetProjectionRaw(vr::EVREye eEye, float * pfLeft, float * pfRight, float * pfTop, float * pfBottom)
{
   LOG_ENTRY("FnPassalongGetProjectionRaw");
   interfaces.cpp_interfaces()->sysi->GetProjectionRaw(eEye,pfLeft,pfRight,pfTop,pfBottom);
   LOG_EXIT("FnPassalongGetProjectionRaw");
}

bool __stdcall VRSystemFnPassalong_ComputeDistortion(vr::EVREye eEye, float fU, float fV, struct vr::DistortionCoordinates_t * pDistortionCoordinates)
{
   LOG_ENTRY("FnPassalongComputeDistortion");
   bool rc = interfaces.cpp_interfaces()->sysi->ComputeDistortion(eEye,fU,fV,pDistortionCoordinates);
   LOG_EXIT_RC(rc, "FnPassalongComputeDistortion");
}

struct vr::HmdMatrix34_t __stdcall VRSystemFnPassalong_GetEyeToHeadTransform(vr::EVREye eEye)
{
   LOG_ENTRY("FnPassalongGetEyeToHeadTransform");
   struct vr::HmdMatrix34_t rc = interfaces.cpp_interfaces()->sysi->GetEyeToHeadTransform(eEye);
   LOG_EXIT_RC(rc, "FnPassalongGetEyeToHeadTransform");
}

bool __stdcall VRSystemFnPassalong_GetTimeSinceLastVsync(float * pfSecondsSinceLastVsync, uint64_t * pulFrameCounter)
{
   LOG_ENTRY("FnPassalongGetTimeSinceLastVsync");
   bool rc = interfaces.cpp_interfaces()->sysi->GetTimeSinceLastVsync(pfSecondsSinceLastVsync,pulFrameCounter);
   LOG_EXIT_RC(rc, "FnPassalongGetTimeSinceLastVsync");
}

int32_t __stdcall VRSystemFnPassalong_GetD3D9AdapterIndex()
{
   LOG_ENTRY("FnPassalongGetD3D9AdapterIndex");
   int32_t rc = interfaces.cpp_interfaces()->sysi->GetD3D9AdapterIndex();
   LOG_EXIT_RC(rc, "FnPassalongGetD3D9AdapterIndex");
}

void __stdcall VRSystemFnPassalong_GetDXGIOutputInfo(int32_t * pnAdapterIndex)
{
   LOG_ENTRY("FnPassalongGetDXGIOutputInfo");
   interfaces.cpp_interfaces()->sysi->GetDXGIOutputInfo(pnAdapterIndex);
   LOG_EXIT("FnPassalongGetDXGIOutputInfo");
}

bool __stdcall VRSystemFnPassalong_IsDisplayOnDesktop()
{
   LOG_ENTRY("FnPassalongIsDisplayOnDesktop");
   bool rc = interfaces.cpp_interfaces()->sysi->IsDisplayOnDesktop();
   LOG_EXIT_RC(rc, "FnPassalongIsDisplayOnDesktop");
}

bool __stdcall VRSystemFnPassalong_SetDisplayVisibility(bool bIsVisibleOnDesktop)
{
   LOG_ENTRY("FnPassalongSetDisplayVisibility");
   bool rc = interfaces.cpp_interfaces()->sysi->SetDisplayVisibility(bIsVisibleOnDesktop);
   LOG_EXIT_RC(rc, "FnPassalongSetDisplayVisibility");
}

void __stdcall VRSystemFnPassalong_GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin eOrigin, float fPredictedSecondsToPhotonsFromNow, struct vr::TrackedDevicePose_t * pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount)
{
   LOG_ENTRY("FnPassalongGetDeviceToAbsoluteTrackingPose");
   interfaces.cpp_interfaces()->sysi->GetDeviceToAbsoluteTrackingPose(eOrigin,fPredictedSecondsToPhotonsFromNow,pTrackedDevicePoseArray,unTrackedDevicePoseArrayCount);
   LOG_EXIT("FnPassalongGetDeviceToAbsoluteTrackingPose");
}

void __stdcall VRSystemFnPassalong_ResetSeatedZeroPose()
{
   LOG_ENTRY("FnPassalongResetSeatedZeroPose");
   interfaces.cpp_interfaces()->sysi->ResetSeatedZeroPose();
   LOG_EXIT("FnPassalongResetSeatedZeroPose");
}

struct vr::HmdMatrix34_t __stdcall VRSystemFnPassalong_GetSeatedZeroPoseToStandingAbsoluteTrackingPose()
{
   LOG_ENTRY("FnPassalongGetSeatedZeroPoseToStandingAbsoluteTrackingPose");
   struct vr::HmdMatrix34_t rc = interfaces.cpp_interfaces()->sysi->GetSeatedZeroPoseToStandingAbsoluteTrackingPose();
   LOG_EXIT_RC(rc, "FnPassalongGetSeatedZeroPoseToStandingAbsoluteTrackingPose");
}

struct vr::HmdMatrix34_t __stdcall VRSystemFnPassalong_GetRawZeroPoseToStandingAbsoluteTrackingPose()
{
   LOG_ENTRY("FnPassalongGetRawZeroPoseToStandingAbsoluteTrackingPose");
   struct vr::HmdMatrix34_t rc = interfaces.cpp_interfaces()->sysi->GetRawZeroPoseToStandingAbsoluteTrackingPose();
   LOG_EXIT_RC(rc, "FnPassalongGetRawZeroPoseToStandingAbsoluteTrackingPose");
}

uint32_t __stdcall VRSystemFnPassalong_GetSortedTrackedDeviceIndicesOfClass(vr::ETrackedDeviceClass eTrackedDeviceClass, vr::TrackedDeviceIndex_t * punTrackedDeviceIndexArray, uint32_t unTrackedDeviceIndexArrayCount, vr::TrackedDeviceIndex_t unRelativeToTrackedDeviceIndex)
{
   LOG_ENTRY("FnPassalongGetSortedTrackedDeviceIndicesOfClass");
   uint32_t rc = interfaces.cpp_interfaces()->sysi->GetSortedTrackedDeviceIndicesOfClass(eTrackedDeviceClass,punTrackedDeviceIndexArray,unTrackedDeviceIndexArrayCount,unRelativeToTrackedDeviceIndex);
   LOG_EXIT_RC(rc, "FnPassalongGetSortedTrackedDeviceIndicesOfClass");
}

vr::EDeviceActivityLevel __stdcall VRSystemFnPassalong_GetTrackedDeviceActivityLevel(vr::TrackedDeviceIndex_t unDeviceId)
{
   LOG_ENTRY("FnPassalongGetTrackedDeviceActivityLevel");
   vr::EDeviceActivityLevel rc = interfaces.cpp_interfaces()->sysi->GetTrackedDeviceActivityLevel(unDeviceId);
   LOG_EXIT_RC(rc, "FnPassalongGetTrackedDeviceActivityLevel");
}

void __stdcall VRSystemFnPassalong_ApplyTransform(struct vr::TrackedDevicePose_t * pOutputPose, const struct vr::TrackedDevicePose_t * pTrackedDevicePose, const struct vr::HmdMatrix34_t * pTransform)
{
   LOG_ENTRY("FnPassalongApplyTransform");
   interfaces.cpp_interfaces()->sysi->ApplyTransform(pOutputPose,pTrackedDevicePose,pTransform);
   LOG_EXIT("FnPassalongApplyTransform");
}

vr::TrackedDeviceIndex_t __stdcall VRSystemFnPassalong_GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole unDeviceType)
{
   LOG_ENTRY("FnPassalongGetTrackedDeviceIndexForControllerRole");
   vr::TrackedDeviceIndex_t rc = interfaces.cpp_interfaces()->sysi->GetTrackedDeviceIndexForControllerRole(unDeviceType);
   LOG_EXIT_RC(rc, "FnPassalongGetTrackedDeviceIndexForControllerRole");
}

vr::ETrackedControllerRole __stdcall VRSystemFnPassalong_GetControllerRoleForTrackedDeviceIndex(vr::TrackedDeviceIndex_t unDeviceIndex)
{
   LOG_ENTRY("FnPassalongGetControllerRoleForTrackedDeviceIndex");
   vr::ETrackedControllerRole rc = interfaces.cpp_interfaces()->sysi->GetControllerRoleForTrackedDeviceIndex(unDeviceIndex);
   LOG_EXIT_RC(rc, "FnPassalongGetControllerRoleForTrackedDeviceIndex");
}

vr::ETrackedDeviceClass __stdcall VRSystemFnPassalong_GetTrackedDeviceClass(vr::TrackedDeviceIndex_t unDeviceIndex)
{
   LOG_ENTRY("FnPassalongGetTrackedDeviceClass");
   vr::ETrackedDeviceClass rc = interfaces.cpp_interfaces()->sysi->GetTrackedDeviceClass(unDeviceIndex);
   LOG_EXIT_RC(rc, "FnPassalongGetTrackedDeviceClass");
}

bool __stdcall VRSystemFnPassalong_IsTrackedDeviceConnected(vr::TrackedDeviceIndex_t unDeviceIndex)
{
   LOG_ENTRY_1("FnPassalongIsTrackedDeviceConnected", unDeviceIndex);
   bool rc = interfaces.cpp_interfaces()->sysi->IsTrackedDeviceConnected(unDeviceIndex);
   LOG_EXIT_RC(rc, "FnPassalongIsTrackedDeviceConnected");
}

bool __stdcall VRSystemFnPassalong_GetBoolTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("FnPassalongGetBoolTrackedDeviceProperty");
   bool rc = interfaces.cpp_interfaces()->sysi->GetBoolTrackedDeviceProperty(unDeviceIndex,prop,pError);
   LOG_EXIT_RC(rc, "FnPassalongGetBoolTrackedDeviceProperty");
}

float __stdcall VRSystemFnPassalong_GetFloatTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("FnPassalongGetFloatTrackedDeviceProperty");
   float rc = interfaces.cpp_interfaces()->sysi->GetFloatTrackedDeviceProperty(unDeviceIndex,prop,pError);
   LOG_EXIT_RC(rc, "FnPassalongGetFloatTrackedDeviceProperty");
}

int32_t __stdcall VRSystemFnPassalong_GetInt32TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("FnPassalongGetInt32TrackedDeviceProperty");
   int32_t rc = interfaces.cpp_interfaces()->sysi->GetInt32TrackedDeviceProperty(unDeviceIndex,prop,pError);
   LOG_EXIT_RC(rc, "FnPassalongGetInt32TrackedDeviceProperty");
}

uint64_t __stdcall VRSystemFnPassalong_GetUint64TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("FnPassalongGetUint64TrackedDeviceProperty");
   uint64_t rc = interfaces.cpp_interfaces()->sysi->GetUint64TrackedDeviceProperty(unDeviceIndex,prop,pError);
   LOG_EXIT_RC(rc, "FnPassalongGetUint64TrackedDeviceProperty");
}

struct vr::HmdMatrix34_t __stdcall VRSystemFnPassalong_GetMatrix34TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("FnPassalongGetMatrix34TrackedDeviceProperty");
   struct vr::HmdMatrix34_t rc = interfaces.cpp_interfaces()->sysi->GetMatrix34TrackedDeviceProperty(unDeviceIndex,prop,pError);
   LOG_EXIT_RC(rc, "FnPassalongGetMatrix34TrackedDeviceProperty");
}

uint32_t __stdcall VRSystemFnPassalong_GetStringTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, char * pchValue, uint32_t unBufferSize, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("FnPassalongGetStringTrackedDeviceProperty");
   uint32_t rc = interfaces.cpp_interfaces()->sysi->GetStringTrackedDeviceProperty(unDeviceIndex,prop,pchValue,unBufferSize,pError);
   LOG_EXIT_RC(rc, "FnPassalongGetStringTrackedDeviceProperty");
}

const char * __stdcall VRSystemFnPassalong_GetPropErrorNameFromEnum(vr::ETrackedPropertyError error)
{
   LOG_ENTRY("FnPassalongGetPropErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->sysi->GetPropErrorNameFromEnum(error);
   LOG_EXIT_RC(rc, "FnPassalongGetPropErrorNameFromEnum");
}

bool __stdcall VRSystemFnPassalong_PollNextEvent(struct vr::VREvent_t * pEvent, uint32_t uncbVREvent)
{
   LOG_ENTRY("FnPassalongPollNextEvent");
   bool rc = interfaces.cpp_interfaces()->sysi->PollNextEvent(pEvent,uncbVREvent);

   if (rc == false && tracker)
   {
	   dprintf("capture\n");
		capture_vr_state(tracker, *interfaces.cpp_interfaces());
   }

   LOG_EXIT_RC(rc, "FnPassalongPollNextEvent");
}

bool __stdcall VRSystemFnPassalong_PollNextEventWithPose(vr::ETrackingUniverseOrigin eOrigin, struct vr::VREvent_t * pEvent, uint32_t uncbVREvent, vr::TrackedDevicePose_t * pTrackedDevicePose)
{
   LOG_ENTRY("FnPassalongPollNextEventWithPose");
   bool rc = interfaces.cpp_interfaces()->sysi->PollNextEventWithPose(eOrigin,pEvent,uncbVREvent,pTrackedDevicePose);
   if (rc == false && tracker)
   {
	   dprintf("capture2\n");
	   capture_vr_state(tracker, *interfaces.cpp_interfaces());
   }
   LOG_EXIT_RC(rc, "FnPassalongPollNextEventWithPose");
}

const char * __stdcall VRSystemFnPassalong_GetEventTypeNameFromEnum(vr::EVREventType eType)
{
   LOG_ENTRY("FnPassalongGetEventTypeNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->sysi->GetEventTypeNameFromEnum(eType);
   LOG_EXIT_RC(rc, "FnPassalongGetEventTypeNameFromEnum");
}

struct vr::HiddenAreaMesh_t __stdcall VRSystemFnPassalong_GetHiddenAreaMesh(vr::EVREye eEye, vr::EHiddenAreaMeshType type)
{
   LOG_ENTRY("FnPassalongGetHiddenAreaMesh");
   struct vr::HiddenAreaMesh_t rc = interfaces.cpp_interfaces()->sysi->GetHiddenAreaMesh(eEye,type);
   LOG_EXIT_RC(rc, "FnPassalongGetHiddenAreaMesh");
}

bool __stdcall VRSystemFnPassalong_GetControllerState(vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t * pControllerState, uint32_t unControllerStateSize)
{
   LOG_ENTRY("FnPassalongGetControllerState");
   bool rc = interfaces.cpp_interfaces()->sysi->GetControllerState(unControllerDeviceIndex,pControllerState,unControllerStateSize);
   LOG_EXIT_RC(rc, "FnPassalongGetControllerState");
}

bool __stdcall VRSystemFnPassalong_GetControllerStateWithPose(vr::ETrackingUniverseOrigin eOrigin, vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t * pControllerState, uint32_t unControllerStateSize, struct vr::TrackedDevicePose_t * pTrackedDevicePose)
{
   LOG_ENTRY("FnPassalongGetControllerStateWithPose");
   bool rc = interfaces.cpp_interfaces()->sysi->GetControllerStateWithPose(eOrigin,unControllerDeviceIndex,pControllerState,unControllerStateSize,pTrackedDevicePose);
   LOG_EXIT_RC(rc, "FnPassalongGetControllerStateWithPose");
}

void __stdcall VRSystemFnPassalong_TriggerHapticPulse(vr::TrackedDeviceIndex_t unControllerDeviceIndex, uint32_t unAxisId, unsigned short usDurationMicroSec)
{
   LOG_ENTRY("FnPassalongTriggerHapticPulse");
   interfaces.cpp_interfaces()->sysi->TriggerHapticPulse(unControllerDeviceIndex,unAxisId,usDurationMicroSec);
   LOG_EXIT("FnPassalongTriggerHapticPulse");
}

const char * __stdcall VRSystemFnPassalong_GetButtonIdNameFromEnum(vr::EVRButtonId eButtonId)
{
   LOG_ENTRY("FnPassalongGetButtonIdNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->sysi->GetButtonIdNameFromEnum(eButtonId);
   LOG_EXIT_RC(rc, "FnPassalongGetButtonIdNameFromEnum");
}

const char * __stdcall VRSystemFnPassalong_GetControllerAxisTypeNameFromEnum(vr::EVRControllerAxisType eAxisType)
{
   LOG_ENTRY("FnPassalongGetControllerAxisTypeNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->sysi->GetControllerAxisTypeNameFromEnum(eAxisType);
   LOG_EXIT_RC(rc, "FnPassalongGetControllerAxisTypeNameFromEnum");
}

bool __stdcall VRSystemFnPassalong_CaptureInputFocus()
{
   LOG_ENTRY("FnPassalongCaptureInputFocus");
   bool rc = interfaces.cpp_interfaces()->sysi->CaptureInputFocus();
   LOG_EXIT_RC(rc, "FnPassalongCaptureInputFocus");
}

void __stdcall VRSystemFnPassalong_ReleaseInputFocus()
{
   LOG_ENTRY("FnPassalongReleaseInputFocus");
   interfaces.cpp_interfaces()->sysi->ReleaseInputFocus();
   LOG_EXIT("FnPassalongReleaseInputFocus");
}

bool __stdcall VRSystemFnPassalong_IsInputFocusCapturedByAnotherProcess()
{
   LOG_ENTRY("FnPassalongIsInputFocusCapturedByAnotherProcess");
   bool rc = interfaces.cpp_interfaces()->sysi->IsInputFocusCapturedByAnotherProcess();
   LOG_EXIT_RC(rc, "FnPassalongIsInputFocusCapturedByAnotherProcess");
}

uint32_t __stdcall VRSystemFnPassalong_DriverDebugRequest(vr::TrackedDeviceIndex_t unDeviceIndex, const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize)
{
   LOG_ENTRY("FnPassalongDriverDebugRequest");
   uint32_t rc = interfaces.cpp_interfaces()->sysi->DriverDebugRequest(unDeviceIndex,pchRequest,pchResponseBuffer,unResponseBufferSize);
   LOG_EXIT_RC(rc, "FnPassalongDriverDebugRequest");
}

vr::EVRFirmwareError __stdcall VRSystemFnPassalong_PerformFirmwareUpdate(vr::TrackedDeviceIndex_t unDeviceIndex)
{
   LOG_ENTRY("FnPassalongPerformFirmwareUpdate");
   vr::EVRFirmwareError rc = interfaces.cpp_interfaces()->sysi->PerformFirmwareUpdate(unDeviceIndex);
   LOG_EXIT_RC(rc, "FnPassalongPerformFirmwareUpdate");
}

void __stdcall VRSystemFnPassalong_AcknowledgeQuit_Exiting()
{
   LOG_ENTRY("FnPassalongAcknowledgeQuit_Exiting");
   interfaces.cpp_interfaces()->sysi->AcknowledgeQuit_Exiting();
   LOG_EXIT("FnPassalongAcknowledgeQuit_Exiting");
}

void __stdcall VRSystemFnPassalong_AcknowledgeQuit_UserPrompt()
{
   LOG_ENTRY("FnPassalongAcknowledgeQuit_UserPrompt");
   interfaces.cpp_interfaces()->sysi->AcknowledgeQuit_UserPrompt();
   LOG_EXIT("FnPassalongAcknowledgeQuit_UserPrompt");
}

void __stdcall VRExtendedDisplayFnPassalong_GetWindowBounds(int32_t * pnX, int32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight)
{
   LOG_ENTRY("FnPassalongGetWindowBounds");
   interfaces.cpp_interfaces()->exdi->GetWindowBounds(pnX,pnY,pnWidth,pnHeight);
   LOG_EXIT("FnPassalongGetWindowBounds");
}

void __stdcall VRExtendedDisplayFnPassalong_GetEyeOutputViewport(vr::EVREye eEye, uint32_t * pnX, uint32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight)
{
   LOG_ENTRY("FnPassalongGetEyeOutputViewport");
   interfaces.cpp_interfaces()->exdi->GetEyeOutputViewport(eEye,pnX,pnY,pnWidth,pnHeight);
   LOG_EXIT("FnPassalongGetEyeOutputViewport");
}

void __stdcall VRExtendedDisplayFnPassalong_GetDXGIOutputInfo(int32_t * pnAdapterIndex, int32_t * pnAdapterOutputIndex)
{
   LOG_ENTRY("FnPassalongGetDXGIOutputInfo");
   interfaces.cpp_interfaces()->exdi->GetDXGIOutputInfo(pnAdapterIndex,pnAdapterOutputIndex);
   LOG_EXIT("FnPassalongGetDXGIOutputInfo");
}

const char * __stdcall VRTrackedCameraFnPassalong_GetCameraErrorNameFromEnum(vr::EVRTrackedCameraError eCameraError)
{
   LOG_ENTRY("FnPassalongGetCameraErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->taci->GetCameraErrorNameFromEnum(eCameraError);
   LOG_EXIT_RC(rc, "FnPassalongGetCameraErrorNameFromEnum");
}

vr::EVRTrackedCameraError __stdcall VRTrackedCameraFnPassalong_HasCamera(vr::TrackedDeviceIndex_t nDeviceIndex, bool * pHasCamera)
{
   LOG_ENTRY("FnPassalongHasCamera");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->HasCamera(nDeviceIndex,pHasCamera);
   LOG_EXIT_RC(rc, "FnPassalongHasCamera");
}

vr::EVRTrackedCameraError __stdcall VRTrackedCameraFnPassalong_GetCameraFrameSize(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType eFrameType, uint32_t * pnWidth, uint32_t * pnHeight, uint32_t * pnFrameBufferSize)
{
   LOG_ENTRY("FnPassalongGetCameraFrameSize");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetCameraFrameSize(nDeviceIndex,eFrameType,pnWidth,pnHeight,pnFrameBufferSize);
   LOG_EXIT_RC(rc, "FnPassalongGetCameraFrameSize");
}

vr::EVRTrackedCameraError __stdcall VRTrackedCameraFnPassalong_GetCameraIntrinsics(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType eFrameType, vr::HmdVector2_t * pFocalLength, vr::HmdVector2_t * pCenter)
{
   LOG_ENTRY("FnPassalongGetCameraIntrinsics");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetCameraIntrinsics(nDeviceIndex,eFrameType,pFocalLength,pCenter);
   LOG_EXIT_RC(rc, "FnPassalongGetCameraIntrinsics");
}

vr::EVRTrackedCameraError __stdcall VRTrackedCameraFnPassalong_GetCameraProjection(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType eFrameType, float flZNear, float flZFar, vr::HmdMatrix44_t * pProjection)
{
   LOG_ENTRY("FnPassalongGetCameraProjection");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetCameraProjection(nDeviceIndex,eFrameType,flZNear,flZFar,pProjection);
   LOG_EXIT_RC(rc, "FnPassalongGetCameraProjection");
}

vr::EVRTrackedCameraError __stdcall VRTrackedCameraFnPassalong_AcquireVideoStreamingService(vr::TrackedDeviceIndex_t nDeviceIndex, vr::TrackedCameraHandle_t * pHandle)
{
   LOG_ENTRY("FnPassalongAcquireVideoStreamingService");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->AcquireVideoStreamingService(nDeviceIndex,pHandle);
   LOG_EXIT_RC(rc, "FnPassalongAcquireVideoStreamingService");
}

vr::EVRTrackedCameraError __stdcall VRTrackedCameraFnPassalong_ReleaseVideoStreamingService(vr::TrackedCameraHandle_t hTrackedCamera)
{
   LOG_ENTRY("FnPassalongReleaseVideoStreamingService");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->ReleaseVideoStreamingService(hTrackedCamera);
   LOG_EXIT_RC(rc, "FnPassalongReleaseVideoStreamingService");
}

vr::EVRTrackedCameraError __stdcall VRTrackedCameraFnPassalong_GetVideoStreamFrameBuffer(vr::TrackedCameraHandle_t hTrackedCamera, vr::EVRTrackedCameraFrameType eFrameType, void * pFrameBuffer, uint32_t nFrameBufferSize, vr::CameraVideoStreamFrameHeader_t * pFrameHeader, uint32_t nFrameHeaderSize)
{
   LOG_ENTRY("FnPassalongGetVideoStreamFrameBuffer");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetVideoStreamFrameBuffer(hTrackedCamera,eFrameType,pFrameBuffer,nFrameBufferSize,pFrameHeader,nFrameHeaderSize);
   LOG_EXIT_RC(rc, "FnPassalongGetVideoStreamFrameBuffer");
}

vr::EVRTrackedCameraError __stdcall VRTrackedCameraFnPassalong_GetVideoStreamTextureSize(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType eFrameType, vr::VRTextureBounds_t * pTextureBounds, uint32_t * pnWidth, uint32_t * pnHeight)
{
   LOG_ENTRY("FnPassalongGetVideoStreamTextureSize");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetVideoStreamTextureSize(nDeviceIndex,eFrameType,pTextureBounds,pnWidth,pnHeight);
   LOG_EXIT_RC(rc, "FnPassalongGetVideoStreamTextureSize");
}

vr::EVRTrackedCameraError __stdcall VRTrackedCameraFnPassalong_GetVideoStreamTextureD3D11(vr::TrackedCameraHandle_t hTrackedCamera, vr::EVRTrackedCameraFrameType eFrameType, void * pD3D11DeviceOrResource, void ** ppD3D11ShaderResourceView, vr::CameraVideoStreamFrameHeader_t * pFrameHeader, uint32_t nFrameHeaderSize)
{
   LOG_ENTRY("FnPassalongGetVideoStreamTextureD3D11");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetVideoStreamTextureD3D11(hTrackedCamera,eFrameType,pD3D11DeviceOrResource,ppD3D11ShaderResourceView,pFrameHeader,nFrameHeaderSize);
   LOG_EXIT_RC(rc, "FnPassalongGetVideoStreamTextureD3D11");
}

vr::EVRTrackedCameraError __stdcall VRTrackedCameraFnPassalong_GetVideoStreamTextureGL(vr::TrackedCameraHandle_t hTrackedCamera, vr::EVRTrackedCameraFrameType eFrameType, vr::glUInt_t * pglTextureId, vr::CameraVideoStreamFrameHeader_t * pFrameHeader, uint32_t nFrameHeaderSize)
{
   LOG_ENTRY("FnPassalongGetVideoStreamTextureGL");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetVideoStreamTextureGL(hTrackedCamera,eFrameType,pglTextureId,pFrameHeader,nFrameHeaderSize);
   LOG_EXIT_RC(rc, "FnPassalongGetVideoStreamTextureGL");
}

vr::EVRTrackedCameraError __stdcall VRTrackedCameraFnPassalong_ReleaseVideoStreamTextureGL(vr::TrackedCameraHandle_t hTrackedCamera, vr::glUInt_t glTextureId)
{
   LOG_ENTRY("FnPassalongReleaseVideoStreamTextureGL");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->ReleaseVideoStreamTextureGL(hTrackedCamera,glTextureId);
   LOG_EXIT_RC(rc, "FnPassalongReleaseVideoStreamTextureGL");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_AddApplicationManifest(const char * pchApplicationManifestFullPath, bool bTemporary)
{
   LOG_ENTRY("FnPassalongAddApplicationManifest");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->AddApplicationManifest(pchApplicationManifestFullPath,bTemporary);
   LOG_EXIT_RC(rc, "FnPassalongAddApplicationManifest");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_RemoveApplicationManifest(const char * pchApplicationManifestFullPath)
{
   LOG_ENTRY("FnPassalongRemoveApplicationManifest");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->RemoveApplicationManifest(pchApplicationManifestFullPath);
   LOG_EXIT_RC(rc, "FnPassalongRemoveApplicationManifest");
}

bool __stdcall VRApplicationsFnPassalong_IsApplicationInstalled(const char * pchAppKey)
{
   LOG_ENTRY("FnPassalongIsApplicationInstalled");
   bool rc = interfaces.cpp_interfaces()->appi->IsApplicationInstalled(pchAppKey);
   LOG_EXIT_RC(rc, "FnPassalongIsApplicationInstalled");
}

uint32_t __stdcall VRApplicationsFnPassalong_GetApplicationCount()
{
   LOG_ENTRY("FnPassalongGetApplicationCount");
   uint32_t rc = interfaces.cpp_interfaces()->appi->GetApplicationCount();
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationCount");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_GetApplicationKeyByIndex(uint32_t unApplicationIndex, char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
   LOG_ENTRY("FnPassalongGetApplicationKeyByIndex");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->GetApplicationKeyByIndex(unApplicationIndex,pchAppKeyBuffer,unAppKeyBufferLen);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationKeyByIndex");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_GetApplicationKeyByProcessId(uint32_t unProcessId, char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
   LOG_ENTRY("FnPassalongGetApplicationKeyByProcessId");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->GetApplicationKeyByProcessId(unProcessId,pchAppKeyBuffer,unAppKeyBufferLen);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationKeyByProcessId");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_LaunchApplication(const char * pchAppKey)
{
   LOG_ENTRY("FnPassalongLaunchApplication");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->LaunchApplication(pchAppKey);
   LOG_EXIT_RC(rc, "FnPassalongLaunchApplication");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_LaunchTemplateApplication(const char * pchTemplateAppKey, const char * pchNewAppKey, const struct vr::AppOverrideKeys_t * pKeys, uint32_t unKeys)
{
   LOG_ENTRY("FnPassalongLaunchTemplateApplication");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->LaunchTemplateApplication(pchTemplateAppKey,pchNewAppKey,pKeys,unKeys);
   LOG_EXIT_RC(rc, "FnPassalongLaunchTemplateApplication");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_LaunchApplicationFromMimeType(const char * pchMimeType, const char * pchArgs)
{
   LOG_ENTRY("FnPassalongLaunchApplicationFromMimeType");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->LaunchApplicationFromMimeType(pchMimeType,pchArgs);
   LOG_EXIT_RC(rc, "FnPassalongLaunchApplicationFromMimeType");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_LaunchDashboardOverlay(const char * pchAppKey)
{
   LOG_ENTRY("FnPassalongLaunchDashboardOverlay");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->LaunchDashboardOverlay(pchAppKey);
   LOG_EXIT_RC(rc, "FnPassalongLaunchDashboardOverlay");
}

bool __stdcall VRApplicationsFnPassalong_CancelApplicationLaunch(const char * pchAppKey)
{
   LOG_ENTRY("FnPassalongCancelApplicationLaunch");
   bool rc = interfaces.cpp_interfaces()->appi->CancelApplicationLaunch(pchAppKey);
   LOG_EXIT_RC(rc, "FnPassalongCancelApplicationLaunch");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_IdentifyApplication(uint32_t unProcessId, const char * pchAppKey)
{
   LOG_ENTRY("FnPassalongIdentifyApplication");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->IdentifyApplication(unProcessId,pchAppKey);
   LOG_EXIT_RC(rc, "FnPassalongIdentifyApplication");
}

uint32_t __stdcall VRApplicationsFnPassalong_GetApplicationProcessId(const char * pchAppKey)
{
   LOG_ENTRY("FnPassalongGetApplicationProcessId");
   uint32_t rc = interfaces.cpp_interfaces()->appi->GetApplicationProcessId(pchAppKey);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationProcessId");
}

const char * __stdcall VRApplicationsFnPassalong_GetApplicationsErrorNameFromEnum(vr::EVRApplicationError error)
{
   LOG_ENTRY("FnPassalongGetApplicationsErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->appi->GetApplicationsErrorNameFromEnum(error);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationsErrorNameFromEnum");
}

uint32_t __stdcall VRApplicationsFnPassalong_GetApplicationPropertyString(const char * pchAppKey, vr::EVRApplicationProperty eProperty, char * pchPropertyValueBuffer, uint32_t unPropertyValueBufferLen, vr::EVRApplicationError * peError)
{
   LOG_ENTRY("FnPassalongGetApplicationPropertyString");
   uint32_t rc = interfaces.cpp_interfaces()->appi->GetApplicationPropertyString(pchAppKey,eProperty,pchPropertyValueBuffer,unPropertyValueBufferLen,peError);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationPropertyString");
}

bool __stdcall VRApplicationsFnPassalong_GetApplicationPropertyBool(const char * pchAppKey, vr::EVRApplicationProperty eProperty, vr::EVRApplicationError * peError)
{
   LOG_ENTRY("FnPassalongGetApplicationPropertyBool");
   bool rc = interfaces.cpp_interfaces()->appi->GetApplicationPropertyBool(pchAppKey,eProperty,peError);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationPropertyBool");
}

uint64_t __stdcall VRApplicationsFnPassalong_GetApplicationPropertyUint64(const char * pchAppKey, vr::EVRApplicationProperty eProperty, vr::EVRApplicationError * peError)
{
   LOG_ENTRY("FnPassalongGetApplicationPropertyUint64");
   uint64_t rc = interfaces.cpp_interfaces()->appi->GetApplicationPropertyUint64(pchAppKey,eProperty,peError);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationPropertyUint64");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_SetApplicationAutoLaunch(const char * pchAppKey, bool bAutoLaunch)
{
   LOG_ENTRY("FnPassalongSetApplicationAutoLaunch");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->SetApplicationAutoLaunch(pchAppKey,bAutoLaunch);
   LOG_EXIT_RC(rc, "FnPassalongSetApplicationAutoLaunch");
}

bool __stdcall VRApplicationsFnPassalong_GetApplicationAutoLaunch(const char * pchAppKey)
{
   LOG_ENTRY("FnPassalongGetApplicationAutoLaunch");
   bool rc = interfaces.cpp_interfaces()->appi->GetApplicationAutoLaunch(pchAppKey);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationAutoLaunch");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_SetDefaultApplicationForMimeType(const char * pchAppKey, const char * pchMimeType)
{
   LOG_ENTRY("FnPassalongSetDefaultApplicationForMimeType");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->SetDefaultApplicationForMimeType(pchAppKey,pchMimeType);
   LOG_EXIT_RC(rc, "FnPassalongSetDefaultApplicationForMimeType");
}

bool __stdcall VRApplicationsFnPassalong_GetDefaultApplicationForMimeType(const char * pchMimeType, char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
   LOG_ENTRY("FnPassalongGetDefaultApplicationForMimeType");
   bool rc = interfaces.cpp_interfaces()->appi->GetDefaultApplicationForMimeType(pchMimeType,pchAppKeyBuffer,unAppKeyBufferLen);
   LOG_EXIT_RC(rc, "FnPassalongGetDefaultApplicationForMimeType");
}

bool __stdcall VRApplicationsFnPassalong_GetApplicationSupportedMimeTypes(const char * pchAppKey, char * pchMimeTypesBuffer, uint32_t unMimeTypesBuffer)
{
   LOG_ENTRY("FnPassalongGetApplicationSupportedMimeTypes");
   bool rc = interfaces.cpp_interfaces()->appi->GetApplicationSupportedMimeTypes(pchAppKey,pchMimeTypesBuffer,unMimeTypesBuffer);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationSupportedMimeTypes");
}

uint32_t __stdcall VRApplicationsFnPassalong_GetApplicationsThatSupportMimeType(const char * pchMimeType, char * pchAppKeysThatSupportBuffer, uint32_t unAppKeysThatSupportBuffer)
{
   LOG_ENTRY("FnPassalongGetApplicationsThatSupportMimeType");
   uint32_t rc = interfaces.cpp_interfaces()->appi->GetApplicationsThatSupportMimeType(pchMimeType,pchAppKeysThatSupportBuffer,unAppKeysThatSupportBuffer);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationsThatSupportMimeType");
}

uint32_t __stdcall VRApplicationsFnPassalong_GetApplicationLaunchArguments(uint32_t unHandle, char * pchArgs, uint32_t unArgs)
{
   LOG_ENTRY("FnPassalongGetApplicationLaunchArguments");
   uint32_t rc = interfaces.cpp_interfaces()->appi->GetApplicationLaunchArguments(unHandle,pchArgs,unArgs);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationLaunchArguments");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_GetStartingApplication(char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
   LOG_ENTRY("FnPassalongGetStartingApplication");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->GetStartingApplication(pchAppKeyBuffer,unAppKeyBufferLen);
   LOG_EXIT_RC(rc, "FnPassalongGetStartingApplication");
}

vr::EVRApplicationTransitionState __stdcall VRApplicationsFnPassalong_GetTransitionState()
{
   LOG_ENTRY("FnPassalongGetTransitionState");
   vr::EVRApplicationTransitionState rc = interfaces.cpp_interfaces()->appi->GetTransitionState();
   LOG_EXIT_RC(rc, "FnPassalongGetTransitionState");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_PerformApplicationPrelaunchCheck(const char * pchAppKey)
{
   LOG_ENTRY("FnPassalongPerformApplicationPrelaunchCheck");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->PerformApplicationPrelaunchCheck(pchAppKey);
   LOG_EXIT_RC(rc, "FnPassalongPerformApplicationPrelaunchCheck");
}

const char * __stdcall VRApplicationsFnPassalong_GetApplicationsTransitionStateNameFromEnum(vr::EVRApplicationTransitionState state)
{
   LOG_ENTRY("FnPassalongGetApplicationsTransitionStateNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->appi->GetApplicationsTransitionStateNameFromEnum(state);
   LOG_EXIT_RC(rc, "FnPassalongGetApplicationsTransitionStateNameFromEnum");
}

bool __stdcall VRApplicationsFnPassalong_IsQuitUserPromptRequested()
{
   LOG_ENTRY("FnPassalongIsQuitUserPromptRequested");
   bool rc = interfaces.cpp_interfaces()->appi->IsQuitUserPromptRequested();
   LOG_EXIT_RC(rc, "FnPassalongIsQuitUserPromptRequested");
}

vr::EVRApplicationError __stdcall VRApplicationsFnPassalong_LaunchInternalProcess(const char * pchBinaryPath, const char * pchArguments, const char * pchWorkingDirectory)
{
   LOG_ENTRY("FnPassalongLaunchInternalProcess");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->LaunchInternalProcess(pchBinaryPath,pchArguments,pchWorkingDirectory);
   LOG_EXIT_RC(rc, "FnPassalongLaunchInternalProcess");
}

vr::ChaperoneCalibrationState __stdcall VRChaperoneFnPassalong_GetCalibrationState()
{
   LOG_ENTRY("FnPassalongGetCalibrationState");
   vr::ChaperoneCalibrationState rc = interfaces.cpp_interfaces()->chapi->GetCalibrationState();
   LOG_EXIT_RC(rc, "FnPassalongGetCalibrationState");
}

bool __stdcall VRChaperoneFnPassalong_GetPlayAreaSize(float * pSizeX, float * pSizeZ)
{
   LOG_ENTRY("FnPassalongGetPlayAreaSize");
   bool rc = interfaces.cpp_interfaces()->chapi->GetPlayAreaSize(pSizeX,pSizeZ);
   LOG_EXIT_RC(rc, "FnPassalongGetPlayAreaSize");
}

bool __stdcall VRChaperoneFnPassalong_GetPlayAreaRect(struct vr::HmdQuad_t * rect)
{
   LOG_ENTRY("FnPassalongGetPlayAreaRect");
   bool rc = interfaces.cpp_interfaces()->chapi->GetPlayAreaRect(rect);
   LOG_EXIT_RC(rc, "FnPassalongGetPlayAreaRect");
}

void __stdcall VRChaperoneFnPassalong_ReloadInfo()
{
   LOG_ENTRY("FnPassalongReloadInfo");
   interfaces.cpp_interfaces()->chapi->ReloadInfo();
   LOG_EXIT("FnPassalongReloadInfo");
}

void __stdcall VRChaperoneFnPassalong_SetSceneColor(struct vr::HmdColor_t color)
{
   LOG_ENTRY("FnPassalongSetSceneColor");
   interfaces.cpp_interfaces()->chapi->SetSceneColor(color);
   LOG_EXIT("FnPassalongSetSceneColor");
}

void __stdcall VRChaperoneFnPassalong_GetBoundsColor(struct vr::HmdColor_t * pOutputColorArray, int nNumOutputColors, float flCollisionBoundsFadeDistance, struct vr::HmdColor_t * pOutputCameraColor)
{
   LOG_ENTRY("FnPassalongGetBoundsColor");
   interfaces.cpp_interfaces()->chapi->GetBoundsColor(pOutputColorArray,nNumOutputColors,flCollisionBoundsFadeDistance,pOutputCameraColor);
   LOG_EXIT("FnPassalongGetBoundsColor");
}

bool __stdcall VRChaperoneFnPassalong_AreBoundsVisible()
{
   LOG_ENTRY("FnPassalongAreBoundsVisible");
   bool rc = interfaces.cpp_interfaces()->chapi->AreBoundsVisible();
   LOG_EXIT_RC(rc, "FnPassalongAreBoundsVisible");
}

void __stdcall VRChaperoneFnPassalong_ForceBoundsVisible(bool bForce)
{
   LOG_ENTRY("FnPassalongForceBoundsVisible");
   interfaces.cpp_interfaces()->chapi->ForceBoundsVisible(bForce);
   LOG_EXIT("FnPassalongForceBoundsVisible");
}

bool __stdcall VRChaperoneSetupFnPassalong_CommitWorkingCopy(vr::EChaperoneConfigFile configFile)
{
   LOG_ENTRY("FnPassalongCommitWorkingCopy");
   bool rc = interfaces.cpp_interfaces()->chapsi->CommitWorkingCopy(configFile);
   LOG_EXIT_RC(rc, "FnPassalongCommitWorkingCopy");
}

void __stdcall VRChaperoneSetupFnPassalong_RevertWorkingCopy()
{
   LOG_ENTRY("FnPassalongRevertWorkingCopy");
   interfaces.cpp_interfaces()->chapsi->RevertWorkingCopy();
   LOG_EXIT("FnPassalongRevertWorkingCopy");
}

bool __stdcall VRChaperoneSetupFnPassalong_GetWorkingPlayAreaSize(float * pSizeX, float * pSizeZ)
{
   LOG_ENTRY("FnPassalongGetWorkingPlayAreaSize");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetWorkingPlayAreaSize(pSizeX,pSizeZ);
   LOG_EXIT_RC(rc, "FnPassalongGetWorkingPlayAreaSize");
}

bool __stdcall VRChaperoneSetupFnPassalong_GetWorkingPlayAreaRect(struct vr::HmdQuad_t * rect)
{
   LOG_ENTRY("FnPassalongGetWorkingPlayAreaRect");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetWorkingPlayAreaRect(rect);
   LOG_EXIT_RC(rc, "FnPassalongGetWorkingPlayAreaRect");
}

bool __stdcall VRChaperoneSetupFnPassalong_GetWorkingCollisionBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t * punQuadsCount)
{
   LOG_ENTRY("FnPassalongGetWorkingCollisionBoundsInfo");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetWorkingCollisionBoundsInfo(pQuadsBuffer,punQuadsCount);
   LOG_EXIT_RC(rc, "FnPassalongGetWorkingCollisionBoundsInfo");
}

bool __stdcall VRChaperoneSetupFnPassalong_GetLiveCollisionBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t * punQuadsCount)
{
   LOG_ENTRY("FnPassalongGetLiveCollisionBoundsInfo");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetLiveCollisionBoundsInfo(pQuadsBuffer,punQuadsCount);
   LOG_EXIT_RC(rc, "FnPassalongGetLiveCollisionBoundsInfo");
}

bool __stdcall VRChaperoneSetupFnPassalong_GetWorkingSeatedZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pmatSeatedZeroPoseToRawTrackingPose)
{
   LOG_ENTRY("FnPassalongGetWorkingSeatedZeroPoseToRawTrackingPose");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetWorkingSeatedZeroPoseToRawTrackingPose(pmatSeatedZeroPoseToRawTrackingPose);
   LOG_EXIT_RC(rc, "FnPassalongGetWorkingSeatedZeroPoseToRawTrackingPose");
}

bool __stdcall VRChaperoneSetupFnPassalong_GetWorkingStandingZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pmatStandingZeroPoseToRawTrackingPose)
{
   LOG_ENTRY("FnPassalongGetWorkingStandingZeroPoseToRawTrackingPose");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetWorkingStandingZeroPoseToRawTrackingPose(pmatStandingZeroPoseToRawTrackingPose);
   LOG_EXIT_RC(rc, "FnPassalongGetWorkingStandingZeroPoseToRawTrackingPose");
}

void __stdcall VRChaperoneSetupFnPassalong_SetWorkingPlayAreaSize(float sizeX, float sizeZ)
{
   LOG_ENTRY("FnPassalongSetWorkingPlayAreaSize");
   interfaces.cpp_interfaces()->chapsi->SetWorkingPlayAreaSize(sizeX,sizeZ);
   LOG_EXIT("FnPassalongSetWorkingPlayAreaSize");
}

void __stdcall VRChaperoneSetupFnPassalong_SetWorkingCollisionBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t unQuadsCount)
{
   LOG_ENTRY("FnPassalongSetWorkingCollisionBoundsInfo");
   interfaces.cpp_interfaces()->chapsi->SetWorkingCollisionBoundsInfo(pQuadsBuffer,unQuadsCount);
   LOG_EXIT("FnPassalongSetWorkingCollisionBoundsInfo");
}

void __stdcall VRChaperoneSetupFnPassalong_SetWorkingSeatedZeroPoseToRawTrackingPose(const struct vr::HmdMatrix34_t * pMatSeatedZeroPoseToRawTrackingPose)
{
   LOG_ENTRY("FnPassalongSetWorkingSeatedZeroPoseToRawTrackingPose");
   interfaces.cpp_interfaces()->chapsi->SetWorkingSeatedZeroPoseToRawTrackingPose(pMatSeatedZeroPoseToRawTrackingPose);
   LOG_EXIT("FnPassalongSetWorkingSeatedZeroPoseToRawTrackingPose");
}

void __stdcall VRChaperoneSetupFnPassalong_SetWorkingStandingZeroPoseToRawTrackingPose(const struct vr::HmdMatrix34_t * pMatStandingZeroPoseToRawTrackingPose)
{
   LOG_ENTRY("FnPassalongSetWorkingStandingZeroPoseToRawTrackingPose");
   interfaces.cpp_interfaces()->chapsi->SetWorkingStandingZeroPoseToRawTrackingPose(pMatStandingZeroPoseToRawTrackingPose);
   LOG_EXIT("FnPassalongSetWorkingStandingZeroPoseToRawTrackingPose");
}

void __stdcall VRChaperoneSetupFnPassalong_ReloadFromDisk(vr::EChaperoneConfigFile configFile)
{
   LOG_ENTRY("FnPassalongReloadFromDisk");
   interfaces.cpp_interfaces()->chapsi->ReloadFromDisk(configFile);
   LOG_EXIT("FnPassalongReloadFromDisk");
}

bool __stdcall VRChaperoneSetupFnPassalong_GetLiveSeatedZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pmatSeatedZeroPoseToRawTrackingPose)
{
   LOG_ENTRY("FnPassalongGetLiveSeatedZeroPoseToRawTrackingPose");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(pmatSeatedZeroPoseToRawTrackingPose);
   LOG_EXIT_RC(rc, "FnPassalongGetLiveSeatedZeroPoseToRawTrackingPose");
}

void __stdcall VRChaperoneSetupFnPassalong_SetWorkingCollisionBoundsTagsInfo(uint8_t * pTagsBuffer, uint32_t unTagCount)
{
   LOG_ENTRY("FnPassalongSetWorkingCollisionBoundsTagsInfo");
   interfaces.cpp_interfaces()->chapsi->SetWorkingCollisionBoundsTagsInfo(pTagsBuffer,unTagCount);
   LOG_EXIT("FnPassalongSetWorkingCollisionBoundsTagsInfo");
}

bool __stdcall VRChaperoneSetupFnPassalong_GetLiveCollisionBoundsTagsInfo(uint8_t * pTagsBuffer, uint32_t * punTagCount)
{
   LOG_ENTRY("FnPassalongGetLiveCollisionBoundsTagsInfo");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetLiveCollisionBoundsTagsInfo(pTagsBuffer,punTagCount);
   LOG_EXIT_RC(rc, "FnPassalongGetLiveCollisionBoundsTagsInfo");
}

bool __stdcall VRChaperoneSetupFnPassalong_SetWorkingPhysicalBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t unQuadsCount)
{
   LOG_ENTRY("FnPassalongSetWorkingPhysicalBoundsInfo");
   bool rc = interfaces.cpp_interfaces()->chapsi->SetWorkingPhysicalBoundsInfo(pQuadsBuffer,unQuadsCount);
   LOG_EXIT_RC(rc, "FnPassalongSetWorkingPhysicalBoundsInfo");
}

bool __stdcall VRChaperoneSetupFnPassalong_GetLivePhysicalBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t * punQuadsCount)
{
   LOG_ENTRY("FnPassalongGetLivePhysicalBoundsInfo");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetLivePhysicalBoundsInfo(pQuadsBuffer,punQuadsCount);
   LOG_EXIT_RC(rc, "FnPassalongGetLivePhysicalBoundsInfo");
}

bool __stdcall VRChaperoneSetupFnPassalong_ExportLiveToBuffer(char * pBuffer, uint32_t * pnBufferLength)
{
   LOG_ENTRY("FnPassalongExportLiveToBuffer");
   bool rc = interfaces.cpp_interfaces()->chapsi->ExportLiveToBuffer(pBuffer,pnBufferLength);
   LOG_EXIT_RC(rc, "FnPassalongExportLiveToBuffer");
}

bool __stdcall VRChaperoneSetupFnPassalong_ImportFromBufferToWorking(const char * pBuffer, uint32_t nImportFlags)
{
   LOG_ENTRY("FnPassalongImportFromBufferToWorking");
   bool rc = interfaces.cpp_interfaces()->chapsi->ImportFromBufferToWorking(pBuffer,nImportFlags);
   LOG_EXIT_RC(rc, "FnPassalongImportFromBufferToWorking");
}

void __stdcall VRCompositorFnPassalong_SetTrackingSpace(vr::ETrackingUniverseOrigin eOrigin)
{
   LOG_ENTRY("FnPassalongSetTrackingSpace");
   interfaces.cpp_interfaces()->compi->SetTrackingSpace(eOrigin);
   LOG_EXIT("FnPassalongSetTrackingSpace");
}

vr::ETrackingUniverseOrigin __stdcall VRCompositorFnPassalong_GetTrackingSpace()
{
   LOG_ENTRY("FnPassalongGetTrackingSpace");
   vr::ETrackingUniverseOrigin rc = interfaces.cpp_interfaces()->compi->GetTrackingSpace();
   LOG_EXIT_RC(rc, "FnPassalongGetTrackingSpace");
}

vr::EVRCompositorError __stdcall VRCompositorFnPassalong_WaitGetPoses(struct vr::TrackedDevicePose_t * pRenderPoseArray, uint32_t unRenderPoseArrayCount, struct vr::TrackedDevicePose_t * pGamePoseArray, uint32_t unGamePoseArrayCount)
{
   LOG_ENTRY("FnPassalongWaitGetPoses");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->WaitGetPoses(pRenderPoseArray,unRenderPoseArrayCount,pGamePoseArray,unGamePoseArrayCount);
   LOG_EXIT_RC(rc, "FnPassalongWaitGetPoses");
}

vr::EVRCompositorError __stdcall VRCompositorFnPassalong_GetLastPoses(struct vr::TrackedDevicePose_t * pRenderPoseArray, uint32_t unRenderPoseArrayCount, struct vr::TrackedDevicePose_t * pGamePoseArray, uint32_t unGamePoseArrayCount)
{
	static int i; 
	dprintf("beep %d\n", i++);
   LOG_ENTRY("FnPassalongGetLastPoses");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->GetLastPoses(pRenderPoseArray,unRenderPoseArrayCount,pGamePoseArray,unGamePoseArrayCount);
   LOG_EXIT_RC(rc, "FnPassalongGetLastPoses");
}

vr::EVRCompositorError __stdcall VRCompositorFnPassalong_GetLastPoseForTrackedDeviceIndex(vr::TrackedDeviceIndex_t unDeviceIndex, struct vr::TrackedDevicePose_t * pOutputPose, struct vr::TrackedDevicePose_t * pOutputGamePose)
{
	dprintf("boop\n");
   LOG_ENTRY("FnPassalongGetLastPoseForTrackedDeviceIndex");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->GetLastPoseForTrackedDeviceIndex(unDeviceIndex,pOutputPose,pOutputGamePose);
   LOG_EXIT_RC(rc, "FnPassalongGetLastPoseForTrackedDeviceIndex");
}

vr::EVRCompositorError __stdcall VRCompositorFnPassalong_Submit(vr::EVREye eEye, const struct vr::Texture_t * pTexture, const struct vr::VRTextureBounds_t * pBounds, vr::EVRSubmitFlags nSubmitFlags)
{
   LOG_ENTRY("FnPassalongSubmit");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->Submit(eEye,pTexture,pBounds,nSubmitFlags);
   LOG_EXIT_RC(rc, "FnPassalongSubmit");
}

void __stdcall VRCompositorFnPassalong_ClearLastSubmittedFrame()
{
   LOG_ENTRY("FnPassalongClearLastSubmittedFrame");
   interfaces.cpp_interfaces()->compi->ClearLastSubmittedFrame();
   LOG_EXIT("FnPassalongClearLastSubmittedFrame");
}

void __stdcall VRCompositorFnPassalong_PostPresentHandoff()
{
   LOG_ENTRY("FnPassalongPostPresentHandoff");
   interfaces.cpp_interfaces()->compi->PostPresentHandoff();
   LOG_EXIT("FnPassalongPostPresentHandoff");
}

bool __stdcall VRCompositorFnPassalong_GetFrameTiming(struct vr::Compositor_FrameTiming * pTiming, uint32_t unFramesAgo)
{
   LOG_ENTRY("FnPassalongGetFrameTiming");
   bool rc = interfaces.cpp_interfaces()->compi->GetFrameTiming(pTiming,unFramesAgo);
   LOG_EXIT_RC(rc, "FnPassalongGetFrameTiming");
}

uint32_t __stdcall VRCompositorFnPassalong_GetFrameTimings(struct vr::Compositor_FrameTiming * pTiming, uint32_t nFrames)
{
   LOG_ENTRY("FnPassalongGetFrameTimings");
   uint32_t rc = interfaces.cpp_interfaces()->compi->GetFrameTimings(pTiming,nFrames);
   LOG_EXIT_RC(rc, "FnPassalongGetFrameTimings");
}

float __stdcall VRCompositorFnPassalong_GetFrameTimeRemaining()
{
   LOG_ENTRY("FnPassalongGetFrameTimeRemaining");
   float rc = interfaces.cpp_interfaces()->compi->GetFrameTimeRemaining();
   LOG_EXIT_RC(rc, "FnPassalongGetFrameTimeRemaining");
}

void __stdcall VRCompositorFnPassalong_GetCumulativeStats(struct vr::Compositor_CumulativeStats * pStats, uint32_t nStatsSizeInBytes)
{
   LOG_ENTRY("FnPassalongGetCumulativeStats");
   interfaces.cpp_interfaces()->compi->GetCumulativeStats(pStats,nStatsSizeInBytes);
   LOG_EXIT("FnPassalongGetCumulativeStats");
}

void __stdcall VRCompositorFnPassalong_FadeToColor(float fSeconds, float fRed, float fGreen, float fBlue, float fAlpha, bool bBackground)
{
   LOG_ENTRY("FnPassalongFadeToColor");
   interfaces.cpp_interfaces()->compi->FadeToColor(fSeconds,fRed,fGreen,fBlue,fAlpha,bBackground);
   LOG_EXIT("FnPassalongFadeToColor");
}

struct vr::HmdColor_t __stdcall VRCompositorFnPassalong_GetCurrentFadeColor(bool bBackground)
{
   LOG_ENTRY("FnPassalongGetCurrentFadeColor");
   struct vr::HmdColor_t rc = interfaces.cpp_interfaces()->compi->GetCurrentFadeColor(bBackground);
   LOG_EXIT_RC(rc, "FnPassalongGetCurrentFadeColor");
}

void __stdcall VRCompositorFnPassalong_FadeGrid(float fSeconds, bool bFadeIn)
{
   LOG_ENTRY("FnPassalongFadeGrid");
   interfaces.cpp_interfaces()->compi->FadeGrid(fSeconds,bFadeIn);
   LOG_EXIT("FnPassalongFadeGrid");
}

float __stdcall VRCompositorFnPassalong_GetCurrentGridAlpha()
{
   LOG_ENTRY("FnPassalongGetCurrentGridAlpha");
   float rc = interfaces.cpp_interfaces()->compi->GetCurrentGridAlpha();
   LOG_EXIT_RC(rc, "FnPassalongGetCurrentGridAlpha");
}

vr::EVRCompositorError __stdcall VRCompositorFnPassalong_SetSkyboxOverride(const struct vr::Texture_t * pTextures, uint32_t unTextureCount)
{
   LOG_ENTRY("FnPassalongSetSkyboxOverride");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->SetSkyboxOverride(pTextures,unTextureCount);
   LOG_EXIT_RC(rc, "FnPassalongSetSkyboxOverride");
}

void __stdcall VRCompositorFnPassalong_ClearSkyboxOverride()
{
   LOG_ENTRY("FnPassalongClearSkyboxOverride");
   interfaces.cpp_interfaces()->compi->ClearSkyboxOverride();
   LOG_EXIT("FnPassalongClearSkyboxOverride");
}

void __stdcall VRCompositorFnPassalong_CompositorBringToFront()
{
   LOG_ENTRY("FnPassalongCompositorBringToFront");
   interfaces.cpp_interfaces()->compi->CompositorBringToFront();
   LOG_EXIT("FnPassalongCompositorBringToFront");
}

void __stdcall VRCompositorFnPassalong_CompositorGoToBack()
{
   LOG_ENTRY("FnPassalongCompositorGoToBack");
   interfaces.cpp_interfaces()->compi->CompositorGoToBack();
   LOG_EXIT("FnPassalongCompositorGoToBack");
}

void __stdcall VRCompositorFnPassalong_CompositorQuit()
{
   LOG_ENTRY("FnPassalongCompositorQuit");
   interfaces.cpp_interfaces()->compi->CompositorQuit();
   LOG_EXIT("FnPassalongCompositorQuit");
}

bool __stdcall VRCompositorFnPassalong_IsFullscreen()
{
   LOG_ENTRY("FnPassalongIsFullscreen");
   bool rc = interfaces.cpp_interfaces()->compi->IsFullscreen();
   LOG_EXIT_RC(rc, "FnPassalongIsFullscreen");
}

uint32_t __stdcall VRCompositorFnPassalong_GetCurrentSceneFocusProcess()
{
   LOG_ENTRY("FnPassalongGetCurrentSceneFocusProcess");
   uint32_t rc = interfaces.cpp_interfaces()->compi->GetCurrentSceneFocusProcess();
   LOG_EXIT_RC(rc, "FnPassalongGetCurrentSceneFocusProcess");
}

uint32_t __stdcall VRCompositorFnPassalong_GetLastFrameRenderer()
{
   LOG_ENTRY("FnPassalongGetLastFrameRenderer");
   uint32_t rc = interfaces.cpp_interfaces()->compi->GetLastFrameRenderer();
   LOG_EXIT_RC(rc, "FnPassalongGetLastFrameRenderer");
}

bool __stdcall VRCompositorFnPassalong_CanRenderScene()
{
   LOG_ENTRY("FnPassalongCanRenderScene");
   bool rc = interfaces.cpp_interfaces()->compi->CanRenderScene();
   LOG_EXIT_RC(rc, "FnPassalongCanRenderScene");
}

void __stdcall VRCompositorFnPassalong_ShowMirrorWindow()
{
   LOG_ENTRY("FnPassalongShowMirrorWindow");
   interfaces.cpp_interfaces()->compi->ShowMirrorWindow();
   LOG_EXIT("FnPassalongShowMirrorWindow");
}

void __stdcall VRCompositorFnPassalong_HideMirrorWindow()
{
   LOG_ENTRY("FnPassalongHideMirrorWindow");
   interfaces.cpp_interfaces()->compi->HideMirrorWindow();
   LOG_EXIT("FnPassalongHideMirrorWindow");
}

bool __stdcall VRCompositorFnPassalong_IsMirrorWindowVisible()
{
   LOG_ENTRY("FnPassalongIsMirrorWindowVisible");
   bool rc = interfaces.cpp_interfaces()->compi->IsMirrorWindowVisible();
   LOG_EXIT_RC(rc, "FnPassalongIsMirrorWindowVisible");
}

void __stdcall VRCompositorFnPassalong_CompositorDumpImages()
{
   LOG_ENTRY("FnPassalongCompositorDumpImages");
   interfaces.cpp_interfaces()->compi->CompositorDumpImages();
   LOG_EXIT("FnPassalongCompositorDumpImages");
}

bool __stdcall VRCompositorFnPassalong_ShouldAppRenderWithLowResources()
{
   LOG_ENTRY("FnPassalongShouldAppRenderWithLowResources");
   bool rc = interfaces.cpp_interfaces()->compi->ShouldAppRenderWithLowResources();
   LOG_EXIT_RC(rc, "FnPassalongShouldAppRenderWithLowResources");
}

void __stdcall VRCompositorFnPassalong_ForceInterleavedReprojectionOn(bool bOverride)
{
   LOG_ENTRY("FnPassalongForceInterleavedReprojectionOn");
   interfaces.cpp_interfaces()->compi->ForceInterleavedReprojectionOn(bOverride);
   LOG_EXIT("FnPassalongForceInterleavedReprojectionOn");
}

void __stdcall VRCompositorFnPassalong_ForceReconnectProcess()
{
   LOG_ENTRY("FnPassalongForceReconnectProcess");
   interfaces.cpp_interfaces()->compi->ForceReconnectProcess();
   LOG_EXIT("FnPassalongForceReconnectProcess");
}

void __stdcall VRCompositorFnPassalong_SuspendRendering(bool bSuspend)
{
   LOG_ENTRY("FnPassalongSuspendRendering");
   interfaces.cpp_interfaces()->compi->SuspendRendering(bSuspend);
   LOG_EXIT("FnPassalongSuspendRendering");
}

vr::EVRCompositorError __stdcall VRCompositorFnPassalong_GetMirrorTextureD3D11(vr::EVREye eEye, void * pD3D11DeviceOrResource, void ** ppD3D11ShaderResourceView)
{
   LOG_ENTRY("FnPassalongGetMirrorTextureD3D11");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->GetMirrorTextureD3D11(eEye,pD3D11DeviceOrResource,ppD3D11ShaderResourceView);
   LOG_EXIT_RC(rc, "FnPassalongGetMirrorTextureD3D11");
}

vr::EVRCompositorError __stdcall VRCompositorFnPassalong_GetMirrorTextureGL(vr::EVREye eEye, vr::glUInt_t * pglTextureId, vr::glSharedTextureHandle_t * pglSharedTextureHandle)
{
   LOG_ENTRY("FnPassalongGetMirrorTextureGL");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->GetMirrorTextureGL(eEye,pglTextureId,pglSharedTextureHandle);
   LOG_EXIT_RC(rc, "FnPassalongGetMirrorTextureGL");
}

bool __stdcall VRCompositorFnPassalong_ReleaseSharedGLTexture(vr::glUInt_t glTextureId, vr::glSharedTextureHandle_t glSharedTextureHandle)
{
   LOG_ENTRY("FnPassalongReleaseSharedGLTexture");
   bool rc = interfaces.cpp_interfaces()->compi->ReleaseSharedGLTexture(glTextureId,glSharedTextureHandle);
   LOG_EXIT_RC(rc, "FnPassalongReleaseSharedGLTexture");
}

void __stdcall VRCompositorFnPassalong_LockGLSharedTextureForAccess(vr::glSharedTextureHandle_t glSharedTextureHandle)
{
   LOG_ENTRY("FnPassalongLockGLSharedTextureForAccess");
   interfaces.cpp_interfaces()->compi->LockGLSharedTextureForAccess(glSharedTextureHandle);
   LOG_EXIT("FnPassalongLockGLSharedTextureForAccess");
}

void __stdcall VRCompositorFnPassalong_UnlockGLSharedTextureForAccess(vr::glSharedTextureHandle_t glSharedTextureHandle)
{
   LOG_ENTRY("FnPassalongUnlockGLSharedTextureForAccess");
   interfaces.cpp_interfaces()->compi->UnlockGLSharedTextureForAccess(glSharedTextureHandle);
   LOG_EXIT("FnPassalongUnlockGLSharedTextureForAccess");
}

uint32_t __stdcall VRCompositorFnPassalong_GetVulkanInstanceExtensionsRequired(char * pchValue, uint32_t unBufferSize)
{
   LOG_ENTRY("FnPassalongGetVulkanInstanceExtensionsRequired");
   uint32_t rc = interfaces.cpp_interfaces()->compi->GetVulkanInstanceExtensionsRequired(pchValue,unBufferSize);
   LOG_EXIT_RC(rc, "FnPassalongGetVulkanInstanceExtensionsRequired");
}

uint32_t __stdcall VRCompositorFnPassalong_GetVulkanDeviceExtensionsRequired(struct VkPhysicalDevice_T * pPhysicalDevice, char * pchValue, uint32_t unBufferSize)
{
   LOG_ENTRY("FnPassalongGetVulkanDeviceExtensionsRequired");
   uint32_t rc = interfaces.cpp_interfaces()->compi->GetVulkanDeviceExtensionsRequired(pPhysicalDevice,pchValue,unBufferSize);
   LOG_EXIT_RC(rc, "FnPassalongGetVulkanDeviceExtensionsRequired");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_FindOverlay(const char * pchOverlayKey, vr::VROverlayHandle_t * pOverlayHandle)
{
   LOG_ENTRY("FnPassalongFindOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->FindOverlay(pchOverlayKey,pOverlayHandle);
   LOG_EXIT_RC(rc, "FnPassalongFindOverlay");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_CreateOverlay(const char * pchOverlayKey, const char * pchOverlayFriendlyName, vr::VROverlayHandle_t * pOverlayHandle)
{
   LOG_ENTRY("FnPassalongCreateOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->CreateOverlay(pchOverlayKey,pchOverlayFriendlyName,pOverlayHandle);
   LOG_EXIT_RC(rc, "FnPassalongCreateOverlay");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_DestroyOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("FnPassalongDestroyOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->DestroyOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "FnPassalongDestroyOverlay");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetHighQualityOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("FnPassalongSetHighQualityOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetHighQualityOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "FnPassalongSetHighQualityOverlay");
}

vr::VROverlayHandle_t __stdcall VROverlayFnPassalong_GetHighQualityOverlay()
{
   LOG_ENTRY("FnPassalongGetHighQualityOverlay");
   vr::VROverlayHandle_t rc = interfaces.cpp_interfaces()->ovi->GetHighQualityOverlay();
   LOG_EXIT_RC(rc, "FnPassalongGetHighQualityOverlay");
}

uint32_t __stdcall VROverlayFnPassalong_GetOverlayKey(vr::VROverlayHandle_t ulOverlayHandle, char * pchValue, uint32_t unBufferSize, vr::EVROverlayError * pError)
{
   LOG_ENTRY("FnPassalongGetOverlayKey");
   uint32_t rc = interfaces.cpp_interfaces()->ovi->GetOverlayKey(ulOverlayHandle,pchValue,unBufferSize,pError);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayKey");
}

uint32_t __stdcall VROverlayFnPassalong_GetOverlayName(vr::VROverlayHandle_t ulOverlayHandle, char * pchValue, uint32_t unBufferSize, vr::EVROverlayError * pError)
{
   LOG_ENTRY("FnPassalongGetOverlayName");
   uint32_t rc = interfaces.cpp_interfaces()->ovi->GetOverlayName(ulOverlayHandle,pchValue,unBufferSize,pError);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayName");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayImageData(vr::VROverlayHandle_t ulOverlayHandle, void * pvBuffer, uint32_t unBufferSize, uint32_t * punWidth, uint32_t * punHeight)
{
   LOG_ENTRY("FnPassalongGetOverlayImageData");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayImageData(ulOverlayHandle,pvBuffer,unBufferSize,punWidth,punHeight);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayImageData");
}

const char * __stdcall VROverlayFnPassalong_GetOverlayErrorNameFromEnum(vr::EVROverlayError error)
{
   LOG_ENTRY("FnPassalongGetOverlayErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->ovi->GetOverlayErrorNameFromEnum(error);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayErrorNameFromEnum");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayRenderingPid(vr::VROverlayHandle_t ulOverlayHandle, uint32_t unPID)
{
   LOG_ENTRY("FnPassalongSetOverlayRenderingPid");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayRenderingPid(ulOverlayHandle,unPID);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayRenderingPid");
}

uint32_t __stdcall VROverlayFnPassalong_GetOverlayRenderingPid(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("FnPassalongGetOverlayRenderingPid");
   uint32_t rc = interfaces.cpp_interfaces()->ovi->GetOverlayRenderingPid(ulOverlayHandle);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayRenderingPid");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayFlag(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayFlags eOverlayFlag, bool bEnabled)
{
   LOG_ENTRY("FnPassalongSetOverlayFlag");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayFlag(ulOverlayHandle,eOverlayFlag,bEnabled);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayFlag");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayFlag(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayFlags eOverlayFlag, bool * pbEnabled)
{
   LOG_ENTRY("FnPassalongGetOverlayFlag");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayFlag(ulOverlayHandle,eOverlayFlag,pbEnabled);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayFlag");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayColor(vr::VROverlayHandle_t ulOverlayHandle, float fRed, float fGreen, float fBlue)
{
   LOG_ENTRY("FnPassalongSetOverlayColor");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayColor(ulOverlayHandle,fRed,fGreen,fBlue);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayColor");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayColor(vr::VROverlayHandle_t ulOverlayHandle, float * pfRed, float * pfGreen, float * pfBlue)
{
   LOG_ENTRY("FnPassalongGetOverlayColor");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayColor(ulOverlayHandle,pfRed,pfGreen,pfBlue);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayColor");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayAlpha(vr::VROverlayHandle_t ulOverlayHandle, float fAlpha)
{
   LOG_ENTRY("FnPassalongSetOverlayAlpha");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayAlpha(ulOverlayHandle,fAlpha);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayAlpha");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayAlpha(vr::VROverlayHandle_t ulOverlayHandle, float * pfAlpha)
{
   LOG_ENTRY("FnPassalongGetOverlayAlpha");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayAlpha(ulOverlayHandle,pfAlpha);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayAlpha");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayTexelAspect(vr::VROverlayHandle_t ulOverlayHandle, float fTexelAspect)
{
   LOG_ENTRY("FnPassalongSetOverlayTexelAspect");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTexelAspect(ulOverlayHandle,fTexelAspect);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayTexelAspect");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayTexelAspect(vr::VROverlayHandle_t ulOverlayHandle, float * pfTexelAspect)
{
   LOG_ENTRY("FnPassalongGetOverlayTexelAspect");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTexelAspect(ulOverlayHandle,pfTexelAspect);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayTexelAspect");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlaySortOrder(vr::VROverlayHandle_t ulOverlayHandle, uint32_t unSortOrder)
{
   LOG_ENTRY("FnPassalongSetOverlaySortOrder");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlaySortOrder(ulOverlayHandle,unSortOrder);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlaySortOrder");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlaySortOrder(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * punSortOrder)
{
   LOG_ENTRY("FnPassalongGetOverlaySortOrder");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlaySortOrder(ulOverlayHandle,punSortOrder);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlaySortOrder");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayWidthInMeters(vr::VROverlayHandle_t ulOverlayHandle, float fWidthInMeters)
{
   LOG_ENTRY("FnPassalongSetOverlayWidthInMeters");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayWidthInMeters(ulOverlayHandle,fWidthInMeters);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayWidthInMeters");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayWidthInMeters(vr::VROverlayHandle_t ulOverlayHandle, float * pfWidthInMeters)
{
   LOG_ENTRY("FnPassalongGetOverlayWidthInMeters");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayWidthInMeters(ulOverlayHandle,pfWidthInMeters);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayWidthInMeters");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayAutoCurveDistanceRangeInMeters(vr::VROverlayHandle_t ulOverlayHandle, float fMinDistanceInMeters, float fMaxDistanceInMeters)
{
   LOG_ENTRY("FnPassalongSetOverlayAutoCurveDistanceRangeInMeters");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayAutoCurveDistanceRangeInMeters(ulOverlayHandle,fMinDistanceInMeters,fMaxDistanceInMeters);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayAutoCurveDistanceRangeInMeters");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayAutoCurveDistanceRangeInMeters(vr::VROverlayHandle_t ulOverlayHandle, float * pfMinDistanceInMeters, float * pfMaxDistanceInMeters)
{
   LOG_ENTRY("FnPassalongGetOverlayAutoCurveDistanceRangeInMeters");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayAutoCurveDistanceRangeInMeters(ulOverlayHandle,pfMinDistanceInMeters,pfMaxDistanceInMeters);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayAutoCurveDistanceRangeInMeters");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayTextureColorSpace(vr::VROverlayHandle_t ulOverlayHandle, vr::EColorSpace eTextureColorSpace)
{
   LOG_ENTRY("FnPassalongSetOverlayTextureColorSpace");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTextureColorSpace(ulOverlayHandle,eTextureColorSpace);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayTextureColorSpace");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayTextureColorSpace(vr::VROverlayHandle_t ulOverlayHandle, vr::EColorSpace * peTextureColorSpace)
{
   LOG_ENTRY("FnPassalongGetOverlayTextureColorSpace");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTextureColorSpace(ulOverlayHandle,peTextureColorSpace);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayTextureColorSpace");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayTextureBounds(vr::VROverlayHandle_t ulOverlayHandle, const struct vr::VRTextureBounds_t * pOverlayTextureBounds)
{
   LOG_ENTRY("FnPassalongSetOverlayTextureBounds");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTextureBounds(ulOverlayHandle,pOverlayTextureBounds);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayTextureBounds");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayTextureBounds(vr::VROverlayHandle_t ulOverlayHandle, struct vr::VRTextureBounds_t * pOverlayTextureBounds)
{
   LOG_ENTRY("FnPassalongGetOverlayTextureBounds");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTextureBounds(ulOverlayHandle,pOverlayTextureBounds);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayTextureBounds");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayTransformType(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayTransformType * peTransformType)
{
   LOG_ENTRY("FnPassalongGetOverlayTransformType");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTransformType(ulOverlayHandle,peTransformType);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayTransformType");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayTransformAbsolute(vr::VROverlayHandle_t ulOverlayHandle, vr::ETrackingUniverseOrigin eTrackingOrigin, const struct vr::HmdMatrix34_t * pmatTrackingOriginToOverlayTransform)
{
   LOG_ENTRY("FnPassalongSetOverlayTransformAbsolute");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTransformAbsolute(ulOverlayHandle,eTrackingOrigin,pmatTrackingOriginToOverlayTransform);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayTransformAbsolute");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayTransformAbsolute(vr::VROverlayHandle_t ulOverlayHandle, vr::ETrackingUniverseOrigin * peTrackingOrigin, struct vr::HmdMatrix34_t * pmatTrackingOriginToOverlayTransform)
{
   LOG_ENTRY("FnPassalongGetOverlayTransformAbsolute");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTransformAbsolute(ulOverlayHandle,peTrackingOrigin,pmatTrackingOriginToOverlayTransform);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayTransformAbsolute");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayTransformTrackedDeviceRelative(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t unTrackedDevice, const struct vr::HmdMatrix34_t * pmatTrackedDeviceToOverlayTransform)
{
   LOG_ENTRY("FnPassalongSetOverlayTransformTrackedDeviceRelative");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTransformTrackedDeviceRelative(ulOverlayHandle,unTrackedDevice,pmatTrackedDeviceToOverlayTransform);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayTransformTrackedDeviceRelative");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayTransformTrackedDeviceRelative(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t * punTrackedDevice, struct vr::HmdMatrix34_t * pmatTrackedDeviceToOverlayTransform)
{
   LOG_ENTRY("FnPassalongGetOverlayTransformTrackedDeviceRelative");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTransformTrackedDeviceRelative(ulOverlayHandle,punTrackedDevice,pmatTrackedDeviceToOverlayTransform);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayTransformTrackedDeviceRelative");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayTransformTrackedDeviceComponent(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t unDeviceIndex, const char * pchComponentName)
{
   LOG_ENTRY("FnPassalongSetOverlayTransformTrackedDeviceComponent");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTransformTrackedDeviceComponent(ulOverlayHandle,unDeviceIndex,pchComponentName);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayTransformTrackedDeviceComponent");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayTransformTrackedDeviceComponent(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t * punDeviceIndex, char * pchComponentName, uint32_t unComponentNameSize)
{
   LOG_ENTRY("FnPassalongGetOverlayTransformTrackedDeviceComponent");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTransformTrackedDeviceComponent(ulOverlayHandle,punDeviceIndex,pchComponentName,unComponentNameSize);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayTransformTrackedDeviceComponent");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_ShowOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("FnPassalongShowOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->ShowOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "FnPassalongShowOverlay");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_HideOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("FnPassalongHideOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->HideOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "FnPassalongHideOverlay");
}

bool __stdcall VROverlayFnPassalong_IsOverlayVisible(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("FnPassalongIsOverlayVisible");
   bool rc = interfaces.cpp_interfaces()->ovi->IsOverlayVisible(ulOverlayHandle);
   LOG_EXIT_RC(rc, "FnPassalongIsOverlayVisible");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetTransformForOverlayCoordinates(vr::VROverlayHandle_t ulOverlayHandle, vr::ETrackingUniverseOrigin eTrackingOrigin, struct vr::HmdVector2_t coordinatesInOverlay, struct vr::HmdMatrix34_t * pmatTransform)
{
   LOG_ENTRY("FnPassalongGetTransformForOverlayCoordinates");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetTransformForOverlayCoordinates(ulOverlayHandle,eTrackingOrigin,coordinatesInOverlay,pmatTransform);
   LOG_EXIT_RC(rc, "FnPassalongGetTransformForOverlayCoordinates");
}

bool __stdcall VROverlayFnPassalong_PollNextOverlayEvent(vr::VROverlayHandle_t ulOverlayHandle, struct vr::VREvent_t * pEvent, uint32_t uncbVREvent)
{
   LOG_ENTRY("FnPassalongPollNextOverlayEvent");
   bool rc = interfaces.cpp_interfaces()->ovi->PollNextOverlayEvent(ulOverlayHandle,pEvent,uncbVREvent);
   LOG_EXIT_RC(rc, "FnPassalongPollNextOverlayEvent");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayInputMethod(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayInputMethod * peInputMethod)
{
   LOG_ENTRY("FnPassalongGetOverlayInputMethod");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayInputMethod(ulOverlayHandle,peInputMethod);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayInputMethod");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayInputMethod(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayInputMethod eInputMethod)
{
   LOG_ENTRY("FnPassalongSetOverlayInputMethod");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayInputMethod(ulOverlayHandle,eInputMethod);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayInputMethod");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayMouseScale(vr::VROverlayHandle_t ulOverlayHandle, struct vr::HmdVector2_t * pvecMouseScale)
{
   LOG_ENTRY("FnPassalongGetOverlayMouseScale");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayMouseScale(ulOverlayHandle,pvecMouseScale);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayMouseScale");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayMouseScale(vr::VROverlayHandle_t ulOverlayHandle, const struct vr::HmdVector2_t * pvecMouseScale)
{
   LOG_ENTRY("FnPassalongSetOverlayMouseScale");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayMouseScale(ulOverlayHandle,pvecMouseScale);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayMouseScale");
}

bool __stdcall VROverlayFnPassalong_ComputeOverlayIntersection(vr::VROverlayHandle_t ulOverlayHandle, const struct vr::VROverlayIntersectionParams_t * pParams, struct vr::VROverlayIntersectionResults_t * pResults)
{
   LOG_ENTRY("FnPassalongComputeOverlayIntersection");
   bool rc = interfaces.cpp_interfaces()->ovi->ComputeOverlayIntersection(ulOverlayHandle,pParams,pResults);
   LOG_EXIT_RC(rc, "FnPassalongComputeOverlayIntersection");
}

bool __stdcall VROverlayFnPassalong_HandleControllerOverlayInteractionAsMouse(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t unControllerDeviceIndex)
{
   LOG_ENTRY("FnPassalongHandleControllerOverlayInteractionAsMouse");
   bool rc = interfaces.cpp_interfaces()->ovi->HandleControllerOverlayInteractionAsMouse(ulOverlayHandle,unControllerDeviceIndex);
   LOG_EXIT_RC(rc, "FnPassalongHandleControllerOverlayInteractionAsMouse");
}

bool __stdcall VROverlayFnPassalong_IsHoverTargetOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("FnPassalongIsHoverTargetOverlay");
   bool rc = interfaces.cpp_interfaces()->ovi->IsHoverTargetOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "FnPassalongIsHoverTargetOverlay");
}

vr::VROverlayHandle_t __stdcall VROverlayFnPassalong_GetGamepadFocusOverlay()
{
   LOG_ENTRY("FnPassalongGetGamepadFocusOverlay");
   vr::VROverlayHandle_t rc = interfaces.cpp_interfaces()->ovi->GetGamepadFocusOverlay();
   LOG_EXIT_RC(rc, "FnPassalongGetGamepadFocusOverlay");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetGamepadFocusOverlay(vr::VROverlayHandle_t ulNewFocusOverlay)
{
   LOG_ENTRY("FnPassalongSetGamepadFocusOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetGamepadFocusOverlay(ulNewFocusOverlay);
   LOG_EXIT_RC(rc, "FnPassalongSetGamepadFocusOverlay");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayNeighbor(vr::EOverlayDirection eDirection, vr::VROverlayHandle_t ulFrom, vr::VROverlayHandle_t ulTo)
{
   LOG_ENTRY("FnPassalongSetOverlayNeighbor");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayNeighbor(eDirection,ulFrom,ulTo);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayNeighbor");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_MoveGamepadFocusToNeighbor(vr::EOverlayDirection eDirection, vr::VROverlayHandle_t ulFrom)
{
   LOG_ENTRY("FnPassalongMoveGamepadFocusToNeighbor");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->MoveGamepadFocusToNeighbor(eDirection,ulFrom);
   LOG_EXIT_RC(rc, "FnPassalongMoveGamepadFocusToNeighbor");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayTexture(vr::VROverlayHandle_t ulOverlayHandle, const struct vr::Texture_t * pTexture)
{
   LOG_ENTRY("FnPassalongSetOverlayTexture");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTexture(ulOverlayHandle,pTexture);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayTexture");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_ClearOverlayTexture(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("FnPassalongClearOverlayTexture");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->ClearOverlayTexture(ulOverlayHandle);
   LOG_EXIT_RC(rc, "FnPassalongClearOverlayTexture");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayRaw(vr::VROverlayHandle_t ulOverlayHandle, void * pvBuffer, uint32_t unWidth, uint32_t unHeight, uint32_t unDepth)
{
   LOG_ENTRY("FnPassalongSetOverlayRaw");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayRaw(ulOverlayHandle,pvBuffer,unWidth,unHeight,unDepth);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayRaw");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayFromFile(vr::VROverlayHandle_t ulOverlayHandle, const char * pchFilePath)
{
   LOG_ENTRY("FnPassalongSetOverlayFromFile");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayFromFile(ulOverlayHandle,pchFilePath);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayFromFile");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayTexture(vr::VROverlayHandle_t ulOverlayHandle, void ** pNativeTextureHandle, void * pNativeTextureRef, uint32_t * pWidth, uint32_t * pHeight, uint32_t * pNativeFormat, vr::ETextureType * pAPIType, vr::EColorSpace * pColorSpace, struct vr::VRTextureBounds_t * pTextureBounds)
{
   LOG_ENTRY("FnPassalongGetOverlayTexture");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTexture(ulOverlayHandle,pNativeTextureHandle,pNativeTextureRef,pWidth,pHeight,pNativeFormat,pAPIType,pColorSpace,pTextureBounds);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayTexture");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_ReleaseNativeOverlayHandle(vr::VROverlayHandle_t ulOverlayHandle, void * pNativeTextureHandle)
{
   LOG_ENTRY("FnPassalongReleaseNativeOverlayHandle");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->ReleaseNativeOverlayHandle(ulOverlayHandle,pNativeTextureHandle);
   LOG_EXIT_RC(rc, "FnPassalongReleaseNativeOverlayHandle");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayTextureSize(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * pWidth, uint32_t * pHeight)
{
   LOG_ENTRY("FnPassalongGetOverlayTextureSize");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTextureSize(ulOverlayHandle,pWidth,pHeight);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayTextureSize");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_CreateDashboardOverlay(const char * pchOverlayKey, const char * pchOverlayFriendlyName, vr::VROverlayHandle_t * pMainHandle, vr::VROverlayHandle_t * pThumbnailHandle)
{
   LOG_ENTRY("FnPassalongCreateDashboardOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->CreateDashboardOverlay(pchOverlayKey,pchOverlayFriendlyName,pMainHandle,pThumbnailHandle);
   LOG_EXIT_RC(rc, "FnPassalongCreateDashboardOverlay");
}

bool __stdcall VROverlayFnPassalong_IsDashboardVisible()
{
   LOG_ENTRY("FnPassalongIsDashboardVisible");
   bool rc = interfaces.cpp_interfaces()->ovi->IsDashboardVisible();
   LOG_EXIT_RC(rc, "FnPassalongIsDashboardVisible");
}

bool __stdcall VROverlayFnPassalong_IsActiveDashboardOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("FnPassalongIsActiveDashboardOverlay");
   bool rc = interfaces.cpp_interfaces()->ovi->IsActiveDashboardOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "FnPassalongIsActiveDashboardOverlay");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetDashboardOverlaySceneProcess(vr::VROverlayHandle_t ulOverlayHandle, uint32_t unProcessId)
{
   LOG_ENTRY("FnPassalongSetDashboardOverlaySceneProcess");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetDashboardOverlaySceneProcess(ulOverlayHandle,unProcessId);
   LOG_EXIT_RC(rc, "FnPassalongSetDashboardOverlaySceneProcess");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetDashboardOverlaySceneProcess(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * punProcessId)
{
   LOG_ENTRY("FnPassalongGetDashboardOverlaySceneProcess");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetDashboardOverlaySceneProcess(ulOverlayHandle,punProcessId);
   LOG_EXIT_RC(rc, "FnPassalongGetDashboardOverlaySceneProcess");
}

void __stdcall VROverlayFnPassalong_ShowDashboard(const char * pchOverlayToShow)
{
   LOG_ENTRY("FnPassalongShowDashboard");
   interfaces.cpp_interfaces()->ovi->ShowDashboard(pchOverlayToShow);
   LOG_EXIT("FnPassalongShowDashboard");
}

vr::TrackedDeviceIndex_t __stdcall VROverlayFnPassalong_GetPrimaryDashboardDevice()
{
   LOG_ENTRY("FnPassalongGetPrimaryDashboardDevice");
   vr::TrackedDeviceIndex_t rc = interfaces.cpp_interfaces()->ovi->GetPrimaryDashboardDevice();
   LOG_EXIT_RC(rc, "FnPassalongGetPrimaryDashboardDevice");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_ShowKeyboard(vr::EGamepadTextInputMode eInputMode, vr::EGamepadTextInputLineMode eLineInputMode, const char * pchDescription, uint32_t unCharMax, const char * pchExistingText, bool bUseMinimalMode, uint64_t uUserValue)
{
   LOG_ENTRY("FnPassalongShowKeyboard");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->ShowKeyboard(eInputMode,eLineInputMode,pchDescription,unCharMax,pchExistingText,bUseMinimalMode,uUserValue);
   LOG_EXIT_RC(rc, "FnPassalongShowKeyboard");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_ShowKeyboardForOverlay(vr::VROverlayHandle_t ulOverlayHandle, vr::EGamepadTextInputMode eInputMode, vr::EGamepadTextInputLineMode eLineInputMode, const char * pchDescription, uint32_t unCharMax, const char * pchExistingText, bool bUseMinimalMode, uint64_t uUserValue)
{
   LOG_ENTRY("FnPassalongShowKeyboardForOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->ShowKeyboardForOverlay(ulOverlayHandle,eInputMode,eLineInputMode,pchDescription,unCharMax,pchExistingText,bUseMinimalMode,uUserValue);
   LOG_EXIT_RC(rc, "FnPassalongShowKeyboardForOverlay");
}

uint32_t __stdcall VROverlayFnPassalong_GetKeyboardText(char * pchText, uint32_t cchText)
{
   LOG_ENTRY("FnPassalongGetKeyboardText");
   uint32_t rc = interfaces.cpp_interfaces()->ovi->GetKeyboardText(pchText,cchText);
   LOG_EXIT_RC(rc, "FnPassalongGetKeyboardText");
}

void __stdcall VROverlayFnPassalong_HideKeyboard()
{
   LOG_ENTRY("FnPassalongHideKeyboard");
   interfaces.cpp_interfaces()->ovi->HideKeyboard();
   LOG_EXIT("FnPassalongHideKeyboard");
}

void __stdcall VROverlayFnPassalong_SetKeyboardTransformAbsolute(vr::ETrackingUniverseOrigin eTrackingOrigin, const struct vr::HmdMatrix34_t * pmatTrackingOriginToKeyboardTransform)
{
   LOG_ENTRY("FnPassalongSetKeyboardTransformAbsolute");
   interfaces.cpp_interfaces()->ovi->SetKeyboardTransformAbsolute(eTrackingOrigin,pmatTrackingOriginToKeyboardTransform);
   LOG_EXIT("FnPassalongSetKeyboardTransformAbsolute");
}

void __stdcall VROverlayFnPassalong_SetKeyboardPositionForOverlay(vr::VROverlayHandle_t ulOverlayHandle, struct vr::HmdRect2_t avoidRect)
{
   LOG_ENTRY("FnPassalongSetKeyboardPositionForOverlay");
   interfaces.cpp_interfaces()->ovi->SetKeyboardPositionForOverlay(ulOverlayHandle,avoidRect);
   LOG_EXIT("FnPassalongSetKeyboardPositionForOverlay");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_SetOverlayIntersectionMask(vr::VROverlayHandle_t ulOverlayHandle, struct vr::VROverlayIntersectionMaskPrimitive_t * pMaskPrimitives, uint32_t unNumMaskPrimitives, uint32_t unPrimitiveSize)
{
   LOG_ENTRY("FnPassalongSetOverlayIntersectionMask");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayIntersectionMask(ulOverlayHandle,pMaskPrimitives,unNumMaskPrimitives,unPrimitiveSize);
   LOG_EXIT_RC(rc, "FnPassalongSetOverlayIntersectionMask");
}

vr::EVROverlayError __stdcall VROverlayFnPassalong_GetOverlayFlags(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * pFlags)
{
   LOG_ENTRY("FnPassalongGetOverlayFlags");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayFlags(ulOverlayHandle,pFlags);
   LOG_EXIT_RC(rc, "FnPassalongGetOverlayFlags");
}

vr::VRMessageOverlayResponse __stdcall VROverlayFnPassalong_ShowMessageOverlay(const char * pchText, const char * pchCaption, const char * pchButton0Text, const char * pchButton1Text, const char * pchButton2Text, const char * pchButton3Text)
{
   LOG_ENTRY("FnPassalongShowMessageOverlay");
   vr::VRMessageOverlayResponse rc = interfaces.cpp_interfaces()->ovi->ShowMessageOverlay(pchText,pchCaption,pchButton0Text,pchButton1Text,pchButton2Text,pchButton3Text);
   LOG_EXIT_RC(rc, "FnPassalongShowMessageOverlay");
}

vr::EVRRenderModelError __stdcall VRRenderModelsFnPassalong_LoadRenderModel_Async(const char * pchRenderModelName, struct vr::RenderModel_t ** ppRenderModel)
{
   LOG_ENTRY("FnPassalongLoadRenderModel_Async");
   vr::EVRRenderModelError rc = interfaces.cpp_interfaces()->remi->LoadRenderModel_Async(pchRenderModelName,ppRenderModel);
   LOG_EXIT_RC(rc, "FnPassalongLoadRenderModel_Async");
}

void __stdcall VRRenderModelsFnPassalong_FreeRenderModel(struct vr::RenderModel_t * pRenderModel)
{
   LOG_ENTRY("FnPassalongFreeRenderModel");
   interfaces.cpp_interfaces()->remi->FreeRenderModel(pRenderModel);
   LOG_EXIT("FnPassalongFreeRenderModel");
}

vr::EVRRenderModelError __stdcall VRRenderModelsFnPassalong_LoadTexture_Async(vr::TextureID_t textureId, struct vr::RenderModel_TextureMap_t ** ppTexture)
{
   LOG_ENTRY("FnPassalongLoadTexture_Async");
   vr::EVRRenderModelError rc = interfaces.cpp_interfaces()->remi->LoadTexture_Async(textureId,ppTexture);
   LOG_EXIT_RC(rc, "FnPassalongLoadTexture_Async");
}

void __stdcall VRRenderModelsFnPassalong_FreeTexture(struct vr::RenderModel_TextureMap_t * pTexture)
{
   LOG_ENTRY("FnPassalongFreeTexture");
   interfaces.cpp_interfaces()->remi->FreeTexture(pTexture);
   LOG_EXIT("FnPassalongFreeTexture");
}

vr::EVRRenderModelError __stdcall VRRenderModelsFnPassalong_LoadTextureD3D11_Async(vr::TextureID_t textureId, void * pD3D11Device, void ** ppD3D11Texture2D)
{
   LOG_ENTRY("FnPassalongLoadTextureD3D11_Async");
   vr::EVRRenderModelError rc = interfaces.cpp_interfaces()->remi->LoadTextureD3D11_Async(textureId,pD3D11Device,ppD3D11Texture2D);
   LOG_EXIT_RC(rc, "FnPassalongLoadTextureD3D11_Async");
}

vr::EVRRenderModelError __stdcall VRRenderModelsFnPassalong_LoadIntoTextureD3D11_Async(vr::TextureID_t textureId, void * pDstTexture)
{
   LOG_ENTRY("FnPassalongLoadIntoTextureD3D11_Async");
   vr::EVRRenderModelError rc = interfaces.cpp_interfaces()->remi->LoadIntoTextureD3D11_Async(textureId,pDstTexture);
   LOG_EXIT_RC(rc, "FnPassalongLoadIntoTextureD3D11_Async");
}

void __stdcall VRRenderModelsFnPassalong_FreeTextureD3D11(void * pD3D11Texture2D)
{
   LOG_ENTRY("FnPassalongFreeTextureD3D11");
   interfaces.cpp_interfaces()->remi->FreeTextureD3D11(pD3D11Texture2D);
   LOG_EXIT("FnPassalongFreeTextureD3D11");
}

uint32_t __stdcall VRRenderModelsFnPassalong_GetRenderModelName(uint32_t unRenderModelIndex, char * pchRenderModelName, uint32_t unRenderModelNameLen)
{
   LOG_ENTRY("FnPassalongGetRenderModelName");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetRenderModelName(unRenderModelIndex,pchRenderModelName,unRenderModelNameLen);
   LOG_EXIT_RC(rc, "FnPassalongGetRenderModelName");
}

uint32_t __stdcall VRRenderModelsFnPassalong_GetRenderModelCount()
{
   LOG_ENTRY("FnPassalongGetRenderModelCount");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetRenderModelCount();
   LOG_EXIT_RC(rc, "FnPassalongGetRenderModelCount");
}

uint32_t __stdcall VRRenderModelsFnPassalong_GetComponentCount(const char * pchRenderModelName)
{
   LOG_ENTRY("FnPassalongGetComponentCount");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetComponentCount(pchRenderModelName);
   LOG_EXIT_RC(rc, "FnPassalongGetComponentCount");
}

uint32_t __stdcall VRRenderModelsFnPassalong_GetComponentName(const char * pchRenderModelName, uint32_t unComponentIndex, char * pchComponentName, uint32_t unComponentNameLen)
{
   LOG_ENTRY("FnPassalongGetComponentName");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetComponentName(pchRenderModelName,unComponentIndex,pchComponentName,unComponentNameLen);
   LOG_EXIT_RC(rc, "FnPassalongGetComponentName");
}

uint64_t __stdcall VRRenderModelsFnPassalong_GetComponentButtonMask(const char * pchRenderModelName, const char * pchComponentName)
{
   LOG_ENTRY("FnPassalongGetComponentButtonMask");
   uint64_t rc = interfaces.cpp_interfaces()->remi->GetComponentButtonMask(pchRenderModelName,pchComponentName);
   LOG_EXIT_RC(rc, "FnPassalongGetComponentButtonMask");
}

uint32_t __stdcall VRRenderModelsFnPassalong_GetComponentRenderModelName(const char * pchRenderModelName, const char * pchComponentName, char * pchComponentRenderModelName, uint32_t unComponentRenderModelNameLen)
{
   LOG_ENTRY("FnPassalongGetComponentRenderModelName");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetComponentRenderModelName(pchRenderModelName,pchComponentName,pchComponentRenderModelName,unComponentRenderModelNameLen);
   LOG_EXIT_RC(rc, "FnPassalongGetComponentRenderModelName");
}

bool __stdcall VRRenderModelsFnPassalong_GetComponentState(const char * pchRenderModelName, const char * pchComponentName, const vr::VRControllerState_t * pControllerState, const struct vr::RenderModel_ControllerMode_State_t * pState, struct vr::RenderModel_ComponentState_t * pComponentState)
{
   LOG_ENTRY("FnPassalongGetComponentState");
   bool rc = interfaces.cpp_interfaces()->remi->GetComponentState(pchRenderModelName,pchComponentName,pControllerState,pState,pComponentState);
   LOG_EXIT_RC(rc, "FnPassalongGetComponentState");
}

bool __stdcall VRRenderModelsFnPassalong_RenderModelHasComponent(const char * pchRenderModelName, const char * pchComponentName)
{
   LOG_ENTRY("FnPassalongRenderModelHasComponent");
   bool rc = interfaces.cpp_interfaces()->remi->RenderModelHasComponent(pchRenderModelName,pchComponentName);
   LOG_EXIT_RC(rc, "FnPassalongRenderModelHasComponent");
}

uint32_t __stdcall VRRenderModelsFnPassalong_GetRenderModelThumbnailURL(const char * pchRenderModelName, char * pchThumbnailURL, uint32_t unThumbnailURLLen, vr::EVRRenderModelError * peError)
{
   LOG_ENTRY("FnPassalongGetRenderModelThumbnailURL");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetRenderModelThumbnailURL(pchRenderModelName,pchThumbnailURL,unThumbnailURLLen,peError);
   LOG_EXIT_RC(rc, "FnPassalongGetRenderModelThumbnailURL");
}

uint32_t __stdcall VRRenderModelsFnPassalong_GetRenderModelOriginalPath(const char * pchRenderModelName, char * pchOriginalPath, uint32_t unOriginalPathLen, vr::EVRRenderModelError * peError)
{
   LOG_ENTRY("FnPassalongGetRenderModelOriginalPath");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetRenderModelOriginalPath(pchRenderModelName,pchOriginalPath,unOriginalPathLen,peError);
   LOG_EXIT_RC(rc, "FnPassalongGetRenderModelOriginalPath");
}

const char * __stdcall VRRenderModelsFnPassalong_GetRenderModelErrorNameFromEnum(vr::EVRRenderModelError error)
{
   LOG_ENTRY("FnPassalongGetRenderModelErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->remi->GetRenderModelErrorNameFromEnum(error);
   LOG_EXIT_RC(rc, "FnPassalongGetRenderModelErrorNameFromEnum");
}

vr::EVRNotificationError __stdcall VRNotificationsFnPassalong_CreateNotification(vr::VROverlayHandle_t ulOverlayHandle, uint64_t ulUserValue, vr::EVRNotificationType type, const char * pchText, vr::EVRNotificationStyle style, const struct vr::NotificationBitmap_t * pImage, vr::VRNotificationId * pNotificationId)
{
   LOG_ENTRY("FnPassalongCreateNotification");
   vr::EVRNotificationError rc = interfaces.cpp_interfaces()->noti->CreateNotification(ulOverlayHandle,ulUserValue,type,pchText,style,pImage,pNotificationId);
   LOG_EXIT_RC(rc, "FnPassalongCreateNotification");
}

vr::EVRNotificationError __stdcall VRNotificationsFnPassalong_RemoveNotification(vr::VRNotificationId notificationId)
{
   LOG_ENTRY("FnPassalongRemoveNotification");
   vr::EVRNotificationError rc = interfaces.cpp_interfaces()->noti->RemoveNotification(notificationId);
   LOG_EXIT_RC(rc, "FnPassalongRemoveNotification");
}

const char * __stdcall VRSettingsFnPassalong_GetSettingsErrorNameFromEnum(vr::EVRSettingsError eError)
{
   LOG_ENTRY("FnPassalongGetSettingsErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->seti->GetSettingsErrorNameFromEnum(eError);
   LOG_EXIT_RC(rc, "FnPassalongGetSettingsErrorNameFromEnum");
}

bool __stdcall VRSettingsFnPassalong_Sync(bool bForce, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("FnPassalongSync");
   bool rc = interfaces.cpp_interfaces()->seti->Sync(bForce,peError);
   LOG_EXIT_RC(rc, "FnPassalongSync");
}

void __stdcall VRSettingsFnPassalong_SetBool(const char * pchSection, const char * pchSettingsKey, bool bValue, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("FnPassalongSetBool");
   interfaces.cpp_interfaces()->seti->SetBool(pchSection,pchSettingsKey,bValue,peError);
   LOG_EXIT("FnPassalongSetBool");
}

void __stdcall VRSettingsFnPassalong_SetInt32(const char * pchSection, const char * pchSettingsKey, int32_t nValue, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("FnPassalongSetInt32");
   interfaces.cpp_interfaces()->seti->SetInt32(pchSection,pchSettingsKey,nValue,peError);
   LOG_EXIT("FnPassalongSetInt32");
}

void __stdcall VRSettingsFnPassalong_SetFloat(const char * pchSection, const char * pchSettingsKey, float flValue, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("FnPassalongSetFloat");
   interfaces.cpp_interfaces()->seti->SetFloat(pchSection,pchSettingsKey,flValue,peError);
   LOG_EXIT("FnPassalongSetFloat");
}

void __stdcall VRSettingsFnPassalong_SetString(const char * pchSection, const char * pchSettingsKey, const char * pchValue, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("FnPassalongSetString");
   interfaces.cpp_interfaces()->seti->SetString(pchSection,pchSettingsKey,pchValue,peError);
   LOG_EXIT("FnPassalongSetString");
}

bool __stdcall VRSettingsFnPassalong_GetBool(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("FnPassalongGetBool");
   bool rc = interfaces.cpp_interfaces()->seti->GetBool(pchSection,pchSettingsKey,peError);
   LOG_EXIT_RC(rc, "FnPassalongGetBool");
}

int32_t __stdcall VRSettingsFnPassalong_GetInt32(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("FnPassalongGetInt32");
   int32_t rc = interfaces.cpp_interfaces()->seti->GetInt32(pchSection,pchSettingsKey,peError);
   LOG_EXIT_RC(rc, "FnPassalongGetInt32");
}

float __stdcall VRSettingsFnPassalong_GetFloat(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("FnPassalongGetFloat");
   float rc = interfaces.cpp_interfaces()->seti->GetFloat(pchSection,pchSettingsKey,peError);
   LOG_EXIT_RC(rc, "FnPassalongGetFloat");
}

void __stdcall VRSettingsFnPassalong_GetString(const char * pchSection, const char * pchSettingsKey, char * pchValue, uint32_t unValueLen, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("FnPassalongGetString");
   interfaces.cpp_interfaces()->seti->GetString(pchSection,pchSettingsKey,pchValue,unValueLen,peError);
   LOG_EXIT("FnPassalongGetString");
}

void __stdcall VRSettingsFnPassalong_RemoveSection(const char * pchSection, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("FnPassalongRemoveSection");
   interfaces.cpp_interfaces()->seti->RemoveSection(pchSection,peError);
   LOG_EXIT("FnPassalongRemoveSection");
}

void __stdcall VRSettingsFnPassalong_RemoveKeyInSection(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("FnPassalongRemoveKeyInSection");
   interfaces.cpp_interfaces()->seti->RemoveKeyInSection(pchSection,pchSettingsKey,peError);
   LOG_EXIT("FnPassalongRemoveKeyInSection");
}

vr::EVRScreenshotError __stdcall VRScreenshotsFnPassalong_RequestScreenshot(vr::ScreenshotHandle_t * pOutScreenshotHandle, vr::EVRScreenshotType type, const char * pchPreviewFilename, const char * pchVRFilename)
{
   LOG_ENTRY("FnPassalongRequestScreenshot");
   vr::EVRScreenshotError rc = interfaces.cpp_interfaces()->screeni->RequestScreenshot(pOutScreenshotHandle,type,pchPreviewFilename,pchVRFilename);
   LOG_EXIT_RC(rc, "FnPassalongRequestScreenshot");
}

vr::EVRScreenshotError __stdcall VRScreenshotsFnPassalong_HookScreenshot(const vr::EVRScreenshotType * pSupportedTypes, int numTypes)
{
   LOG_ENTRY("FnPassalongHookScreenshot");
   vr::EVRScreenshotError rc = interfaces.cpp_interfaces()->screeni->HookScreenshot(pSupportedTypes,numTypes);
   LOG_EXIT_RC(rc, "FnPassalongHookScreenshot");
}

vr::EVRScreenshotType __stdcall VRScreenshotsFnPassalong_GetScreenshotPropertyType(vr::ScreenshotHandle_t screenshotHandle, vr::EVRScreenshotError * pError)
{
   LOG_ENTRY("FnPassalongGetScreenshotPropertyType");
   vr::EVRScreenshotType rc = interfaces.cpp_interfaces()->screeni->GetScreenshotPropertyType(screenshotHandle,pError);
   LOG_EXIT_RC(rc, "FnPassalongGetScreenshotPropertyType");
}

uint32_t __stdcall VRScreenshotsFnPassalong_GetScreenshotPropertyFilename(vr::ScreenshotHandle_t screenshotHandle, vr::EVRScreenshotPropertyFilenames filenameType, char * pchFilename, uint32_t cchFilename, vr::EVRScreenshotError * pError)
{
   LOG_ENTRY("FnPassalongGetScreenshotPropertyFilename");
   uint32_t rc = interfaces.cpp_interfaces()->screeni->GetScreenshotPropertyFilename(screenshotHandle,filenameType,pchFilename,cchFilename,pError);
   LOG_EXIT_RC(rc, "FnPassalongGetScreenshotPropertyFilename");
}

vr::EVRScreenshotError __stdcall VRScreenshotsFnPassalong_UpdateScreenshotProgress(vr::ScreenshotHandle_t screenshotHandle, float flProgress)
{
   LOG_ENTRY("FnPassalongUpdateScreenshotProgress");
   vr::EVRScreenshotError rc = interfaces.cpp_interfaces()->screeni->UpdateScreenshotProgress(screenshotHandle,flProgress);
   LOG_EXIT_RC(rc, "FnPassalongUpdateScreenshotProgress");
}

vr::EVRScreenshotError __stdcall VRScreenshotsFnPassalong_TakeStereoScreenshot(vr::ScreenshotHandle_t * pOutScreenshotHandle, const char * pchPreviewFilename, const char * pchVRFilename)
{
   LOG_ENTRY("FnPassalongTakeStereoScreenshot");
   vr::EVRScreenshotError rc = interfaces.cpp_interfaces()->screeni->TakeStereoScreenshot(pOutScreenshotHandle,pchPreviewFilename,pchVRFilename);
   LOG_EXIT_RC(rc, "FnPassalongTakeStereoScreenshot");
}

vr::EVRScreenshotError __stdcall VRScreenshotsFnPassalong_SubmitScreenshot(vr::ScreenshotHandle_t screenshotHandle, vr::EVRScreenshotType type, const char * pchSourcePreviewFilename, const char * pchSourceVRFilename)
{
   LOG_ENTRY("FnPassalongSubmitScreenshot");
   vr::EVRScreenshotError rc = interfaces.cpp_interfaces()->screeni->SubmitScreenshot(screenshotHandle,type,pchSourcePreviewFilename,pchSourceVRFilename);
   LOG_EXIT_RC(rc, "FnPassalongSubmitScreenshot");
}

uint32_t __stdcall VRResourcesFnPassalong_LoadSharedResource(const char * pchResourceName, char * pchBuffer, uint32_t unBufferLen)
{
   LOG_ENTRY("FnPassalongLoadSharedResource");
   uint32_t rc = interfaces.cpp_interfaces()->resi->LoadSharedResource(pchResourceName,pchBuffer,unBufferLen);
   LOG_EXIT_RC(rc, "FnPassalongLoadSharedResource");
}

uint32_t __stdcall VRResourcesFnPassalong_GetResourceFullPath(const char * pchResourceName, const char * pchResourceTypeDirectory, char * pchPathBuffer, uint32_t unBufferLen)
{
   LOG_ENTRY("FnPassalongGetResourceFullPath");
   uint32_t rc = interfaces.cpp_interfaces()->resi->GetResourceFullPath(pchResourceName,pchResourceTypeDirectory,pchPathBuffer,unBufferLen);
   LOG_EXIT_RC(rc, "FnPassalongGetResourceFullPath");
}

