// openvr_semantics_unit_test.cpp : Defines the entry point for the console application.
//

#include <assert.h>
#include <vrdelta.h>

#include <chrono>
#include <thread>
#include "openvr_softcompare.h"

#include "openvr_broker.h"
#include "dprintf.h"





// passthru with dirty sync
// cursor with latest mode
// together these would give a close approximation to the behaviour of the raw interface
// and then would be easier to cross check

void compare_sysi_interfaces(openvr_broker::open_vr_interfaces *a, openvr_broker::open_vr_interfaces *b, const TrackerConfig &c);

void compare_interfaces(openvr_broker::open_vr_interfaces *a, openvr_broker::open_vr_interfaces *b);

void compare_trackedcamera_interfaces(openvr_broker::open_vr_interfaces *a, openvr_broker::open_vr_interfaces *b);

void component_state_test(openvr_broker::open_vr_interfaces *a, openvr_broker::open_vr_interfaces *b,
	vr_state_tracker_t tracker, vr_cursor_t cursor);



int main()
{
	// direct interface to open vr
	openvr_broker::open_vr_interfaces raw_interface;
	char *error;
	if (!openvr_broker::acquire_interfaces("raw", &raw_interface, &error))
	{
		dprintf("error %s\n", error);
		exit(0);
	}

	// snapshot of openvr
	TrackerConfig c = { 0.02f, 100.0f };
	vr_state_tracker_t tracker = create_vr_state_tracker(c);
	capture_vr_state(tracker, raw_interface);
	int start_frame; int end_frame;
	get_frame_range(tracker, &start_frame, &end_frame);

	// cursor into the snapshot
	vr_cursor_t cursor = create_cursor(tracker);
	openvr_broker::open_vr_interfaces cursor_interface = get_cursor_interfaces(cursor);
	set_cursor_frame(cursor, end_frame);

	compare_sysi_interfaces(&raw_interface, &cursor_interface, c);

	compare_trackedcamera_interfaces(&raw_interface, &cursor_interface);
	// component_state_test(&raw_interface, &cursor_interface, tracker, cursor);
	//compare_interfaces(&raw_interface, &cursor_interface);

    return 0;
}



void compare_sysi_interfaces(openvr_broker::open_vr_interfaces *a, openvr_broker::open_vr_interfaces *b, 
								const TrackerConfig &c)
{
	{
		uint32_t awidth;
		uint32_t aheight;
		uint32_t bwidth;
		uint32_t bheight;
		a->sysi->GetRecommendedRenderTargetSize(&awidth, &aheight);
		b->sysi->GetRecommendedRenderTargetSize(&bwidth, &bheight);
		assert(awidth == bwidth);
		assert(aheight == bheight);

		// openvr crashes
		//a->sysi->GetRecommendedRenderTargetSize(nullptr, &aheight);
		//b->sysi->GetRecommendedRenderTargetSize(nullptr, &bheight);
		//assert(awidth == bwidth);
		//assert(aheight == bheight);
		//a->sysi->GetRecommendedRenderTargetSize(&awidth, nullptr);
		//b->sysi->GetRecommendedRenderTargetSize(&bwidth, nullptr);
		//assert(awidth == bwidth);
		//assert(aheight == bheight);
	}

	{
		for (int i = 0; i < 2; i++)
		{
			vr::HmdMatrix44_t mata = a->sysi->GetProjectionMatrix(vr::EVREye(i), c.nearz, c.farz);
			vr::HmdMatrix44_t matb = b->sysi->GetProjectionMatrix(vr::EVREye(i), c.nearz, c.farz);
			assert(mata == matb);

			float aa[4];
			a->sysi->GetProjectionRaw(vr::EVREye(i), &aa[0], &aa[1], &aa[2], &aa[3]);
			float bb[4];
			b->sysi->GetProjectionRaw(vr::EVREye(i), &bb[0], &bb[1], &bb[2], &bb[3]);
			assert(memcmp(aa, bb, sizeof(aa[0] * 4)) == 0);

			vr::DistortionCoordinates_t ad;
			bool disa = a->sysi->ComputeDistortion(vr::EVREye(i), c.distortionU, c.distortionV, &ad);

			vr::DistortionCoordinates_t bd;
			bool disb = b->sysi->ComputeDistortion(vr::EVREye(i), c.distortionU, c.distortionV, &bd);
			assert(disa == disb);
			assert(ad == bd);

			vr::HmdMatrix34_t eha = a->sysi->GetEyeToHeadTransform(vr::EVREye(i));
			vr::HmdMatrix34_t ehb = b->sysi->GetEyeToHeadTransform(vr::EVREye(i));
			assert(eha == ehb);

			for (int mesh_type = 0; mesh_type < 3; mesh_type++)
			{
				vr::HiddenAreaMesh_t mesha = a->sysi->GetHiddenAreaMesh(vr::EVREye(i), vr::EHiddenAreaMeshType(mesh_type));
				vr::HiddenAreaMesh_t meshb = b->sysi->GetHiddenAreaMesh(vr::EVREye(i), vr::EHiddenAreaMeshType(mesh_type));

				assert(mesha.unTriangleCount == meshb.unTriangleCount);
				assert(memcmp(mesha.pVertexData, meshb.pVertexData, sizeof(float) * 2 * mesha.unTriangleCount) == 0);


			}
		}


	}
	{
		float	seca = 0;
		uint64_t fca = 0;
		bool vsa = a->sysi->GetTimeSinceLastVsync(&seca, &fca);

		float	secb = 0;
		uint64_t fcb = 0;
		bool vsb = b->sysi->GetTimeSinceLastVsync(&secb, &fcb);
		assert(vsa == vsb);

		// since sample timings differ - use a coarse filter 
		assert(fabs(seca - secb) < 0.5f);
		assert(fabs(fca - fcb) < 500); // ignore this if you were stepping through code!

		bool ona = a->sysi->IsDisplayOnDesktop();
		bool onb = b->sysi->IsDisplayOnDesktop();
		assert(ona == onb);
	}

	{
		for (int universe = 0; universe < 3; universe++)
		{
			vr::TrackedDevicePose_t posea[vr::k_unMaxTrackedDeviceCount];
			vr::TrackedDevicePose_t poseb[vr::k_unMaxTrackedDeviceCount];

			memset(posea, 0, sizeof(posea));
			memset(poseb, 0, sizeof(poseb));

			a->sysi->GetDeviceToAbsoluteTrackingPose(
				vr::ETrackingUniverseOrigin(universe),
				c.predicted_seconds_to_photon,
				posea,
				vr::k_unMaxTrackedDeviceCount);

			b->sysi->GetDeviceToAbsoluteTrackingPose(
				vr::ETrackingUniverseOrigin(universe),
				c.predicted_seconds_to_photon,
				poseb,
				vr::k_unMaxTrackedDeviceCount);

			for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
			{
				assert(softcompare_poses(posea[i],poseb[i]));
			}
		}
		vr::HmdMatrix34_t seateda = a->sysi->GetSeatedZeroPoseToStandingAbsoluteTrackingPose();
		vr::HmdMatrix34_t seatedb = b->sysi->GetSeatedZeroPoseToStandingAbsoluteTrackingPose();
		assert(seateda == seatedb);

		/** Returns the transform from the tracking origin to the standing absolute tracking system. This allows
		* applications to convert from raw tracking space to the calibrated standing coordinate system. */
		vr::HmdMatrix34_t rawa = a->sysi->GetRawZeroPoseToStandingAbsoluteTrackingPose();
		vr::HmdMatrix34_t rawb = b->sysi->GetRawZeroPoseToStandingAbsoluteTrackingPose();
		assert(rawa == rawb);

	}

	{
		for (int i = 0; i <= 4; i++)
		{
			for (int j = 0; j < vr::k_unMaxTrackedDeviceCount+1; j++)
			{
				vr::TrackedDeviceIndex_t unRelativeToTrackedDeviceIndex;
				if (j == 0)
				{
					unRelativeToTrackedDeviceIndex = -1;
				}
				else
				{
					unRelativeToTrackedDeviceIndex = j - 1;
				}
				

				vr::TrackedDeviceIndex_t indicesa[vr::k_unMaxTrackedDeviceCount];
				memset(indicesa, 0, sizeof(indicesa));
				uint32_t sorteda = a->sysi->GetSortedTrackedDeviceIndicesOfClass(vr::ETrackedDeviceClass(i),
					indicesa, vr::k_unMaxTrackedDeviceCount,
					unRelativeToTrackedDeviceIndex);

				vr::TrackedDeviceIndex_t indicesb[vr::k_unMaxTrackedDeviceCount];
				memset(indicesb, 0, sizeof(indicesb));
				uint32_t sortedb = b->sysi->GetSortedTrackedDeviceIndicesOfClass(vr::ETrackedDeviceClass(i),
					indicesb, vr::k_unMaxTrackedDeviceCount,
					unRelativeToTrackedDeviceIndex);
				assert(sorteda == sortedb);
				assert(memcmp(indicesa, indicesb, sizeof(indicesa)) == 0);

			}

		}
	}

	{
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
		{
			vr::EDeviceActivityLevel alevel = a->sysi->GetTrackedDeviceActivityLevel(i);
			vr::EDeviceActivityLevel blevel = b->sysi->GetTrackedDeviceActivityLevel(i);
			// timing differences?
			// anyways doesn't sync assert(alevel == blevel);
		}
	}

	{
		vr::TrackedDeviceIndex_t lefta = a->sysi->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
		vr::TrackedDeviceIndex_t righta = a->sysi->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
		vr::TrackedDeviceIndex_t leftb = b->sysi->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
		vr::TrackedDeviceIndex_t rightb = b->sysi->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
		assert(lefta == leftb);
		assert(righta == rightb);
	}


	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		vr::ETrackedControllerRole	rolea = a->sysi->GetControllerRoleForTrackedDeviceIndex(i);
		vr::ETrackedDeviceClass classa = a->sysi->GetTrackedDeviceClass(i);
		bool connecteda = a->sysi->IsTrackedDeviceConnected(i);

		vr::ETrackedControllerRole roleb = b->sysi->GetControllerRoleForTrackedDeviceIndex(i);
		vr::ETrackedDeviceClass classb = b->sysi->GetTrackedDeviceClass(i);
		bool connectedb = b->sysi->IsTrackedDeviceConnected(i);

		assert(rolea == roleb);
		assert(classa == classb);
		assert(connecteda == connectedb);
	}
	

	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		vr::VRControllerState_t statea;
		bool reta = a->sysi->GetControllerState(i, &statea, sizeof(statea));
		vr::VRControllerState_t stateb;
		bool retb = b->sysi->GetControllerState(i, &stateb, sizeof(stateb));
		int compare_score = softcompare_controllerstates(&statea, &stateb);
		assert(compare_score < 3);
	}


	
	{
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				vr::VRControllerState_t statea;
				vr::TrackedDevicePose_t posea;
				memset(&statea, 0, sizeof(statea));
				memset(&posea, 0, sizeof(posea));
				bool reta = a->sysi->GetControllerStateWithPose(
					vr::ETrackingUniverseOrigin(j),
					i,
					&statea, sizeof(statea),
					&posea);

				vr::VRControllerState_t stateb;
				vr::TrackedDevicePose_t poseb;
				memset(&stateb, 0, sizeof(stateb));
				memset(&poseb, 0, sizeof(poseb));
				bool retb = b->sysi->GetControllerStateWithPose(
					vr::ETrackingUniverseOrigin(j),
					i,
					&stateb, sizeof(stateb),
					&poseb);

				if (reta != retb)
				{
					assert(reta == retb);
				}
				
				int compare_score = softcompare_controllerstates(&statea, &stateb);
				assert(compare_score < 3);
				assert(softcompare_poses(posea, poseb));
			}
		}
	}

	{
		bool inputa = a->sysi->IsInputFocusCapturedByAnotherProcess();
		bool inputb = b->sysi->IsInputFocusCapturedByAnotherProcess();
		assert(inputa == inputb);
	}

	dprintf("ba");

}

void compare_trackedcamera_interfaces(openvr_broker::open_vr_interfaces *a, openvr_broker::open_vr_interfaces *b)
{
	// pass an invalid interface
	bool has_cameraa = true;
	bool has_camerab = true;
	vr::EVRTrackedCameraError rca = a->taci->HasCamera(99, &has_cameraa);
	vr::EVRTrackedCameraError rcb = b->taci->HasCamera(99, &has_camerab);
	assert(rca == rcb);
	assert(has_cameraa == has_camerab);

	// pass a null value
	rca = a->taci->HasCamera(0, nullptr);
	rcb = b->taci->HasCamera(0, nullptr);
	assert(rca == rcb);

	// pass a valid value
	rca = a->taci->HasCamera(0, &has_cameraa);
	rcb = b->taci->HasCamera(0, &has_camerab);
	assert(rca == rcb);
	assert(has_cameraa == has_camerab);

	rca = a->taci->HasCamera(1, &has_cameraa);
	rcb = b->taci->HasCamera(1, &has_camerab);
	assert(rca == rcb);
	assert(has_cameraa == has_camerab);
}


void compare_interfaces(openvr_broker::open_vr_interfaces *a, openvr_broker::open_vr_interfaces *b)
{
	// NO CONFIG CHECK: do some consitency checks with no config changes at the same time:
	// 
	//  * Expect 
	//		* configuration stuff should be identical in valid and non valid cases
	//		* realtime-stuff should be the same for non valid cases
	//

	


	// purpose: test the wrinkle interface - does it return true?

	// passing a null and count > 0 always gets to a false and does not touch count
	{
		uint32_t counta = 66;
		bool rca = a->chapsi->GetLiveCollisionBoundsInfo(nullptr, &counta);
		uint32_t countb=77;
		bool rcb = b->chapsi->GetLiveCollisionBoundsInfo(nullptr, &countb);
		assert(counta == 66);
		assert(countb == 77);
		assert((rca == false) && (rcb == false));
		dprintf("ba");
	}

	// passing a 0 and a null gives the required count and a false
	{
		uint32_t counta = 0;
		bool rca = a->chapsi->GetLiveCollisionBoundsInfo(nullptr, &counta);
		uint32_t countb = 0;
		bool rcb = b->chapsi->GetLiveCollisionBoundsInfo(nullptr, &countb);
		assert(counta == countb && counta > 0);
		assert((rca == false) && (rcb == false));
		dprintf("ba");
	}

	// here is are valid inputs to LiveCollisionBoundsInfo
	{
		uint32_t counta = 0;
		bool rca = a->chapsi->GetLiveCollisionBoundsInfo(nullptr, &counta);
		vr::HmdQuad_t *p = new vr::HmdQuad_t[counta];
		bool rcaa = a->chapsi->GetLiveCollisionBoundsInfo(p, &counta);
		delete[] p;

		uint32_t countb = 0;
		bool rcb = b->chapsi->GetLiveCollisionBoundsInfo(nullptr, &countb);
		vr::HmdQuad_t *pb = new vr::HmdQuad_t[counta];
		bool rcbb = a->chapsi->GetLiveCollisionBoundsInfo(pb, &countb);
		delete[] pb;

		assert(counta == countb && counta > 0);
		assert((rca == false) && (rcb == false));
		assert((rcaa == true) && (rcbb == true));
	}

	
	// GetLiveSeatedZeroPoseToRawTrackingPose
	{	
		bool ba = a->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(nullptr);
		bool bb = b->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(nullptr);
		assert(ba == bb);

		vr::HmdMatrix34_t posea;
		vr::HmdMatrix34_t poseb;
		bool bba = a->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(&posea);
		bool bbb = b->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(&poseb);
		assert(posea == poseb);
		assert(bba == bbb);
	}

	// GetVulkanInstanceExtensionsRequired
	{
		// nullptr test 1
		{
			uint32_t ba = a->compi->GetVulkanInstanceExtensionsRequired(nullptr, 0);
			uint32_t bb = b->compi->GetVulkanInstanceExtensionsRequired(nullptr, 0);
			assert(ba == bb);
			dprintf("ba");
		}
		
		// nullptr test 2
		{
			uint32_t ba = a->compi->GetVulkanInstanceExtensionsRequired(nullptr, 42);
			uint32_t bb = b->compi->GetVulkanInstanceExtensionsRequired(nullptr, 42);
			assert(ba == bb);
			dprintf("ba");
		}
		
		// nullptr test 2
		{
			char bufa[256];
			uint32_t ba = a->compi->GetVulkanInstanceExtensionsRequired(bufa, 256);
			char bufb[256];
			uint32_t bb = b->compi->GetVulkanInstanceExtensionsRequired(bufb, 256);
			assert(ba == bb);
			assert(strcmp(bufa, bufb) == 0);
			dprintf("ba");
		}			
	}
	
	{
		vr::EVRCompositorError ba = a->compi->GetLastPoses(nullptr, 0,nullptr, 0);
		vr::EVRCompositorError bb = b->compi->GetLastPoses(nullptr, 0, nullptr, 0);
		assert(ba == bb);
	}

	{
		vr::TrackedDevicePose_t gposesa[16];
		vr::TrackedDevicePose_t rposesa[16];
		vr::TrackedDevicePose_t gposesb[16];
		vr::TrackedDevicePose_t rposesb[16];
		vr::EVRCompositorError ba = a->compi->GetLastPoses(rposesa, 1, gposesa, 1);
		vr::EVRCompositorError bb = b->compi->GetLastPoses(rposesb, 1, gposesb, 1);
		
		assert(softcompare_poses(gposesa[0], gposesb[0]));
		assert(softcompare_poses(rposesa[0], rposesb[0]));
		assert(ba == bb);
	}

	{
		//rendermodel //invalid modelname test: banana
		vr::EVRRenderModelError errora;
		vr::RenderModel_t *modela = nullptr;
		errora = a->remi->LoadRenderModel_Async("banana", &modela);

		while (1)
		{
			errora = a->remi->LoadRenderModel_Async("banana", &modela);
			if (errora != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}	

		vr::EVRRenderModelError errorb;
		vr::RenderModel_t *modelb = nullptr;
		errorb = b->remi->LoadRenderModel_Async("banana", &modelb);
		while (1)
		{
			errorb = b->remi->LoadRenderModel_Async("banana", &modela);
			if (errorb != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		assert(errora == errorb);
	}

	{
		//rendermodel //invalid modelname test: nullptr

		vr::EVRRenderModelError errora;
		vr::RenderModel_t *modela = nullptr;
		errora = a->remi->LoadRenderModel_Async(nullptr, &modela);
		while (1)
		{
			errora = a->remi->LoadRenderModel_Async(nullptr, &modela);
			if (errora != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}


		vr::EVRRenderModelError errorb;
		vr::RenderModel_t *modelb = nullptr;
		errorb = b->remi->LoadRenderModel_Async(nullptr, &modelb);
		while (1)
		{
			errora = a->remi->LoadRenderModel_Async(nullptr, &modela);
			if (errora != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		assert(errora == errorb);
	}


	// rendermodel test
	{
		char render_model_name[256];
		a->remi->GetRenderModelName(0, render_model_name, sizeof(render_model_name));

		// load from a
		vr::EVRRenderModelError errora;
		vr::RenderModel_t *modela = nullptr;
		while (1)
		{
			 errora = a->remi->LoadRenderModel_Async(render_model_name, &modela);
			if (errora != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		// load from b
		vr::EVRRenderModelError errorb;
		vr::RenderModel_t *modelb = nullptr;
		while (1)
		{
			errorb = b->remi->LoadRenderModel_Async(render_model_name, &modelb);
			if (errorb != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		assert(errora == errorb);
	}


	// rendermodel and texture test
	{
		char render_model_name[256];
		a->remi->GetRenderModelName(0, render_model_name, sizeof(render_model_name));

		// load from a
		vr::EVRRenderModelError errora;
		vr::RenderModel_t *modela = nullptr;
		while (1)
		{
			errora = a->remi->LoadRenderModel_Async(render_model_name, &modela);
			if (errora != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		// test can't continue unless
		assert(errora == vr::VRRenderModelError_None);

		vr::EVRRenderModelError error_tex_a;
		vr::RenderModel_TextureMap_t *texa = nullptr;
		error_tex_a = a->remi->LoadTexture_Async(modela->diffuseTextureId, &texa);
		while (1)
		{
			error_tex_a = a->remi->LoadTexture_Async(modela->diffuseTextureId, &texa);
			if (error_tex_a != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		assert(error_tex_a == vr::VRRenderModelError_None);



		// load model from b
		vr::EVRRenderModelError errorb;
		vr::RenderModel_t *modelb = nullptr;
		while (1)
		{
			errorb = b->remi->LoadRenderModel_Async(render_model_name, &modelb);
			if (errorb != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		assert(errorb == vr::VRRenderModelError_None);

		//
		// load texture from b
		//

		vr::EVRRenderModelError error_tex_b;
		vr::RenderModel_TextureMap_t *texb = nullptr;
		error_tex_b = b->remi->LoadTexture_Async(modelb->diffuseTextureId, &texb);
		while (1)
		{
			error_tex_b = b->remi->LoadTexture_Async(modelb->diffuseTextureId, &texb);
			if (error_tex_b != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		assert(errora == errorb);
		assert(error_tex_a == error_tex_b);
	}


	// rendermodel walk down models and components
	{
		uint32_t counta = a->remi->GetRenderModelCount();
		uint32_t countb = b->remi->GetRenderModelCount();
		assert(counta == countb);
		for (int i = 0; i < (int)counta; i++)
		{
			char namea[256];
			char nameb[256];
			uint32_t a_ret = a->remi->GetRenderModelName(i, namea, sizeof(namea));
			uint32_t b_ret = b->remi->GetRenderModelName(i, nameb, sizeof(nameb));
			assert(a_ret == b_ret);
			assert(strcmp(namea, nameb) == 0);

			uint32_t ccounta = a->remi->GetComponentCount(namea);
			uint32_t ccountb = b->remi->GetComponentCount(nameb);
			assert(ccounta == ccountb);

			char thumba[256];
			char thumbb[256];
			vr::EVRRenderModelError terrora;
			vr::EVRRenderModelError terrorb;
			uint32_t treta = a->remi->GetRenderModelThumbnailURL(namea, thumba, sizeof(thumba), &terrora);
			uint32_t tretb = b->remi->GetRenderModelThumbnailURL(nameb, thumbb, sizeof(thumbb), &terrorb);
			assert(treta == tretb);

			// GetRenderModelThumbnailURL sometimes returns 
			// Error_None with an empty string and sometimes returns 
			// Invalid_Model

			// so in the case of ok and empty string, pretend it returned
			// InvalidModel
			if (treta == 1)
			{
				assert(terrorb == vr::VRRenderModelError_None || terrorb == vr::VRRenderModelError_InvalidModel);
			}
			else
			{
				assert(terrora == terrorb);
			}
			

			assert(strcmp(thumba, thumbb) == 0);

			char patha[256];
			char pathb[256];
			vr::EVRRenderModelError perrora;
			vr::EVRRenderModelError perrorb;
			uint32_t pathreta = a->remi->GetRenderModelOriginalPath(namea, patha, sizeof(patha), &perrora);
			uint32_t pathretb = b->remi->GetRenderModelOriginalPath(nameb, pathb, sizeof(pathb), &perrorb);
			assert(pathreta == pathretb);
			assert(perrora == perrorb);
			assert(strcmp(patha, pathb) == 0);

			for (int j = 0; j < (int)ccounta; j++)
			{
				char cnamea[256];
				char cnameb[256];
				uint32_t cnreta = a->remi->GetComponentName(namea, j, cnamea, sizeof(cnamea));
				uint32_t cnretb = b->remi->GetComponentName(nameb, j, cnameb, sizeof(cnameb));
				assert(cnreta == cnretb);
				assert(strcmp(cnamea, cnameb) == 0);

				uint64_t bma = a->remi->GetComponentButtonMask(namea, cnamea);
				uint64_t bmb = b->remi->GetComponentButtonMask(nameb, cnameb);
				assert(bma == bmb);


				char crmnamea[256];
				char crmnameb[256];
				uint32_t crnreta = a->remi->GetComponentRenderModelName(namea, cnamea, crmnamea, sizeof(crmnamea));
				uint32_t crnretb = b->remi->GetComponentRenderModelName(nameb, cnameb, crmnameb, sizeof(crmnameb));
				if (crnreta != 0 || crnretb != 0)
				{
					assert(crnreta == crnretb);
					// if there is a render model name, then compare them
					assert(strcmp(crmnamea, crmnameb) == 0);
				}
				
				bool hasa = a->remi->RenderModelHasComponent(namea, cnamea);
				bool hasb = b->remi->RenderModelHasComponent(nameb, cnameb);
				assert(hasa == hasb);
			}
		}
	}

	dprintf("bla");
}



void component_state_test(openvr_broker::open_vr_interfaces *a, openvr_broker::open_vr_interfaces *b,
	vr_state_tracker_t tracker, vr_cursor_t cursor)
{

	int controller_index = 3;
	char rendermodelname[256];
	a->sysi->GetStringTrackedDeviceProperty(controller_index, vr::Prop_RenderModelName_String, rendermodelname, sizeof(rendermodelname));

#if 0
	for (int i = 0; i < (int)a->remi->GetComponentCount(rendermodelname); i++)
	{
		char componentname[256];
		a->remi->GetComponentName(rendermodelname, i, componentname, sizeof(componentname));
		dprintf("componenetname %s\n", componentname);
	}
#endif 

	// dynamic test
	const char *pchRenderModelName = rendermodelname;
	const char *pchComponentName = "trigger";

	while (1)
	{
		vr::VRControllerState_t controller_statea;
		vr::VRControllerState_t controller_stateb;

		vr::RenderModel_ControllerMode_State_t render_statea;
		render_statea.bScrollWheelVisible = true;
		vr::RenderModel_ControllerMode_State_t render_stateb;
		render_stateb.bScrollWheelVisible = true;

		vr::RenderModel_ComponentState_t component_state_outputa;
		vr::RenderModel_ComponentState_t component_state_outputb;

		// call into get controller state once
		bool breta = a->sysi->GetControllerState(3, &controller_statea, sizeof(controller_statea));

		// snapshot it
		// this calls into it again - so there might 
		// be some deviation (unPackedNum)
		capture_vr_state(tracker, *a);

		// move cursor
		int start_frame; int end_frame;
		get_frame_range(tracker, &start_frame, &end_frame);
		set_cursor_frame(cursor, end_frame);

		bool bretb = b->sysi->GetControllerState(3, &controller_stateb, sizeof(controller_stateb));
		assert(breta == bretb);
		
		bool csa = a->remi->GetComponentState(
			pchRenderModelName,
			pchComponentName,
			&controller_statea,
			&render_statea,
			&component_state_outputa);

		bool csb = b->remi->GetComponentState(
			pchRenderModelName,
			pchComponentName,
			&controller_stateb,
			&render_stateb,
			&component_state_outputb);

		assert(csa == csb);
		// since the inputs are not identical, nor will the outputs be
		if (component_state_outputa != component_state_outputb)
		{
			// calculate a simularity score
			int score = 0;
			if (component_state_outputa.uProperties != component_state_outputb.uProperties)
			{
				score += 10;
			}

			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (fabs(component_state_outputa.mTrackingToComponentRenderModel.m[i][j] -
						component_state_outputb.mTrackingToComponentRenderModel.m[i][j]) > 0.001f)
					{
						score++;
					}
					if (fabs(component_state_outputa.mTrackingToComponentLocal.m[i][j] -
						component_state_outputb.mTrackingToComponentLocal.m[i][j]) > 0.001f)
					{
						score++;
					}
				}
			}
			// assert the score is better than some value
			if (score > 12)
			{
				dprintf("score is %d\n", score);
			}
		}	
	}


	
	

}

