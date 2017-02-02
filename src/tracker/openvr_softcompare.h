#pragma once
#include <openvr.h>

inline bool softcompare_floats(float a, float b, float epsilon)
{
	float delta;
	if (a > b)
		delta = a - b;
	else
		delta = b - a;

	if (delta > -epsilon && delta < epsilon)
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool softcompare_is_similar(
	const vr::HmdColor_t &a,
	const vr::HmdColor_t &b,

	float epsilon = 0.0001f)
{
	bool similar = true;
	if (fabs(a.r - b.r) > epsilon ||
		fabs(a.g - b.g) > epsilon ||
		fabs(a.b - b.b) > epsilon ||
		fabs(a.a - b.a) > epsilon)
	{
		similar = false;
	}
	return similar;
}

inline bool softcompare_is_similar(
	const vr::HmdColor_t *a,
	const vr::HmdColor_t *b,
	int count,
	float epsilon = 0.0001f)
{
	bool similar = true;
	for (int i = 0; i < count; i++)
	{
		if (!softcompare_is_similar(a[i], b[i], epsilon))
		{ 
			similar = false;
			break;
		}
	}
	return similar;
}

inline bool softcompare_is_similar(
	vr::HmdMatrix34_t &a,
	vr::HmdMatrix34_t &b,
	float epsilon = 0.0001f)
{
	bool similar = true;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (fabs(a.m[i][j] - b.m[i][j]) > epsilon)
			{
				similar = false;
				goto done;
			}
		}
	}
done:
	return similar;
}


// returns true if similar
inline bool softcompare_poses(
	const vr::TrackedDevicePose_t &a,
	const vr::TrackedDevicePose_t &b,
	float epsilon = 0.0001f)
{
	bool similar = true;
	if (a.bPoseIsValid != b.bPoseIsValid)
	{
		similar = false;
	}
	if (a.bDeviceIsConnected != b.bDeviceIsConnected)
	{
		similar = false;
	}
	if (a.eTrackingResult != b.eTrackingResult)
	{
		similar = false;
	}

	if (a.bPoseIsValid != b.bPoseIsValid)
	{
		for (int i = 0; i < 3 && similar; i++)
		{
			similar = softcompare_floats(a.vAngularVelocity.v[i], b.vAngularVelocity.v[i], epsilon);
			similar = softcompare_floats(a.vVelocity.v[i], b.vVelocity.v[i], epsilon);
			for (int j = 0; j < 4 && similar; j++)
			{
				similar = softcompare_floats(a.mDeviceToAbsoluteTracking.m[i][j],
					b.mDeviceToAbsoluteTracking.m[i][j], epsilon);
			}
		}
	}
	return similar;
}

inline bool softcompare_pose_arrays(
	const vr::TrackedDevicePose_t *a_poses,
	const vr::TrackedDevicePose_t *b_poses,
	int num_poses,
	float epsilon = 0.0001f)
{
	bool similar = true;
	for (int i = 0; i < num_poses; i++)
	{
		if (!softcompare_poses(a_poses[i], b_poses[i], epsilon))
		{
			similar = false;
			break;
		}
	}
	return similar;
}


// heuristic
// return 0 for identical or a bigger number
inline int softcompare_controllerstates(const vr::VRControllerState_t *a, const vr::VRControllerState_t *b)
{
	int score = 0;
	if (a->unPacketNum != b->unPacketNum)
	{
		score++; // 1 point. basically a tie breaker if the buttons match
	}

	if (a->ulButtonPressed != b->ulButtonPressed)
	{
		score += 3;
	}

	if (a->ulButtonTouched != b->ulButtonTouched)
	{
		score += 3;
	}

	// max score of 10
	for (int axis = 0; axis < vr::k_unControllerStateAxisCount; axis++)
	{
		if (fabs(a->rAxis[axis].y - b->rAxis[axis].y) > 0.001f)
		{
			score++;
		}
		if (fabs(a->rAxis[axis].x - b->rAxis[axis].x) > 0.001f)
		{
			score++;
		}
	}
	return score;
}
