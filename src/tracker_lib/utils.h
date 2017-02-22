#pragma once

#include "shared/Matrices.h"
#include <GL/gl3w.h>
#include <openvr_string.h>

namespace openvr_string
{
	uint32_t GetAsString(const Matrix4 &e, VR_OUT_STRING() char *s, uint32_t n);
};

struct FramebufferDesc
{
	GLuint m_nDepthBufferId;
	GLuint m_nRenderTextureId;
	GLuint m_nRenderFramebufferId;
	GLuint m_nResolveTextureId;
	GLuint m_nResolveFramebufferId;
};

bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc);
Matrix4 MakeMatrix4(vr::HmdMatrix44_t &mat);
Matrix4 MakeMatrix4(const vr::HmdMatrix34_t &matPose);
std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL);
void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam);
