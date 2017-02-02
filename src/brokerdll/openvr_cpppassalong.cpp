#include "openvr_cpppassalong.h"
#include <openvr_broker.h>
#include <stdint.h>
#include "openvr_method_ids.h"
#include "dprintf.h"

#include <InterfacesIntoOpenVR.h>
static InterfacesIntoOpenVR interfaces;

static VRSystemCppPassalong VRSystemCppPassalongInstance;
static VRExtendedDisplayCppPassalong VRExtendedDisplayCppPassalongInstance;
static VRTrackedCameraCppPassalong VRTrackedCameraCppPassalongInstance;
static VRApplicationsCppPassalong VRApplicationsCppPassalongInstance;
static VRChaperoneCppPassalong VRChaperoneCppPassalongInstance;
static VRChaperoneSetupCppPassalong VRChaperoneSetupCppPassalongInstance;
static VRCompositorCppPassalong VRCompositorCppPassalongInstance;
static VROverlayCppPassalong VROverlayCppPassalongInstance;
static VRRenderModelsCppPassalong VRRenderModelsCppPassalongInstance;
static VRNotificationsCppPassalong VRNotificationsCppPassalongInstance;
static VRSettingsCppPassalong VRSettingsCppPassalongInstance;
static VRScreenshotsCppPassalong VRScreenshotsCppPassalongInstance;
static VRResourcesCppPassalong VRResourcesCppPassalongInstance;

static void *vr_interfaces_cpppassalong[] =
{
&VRSystemCppPassalongInstance,
&VRApplicationsCppPassalongInstance,
&VRSettingsCppPassalongInstance,
&VRChaperoneCppPassalongInstance,
&VRChaperoneSetupCppPassalongInstance,
&VRCompositorCppPassalongInstance,
&VRNotificationsCppPassalongInstance,
&VROverlayCppPassalongInstance,
&VRRenderModelsCppPassalongInstance,
&VRExtendedDisplayCppPassalongInstance,
&VRTrackedCameraCppPassalongInstance,
&VRScreenshotsCppPassalongInstance,
&VRResourcesCppPassalongInstance,
};

void **GetCppPassalongInterfaces()
{
  static LPCTSTR extra_path = L"C:\\projects\\openvr_clean\\openvr\\bin\\win32";
  static LPCTSTR server_dll = L"openvr_api.dll";
  if (!interfaces.is_open())
  {
    interfaces.open_lib(server_dll, extra_path);
  }
  return vr_interfaces_cpppassalong;
}

void VRSystemCppPassalong::GetRecommendedRenderTargetSize(uint32_t * pnWidth, uint32_t * pnHeight)
{
   LOG_ENTRY("CppPassalongGetRecommendedRenderTargetSize");
   interfaces.cpp_interfaces()->sysi->GetRecommendedRenderTargetSize(pnWidth,pnHeight);
   LOG_EXIT("CppPassalongGetRecommendedRenderTargetSize");
}

struct vr::HmdMatrix44_t VRSystemCppPassalong::GetProjectionMatrix(vr::EVREye eEye, float fNearZ, float fFarZ)
{
   LOG_ENTRY("CppPassalongGetProjectionMatrix");
   struct vr::HmdMatrix44_t rc = interfaces.cpp_interfaces()->sysi->GetProjectionMatrix(eEye,fNearZ,fFarZ);
   LOG_EXIT_RC(rc, "CppPassalongGetProjectionMatrix");
}

void VRSystemCppPassalong::GetProjectionRaw(vr::EVREye eEye, float * pfLeft, float * pfRight, float * pfTop, float * pfBottom)
{
   LOG_ENTRY("CppPassalongGetProjectionRaw");
   interfaces.cpp_interfaces()->sysi->GetProjectionRaw(eEye,pfLeft,pfRight,pfTop,pfBottom);
   LOG_EXIT("CppPassalongGetProjectionRaw");
}

bool VRSystemCppPassalong::ComputeDistortion(vr::EVREye eEye, float fU, float fV, struct vr::DistortionCoordinates_t * pDistortionCoordinates)
{
   LOG_ENTRY("CppPassalongComputeDistortion");
   bool rc = interfaces.cpp_interfaces()->sysi->ComputeDistortion(eEye,fU,fV,pDistortionCoordinates);
   LOG_EXIT_RC(rc, "CppPassalongComputeDistortion");
}

struct vr::HmdMatrix34_t VRSystemCppPassalong::GetEyeToHeadTransform(vr::EVREye eEye)
{
   LOG_ENTRY("CppPassalongGetEyeToHeadTransform");
   struct vr::HmdMatrix34_t rc = interfaces.cpp_interfaces()->sysi->GetEyeToHeadTransform(eEye);
   LOG_EXIT_RC(rc, "CppPassalongGetEyeToHeadTransform");
}

bool VRSystemCppPassalong::GetTimeSinceLastVsync(float * pfSecondsSinceLastVsync, uint64_t * pulFrameCounter)
{
   LOG_ENTRY("CppPassalongGetTimeSinceLastVsync");
   bool rc = interfaces.cpp_interfaces()->sysi->GetTimeSinceLastVsync(pfSecondsSinceLastVsync,pulFrameCounter);
   LOG_EXIT_RC(rc, "CppPassalongGetTimeSinceLastVsync");
}

int32_t VRSystemCppPassalong::GetD3D9AdapterIndex()
{
   LOG_ENTRY("CppPassalongGetD3D9AdapterIndex");
   int32_t rc = interfaces.cpp_interfaces()->sysi->GetD3D9AdapterIndex();
   LOG_EXIT_RC(rc, "CppPassalongGetD3D9AdapterIndex");
}

void VRSystemCppPassalong::GetDXGIOutputInfo(int32_t * pnAdapterIndex)
{
   LOG_ENTRY("CppPassalongGetDXGIOutputInfo");
   interfaces.cpp_interfaces()->sysi->GetDXGIOutputInfo(pnAdapterIndex);
   LOG_EXIT("CppPassalongGetDXGIOutputInfo");
}

bool VRSystemCppPassalong::IsDisplayOnDesktop()
{
   LOG_ENTRY("CppPassalongIsDisplayOnDesktop");
   bool rc = interfaces.cpp_interfaces()->sysi->IsDisplayOnDesktop();
   LOG_EXIT_RC(rc, "CppPassalongIsDisplayOnDesktop");
}

bool VRSystemCppPassalong::SetDisplayVisibility(bool bIsVisibleOnDesktop)
{
   LOG_ENTRY("CppPassalongSetDisplayVisibility");
   bool rc = interfaces.cpp_interfaces()->sysi->SetDisplayVisibility(bIsVisibleOnDesktop);
   LOG_EXIT_RC(rc, "CppPassalongSetDisplayVisibility");
}

void VRSystemCppPassalong::GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin eOrigin, float fPredictedSecondsToPhotonsFromNow, struct vr::TrackedDevicePose_t * pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount)
{
   LOG_ENTRY("CppPassalongGetDeviceToAbsoluteTrackingPose");
   interfaces.cpp_interfaces()->sysi->GetDeviceToAbsoluteTrackingPose(eOrigin,fPredictedSecondsToPhotonsFromNow,pTrackedDevicePoseArray,unTrackedDevicePoseArrayCount);
   LOG_EXIT("CppPassalongGetDeviceToAbsoluteTrackingPose");
}

void VRSystemCppPassalong::ResetSeatedZeroPose()
{
   LOG_ENTRY("CppPassalongResetSeatedZeroPose");
   interfaces.cpp_interfaces()->sysi->ResetSeatedZeroPose();
   LOG_EXIT("CppPassalongResetSeatedZeroPose");
}

struct vr::HmdMatrix34_t VRSystemCppPassalong::GetSeatedZeroPoseToStandingAbsoluteTrackingPose()
{
   LOG_ENTRY("CppPassalongGetSeatedZeroPoseToStandingAbsoluteTrackingPose");
   struct vr::HmdMatrix34_t rc = interfaces.cpp_interfaces()->sysi->GetSeatedZeroPoseToStandingAbsoluteTrackingPose();
   LOG_EXIT_RC(rc, "CppPassalongGetSeatedZeroPoseToStandingAbsoluteTrackingPose");
}

struct vr::HmdMatrix34_t VRSystemCppPassalong::GetRawZeroPoseToStandingAbsoluteTrackingPose()
{
   LOG_ENTRY("CppPassalongGetRawZeroPoseToStandingAbsoluteTrackingPose");
   struct vr::HmdMatrix34_t rc = interfaces.cpp_interfaces()->sysi->GetRawZeroPoseToStandingAbsoluteTrackingPose();
   LOG_EXIT_RC(rc, "CppPassalongGetRawZeroPoseToStandingAbsoluteTrackingPose");
}

uint32_t VRSystemCppPassalong::GetSortedTrackedDeviceIndicesOfClass(vr::ETrackedDeviceClass eTrackedDeviceClass, vr::TrackedDeviceIndex_t * punTrackedDeviceIndexArray, uint32_t unTrackedDeviceIndexArrayCount, vr::TrackedDeviceIndex_t unRelativeToTrackedDeviceIndex)
{
   LOG_ENTRY("CppPassalongGetSortedTrackedDeviceIndicesOfClass");
   uint32_t rc = interfaces.cpp_interfaces()->sysi->GetSortedTrackedDeviceIndicesOfClass(eTrackedDeviceClass,punTrackedDeviceIndexArray,unTrackedDeviceIndexArrayCount,unRelativeToTrackedDeviceIndex);
   LOG_EXIT_RC(rc, "CppPassalongGetSortedTrackedDeviceIndicesOfClass");
}

vr::EDeviceActivityLevel VRSystemCppPassalong::GetTrackedDeviceActivityLevel(vr::TrackedDeviceIndex_t unDeviceId)
{
   LOG_ENTRY("CppPassalongGetTrackedDeviceActivityLevel");
   vr::EDeviceActivityLevel rc = interfaces.cpp_interfaces()->sysi->GetTrackedDeviceActivityLevel(unDeviceId);
   LOG_EXIT_RC(rc, "CppPassalongGetTrackedDeviceActivityLevel");
}

void VRSystemCppPassalong::ApplyTransform(struct vr::TrackedDevicePose_t * pOutputPose, const struct vr::TrackedDevicePose_t * pTrackedDevicePose, const struct vr::HmdMatrix34_t * pTransform)
{
   LOG_ENTRY("CppPassalongApplyTransform");
   interfaces.cpp_interfaces()->sysi->ApplyTransform(pOutputPose,pTrackedDevicePose,pTransform);
   LOG_EXIT("CppPassalongApplyTransform");
}

vr::TrackedDeviceIndex_t VRSystemCppPassalong::GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole unDeviceType)
{
   LOG_ENTRY("CppPassalongGetTrackedDeviceIndexForControllerRole");
   vr::TrackedDeviceIndex_t rc = interfaces.cpp_interfaces()->sysi->GetTrackedDeviceIndexForControllerRole(unDeviceType);
   LOG_EXIT_RC(rc, "CppPassalongGetTrackedDeviceIndexForControllerRole");
}

vr::ETrackedControllerRole VRSystemCppPassalong::GetControllerRoleForTrackedDeviceIndex(vr::TrackedDeviceIndex_t unDeviceIndex)
{
   LOG_ENTRY("CppPassalongGetControllerRoleForTrackedDeviceIndex");
   vr::ETrackedControllerRole rc = interfaces.cpp_interfaces()->sysi->GetControllerRoleForTrackedDeviceIndex(unDeviceIndex);
   LOG_EXIT_RC(rc, "CppPassalongGetControllerRoleForTrackedDeviceIndex");
}

vr::ETrackedDeviceClass VRSystemCppPassalong::GetTrackedDeviceClass(vr::TrackedDeviceIndex_t unDeviceIndex)
{
   LOG_ENTRY("CppPassalongGetTrackedDeviceClass");
   vr::ETrackedDeviceClass rc = interfaces.cpp_interfaces()->sysi->GetTrackedDeviceClass(unDeviceIndex);
   LOG_EXIT_RC(rc, "CppPassalongGetTrackedDeviceClass");
}

bool VRSystemCppPassalong::IsTrackedDeviceConnected(vr::TrackedDeviceIndex_t unDeviceIndex)
{
   LOG_ENTRY("CppPassalongIsTrackedDeviceConnected");
   bool rc = interfaces.cpp_interfaces()->sysi->IsTrackedDeviceConnected(unDeviceIndex);
   LOG_EXIT_RC(rc, "CppPassalongIsTrackedDeviceConnected");
}

bool VRSystemCppPassalong::GetBoolTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("CppPassalongGetBoolTrackedDeviceProperty");
   bool rc = interfaces.cpp_interfaces()->sysi->GetBoolTrackedDeviceProperty(unDeviceIndex,prop,pError);
   LOG_EXIT_RC(rc, "CppPassalongGetBoolTrackedDeviceProperty");
}

float VRSystemCppPassalong::GetFloatTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("CppPassalongGetFloatTrackedDeviceProperty");
   float rc = interfaces.cpp_interfaces()->sysi->GetFloatTrackedDeviceProperty(unDeviceIndex,prop,pError);
   LOG_EXIT_RC(rc, "CppPassalongGetFloatTrackedDeviceProperty");
}

int32_t VRSystemCppPassalong::GetInt32TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("CppPassalongGetInt32TrackedDeviceProperty");
   int32_t rc = interfaces.cpp_interfaces()->sysi->GetInt32TrackedDeviceProperty(unDeviceIndex,prop,pError);
   LOG_EXIT_RC(rc, "CppPassalongGetInt32TrackedDeviceProperty");
}

uint64_t VRSystemCppPassalong::GetUint64TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("CppPassalongGetUint64TrackedDeviceProperty");
   uint64_t rc = interfaces.cpp_interfaces()->sysi->GetUint64TrackedDeviceProperty(unDeviceIndex,prop,pError);
   LOG_EXIT_RC(rc, "CppPassalongGetUint64TrackedDeviceProperty");
}

struct vr::HmdMatrix34_t VRSystemCppPassalong::GetMatrix34TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("CppPassalongGetMatrix34TrackedDeviceProperty");
   struct vr::HmdMatrix34_t rc = interfaces.cpp_interfaces()->sysi->GetMatrix34TrackedDeviceProperty(unDeviceIndex,prop,pError);
   LOG_EXIT_RC(rc, "CppPassalongGetMatrix34TrackedDeviceProperty");
}

uint32_t VRSystemCppPassalong::GetStringTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, char * pchValue, uint32_t unBufferSize, vr::ETrackedPropertyError * pError)
{
   LOG_ENTRY("CppPassalongGetStringTrackedDeviceProperty");
   uint32_t rc = interfaces.cpp_interfaces()->sysi->GetStringTrackedDeviceProperty(unDeviceIndex,prop,pchValue,unBufferSize,pError);
   LOG_EXIT_RC(rc, "CppPassalongGetStringTrackedDeviceProperty");
}

const char * VRSystemCppPassalong::GetPropErrorNameFromEnum(vr::ETrackedPropertyError error)
{
   LOG_ENTRY("CppPassalongGetPropErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->sysi->GetPropErrorNameFromEnum(error);
   LOG_EXIT_RC(rc, "CppPassalongGetPropErrorNameFromEnum");
}

bool VRSystemCppPassalong::PollNextEvent(struct vr::VREvent_t * pEvent, uint32_t uncbVREvent)
{
   LOG_ENTRY("CppPassalongPollNextEvent");
   bool rc = interfaces.cpp_interfaces()->sysi->PollNextEvent(pEvent,uncbVREvent);
   LOG_EXIT_RC(rc, "CppPassalongPollNextEvent");
}

bool VRSystemCppPassalong::PollNextEventWithPose(vr::ETrackingUniverseOrigin eOrigin, struct vr::VREvent_t * pEvent, uint32_t uncbVREvent, vr::TrackedDevicePose_t * pTrackedDevicePose)
{
   LOG_ENTRY("CppPassalongPollNextEventWithPose");
   bool rc = interfaces.cpp_interfaces()->sysi->PollNextEventWithPose(eOrigin,pEvent,uncbVREvent,pTrackedDevicePose);
   LOG_EXIT_RC(rc, "CppPassalongPollNextEventWithPose");
}

const char * VRSystemCppPassalong::GetEventTypeNameFromEnum(vr::EVREventType eType)
{
   LOG_ENTRY("CppPassalongGetEventTypeNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->sysi->GetEventTypeNameFromEnum(eType);
   LOG_EXIT_RC(rc, "CppPassalongGetEventTypeNameFromEnum");
}

struct vr::HiddenAreaMesh_t VRSystemCppPassalong::GetHiddenAreaMesh(vr::EVREye eEye, vr::EHiddenAreaMeshType type)
{
   LOG_ENTRY("CppPassalongGetHiddenAreaMesh");
   struct vr::HiddenAreaMesh_t rc = interfaces.cpp_interfaces()->sysi->GetHiddenAreaMesh(eEye,type);
   LOG_EXIT_RC(rc, "CppPassalongGetHiddenAreaMesh");
}

bool VRSystemCppPassalong::GetControllerState(vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t * pControllerState, uint32_t unControllerStateSize)
{
   LOG_ENTRY("CppPassalongGetControllerState");
   bool rc = interfaces.cpp_interfaces()->sysi->GetControllerState(unControllerDeviceIndex,pControllerState,unControllerStateSize);
   LOG_EXIT_RC(rc, "CppPassalongGetControllerState");
}

bool VRSystemCppPassalong::GetControllerStateWithPose(vr::ETrackingUniverseOrigin eOrigin, vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t * pControllerState, uint32_t unControllerStateSize, struct vr::TrackedDevicePose_t * pTrackedDevicePose)
{
   LOG_ENTRY("CppPassalongGetControllerStateWithPose");
   bool rc = interfaces.cpp_interfaces()->sysi->GetControllerStateWithPose(eOrigin,unControllerDeviceIndex,pControllerState,unControllerStateSize,pTrackedDevicePose);
   LOG_EXIT_RC(rc, "CppPassalongGetControllerStateWithPose");
}

void VRSystemCppPassalong::TriggerHapticPulse(vr::TrackedDeviceIndex_t unControllerDeviceIndex, uint32_t unAxisId, unsigned short usDurationMicroSec)
{
   LOG_ENTRY("CppPassalongTriggerHapticPulse");
   interfaces.cpp_interfaces()->sysi->TriggerHapticPulse(unControllerDeviceIndex,unAxisId,usDurationMicroSec);
   LOG_EXIT("CppPassalongTriggerHapticPulse");
}

const char * VRSystemCppPassalong::GetButtonIdNameFromEnum(vr::EVRButtonId eButtonId)
{
   LOG_ENTRY("CppPassalongGetButtonIdNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->sysi->GetButtonIdNameFromEnum(eButtonId);
   LOG_EXIT_RC(rc, "CppPassalongGetButtonIdNameFromEnum");
}

const char * VRSystemCppPassalong::GetControllerAxisTypeNameFromEnum(vr::EVRControllerAxisType eAxisType)
{
   LOG_ENTRY("CppPassalongGetControllerAxisTypeNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->sysi->GetControllerAxisTypeNameFromEnum(eAxisType);
   LOG_EXIT_RC(rc, "CppPassalongGetControllerAxisTypeNameFromEnum");
}

bool VRSystemCppPassalong::CaptureInputFocus()
{
   LOG_ENTRY("CppPassalongCaptureInputFocus");
   bool rc = interfaces.cpp_interfaces()->sysi->CaptureInputFocus();
   LOG_EXIT_RC(rc, "CppPassalongCaptureInputFocus");
}

void VRSystemCppPassalong::ReleaseInputFocus()
{
   LOG_ENTRY("CppPassalongReleaseInputFocus");
   interfaces.cpp_interfaces()->sysi->ReleaseInputFocus();
   LOG_EXIT("CppPassalongReleaseInputFocus");
}

bool VRSystemCppPassalong::IsInputFocusCapturedByAnotherProcess()
{
   LOG_ENTRY("CppPassalongIsInputFocusCapturedByAnotherProcess");
   bool rc = interfaces.cpp_interfaces()->sysi->IsInputFocusCapturedByAnotherProcess();
   LOG_EXIT_RC(rc, "CppPassalongIsInputFocusCapturedByAnotherProcess");
}

uint32_t VRSystemCppPassalong::DriverDebugRequest(vr::TrackedDeviceIndex_t unDeviceIndex, const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize)
{
   LOG_ENTRY("CppPassalongDriverDebugRequest");
   uint32_t rc = interfaces.cpp_interfaces()->sysi->DriverDebugRequest(unDeviceIndex,pchRequest,pchResponseBuffer,unResponseBufferSize);
   LOG_EXIT_RC(rc, "CppPassalongDriverDebugRequest");
}

vr::EVRFirmwareError VRSystemCppPassalong::PerformFirmwareUpdate(vr::TrackedDeviceIndex_t unDeviceIndex)
{
   LOG_ENTRY("CppPassalongPerformFirmwareUpdate");
   vr::EVRFirmwareError rc = interfaces.cpp_interfaces()->sysi->PerformFirmwareUpdate(unDeviceIndex);
   LOG_EXIT_RC(rc, "CppPassalongPerformFirmwareUpdate");
}

void VRSystemCppPassalong::AcknowledgeQuit_Exiting()
{
   LOG_ENTRY("CppPassalongAcknowledgeQuit_Exiting");
   interfaces.cpp_interfaces()->sysi->AcknowledgeQuit_Exiting();
   LOG_EXIT("CppPassalongAcknowledgeQuit_Exiting");
}

void VRSystemCppPassalong::AcknowledgeQuit_UserPrompt()
{
   LOG_ENTRY("CppPassalongAcknowledgeQuit_UserPrompt");
   interfaces.cpp_interfaces()->sysi->AcknowledgeQuit_UserPrompt();
   LOG_EXIT("CppPassalongAcknowledgeQuit_UserPrompt");
}

void VRExtendedDisplayCppPassalong::GetWindowBounds(int32_t * pnX, int32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight)
{
   LOG_ENTRY("CppPassalongGetWindowBounds");
   interfaces.cpp_interfaces()->exdi->GetWindowBounds(pnX,pnY,pnWidth,pnHeight);
   LOG_EXIT("CppPassalongGetWindowBounds");
}

void VRExtendedDisplayCppPassalong::GetEyeOutputViewport(vr::EVREye eEye, uint32_t * pnX, uint32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight)
{
   LOG_ENTRY("CppPassalongGetEyeOutputViewport");
   interfaces.cpp_interfaces()->exdi->GetEyeOutputViewport(eEye,pnX,pnY,pnWidth,pnHeight);
   LOG_EXIT("CppPassalongGetEyeOutputViewport");
}

void VRExtendedDisplayCppPassalong::GetDXGIOutputInfo(int32_t * pnAdapterIndex, int32_t * pnAdapterOutputIndex)
{
   LOG_ENTRY("CppPassalongGetDXGIOutputInfo");
   interfaces.cpp_interfaces()->exdi->GetDXGIOutputInfo(pnAdapterIndex,pnAdapterOutputIndex);
   LOG_EXIT("CppPassalongGetDXGIOutputInfo");
}

const char * VRTrackedCameraCppPassalong::GetCameraErrorNameFromEnum(vr::EVRTrackedCameraError eCameraError)
{
   LOG_ENTRY("CppPassalongGetCameraErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->taci->GetCameraErrorNameFromEnum(eCameraError);
   LOG_EXIT_RC(rc, "CppPassalongGetCameraErrorNameFromEnum");
}

vr::EVRTrackedCameraError VRTrackedCameraCppPassalong::HasCamera(vr::TrackedDeviceIndex_t nDeviceIndex, bool * pHasCamera)
{
   LOG_ENTRY("CppPassalongHasCamera");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->HasCamera(nDeviceIndex,pHasCamera);
   LOG_EXIT_RC(rc, "CppPassalongHasCamera");
}

vr::EVRTrackedCameraError VRTrackedCameraCppPassalong::GetCameraFrameSize(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType eFrameType, uint32_t * pnWidth, uint32_t * pnHeight, uint32_t * pnFrameBufferSize)
{
   LOG_ENTRY("CppPassalongGetCameraFrameSize");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetCameraFrameSize(nDeviceIndex,eFrameType,pnWidth,pnHeight,pnFrameBufferSize);
   LOG_EXIT_RC(rc, "CppPassalongGetCameraFrameSize");
}

vr::EVRTrackedCameraError VRTrackedCameraCppPassalong::GetCameraIntrinsics(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType eFrameType, vr::HmdVector2_t * pFocalLength, vr::HmdVector2_t * pCenter)
{
   LOG_ENTRY("CppPassalongGetCameraIntrinsics");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetCameraIntrinsics(nDeviceIndex,eFrameType,pFocalLength,pCenter);
   LOG_EXIT_RC(rc, "CppPassalongGetCameraIntrinsics");
}

vr::EVRTrackedCameraError VRTrackedCameraCppPassalong::GetCameraProjection(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType eFrameType, float flZNear, float flZFar, vr::HmdMatrix44_t * pProjection)
{
   LOG_ENTRY("CppPassalongGetCameraProjection");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetCameraProjection(nDeviceIndex,eFrameType,flZNear,flZFar,pProjection);
   LOG_EXIT_RC(rc, "CppPassalongGetCameraProjection");
}

vr::EVRTrackedCameraError VRTrackedCameraCppPassalong::AcquireVideoStreamingService(vr::TrackedDeviceIndex_t nDeviceIndex, vr::TrackedCameraHandle_t * pHandle)
{
   LOG_ENTRY("CppPassalongAcquireVideoStreamingService");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->AcquireVideoStreamingService(nDeviceIndex,pHandle);
   LOG_EXIT_RC(rc, "CppPassalongAcquireVideoStreamingService");
}

vr::EVRTrackedCameraError VRTrackedCameraCppPassalong::ReleaseVideoStreamingService(vr::TrackedCameraHandle_t hTrackedCamera)
{
   LOG_ENTRY("CppPassalongReleaseVideoStreamingService");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->ReleaseVideoStreamingService(hTrackedCamera);
   LOG_EXIT_RC(rc, "CppPassalongReleaseVideoStreamingService");
}

vr::EVRTrackedCameraError VRTrackedCameraCppPassalong::GetVideoStreamFrameBuffer(vr::TrackedCameraHandle_t hTrackedCamera, vr::EVRTrackedCameraFrameType eFrameType, void * pFrameBuffer, uint32_t nFrameBufferSize, vr::CameraVideoStreamFrameHeader_t * pFrameHeader, uint32_t nFrameHeaderSize)
{
   LOG_ENTRY("CppPassalongGetVideoStreamFrameBuffer");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetVideoStreamFrameBuffer(hTrackedCamera,eFrameType,pFrameBuffer,nFrameBufferSize,pFrameHeader,nFrameHeaderSize);
   LOG_EXIT_RC(rc, "CppPassalongGetVideoStreamFrameBuffer");
}

vr::EVRTrackedCameraError VRTrackedCameraCppPassalong::GetVideoStreamTextureSize(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType eFrameType, vr::VRTextureBounds_t * pTextureBounds, uint32_t * pnWidth, uint32_t * pnHeight)
{
   LOG_ENTRY("CppPassalongGetVideoStreamTextureSize");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetVideoStreamTextureSize(nDeviceIndex,eFrameType,pTextureBounds,pnWidth,pnHeight);
   LOG_EXIT_RC(rc, "CppPassalongGetVideoStreamTextureSize");
}

vr::EVRTrackedCameraError VRTrackedCameraCppPassalong::GetVideoStreamTextureD3D11(vr::TrackedCameraHandle_t hTrackedCamera, vr::EVRTrackedCameraFrameType eFrameType, void * pD3D11DeviceOrResource, void ** ppD3D11ShaderResourceView, vr::CameraVideoStreamFrameHeader_t * pFrameHeader, uint32_t nFrameHeaderSize)
{
   LOG_ENTRY("CppPassalongGetVideoStreamTextureD3D11");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetVideoStreamTextureD3D11(hTrackedCamera,eFrameType,pD3D11DeviceOrResource,ppD3D11ShaderResourceView,pFrameHeader,nFrameHeaderSize);
   LOG_EXIT_RC(rc, "CppPassalongGetVideoStreamTextureD3D11");
}

vr::EVRTrackedCameraError VRTrackedCameraCppPassalong::GetVideoStreamTextureGL(vr::TrackedCameraHandle_t hTrackedCamera, vr::EVRTrackedCameraFrameType eFrameType, vr::glUInt_t * pglTextureId, vr::CameraVideoStreamFrameHeader_t * pFrameHeader, uint32_t nFrameHeaderSize)
{
   LOG_ENTRY("CppPassalongGetVideoStreamTextureGL");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->GetVideoStreamTextureGL(hTrackedCamera,eFrameType,pglTextureId,pFrameHeader,nFrameHeaderSize);
   LOG_EXIT_RC(rc, "CppPassalongGetVideoStreamTextureGL");
}

vr::EVRTrackedCameraError VRTrackedCameraCppPassalong::ReleaseVideoStreamTextureGL(vr::TrackedCameraHandle_t hTrackedCamera, vr::glUInt_t glTextureId)
{
   LOG_ENTRY("CppPassalongReleaseVideoStreamTextureGL");
   vr::EVRTrackedCameraError rc = interfaces.cpp_interfaces()->taci->ReleaseVideoStreamTextureGL(hTrackedCamera,glTextureId);
   LOG_EXIT_RC(rc, "CppPassalongReleaseVideoStreamTextureGL");
}

vr::EVRApplicationError VRApplicationsCppPassalong::AddApplicationManifest(const char * pchApplicationManifestFullPath, bool bTemporary)
{
   LOG_ENTRY("CppPassalongAddApplicationManifest");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->AddApplicationManifest(pchApplicationManifestFullPath,bTemporary);
   LOG_EXIT_RC(rc, "CppPassalongAddApplicationManifest");
}

vr::EVRApplicationError VRApplicationsCppPassalong::RemoveApplicationManifest(const char * pchApplicationManifestFullPath)
{
   LOG_ENTRY("CppPassalongRemoveApplicationManifest");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->RemoveApplicationManifest(pchApplicationManifestFullPath);
   LOG_EXIT_RC(rc, "CppPassalongRemoveApplicationManifest");
}

bool VRApplicationsCppPassalong::IsApplicationInstalled(const char * pchAppKey)
{
   LOG_ENTRY("CppPassalongIsApplicationInstalled");
   bool rc = interfaces.cpp_interfaces()->appi->IsApplicationInstalled(pchAppKey);
   LOG_EXIT_RC(rc, "CppPassalongIsApplicationInstalled");
}

uint32_t VRApplicationsCppPassalong::GetApplicationCount()
{
   LOG_ENTRY("CppPassalongGetApplicationCount");
   uint32_t rc = interfaces.cpp_interfaces()->appi->GetApplicationCount();
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationCount");
}

vr::EVRApplicationError VRApplicationsCppPassalong::GetApplicationKeyByIndex(uint32_t unApplicationIndex, char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
   LOG_ENTRY("CppPassalongGetApplicationKeyByIndex");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->GetApplicationKeyByIndex(unApplicationIndex,pchAppKeyBuffer,unAppKeyBufferLen);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationKeyByIndex");
}

vr::EVRApplicationError VRApplicationsCppPassalong::GetApplicationKeyByProcessId(uint32_t unProcessId, char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
   LOG_ENTRY("CppPassalongGetApplicationKeyByProcessId");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->GetApplicationKeyByProcessId(unProcessId,pchAppKeyBuffer,unAppKeyBufferLen);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationKeyByProcessId");
}

vr::EVRApplicationError VRApplicationsCppPassalong::LaunchApplication(const char * pchAppKey)
{
   LOG_ENTRY("CppPassalongLaunchApplication");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->LaunchApplication(pchAppKey);
   LOG_EXIT_RC(rc, "CppPassalongLaunchApplication");
}

vr::EVRApplicationError VRApplicationsCppPassalong::LaunchTemplateApplication(const char * pchTemplateAppKey, const char * pchNewAppKey, const struct vr::AppOverrideKeys_t * pKeys, uint32_t unKeys)
{
   LOG_ENTRY("CppPassalongLaunchTemplateApplication");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->LaunchTemplateApplication(pchTemplateAppKey,pchNewAppKey,pKeys,unKeys);
   LOG_EXIT_RC(rc, "CppPassalongLaunchTemplateApplication");
}

vr::EVRApplicationError VRApplicationsCppPassalong::LaunchApplicationFromMimeType(const char * pchMimeType, const char * pchArgs)
{
   LOG_ENTRY("CppPassalongLaunchApplicationFromMimeType");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->LaunchApplicationFromMimeType(pchMimeType,pchArgs);
   LOG_EXIT_RC(rc, "CppPassalongLaunchApplicationFromMimeType");
}

vr::EVRApplicationError VRApplicationsCppPassalong::LaunchDashboardOverlay(const char * pchAppKey)
{
   LOG_ENTRY("CppPassalongLaunchDashboardOverlay");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->LaunchDashboardOverlay(pchAppKey);
   LOG_EXIT_RC(rc, "CppPassalongLaunchDashboardOverlay");
}

bool VRApplicationsCppPassalong::CancelApplicationLaunch(const char * pchAppKey)
{
   LOG_ENTRY("CppPassalongCancelApplicationLaunch");
   bool rc = interfaces.cpp_interfaces()->appi->CancelApplicationLaunch(pchAppKey);
   LOG_EXIT_RC(rc, "CppPassalongCancelApplicationLaunch");
}

vr::EVRApplicationError VRApplicationsCppPassalong::IdentifyApplication(uint32_t unProcessId, const char * pchAppKey)
{
   LOG_ENTRY("CppPassalongIdentifyApplication");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->IdentifyApplication(unProcessId,pchAppKey);
   LOG_EXIT_RC(rc, "CppPassalongIdentifyApplication");
}

uint32_t VRApplicationsCppPassalong::GetApplicationProcessId(const char * pchAppKey)
{
   LOG_ENTRY("CppPassalongGetApplicationProcessId");
   uint32_t rc = interfaces.cpp_interfaces()->appi->GetApplicationProcessId(pchAppKey);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationProcessId");
}

const char * VRApplicationsCppPassalong::GetApplicationsErrorNameFromEnum(vr::EVRApplicationError error)
{
   LOG_ENTRY("CppPassalongGetApplicationsErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->appi->GetApplicationsErrorNameFromEnum(error);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationsErrorNameFromEnum");
}

uint32_t VRApplicationsCppPassalong::GetApplicationPropertyString(const char * pchAppKey, vr::EVRApplicationProperty eProperty, char * pchPropertyValueBuffer, uint32_t unPropertyValueBufferLen, vr::EVRApplicationError * peError)
{
   LOG_ENTRY("CppPassalongGetApplicationPropertyString");
   uint32_t rc = interfaces.cpp_interfaces()->appi->GetApplicationPropertyString(pchAppKey,eProperty,pchPropertyValueBuffer,unPropertyValueBufferLen,peError);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationPropertyString");
}

bool VRApplicationsCppPassalong::GetApplicationPropertyBool(const char * pchAppKey, vr::EVRApplicationProperty eProperty, vr::EVRApplicationError * peError)
{
   LOG_ENTRY("CppPassalongGetApplicationPropertyBool");
   bool rc = interfaces.cpp_interfaces()->appi->GetApplicationPropertyBool(pchAppKey,eProperty,peError);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationPropertyBool");
}

uint64_t VRApplicationsCppPassalong::GetApplicationPropertyUint64(const char * pchAppKey, vr::EVRApplicationProperty eProperty, vr::EVRApplicationError * peError)
{
   LOG_ENTRY("CppPassalongGetApplicationPropertyUint64");
   uint64_t rc = interfaces.cpp_interfaces()->appi->GetApplicationPropertyUint64(pchAppKey,eProperty,peError);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationPropertyUint64");
}

vr::EVRApplicationError VRApplicationsCppPassalong::SetApplicationAutoLaunch(const char * pchAppKey, bool bAutoLaunch)
{
   LOG_ENTRY("CppPassalongSetApplicationAutoLaunch");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->SetApplicationAutoLaunch(pchAppKey,bAutoLaunch);
   LOG_EXIT_RC(rc, "CppPassalongSetApplicationAutoLaunch");
}

bool VRApplicationsCppPassalong::GetApplicationAutoLaunch(const char * pchAppKey)
{
   LOG_ENTRY("CppPassalongGetApplicationAutoLaunch");
   bool rc = interfaces.cpp_interfaces()->appi->GetApplicationAutoLaunch(pchAppKey);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationAutoLaunch");
}

vr::EVRApplicationError VRApplicationsCppPassalong::SetDefaultApplicationForMimeType(const char * pchAppKey, const char * pchMimeType)
{
   LOG_ENTRY("CppPassalongSetDefaultApplicationForMimeType");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->SetDefaultApplicationForMimeType(pchAppKey,pchMimeType);
   LOG_EXIT_RC(rc, "CppPassalongSetDefaultApplicationForMimeType");
}

bool VRApplicationsCppPassalong::GetDefaultApplicationForMimeType(const char * pchMimeType, char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
   LOG_ENTRY("CppPassalongGetDefaultApplicationForMimeType");
   bool rc = interfaces.cpp_interfaces()->appi->GetDefaultApplicationForMimeType(pchMimeType,pchAppKeyBuffer,unAppKeyBufferLen);
   LOG_EXIT_RC(rc, "CppPassalongGetDefaultApplicationForMimeType");
}

bool VRApplicationsCppPassalong::GetApplicationSupportedMimeTypes(const char * pchAppKey, char * pchMimeTypesBuffer, uint32_t unMimeTypesBuffer)
{
   LOG_ENTRY("CppPassalongGetApplicationSupportedMimeTypes");
   bool rc = interfaces.cpp_interfaces()->appi->GetApplicationSupportedMimeTypes(pchAppKey,pchMimeTypesBuffer,unMimeTypesBuffer);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationSupportedMimeTypes");
}

uint32_t VRApplicationsCppPassalong::GetApplicationsThatSupportMimeType(const char * pchMimeType, char * pchAppKeysThatSupportBuffer, uint32_t unAppKeysThatSupportBuffer)
{
   LOG_ENTRY("CppPassalongGetApplicationsThatSupportMimeType");
   uint32_t rc = interfaces.cpp_interfaces()->appi->GetApplicationsThatSupportMimeType(pchMimeType,pchAppKeysThatSupportBuffer,unAppKeysThatSupportBuffer);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationsThatSupportMimeType");
}

uint32_t VRApplicationsCppPassalong::GetApplicationLaunchArguments(uint32_t unHandle, char * pchArgs, uint32_t unArgs)
{
   LOG_ENTRY("CppPassalongGetApplicationLaunchArguments");
   uint32_t rc = interfaces.cpp_interfaces()->appi->GetApplicationLaunchArguments(unHandle,pchArgs,unArgs);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationLaunchArguments");
}

vr::EVRApplicationError VRApplicationsCppPassalong::GetStartingApplication(char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
   LOG_ENTRY("CppPassalongGetStartingApplication");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->GetStartingApplication(pchAppKeyBuffer,unAppKeyBufferLen);
   LOG_EXIT_RC(rc, "CppPassalongGetStartingApplication");
}

vr::EVRApplicationTransitionState VRApplicationsCppPassalong::GetTransitionState()
{
   LOG_ENTRY("CppPassalongGetTransitionState");
   vr::EVRApplicationTransitionState rc = interfaces.cpp_interfaces()->appi->GetTransitionState();
   LOG_EXIT_RC(rc, "CppPassalongGetTransitionState");
}

vr::EVRApplicationError VRApplicationsCppPassalong::PerformApplicationPrelaunchCheck(const char * pchAppKey)
{
   LOG_ENTRY("CppPassalongPerformApplicationPrelaunchCheck");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->PerformApplicationPrelaunchCheck(pchAppKey);
   LOG_EXIT_RC(rc, "CppPassalongPerformApplicationPrelaunchCheck");
}

const char * VRApplicationsCppPassalong::GetApplicationsTransitionStateNameFromEnum(vr::EVRApplicationTransitionState state)
{
   LOG_ENTRY("CppPassalongGetApplicationsTransitionStateNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->appi->GetApplicationsTransitionStateNameFromEnum(state);
   LOG_EXIT_RC(rc, "CppPassalongGetApplicationsTransitionStateNameFromEnum");
}

bool VRApplicationsCppPassalong::IsQuitUserPromptRequested()
{
   LOG_ENTRY("CppPassalongIsQuitUserPromptRequested");
   bool rc = interfaces.cpp_interfaces()->appi->IsQuitUserPromptRequested();
   LOG_EXIT_RC(rc, "CppPassalongIsQuitUserPromptRequested");
}

vr::EVRApplicationError VRApplicationsCppPassalong::LaunchInternalProcess(const char * pchBinaryPath, const char * pchArguments, const char * pchWorkingDirectory)
{
   LOG_ENTRY("CppPassalongLaunchInternalProcess");
   vr::EVRApplicationError rc = interfaces.cpp_interfaces()->appi->LaunchInternalProcess(pchBinaryPath,pchArguments,pchWorkingDirectory);
   LOG_EXIT_RC(rc, "CppPassalongLaunchInternalProcess");
}

uint32_t VRApplicationsCppPassalong::GetCurrentSceneProcessId()
{
   LOG_ENTRY("CppPassalongGetCurrentSceneProcessId");
   uint32_t rc = interfaces.cpp_interfaces()->appi->GetCurrentSceneProcessId();
   LOG_EXIT_RC(rc, "CppPassalongGetCurrentSceneProcessId");
}

vr::ChaperoneCalibrationState VRChaperoneCppPassalong::GetCalibrationState()
{
   LOG_ENTRY("CppPassalongGetCalibrationState");
   vr::ChaperoneCalibrationState rc = interfaces.cpp_interfaces()->chapi->GetCalibrationState();
   LOG_EXIT_RC(rc, "CppPassalongGetCalibrationState");
}

bool VRChaperoneCppPassalong::GetPlayAreaSize(float * pSizeX, float * pSizeZ)
{
   LOG_ENTRY("CppPassalongGetPlayAreaSize");
   bool rc = interfaces.cpp_interfaces()->chapi->GetPlayAreaSize(pSizeX,pSizeZ);
   LOG_EXIT_RC(rc, "CppPassalongGetPlayAreaSize");
}

bool VRChaperoneCppPassalong::GetPlayAreaRect(struct vr::HmdQuad_t * rect)
{
   LOG_ENTRY("CppPassalongGetPlayAreaRect");
   bool rc = interfaces.cpp_interfaces()->chapi->GetPlayAreaRect(rect);
   LOG_EXIT_RC(rc, "CppPassalongGetPlayAreaRect");
}

void VRChaperoneCppPassalong::ReloadInfo()
{
   LOG_ENTRY("CppPassalongReloadInfo");
   interfaces.cpp_interfaces()->chapi->ReloadInfo();
   LOG_EXIT("CppPassalongReloadInfo");
}

void VRChaperoneCppPassalong::SetSceneColor(struct vr::HmdColor_t color)
{
   LOG_ENTRY("CppPassalongSetSceneColor");
   interfaces.cpp_interfaces()->chapi->SetSceneColor(color);
   LOG_EXIT("CppPassalongSetSceneColor");
}

void VRChaperoneCppPassalong::GetBoundsColor(struct vr::HmdColor_t * pOutputColorArray, int nNumOutputColors, float flCollisionBoundsFadeDistance, struct vr::HmdColor_t * pOutputCameraColor)
{
   LOG_ENTRY("CppPassalongGetBoundsColor");
   interfaces.cpp_interfaces()->chapi->GetBoundsColor(pOutputColorArray,nNumOutputColors,flCollisionBoundsFadeDistance,pOutputCameraColor);
   LOG_EXIT("CppPassalongGetBoundsColor");
}

bool VRChaperoneCppPassalong::AreBoundsVisible()
{
   LOG_ENTRY("CppPassalongAreBoundsVisible");
   bool rc = interfaces.cpp_interfaces()->chapi->AreBoundsVisible();
   LOG_EXIT_RC(rc, "CppPassalongAreBoundsVisible");
}

void VRChaperoneCppPassalong::ForceBoundsVisible(bool bForce)
{
   LOG_ENTRY("CppPassalongForceBoundsVisible");
   interfaces.cpp_interfaces()->chapi->ForceBoundsVisible(bForce);
   LOG_EXIT("CppPassalongForceBoundsVisible");
}

bool VRChaperoneSetupCppPassalong::CommitWorkingCopy(vr::EChaperoneConfigFile configFile)
{
   LOG_ENTRY("CppPassalongCommitWorkingCopy");
   bool rc = interfaces.cpp_interfaces()->chapsi->CommitWorkingCopy(configFile);
   LOG_EXIT_RC(rc, "CppPassalongCommitWorkingCopy");
}

void VRChaperoneSetupCppPassalong::RevertWorkingCopy()
{
   LOG_ENTRY("CppPassalongRevertWorkingCopy");
   interfaces.cpp_interfaces()->chapsi->RevertWorkingCopy();
   LOG_EXIT("CppPassalongRevertWorkingCopy");
}

bool VRChaperoneSetupCppPassalong::GetWorkingPlayAreaSize(float * pSizeX, float * pSizeZ)
{
   LOG_ENTRY("CppPassalongGetWorkingPlayAreaSize");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetWorkingPlayAreaSize(pSizeX,pSizeZ);
   LOG_EXIT_RC(rc, "CppPassalongGetWorkingPlayAreaSize");
}

bool VRChaperoneSetupCppPassalong::GetWorkingPlayAreaRect(struct vr::HmdQuad_t * rect)
{
   LOG_ENTRY("CppPassalongGetWorkingPlayAreaRect");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetWorkingPlayAreaRect(rect);
   LOG_EXIT_RC(rc, "CppPassalongGetWorkingPlayAreaRect");
}

bool VRChaperoneSetupCppPassalong::GetWorkingCollisionBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t * punQuadsCount)
{
   LOG_ENTRY("CppPassalongGetWorkingCollisionBoundsInfo");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetWorkingCollisionBoundsInfo(pQuadsBuffer,punQuadsCount);
   LOG_EXIT_RC(rc, "CppPassalongGetWorkingCollisionBoundsInfo");
}

bool VRChaperoneSetupCppPassalong::GetLiveCollisionBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t * punQuadsCount)
{
   LOG_ENTRY("CppPassalongGetLiveCollisionBoundsInfo");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetLiveCollisionBoundsInfo(pQuadsBuffer,punQuadsCount);
   LOG_EXIT_RC(rc, "CppPassalongGetLiveCollisionBoundsInfo");
}

bool VRChaperoneSetupCppPassalong::GetWorkingSeatedZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pmatSeatedZeroPoseToRawTrackingPose)
{
   LOG_ENTRY("CppPassalongGetWorkingSeatedZeroPoseToRawTrackingPose");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetWorkingSeatedZeroPoseToRawTrackingPose(pmatSeatedZeroPoseToRawTrackingPose);
   LOG_EXIT_RC(rc, "CppPassalongGetWorkingSeatedZeroPoseToRawTrackingPose");
}

bool VRChaperoneSetupCppPassalong::GetWorkingStandingZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pmatStandingZeroPoseToRawTrackingPose)
{
   LOG_ENTRY("CppPassalongGetWorkingStandingZeroPoseToRawTrackingPose");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetWorkingStandingZeroPoseToRawTrackingPose(pmatStandingZeroPoseToRawTrackingPose);
   LOG_EXIT_RC(rc, "CppPassalongGetWorkingStandingZeroPoseToRawTrackingPose");
}

void VRChaperoneSetupCppPassalong::SetWorkingPlayAreaSize(float sizeX, float sizeZ)
{
   LOG_ENTRY("CppPassalongSetWorkingPlayAreaSize");
   interfaces.cpp_interfaces()->chapsi->SetWorkingPlayAreaSize(sizeX,sizeZ);
   LOG_EXIT("CppPassalongSetWorkingPlayAreaSize");
}

void VRChaperoneSetupCppPassalong::SetWorkingCollisionBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t unQuadsCount)
{
   LOG_ENTRY("CppPassalongSetWorkingCollisionBoundsInfo");
   interfaces.cpp_interfaces()->chapsi->SetWorkingCollisionBoundsInfo(pQuadsBuffer,unQuadsCount);
   LOG_EXIT("CppPassalongSetWorkingCollisionBoundsInfo");
}

void VRChaperoneSetupCppPassalong::SetWorkingSeatedZeroPoseToRawTrackingPose(const struct vr::HmdMatrix34_t * pMatSeatedZeroPoseToRawTrackingPose)
{
   LOG_ENTRY("CppPassalongSetWorkingSeatedZeroPoseToRawTrackingPose");
   interfaces.cpp_interfaces()->chapsi->SetWorkingSeatedZeroPoseToRawTrackingPose(pMatSeatedZeroPoseToRawTrackingPose);
   LOG_EXIT("CppPassalongSetWorkingSeatedZeroPoseToRawTrackingPose");
}

void VRChaperoneSetupCppPassalong::SetWorkingStandingZeroPoseToRawTrackingPose(const struct vr::HmdMatrix34_t * pMatStandingZeroPoseToRawTrackingPose)
{
   LOG_ENTRY("CppPassalongSetWorkingStandingZeroPoseToRawTrackingPose");
   interfaces.cpp_interfaces()->chapsi->SetWorkingStandingZeroPoseToRawTrackingPose(pMatStandingZeroPoseToRawTrackingPose);
   LOG_EXIT("CppPassalongSetWorkingStandingZeroPoseToRawTrackingPose");
}

void VRChaperoneSetupCppPassalong::ReloadFromDisk(vr::EChaperoneConfigFile configFile)
{
   LOG_ENTRY("CppPassalongReloadFromDisk");
   interfaces.cpp_interfaces()->chapsi->ReloadFromDisk(configFile);
   LOG_EXIT("CppPassalongReloadFromDisk");
}

bool VRChaperoneSetupCppPassalong::GetLiveSeatedZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pmatSeatedZeroPoseToRawTrackingPose)
{
   LOG_ENTRY("CppPassalongGetLiveSeatedZeroPoseToRawTrackingPose");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(pmatSeatedZeroPoseToRawTrackingPose);
   LOG_EXIT_RC(rc, "CppPassalongGetLiveSeatedZeroPoseToRawTrackingPose");
}

void VRChaperoneSetupCppPassalong::SetWorkingCollisionBoundsTagsInfo(uint8_t * pTagsBuffer, uint32_t unTagCount)
{
   LOG_ENTRY("CppPassalongSetWorkingCollisionBoundsTagsInfo");
   interfaces.cpp_interfaces()->chapsi->SetWorkingCollisionBoundsTagsInfo(pTagsBuffer,unTagCount);
   LOG_EXIT("CppPassalongSetWorkingCollisionBoundsTagsInfo");
}

bool VRChaperoneSetupCppPassalong::GetLiveCollisionBoundsTagsInfo(uint8_t * pTagsBuffer, uint32_t * punTagCount)
{
   LOG_ENTRY("CppPassalongGetLiveCollisionBoundsTagsInfo");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetLiveCollisionBoundsTagsInfo(pTagsBuffer,punTagCount);
   LOG_EXIT_RC(rc, "CppPassalongGetLiveCollisionBoundsTagsInfo");
}

bool VRChaperoneSetupCppPassalong::SetWorkingPhysicalBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t unQuadsCount)
{
   LOG_ENTRY("CppPassalongSetWorkingPhysicalBoundsInfo");
   bool rc = interfaces.cpp_interfaces()->chapsi->SetWorkingPhysicalBoundsInfo(pQuadsBuffer,unQuadsCount);
   LOG_EXIT_RC(rc, "CppPassalongSetWorkingPhysicalBoundsInfo");
}

bool VRChaperoneSetupCppPassalong::GetLivePhysicalBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t * punQuadsCount)
{
   LOG_ENTRY("CppPassalongGetLivePhysicalBoundsInfo");
   bool rc = interfaces.cpp_interfaces()->chapsi->GetLivePhysicalBoundsInfo(pQuadsBuffer,punQuadsCount);
   LOG_EXIT_RC(rc, "CppPassalongGetLivePhysicalBoundsInfo");
}

bool VRChaperoneSetupCppPassalong::ExportLiveToBuffer(char * pBuffer, uint32_t * pnBufferLength)
{
   LOG_ENTRY("CppPassalongExportLiveToBuffer");
   bool rc = interfaces.cpp_interfaces()->chapsi->ExportLiveToBuffer(pBuffer,pnBufferLength);
   LOG_EXIT_RC(rc, "CppPassalongExportLiveToBuffer");
}

bool VRChaperoneSetupCppPassalong::ImportFromBufferToWorking(const char * pBuffer, uint32_t nImportFlags)
{
   LOG_ENTRY("CppPassalongImportFromBufferToWorking");
   bool rc = interfaces.cpp_interfaces()->chapsi->ImportFromBufferToWorking(pBuffer,nImportFlags);
   LOG_EXIT_RC(rc, "CppPassalongImportFromBufferToWorking");
}

void VRCompositorCppPassalong::SetTrackingSpace(vr::ETrackingUniverseOrigin eOrigin)
{
   LOG_ENTRY("CppPassalongSetTrackingSpace");
   interfaces.cpp_interfaces()->compi->SetTrackingSpace(eOrigin);
   LOG_EXIT("CppPassalongSetTrackingSpace");
}

vr::ETrackingUniverseOrigin VRCompositorCppPassalong::GetTrackingSpace()
{
   LOG_ENTRY("CppPassalongGetTrackingSpace");
   vr::ETrackingUniverseOrigin rc = interfaces.cpp_interfaces()->compi->GetTrackingSpace();
   LOG_EXIT_RC(rc, "CppPassalongGetTrackingSpace");
}

vr::EVRCompositorError VRCompositorCppPassalong::WaitGetPoses(struct vr::TrackedDevicePose_t * pRenderPoseArray, uint32_t unRenderPoseArrayCount, struct vr::TrackedDevicePose_t * pGamePoseArray, uint32_t unGamePoseArrayCount)
{
   LOG_ENTRY("CppPassalongWaitGetPoses");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->WaitGetPoses(pRenderPoseArray,unRenderPoseArrayCount,pGamePoseArray,unGamePoseArrayCount);
   LOG_EXIT_RC(rc, "CppPassalongWaitGetPoses");
}

vr::EVRCompositorError VRCompositorCppPassalong::GetLastPoses(struct vr::TrackedDevicePose_t * pRenderPoseArray, uint32_t unRenderPoseArrayCount, struct vr::TrackedDevicePose_t * pGamePoseArray, uint32_t unGamePoseArrayCount)
{
   LOG_ENTRY("CppPassalongGetLastPoses");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->GetLastPoses(pRenderPoseArray,unRenderPoseArrayCount,pGamePoseArray,unGamePoseArrayCount);
   LOG_EXIT_RC(rc, "CppPassalongGetLastPoses");
}

vr::EVRCompositorError VRCompositorCppPassalong::GetLastPoseForTrackedDeviceIndex(vr::TrackedDeviceIndex_t unDeviceIndex, struct vr::TrackedDevicePose_t * pOutputPose, struct vr::TrackedDevicePose_t * pOutputGamePose)
{
   LOG_ENTRY("CppPassalongGetLastPoseForTrackedDeviceIndex");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->GetLastPoseForTrackedDeviceIndex(unDeviceIndex,pOutputPose,pOutputGamePose);
   LOG_EXIT_RC(rc, "CppPassalongGetLastPoseForTrackedDeviceIndex");
}

vr::EVRCompositorError VRCompositorCppPassalong::Submit(vr::EVREye eEye, const struct vr::Texture_t * pTexture, const struct vr::VRTextureBounds_t * pBounds, vr::EVRSubmitFlags nSubmitFlags)
{
   LOG_ENTRY("CppPassalongSubmit");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->Submit(eEye,pTexture,pBounds,nSubmitFlags);
   LOG_EXIT_RC(rc, "CppPassalongSubmit");
}

void VRCompositorCppPassalong::ClearLastSubmittedFrame()
{
   LOG_ENTRY("CppPassalongClearLastSubmittedFrame");
   interfaces.cpp_interfaces()->compi->ClearLastSubmittedFrame();
   LOG_EXIT("CppPassalongClearLastSubmittedFrame");
}

void VRCompositorCppPassalong::PostPresentHandoff()
{
   LOG_ENTRY("CppPassalongPostPresentHandoff");
   interfaces.cpp_interfaces()->compi->PostPresentHandoff();
   LOG_EXIT("CppPassalongPostPresentHandoff");
}

bool VRCompositorCppPassalong::GetFrameTiming(struct vr::Compositor_FrameTiming * pTiming, uint32_t unFramesAgo)
{
   LOG_ENTRY("CppPassalongGetFrameTiming");
   bool rc = interfaces.cpp_interfaces()->compi->GetFrameTiming(pTiming,unFramesAgo);
   LOG_EXIT_RC(rc, "CppPassalongGetFrameTiming");
}

uint32_t VRCompositorCppPassalong::GetFrameTimings(struct vr::Compositor_FrameTiming * pTiming, uint32_t nFrames)
{
   LOG_ENTRY("CppPassalongGetFrameTimings");
   uint32_t rc = interfaces.cpp_interfaces()->compi->GetFrameTimings(pTiming,nFrames);
   LOG_EXIT_RC(rc, "CppPassalongGetFrameTimings");
}

float VRCompositorCppPassalong::GetFrameTimeRemaining()
{
   LOG_ENTRY("CppPassalongGetFrameTimeRemaining");
   float rc = interfaces.cpp_interfaces()->compi->GetFrameTimeRemaining();
   LOG_EXIT_RC(rc, "CppPassalongGetFrameTimeRemaining");
}

void VRCompositorCppPassalong::GetCumulativeStats(struct vr::Compositor_CumulativeStats * pStats, uint32_t nStatsSizeInBytes)
{
   LOG_ENTRY("CppPassalongGetCumulativeStats");
   interfaces.cpp_interfaces()->compi->GetCumulativeStats(pStats,nStatsSizeInBytes);
   LOG_EXIT("CppPassalongGetCumulativeStats");
}

void VRCompositorCppPassalong::FadeToColor(float fSeconds, float fRed, float fGreen, float fBlue, float fAlpha, bool bBackground)
{
   LOG_ENTRY("CppPassalongFadeToColor");
   interfaces.cpp_interfaces()->compi->FadeToColor(fSeconds,fRed,fGreen,fBlue,fAlpha,bBackground);
   LOG_EXIT("CppPassalongFadeToColor");
}

struct vr::HmdColor_t VRCompositorCppPassalong::GetCurrentFadeColor(bool bBackground)
{
   LOG_ENTRY("CppPassalongGetCurrentFadeColor");
   struct vr::HmdColor_t rc = interfaces.cpp_interfaces()->compi->GetCurrentFadeColor(bBackground);
   LOG_EXIT_RC(rc, "CppPassalongGetCurrentFadeColor");
}

void VRCompositorCppPassalong::FadeGrid(float fSeconds, bool bFadeIn)
{
   LOG_ENTRY("CppPassalongFadeGrid");
   interfaces.cpp_interfaces()->compi->FadeGrid(fSeconds,bFadeIn);
   LOG_EXIT("CppPassalongFadeGrid");
}

float VRCompositorCppPassalong::GetCurrentGridAlpha()
{
   LOG_ENTRY("CppPassalongGetCurrentGridAlpha");
   float rc = interfaces.cpp_interfaces()->compi->GetCurrentGridAlpha();
   LOG_EXIT_RC(rc, "CppPassalongGetCurrentGridAlpha");
}

vr::EVRCompositorError VRCompositorCppPassalong::SetSkyboxOverride(const struct vr::Texture_t * pTextures, uint32_t unTextureCount)
{
   LOG_ENTRY("CppPassalongSetSkyboxOverride");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->SetSkyboxOverride(pTextures,unTextureCount);
   LOG_EXIT_RC(rc, "CppPassalongSetSkyboxOverride");
}

void VRCompositorCppPassalong::ClearSkyboxOverride()
{
   LOG_ENTRY("CppPassalongClearSkyboxOverride");
   interfaces.cpp_interfaces()->compi->ClearSkyboxOverride();
   LOG_EXIT("CppPassalongClearSkyboxOverride");
}

void VRCompositorCppPassalong::CompositorBringToFront()
{
   LOG_ENTRY("CppPassalongCompositorBringToFront");
   interfaces.cpp_interfaces()->compi->CompositorBringToFront();
   LOG_EXIT("CppPassalongCompositorBringToFront");
}

void VRCompositorCppPassalong::CompositorGoToBack()
{
   LOG_ENTRY("CppPassalongCompositorGoToBack");
   interfaces.cpp_interfaces()->compi->CompositorGoToBack();
   LOG_EXIT("CppPassalongCompositorGoToBack");
}

void VRCompositorCppPassalong::CompositorQuit()
{
   LOG_ENTRY("CppPassalongCompositorQuit");
   interfaces.cpp_interfaces()->compi->CompositorQuit();
   LOG_EXIT("CppPassalongCompositorQuit");
}

bool VRCompositorCppPassalong::IsFullscreen()
{
   LOG_ENTRY("CppPassalongIsFullscreen");
   bool rc = interfaces.cpp_interfaces()->compi->IsFullscreen();
   LOG_EXIT_RC(rc, "CppPassalongIsFullscreen");
}

uint32_t VRCompositorCppPassalong::GetCurrentSceneFocusProcess()
{
   LOG_ENTRY("CppPassalongGetCurrentSceneFocusProcess");
   uint32_t rc = interfaces.cpp_interfaces()->compi->GetCurrentSceneFocusProcess();
   LOG_EXIT_RC(rc, "CppPassalongGetCurrentSceneFocusProcess");
}

uint32_t VRCompositorCppPassalong::GetLastFrameRenderer()
{
   LOG_ENTRY("CppPassalongGetLastFrameRenderer");
   uint32_t rc = interfaces.cpp_interfaces()->compi->GetLastFrameRenderer();
   LOG_EXIT_RC(rc, "CppPassalongGetLastFrameRenderer");
}

bool VRCompositorCppPassalong::CanRenderScene()
{
   LOG_ENTRY("CppPassalongCanRenderScene");
   bool rc = interfaces.cpp_interfaces()->compi->CanRenderScene();
   LOG_EXIT_RC(rc, "CppPassalongCanRenderScene");
}

void VRCompositorCppPassalong::ShowMirrorWindow()
{
   LOG_ENTRY("CppPassalongShowMirrorWindow");
   interfaces.cpp_interfaces()->compi->ShowMirrorWindow();
   LOG_EXIT("CppPassalongShowMirrorWindow");
}

void VRCompositorCppPassalong::HideMirrorWindow()
{
   LOG_ENTRY("CppPassalongHideMirrorWindow");
   interfaces.cpp_interfaces()->compi->HideMirrorWindow();
   LOG_EXIT("CppPassalongHideMirrorWindow");
}

bool VRCompositorCppPassalong::IsMirrorWindowVisible()
{
   LOG_ENTRY("CppPassalongIsMirrorWindowVisible");
   bool rc = interfaces.cpp_interfaces()->compi->IsMirrorWindowVisible();
   LOG_EXIT_RC(rc, "CppPassalongIsMirrorWindowVisible");
}

void VRCompositorCppPassalong::CompositorDumpImages()
{
   LOG_ENTRY("CppPassalongCompositorDumpImages");
   interfaces.cpp_interfaces()->compi->CompositorDumpImages();
   LOG_EXIT("CppPassalongCompositorDumpImages");
}

bool VRCompositorCppPassalong::ShouldAppRenderWithLowResources()
{
   LOG_ENTRY("CppPassalongShouldAppRenderWithLowResources");
   bool rc = interfaces.cpp_interfaces()->compi->ShouldAppRenderWithLowResources();
   LOG_EXIT_RC(rc, "CppPassalongShouldAppRenderWithLowResources");
}

void VRCompositorCppPassalong::ForceInterleavedReprojectionOn(bool bOverride)
{
   LOG_ENTRY("CppPassalongForceInterleavedReprojectionOn");
   interfaces.cpp_interfaces()->compi->ForceInterleavedReprojectionOn(bOverride);
   LOG_EXIT("CppPassalongForceInterleavedReprojectionOn");
}

void VRCompositorCppPassalong::ForceReconnectProcess()
{
   LOG_ENTRY("CppPassalongForceReconnectProcess");
   interfaces.cpp_interfaces()->compi->ForceReconnectProcess();
   LOG_EXIT("CppPassalongForceReconnectProcess");
}

void VRCompositorCppPassalong::SuspendRendering(bool bSuspend)
{
   LOG_ENTRY("CppPassalongSuspendRendering");
   interfaces.cpp_interfaces()->compi->SuspendRendering(bSuspend);
   LOG_EXIT("CppPassalongSuspendRendering");
}

vr::EVRCompositorError VRCompositorCppPassalong::GetMirrorTextureD3D11(vr::EVREye eEye, void * pD3D11DeviceOrResource, void ** ppD3D11ShaderResourceView)
{
   LOG_ENTRY("CppPassalongGetMirrorTextureD3D11");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->GetMirrorTextureD3D11(eEye,pD3D11DeviceOrResource,ppD3D11ShaderResourceView);
   LOG_EXIT_RC(rc, "CppPassalongGetMirrorTextureD3D11");
}

void VRCompositorCppPassalong::ReleaseMirrorTextureD3D11(void * pD3D11ShaderResourceView)
{
   LOG_ENTRY("CppPassalongReleaseMirrorTextureD3D11");
   interfaces.cpp_interfaces()->compi->ReleaseMirrorTextureD3D11(pD3D11ShaderResourceView);
   LOG_EXIT("CppPassalongReleaseMirrorTextureD3D11");
}

vr::EVRCompositorError VRCompositorCppPassalong::GetMirrorTextureGL(vr::EVREye eEye, vr::glUInt_t * pglTextureId, vr::glSharedTextureHandle_t * pglSharedTextureHandle)
{
   LOG_ENTRY("CppPassalongGetMirrorTextureGL");
   vr::EVRCompositorError rc = interfaces.cpp_interfaces()->compi->GetMirrorTextureGL(eEye,pglTextureId,pglSharedTextureHandle);
   LOG_EXIT_RC(rc, "CppPassalongGetMirrorTextureGL");
}

bool VRCompositorCppPassalong::ReleaseSharedGLTexture(vr::glUInt_t glTextureId, vr::glSharedTextureHandle_t glSharedTextureHandle)
{
   LOG_ENTRY("CppPassalongReleaseSharedGLTexture");
   bool rc = interfaces.cpp_interfaces()->compi->ReleaseSharedGLTexture(glTextureId,glSharedTextureHandle);
   LOG_EXIT_RC(rc, "CppPassalongReleaseSharedGLTexture");
}

void VRCompositorCppPassalong::LockGLSharedTextureForAccess(vr::glSharedTextureHandle_t glSharedTextureHandle)
{
   LOG_ENTRY("CppPassalongLockGLSharedTextureForAccess");
   interfaces.cpp_interfaces()->compi->LockGLSharedTextureForAccess(glSharedTextureHandle);
   LOG_EXIT("CppPassalongLockGLSharedTextureForAccess");
}

void VRCompositorCppPassalong::UnlockGLSharedTextureForAccess(vr::glSharedTextureHandle_t glSharedTextureHandle)
{
   LOG_ENTRY("CppPassalongUnlockGLSharedTextureForAccess");
   interfaces.cpp_interfaces()->compi->UnlockGLSharedTextureForAccess(glSharedTextureHandle);
   LOG_EXIT("CppPassalongUnlockGLSharedTextureForAccess");
}

uint32_t VRCompositorCppPassalong::GetVulkanInstanceExtensionsRequired(char * pchValue, uint32_t unBufferSize)
{
   LOG_ENTRY("CppPassalongGetVulkanInstanceExtensionsRequired");
   uint32_t rc = interfaces.cpp_interfaces()->compi->GetVulkanInstanceExtensionsRequired(pchValue,unBufferSize);
   LOG_EXIT_RC(rc, "CppPassalongGetVulkanInstanceExtensionsRequired");
}

uint32_t VRCompositorCppPassalong::GetVulkanDeviceExtensionsRequired(struct VkPhysicalDevice_T * pPhysicalDevice, char * pchValue, uint32_t unBufferSize)
{
   LOG_ENTRY("CppPassalongGetVulkanDeviceExtensionsRequired");
   uint32_t rc = interfaces.cpp_interfaces()->compi->GetVulkanDeviceExtensionsRequired(pPhysicalDevice,pchValue,unBufferSize);
   LOG_EXIT_RC(rc, "CppPassalongGetVulkanDeviceExtensionsRequired");
}

vr::EVROverlayError VROverlayCppPassalong::FindOverlay(const char * pchOverlayKey, vr::VROverlayHandle_t * pOverlayHandle)
{
   LOG_ENTRY("CppPassalongFindOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->FindOverlay(pchOverlayKey,pOverlayHandle);
   LOG_EXIT_RC(rc, "CppPassalongFindOverlay");
}

vr::EVROverlayError VROverlayCppPassalong::CreateOverlay(const char * pchOverlayKey, const char * pchOverlayFriendlyName, vr::VROverlayHandle_t * pOverlayHandle)
{
   LOG_ENTRY("CppPassalongCreateOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->CreateOverlay(pchOverlayKey,pchOverlayFriendlyName,pOverlayHandle);
   LOG_EXIT_RC(rc, "CppPassalongCreateOverlay");
}

vr::EVROverlayError VROverlayCppPassalong::DestroyOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("CppPassalongDestroyOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->DestroyOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "CppPassalongDestroyOverlay");
}

vr::EVROverlayError VROverlayCppPassalong::SetHighQualityOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("CppPassalongSetHighQualityOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetHighQualityOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "CppPassalongSetHighQualityOverlay");
}

vr::VROverlayHandle_t VROverlayCppPassalong::GetHighQualityOverlay()
{
   LOG_ENTRY("CppPassalongGetHighQualityOverlay");
   vr::VROverlayHandle_t rc = interfaces.cpp_interfaces()->ovi->GetHighQualityOverlay();
   LOG_EXIT_RC(rc, "CppPassalongGetHighQualityOverlay");
}

uint32_t VROverlayCppPassalong::GetOverlayKey(vr::VROverlayHandle_t ulOverlayHandle, char * pchValue, uint32_t unBufferSize, vr::EVROverlayError * pError)
{
   LOG_ENTRY("CppPassalongGetOverlayKey");
   uint32_t rc = interfaces.cpp_interfaces()->ovi->GetOverlayKey(ulOverlayHandle,pchValue,unBufferSize,pError);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayKey");
}

uint32_t VROverlayCppPassalong::GetOverlayName(vr::VROverlayHandle_t ulOverlayHandle, char * pchValue, uint32_t unBufferSize, vr::EVROverlayError * pError)
{
   LOG_ENTRY("CppPassalongGetOverlayName");
   uint32_t rc = interfaces.cpp_interfaces()->ovi->GetOverlayName(ulOverlayHandle,pchValue,unBufferSize,pError);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayName");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayImageData(vr::VROverlayHandle_t ulOverlayHandle, void * pvBuffer, uint32_t unBufferSize, uint32_t * punWidth, uint32_t * punHeight)
{
   LOG_ENTRY("CppPassalongGetOverlayImageData");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayImageData(ulOverlayHandle,pvBuffer,unBufferSize,punWidth,punHeight);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayImageData");
}

const char * VROverlayCppPassalong::GetOverlayErrorNameFromEnum(vr::EVROverlayError error)
{
   LOG_ENTRY("CppPassalongGetOverlayErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->ovi->GetOverlayErrorNameFromEnum(error);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayErrorNameFromEnum");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayRenderingPid(vr::VROverlayHandle_t ulOverlayHandle, uint32_t unPID)
{
   LOG_ENTRY("CppPassalongSetOverlayRenderingPid");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayRenderingPid(ulOverlayHandle,unPID);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayRenderingPid");
}

uint32_t VROverlayCppPassalong::GetOverlayRenderingPid(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("CppPassalongGetOverlayRenderingPid");
   uint32_t rc = interfaces.cpp_interfaces()->ovi->GetOverlayRenderingPid(ulOverlayHandle);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayRenderingPid");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayFlag(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayFlags eOverlayFlag, bool bEnabled)
{
   LOG_ENTRY("CppPassalongSetOverlayFlag");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayFlag(ulOverlayHandle,eOverlayFlag,bEnabled);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayFlag");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayFlag(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayFlags eOverlayFlag, bool * pbEnabled)
{
   LOG_ENTRY("CppPassalongGetOverlayFlag");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayFlag(ulOverlayHandle,eOverlayFlag,pbEnabled);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayFlag");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayColor(vr::VROverlayHandle_t ulOverlayHandle, float fRed, float fGreen, float fBlue)
{
   LOG_ENTRY("CppPassalongSetOverlayColor");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayColor(ulOverlayHandle,fRed,fGreen,fBlue);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayColor");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayColor(vr::VROverlayHandle_t ulOverlayHandle, float * pfRed, float * pfGreen, float * pfBlue)
{
   LOG_ENTRY("CppPassalongGetOverlayColor");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayColor(ulOverlayHandle,pfRed,pfGreen,pfBlue);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayColor");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayAlpha(vr::VROverlayHandle_t ulOverlayHandle, float fAlpha)
{
   LOG_ENTRY("CppPassalongSetOverlayAlpha");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayAlpha(ulOverlayHandle,fAlpha);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayAlpha");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayAlpha(vr::VROverlayHandle_t ulOverlayHandle, float * pfAlpha)
{
   LOG_ENTRY("CppPassalongGetOverlayAlpha");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayAlpha(ulOverlayHandle,pfAlpha);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayAlpha");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayTexelAspect(vr::VROverlayHandle_t ulOverlayHandle, float fTexelAspect)
{
   LOG_ENTRY("CppPassalongSetOverlayTexelAspect");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTexelAspect(ulOverlayHandle,fTexelAspect);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayTexelAspect");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayTexelAspect(vr::VROverlayHandle_t ulOverlayHandle, float * pfTexelAspect)
{
   LOG_ENTRY("CppPassalongGetOverlayTexelAspect");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTexelAspect(ulOverlayHandle,pfTexelAspect);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayTexelAspect");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlaySortOrder(vr::VROverlayHandle_t ulOverlayHandle, uint32_t unSortOrder)
{
   LOG_ENTRY("CppPassalongSetOverlaySortOrder");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlaySortOrder(ulOverlayHandle,unSortOrder);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlaySortOrder");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlaySortOrder(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * punSortOrder)
{
   LOG_ENTRY("CppPassalongGetOverlaySortOrder");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlaySortOrder(ulOverlayHandle,punSortOrder);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlaySortOrder");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayWidthInMeters(vr::VROverlayHandle_t ulOverlayHandle, float fWidthInMeters)
{
   LOG_ENTRY("CppPassalongSetOverlayWidthInMeters");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayWidthInMeters(ulOverlayHandle,fWidthInMeters);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayWidthInMeters");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayWidthInMeters(vr::VROverlayHandle_t ulOverlayHandle, float * pfWidthInMeters)
{
   LOG_ENTRY("CppPassalongGetOverlayWidthInMeters");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayWidthInMeters(ulOverlayHandle,pfWidthInMeters);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayWidthInMeters");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayAutoCurveDistanceRangeInMeters(vr::VROverlayHandle_t ulOverlayHandle, float fMinDistanceInMeters, float fMaxDistanceInMeters)
{
   LOG_ENTRY("CppPassalongSetOverlayAutoCurveDistanceRangeInMeters");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayAutoCurveDistanceRangeInMeters(ulOverlayHandle,fMinDistanceInMeters,fMaxDistanceInMeters);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayAutoCurveDistanceRangeInMeters");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayAutoCurveDistanceRangeInMeters(vr::VROverlayHandle_t ulOverlayHandle, float * pfMinDistanceInMeters, float * pfMaxDistanceInMeters)
{
   LOG_ENTRY("CppPassalongGetOverlayAutoCurveDistanceRangeInMeters");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayAutoCurveDistanceRangeInMeters(ulOverlayHandle,pfMinDistanceInMeters,pfMaxDistanceInMeters);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayAutoCurveDistanceRangeInMeters");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayTextureColorSpace(vr::VROverlayHandle_t ulOverlayHandle, vr::EColorSpace eTextureColorSpace)
{
   LOG_ENTRY("CppPassalongSetOverlayTextureColorSpace");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTextureColorSpace(ulOverlayHandle,eTextureColorSpace);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayTextureColorSpace");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayTextureColorSpace(vr::VROverlayHandle_t ulOverlayHandle, vr::EColorSpace * peTextureColorSpace)
{
   LOG_ENTRY("CppPassalongGetOverlayTextureColorSpace");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTextureColorSpace(ulOverlayHandle,peTextureColorSpace);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayTextureColorSpace");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayTextureBounds(vr::VROverlayHandle_t ulOverlayHandle, const struct vr::VRTextureBounds_t * pOverlayTextureBounds)
{
   LOG_ENTRY("CppPassalongSetOverlayTextureBounds");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTextureBounds(ulOverlayHandle,pOverlayTextureBounds);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayTextureBounds");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayTextureBounds(vr::VROverlayHandle_t ulOverlayHandle, struct vr::VRTextureBounds_t * pOverlayTextureBounds)
{
   LOG_ENTRY("CppPassalongGetOverlayTextureBounds");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTextureBounds(ulOverlayHandle,pOverlayTextureBounds);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayTextureBounds");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayTransformType(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayTransformType * peTransformType)
{
   LOG_ENTRY("CppPassalongGetOverlayTransformType");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTransformType(ulOverlayHandle,peTransformType);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayTransformType");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayTransformAbsolute(vr::VROverlayHandle_t ulOverlayHandle, vr::ETrackingUniverseOrigin eTrackingOrigin, const struct vr::HmdMatrix34_t * pmatTrackingOriginToOverlayTransform)
{
   LOG_ENTRY("CppPassalongSetOverlayTransformAbsolute");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTransformAbsolute(ulOverlayHandle,eTrackingOrigin,pmatTrackingOriginToOverlayTransform);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayTransformAbsolute");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayTransformAbsolute(vr::VROverlayHandle_t ulOverlayHandle, vr::ETrackingUniverseOrigin * peTrackingOrigin, struct vr::HmdMatrix34_t * pmatTrackingOriginToOverlayTransform)
{
   LOG_ENTRY("CppPassalongGetOverlayTransformAbsolute");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTransformAbsolute(ulOverlayHandle,peTrackingOrigin,pmatTrackingOriginToOverlayTransform);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayTransformAbsolute");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayTransformTrackedDeviceRelative(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t unTrackedDevice, const struct vr::HmdMatrix34_t * pmatTrackedDeviceToOverlayTransform)
{
   LOG_ENTRY("CppPassalongSetOverlayTransformTrackedDeviceRelative");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTransformTrackedDeviceRelative(ulOverlayHandle,unTrackedDevice,pmatTrackedDeviceToOverlayTransform);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayTransformTrackedDeviceRelative");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayTransformTrackedDeviceRelative(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t * punTrackedDevice, struct vr::HmdMatrix34_t * pmatTrackedDeviceToOverlayTransform)
{
   LOG_ENTRY("CppPassalongGetOverlayTransformTrackedDeviceRelative");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTransformTrackedDeviceRelative(ulOverlayHandle,punTrackedDevice,pmatTrackedDeviceToOverlayTransform);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayTransformTrackedDeviceRelative");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayTransformTrackedDeviceComponent(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t unDeviceIndex, const char * pchComponentName)
{
   LOG_ENTRY("CppPassalongSetOverlayTransformTrackedDeviceComponent");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTransformTrackedDeviceComponent(ulOverlayHandle,unDeviceIndex,pchComponentName);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayTransformTrackedDeviceComponent");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayTransformTrackedDeviceComponent(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t * punDeviceIndex, char * pchComponentName, uint32_t unComponentNameSize)
{
   LOG_ENTRY("CppPassalongGetOverlayTransformTrackedDeviceComponent");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTransformTrackedDeviceComponent(ulOverlayHandle,punDeviceIndex,pchComponentName,unComponentNameSize);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayTransformTrackedDeviceComponent");
}

vr::EVROverlayError VROverlayCppPassalong::ShowOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("CppPassalongShowOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->ShowOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "CppPassalongShowOverlay");
}

vr::EVROverlayError VROverlayCppPassalong::HideOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("CppPassalongHideOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->HideOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "CppPassalongHideOverlay");
}

bool VROverlayCppPassalong::IsOverlayVisible(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("CppPassalongIsOverlayVisible");
   bool rc = interfaces.cpp_interfaces()->ovi->IsOverlayVisible(ulOverlayHandle);
   LOG_EXIT_RC(rc, "CppPassalongIsOverlayVisible");
}

vr::EVROverlayError VROverlayCppPassalong::GetTransformForOverlayCoordinates(vr::VROverlayHandle_t ulOverlayHandle, vr::ETrackingUniverseOrigin eTrackingOrigin, struct vr::HmdVector2_t coordinatesInOverlay, struct vr::HmdMatrix34_t * pmatTransform)
{
   LOG_ENTRY("CppPassalongGetTransformForOverlayCoordinates");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetTransformForOverlayCoordinates(ulOverlayHandle,eTrackingOrigin,coordinatesInOverlay,pmatTransform);
   LOG_EXIT_RC(rc, "CppPassalongGetTransformForOverlayCoordinates");
}

bool VROverlayCppPassalong::PollNextOverlayEvent(vr::VROverlayHandle_t ulOverlayHandle, struct vr::VREvent_t * pEvent, uint32_t uncbVREvent)
{
   LOG_ENTRY("CppPassalongPollNextOverlayEvent");
   bool rc = interfaces.cpp_interfaces()->ovi->PollNextOverlayEvent(ulOverlayHandle,pEvent,uncbVREvent);
   LOG_EXIT_RC(rc, "CppPassalongPollNextOverlayEvent");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayInputMethod(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayInputMethod * peInputMethod)
{
   LOG_ENTRY("CppPassalongGetOverlayInputMethod");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayInputMethod(ulOverlayHandle,peInputMethod);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayInputMethod");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayInputMethod(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayInputMethod eInputMethod)
{
   LOG_ENTRY("CppPassalongSetOverlayInputMethod");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayInputMethod(ulOverlayHandle,eInputMethod);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayInputMethod");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayMouseScale(vr::VROverlayHandle_t ulOverlayHandle, struct vr::HmdVector2_t * pvecMouseScale)
{
   LOG_ENTRY("CppPassalongGetOverlayMouseScale");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayMouseScale(ulOverlayHandle,pvecMouseScale);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayMouseScale");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayMouseScale(vr::VROverlayHandle_t ulOverlayHandle, const struct vr::HmdVector2_t * pvecMouseScale)
{
   LOG_ENTRY("CppPassalongSetOverlayMouseScale");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayMouseScale(ulOverlayHandle,pvecMouseScale);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayMouseScale");
}

bool VROverlayCppPassalong::ComputeOverlayIntersection(vr::VROverlayHandle_t ulOverlayHandle, const struct vr::VROverlayIntersectionParams_t * pParams, struct vr::VROverlayIntersectionResults_t * pResults)
{
   LOG_ENTRY("CppPassalongComputeOverlayIntersection");
   bool rc = interfaces.cpp_interfaces()->ovi->ComputeOverlayIntersection(ulOverlayHandle,pParams,pResults);
   LOG_EXIT_RC(rc, "CppPassalongComputeOverlayIntersection");
}

bool VROverlayCppPassalong::HandleControllerOverlayInteractionAsMouse(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t unControllerDeviceIndex)
{
   LOG_ENTRY("CppPassalongHandleControllerOverlayInteractionAsMouse");
   bool rc = interfaces.cpp_interfaces()->ovi->HandleControllerOverlayInteractionAsMouse(ulOverlayHandle,unControllerDeviceIndex);
   LOG_EXIT_RC(rc, "CppPassalongHandleControllerOverlayInteractionAsMouse");
}

bool VROverlayCppPassalong::IsHoverTargetOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("CppPassalongIsHoverTargetOverlay");
   bool rc = interfaces.cpp_interfaces()->ovi->IsHoverTargetOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "CppPassalongIsHoverTargetOverlay");
}

vr::VROverlayHandle_t VROverlayCppPassalong::GetGamepadFocusOverlay()
{
   LOG_ENTRY("CppPassalongGetGamepadFocusOverlay");
   vr::VROverlayHandle_t rc = interfaces.cpp_interfaces()->ovi->GetGamepadFocusOverlay();
   LOG_EXIT_RC(rc, "CppPassalongGetGamepadFocusOverlay");
}

vr::EVROverlayError VROverlayCppPassalong::SetGamepadFocusOverlay(vr::VROverlayHandle_t ulNewFocusOverlay)
{
   LOG_ENTRY("CppPassalongSetGamepadFocusOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetGamepadFocusOverlay(ulNewFocusOverlay);
   LOG_EXIT_RC(rc, "CppPassalongSetGamepadFocusOverlay");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayNeighbor(vr::EOverlayDirection eDirection, vr::VROverlayHandle_t ulFrom, vr::VROverlayHandle_t ulTo)
{
   LOG_ENTRY("CppPassalongSetOverlayNeighbor");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayNeighbor(eDirection,ulFrom,ulTo);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayNeighbor");
}

vr::EVROverlayError VROverlayCppPassalong::MoveGamepadFocusToNeighbor(vr::EOverlayDirection eDirection, vr::VROverlayHandle_t ulFrom)
{
   LOG_ENTRY("CppPassalongMoveGamepadFocusToNeighbor");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->MoveGamepadFocusToNeighbor(eDirection,ulFrom);
   LOG_EXIT_RC(rc, "CppPassalongMoveGamepadFocusToNeighbor");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayTexture(vr::VROverlayHandle_t ulOverlayHandle, const struct vr::Texture_t * pTexture)
{
   LOG_ENTRY("CppPassalongSetOverlayTexture");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayTexture(ulOverlayHandle,pTexture);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayTexture");
}

vr::EVROverlayError VROverlayCppPassalong::ClearOverlayTexture(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("CppPassalongClearOverlayTexture");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->ClearOverlayTexture(ulOverlayHandle);
   LOG_EXIT_RC(rc, "CppPassalongClearOverlayTexture");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayRaw(vr::VROverlayHandle_t ulOverlayHandle, void * pvBuffer, uint32_t unWidth, uint32_t unHeight, uint32_t unDepth)
{
   LOG_ENTRY("CppPassalongSetOverlayRaw");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayRaw(ulOverlayHandle,pvBuffer,unWidth,unHeight,unDepth);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayRaw");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayFromFile(vr::VROverlayHandle_t ulOverlayHandle, const char * pchFilePath)
{
   LOG_ENTRY("CppPassalongSetOverlayFromFile");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayFromFile(ulOverlayHandle,pchFilePath);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayFromFile");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayTexture(vr::VROverlayHandle_t ulOverlayHandle, void ** pNativeTextureHandle, void * pNativeTextureRef, uint32_t * pWidth, uint32_t * pHeight, uint32_t * pNativeFormat, vr::ETextureType * pAPIType, vr::EColorSpace * pColorSpace, struct vr::VRTextureBounds_t * pTextureBounds)
{
   LOG_ENTRY("CppPassalongGetOverlayTexture");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTexture(ulOverlayHandle,pNativeTextureHandle,pNativeTextureRef,pWidth,pHeight,pNativeFormat,pAPIType,pColorSpace,pTextureBounds);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayTexture");
}

vr::EVROverlayError VROverlayCppPassalong::ReleaseNativeOverlayHandle(vr::VROverlayHandle_t ulOverlayHandle, void * pNativeTextureHandle)
{
   LOG_ENTRY("CppPassalongReleaseNativeOverlayHandle");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->ReleaseNativeOverlayHandle(ulOverlayHandle,pNativeTextureHandle);
   LOG_EXIT_RC(rc, "CppPassalongReleaseNativeOverlayHandle");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayTextureSize(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * pWidth, uint32_t * pHeight)
{
   LOG_ENTRY("CppPassalongGetOverlayTextureSize");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayTextureSize(ulOverlayHandle,pWidth,pHeight);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayTextureSize");
}

vr::EVROverlayError VROverlayCppPassalong::CreateDashboardOverlay(const char * pchOverlayKey, const char * pchOverlayFriendlyName, vr::VROverlayHandle_t * pMainHandle, vr::VROverlayHandle_t * pThumbnailHandle)
{
   LOG_ENTRY("CppPassalongCreateDashboardOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->CreateDashboardOverlay(pchOverlayKey,pchOverlayFriendlyName,pMainHandle,pThumbnailHandle);
   LOG_EXIT_RC(rc, "CppPassalongCreateDashboardOverlay");
}

bool VROverlayCppPassalong::IsDashboardVisible()
{
   LOG_ENTRY("CppPassalongIsDashboardVisible");
   bool rc = interfaces.cpp_interfaces()->ovi->IsDashboardVisible();
   LOG_EXIT_RC(rc, "CppPassalongIsDashboardVisible");
}

bool VROverlayCppPassalong::IsActiveDashboardOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
   LOG_ENTRY("CppPassalongIsActiveDashboardOverlay");
   bool rc = interfaces.cpp_interfaces()->ovi->IsActiveDashboardOverlay(ulOverlayHandle);
   LOG_EXIT_RC(rc, "CppPassalongIsActiveDashboardOverlay");
}

vr::EVROverlayError VROverlayCppPassalong::SetDashboardOverlaySceneProcess(vr::VROverlayHandle_t ulOverlayHandle, uint32_t unProcessId)
{
   LOG_ENTRY("CppPassalongSetDashboardOverlaySceneProcess");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetDashboardOverlaySceneProcess(ulOverlayHandle,unProcessId);
   LOG_EXIT_RC(rc, "CppPassalongSetDashboardOverlaySceneProcess");
}

vr::EVROverlayError VROverlayCppPassalong::GetDashboardOverlaySceneProcess(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * punProcessId)
{
   LOG_ENTRY("CppPassalongGetDashboardOverlaySceneProcess");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetDashboardOverlaySceneProcess(ulOverlayHandle,punProcessId);
   LOG_EXIT_RC(rc, "CppPassalongGetDashboardOverlaySceneProcess");
}

void VROverlayCppPassalong::ShowDashboard(const char * pchOverlayToShow)
{
   LOG_ENTRY("CppPassalongShowDashboard");
   interfaces.cpp_interfaces()->ovi->ShowDashboard(pchOverlayToShow);
   LOG_EXIT("CppPassalongShowDashboard");
}

vr::TrackedDeviceIndex_t VROverlayCppPassalong::GetPrimaryDashboardDevice()
{
   LOG_ENTRY("CppPassalongGetPrimaryDashboardDevice");
   vr::TrackedDeviceIndex_t rc = interfaces.cpp_interfaces()->ovi->GetPrimaryDashboardDevice();
   LOG_EXIT_RC(rc, "CppPassalongGetPrimaryDashboardDevice");
}

vr::EVROverlayError VROverlayCppPassalong::ShowKeyboard(vr::EGamepadTextInputMode eInputMode, vr::EGamepadTextInputLineMode eLineInputMode, const char * pchDescription, uint32_t unCharMax, const char * pchExistingText, bool bUseMinimalMode, uint64_t uUserValue)
{
   LOG_ENTRY("CppPassalongShowKeyboard");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->ShowKeyboard(eInputMode,eLineInputMode,pchDescription,unCharMax,pchExistingText,bUseMinimalMode,uUserValue);
   LOG_EXIT_RC(rc, "CppPassalongShowKeyboard");
}

vr::EVROverlayError VROverlayCppPassalong::ShowKeyboardForOverlay(vr::VROverlayHandle_t ulOverlayHandle, vr::EGamepadTextInputMode eInputMode, vr::EGamepadTextInputLineMode eLineInputMode, const char * pchDescription, uint32_t unCharMax, const char * pchExistingText, bool bUseMinimalMode, uint64_t uUserValue)
{
   LOG_ENTRY("CppPassalongShowKeyboardForOverlay");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->ShowKeyboardForOverlay(ulOverlayHandle,eInputMode,eLineInputMode,pchDescription,unCharMax,pchExistingText,bUseMinimalMode,uUserValue);
   LOG_EXIT_RC(rc, "CppPassalongShowKeyboardForOverlay");
}

uint32_t VROverlayCppPassalong::GetKeyboardText(char * pchText, uint32_t cchText)
{
   LOG_ENTRY("CppPassalongGetKeyboardText");
   uint32_t rc = interfaces.cpp_interfaces()->ovi->GetKeyboardText(pchText,cchText);
   LOG_EXIT_RC(rc, "CppPassalongGetKeyboardText");
}

void VROverlayCppPassalong::HideKeyboard()
{
   LOG_ENTRY("CppPassalongHideKeyboard");
   interfaces.cpp_interfaces()->ovi->HideKeyboard();
   LOG_EXIT("CppPassalongHideKeyboard");
}

void VROverlayCppPassalong::SetKeyboardTransformAbsolute(vr::ETrackingUniverseOrigin eTrackingOrigin, const struct vr::HmdMatrix34_t * pmatTrackingOriginToKeyboardTransform)
{
   LOG_ENTRY("CppPassalongSetKeyboardTransformAbsolute");
   interfaces.cpp_interfaces()->ovi->SetKeyboardTransformAbsolute(eTrackingOrigin,pmatTrackingOriginToKeyboardTransform);
   LOG_EXIT("CppPassalongSetKeyboardTransformAbsolute");
}

void VROverlayCppPassalong::SetKeyboardPositionForOverlay(vr::VROverlayHandle_t ulOverlayHandle, struct vr::HmdRect2_t avoidRect)
{
   LOG_ENTRY("CppPassalongSetKeyboardPositionForOverlay");
   interfaces.cpp_interfaces()->ovi->SetKeyboardPositionForOverlay(ulOverlayHandle,avoidRect);
   LOG_EXIT("CppPassalongSetKeyboardPositionForOverlay");
}

vr::EVROverlayError VROverlayCppPassalong::SetOverlayIntersectionMask(vr::VROverlayHandle_t ulOverlayHandle, struct vr::VROverlayIntersectionMaskPrimitive_t * pMaskPrimitives, uint32_t unNumMaskPrimitives, uint32_t unPrimitiveSize)
{
   LOG_ENTRY("CppPassalongSetOverlayIntersectionMask");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->SetOverlayIntersectionMask(ulOverlayHandle,pMaskPrimitives,unNumMaskPrimitives,unPrimitiveSize);
   LOG_EXIT_RC(rc, "CppPassalongSetOverlayIntersectionMask");
}

vr::EVROverlayError VROverlayCppPassalong::GetOverlayFlags(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * pFlags)
{
   LOG_ENTRY("CppPassalongGetOverlayFlags");
   vr::EVROverlayError rc = interfaces.cpp_interfaces()->ovi->GetOverlayFlags(ulOverlayHandle,pFlags);
   LOG_EXIT_RC(rc, "CppPassalongGetOverlayFlags");
}

vr::VRMessageOverlayResponse VROverlayCppPassalong::ShowMessageOverlay(const char * pchText, const char * pchCaption, const char * pchButton0Text, const char * pchButton1Text, const char * pchButton2Text, const char * pchButton3Text)
{
   LOG_ENTRY("CppPassalongShowMessageOverlay");
   vr::VRMessageOverlayResponse rc = interfaces.cpp_interfaces()->ovi->ShowMessageOverlay(pchText,pchCaption,pchButton0Text,pchButton1Text,pchButton2Text,pchButton3Text);
   LOG_EXIT_RC(rc, "CppPassalongShowMessageOverlay");
}

vr::EVRRenderModelError VRRenderModelsCppPassalong::LoadRenderModel_Async(const char * pchRenderModelName, struct vr::RenderModel_t ** ppRenderModel)
{
   LOG_ENTRY("CppPassalongLoadRenderModel_Async");
   vr::EVRRenderModelError rc = interfaces.cpp_interfaces()->remi->LoadRenderModel_Async(pchRenderModelName,ppRenderModel);
   LOG_EXIT_RC(rc, "CppPassalongLoadRenderModel_Async");
}

void VRRenderModelsCppPassalong::FreeRenderModel(struct vr::RenderModel_t * pRenderModel)
{
   LOG_ENTRY("CppPassalongFreeRenderModel");
   interfaces.cpp_interfaces()->remi->FreeRenderModel(pRenderModel);
   LOG_EXIT("CppPassalongFreeRenderModel");
}

vr::EVRRenderModelError VRRenderModelsCppPassalong::LoadTexture_Async(vr::TextureID_t textureId, struct vr::RenderModel_TextureMap_t ** ppTexture)
{
   LOG_ENTRY("CppPassalongLoadTexture_Async");
   vr::EVRRenderModelError rc = interfaces.cpp_interfaces()->remi->LoadTexture_Async(textureId,ppTexture);
   LOG_EXIT_RC(rc, "CppPassalongLoadTexture_Async");
}

void VRRenderModelsCppPassalong::FreeTexture(struct vr::RenderModel_TextureMap_t * pTexture)
{
   LOG_ENTRY("CppPassalongFreeTexture");
   interfaces.cpp_interfaces()->remi->FreeTexture(pTexture);
   LOG_EXIT("CppPassalongFreeTexture");
}

vr::EVRRenderModelError VRRenderModelsCppPassalong::LoadTextureD3D11_Async(vr::TextureID_t textureId, void * pD3D11Device, void ** ppD3D11Texture2D)
{
   LOG_ENTRY("CppPassalongLoadTextureD3D11_Async");
   vr::EVRRenderModelError rc = interfaces.cpp_interfaces()->remi->LoadTextureD3D11_Async(textureId,pD3D11Device,ppD3D11Texture2D);
   LOG_EXIT_RC(rc, "CppPassalongLoadTextureD3D11_Async");
}

vr::EVRRenderModelError VRRenderModelsCppPassalong::LoadIntoTextureD3D11_Async(vr::TextureID_t textureId, void * pDstTexture)
{
   LOG_ENTRY("CppPassalongLoadIntoTextureD3D11_Async");
   vr::EVRRenderModelError rc = interfaces.cpp_interfaces()->remi->LoadIntoTextureD3D11_Async(textureId,pDstTexture);
   LOG_EXIT_RC(rc, "CppPassalongLoadIntoTextureD3D11_Async");
}

void VRRenderModelsCppPassalong::FreeTextureD3D11(void * pD3D11Texture2D)
{
   LOG_ENTRY("CppPassalongFreeTextureD3D11");
   interfaces.cpp_interfaces()->remi->FreeTextureD3D11(pD3D11Texture2D);
   LOG_EXIT("CppPassalongFreeTextureD3D11");
}

uint32_t VRRenderModelsCppPassalong::GetRenderModelName(uint32_t unRenderModelIndex, char * pchRenderModelName, uint32_t unRenderModelNameLen)
{
   LOG_ENTRY("CppPassalongGetRenderModelName");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetRenderModelName(unRenderModelIndex,pchRenderModelName,unRenderModelNameLen);
   LOG_EXIT_RC(rc, "CppPassalongGetRenderModelName");
}

uint32_t VRRenderModelsCppPassalong::GetRenderModelCount()
{
   LOG_ENTRY("CppPassalongGetRenderModelCount");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetRenderModelCount();
   LOG_EXIT_RC(rc, "CppPassalongGetRenderModelCount");
}

uint32_t VRRenderModelsCppPassalong::GetComponentCount(const char * pchRenderModelName)
{
   LOG_ENTRY("CppPassalongGetComponentCount");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetComponentCount(pchRenderModelName);
   LOG_EXIT_RC(rc, "CppPassalongGetComponentCount");
}

uint32_t VRRenderModelsCppPassalong::GetComponentName(const char * pchRenderModelName, uint32_t unComponentIndex, char * pchComponentName, uint32_t unComponentNameLen)
{
   LOG_ENTRY("CppPassalongGetComponentName");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetComponentName(pchRenderModelName,unComponentIndex,pchComponentName,unComponentNameLen);
   LOG_EXIT_RC(rc, "CppPassalongGetComponentName");
}

uint64_t VRRenderModelsCppPassalong::GetComponentButtonMask(const char * pchRenderModelName, const char * pchComponentName)
{
   LOG_ENTRY("CppPassalongGetComponentButtonMask");
   uint64_t rc = interfaces.cpp_interfaces()->remi->GetComponentButtonMask(pchRenderModelName,pchComponentName);
   LOG_EXIT_RC(rc, "CppPassalongGetComponentButtonMask");
}

uint32_t VRRenderModelsCppPassalong::GetComponentRenderModelName(const char * pchRenderModelName, const char * pchComponentName, char * pchComponentRenderModelName, uint32_t unComponentRenderModelNameLen)
{
   LOG_ENTRY("CppPassalongGetComponentRenderModelName");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetComponentRenderModelName(pchRenderModelName,pchComponentName,pchComponentRenderModelName,unComponentRenderModelNameLen);
   LOG_EXIT_RC(rc, "CppPassalongGetComponentRenderModelName");
}

bool VRRenderModelsCppPassalong::GetComponentState(const char * pchRenderModelName, const char * pchComponentName, const vr::VRControllerState_t * pControllerState, const struct vr::RenderModel_ControllerMode_State_t * pState, struct vr::RenderModel_ComponentState_t * pComponentState)
{
   LOG_ENTRY("CppPassalongGetComponentState");
   bool rc = interfaces.cpp_interfaces()->remi->GetComponentState(pchRenderModelName,pchComponentName,pControllerState,pState,pComponentState);
   LOG_EXIT_RC(rc, "CppPassalongGetComponentState");
}

bool VRRenderModelsCppPassalong::RenderModelHasComponent(const char * pchRenderModelName, const char * pchComponentName)
{
   LOG_ENTRY("CppPassalongRenderModelHasComponent");
   bool rc = interfaces.cpp_interfaces()->remi->RenderModelHasComponent(pchRenderModelName,pchComponentName);
   LOG_EXIT_RC(rc, "CppPassalongRenderModelHasComponent");
}

uint32_t VRRenderModelsCppPassalong::GetRenderModelThumbnailURL(const char * pchRenderModelName, char * pchThumbnailURL, uint32_t unThumbnailURLLen, vr::EVRRenderModelError * peError)
{
   LOG_ENTRY("CppPassalongGetRenderModelThumbnailURL");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetRenderModelThumbnailURL(pchRenderModelName,pchThumbnailURL,unThumbnailURLLen,peError);
   LOG_EXIT_RC(rc, "CppPassalongGetRenderModelThumbnailURL");
}

uint32_t VRRenderModelsCppPassalong::GetRenderModelOriginalPath(const char * pchRenderModelName, char * pchOriginalPath, uint32_t unOriginalPathLen, vr::EVRRenderModelError * peError)
{
   LOG_ENTRY("CppPassalongGetRenderModelOriginalPath");
   uint32_t rc = interfaces.cpp_interfaces()->remi->GetRenderModelOriginalPath(pchRenderModelName,pchOriginalPath,unOriginalPathLen,peError);
   LOG_EXIT_RC(rc, "CppPassalongGetRenderModelOriginalPath");
}

const char * VRRenderModelsCppPassalong::GetRenderModelErrorNameFromEnum(vr::EVRRenderModelError error)
{
   LOG_ENTRY("CppPassalongGetRenderModelErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->remi->GetRenderModelErrorNameFromEnum(error);
   LOG_EXIT_RC(rc, "CppPassalongGetRenderModelErrorNameFromEnum");
}

vr::EVRNotificationError VRNotificationsCppPassalong::CreateNotification(vr::VROverlayHandle_t ulOverlayHandle, uint64_t ulUserValue, vr::EVRNotificationType type, const char * pchText, vr::EVRNotificationStyle style, const struct vr::NotificationBitmap_t * pImage, vr::VRNotificationId * pNotificationId)
{
   LOG_ENTRY("CppPassalongCreateNotification");
   vr::EVRNotificationError rc = interfaces.cpp_interfaces()->noti->CreateNotification(ulOverlayHandle,ulUserValue,type,pchText,style,pImage,pNotificationId);
   LOG_EXIT_RC(rc, "CppPassalongCreateNotification");
}

vr::EVRNotificationError VRNotificationsCppPassalong::RemoveNotification(vr::VRNotificationId notificationId)
{
   LOG_ENTRY("CppPassalongRemoveNotification");
   vr::EVRNotificationError rc = interfaces.cpp_interfaces()->noti->RemoveNotification(notificationId);
   LOG_EXIT_RC(rc, "CppPassalongRemoveNotification");
}

const char * VRSettingsCppPassalong::GetSettingsErrorNameFromEnum(vr::EVRSettingsError eError)
{
   LOG_ENTRY("CppPassalongGetSettingsErrorNameFromEnum");
   const char * rc = interfaces.cpp_interfaces()->seti->GetSettingsErrorNameFromEnum(eError);
   LOG_EXIT_RC(rc, "CppPassalongGetSettingsErrorNameFromEnum");
}

bool VRSettingsCppPassalong::Sync(bool bForce, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("CppPassalongSync");
   bool rc = interfaces.cpp_interfaces()->seti->Sync(bForce,peError);
   LOG_EXIT_RC(rc, "CppPassalongSync");
}

void VRSettingsCppPassalong::SetBool(const char * pchSection, const char * pchSettingsKey, bool bValue, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("CppPassalongSetBool");
   interfaces.cpp_interfaces()->seti->SetBool(pchSection,pchSettingsKey,bValue,peError);
   LOG_EXIT("CppPassalongSetBool");
}

void VRSettingsCppPassalong::SetInt32(const char * pchSection, const char * pchSettingsKey, int32_t nValue, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("CppPassalongSetInt32");
   interfaces.cpp_interfaces()->seti->SetInt32(pchSection,pchSettingsKey,nValue,peError);
   LOG_EXIT("CppPassalongSetInt32");
}

void VRSettingsCppPassalong::SetFloat(const char * pchSection, const char * pchSettingsKey, float flValue, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("CppPassalongSetFloat");
   interfaces.cpp_interfaces()->seti->SetFloat(pchSection,pchSettingsKey,flValue,peError);
   LOG_EXIT("CppPassalongSetFloat");
}

void VRSettingsCppPassalong::SetString(const char * pchSection, const char * pchSettingsKey, const char * pchValue, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("CppPassalongSetString");
   interfaces.cpp_interfaces()->seti->SetString(pchSection,pchSettingsKey,pchValue,peError);
   LOG_EXIT("CppPassalongSetString");
}

bool VRSettingsCppPassalong::GetBool(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("CppPassalongGetBool");
   bool rc = interfaces.cpp_interfaces()->seti->GetBool(pchSection,pchSettingsKey,peError);
   LOG_EXIT_RC(rc, "CppPassalongGetBool");
}

int32_t VRSettingsCppPassalong::GetInt32(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("CppPassalongGetInt32");
   int32_t rc = interfaces.cpp_interfaces()->seti->GetInt32(pchSection,pchSettingsKey,peError);
   LOG_EXIT_RC(rc, "CppPassalongGetInt32");
}

float VRSettingsCppPassalong::GetFloat(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("CppPassalongGetFloat");
   float rc = interfaces.cpp_interfaces()->seti->GetFloat(pchSection,pchSettingsKey,peError);
   LOG_EXIT_RC(rc, "CppPassalongGetFloat");
}

void VRSettingsCppPassalong::GetString(const char * pchSection, const char * pchSettingsKey, char * pchValue, uint32_t unValueLen, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("CppPassalongGetString");
   interfaces.cpp_interfaces()->seti->GetString(pchSection,pchSettingsKey,pchValue,unValueLen,peError);
   LOG_EXIT("CppPassalongGetString");
}

void VRSettingsCppPassalong::RemoveSection(const char * pchSection, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("CppPassalongRemoveSection");
   interfaces.cpp_interfaces()->seti->RemoveSection(pchSection,peError);
   LOG_EXIT("CppPassalongRemoveSection");
}

void VRSettingsCppPassalong::RemoveKeyInSection(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * peError)
{
   LOG_ENTRY("CppPassalongRemoveKeyInSection");
   interfaces.cpp_interfaces()->seti->RemoveKeyInSection(pchSection,pchSettingsKey,peError);
   LOG_EXIT("CppPassalongRemoveKeyInSection");
}

vr::EVRScreenshotError VRScreenshotsCppPassalong::RequestScreenshot(vr::ScreenshotHandle_t * pOutScreenshotHandle, vr::EVRScreenshotType type, const char * pchPreviewFilename, const char * pchVRFilename)
{
   LOG_ENTRY("CppPassalongRequestScreenshot");
   vr::EVRScreenshotError rc = interfaces.cpp_interfaces()->screeni->RequestScreenshot(pOutScreenshotHandle,type,pchPreviewFilename,pchVRFilename);
   LOG_EXIT_RC(rc, "CppPassalongRequestScreenshot");
}

vr::EVRScreenshotError VRScreenshotsCppPassalong::HookScreenshot(const vr::EVRScreenshotType * pSupportedTypes, int numTypes)
{
   LOG_ENTRY("CppPassalongHookScreenshot");
   vr::EVRScreenshotError rc = interfaces.cpp_interfaces()->screeni->HookScreenshot(pSupportedTypes,numTypes);
   LOG_EXIT_RC(rc, "CppPassalongHookScreenshot");
}

vr::EVRScreenshotType VRScreenshotsCppPassalong::GetScreenshotPropertyType(vr::ScreenshotHandle_t screenshotHandle, vr::EVRScreenshotError * pError)
{
   LOG_ENTRY("CppPassalongGetScreenshotPropertyType");
   vr::EVRScreenshotType rc = interfaces.cpp_interfaces()->screeni->GetScreenshotPropertyType(screenshotHandle,pError);
   LOG_EXIT_RC(rc, "CppPassalongGetScreenshotPropertyType");
}

uint32_t VRScreenshotsCppPassalong::GetScreenshotPropertyFilename(vr::ScreenshotHandle_t screenshotHandle, vr::EVRScreenshotPropertyFilenames filenameType, char * pchFilename, uint32_t cchFilename, vr::EVRScreenshotError * pError)
{
   LOG_ENTRY("CppPassalongGetScreenshotPropertyFilename");
   uint32_t rc = interfaces.cpp_interfaces()->screeni->GetScreenshotPropertyFilename(screenshotHandle,filenameType,pchFilename,cchFilename,pError);
   LOG_EXIT_RC(rc, "CppPassalongGetScreenshotPropertyFilename");
}

vr::EVRScreenshotError VRScreenshotsCppPassalong::UpdateScreenshotProgress(vr::ScreenshotHandle_t screenshotHandle, float flProgress)
{
   LOG_ENTRY("CppPassalongUpdateScreenshotProgress");
   vr::EVRScreenshotError rc = interfaces.cpp_interfaces()->screeni->UpdateScreenshotProgress(screenshotHandle,flProgress);
   LOG_EXIT_RC(rc, "CppPassalongUpdateScreenshotProgress");
}

vr::EVRScreenshotError VRScreenshotsCppPassalong::TakeStereoScreenshot(vr::ScreenshotHandle_t * pOutScreenshotHandle, const char * pchPreviewFilename, const char * pchVRFilename)
{
   LOG_ENTRY("CppPassalongTakeStereoScreenshot");
   vr::EVRScreenshotError rc = interfaces.cpp_interfaces()->screeni->TakeStereoScreenshot(pOutScreenshotHandle,pchPreviewFilename,pchVRFilename);
   LOG_EXIT_RC(rc, "CppPassalongTakeStereoScreenshot");
}

vr::EVRScreenshotError VRScreenshotsCppPassalong::SubmitScreenshot(vr::ScreenshotHandle_t screenshotHandle, vr::EVRScreenshotType type, const char * pchSourcePreviewFilename, const char * pchSourceVRFilename)
{
   LOG_ENTRY("CppPassalongSubmitScreenshot");
   vr::EVRScreenshotError rc = interfaces.cpp_interfaces()->screeni->SubmitScreenshot(screenshotHandle,type,pchSourcePreviewFilename,pchSourceVRFilename);
   LOG_EXIT_RC(rc, "CppPassalongSubmitScreenshot");
}

uint32_t VRResourcesCppPassalong::LoadSharedResource(const char * pchResourceName, char * pchBuffer, uint32_t unBufferLen)
{
   LOG_ENTRY("CppPassalongLoadSharedResource");
   uint32_t rc = interfaces.cpp_interfaces()->resi->LoadSharedResource(pchResourceName,pchBuffer,unBufferLen);
   LOG_EXIT_RC(rc, "CppPassalongLoadSharedResource");
}

uint32_t VRResourcesCppPassalong::GetResourceFullPath(const char * pchResourceName, const char * pchResourceTypeDirectory, char * pchPathBuffer, uint32_t unBufferLen)
{
   LOG_ENTRY("CppPassalongGetResourceFullPath");
   uint32_t rc = interfaces.cpp_interfaces()->resi->GetResourceFullPath(pchResourceName,pchResourceTypeDirectory,pchPathBuffer,unBufferLen);
   LOG_EXIT_RC(rc, "CppPassalongGetResourceFullPath");
}

