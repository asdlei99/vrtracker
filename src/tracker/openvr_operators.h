#pragma once

#include <openvr.h>
#include <memory>
#include <string>


inline std::string to_string(bool b)
{
	if (b)
	{
		return std::string("true");
	}
	else
	{
		return std::string("false");
	}
};

inline std::string to_string(unsigned short s)
{
	return std::to_string((unsigned int)s);
}

struct AbsoluteTransform
{
	vr::ETrackingUniverseOrigin tracking_origin;
	vr::HmdMatrix34_t origin2overlaytransform;
};

inline std::string to_string(const AbsoluteTransform& t)
{
	std::string ret; 
	ret += "tracking_origin" + openvr_string::to_string(t.tracking_origin);
	ret += "origin2overlaytransform" + openvr_string::to_string(t.origin2overlaytransform);
	return ret;
}

struct TrackedDeviceRelativeTransform
{
	vr::TrackedDeviceIndex_t tracked_device;
	vr::HmdMatrix34_t device2overlaytransform;
};

inline std::string to_string(const TrackedDeviceRelativeTransform& t)
{
	std::string ret;
	ret += "tracked_device" + std::to_string(t.tracked_device);
	ret += "device2overlaytransform" + openvr_string::to_string(t.device2overlaytransform);
	return ret;
}

struct RGBColor
{
	float r;
	float g;
	float b;
};

inline std::string to_string(const RGBColor &vp)
{
	std::string s;
	s += std::to_string(vp.r) + "," + std::to_string(vp.g) + "," + std::to_string(vp.b);
	return s;
}

struct FloatRange
{
	float min;
	float max;
};

inline std::string to_string(const FloatRange &vp)
{
	std::string s;
	s += "min: " + std::to_string(vp.min);
	s += "max: " + std::to_string(vp.max);
	return s;
}

struct Uint32Size
{
	uint32_t width;
	uint32_t height;
};

inline std::string to_string(Uint32Size u)
{
	return std::to_string(u.width) + "," + std::to_string(u.height);
}

struct WindowBounds_t
{
	int32_t i[2];
	uint32_t u[2];
};

inline std::string to_string(const WindowBounds_t &wb)
{
	std::string s;
	s += "x:" + std::to_string(wb.i[0]) + " y:" + std::to_string(wb.i[1]);
	s += " width: " + std::to_string(wb.u[0]) + " height:" + std::to_string(wb.u[1]);
	return s;
}

struct ViewPort_t
{
	uint32_t u[4];
};

inline std::string to_string(const ViewPort_t &vp)
{
	std::string s;
	s += "x:" + std::to_string(vp.u[0]) + " y:" + std::to_string(vp.u[1]);
	s += " width: " + std::to_string(vp.u[2]) + " height:" + std::to_string(vp.u[3]);
	return s;
}

struct CameraFrameSize_t
{
	uint32_t width;
	uint32_t height;
	uint32_t size;
};

inline std::string to_string(const CameraFrameSize_t &vp)
{
	std::string s;
	s += "width:" + std::to_string(vp.width) + " height:" + std::to_string(vp.height);
	s += " size: " + std::to_string(vp.size);
	return s;
}

struct CameraFrameIntrinsics_t
{
	vr::HmdVector2_t focal_length;
	vr::HmdVector2_t center;
};

inline std::string to_string(const CameraFrameIntrinsics_t &vp)
{
	std::string s;
	s += "focal_length:" + std::to_string(vp.focal_length.v[0]) + "," + std::to_string(vp.focal_length.v[1]);
	s += " center:" + std::to_string(vp.center.v[0]) + "," + std::to_string(vp.center.v[1]);
	
	return s;
}

struct VideoStreamTextureSize_t
{
	vr::VRTextureBounds_t texture_bounds;
	uint32_t width;
	uint32_t height;
};

inline std::string to_string(const VideoStreamTextureSize_t &vp)
{
	std::string s;
	s += "texture_bounds: TODO";
	return s;
}

#define MEMCMP_OPERATOR_EQ(my_typename)\
inline bool operator == (const my_typename &lhs, const my_typename &rhs)\
{\
	return (memcmp(&lhs, &rhs, sizeof(lhs)) == 0);\
}\
inline bool operator != (const my_typename &lhs, const my_typename &rhs)\
{\
	return !(lhs == rhs);\
}

MEMCMP_OPERATOR_EQ(TrackedDeviceRelativeTransform)
MEMCMP_OPERATOR_EQ(AbsoluteTransform)
MEMCMP_OPERATOR_EQ(FloatRange)
MEMCMP_OPERATOR_EQ(RGBColor)
MEMCMP_OPERATOR_EQ(VideoStreamTextureSize_t)
MEMCMP_OPERATOR_EQ(CameraFrameIntrinsics_t)
MEMCMP_OPERATOR_EQ(CameraFrameSize_t)
MEMCMP_OPERATOR_EQ(WindowBounds_t)
MEMCMP_OPERATOR_EQ(ViewPort_t)
MEMCMP_OPERATOR_EQ(Uint32Size)
MEMCMP_OPERATOR_EQ(vr::HmdVector2_t)
MEMCMP_OPERATOR_EQ(vr::HmdVector4_t)
MEMCMP_OPERATOR_EQ(vr::DistortionCoordinates_t)
MEMCMP_OPERATOR_EQ(vr::HmdMatrix34_t)
MEMCMP_OPERATOR_EQ(vr::HmdMatrix44_t)
MEMCMP_OPERATOR_EQ(vr::VRControllerState_t)
MEMCMP_OPERATOR_EQ(vr::HmdQuad_t)
MEMCMP_OPERATOR_EQ(vr::Compositor_FrameTiming)
MEMCMP_OPERATOR_EQ(vr::Compositor_CumulativeStats)
MEMCMP_OPERATOR_EQ(vr::HmdColor_t)
MEMCMP_OPERATOR_EQ(vr::RenderModel_ComponentState_t)
MEMCMP_OPERATOR_EQ(vr::RenderModel_Vertex_t)
MEMCMP_OPERATOR_EQ(vr::VRTextureBounds_t)


inline bool operator == (const vr::TrackedDevicePose_t &lhs, const vr::TrackedDevicePose_t &rhs)
{
	if (lhs.bPoseIsValid == false && rhs.bPoseIsValid == false)
		return true;
	if (lhs.bPoseIsValid != false && rhs.bPoseIsValid)
		return false;
	// otherwise both poses are valid - go big:
	return (memcmp(&lhs, &rhs, sizeof(lhs)) == 0);
}
inline bool operator != (const vr::TrackedDevicePose_t &lhs, const vr::TrackedDevicePose_t &rhs)
{
	return !(lhs == rhs);
}

inline bool operator == (const vr::HiddenAreaMesh_t &lhs, const vr::HiddenAreaMesh_t &rhs)
{
	if (lhs.unTriangleCount != rhs.unTriangleCount)
		return false;
	if (lhs.pVertexData == rhs.pVertexData)
		return true;
	else
	{
		return (memcmp(lhs.pVertexData, rhs.pVertexData, lhs.unTriangleCount * 3 * sizeof(lhs.pVertexData[0])) == 0);
	}
}
inline bool operator != (const vr::HiddenAreaMesh_t &lhs, const vr::HiddenAreaMesh_t &rhs)
{
	return !(lhs == rhs);
}
