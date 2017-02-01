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
