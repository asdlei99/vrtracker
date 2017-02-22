#include "InterfaceAuditor.h"
#include "InterfaceAuditorInternal.h"
#include <vrdelta.h>
#include <windows.h>
#include <assert.h>
#include <vector>
#include <chrono>
#include <thread>
#include "openvr_softcompare.h"
#include "openvr_broker.h"
#include "dprintf.h"


// how to calculate a velocity from a transform
void ApplyTranformIdea(
	vr::TrackedDevicePose_t *B,
	const vr::TrackedDevicePose_t *A,
	const vr::HmdMatrix34_t *transform)
{
	B->vAngularVelocity = A->vAngularVelocity;
	B->eTrackingResult	= A->eTrackingResult;
	B->bPoseIsValid		= A->bPoseIsValid;
	B->bDeviceIsConnected = A->bDeviceIsConnected;

	// apply transform to pose
	for (int i = 0; i < 4; i++)
	{
		float a = transform->m[0][i];
		float b = transform->m[1][i];
		float c = transform->m[2][i];
		B->mDeviceToAbsoluteTracking.m[0][i] = A->mDeviceToAbsoluteTracking.m[0][0] * a + 
											   A->mDeviceToAbsoluteTracking.m[0][1] * b + 
											   A->mDeviceToAbsoluteTracking.m[0][2] * c + 
											   A->mDeviceToAbsoluteTracking.m[0][3];

		B->mDeviceToAbsoluteTracking.m[1][i] = A->mDeviceToAbsoluteTracking.m[1][0] * a + 
											   A->mDeviceToAbsoluteTracking.m[1][1] * b + 
											   A->mDeviceToAbsoluteTracking.m[1][2] * c +
											   A->mDeviceToAbsoluteTracking.m[1][3];

		B->mDeviceToAbsoluteTracking.m[2][i] = A->mDeviceToAbsoluteTracking.m[2][0] * a + 
											   A->mDeviceToAbsoluteTracking.m[2][1] * b + 
											   A->mDeviceToAbsoluteTracking.m[2][2] * c +
											   A->mDeviceToAbsoluteTracking.m[2][3];
	}

	//
	//  update linear velocity
	// 

	// 1. apply rotation part of A to the translation component of the transform ([R][T])
	float tx = transform->m[0][3];
	float ty = transform->m[1][3];
	float tz = transform->m[2][3];
	float rx = A->mDeviceToAbsoluteTracking.m[0][0] * tx +
		       A->mDeviceToAbsoluteTracking.m[0][1] * ty +
			   A->mDeviceToAbsoluteTracking.m[0][2] * tz;
	float ry = A->mDeviceToAbsoluteTracking.m[1][0] * tx +
		       A->mDeviceToAbsoluteTracking.m[1][1] * ty +
			   A->mDeviceToAbsoluteTracking.m[1][2] * tz;
	float rz = A->mDeviceToAbsoluteTracking.m[2][0] * tx +
		       A->mDeviceToAbsoluteTracking.m[2][1] * ty +
			   A->mDeviceToAbsoluteTracking.m[2][2] * tz;
	
	// final linear velocity is angular cross-prod step-1
	B->vVelocity.v[0] = A->vVelocity.v[0] + A->vAngularVelocity.v[1] * rz - A->vAngularVelocity.v[2] * ry;
	B->vVelocity.v[1] = A->vVelocity.v[1] - (A->vAngularVelocity.v[0] * rz - A->vAngularVelocity.v[2] * rx);
	B->vVelocity.v[2] = A->vVelocity.v[2] + A->vAngularVelocity.v[0] * ry - A->vAngularVelocity.v[1] * rx;
}

void DoApplyTransform(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib,
	vr::TrackedDevicePose_t *B,
	const vr::TrackedDevicePose_t *A,
	const vr::HmdMatrix34_t *transform)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	vr::TrackedDevicePose_t output;
	uninit(output);
	a->sysi->ApplyTransform(B, A, transform);

	vr::TrackedDevicePose_t output_idea;
	uninit(output_idea);

	ApplyTranformIdea(B, A, transform);

	assert(memcmp(&output, &output_idea, sizeof(output)) == 0);

}

static void IsTrackingTransformStraightMultiply(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	vr::TrackedDevicePose_t pose;
	memset(&pose, 0, sizeof(pose));
	pose.eTrackingResult = vr::TrackingResult_Running_OK;
	pose.bPoseIsValid = true;
	pose.bDeviceIsConnected = true;

	pose.mDeviceToAbsoluteTracking = {
		1.0f, 2.0f, 3.0f, 4.0f,
		5.0f, 6.0f, 7.0f, 8.0f,
		9.0f, 10.0f, 11.0f, 12.0f,
	};

	vr::HmdMatrix34_t mult =
	{
		11.0f, 2.0f, 3.0f, 4.0f,
		5.0f, 6.0f, 7.0f, 8.0f,
		9.0f, 10.0f, 11.0f, 12.0f,
	};
	
	vr::TrackedDevicePose_t outputA;
	DoApplyTransform(ia, ib,&outputA, &pose, &mult);

	// yes it is!. mDeviceToAbsoluteTracking * mult (verified in octave)


}

static double randMToN(double M, double N)
{
	return M + (rand() / (RAND_MAX / (N - M)));
}


static void scratch(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();

	vr::TrackedDevicePose_t pose;
	memset(&pose, 0, sizeof(pose));
	pose.eTrackingResult = vr::TrackingResult_Running_OK;
	pose.bPoseIsValid = true;
	pose.bDeviceIsConnected = true;

	pose.mDeviceToAbsoluteTracking = {
		1.f,0,0,0.f,
		0,1.f,0,0.f,
		0,0,1.f,0.f,
	};
	pose.vVelocity = { 10000, 20000, 30000.0f };
	pose.vAngularVelocity = { 0.0f, 3.0f, 5.0f };

	vr::HmdMatrix34_t x_translation =
	{ 1, 0, 0, 1000,
		0, 1, 0, 0,
		0, 0, 1, 0 };
	// example 1
	vr::TrackedDevicePose_t output;
	uninit(output);
	DoApplyTransform(ia, ib, &output, &pose, &x_translation);

	vr::TrackedDevicePose_t output_idea;
	uninit(output_idea);
	ApplyTranformIdea(&output_idea, &pose, &x_translation);
	
	memset(&output, 0, sizeof(output));
	DoApplyTransform(ia, ib,&output, &pose, &x_translation);

	memset(&output_idea, 0, sizeof(output));
	ApplyTranformIdea(&output_idea, &pose, &x_translation);
	assert(output.bDeviceIsConnected == output_idea.bDeviceIsConnected);
	assert(output.bPoseIsValid == output_idea.bPoseIsValid);
	assert(output.eTrackingResult == output_idea.eTrackingResult);
	assert(output.vAngularVelocity == output_idea.vAngularVelocity);
	assert(output.vVelocity == output_idea.vVelocity);
	assert(memcmp(&output, &output_idea, sizeof(output)) == 0);

	// example 2 modify the inital pose to see if it contributes
	// to the final rotation

	// 2A: modify the rotation part
	vr::TrackedDevicePose_t output2A;
	vr::TrackedDevicePose_t pose2A;
	pose2A.mDeviceToAbsoluteTracking =
	{
		2, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0
	};
	pose2A.vVelocity = { 10000, 20000, 30000.0f };
	pose2A.vAngularVelocity = { 0.0f, 3.0f, 5.0f };

	DoApplyTransform(ia, ib, &output2A, &pose2A, &x_translation);

	DoApplyTransform(ia, ib,&output2A, &pose2A, &x_translation);
	ApplyTranformIdea(&output_idea, &pose, &x_translation);
	assert(memcmp(&output_idea, &output_idea, sizeof(output)) == 0);


	// does 'idea2 hold for another one pose
	vr::TrackedDevicePose_t output2A1;
	vr::TrackedDevicePose_t pose2A1;
	pose2A1.mDeviceToAbsoluteTracking =
	{
		1, 2, 0, 0,
		4, 1, 0, 0,
		0, 0, 1, 0
	};
	pose2A1.vVelocity = { 10000, 20000, 30000.0f };
	pose2A1.vAngularVelocity = { 0.0f, 3.0f, 5.0f };
	//worked:
	DoApplyTransform(ia, ib,&output2A1, &pose2A1, &x_translation);
	

	vr::TrackedDevicePose_t output2A2;
	vr::TrackedDevicePose_t pose2A2;
	pose2A2.mDeviceToAbsoluteTracking =
	{
		-0.6f, .6f, 0.6f, -0.7f,
		0.1f, 0.7f, -0.6f, 1.f,
		-0.8f, -.3f, -.5f, 0.2f
	};
	pose2A2.vVelocity = { 10000, 20000, 30000.0f };
	pose2A2.vAngularVelocity = { 0.0f, 3.0f, 5.0f };
	DoApplyTransform(ia, ib,&output2A2, &pose2A2, &x_translation);

	// change angular velocity
	vr::TrackedDevicePose_t output2A3;
	vr::TrackedDevicePose_t pose2A3;
	pose2A3.mDeviceToAbsoluteTracking =
	{
		-0.6f, .6f, 0.6f, -0.7f,
		0.1f, 0.7f, -0.6f, 1.f,
		-0.8f, -.3f, -.5f, 0.2f
	};
	pose2A3.vVelocity = { 10000, 20000, 30000.0f };
	pose2A3.vAngularVelocity = { 3.0f, 5.0f, 0.f };
	DoApplyTransform(ia, ib,&output2A3, &pose2A3, &x_translation);

	// change angular velocity - one param
	vr::TrackedDevicePose_t output2A4;
	vr::TrackedDevicePose_t pose2A4;
	pose2A4.mDeviceToAbsoluteTracking =
	{
		-0.6f, .6f, 0.6f, -0.7f,
		0.1f, 0.7f, -0.6f, 1.f,
		-0.8f, -.3f, -.5f, 0.2f
	};
	pose2A4.vVelocity = { 10000, 20000, 30000.0f };
	pose2A4.vAngularVelocity = { 0.f, 0.f, 3.f };
	DoApplyTransform(ia, ib,&output2A4, &pose2A4, &x_translation);

	// change angular velocity - one param
	vr::TrackedDevicePose_t output2A5;
	vr::TrackedDevicePose_t pose2A5;
	pose2A5.mDeviceToAbsoluteTracking =
	{
		-0.6f, .6f, 0.6f, -0.7f,
		0.1f, 0.7f, -0.6f, 1.f,
		-0.8f, -.3f, -.5f, 0.2f
	};
	pose2A5.vVelocity = { 10000, 20000, 30000.0f };
	pose2A5.vAngularVelocity = { 0.f, 0.f, 0.f };
	DoApplyTransform(ia, ib,&output2A5, &pose2A5, &x_translation);

	vr::TrackedDevicePose_t output2A6;
	vr::TrackedDevicePose_t pose2A6;
	pose2A6.mDeviceToAbsoluteTracking =
	{
		-0.6f, .6f, 0.6f, -0.7f,
		0.1f, 0.7f, -0.6f, 1.f,
		-0.8f, -.3f, -.5f, 0.2f
	};
	pose2A6.vVelocity = { 10000, 20000, 30000.0f };
	pose2A6.vAngularVelocity = { 1.f, 2.f, 3.f };
	DoApplyTransform(ia, ib,&output2A6, &pose2A6, &x_translation);

	// modify the translation part of the transform
	vr::TrackedDevicePose_t output2A7;
	vr::TrackedDevicePose_t pose2A7;
	pose2A7.mDeviceToAbsoluteTracking =
	{
		-0.6f, .6f, 0.6f, -0.7f,
		0.1f, 0.7f, -0.6f, 1.f,
		-0.8f, -.3f, -.5f, 0.2f
	};
	pose2A7.vVelocity = { 10000, 20000, 30000.0f };	
	pose2A7.vAngularVelocity = { 1.f, 2.f, 3.f };
	vr::HmdMatrix34_t transform_with_modified_translation =
	{ 1, 0, 0, 1000,
		0, 1, 0, 5,
		0, 0, 1, 17 };
	DoApplyTransform(ia, ib,&output2A7, &pose2A7, &transform_with_modified_translation);

	// show messing with rotation part of the transform has no effect
	// (gives same result at 2A7)
	vr::TrackedDevicePose_t output2A8;
	vr::TrackedDevicePose_t pose2A8;
	pose2A8.mDeviceToAbsoluteTracking =
	{
		-0.6f, .6f, 0.6f, -0.7f,
		0.1f, 0.7f, -0.6f, 1.f,
		-0.8f, -.3f, -.5f, 0.2f
	};
	pose2A8.vVelocity = { 10000, 20000, 30000.0f };
	pose2A8.vAngularVelocity = { 1.f, 2.f, 3.f };
	vr::HmdMatrix34_t transform_with_modified_translation2 =
	{ 11, 3, 775, 1000,
		6, .3, 76, 5,
		2, 22, 9090, 17 };
	DoApplyTransform(ia, ib,&output2A8, &pose2A8, 
					&transform_with_modified_translation2);
	assert(output2A8.vVelocity == output2A7.vVelocity);
	 
	// show messing with transform part of the pose has no effect
	// give same result as 2A7 and 2A8
	vr::TrackedDevicePose_t output2A9;
	vr::TrackedDevicePose_t pose2A9;
	pose2A9.mDeviceToAbsoluteTracking =
	{
		-0.6f, .6f, 0.6f, -4.7f,
		0.1f, 0.7f, -0.6f, 11.f,
		-0.8f, -.3f, -.5f, 7222.2f
	};
	pose2A9.vVelocity = { 10000, 20000, 30000.0f };
	pose2A9.vAngularVelocity = { 1.f, 2.f, 3.f };
	DoApplyTransform(ia, ib,&output2A9, &pose2A9,
		&transform_with_modified_translation2);
	assert(output2A8.vVelocity == output2A9.vVelocity);





	// 2B: modify the translation part of the pose
	vr::TrackedDevicePose_t output2B;
	vr::TrackedDevicePose_t pose2B;
	pose2B.mDeviceToAbsoluteTracking =
	{
		1.f,0,0,6.f,
		0,1.f,0,0.f,
		0,0,1.f,0.f,
	};
	pose2B.vVelocity = { 10000, 20000, 30000.0f };
	pose2B.vAngularVelocity = { 0.0f, 3.0f, 5.0f };
	DoApplyTransform(ia, ib,&output2B, &pose2B, &x_translation);
	assert(output2B.vVelocity == output.vVelocity);



	// 2B: modify the translation part of pose
	vr::TrackedDevicePose_t output2B1;
	vr::TrackedDevicePose_t pose2B1;
	pose2B1.mDeviceToAbsoluteTracking =
	{
		1.f,0,0,6.f,
		0,1.f,0,5.f,
		0,0,1.f,4.f,
	};
	pose2B1.vVelocity = { 10000, 20000, 30000.0f };
	pose2B1.vAngularVelocity = { 0.0f, 3.0f, 5.0f };
	DoApplyTransform(ia, ib,&output2B1, &pose2B1, &x_translation);
	assert(output2B1.vVelocity == output.vVelocity);
	for (int i = 0; i < 1000; i++)
	{
		pose2B1.mDeviceToAbsoluteTracking.m[0][3] = randMToN(-10.0f, 10.0f);
		pose2B1.mDeviceToAbsoluteTracking.m[1][3] = randMToN(-10.0f, 10.0f);
		pose2B1.mDeviceToAbsoluteTracking.m[2][3] = randMToN(-10.0f, 10.0f);
		DoApplyTransform(ia, ib,&output2B1, &pose2B1, &x_translation);
		assert(output2B1.vVelocity == output.vVelocity);
	}



	// example3
	// example 3 0 modify the rotation parts of the transform
	vr::TrackedDevicePose_t output3;
	float angle = 3.14f / 4.0f;
	vr::HmdMatrix34_t y_axis_rotation =
	{
		cos(angle)*.1, .1, sin(angle), 1000,
		0,        1, 0,					0,
		-sin(angle)*.1, 0, cos(angle),	0
	};
	DoApplyTransform(ia, ib,&output3, &pose, &y_axis_rotation);
	assert(output3.vVelocity == output.vVelocity);
	vr::HmdMatrix34_t x_axis_rotation =
	{ 1, 0, 0,						1000,
		0, cos(angle), -sin(angle), 0,
		0, sin(angle), cos(angle),	0 };
	DoApplyTransform(ia, ib,&output3, &pose, &y_axis_rotation);
	assert(output3.vVelocity == output.vVelocity);
	
	//
	for (int i = 0; i < 1000; i++)
	{	
		vr::HmdMatrix34_t tx;
		for (int r = 0; r < 3; r++)
		{
			for (int c = 0; c < 3; c++) // don't to last col
			{
				tx.m[r][c] = randMToN(-10.0f, 10.0f);
			}
		}
		tx.m[0][3] = 1000;
		tx.m[1][3] = 0;
		tx.m[2][3] = 0;
		DoApplyTransform(ia, ib,&output3, &pose, &tx);
		assert(output3.vVelocity == output.vVelocity);
	}
	
	
	DoApplyTransform(ia, ib,&output, &pose, &y_axis_rotation);

#if 0

	// random stress
	struct stuff
	{
		vr::TrackedDevicePose_t random_pose;
		vr::HmdMatrix34_t transform;
	};
	
	for (uint64_t i = 0; i < 1000000000; i++)
	{
		stuff s;
		char *c = (char*)&s;
		for (int j = 0; j < sizeof(stuff); j++)
		{
			c[j] = rand();
		}
		vr::TrackedDevicePose_t random_output;
		DoApplyTransform(ia, ib, &random_output, &s.random_pose, &s.transform);

	}
#endif



}
		
// does mDeviceToAbsoluteTracking matrix affect how transforms
// are applied to vVelocity or vAngularVelocity
//
//   YES, for example an X translation is interpreted differently in velocity based on (mDeviceToAbsoluteTracking).
//        when it has a translation AND when there is an angular velocity
//
static void DoesFrameAffectVelocities(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();

	vr::TrackedDevicePose_t pose;
	memset(&pose, 0, sizeof(pose));
	pose.eTrackingResult = vr::TrackingResult_Running_OK;
	pose.bPoseIsValid = true;
	pose.bDeviceIsConnected = true;

	pose.mDeviceToAbsoluteTracking = {
		-0.59f, 0.58f, 0.56f, -0.65f ,
		0.14f, 0.76f, -0.64f, 1.1f ,
		-0.80f, -.30f, -.52f, 0.15f
	};
		
	pose.vVelocity = { -.56f, .64f, .52f };
	pose.vAngularVelocity = { -0.58f, -3.7f, -2.5f };

	// perturb pose
	vr::TrackedDevicePose_t pose2 = pose;
	pose2.mDeviceToAbsoluteTracking.m[0][0] = -1.0f;
	pose2.mDeviceToAbsoluteTracking.m[1][1] = 1.0f;
	pose2.mDeviceToAbsoluteTracking.m[2][2] = 1.0f;
	
	//perturb pose
	vr::TrackedDevicePose_t pose3 = pose;
	pose3.mDeviceToAbsoluteTracking.m[0][0] = 1.0f;
	pose3.mDeviceToAbsoluteTracking.m[0][1] = 2.0f;
	pose3.mDeviceToAbsoluteTracking.m[0][2] = 3.2f;
	pose3.mDeviceToAbsoluteTracking.m[1][1] = 1.0f;
	pose3.mDeviceToAbsoluteTracking.m[2][2] = 1.0f;

	vr::TrackedDevicePose_t pose4 = pose;
	pose4.mDeviceToAbsoluteTracking.m[0][0] = 1.0f;
	pose4.mDeviceToAbsoluteTracking.m[0][1] = 2.0f;
	pose4.mDeviceToAbsoluteTracking.m[0][2] = 3.2f;
	pose4.mDeviceToAbsoluteTracking.m[1][2] = 7.2f;
	pose4.mDeviceToAbsoluteTracking.m[1][1] = 1.0f;
	pose4.mDeviceToAbsoluteTracking.m[2][2] = 1.0f;




	float angle = 3.14f / 4.0f;
	vr::HmdMatrix34_t x_axis_rotation =
	{ 1, 0, 0, 0,
		0, cos(angle), -sin(angle), 0,
		0, sin(angle), cos(angle), 0 };


	vr::HmdMatrix34_t x_translation =
	{ 1, 0, 0, 1,
		0, 1, 0, 0,
		0, 0, 1, 0 };

	vr::HmdMatrix34_t x_scale =
	{ 2.0, 0, 0, 0,
		0, 1.f, 0, 0,
		0, 0, 1.f, 0 };


	vr::HmdMatrix34_t x_shear =
	{ 2.0f, .2f, 0, 0,
		0, 1.0f, 0, 0,
		0, 0, 1.f, 0 };


	std::vector<vr::HmdMatrix34_t> transforms = { x_axis_rotation , x_translation, x_scale, x_shear };
	vr::TrackedDevicePose_t outputA;
	for (auto transform : transforms)
	{
		vr::TrackedDevicePose_t outputA, output2, output3, output4;
		uninit(outputA); uninit(output2); uninit(output3); uninit(output4);
		
		vr::TrackedDevicePose_t outputAClear;			
		memset(&outputAClear, 0, sizeof(outputAClear));
		
		vr::TrackedDevicePose_t outputABefore = outputA;
		DoApplyTransform(ia, ib,&outputA, &pose, &transform);		// apply transform to pose

		vr::TrackedDevicePose_t outputAClearBefore = outputAClear;
		DoApplyTransform(ia, ib,&outputAClear, &pose, &transform);

		DoApplyTransform(ia, ib,&output2, &pose2, &transform);     // apply transform to a different pose
		DoApplyTransform(ia, ib,&output3, &pose3, &transform);
		DoApplyTransform(ia, ib,&output4, &pose4, &transform);

		assert(outputA.vVelocity == outputAClear.vVelocity);

		
		assert(outputA.vAngularVelocity == output2.vAngularVelocity);
		assert(outputA.vAngularVelocity == output3.vAngularVelocity);	
		assert(outputA.vAngularVelocity == output4.vAngularVelocity);
	}
}

// do bPoseValid eTrackingResult or bDeviceConnected affect the transforms?
//
// ANSWER IS:
//                NO!
//
static void DoPoseFlagsAffectTransforms(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();

	// basic pose with no velocities
	vr::TrackedDevicePose_t pose_no_vel;
	memset(&pose_no_vel, 0, sizeof(pose_no_vel));
	pose_no_vel.eTrackingResult = vr::TrackingResult_Running_OK;
	pose_no_vel.bPoseIsValid = true;
	pose_no_vel.bDeviceIsConnected = true;
	pose_no_vel.mDeviceToAbsoluteTracking.m[0][0] = 1.0f;
	pose_no_vel.mDeviceToAbsoluteTracking.m[1][1] = 1.0f;
	pose_no_vel.mDeviceToAbsoluteTracking.m[2][2] = 1.0f;

	// basic pose with x velocity
	vr::TrackedDevicePose_t pose_vel_x;
	memset(&pose_vel_x, 0, sizeof(pose_no_vel));
	pose_vel_x.eTrackingResult = vr::TrackingResult_Running_OK;
	pose_vel_x.bPoseIsValid = true;
	pose_vel_x.bDeviceIsConnected = true;
	pose_vel_x.mDeviceToAbsoluteTracking.m[0][0] = 1.0f;
	pose_vel_x.mDeviceToAbsoluteTracking.m[1][1] = 1.0f;
	pose_vel_x.mDeviceToAbsoluteTracking.m[2][2] = 1.0f;
	pose_vel_x.vVelocity.v[0] = 900.0f;
	pose_vel_x.vVelocity.v[1] = 20.0f;

	// basic pose with x angular velocity
	vr::TrackedDevicePose_t pose_avel_x;
	memset(&pose_avel_x, 0, sizeof(pose_no_vel));
	pose_avel_x.eTrackingResult = vr::TrackingResult_Running_OK;
	pose_avel_x.bPoseIsValid = true;
	pose_avel_x.bDeviceIsConnected = true;
	pose_avel_x.mDeviceToAbsoluteTracking.m[0][0] = 1.0f;
	pose_avel_x.mDeviceToAbsoluteTracking.m[1][1] = 1.0f;
	pose_avel_x.mDeviceToAbsoluteTracking.m[2][2] = 1.0f;
	pose_avel_x.vAngularVelocity.v[0] = 13.0f;
	pose_avel_x.vAngularVelocity.v[1] = 8.0f;

	// pose with both linear and angular velocity
	vr::TrackedDevicePose_t pose_bvel_x;
	memset(&pose_bvel_x, 0, sizeof(pose_no_vel));
	pose_bvel_x.eTrackingResult = vr::TrackingResult_Running_OK;
	pose_bvel_x.bPoseIsValid = true;
	pose_bvel_x.bDeviceIsConnected = true;
	pose_bvel_x.mDeviceToAbsoluteTracking.m[0][0] = 1.0f;
	pose_bvel_x.mDeviceToAbsoluteTracking.m[1][1] = 1.0f;
	pose_bvel_x.mDeviceToAbsoluteTracking.m[2][2] = 1.0f;
	pose_bvel_x.vAngularVelocity.v[0] = 13.0f;
	pose_bvel_x.vAngularVelocity.v[1] = 3.0f;
	pose_bvel_x.vVelocity.v[0] = 900.0f;
	pose_bvel_x.vVelocity.v[1] = 200.0f;

	std::vector<vr::TrackedDevicePose_t> poses = { pose_no_vel, pose_vel_x, pose_avel_x, pose_bvel_x };

	float angle = 3.14 / 4.0f;
	vr::HmdMatrix34_t x_axis_rotation =
	{ 1, 0, 0, 0,
		0, cos(angle), -sin(angle), 0,
		0, sin(angle), cos(angle), 0 };

	vr::HmdMatrix34_t y_axis_rotation =
	{ cos(angle), 0, sin(angle), 0,
		0, 1, 0, 0,
		-sin(angle), 0, cos(angle), 0 };

	vr::HmdMatrix34_t x_translation =
	{ 1, 0, 0, 1,
		0, 1, 0, 0,
		0, 0, 1, 0 };

	vr::HmdMatrix34_t x_scale =
	{ 2.0, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0 };


	vr::HmdMatrix34_t x_shear =
	{ 2.0, .2, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0 };


	std::vector<vr::HmdMatrix34_t> transforms = { x_axis_rotation , x_translation, x_scale, x_shear, y_axis_rotation };

	for (auto pose : poses)
	{
		for (auto transform : transforms)
		{

			vr::TrackedDevicePose_t outputA;
			memset(&outputA, 0, sizeof(outputA)); // keep this clear 
			DoApplyTransform(ia, ib,&outputA, &pose, &transform);


			vr::TrackedDevicePose_t pose2 = pose;
			pose2.bDeviceIsConnected = false;
			vr::TrackedDevicePose_t outputA2;
			memset(&outputA2, 0, sizeof(outputA2));
			DoApplyTransform(ia, ib,&outputA2, &pose2, &transform);
			outputA2.bDeviceIsConnected = true;

			vr::TrackedDevicePose_t pose3 = pose;
			pose3.eTrackingResult = vr::TrackingResult_Uninitialized;
			vr::TrackedDevicePose_t outputA3;
			memset(&outputA3, 0, sizeof(outputA3));
			DoApplyTransform(ia, ib,&outputA3, &pose3, &transform);
			outputA3.eTrackingResult = vr::TrackingResult_Running_OK;

			vr::TrackedDevicePose_t pose4 = pose;
			pose4.bPoseIsValid = false;
			vr::TrackedDevicePose_t outputA4;
			memset(&outputA4, 0, sizeof(outputA4));
			DoApplyTransform(ia, ib,&outputA4, &pose4, &transform);
			outputA4.bPoseIsValid = true;

			vr::TrackedDevicePose_t pose5 = pose;
			pose5.bPoseIsValid = false;
			pose5.eTrackingResult = vr::TrackingResult_Uninitialized;
			vr::TrackedDevicePose_t outputA5;
			memset(&outputA5, 0, sizeof(outputA5));
			DoApplyTransform(ia, ib,&outputA5, &pose5, &transform);
			outputA5.bPoseIsValid = true;
			outputA5.eTrackingResult = vr::TrackingResult_Running_OK;

			vr::TrackedDevicePose_t pose6 = pose;
			pose6.bPoseIsValid = false;
			pose6.eTrackingResult = vr::TrackingResult_Uninitialized;
			pose6.bDeviceIsConnected = false;
			vr::TrackedDevicePose_t outputA6;
			memset(&outputA6, 0, sizeof(outputA6));
			DoApplyTransform(ia, ib,&outputA6, &pose6, &transform);
			outputA6.bPoseIsValid = true;
			outputA6.eTrackingResult = vr::TrackingResult_Running_OK;
			outputA6.bDeviceIsConnected = true;

			assert(outputA == outputA2);
			assert(outputA == outputA3);
			assert(outputA == outputA4);
			assert(outputA == outputA5);
			assert(outputA == outputA6);
		}
	}
}

void InterfaceAuditor::compare_apply_transforms(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	scratch(ia, ib);
	DoPoseFlagsAffectTransforms(ia, ib);
	DoesFrameAffectVelocities(ia, ib);
	IsTrackingTransformStraightMultiply(ia, ib);

	float pi = std::atanf(1) * 4.0f;
	std::vector<float> angles = { 0, pi / 2.0f, pi, 1.5f * pi, 2 * pi };
	std::vector<vr::HmdMatrix34_t> transforms;

	for (auto angle : angles)
	{
		vr::HmdMatrix34_t x_axis_rotation =
		{ 1, 0, 0, 0,
			0, cos(angle), -sin(angle), 0,
			0, sin(angle), cos(angle), 0 };

		vr::HmdMatrix34_t y_axis_rotation =
		{ cos(angle), 0, sin(angle), 0,
			0, 1, 0, 0,
			-sin(angle), 0, cos(angle), 0 };

		vr::HmdMatrix34_t z_axis_rotation =
		{ cos(angle), -sin(angle), 0, 0,
			sin(angle), cos(angle), 0, 0,
			0,0,1,0 };

		vr::HmdMatrix34_t x_translation =
		{ 1, 0, 0, 1,
			0, 1, 0, 0,
			0, 0, 1, 0 };

		vr::HmdMatrix34_t y_translation =
		{ 1, 0, 0, 0,
			0, 1, 0, 1,
			0, 0, 1, 0 };

		vr::HmdMatrix34_t z_translation =
		{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 1 };

		vr::HmdMatrix34_t zero =
		{ 0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0 };

		vr::HmdMatrix34_t ident =
		{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0 };

		transforms.push_back(x_axis_rotation);
		transforms.push_back(y_axis_rotation);
		transforms.push_back(z_axis_rotation);
		transforms.push_back(x_translation);
		transforms.push_back(y_translation);
		transforms.push_back(z_translation);
		transforms.push_back(zero);
		transforms.push_back(ident);
	}
	
	std::vector<vr::TrackedDevicePose_t> poses;
	for (int i = 0; i < 2; i++)
	{
		bool pose_valid = (i == 0);
		// consider the effects of the device being disconnected
		for (int j = 0; j < 2; j++)
		{
			bool device_connected = (j == 0);
			for (int k = 0; k < 2; k++)
			{
				vr::ETrackingResult tracking;
				if (k == 0)
				{
					tracking = vr::TrackingResult_Running_OK;
				}
				else
				{
					tracking = vr::TrackingResult_Uninitialized;
				}

				vr::TrackedDevicePose_t pose;
				memset(&pose, 0, sizeof(pose));
				pose.eTrackingResult = tracking;
				pose.bPoseIsValid = pose_valid;
				pose.bDeviceIsConnected = device_connected;
				poses.push_back(pose);

				pose.mDeviceToAbsoluteTracking.m[0][0] = 1.0f;
				pose.mDeviceToAbsoluteTracking.m[1][1] = 1.0f;
				pose.mDeviceToAbsoluteTracking.m[2][2] = 1.0f;
				poses.push_back(pose);


				// pose with velocity 
				pose.vVelocity.v[0] = 1.0f;
				poses.push_back(pose);

				// pose angular velocity 
				pose.vAngularVelocity.v[0] = 10.0f;

				float angle = 42.0f;
				pose.mDeviceToAbsoluteTracking =
				{ cos(angle), -sin(angle), 0, 0,
					sin(angle), cos(angle), 0, 0,
					0,0,1,0 };
				poses.push_back(pose);
			}
		}
	}

	for (const auto pose : poses)
	{
		for (const auto transform : transforms)
		{
			vr::TrackedDevicePose_t posea;
			uninit(posea);
			DoApplyTransform(ia, ib,&posea, &pose, &transform);
		}
	}

}

