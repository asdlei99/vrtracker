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

InterfaceAuditor::InterfaceAuditor()
{
	num_passes = 0;
	num_failures = 0;
	num_waivers = 0;
}

void InterfaceAuditor::PrintResults()
{
	dprintf("Audit took %d us\n", std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count());
	dprintf(" %d passes\n", num_passes);
	dprintf(" %d waives\n", num_waivers);
	dprintf(" %d failures\n", num_failures);
}

void InterfaceAuditor::ReportFailure(const char *string, const char *file, int line)
{
 	dprintf("%s %s %d\n", string, file, line);
}

void InterfaceAuditor::ReportWaive(const char *string, const char *desc, const char *file, int line)
{
//	dprintf("waive %s\n", desc);
}

void InterfaceAuditor::AuditInterfaces(
	OpenVRInterfaceUnderTest *ia, 
	OpenVRInterfaceUnderTest *ib, const TrackerConfig &c, 
	bool read_only_tests,
	bool large_time_gap_override,
	bool do_interactive)
{
	start_time = std::chrono::steady_clock::now();
	

	ia->Refresh();
	ib->Refresh();
	compare_sysi_interfaces(ia, ib, c);
	compare_appi_interfaces(ia, ib, c);
	compare_seti_interfaces(ia, ib, c, read_only_tests);
	compare_chapi_interfaces(ia, ib, c, large_time_gap_override, read_only_tests);
	compare_chapsi_interfaces(ia, ib, read_only_tests);
	compare_taci_interfaces(ia, ib, c);
	compare_remi_strange_interfaces(ia, ib);
	compare_compi_interfaces(ia, ib, c, large_time_gap_override, read_only_tests);
	compare_ovi_interfaces(ia, ib, c, read_only_tests);
	compare_exdi_interfaces(ia, ib, c);
	compare_screeni_interfaces(ia, ib, c);
	compare_resi_interfaces(ia, ib, c);

	if (do_interactive)
	{
		interactive_component_state_test(ia, ib);
	}
	end_time = std::chrono::steady_clock::now();

	
		

}

void InterfaceAuditor::compare_resi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	for (int i = 0; i < c.num_resources; i++)
	{
		char apathbuffer[256];
		uninit(apathbuffer);
		{
			uint32_t aret = a->resi->GetResourceFullPath(
				c.resource_filenames[i],
				c.resource_directories[i],
				apathbuffer, sizeof(apathbuffer));

			char bpathbuffer[256];
			uninit(bpathbuffer);

			uint32_t bret = b->resi->GetResourceFullPath(
				c.resource_filenames[i],
				c.resource_directories[i],
				bpathbuffer, sizeof(bpathbuffer));
			ASSERT(aret == bret);
			ASSERT(strcmp(apathbuffer, bpathbuffer) == 0);
		}

		{
			char joinedname[2048];
			sprintf(joinedname, "%s/%s", c.resource_directories[i], c.resource_filenames[i]);  //stupid?
			uint32_t asize = a->resi->LoadSharedResource(joinedname, nullptr, 0);

			uint32_t bsize = b->resi->LoadSharedResource(joinedname, nullptr, 0);
			ASSERT(asize == bsize);

			char *abuf = (char *)malloc(asize);
			uninit(abuf, asize);
			uint32_t aret = a->resi->LoadSharedResource(joinedname, abuf, asize);
			char *bbuf = (char *)malloc(bsize);
			uninit(bbuf, bsize);
			uint32_t bret = b->resi->LoadSharedResource(joinedname, bbuf, bsize);
			ASSERT(aret == bret);
			ASSERT(memcmp(abuf, bbuf, asize) == 0);
			free(abuf);
			free(bbuf);

			char *abuf2 = (char *)malloc(asize);
			uninit(abuf2, asize);
			uint32_t aret2 = a->resi->LoadSharedResource(apathbuffer, abuf2, asize);
			char *bbuf2 = (char *)malloc(asize);
			uninit(bbuf2, asize);
			uint32_t bret2 = b->resi->LoadSharedResource(apathbuffer, bbuf2, asize);
			ASSERT(aret2 == bret2);
			ASSERT(memcmp(abuf2, bbuf2, asize) == 0);

			free(abuf2);
			free(bbuf2);
		}
	}
}


void InterfaceAuditor::compare_screeni_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();
	ia->Refresh();
	ib->Refresh();

	vr::ScreenshotHandle_t handlea; uninit(handlea);
	vr::EVRScreenshotError rca = a->screeni->RequestScreenshot(
		&handlea,
		vr::VRScreenshotType_Stereo, "C:\vr_streams\\preview_screenshot", "C:\vr_streams\\final_screenshot");

	{
		vr::EVRScreenshotError errora;
		vr::EVRScreenshotType typea = a->screeni->GetScreenshotPropertyType(handlea, &errora);
	}
	{
		char preview_filename[256];
		uninit(preview_filename);
		vr::EVRScreenshotError errora; uninit(errora);
		uint32_t rca = a->screeni->GetScreenshotPropertyFilename(
			handlea, vr::VRScreenshotPropertyFilenames_Preview,
			preview_filename, sizeof(preview_filename), &errora);
	}

	{
		char vr_filename[256];
		uninit(vr_filename);
		vr::EVRScreenshotError errora; uninit(errora);
		uint32_t rca = a->screeni->GetScreenshotPropertyFilename(
			handlea, vr::VRScreenshotPropertyFilenames_VR,
			vr_filename, sizeof(vr_filename), &errora);
	}
}

void InterfaceAuditor::compare_exdi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();
	ia->Refresh();
	ib->Refresh();

	{
		int ax; uninit(ax);
		int ay; uninit(ay);
		uint32_t aw; uninit(aw);
		uint32_t ah; uninit(ah);
		a->exdi->GetWindowBounds(&ax, &ay, &aw, &ah);

		int bx;  uninit(bx);
		int by; uninit(by);
		uint32_t bw; uninit(bw);
		uint32_t bh; uninit(bh);
		b->exdi->GetWindowBounds(&bx, &by, &bw, &bh);
		ASSERT(ax == bx);
		ASSERT(ay == by);
		ASSERT(aw == bw);
		ASSERT(ah == bh);
	}

	{
		uint32_t ax; uninit(ax);
		uint32_t ay; uninit(ay);
		uint32_t aw; uninit(aw);
		uint32_t ah; uninit(ah);
		a->exdi->GetEyeOutputViewport(vr::Eye_Left, &ax, &ay, &aw, &ah);
		uint32_t bx; uninit(bx);
		uint32_t by; uninit(by);
		uint32_t bw; uninit(bw);
		uint32_t bh; uninit(bh);
		b->exdi->GetEyeOutputViewport(vr::Eye_Left, &bx, &by, &bw, &bh);
		ASSERT(ax == bx);
		ASSERT(ay == by);
		ASSERT(ah == bh);
		ASSERT(aw == bw);
	}

	{
		uint32_t ax; uninit(ax);
		uint32_t ay; uninit(ay);
		uint32_t aw; uninit(aw);
		uint32_t ah; uninit(ah);
		a->exdi->GetEyeOutputViewport(vr::Eye_Right, &ax, &ay, &aw, &ah);
		uint32_t bx; uninit(bx);
		uint32_t by; uninit(by);
		uint32_t bw; uninit(bw);
		uint32_t bh; uninit(bh);
		b->exdi->GetEyeOutputViewport(vr::Eye_Right, &bx, &by, &bw, &bh);
		ASSERT(ax == bx);
		ASSERT(ay == by);
		ASSERT(ah == bh);
		ASSERT(aw == bw);
	}
}

void InterfaceAuditor::process_overlay_events_on_handles(
	OpenVRInterfaceUnderTest *ia, std::vector<vr::VROverlayHandle_t> *h,
	OpenVRInterfaceUnderTest *ib, std::vector<vr::VROverlayHandle_t> *h2)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();

	for (auto handle : *h)
	{
		vr::VREvent_t event_a;
		bool brca = a->ovi->PollNextOverlayEvent(handle, &event_a, sizeof(vr::VREvent_t));

		if (brca)
		{
			//dprintf("%s\n", openvr_string::to_string(event_a).c_str());
			ia->PushOverlayEvent(handle, &event_a);
			ib->PushOverlayEvent(handle, &event_a);
		}
	}
}

void InterfaceAuditor::compare_per_overlay_handles(vr::VROverlayHandle_t overlay_handle_a,
	vr::VROverlayHandle_t overlay_handle_b,
	openvr_broker::open_vr_interfaces *a,
	openvr_broker::open_vr_interfaces *b)
{
	{
		uint32_t pid_a = a->ovi->GetOverlayRenderingPid(overlay_handle_a);
		uint32_t pid_b = b->ovi->GetOverlayRenderingPid(overlay_handle_b);
		ASSERT(pid_a == pid_b);
	}

	{
		std::vector<vr::VROverlayFlags> flags = {
			vr::VROverlayFlags_Curved,
			vr::VROverlayFlags_RGSS4X,
			vr::VROverlayFlags_NoDashboardTab,
			vr::VROverlayFlags_AcceptsGamepadEvents,
			vr::VROverlayFlags_ShowGamepadFocus,
			vr::VROverlayFlags_SendVRScrollEvents,
			vr::VROverlayFlags_SendVRTouchpadEvents,
			vr::VROverlayFlags_ShowTouchPadScrollWheel,
			vr::VROverlayFlags_TransferOwnershipToInternalProcess,
			vr::VROverlayFlags_SideBySide_Parallel,
			vr::VROverlayFlags_SideBySide_Crossed,
			vr::VROverlayFlags_Panorama,
			vr::VROverlayFlags_StereoPanorama,
			vr::VROverlayFlags_SortWithNonSceneOverlays,
			vr::VROverlayFlags_VisibleInDashboard
		};

		for (auto flag : flags)
		{
			bool enabled_a; uninit(enabled_a);
			bool enabled_b; uninit(enabled_b);
			vr::EVROverlayError reta = a->ovi->GetOverlayFlag(overlay_handle_a, flag, &enabled_a);
			vr::EVROverlayError retb = b->ovi->GetOverlayFlag(overlay_handle_b, flag, &enabled_b);
			ASSERT(enabled_a == enabled_b);
			ASSERT(reta == retb);
		}
	}

	{
		float ared;		uninit(ared);
		float agreen;	uninit(agreen);
		float ablue;	uninit(ablue);
		vr::EVROverlayError reta = a->ovi->GetOverlayColor(overlay_handle_a, &ared, &agreen, &ablue);

		float bred;		uninit(bred);
		float bgreen;	uninit(bgreen);
		float bblue;	uninit(bblue);
		vr::EVROverlayError retb = b->ovi->GetOverlayColor(overlay_handle_b, &bred, &bgreen, &bblue);

		ASSERT(ared == bred);
		ASSERT(agreen == bgreen);
		ASSERT(ablue == bblue);
		ASSERT(reta == retb);
	}

	{
		float aalpha; uninit(aalpha);
		vr::EVROverlayError reta = a->ovi->GetOverlayAlpha(overlay_handle_a, &aalpha);

		float balpha; uninit(balpha);
		vr::EVROverlayError retb = b->ovi->GetOverlayAlpha(overlay_handle_b, &balpha);
		ASSERT(reta == retb);
		ASSERT(aalpha == balpha);
	}

	{
		float texelaspecta; uninit(texelaspecta);
		float texelaspectb; uninit(texelaspectb);
		vr::EVROverlayError reta = a->ovi->GetOverlayTexelAspect(overlay_handle_a, &texelaspecta);
		vr::EVROverlayError retb = b->ovi->GetOverlayTexelAspect(overlay_handle_b, &texelaspectb);
		ASSERT(reta == retb);
		ASSERT(texelaspecta == texelaspectb);
	}

	{
		uint32_t sort_ordera; uninit(sort_ordera);
		vr::EVROverlayError reta = a->ovi->GetOverlaySortOrder(overlay_handle_a, &sort_ordera);
		uint32_t sort_orderb; uninit(sort_orderb);
		vr::EVROverlayError retb = b->ovi->GetOverlaySortOrder(overlay_handle_b, &sort_orderb);
		ASSERT(sort_ordera == sort_orderb);
		ASSERT(reta == retb);
	}

	{
		float width_in_metersa; uninit(width_in_metersa);
		float width_in_metersb; uninit(width_in_metersb);
		vr::EVROverlayError reta = a->ovi->GetOverlayWidthInMeters(overlay_handle_a, &width_in_metersa);
		vr::EVROverlayError retb = b->ovi->GetOverlayWidthInMeters(overlay_handle_b, &width_in_metersb);
		ASSERT(reta == retb);
		ASSERT(width_in_metersa == width_in_metersb);
	}

	{
		float MinDistanceInMetersa; uninit(MinDistanceInMetersa);
		float MaxDistanceInMetersa; uninit(MaxDistanceInMetersa);
		vr::EVROverlayError reta = a->ovi->GetOverlayAutoCurveDistanceRangeInMeters(overlay_handle_a,
			&MinDistanceInMetersa, &MaxDistanceInMetersa);

		float MinDistanceInMetersb; uninit(MinDistanceInMetersb);
		float MaxDistanceInMetersb; uninit(MaxDistanceInMetersb);
		vr::EVROverlayError retb = b->ovi->GetOverlayAutoCurveDistanceRangeInMeters(overlay_handle_b,
			&MinDistanceInMetersb, &MaxDistanceInMetersb);
		ASSERT(reta == retb);
		ASSERT(MinDistanceInMetersa == MinDistanceInMetersb);
		ASSERT(MaxDistanceInMetersb == MaxDistanceInMetersb);
	}

	{
		vr::EColorSpace color_spacea; uninit(color_spacea);
		vr::EVROverlayError reta = a->ovi->GetOverlayTextureColorSpace(overlay_handle_a, &color_spacea);

		vr::EColorSpace color_spaceb; uninit(color_spaceb);
		vr::EVROverlayError retb = b->ovi->GetOverlayTextureColorSpace(overlay_handle_b, &color_spaceb);
		ASSERT(color_spacea == color_spaceb);
		ASSERT(reta == retb);
	}

	{
		vr::VRTextureBounds_t texture_boundsa; uninit(texture_boundsa);
		vr::EVROverlayError reta = a->ovi->GetOverlayTextureBounds(overlay_handle_a, &texture_boundsa);

		vr::VRTextureBounds_t texture_boundsb; uninit(texture_boundsb);
		vr::EVROverlayError retb = b->ovi->GetOverlayTextureBounds(overlay_handle_b, &texture_boundsb);
		ASSERT(texture_boundsa == texture_boundsb);
		ASSERT(reta == retb);
	}

	{
		vr::VROverlayTransformType transform_typea; uninit(transform_typea);
		vr::EVROverlayError reta = a->ovi->GetOverlayTransformType(overlay_handle_a, &transform_typea);

		vr::VROverlayTransformType transform_typeb; uninit(transform_typeb);
		vr::EVROverlayError retb = b->ovi->GetOverlayTransformType(overlay_handle_b, &transform_typeb);
		ASSERT(transform_typea == transform_typeb);
		ASSERT(reta == retb);
	}

	{
		vr::ETrackingUniverseOrigin eTrackingOrigina; uninit(eTrackingOrigina);
		vr::HmdMatrix34_t			matTrackingOriginToOverlayTransforma; uninit(matTrackingOriginToOverlayTransforma);
		vr::EVROverlayError reta = a->ovi->GetOverlayTransformAbsolute(overlay_handle_a, &eTrackingOrigina, &matTrackingOriginToOverlayTransforma);

		vr::ETrackingUniverseOrigin eTrackingOriginb; uninit(eTrackingOriginb);;
		vr::HmdMatrix34_t			matTrackingOriginToOverlayTransformb; uninit(matTrackingOriginToOverlayTransformb);
		vr::EVROverlayError retb = b->ovi->GetOverlayTransformAbsolute(overlay_handle_b, &eTrackingOriginb, &matTrackingOriginToOverlayTransformb);

		ASSERT(reta == retb);
		ASSERT(eTrackingOrigina == eTrackingOriginb);
		ASSERT(softcompare_is_similar(matTrackingOriginToOverlayTransforma, matTrackingOriginToOverlayTransformb) == true);
	}

	{
		vr::TrackedDeviceIndex_t	unTrackedDevicea; uninit(unTrackedDevicea);
		vr::HmdMatrix34_t			matTrackedDeviceToOverlayTransforma; uninit(matTrackedDeviceToOverlayTransforma);
		vr::EVROverlayError reta = a->ovi->GetOverlayTransformTrackedDeviceRelative(overlay_handle_a,
			&unTrackedDevicea,
			&matTrackedDeviceToOverlayTransforma);

		vr::TrackedDeviceIndex_t	unTrackedDeviceb; uninit(unTrackedDeviceb);
		vr::HmdMatrix34_t			matTrackedDeviceToOverlayTransformb; uninit(matTrackedDeviceToOverlayTransformb);
		vr::EVROverlayError retb = b->ovi->GetOverlayTransformTrackedDeviceRelative(overlay_handle_b,
			&unTrackedDeviceb,
			&matTrackedDeviceToOverlayTransformb);
		ASSERT(reta == retb);
		ASSERT(unTrackedDevicea == unTrackedDeviceb);
		ASSERT(softcompare_is_similar(matTrackedDeviceToOverlayTransforma, matTrackedDeviceToOverlayTransformb) == true);
	}

	{
		vr::TrackedDeviceIndex_t device_indexa;
		// 2/17/2017 will return true without setting this
		uninit(device_indexa);
		char chComponentNamea[256];
		uninit(chComponentNamea);
		vr::EVROverlayError reta = a->ovi->GetOverlayTransformTrackedDeviceComponent(
			overlay_handle_a, &device_indexa, chComponentNamea, sizeof(chComponentNamea));

		vr::TrackedDeviceIndex_t device_indexb; 
		uninit(device_indexb);
		char chComponentNameb[256];
		uninit(chComponentNameb);
		vr::EVROverlayError retb = b->ovi->GetOverlayTransformTrackedDeviceComponent(
			overlay_handle_b, &device_indexb, chComponentNameb, sizeof(chComponentNameb));
		ASSERT(device_indexa == device_indexb);
		ASSERT(strcmp(chComponentNamea, chComponentNameb) == 0);
	}

	{
		vr::VROverlayInputMethod input_methoda; uninit(input_methoda);
		vr::EVROverlayError rca = a->ovi->GetOverlayInputMethod(overlay_handle_a, &input_methoda);
		vr::VROverlayInputMethod input_methodb; uninit(input_methodb);
		vr::EVROverlayError rcb = b->ovi->GetOverlayInputMethod(overlay_handle_b, &input_methodb);
		ASSERT(input_methoda == input_methodb);
		ASSERT(rca == rcb);
	}

	{
		vr::HmdVector2_t mouse_scalea; uninit(mouse_scalea);
		vr::EVROverlayError rca = a->ovi->GetOverlayMouseScale(overlay_handle_a, &mouse_scalea);

		vr::HmdVector2_t mouse_scaleb; uninit(mouse_scaleb);
		vr::EVROverlayError rcb = b->ovi->GetOverlayMouseScale(overlay_handle_a, &mouse_scaleb);

		ASSERT(mouse_scalea == mouse_scaleb);
		ASSERT(rca == rcb);
	}

	{
		bool rca = a->ovi->IsHoverTargetOverlay(overlay_handle_a);
		bool rcb = b->ovi->IsHoverTargetOverlay(overlay_handle_b);
		ASSERT(rca == rcb);
	}

	{
		uint32_t widtha; uninit(widtha);
		uint32_t heighta; uninit(heighta);
		vr::EVROverlayError rca = a->ovi->GetOverlayTextureSize(overlay_handle_a, &widtha, &heighta);

		uint32_t widthb; uninit(widthb);
		uint32_t heightb; uninit(heightb);
		vr::EVROverlayError rcb = b->ovi->GetOverlayTextureSize(overlay_handle_b, &widthb, &heightb);
		ASSERT(widtha == widthb);
		ASSERT(heighta == heightb);
		ASSERT(rca == rcb);
	}

	{
		bool rca = a->ovi->IsActiveDashboardOverlay(overlay_handle_a);
		bool rcb = b->ovi->IsActiveDashboardOverlay(overlay_handle_b);
		ASSERT(rca == rcb);
	}

	{
		uint32_t process_id_a; uninit(process_id_a);
		vr::EVROverlayError rca = a->ovi->GetDashboardOverlaySceneProcess(overlay_handle_a, &process_id_a);
		uint32_t process_id_b; uninit(process_id_b);
		vr::EVROverlayError rcb = b->ovi->GetDashboardOverlaySceneProcess(overlay_handle_b, &process_id_b);
		ASSERT(rca == rcb);
		ASSERT(process_id_a == process_id_b);
	}

	{
		vr::TrackedDeviceIndex_t device_index_a = a->ovi->GetPrimaryDashboardDevice();
		vr::TrackedDeviceIndex_t device_index_b = b->ovi->GetPrimaryDashboardDevice();
		ASSERT(device_index_a == device_index_b);
	}

	{
		vr::TrackedDeviceIndex_t device_index_a = a->ovi->IsDashboardVisible();
		vr::TrackedDeviceIndex_t device_index_b = b->ovi->IsDashboardVisible();
		ASSERT(device_index_a == device_index_b);
	}

	{
		char bufa[256];
		uninit(bufa);
		uint32_t rca = a->ovi->GetKeyboardText(bufa, sizeof(bufa));

		char bufb[256];
		uninit(bufb);
		uint32_t rcb = b->ovi->GetKeyboardText(bufb, sizeof(bufb));

		ASSERT(rcb == rca);
		ASSERT(strcmp(bufa, bufb) == 0);
	}
}

void InterfaceAuditor::compare_ovi_interfaces(
	OpenVRInterfaceUnderTest *ia, 
	OpenVRInterfaceUnderTest *ib, const TrackerConfig &c,
	bool read_only_tests)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();
	ia->Refresh();
	ib->Refresh();

	// create overlays per TrackerConfig
	for (int i = 0; i < c.num_overlays; i++)
	{
		vr::VROverlayHandle_t overlay_handle_a; uninit(overlay_handle_a);
		std::string friendly_name = std::string(c.overlay_keys[i]) + "friendly";
		vr::EVROverlayError errora = a->ovi->CreateOverlay(c.overlay_keys[i], friendly_name.c_str(), &overlay_handle_a);

		vr::VROverlayHandle_t overlay_handle_b; uninit(overlay_handle_b);
		vr::EVROverlayError errorb = b->ovi->CreateOverlay(c.overlay_keys[i], friendly_name.c_str(), &overlay_handle_b);
		ASSERT(errora == errorb);
	}

	// check that the handles can find their keys

	ia->Refresh();
	ib->Refresh();

	std::vector<vr::VROverlayHandle_t> a_handles;
	std::vector<vr::VROverlayHandle_t> b_handles;

	// find handles for keys
	for (int i = 0; i < c.num_overlays; i++)
	{
		vr::VROverlayHandle_t overlay_handle_a; uninit(overlay_handle_a);
		vr::EVROverlayError errora = a->ovi->FindOverlay(c.overlay_keys[i], &overlay_handle_a);

		vr::VROverlayHandle_t overlay_handle_b; uninit(overlay_handle_b);
		vr::EVROverlayError errorb = b->ovi->FindOverlay(c.overlay_keys[i], &overlay_handle_b);

		a_handles.push_back(overlay_handle_a);
		b_handles.push_back(overlay_handle_b);

		ASSERT(errora == errorb);
		ASSERT(overlay_handle_a == overlay_handle_b);
	}

	process_overlay_events_on_handles(ia, &a_handles, ib, &b_handles);

	for (int i = 0; i < c.num_overlays; i++)
	{
		char szbufa[256];
		uninit(szbufa);
		vr::EVROverlayError erra; uninit(erra);
		uint32_t a_ret = a->ovi->GetOverlayKey(a_handles[i], szbufa, sizeof(szbufa), &erra);

		char szbufb[256];
		uninit(szbufb);
		vr::EVROverlayError errb; uninit(errb);
		uint32_t b_ret = b->ovi->GetOverlayKey(b_handles[i], szbufb, sizeof(szbufb), &errb);

		ASSERT(erra == errb);
		ASSERT(strcmp(szbufa, szbufb) == 0);
		ASSERT(a_ret == b_ret);
	}

	// try an invalid handle 0x22 - expect error code
	{
		char szbufa[256];
		uninit(szbufa);
		vr::EVROverlayError erra; uninit(erra);
		uint32_t a_ret = a->ovi->GetOverlayKey(0x22, szbufa, sizeof(szbufa), &erra);

		char szbufb[256];
		uninit(szbufb);
		vr::EVROverlayError errb; uninit(errb);
		uint32_t b_ret = b->ovi->GetOverlayKey(0x22, szbufb, sizeof(szbufb), &errb);
		ASSERT(erra != vr::VROverlayError_None);
		ASSERT(errb != vr::VROverlayError_None);
		ASSERT(erra == errb);	// strict mode
		ASSERT(a_ret == b_ret);

	}

	// try an invalid name - expect error code
	{
		vr::VROverlayHandle_t overlay_handle_a; uninit(overlay_handle_a);
		vr::EVROverlayError errora = a->ovi->FindOverlay("invalid", &overlay_handle_a);

		vr::VROverlayHandle_t overlay_handle_b; uninit(overlay_handle_b);
		vr::EVROverlayError errorb = b->ovi->FindOverlay("invalid", &overlay_handle_b);

		ASSERT(errora == errorb);
		ASSERT(overlay_handle_a == overlay_handle_b);
	}

	if (!read_only_tests)
	{
		{
			// try deleting an existing handle ('a') and query - expect error code
			{
				vr::EVROverlayError destroyerrora = a->ovi->DestroyOverlay(a_handles[0]);
				char szbufa[256];
				uninit(szbufa);
				vr::EVROverlayError queryerra; uninit(queryerra);
				uint32_t a_ret = a->ovi->GetOverlayKey(a_handles[0], szbufa, sizeof(szbufa), &queryerra); // should be invalid

				vr::EVROverlayError destroyerrorb = b->ovi->DestroyOverlay(b_handles[0]);

				ia->Refresh();			// give b a chance to notice it's gone
				ib->Refresh();

				char szbufb[256];
				uninit(szbufb);
				vr::EVROverlayError queryerrb; uninit(queryerrb);
				uint32_t b_ret = b->ovi->GetOverlayKey(b_handles[0], szbufb, sizeof(szbufb), &queryerrb); // should be invalid
				ASSERT(a_ret == b_ret);
				ASSERT(queryerra == queryerrb);
			}
		}

		{
			// add it back in
			vr::VROverlayHandle_t overlay_handle_a; uninit(overlay_handle_a);
			vr::VROverlayHandle_t overlay_handle_b; uninit(overlay_handle_b);
			std::string friendly_name = std::string(c.overlay_keys[0]) + "friendly";
			vr::EVROverlayError errora = a->ovi->CreateOverlay(c.overlay_keys[0], friendly_name.c_str(), &overlay_handle_a);
			vr::EVROverlayError errorb = b->ovi->CreateOverlay(c.overlay_keys[0], friendly_name.c_str(), &overlay_handle_b);
			ASSERT(errora == errorb);


			vr::VROverlayHandle_t found_overlay_handle_a; uninit(found_overlay_handle_a);
			vr::EVROverlayError found_errora = a->ovi->FindOverlay(c.overlay_keys[0], &found_overlay_handle_a);

			// give b a chance to detect the new overlay
			ia->Refresh();
			ib->Refresh();

			vr::VROverlayHandle_t found_overlay_handle_b; uninit(found_overlay_handle_b);
			vr::EVROverlayError found_errorb = b->ovi->FindOverlay(c.overlay_keys[0], &found_overlay_handle_b);
			ASSERT(found_errora == found_errorb);
			ASSERT(found_overlay_handle_a == found_overlay_handle_b);

			a_handles[0] = found_overlay_handle_a;
			b_handles[0] = found_overlay_handle_b;
		}

		// check they can find their names
		for (int i = 0; i < c.num_overlays; i++)
		{
			char szbufa[256];
			uninit(szbufa);
			vr::EVROverlayError erra; uninit(erra);
			uint32_t a_ret = a->ovi->GetOverlayName(a_handles[i], szbufa, sizeof(szbufa), &erra);

			char szbufb[256];
			uninit(szbufb);
			vr::EVROverlayError errb; uninit(errb);
			uint32_t b_ret = b->ovi->GetOverlayName(b_handles[i], szbufb, sizeof(szbufb), &errb);

			ASSERT(erra == errb);
			ASSERT(strcmp(szbufa, szbufb) == 0);
			ASSERT(a_ret == b_ret);
		}

		// test with no textures assigned
		{
			for (int i = 0; i < c.num_overlays; i++)
			{
				uint32_t aw = 99, ah = 99;
				vr::EVROverlayError aimg_err = a->ovi->GetOverlayImageData(a_handles[i], nullptr, 0, &aw, &ah);

				uint32_t bw = 99, bh = 99;
				vr::EVROverlayError bimg_err = b->ovi->GetOverlayImageData(b_handles[i], nullptr, 0, &bw, &bh);
				ASSERT(aw == bw);
				ASSERT(ah == bh);
			}
		}
		// test with textures
		{
			uint32_t texture_w = 128;
			uint32_t texture_h = 64;
			uint32_t depth = 4;
			uint32_t tex_size = texture_w * texture_h * depth;
			char *tex = (char *)malloc(tex_size);
			char *texa = (char *)malloc(tex_size);
			char *texb = (char *)malloc(tex_size);
			for (uint32_t i = 0; i < tex_size; i++)
			{
				tex[i] = (char)i;
			}

			for (int i = 0; i < c.num_overlays; i++)
			{
				vr::EVROverlayError erra = a->ovi->SetOverlayRaw(a_handles[i], tex, texture_w, texture_h, depth);
				vr::EVROverlayError errb = b->ovi->SetOverlayRaw(b_handles[i], tex, texture_w, texture_h, depth);
				ia->Refresh();
				ib->Refresh();
				uint32_t a_wout; uninit(a_wout);
				uint32_t a_hout; uninit(a_hout);
				vr::EVROverlayError geterra = a->ovi->GetOverlayImageData(a_handles[i], texa, tex_size, &a_wout, &a_hout);
				uint32_t b_wout; uninit(b_wout);
				uint32_t b_hout; uninit(b_hout);
				vr::EVROverlayError geterrb = b->ovi->GetOverlayImageData(b_handles[i], texb, tex_size, &b_wout, &b_hout);

				ASSERT(geterra == geterrb);
				ASSERT(a_wout = b_wout);
				ASSERT(a_hout = b_hout);
				ASSERT(memcmp(tex, texa, tex_size) == 0);
				ASSERT(memcmp(tex, texb, tex_size) == 0);
			}

			free(tex);
			free(texa);
			free(texb);
		}
	}
	for (int i = 0; i < c.num_overlays; i++)
	{
		vr::VROverlayHandle_t handle_a = a_handles[i];
		vr::VROverlayHandle_t handle_b = b_handles[i];

		compare_per_overlay_handles(handle_a, handle_b, &ia->Get(), &ib->Get());
	}

}

void InterfaceAuditor::compare_compi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c,
	bool large_time_gap_override, bool read_only_tests)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	ia->Refresh();
	ib->Refresh();

	{
		vr::ETrackingUniverseOrigin aret = a->compi->GetTrackingSpace();
		vr::ETrackingUniverseOrigin bret = b->compi->GetTrackingSpace();
		ASSERT(aret == bret);
	}

	{
		vr::TrackedDevicePose_t render_posesa[vr::k_unMaxTrackedDeviceCount];
		vr::TrackedDevicePose_t game_posesa[vr::k_unMaxTrackedDeviceCount];
		uninit(render_posesa);
		uninit(game_posesa);
		vr::EVRCompositorError rca = a->compi->GetLastPoses(
			render_posesa, vr::k_unMaxTrackedDeviceCount,
			game_posesa, vr::k_unMaxTrackedDeviceCount);

		vr::TrackedDevicePose_t render_posesb[vr::k_unMaxTrackedDeviceCount];
		vr::TrackedDevicePose_t game_posesb[vr::k_unMaxTrackedDeviceCount];
		uninit(render_posesb);
		uninit(game_posesb);

		vr::EVRCompositorError rcb = b->compi->GetLastPoses(
			render_posesb, vr::k_unMaxTrackedDeviceCount,
			game_posesb, vr::k_unMaxTrackedDeviceCount);

		ASSERT(rca == rcb);
		ASSERT(softcompare_is_similar(render_posesa, render_posesb, vr::k_unMaxTrackedDeviceCount) == true);
		ASSERT(softcompare_is_similar(game_posesa, game_posesb, vr::k_unMaxTrackedDeviceCount) == true);

	}

	{
		ia->Refresh();
		ib->Refresh();
		vr::Compositor_FrameTiming timing_a;
		timing_a.m_nSize = sizeof(vr::Compositor_FrameTiming);
		bool rca = a->compi->GetFrameTiming(&timing_a, c.frame_timing_frames_ago);
		vr::Compositor_FrameTiming timing_b;
		timing_b.m_nSize = sizeof(vr::Compositor_FrameTiming);
		bool rcb = b->compi->GetFrameTiming(&timing_b, c.frame_timing_frames_ago);
		ASSERT(rca == rcb);
		if (!large_time_gap_override)
		{
			ASSERT(softcompare_is_similar(timing_a, timing_b));
		}
	}

	{
		vr::Compositor_FrameTiming *atimings;
		vr::Compositor_FrameTiming *btimings;
		atimings = (vr::Compositor_FrameTiming *)malloc(c.frame_timings_num_frames * sizeof(vr::Compositor_FrameTiming));
		btimings = (vr::Compositor_FrameTiming *)malloc(c.frame_timings_num_frames * sizeof(vr::Compositor_FrameTiming));
		for (int i = 0; i < (int)c.frame_timings_num_frames; i++)
		{
			atimings[i].m_nSize = sizeof(vr::Compositor_FrameTiming);
			btimings[i].m_nSize = sizeof(vr::Compositor_FrameTiming);
		}

		ia->Refresh();
		ib->Refresh();
		uint32_t reta = a->compi->GetFrameTimings(atimings, c.frame_timings_num_frames);
		uint32_t retb = b->compi->GetFrameTimings(btimings, c.frame_timings_num_frames);
		ASSERT(reta == retb);
		if (!large_time_gap_override)
		{
			for (int i = 0; i < (int)c.frame_timings_num_frames; i++)
			{
				ASSERT(softcompare_is_similar(atimings[i], btimings[i]));
			}
		}
		free(atimings);
		free(btimings);
	}

	{
		ia->Refresh();
		ib->Refresh();
		float aret = a->compi->GetFrameTimeRemaining();
		float bret = b->compi->GetFrameTimeRemaining();
		if (!large_time_gap_override)
		{
			ASSERT(softcompare_is_similar(aret, bret, 0.5f));
		}
	}

	{
		vr::Compositor_CumulativeStats a_stats;
		vr::Compositor_CumulativeStats b_stats;
		uninit(a_stats);
		uninit(b_stats);
		a->compi->GetCumulativeStats(&a_stats, sizeof(vr::Compositor_CumulativeStats));
		b->compi->GetCumulativeStats(&b_stats, sizeof(vr::Compositor_CumulativeStats));
		ASSERT(softcompare_is_similar(a_stats, b_stats));
	}
	for (int i = 0; i < 2; i++)
	{
		if (i == 1)	// on second pass start fading the grid
		{
			a->compi->FadeGrid(1, true);
			b->compi->FadeGrid(1, true);
			ia->Refresh();
			ib->Refresh();
		}
		{
			vr::HmdColor_t color_a = a->compi->GetCurrentFadeColor(false);
			vr::HmdColor_t color_b = b->compi->GetCurrentFadeColor(false);
			ASSERT(softcompare_is_similar(color_a, color_b, 0.01f));
		}
		{
			vr::HmdColor_t color_a = a->compi->GetCurrentFadeColor(true);
			vr::HmdColor_t color_b = b->compi->GetCurrentFadeColor(true);
			ASSERT(softcompare_is_similar(color_a, color_b, 0.01f));
		}

		float alpha_a = a->compi->GetCurrentGridAlpha();
		float alpha_b = b->compi->GetCurrentGridAlpha();
		ASSERT(softcompare_is_similar(alpha_a, alpha_b, .01f));
	}
	{
		bool rca = a->compi->IsFullscreen();
		bool rcb = b->compi->IsFullscreen();
		ASSERT(rca == rcb);
	}
	{
		uint32_t rca = a->compi->GetCurrentSceneFocusProcess();
		uint32_t rcb = b->compi->GetCurrentSceneFocusProcess();
		ASSERT(rca == rcb);
	}

	{
		uint32_t rca = a->compi->GetLastFrameRenderer();
		uint32_t rcb = b->compi->GetLastFrameRenderer();
		ASSERT(rca == rcb);
	}

	{
		bool rca = a->compi->CanRenderScene();
		bool rcb = b->compi->CanRenderScene();
		ASSERT(rca == rcb);
	}

	{
		bool rca = a->compi->IsMirrorWindowVisible();
		bool rcb = b->compi->IsMirrorWindowVisible();
		ASSERT(rca == rcb);
	}

	{
		bool rca = a->compi->ShouldAppRenderWithLowResources();
		bool rcb = b->compi->ShouldAppRenderWithLowResources();
		ASSERT(rca == rcb);
	}
}

void InterfaceAuditor::compare_chapi_interfaces(
	OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, 
	const TrackerConfig &c,
	bool large_time_gap_override, bool read_only_tests)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	{
		vr::ChaperoneCalibrationState reta = a->chapi->GetCalibrationState();
		vr::ChaperoneCalibrationState retb = b->chapi->GetCalibrationState();
		ASSERT(reta == retb);
	}

	{
		float sizexa; uninit(sizexa);
		float sizeza; uninit(sizeza);
		bool reta = a->chapi->GetPlayAreaSize(&sizexa, &sizeza);

		float sizexb; uninit(sizexb);
		float sizezb; uninit(sizezb);
		bool retb = b->chapi->GetPlayAreaSize(&sizexb, &sizezb);
		ASSERT(reta == retb);
		ASSERT(sizexa == sizexb);
		ASSERT(sizeza == sizezb);
	}

	{
		vr::HmdQuad_t quada; uninit(quada);
		bool reta = a->chapi->GetPlayAreaRect(&quada);

		vr::HmdQuad_t quadb; uninit(quadb);
		bool retb = b->chapi->GetPlayAreaRect(&quadb);
		ASSERT(reta == retb);
		ASSERT(quada == quadb);
	}


	{
		vr::HmdColor_t *color_array_a = (vr::HmdColor_t *)malloc(sizeof(vr::HmdColor_t) * c.num_bounds_colors);
		vr::HmdColor_t camera_a; uninit(camera_a);
		a->chapi->GetBoundsColor(color_array_a, c.num_bounds_colors, c.collision_bounds_fade_distance, &camera_a);

		ib->Refresh();

		vr::HmdColor_t *color_array_b = (vr::HmdColor_t *)malloc(sizeof(vr::HmdColor_t) * c.num_bounds_colors);
		vr::HmdColor_t camera_b; uninit(camera_b);
		b->chapi->GetBoundsColor(color_array_b, c.num_bounds_colors, c.collision_bounds_fade_distance, &camera_b);

		if (!large_time_gap_override)
		{
			ASSERT(softcompare_is_similar(color_array_a, color_array_b, c.num_bounds_colors, 0.01f));
			ASSERT(softcompare_is_similar(camera_a, camera_b, 0.01f));
		}

		free(color_array_a);
		free(color_array_b);
	}

	{
		bool rca = a->chapi->AreBoundsVisible();
		bool rcb = b->chapi->AreBoundsVisible();
		ASSERT(rca == rcb);
	}

	if (!read_only_tests)
	{
		a->chapi->ForceBoundsVisible(true);
		b->chapi->ForceBoundsVisible(true);
		ia->Refresh();
		ib->Refresh();
		bool rca = a->chapi->AreBoundsVisible();
		bool rcb = b->chapi->AreBoundsVisible();
		ASSERT(rca == rcb);

		vr::HmdColor_t *color_array_a = (vr::HmdColor_t *)malloc(sizeof(vr::HmdColor_t) * c.num_bounds_colors);
		vr::HmdColor_t camera_a; uninit(camera_a);
		a->chapi->GetBoundsColor(color_array_a, c.num_bounds_colors, c.collision_bounds_fade_distance, &camera_a);

		vr::HmdColor_t *color_array_b = (vr::HmdColor_t *)malloc(sizeof(vr::HmdColor_t) * c.num_bounds_colors);
		vr::HmdColor_t camera_b; uninit(camera_b);
		b->chapi->GetBoundsColor(color_array_b, c.num_bounds_colors, c.collision_bounds_fade_distance, &camera_b);

		ASSERT(softcompare_is_similar(color_array_a, color_array_b, c.num_bounds_colors, 0.05f));
		ASSERT(softcompare_is_similar(camera_a, camera_b, 0.01f));
		free(color_array_a);
		free(color_array_b);
	}
}

void InterfaceAuditor::compare_chapsi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib,
	bool read_only_tests)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	{
		float sizexa; uninit(sizexa);
		float sizeza; uninit(sizeza);
		vr::HmdQuad_t working_recta; uninit(working_recta);
		bool rca = a->chapsi->GetWorkingPlayAreaSize(&sizexa, &sizeza);
		bool rca1 = a->chapsi->GetWorkingPlayAreaRect(&working_recta);

		float sizexb; uninit(sizexb);
		float sizezb; uninit(sizezb);
		vr::HmdQuad_t working_rectb; uninit(working_rectb);
		bool rcb = b->chapsi->GetWorkingPlayAreaSize(&sizexb, &sizezb);
		bool rcb1 = b->chapsi->GetWorkingPlayAreaRect(&working_rectb);

		ASSERT(rca == rcb);
		ASSERT(rcb1 == rca1);
		ASSERT(sizexa == sizexb);
		ASSERT(sizeza == sizezb);
		ASSERT(working_rectb == working_recta);

		if (!read_only_tests)
		{
			a->chapsi->SetWorkingPlayAreaSize(20.0f, 40.0f);
			b->chapsi->SetWorkingPlayAreaSize(20.0f, 40.0f);

			ia->Refresh();
			ib->Refresh();
			float sizexaa; uninit(sizexaa);
			float sizezaa; uninit(sizezaa);
			bool rcaa = a->chapsi->GetWorkingPlayAreaSize(&sizexaa, &sizezaa);

			float sizexbb; uninit(sizexbb);
			float sizezbb; uninit(sizezbb);
			bool rcbb = b->chapsi->GetWorkingPlayAreaSize(&sizexbb, &sizezbb);

			ASSERT(rcaa == rcbb);
			ASSERT(sizexaa == sizexbb);
			ASSERT(sizezaa == sizezbb);
			ASSERT(sizexaa == 20.0f);	// if this fails check your vrheadset
			ASSERT(sizezbb == 40.0f);
		}
	}


	{
		vr::HmdMatrix34_t seated2rawa;
		uninit(seated2rawa);
		bool rca = a->chapsi->GetWorkingSeatedZeroPoseToRawTrackingPose(&seated2rawa);

		vr::HmdMatrix34_t seated2rawb;
		uninit(seated2rawb);
		bool rcb = a->chapsi->GetWorkingSeatedZeroPoseToRawTrackingPose(&seated2rawb);

		ASSERT(rca == rcb);
		ASSERT(memcmp(&seated2rawa, &seated2rawb, sizeof(seated2rawa)) == 0);
	}

	{
		vr::HmdMatrix34_t standing2rawa; uninit(standing2rawa);
		bool rca = a->chapsi->GetWorkingStandingZeroPoseToRawTrackingPose(&standing2rawa);

		vr::HmdMatrix34_t standing2rawb; uninit(standing2rawb);
		bool rcb = a->chapsi->GetWorkingStandingZeroPoseToRawTrackingPose(&standing2rawb);

		ASSERT(rca == rcb);
		ASSERT(memcmp(&standing2rawa, &standing2rawb, sizeof(standing2rawa)) == 0);
	}


	int num_iterations = 2;
	if (read_only_tests)
	{
		num_iterations = 1;
	}
	for (int i = 0; i < num_iterations; i++)
	{
		if (i == 1)	// write test
		{
			vr::HmdQuad_t quad;
			memset(&quad, 0, sizeof(quad));	// this is the test
			a->chapsi->SetWorkingCollisionBoundsInfo(&quad, 1);
			ia->Refresh();
			ib->Refresh();
		}

		uint32_t quads_counta; uninit(quads_counta);
		bool rca = a->chapsi->GetWorkingCollisionBoundsInfo(nullptr, &quads_counta);
		uint32_t quads_countb; uninit(quads_countb);
		bool rcb = b->chapsi->GetWorkingCollisionBoundsInfo(nullptr, &quads_countb);
		ASSERT(rca == rcb);
	}

	{
		uint32_t quad_counta = 0; // test zero
		bool rca = a->chapsi->GetLiveCollisionBoundsInfo(nullptr, &quad_counta);
		uint32_t quad_countb = 0; // test zero
		bool rcb = b->chapsi->GetLiveCollisionBoundsInfo(nullptr, &quad_countb);
		ASSERT(rca == rcb);
		WAIVE(quad_counta == quad_countb, "live collision bounds info is wacky");

		// insane count and nullptr
		uint32_t quad_countaa = 99;
		bool rcaa = a->chapsi->GetLiveCollisionBoundsInfo(nullptr, &quad_countaa);
		uint32_t quad_countbb = 99;
		bool rcbb = b->chapsi->GetLiveCollisionBoundsInfo(nullptr, &quad_countbb);
		ASSERT(quad_countaa == quad_countbb);

	}
	{
		uint32_t count_guess = 1000;
		vr::HmdQuad_t *quadsa = (vr::HmdQuad_t *)calloc(count_guess, sizeof(vr::HmdQuad_t));
		bool rcaa = a->chapsi->GetLiveCollisionBoundsInfo(quadsa, &count_guess); 

		count_guess = 1000;
		vr::HmdQuad_t *quadsb = (vr::HmdQuad_t *)calloc(count_guess, sizeof(vr::HmdQuad_t));
		bool rcbb = b->chapsi->GetLiveCollisionBoundsInfo(quadsb, &count_guess);

		WAIVE(rcaa == rcbb, "live collision waiver");
		WAIVE(memcmp(quadsa, quadsb, (sizeof(vr::HmdQuad_t) * count_guess)) == 0, "live collision waiver");
		free(quadsa);
		free(quadsb);
	}

	{
		// test zero
		uint32_t quad_counta = 0;
		bool rca = a->chapsi->GetLivePhysicalBoundsInfo(nullptr, &quad_counta);

		// insane count and nullptr
		uint32_t quad_countaa = 99;
		bool rca1 = b->chapsi->GetLivePhysicalBoundsInfo(nullptr, &quad_countaa);

		vr::HmdQuad_t *quadsa = (vr::HmdQuad_t *)calloc(quad_counta, sizeof(vr::HmdQuad_t));

		bool rcaa = a->chapsi->GetLivePhysicalBoundsInfo(quadsa, &quad_counta); // this function seems foobared - does not return count

		uint32_t quad_countb = 0;
		bool rcb = b->chapsi->GetLivePhysicalBoundsInfo(nullptr, &quad_countb);

		// give an insane count and nullptr 
		uint32_t quad_countbb = 99;
		bool rcb1 = b->chapsi->GetLivePhysicalBoundsInfo(nullptr, &quad_countbb);

		vr::HmdQuad_t *quadsb = (vr::HmdQuad_t *)calloc(quad_counta, sizeof(vr::HmdQuad_t));
		bool rcbb = a->chapsi->GetLivePhysicalBoundsInfo(quadsb, &quad_counta);

		ASSERT(rca == rcb);
		ASSERT(rcaa == rcbb);
		ASSERT(quad_countaa == quad_countbb);
		ASSERT(quad_counta == quad_countb);
		ASSERT(memcmp(quadsa, quadsb, (sizeof(vr::HmdQuad_t) * quad_counta)) == 0);
		free(quadsa);
		free(quadsb);
	}

	{
		uint32_t uint_counta = 0;
		bool rca = a->chapsi->GetLiveCollisionBoundsTagsInfo(nullptr, &uint_counta);

		// insane count and nullptr
		uint32_t uint_countaa = 99;
		bool rca1 = b->chapsi->GetLiveCollisionBoundsTagsInfo(nullptr, &uint_countaa);
		uint8_t *uintsa = (uint8_t *)calloc(uint_counta, sizeof(uint8_t));
		bool rcaa = a->chapsi->GetLiveCollisionBoundsTagsInfo(uintsa, &uint_counta); // this function seems foobared - does not return count

		uint32_t uint_countb = 0;
		bool rcb = b->chapsi->GetLiveCollisionBoundsTagsInfo(nullptr, &uint_countb);

		// give an insane count and nullptr 
		uint32_t uint_countbb = 99;
		bool rcb1 = b->chapsi->GetLiveCollisionBoundsTagsInfo(nullptr, &uint_countbb);

		uint8_t *uintsb = (uint8_t *)calloc(uint_counta, sizeof(uint8_t));
		bool rcbb = a->chapsi->GetLiveCollisionBoundsTagsInfo(uintsb, &uint_counta);

		ASSERT(rca == rcb);
		ASSERT(rcaa == rcbb);
		ASSERT(uint_countaa == uint_countbb);
		ASSERT(uint_counta == uint_countb);
		ASSERT(memcmp(uintsa, uintsb, (sizeof(uint8_t) * uint_counta)) == 0);
		free(uintsa);
		free(uintsb);
	}
}

// test settings interfaces by getting and setting values to see that a interfaces behave the same as b interfaces
void InterfaceAuditor::compare_seti_interfaces(
	OpenVRInterfaceUnderTest *ia, 
	OpenVRInterfaceUnderTest *ib, 
	const TrackerConfig &c,
	bool read_only_tests)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	// check steam floats
	std::vector<const char *> steam_floats =
	{
		vr::k_pch_SteamVR_IPD_Float,
		vr::k_pch_SteamVR_BackgroundCameraHeight_Float,
		vr::k_pch_SteamVR_BackgroundDomeRadius_Float,
		vr::k_pch_SteamVR_SpeakersForwardYawOffsetDegrees_Float,
		vr::k_pch_SteamVR_RenderTargetMultiplier_Float,
		vr::k_pch_SteamVR_IpdOffset_Float
	};

	for (auto key : steam_floats)
	{
		vr::EVRSettingsError errora; uninit(errora);
		float fa = a->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &errora);

		vr::EVRSettingsError errorb; uninit(errorb);
		float fb = b->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &errorb);

		ASSERT(fa == fb);
		ASSERT(errora == errorb);

		if (!read_only_tests)
		{
			// change it
			float new_val = fa + 1.0f;

			vr::EVRSettingsError write1errora; uninit(write1errora);
			vr::EVRSettingsError write1errorb; uninit(write1errorb);
			a->seti->SetFloat(vr::k_pch_SteamVR_Section, key, new_val, &write1errora);
			b->seti->SetFloat(vr::k_pch_SteamVR_Section, key, new_val, &write1errorb);	// writes to stub
			ia->Refresh();
			ib->Refresh();
			// read it again
			vr::EVRSettingsError read1errora; uninit(read1errora);
			vr::EVRSettingsError read1errorb; uninit(read1errorb);
			float fa1 = a->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &read1errora);
			float fb1 = b->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &read1errorb);	// reads from snapshot of a


																							// write the old version back
			vr::EVRSettingsError write2errora; uninit(write2errora);
			vr::EVRSettingsError write2errorb; uninit(write2errorb);
			a->seti->SetFloat(vr::k_pch_SteamVR_Section, key, fa, &write2errora);
			b->seti->SetFloat(vr::k_pch_SteamVR_Section, key, fa, &write2errorb);	// writes to stub
			ia->Refresh();
			ib->Refresh();

			vr::EVRSettingsError read2errora; uninit(read2errora);
			vr::EVRSettingsError read2errorb; uninit(read2errorb);
			// check it again
			float finala = a->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &read2errora);
			float finalb = b->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &read2errorb);	// reads from snapshot of a

			ASSERT(write1errora == write1errorb);
			ASSERT(read1errora == read1errorb);
			ASSERT(read1errora == read1errorb);
			ASSERT(fa1 == fb1);
			ASSERT(fa1 == new_val);

			ASSERT(write2errora == write2errorb);
			ASSERT(read2errora == read2errorb);

			ASSERT(finala == finalb);
			ASSERT(finala == fa);
		}
	}

	// try a non-existant setting
	{
		float fa; uninit(fa);
		vr::EVRSettingsError readerrora; uninit(readerrora);
		fa = a->seti->GetFloat("unittest_section", "nonexistant", &readerrora);

		float fb; uninit(fb);
		vr::EVRSettingsError readerrorb; uninit(readerrorb);
		fb = b->seti->GetFloat("unittest_section", "nonexistant", &readerrorb);

		ASSERT(readerrora == readerrorb);
		ASSERT(fa == fb);
	}

	if (!read_only_tests)
	{
		// check custom values
		float f = 22.0f;
		vr::EVRSettingsError write2errora; uninit(write2errora);
		a->seti->SetFloat("unittest_section", "unit_test_key", f, &write2errora);

		vr::EVRSettingsError write2errorb; uninit(write2errorb);
		b->seti->SetFloat("unittest_section", "unit_test_key", f, &write2errorb);

		float fa; uninit(fa);
		vr::EVRSettingsError readerrora; uninit(readerrora);
		fa = a->seti->GetFloat("unittest_section", "unit_test_key", &readerrora);

		float fb; uninit(fb);
		vr::EVRSettingsError readerrorb; uninit(readerrorb);
		fb = b->seti->GetFloat("unittest_section", "unit_test_key", &readerrorb);

		ASSERT(fa == f);
		ASSERT(fb == f);
		ASSERT(readerrora == readerrorb);
	}
}

void InterfaceAuditor::compare_apps(vr::IVRApplications *a_appi, vr::IVRApplications *b_appi)
{
	uint32_t counta1 = a_appi->GetApplicationCount();
	uint32_t countb1 = b_appi->GetApplicationCount();
	ASSERT(counta1 == countb1);
	for (int i = 0; i < (int)counta1; i++)
	{
		char bufa[256];
		uninit(bufa);
		char bufb[256];
		uninit(bufb);
		vr::EVRApplicationError erra = a_appi->GetApplicationKeyByIndex(i, bufa, sizeof(bufa));
		vr::EVRApplicationError errb = b_appi->GetApplicationKeyByIndex(i, bufb, sizeof(bufb));
		ASSERT(erra == errb);
		ASSERT(strcmp(bufa, bufb) == 0);
	}
}

// add some apps,
// remove some apps
// add them back in

static void write_manifest(std::string filename, const std::vector<std::string> &app_keys)
{
	FILE *pf = fopen(filename.c_str(), "wt");
	assert(pf);

	fprintf(pf, "{\n");
	fprintf(pf, "\"applications\": [\n");
	for (int i = 0; i < (int)app_keys.size(); i++)
	{
		fprintf(pf, "{\n");

		fprintf(pf, "  \"app_key\" : \"%s\",\n", app_keys[i].c_str());
		fprintf(pf, "  \"launch_type\" : \"binary\",\n");
		fprintf(pf, "  \"binary_path_windows\" : \"x.exe\",\n");
		fprintf(pf, "  \"binary_path_osx\" : \"x\",\n");
		fprintf(pf, "  \"binary_path_linux\" : \"x\",\n");
		fprintf(pf, "  \"arguments\" : \"\",\n");
		fprintf(pf, "  \"image_path\" : \"\"\n");


		if (i < (int)app_keys.size() - 1)
		{
			fprintf(pf, "},\n");
		}
		else
		{
			fprintf(pf, "}\n");
		}
	}
	fprintf(pf, "]}\n");
	fclose(pf);
}


// broadly tests all get interfaces to make sure they behave
// the same 
void InterfaceAuditor::compare_appi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	{
		bool ba = a->appi->IsApplicationInstalled(nullptr);
		bool bb = b->appi->IsApplicationInstalled(nullptr);
		ASSERT(ba == bb);
	}


	ia->Refresh();
	ib->Refresh();


	// try adding a temporary app manifest
	// to add apps, use a unique filename and list a bunch of things in it
	// to remove apps, use the same filename and remove the apps from it


	{
		// notes on app manifests
		// https://steamcommunity.com/app/358720/discussions/0/357284767245532150/?l=norwegian

		//const char *manifest_path = "D:\\Games\\Steam\\steamapps\\common\\SteamVR\\tools\\seantools.vrmanifest";

		const char *manifest_path = "C:\\vr_streams\\unittest.vrmanifest";
		std::vector<std::string> twokeys = { "appkey1", "appkey2" };
		write_manifest(manifest_path, twokeys);
		vr::EVRApplicationError erra0 = a->appi->AddApplicationManifest(manifest_path, true);
		vr::EVRApplicationError errb0 = b->appi->AddApplicationManifest(manifest_path, true);
		ASSERT(erra0 == errb0);

		ia->Refresh();
		ib->Refresh();
		compare_apps(a->appi, b->appi);

		std::vector<std::string> nokeys;
		write_manifest(manifest_path, nokeys);
		vr::EVRApplicationError erra1 = a->appi->AddApplicationManifest(manifest_path, true);
		vr::EVRApplicationError errb1 = b->appi->AddApplicationManifest(manifest_path, true);
		ASSERT(erra1 == errb1);

		ia->Refresh();
		ib->Refresh();
		compare_apps(a->appi, b->appi);

		write_manifest(manifest_path, twokeys);
		vr::EVRApplicationError erra2 = a->appi->AddApplicationManifest(manifest_path, true);
		vr::EVRApplicationError errb2 = b->appi->AddApplicationManifest(manifest_path, true);
		ASSERT(erra2 == errb2);
		ia->Refresh();
		ib->Refresh();
		compare_apps(a->appi, b->appi);
	}

	// there are race conditions in the application count
	{
		bool succeeded = false;
		for (int i = 0; i < 5; i++)
		{
			uint32_t counta = a->appi->GetApplicationCount();
			uint32_t countb = b->appi->GetApplicationCount();
			if (counta == countb)
			{
				succeeded = true;
				break;
			}
			else
			{
				ia->Refresh();
				ib->Refresh();
			}
		}
		ASSERT(succeeded);
	}

	{

		uint32_t weirda = a->appi->GetApplicationCount();

		bool ba = a->appi->IsApplicationInstalled("smursmur");
		bool bb = b->appi->IsApplicationInstalled("smursmur");
		ASSERT(ba == bb);

		uint32_t weirdb = a->appi->GetApplicationCount();
		ASSERT(weirda == weirdb);
		uint32_t weirdc = a->appi->GetApplicationCount();
		ASSERT(weirdc == weirdb);


	}

	ia->Refresh();
	ib->Refresh();

	{
		uint32_t counta = a->appi->GetApplicationCount();
		uint32_t countb = b->appi->GetApplicationCount();
		//ASSERT(counta == countb);
	}

	{
		for (int i = 0; i < (int)a->appi->GetApplicationCount(); i++)
		{
			vr::EVRApplicationError errora = a->appi->GetApplicationKeyByIndex(i, nullptr, 0);
			vr::EVRApplicationError errorb = b->appi->GetApplicationKeyByIndex(i, nullptr, 0);
			ASSERT(errora == errorb);
		}
	}

	{
		for (int i = 0; i < (int)a->appi->GetApplicationCount(); i++)
		{
			char bufa[1];
			bufa[0] = 0;
			vr::EVRApplicationError errora = a->appi->GetApplicationKeyByIndex(i, bufa, 1);
			char bufb[1];
			bufb[0] = 0;
			vr::EVRApplicationError errorb = b->appi->GetApplicationKeyByIndex(i, bufb, 1);
			ASSERT(errora == errorb);
			ASSERT(bufa[0] == bufb[0]);
		}
	}

	{
		for (int i = 0; i < (int)a->appi->GetApplicationCount(); i++)
		{
			char bufa[256];
			bufa[0] = 0;
			vr::EVRApplicationError errora = a->appi->GetApplicationKeyByIndex(i, bufa, 256);
			uint32_t aprocess_id = a->appi->GetApplicationProcessId(bufa);
			bool autolaunch_a = a->appi->GetApplicationAutoLaunch(bufa);
			char mimesupportastr[512];
			mimesupportastr[0] = 0;
			bool mimesupportboola;
			mimesupportboola = a->appi->GetApplicationSupportedMimeTypes(bufa, mimesupportastr, sizeof(mimesupportastr));


			char bufb[256];
			bufb[0] = 0;
			vr::EVRApplicationError errorb = b->appi->GetApplicationKeyByIndex(i, bufb, 256);
			uint32_t bprocess_id = b->appi->GetApplicationProcessId(bufb);
			bool autolaunch_b = b->appi->GetApplicationAutoLaunch(bufb);

			char mimesupportbstr[512];
			mimesupportbstr[0] = 0;
			bool mimesupportboolb;
			mimesupportboolb = b->appi->GetApplicationSupportedMimeTypes(bufb, mimesupportbstr, sizeof(mimesupportbstr));


			ASSERT(errora == errorb);
			ASSERT(strcmp(bufa, bufb) == 0);
			ASSERT(aprocess_id == bprocess_id);
			ASSERT(autolaunch_a == autolaunch_b);
			ASSERT(mimesupportboola == mimesupportboolb);
			ASSERT(strcmp(mimesupportastr, mimesupportbstr) == 0);

			{
				std::vector<vr::EVRApplicationProperty> string_properties =
				{
					vr::VRApplicationProperty_Name_String,
					vr::VRApplicationProperty_LaunchType_String,
					vr::VRApplicationProperty_WorkingDirectory_String,
					vr::VRApplicationProperty_BinaryPath_String,
					vr::VRApplicationProperty_Arguments_String,
					vr::VRApplicationProperty_URL_String,
					vr::VRApplicationProperty_Description_String,
					vr::VRApplicationProperty_NewsURL_String,
					vr::VRApplicationProperty_ImagePath_String,
					vr::VRApplicationProperty_Source_String
				};

				for (auto prop : string_properties)
				{
					char outa[256]; uninit(outa);
					char outb[256]; uninit(outb);
					vr::EVRApplicationError errora; uninit(errora);
					vr::EVRApplicationError errorb; uninit(errorb);
					uint32_t reta = a->appi->GetApplicationPropertyString(bufa, prop, outa, sizeof(outa), &errora);
					uint32_t retb = b->appi->GetApplicationPropertyString(bufb, prop, outb, sizeof(outb), &errorb);
					ASSERT(reta == retb);
					ASSERT(errora == errorb);
					ASSERT(strcmp(outa, outb) == 0);
				}
			}

			{
				std::vector<vr::EVRApplicationProperty> bool_properties =
				{
					vr::VRApplicationProperty_IsDashboardOverlay_Bool,
					vr::VRApplicationProperty_IsTemplate_Bool,
					vr::VRApplicationProperty_IsInstanced_Bool,
					vr::VRApplicationProperty_IsInternal_Bool
				};

				for (auto prop : bool_properties)
				{
					vr::EVRApplicationError errora; uninit(errora);
					vr::EVRApplicationError errorb; uninit(errorb);
					bool reta = a->appi->GetApplicationPropertyBool(bufa, prop, &errora);
					bool retb = b->appi->GetApplicationPropertyBool(bufb, prop, &errorb);
					ASSERT(reta == retb);
					ASSERT(errora == errorb);
				}
			}
			{
				std::vector<vr::EVRApplicationProperty> uint64_properties =
				{
					vr::VRApplicationProperty_LastLaunchTime_Uint64,
				};

				for (auto prop : uint64_properties)
				{
					vr::EVRApplicationError errora; uninit(errora);
					vr::EVRApplicationError errorb; uninit(errorb);
					uint64_t reta = a->appi->GetApplicationPropertyUint64(bufa, prop, &errora);
					uint64_t retb = b->appi->GetApplicationPropertyUint64(bufb, prop, &errorb);
					ASSERT(reta == retb);
					ASSERT(errora == errorb);
				}
			}
		}
	}

	{
		vr::EVRApplicationTransitionState sta = a->appi->GetTransitionState();
		vr::EVRApplicationTransitionState stb = b->appi->GetTransitionState();
		ASSERT(sta == stb);
	}

	{
		uint32_t currenta = a->appi->GetCurrentSceneProcessId();
		uint32_t currentb = b->appi->GetCurrentSceneProcessId();
		ASSERT(currenta == currentb);
	}
}

void InterfaceAuditor::compare_sysi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	ia->Refresh();
	ib->Refresh();

	compare_apply_transforms(ia, ib);

	{
		uint32_t awidth; uninit(awidth);
		uint32_t aheight; uninit(aheight);
		uint32_t bwidth; uninit(bwidth);
		uint32_t bheight; uninit(bheight);
		a->sysi->GetRecommendedRenderTargetSize(&awidth, &aheight);
		b->sysi->GetRecommendedRenderTargetSize(&bwidth, &bheight);
		ASSERT(awidth == bwidth);
		ASSERT(aheight == bheight);

		// openvr crashes
		//a->sysi->GetRecommendedRenderTargetSize(nullptr, &aheight);
		//b->sysi->GetRecommendedRenderTargetSize(nullptr, &bheight);
		//ASSERT(awidth == bwidth);
		//ASSERT(aheight == bheight);
		//a->sysi->GetRecommendedRenderTargetSize(&awidth, nullptr);
		//b->sysi->GetRecommendedRenderTargetSize(&bwidth, nullptr);
		//ASSERT(awidth == bwidth);
		//ASSERT(aheight == bheight);
	}

	{
		for (int i = 0; i < 2; i++)
		{
			vr::HmdMatrix44_t mata = a->sysi->GetProjectionMatrix(vr::EVREye(i), c.nearz, c.farz);
			vr::HmdMatrix44_t matb = b->sysi->GetProjectionMatrix(vr::EVREye(i), c.nearz, c.farz);
			ASSERT(mata == matb);

			float aa[4];
			a->sysi->GetProjectionRaw(vr::EVREye(i), &aa[0], &aa[1], &aa[2], &aa[3]);
			float bb[4];
			b->sysi->GetProjectionRaw(vr::EVREye(i), &bb[0], &bb[1], &bb[2], &bb[3]);
			ASSERT(memcmp(aa, bb, sizeof(aa[0] * 4)) == 0);

			vr::DistortionCoordinates_t ad;
			bool disa = a->sysi->ComputeDistortion(vr::EVREye(i), 0.0, 0.0, &ad);

			vr::DistortionCoordinates_t bd;
			bool disb = b->sysi->ComputeDistortion(vr::EVREye(i), 0.0, 0.0, &bd);
			ASSERT(disa == disb);
			ASSERT(ad == bd);


			vr::DistortionCoordinates_t ad0;
			bool disa0 = a->sysi->ComputeDistortion(vr::EVREye(i), 1.0f, 1.0f, &ad0);
			vr::DistortionCoordinates_t bd0;
			bool disb0 = b->sysi->ComputeDistortion(vr::EVREye(i), 1.0, 1.0f, &bd0);
			ASSERT(ad0 == bd0);
			ASSERT(disa0 == disb0);

			vr::DistortionCoordinates_t ad1;
			bool disa1 = a->sysi->ComputeDistortion(vr::EVREye(i), 0.75f, 0.1f, &ad1);
			vr::DistortionCoordinates_t bd1;
			bool disb1 = b->sysi->ComputeDistortion(vr::EVREye(i), 0.75f, 0.1f, &bd1);
			ASSERT(disa1 == disb1);
			ASSERT(softcompare_is_similar(&ad1, &bd1, 0.1f)); // error in sampling distortion
			


			ia->Refresh();
			ib->Refresh();

			vr::HmdMatrix34_t eha = a->sysi->GetEyeToHeadTransform(vr::EVREye(i));
			vr::HmdMatrix34_t ehb = b->sysi->GetEyeToHeadTransform(vr::EVREye(i));
			ASSERT(softcompare_is_similar(eha, ehb));

			for (int mesh_type = 0; mesh_type < 3; mesh_type++)
			{
				vr::HiddenAreaMesh_t mesha = a->sysi->GetHiddenAreaMesh(vr::EVREye(i), vr::EHiddenAreaMeshType(mesh_type));
				vr::HiddenAreaMesh_t meshb = b->sysi->GetHiddenAreaMesh(vr::EVREye(i), vr::EHiddenAreaMeshType(mesh_type));

				ASSERT(mesha.unTriangleCount == meshb.unTriangleCount);
				ASSERT(memcmp(mesha.pVertexData, meshb.pVertexData, sizeof(float) * 2 * mesha.unTriangleCount) == 0);
			}
		}
	}
	{
		ia->Refresh();
		ib->Refresh();
		float	seca; uninit(seca);
		uint64_t fca; uninit(fca);
		bool vsa = a->sysi->GetTimeSinceLastVsync(&seca, &fca);

		float	secb; uninit(secb);
		uint64_t fcb; uninit(fcb);
		bool vsb = b->sysi->GetTimeSinceLastVsync(&secb, &fcb);
		ASSERT(vsa == vsb);

		// since sample timings differ - use a coarse filter 
		ASSERT(fabs(seca - secb) < 0.5f);
		ASSERT(fabs(fca - fcb) < 500); // ignore this for serialization test or if you were stepping through code!

		bool ona = a->sysi->IsDisplayOnDesktop();
		bool onb = b->sysi->IsDisplayOnDesktop();
		ASSERT(ona == onb);
	}

	{
		for (int universe = 0; universe < 3; universe++)
		{
			vr::TrackedDevicePose_t posea[vr::k_unMaxTrackedDeviceCount];
			vr::TrackedDevicePose_t poseb[vr::k_unMaxTrackedDeviceCount];
			uninit(posea);
			uninit(poseb);

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
				ASSERT(softcompare_is_similar(posea[i], poseb[i], 0.0001f));
			}
		}
		vr::HmdMatrix34_t seateda = a->sysi->GetSeatedZeroPoseToStandingAbsoluteTrackingPose();
		vr::HmdMatrix34_t seatedb = b->sysi->GetSeatedZeroPoseToStandingAbsoluteTrackingPose();
		ASSERT(seateda == seatedb);

		/** Returns the transform from the tracking origin to the standing absolute tracking system. This allows
		* applications to convert from raw tracking space to the calibrated standing coordinate system. */
		vr::HmdMatrix34_t rawa = a->sysi->GetRawZeroPoseToStandingAbsoluteTrackingPose();
		vr::HmdMatrix34_t rawb = b->sysi->GetRawZeroPoseToStandingAbsoluteTrackingPose();
		ASSERT(rawa == rawb);

	}

	{
		for (int i = 0; i <= 4; i++)
		{
			for (int j = 0; j < vr::k_unMaxTrackedDeviceCount + 1; j++)
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
				uninit(indicesa);
				uint32_t sorteda = a->sysi->GetSortedTrackedDeviceIndicesOfClass(vr::ETrackedDeviceClass(i),
					indicesa, vr::k_unMaxTrackedDeviceCount,
					unRelativeToTrackedDeviceIndex);

				vr::TrackedDeviceIndex_t indicesb[vr::k_unMaxTrackedDeviceCount];
				uninit(indicesb);
				uint32_t sortedb = b->sysi->GetSortedTrackedDeviceIndicesOfClass(vr::ETrackedDeviceClass(i),
					indicesb, vr::k_unMaxTrackedDeviceCount,
					unRelativeToTrackedDeviceIndex);
				ASSERT(sorteda == sortedb);
				ASSERT(memcmp(indicesa, indicesb, sizeof(indicesa)) == 0);

			}

		}
	}

	{
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
		{
			vr::EDeviceActivityLevel alevel = a->sysi->GetTrackedDeviceActivityLevel(i);
			vr::EDeviceActivityLevel blevel = b->sysi->GetTrackedDeviceActivityLevel(i);
			// timing differences?
			// anyways doesn't sync ASSERT(alevel == blevel);
		}
	}

	{
		ia->Refresh();
		ib->Refresh();
		vr::TrackedDeviceIndex_t lefta = a->sysi->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
		vr::TrackedDeviceIndex_t righta = a->sysi->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
		vr::TrackedDeviceIndex_t leftb = b->sysi->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
		vr::TrackedDeviceIndex_t rightb = b->sysi->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
		ASSERT(lefta == leftb);
		ASSERT(righta == rightb);
	}


	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		vr::ETrackedControllerRole	rolea = a->sysi->GetControllerRoleForTrackedDeviceIndex(i);
		vr::ETrackedDeviceClass classa = a->sysi->GetTrackedDeviceClass(i);
		bool connecteda = a->sysi->IsTrackedDeviceConnected(i);

		vr::ETrackedControllerRole roleb = b->sysi->GetControllerRoleForTrackedDeviceIndex(i);
		vr::ETrackedDeviceClass classb = b->sysi->GetTrackedDeviceClass(i);
		bool connectedb = b->sysi->IsTrackedDeviceConnected(i);

		ASSERT(rolea == roleb);
		ASSERT(classa == classb);
		ASSERT(connecteda == connectedb);
	}


	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		vr::VRControllerState_t statea; uninit(statea);
		bool reta = a->sysi->GetControllerState(i, &statea, sizeof(statea));
		vr::VRControllerState_t stateb; uninit(stateb);
		bool retb = b->sysi->GetControllerState(i, &stateb, sizeof(stateb));
		int compare_score = softcompare_controllerstates(&statea, &stateb);
		ASSERT(compare_score < 3);
	}



	{
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				vr::VRControllerState_t statea;
				vr::TrackedDevicePose_t posea;
				uninit(statea);
				uninit(posea);

				bool reta = a->sysi->GetControllerStateWithPose(
					vr::ETrackingUniverseOrigin(j),
					i,
					&statea, sizeof(statea),
					&posea);

				vr::VRControllerState_t stateb;
				uninit(stateb);
				vr::TrackedDevicePose_t poseb;
				uninit(poseb);
				bool retb = b->sysi->GetControllerStateWithPose(
					vr::ETrackingUniverseOrigin(j),
					i,
					&stateb, sizeof(stateb),
					&poseb);

				if (reta != retb)
				{
					ASSERT(reta == retb);
				}

				int compare_score = softcompare_controllerstates(&statea, &stateb);
				ASSERT(compare_score < 3);
				if (reta)
				{
					ASSERT(softcompare_is_similar(posea, poseb, 0.0001f));
				}
			}
		}
	}

	{
		bool inputa = a->sysi->IsInputFocusCapturedByAnotherProcess();
		bool inputb = b->sysi->IsInputFocusCapturedByAnotherProcess();
		ASSERT(inputa == inputb);
	}

	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		std::vector<vr::ETrackedDeviceProperty> props =
		{
			vr::Prop_EdidVendorID_Int32,
			vr::Prop_VendorSpecific_Reserved_Start
		};

		for (auto prop : props)
		{
			vr::ETrackedPropertyError errora;
			int32_t reta = a->sysi->GetInt32TrackedDeviceProperty(i, prop, &errora);

			vr::ETrackedPropertyError errorb;
			int32_t retb = b->sysi->GetInt32TrackedDeviceProperty(i, prop, &errorb);
			ASSERT(reta == retb);
			ASSERT(errora == errorb);
		}
		
	

	}
	


}

void InterfaceAuditor::compare_taci_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	ia->Refresh();
	ib->Refresh();

	// pass an invalid interface
	bool has_cameraa = true;
	bool has_camerab = true;
	vr::EVRTrackedCameraError rca = a->taci->HasCamera(99, &has_cameraa);
	vr::EVRTrackedCameraError rcb = b->taci->HasCamera(99, &has_camerab);
	ASSERT(rca == rcb);
	ASSERT(has_cameraa == has_camerab);

	// pass a null value
	rca = a->taci->HasCamera(0, nullptr);
	rcb = b->taci->HasCamera(0, nullptr);
	ASSERT(rca == rcb);

	// pass a valid value
	rca = a->taci->HasCamera(0, &has_cameraa);
	rcb = b->taci->HasCamera(0, &has_camerab);
	ASSERT(rca == rcb);
	ASSERT(has_cameraa == has_camerab);

	rca = a->taci->HasCamera(1, &has_cameraa);
	rcb = b->taci->HasCamera(1, &has_camerab);
	ASSERT(rca == rcb);
	ASSERT(has_cameraa == has_camerab);


	// HasCamera across devices
	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		for (int j = 0; j < vr::MAX_CAMERA_FRAME_TYPES; j++)
		{
			bool has_cameraa; uninit(has_cameraa);
			bool has_camerab; uninit(has_camerab);
			vr::EVRTrackedCameraError rca = a->taci->HasCamera(1, &has_cameraa);
			vr::EVRTrackedCameraError rcb = b->taci->HasCamera(1, &has_camerab);
		}
	}


	// FrameSize
	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		for (int j = 0; j < vr::MAX_CAMERA_FRAME_TYPES; j++)
		{
			uint32_t wa; uninit(wa);
			uint32_t ha; uninit(ha);
			uint32_t sizea; uninit(sizea);

			uint32_t wb; uninit(wb);
			uint32_t hb; uninit(hb);
			uint32_t sizeb; uninit(sizeb);

			vr::EVRTrackedCameraError rca = a->taci->GetCameraFrameSize(i, (vr::EVRTrackedCameraFrameType)j, &wa, &ha, &sizea);
			vr::EVRTrackedCameraError rcb = b->taci->GetCameraFrameSize(i, (vr::EVRTrackedCameraFrameType)j, &wb, &hb, &sizeb);
			ASSERT(rca == rcb);
			ASSERT(wa == wb);
			ASSERT(ha == hb);
			ASSERT(sizea == sizeb);
		}
	}

	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		for (int j = 0; j < vr::MAX_CAMERA_FRAME_TYPES; j++)
		{
			vr::HmdVector2_t focal_lena; uninit(focal_lena);
			vr::HmdVector2_t center_lena; uninit(center_lena);
			vr::HmdVector2_t focal_lenb; uninit(focal_lenb);
			vr::HmdVector2_t center_lenb; uninit(center_lenb);
			vr::EVRTrackedCameraError rca = a->taci->GetCameraIntrinsics(i, (vr::EVRTrackedCameraFrameType)j, &focal_lena, &center_lena);
			vr::EVRTrackedCameraError rcb = b->taci->GetCameraIntrinsics(i, (vr::EVRTrackedCameraFrameType)j, &focal_lenb, &center_lenb);
			ASSERT(rca == rcb);
			ASSERT(focal_lena == focal_lenb);
			ASSERT(center_lena == center_lenb);
		}
	}

	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		for (int j = 0; j < vr::MAX_CAMERA_FRAME_TYPES; j++)
		{
			vr::HmdMatrix44_t projectiona; uninit(projectiona);
			vr::HmdMatrix44_t projectionb; uninit(projectionb);
			vr::EVRTrackedCameraError rca = a->taci->GetCameraProjection(i, (vr::EVRTrackedCameraFrameType)j, c.nearz, c.farz, &projectiona);
			vr::EVRTrackedCameraError rcb = b->taci->GetCameraProjection(i, (vr::EVRTrackedCameraFrameType)j, c.nearz, c.farz, &projectionb);
			ASSERT(rca == rcb);
			ASSERT(projectiona == projectionb);
		}
	}
}


void InterfaceAuditor::compare_remi_strange_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

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
		uint32_t countb = 77;
		bool rcb = b->chapsi->GetLiveCollisionBoundsInfo(nullptr, &countb);
		ASSERT(counta == 66);
		ASSERT(countb == 77);
		ASSERT((rca == false) && (rcb == false));
	}

	// passing a 0 and a null gives the required count and a false
	{
		uint32_t counta = 0;
		bool rca = a->chapsi->GetLiveCollisionBoundsInfo(nullptr, &counta);
		uint32_t countb = 0;
		bool rcb = b->chapsi->GetLiveCollisionBoundsInfo(nullptr, &countb);
		WAIVE(counta == countb, "live collision error handling is wacky");
		WAIVE((rca == false) && (rcb == false), "live collision error handling is nuts");
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

		ASSERT(counta == countb);
		ASSERT((rca == false) && (rcb == false));
	}


	// GetLiveSeatedZeroPoseToRawTrackingPose
	{
		bool ba = a->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(nullptr);
		bool bb = b->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(nullptr);
		ASSERT(ba == bb);

		vr::HmdMatrix34_t posea;
		vr::HmdMatrix34_t poseb;
		memset(&posea, 0, sizeof(posea));
		memset(&poseb, 0, sizeof(posea));
		bool bba = a->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(&posea);
		bool bbb = b->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(&poseb);
		ASSERT(posea == poseb);
		ASSERT(bba == bbb);
	}

	// GetVulkanInstanceExtensionsRequired
	{
		// nullptr test 1
		{
			uint32_t ba = a->compi->GetVulkanInstanceExtensionsRequired(nullptr, 0);
			uint32_t bb = b->compi->GetVulkanInstanceExtensionsRequired(nullptr, 0);
			ASSERT(ba == bb);
		}

		// nullptr test 2
		{
			uint32_t ba = a->compi->GetVulkanInstanceExtensionsRequired(nullptr, 42);
			uint32_t bb = b->compi->GetVulkanInstanceExtensionsRequired(nullptr, 42);
			ASSERT(ba == bb);
		}


		{
			char bufa[256]; uninit(bufa);
			uint32_t ba = a->compi->GetVulkanInstanceExtensionsRequired(bufa, 256);
			char bufb[256]; uninit(bufb);
			uint32_t bb = b->compi->GetVulkanInstanceExtensionsRequired(bufb, 256);
			ASSERT(ba == bb);
			ASSERT(strcmp(bufa, bufb) == 0);
		}
	}

	{
		vr::EVRCompositorError ba = a->compi->GetLastPoses(nullptr, 0, nullptr, 0);
		vr::EVRCompositorError bb = b->compi->GetLastPoses(nullptr, 0, nullptr, 0);
		ASSERT(ba == bb);
	}

	{
		vr::TrackedDevicePose_t gposesa[16]; uninit(gposesa);
		vr::TrackedDevicePose_t rposesa[16]; uninit(rposesa);
		vr::TrackedDevicePose_t gposesb[16]; uninit(gposesb);
		vr::TrackedDevicePose_t rposesb[16]; uninit(rposesb);
		ia->Refresh();
		ib->Refresh();
		vr::EVRCompositorError ba = a->compi->GetLastPoses(rposesa, 1, gposesa, 1);
		vr::EVRCompositorError bb = b->compi->GetLastPoses(rposesb, 1, gposesb, 1);

		ASSERT(softcompare_is_similar(gposesa[0], gposesb[0],0.0001f));
		ASSERT(softcompare_is_similar(rposesa[0], rposesb[0], 0.0001f));
		ASSERT(ba == bb);
	}

	{
		//rendermodel //invalid modelname test: banana
		vr::EVRRenderModelError errora; uninit(errora);
		vr::RenderModel_t *modela = nullptr;
		errora = a->remi->LoadRenderModel_Async("banana", &modela);

		while (1)
		{
			errora = a->remi->LoadRenderModel_Async("banana", &modela);
			if (errora != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		vr::EVRRenderModelError errorb; uninit(errorb);
		vr::RenderModel_t *modelb = nullptr;
		errorb = b->remi->LoadRenderModel_Async("banana", &modelb);
		while (1)
		{
			errorb = b->remi->LoadRenderModel_Async("banana", &modela);
			if (errorb != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		ASSERT(errora == errorb);
	}

	{
		//rendermodel //invalid modelname test: nullptr

		vr::EVRRenderModelError errora; uninit(errora);
		vr::RenderModel_t *modela = nullptr;
		errora = a->remi->LoadRenderModel_Async(nullptr, &modela);
		while (1)
		{
			errora = a->remi->LoadRenderModel_Async(nullptr, &modela);
			if (errora != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}


		vr::EVRRenderModelError errorb; uninit(errorb);
		vr::RenderModel_t *modelb = nullptr;
		errorb = b->remi->LoadRenderModel_Async(nullptr, &modelb);
		while (1)
		{
			errora = a->remi->LoadRenderModel_Async(nullptr, &modela);
			if (errora != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		ASSERT(errora == errorb);
	}


	// rendermodel test
	{
		char render_model_name[256]; uninit(render_model_name);
		a->remi->GetRenderModelName(0, render_model_name, sizeof(render_model_name));

		// load from a
		vr::EVRRenderModelError errora; uninit(errora);
		vr::RenderModel_t *modela = nullptr;
		while (1)
		{
			errora = a->remi->LoadRenderModel_Async(render_model_name, &modela);
			if (errora != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		// load from b
		vr::EVRRenderModelError errorb; uninit(errorb);
		vr::RenderModel_t *modelb = nullptr;
		while (1)
		{
			errorb = b->remi->LoadRenderModel_Async(render_model_name, &modelb);
			if (errorb != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		ASSERT(errora == errorb);
	}

	// rendermodel and texture test
	{
		char render_model_name[256]; uninit(render_model_name);
		a->remi->GetRenderModelName(0, render_model_name, sizeof(render_model_name));

		// load from a
		vr::EVRRenderModelError errora; uninit(errora);
		vr::RenderModel_t *modela = nullptr;
		while (1)
		{
			errora = a->remi->LoadRenderModel_Async(render_model_name, &modela);
			if (errora != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		// test can't continue unless
		ASSERT(errora == vr::VRRenderModelError_None);

		vr::EVRRenderModelError error_tex_a; uninit(error_tex_a);
		vr::RenderModel_TextureMap_t *texa = nullptr;
		error_tex_a = a->remi->LoadTexture_Async(modela->diffuseTextureId, &texa);
		while (1)
		{
			error_tex_a = a->remi->LoadTexture_Async(modela->diffuseTextureId, &texa);
			if (error_tex_a != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		ASSERT(error_tex_a == vr::VRRenderModelError_None);

		// load model from b
		vr::EVRRenderModelError errorb; uninit(errorb);
		vr::RenderModel_t *modelb = nullptr;
		while (1)
		{
			errorb = b->remi->LoadRenderModel_Async(render_model_name, &modelb);
			if (errorb != vr::VRRenderModelError_Loading)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		ASSERT(errorb == vr::VRRenderModelError_None);

		//
		// load texture from b
		//

		if (modelb)
		{
			vr::EVRRenderModelError error_tex_b; uninit(error_tex_b);
			vr::RenderModel_TextureMap_t *texb = nullptr;
			error_tex_b = b->remi->LoadTexture_Async(modelb->diffuseTextureId, &texb);
			while (1)
			{
				error_tex_b = b->remi->LoadTexture_Async(modelb->diffuseTextureId, &texb);
				if (error_tex_b != vr::VRRenderModelError_Loading)
					break;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			ASSERT(errora == errorb);
			ASSERT(error_tex_a == error_tex_b);
		}
	}


	// rendermodel walk down models and components
	{
		uint32_t counta = a->remi->GetRenderModelCount();
		uint32_t countb = b->remi->GetRenderModelCount();
		ASSERT(counta == countb);
		for (int i = 0; i < (int)counta; i++)
		{
			char namea[256]; uninit(namea);
			char nameb[256]; uninit(nameb);
			uint32_t a_ret = a->remi->GetRenderModelName(i, namea, sizeof(namea));
			uint32_t b_ret = b->remi->GetRenderModelName(i, nameb, sizeof(nameb));
			ASSERT(a_ret == b_ret);
			ASSERT(strcmp(namea, nameb) == 0);

			uint32_t ccounta = a->remi->GetComponentCount(namea);
			uint32_t ccountb = b->remi->GetComponentCount(nameb);
			ASSERT(ccounta == ccountb);

			char thumba[256]; uninit(thumba);
			char thumbb[256]; uninit(thumbb);
			vr::EVRRenderModelError terrora; uninit(terrora);
			vr::EVRRenderModelError terrorb; uninit(terrorb);
			uint32_t treta = a->remi->GetRenderModelThumbnailURL(namea, thumba, sizeof(thumba), &terrora);
			uint32_t tretb = b->remi->GetRenderModelThumbnailURL(nameb, thumbb, sizeof(thumbb), &terrorb);
			ASSERT(treta == tretb);

			// GetRenderModelThumbnailURL sometimes returns 
			// Error_None with an empty string and sometimes returns 
			// Invalid_Model

			// so in the case of ok and empty string, pretend it returned
			// InvalidModel
			if (treta == 1)
			{
				ASSERT(terrorb == vr::VRRenderModelError_None || terrorb == vr::VRRenderModelError_InvalidModel);
			}
			else
			{
				ASSERT(terrora == terrorb);
			}

			ASSERT(strcmp(thumba, thumbb) == 0);
			char patha[256]; uninit(patha);
			char pathb[256]; uninit(pathb);
			vr::EVRRenderModelError perrora; uninit(perrora);
			vr::EVRRenderModelError perrorb; uninit(perrorb);
			uint32_t pathreta = a->remi->GetRenderModelOriginalPath(namea, patha, sizeof(patha), &perrora);
			uint32_t pathretb = b->remi->GetRenderModelOriginalPath(nameb, pathb, sizeof(pathb), &perrorb);
			ASSERT(pathreta == pathretb);
			ASSERT(perrora == perrorb);
			ASSERT(strcmp(patha, pathb) == 0);

			for (int j = 0; j < (int)ccounta; j++)
			{
				char cnamea[256]; uninit(cnamea);
				char cnameb[256]; uninit(cnameb);
				uint32_t cnreta = a->remi->GetComponentName(namea, j, cnamea, sizeof(cnamea));
				uint32_t cnretb = b->remi->GetComponentName(nameb, j, cnameb, sizeof(cnameb));
				ASSERT(cnreta == cnretb);
				ASSERT(strcmp(cnamea, cnameb) == 0);

				uint64_t bma = a->remi->GetComponentButtonMask(namea, cnamea);
				uint64_t bmb = b->remi->GetComponentButtonMask(nameb, cnameb);
				ASSERT(bma == bmb);


				char crmnamea[256]; uninit(crmnamea);
				char crmnameb[256]; uninit(crmnameb);
				uint32_t crnreta = a->remi->GetComponentRenderModelName(namea, cnamea, crmnamea, sizeof(crmnamea));
				uint32_t crnretb = b->remi->GetComponentRenderModelName(nameb, cnameb, crmnameb, sizeof(crmnameb));
				if (crnreta != 0 || crnretb != 0)
				{
					ASSERT(crnreta == crnretb);
					// if there is a render model name, then compare them
					ASSERT(strcmp(crmnamea, crmnameb) == 0);
				}

				bool hasa = a->remi->RenderModelHasComponent(namea, cnamea);
				bool hasb = b->remi->RenderModelHasComponent(nameb, cnameb);
				ASSERT(hasa == hasb);
			}
		}
	}
}



void InterfaceAuditor::interactive_component_state_test(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();


	int controller_index = 3;
	char rendermodelname[256]; uninit(rendermodelname);
	a->sysi->GetStringTrackedDeviceProperty(controller_index, vr::Prop_RenderModelName_String, rendermodelname, sizeof(rendermodelname));

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

		ia->Refresh();
		ib->Refresh();

		// call into get controller state once
		bool breta = a->sysi->GetControllerState(3, &controller_statea, sizeof(controller_statea));
		bool bretb = b->sysi->GetControllerState(3, &controller_stateb, sizeof(controller_stateb));
		ASSERT(breta == bretb);

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

		ASSERT(csa == csb);
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
			// ASSERT( the score is better than some value
			if (score > 12)
			{
				dprintf("score is %d\n", score);
			}
		}
	}
}