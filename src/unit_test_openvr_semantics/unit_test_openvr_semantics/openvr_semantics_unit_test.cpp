// openvr_semantics_unit_test.cpp : Defines the entry point for the console application.
//

#include <assert.h>
#include <vrdelta.h>
#include <windows.h>

#include <vector>
#include <chrono>
#include <thread>
#include "openvr_softcompare.h"

#include "openvr_broker.h"
#include "dprintf.h"

// passthru with dirty sync
// cursor with latest mode
// together these would give a close approximation to the behaviour of the raw interface
// and then would be easier to cross check

struct OpenVRInterfaceUnderTest;

void compare_sysi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
void compare_appi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
void compare_seti_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
void compare_chapi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
void compare_chapsi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib);
void compare_compi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
void compare_remi_strange_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib);
void compare_trackedcamera_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib);
void compare_ovi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
void compare_exdi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);


void interactive_component_state_test(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib);



// for testing/abstraction purposes, package the interfaces in something with
// an Refres method.  This way I can have the 'tracker' based interfaces update their cursor positions
// when writes are being made.

struct OpenVRInterfaceUnderTest 
{
	virtual openvr_broker::open_vr_interfaces &Get() = 0;
	virtual void Refresh() {};

	virtual void PushOverlayEvent(vr::VROverlayHandle_t ulOverlayHandle, vr::VREvent_t *pEvent) {}

};

struct RawInterface : public OpenVRInterfaceUnderTest
{
	RawInterface()
		: m_init(false)
	{
	}
	
	void Init()
	{
		m_init = true;
		char *error;
		if (!openvr_broker::acquire_interfaces("raw", &m_raw_interface, &error))
		{
			dprintf("error %s\n", error);
			exit(0);
		}
	}


	virtual openvr_broker::open_vr_interfaces &Get() override { return m_raw_interface;  }

private:
	bool m_init;
	openvr_broker::open_vr_interfaces m_raw_interface;
};

struct CursorBasedInterface : public OpenVRInterfaceUnderTest
{
	CursorBasedInterface() 
		: m_init(false)
	{}

	~CursorBasedInterface()
	{
		if (m_init)
		{
		}
	}

	openvr_broker::open_vr_interfaces &Get() override { return m_cursor_interfaces;  }

	void Init(const TrackerConfig &c, const openvr_broker::open_vr_interfaces &raw_interfaces)
	{
		m_init = true;
		m_tracker = create_vr_state_tracker(c);
		m_raw_interfaces = raw_interfaces;
		capture_vr_state(m_tracker, m_raw_interfaces);

		int start_frame; int end_frame;
		m_cursor = create_cursor(m_tracker);
		m_cursor_interfaces = get_cursor_interfaces(m_cursor);
		get_frame_range(m_tracker, &start_frame, &end_frame);
		set_cursor_frame(m_cursor, end_frame);
	}

	virtual void PushOverlayEvent(vr::VROverlayHandle_t ulOverlayHandle, vr::VREvent_t *pEvent) override
	{
		capture_vr_overlay_event(m_tracker, ulOverlayHandle, *pEvent);
	}

	virtual void Refresh() override
	{
		capture_vr_state(m_tracker, m_raw_interfaces);

		// move cursor
		int start_frame; int end_frame;
		get_frame_range(m_tracker, &start_frame, &end_frame);
		set_cursor_frame(m_cursor, end_frame);
	}
private:
	bool m_init;
	vr_state_tracker_t m_tracker;
	vr_cursor_t m_cursor;
	openvr_broker::open_vr_interfaces m_raw_interfaces;
	openvr_broker::open_vr_interfaces m_cursor_interfaces;
};

const char *overlay_keys[] = {
	 "a","b","c"
};

int main()
{
	TrackerConfig c;
	c.set_default();
	c.num_overlays_to_sample = 3;
	c.overlay_keys_to_sample = overlay_keys;

	RawInterface raw;
	raw.Init();
	CursorBasedInterface cursor;
	cursor.Init(c, raw.Get());

#if 0
	compare_sysi_interfaces(&raw, &cursor, c);
	compare_appi_interfaces(&raw, &cursor, c);
	compare_seti_interfaces(&raw, &cursor, c);
	compare_chapi_interfaces(&raw, &cursor, c);
	compare_chapsi_interfaces(&raw, &cursor);
	compare_trackedcamera_interfaces(&raw, &cursor);
	compare_strange_interfaces(&raw, &cursor);
	compare_compi_interfaces(&raw, &cursor, c);
	compare_ovi_interfaces(&raw, &cursor, c);
#endif
	compare_exdi_interfaces(&raw, &cursor, c);


	bool do_interactive = false;
	if (do_interactive)
	{
		interactive_component_state_test(&raw, &cursor);
	}

    return 0;
}


void compare_exdi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{

	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();
	ia->Refresh();
	ib->Refresh();

	{
		//virtual void GetWindowBounds(int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight) = 0;
		int ax, ay;
		uint32_t aw, ah;
		a->exdi->GetWindowBounds(&ax, &ay, &aw, &ah);

		int bx, by;
		uint32_t bw, bh;
		b->exdi->GetWindowBounds(&bx, &by, &bw, &bh);
		assert(ax == bx);
		assert(ay == by);
		assert(aw == bw);
		assert(ah == bh);

	}
	
	{
		uint32_t ax, ay;
		uint32_t aw, ah;
		a->exdi->GetEyeOutputViewport(vr::Eye_Left, &ax, &ay, &aw, &ah);
		uint32_t bx, by;
		uint32_t bw, bh;
		b->exdi->GetEyeOutputViewport(vr::Eye_Left, &bx, &by, &bw, &bh);
		assert(ax == bx);
		assert(ay == by);
		assert(ah == bh);
		assert(aw == bw);
	}

	{
		uint32_t ax, ay;
		uint32_t aw, ah;
		a->exdi->GetEyeOutputViewport(vr::Eye_Right, &ax, &ay, &aw, &ah);
		uint32_t bx, by;
		uint32_t bw, bh;
		b->exdi->GetEyeOutputViewport(vr::Eye_Right, &bx, &by, &bw, &bh);
		assert(ax == bx);
		assert(ay == by);
		assert(ah == bh);
		assert(aw == bw);
	}
}


void process_overlay_events_on_handles(
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
			dprintf("%s\n", openvr_string::to_string(event_a).c_str());
			ia->PushOverlayEvent(handle, &event_a);
			ib->PushOverlayEvent(handle, &event_a);
		}
	}
}

void compare_per_overlay_handles(	vr::VROverlayHandle_t overlay_handle_a,
									vr::VROverlayHandle_t overlay_handle_b,
									openvr_broker::open_vr_interfaces *a, 
									openvr_broker::open_vr_interfaces *b)
{
	{
		uint32_t pid_a = a->ovi->GetOverlayRenderingPid(overlay_handle_a);
		uint32_t pid_b = b->ovi->GetOverlayRenderingPid(overlay_handle_b);
		assert(pid_a == pid_b);
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
			bool enabled_a;
			bool enabled_b;
			vr::EVROverlayError reta = a->ovi->GetOverlayFlag(overlay_handle_a, flag, &enabled_a);
			vr::EVROverlayError retb = b->ovi->GetOverlayFlag(overlay_handle_b, flag, &enabled_b);
			assert(enabled_a == enabled_b);
			assert(reta == retb);
		}
	}

	{
		float ared = 0.0f;
		float agreen = 0.0f;
		float ablue = 0.0f;
		vr::EVROverlayError reta = a->ovi->GetOverlayColor(overlay_handle_a, &ared, &agreen, &ablue);

		float bred = 0.0f;
		float bgreen = 0.0f;
		float bblue = 0.0f;
		vr::EVROverlayError retb = b->ovi->GetOverlayColor(overlay_handle_b, &bred, &bgreen, &bblue);

		assert(ared == bred);
		assert(agreen == bgreen);
		assert(ablue == bblue);
		assert(reta == retb);
	}

	{
		float aalpha = 0;
		vr::EVROverlayError reta = a->ovi->GetOverlayAlpha(overlay_handle_a, &aalpha);

		float balpha = 0;
		vr::EVROverlayError retb = b->ovi->GetOverlayAlpha(overlay_handle_b, &balpha);
		assert(reta == retb);
		assert(aalpha == balpha);
	}

	{
		float texelaspecta = 0;
		float texelaspectb = 0;
		vr::EVROverlayError reta = a->ovi->GetOverlayTexelAspect(overlay_handle_a, &texelaspecta);
		vr::EVROverlayError retb = b->ovi->GetOverlayTexelAspect(overlay_handle_b, &texelaspectb);
		assert(reta == retb);
		assert(texelaspecta == texelaspectb);
	}

	{
		uint32_t sort_ordera = 0;
		vr::EVROverlayError reta = a->ovi->GetOverlaySortOrder(overlay_handle_a, &sort_ordera);
		uint32_t sort_orderb = 0;
		vr::EVROverlayError retb = b->ovi->GetOverlaySortOrder(overlay_handle_b, &sort_orderb);
		assert(sort_ordera == sort_orderb);
		assert(reta == retb);
	}

	{
		float width_in_metersa = 0;
		float width_in_metersb = 0;
		vr::EVROverlayError reta = a->ovi->GetOverlayWidthInMeters(overlay_handle_a, &width_in_metersa);
		vr::EVROverlayError retb = b->ovi->GetOverlayWidthInMeters(overlay_handle_b, &width_in_metersb);
		assert(reta == retb);
		assert(width_in_metersa == width_in_metersb);
	}

	{
		float MinDistanceInMetersa = 0;
		float MaxDistanceInMetersa = 0;
		vr::EVROverlayError reta = a->ovi->GetOverlayAutoCurveDistanceRangeInMeters(overlay_handle_a,
			&MinDistanceInMetersa, &MaxDistanceInMetersa);

		float MinDistanceInMetersb = 0;
		float MaxDistanceInMetersb = 0;
		vr::EVROverlayError retb = b->ovi->GetOverlayAutoCurveDistanceRangeInMeters(overlay_handle_b,
			&MinDistanceInMetersb, &MaxDistanceInMetersb);
		assert(reta == retb);
		assert(MinDistanceInMetersa == MinDistanceInMetersb);
		assert(MaxDistanceInMetersb == MaxDistanceInMetersb);
	}

	{
		vr::EColorSpace color_spacea;
		vr::EVROverlayError reta = a->ovi->GetOverlayTextureColorSpace(overlay_handle_a, &color_spacea);

		vr::EColorSpace color_spaceb;
		vr::EVROverlayError retb = b->ovi->GetOverlayTextureColorSpace(overlay_handle_b, &color_spaceb);
		assert(color_spacea == color_spaceb);
		assert(reta == retb);
	}

	{
		vr::VRTextureBounds_t texture_boundsa;
		vr::EVROverlayError reta = a->ovi->GetOverlayTextureBounds(overlay_handle_a, &texture_boundsa);

		vr::VRTextureBounds_t texture_boundsb;
		vr::EVROverlayError retb = b->ovi->GetOverlayTextureBounds(overlay_handle_b, &texture_boundsb);
		assert(texture_boundsa == texture_boundsb);
		assert(reta == retb);
	}

	{
		vr::VROverlayTransformType transform_typea;
		vr::EVROverlayError reta = a->ovi->GetOverlayTransformType(overlay_handle_a, &transform_typea);

		vr::VROverlayTransformType transform_typeb;
		vr::EVROverlayError retb = b->ovi->GetOverlayTransformType(overlay_handle_b, &transform_typeb);
		assert(transform_typea == transform_typeb);
		assert(reta == retb);
	}

	{
		vr::ETrackingUniverseOrigin eTrackingOrigina;
		vr::HmdMatrix34_t			matTrackingOriginToOverlayTransforma;
		vr::EVROverlayError reta = a->ovi->GetOverlayTransformAbsolute(overlay_handle_a, &eTrackingOrigina, &matTrackingOriginToOverlayTransforma);

		vr::ETrackingUniverseOrigin eTrackingOriginb;
		vr::HmdMatrix34_t			matTrackingOriginToOverlayTransformb;
		vr::EVROverlayError retb = b->ovi->GetOverlayTransformAbsolute(overlay_handle_b, &eTrackingOriginb, &matTrackingOriginToOverlayTransformb);

		assert(reta == retb);
		assert(eTrackingOrigina == eTrackingOriginb);
		assert(softcompare_is_similar(matTrackingOriginToOverlayTransforma, matTrackingOriginToOverlayTransformb) == true);
	}

	{
		vr::TrackedDeviceIndex_t	unTrackedDevicea;
		vr::HmdMatrix34_t			matTrackedDeviceToOverlayTransforma;
		vr::EVROverlayError reta = a->ovi->GetOverlayTransformTrackedDeviceRelative(overlay_handle_a,
			&unTrackedDevicea,
			&matTrackedDeviceToOverlayTransforma);

		vr::TrackedDeviceIndex_t	unTrackedDeviceb;
		vr::HmdMatrix34_t			matTrackedDeviceToOverlayTransformb;
		vr::EVROverlayError retb = b->ovi->GetOverlayTransformTrackedDeviceRelative(overlay_handle_b,
			&unTrackedDeviceb,
			&matTrackedDeviceToOverlayTransformb);
		assert(reta == retb);
		assert(unTrackedDevicea == unTrackedDeviceb);
		assert(softcompare_is_similar(matTrackedDeviceToOverlayTransforma, matTrackedDeviceToOverlayTransformb) == true);
	}

	{
		vr::TrackedDeviceIndex_t device_indexa;
		char chComponentNamea[256];
		vr::EVROverlayError reta = a->ovi->GetOverlayTransformTrackedDeviceComponent(
			overlay_handle_a, &device_indexa, chComponentNamea, sizeof(chComponentNamea));
		
		vr::TrackedDeviceIndex_t device_indexb;
		char chComponentNameb[256];
		vr::EVROverlayError retb = b->ovi->GetOverlayTransformTrackedDeviceComponent(
			overlay_handle_b, &device_indexb, chComponentNameb, sizeof(chComponentNameb));
		assert(device_indexa == device_indexb);
		assert(strcmp(chComponentNamea, chComponentNameb) == 0);
	}

	{
		vr::VROverlayInputMethod input_methoda;
		vr::EVROverlayError rca = a->ovi->GetOverlayInputMethod(overlay_handle_a, &input_methoda);
		vr::VROverlayInputMethod input_methodb;
		vr::EVROverlayError rcb = b->ovi->GetOverlayInputMethod(overlay_handle_b, &input_methodb);
		assert(input_methoda == input_methodb);
		assert(rca == rcb);
	}

	{
		vr::HmdVector2_t mouse_scalea;
		vr::EVROverlayError rca = a->ovi->GetOverlayMouseScale(overlay_handle_a, &mouse_scalea);

		vr::HmdVector2_t mouse_scaleb;
		vr::EVROverlayError rcb = b->ovi->GetOverlayMouseScale(overlay_handle_a, &mouse_scaleb);

		assert(mouse_scalea == mouse_scaleb);
		assert(rca == rcb);
	}

	{
		bool rca = a->ovi->IsHoverTargetOverlay(overlay_handle_a);
		bool rcb = b->ovi->IsHoverTargetOverlay(overlay_handle_b);
		assert(rca == rcb);
	}

	{
		uint32_t widtha = 0;
		uint32_t heighta = 0;
		vr::EVROverlayError rca = a->ovi->GetOverlayTextureSize(overlay_handle_a, &widtha, &heighta);

		uint32_t widthb = 0;
		uint32_t heightb = 0;
		vr::EVROverlayError rcb = b->ovi->GetOverlayTextureSize(overlay_handle_b, &widthb, &heightb);
		assert(widtha == widthb);
		assert(heighta == heightb);
		assert(rca == rcb);
	}

	{
		bool rca = a->ovi->IsActiveDashboardOverlay(overlay_handle_a);
		bool rcb = b->ovi->IsActiveDashboardOverlay(overlay_handle_b);
		assert(rca == rcb);
	}

	{
		uint32_t process_id_a;
		vr::EVROverlayError rca = a->ovi->GetDashboardOverlaySceneProcess(overlay_handle_a, &process_id_a);
		uint32_t process_id_b;
		vr::EVROverlayError rcb = b->ovi->GetDashboardOverlaySceneProcess(overlay_handle_b, &process_id_b);
		assert(rca == rcb);
		assert(process_id_a == process_id_b);
	}

	{
		vr::TrackedDeviceIndex_t device_index_a = a->ovi->GetPrimaryDashboardDevice();
		vr::TrackedDeviceIndex_t device_index_b = b->ovi->GetPrimaryDashboardDevice();
		assert(device_index_a == device_index_b);
	}

	{
		char bufa[256];
		uint32_t rca = a->ovi->GetKeyboardText(bufa, sizeof(bufa));

		char bufb[256];
		uint32_t rcb = a->ovi->GetKeyboardText(bufb, sizeof(bufb));

		assert(rcb == rca);
		assert(strcmp(bufa, bufb) == 0);
	}
}

void compare_ovi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();
	ia->Refresh();
	ib->Refresh();

	// create overlays per TrackerConfig
	for (int i = 0; i < c.num_overlays_to_sample; i++)
	{
		vr::VROverlayHandle_t overlay_handle_a;
		std::string friendly_name = std::string(c.overlay_keys_to_sample[i]) + "friendly";
		vr::EVROverlayError errora = a->ovi->CreateOverlay(c.overlay_keys_to_sample[i], friendly_name.c_str(), &overlay_handle_a);

		float alpha = -77.0f;
		a->ovi->GetOverlayAlpha(22, &alpha);
		
		vr::VROverlayHandle_t overlay_handle_b;
		vr::EVROverlayError errorb = b->ovi->CreateOverlay(c.overlay_keys_to_sample[i], friendly_name.c_str(), &overlay_handle_b);
		assert(errora == errorb);	
	}

	// check that the handles can find their keys

	ia->Refresh();
	ib->Refresh();

	std::vector<vr::VROverlayHandle_t> a_handles;
	std::vector<vr::VROverlayHandle_t> b_handles;

	// find handles for keys
	for (int i = 0; i < c.num_overlays_to_sample; i++)
	{
		vr::VROverlayHandle_t overlay_handle_a;
		vr::EVROverlayError errora = a->ovi->FindOverlay(c.overlay_keys_to_sample[i], &overlay_handle_a);

		vr::VROverlayHandle_t overlay_handle_b;
		vr::EVROverlayError errorb = b->ovi->FindOverlay(c.overlay_keys_to_sample[i], &overlay_handle_b);

		a_handles.push_back(overlay_handle_a);
		b_handles.push_back(overlay_handle_b);

		assert(errora == errorb);
		assert(overlay_handle_a == overlay_handle_b);
	}
	
	process_overlay_events_on_handles(ia, &a_handles, ib, &b_handles);

	for (int i = 0; i < c.num_overlays_to_sample; i++)
	{
		char szbufa[256];
		vr::EVROverlayError erra;
		uint32_t a_ret = a->ovi->GetOverlayKey(a_handles[i], szbufa, sizeof(szbufa), &erra);

		char szbufb[256];
		vr::EVROverlayError errb;
		uint32_t b_ret = b->ovi->GetOverlayKey(b_handles[i], szbufb, sizeof(szbufb), &errb);

		assert(erra == errb);
		assert(strcmp(szbufa, szbufb) == 0);
		assert(a_ret == b_ret);
	}

	// try an invalid handle 0x22 - expect error code
	{
		char szbufa[256];
		vr::EVROverlayError erra;
		uint32_t a_ret = a->ovi->GetOverlayKey(0x22, szbufa, sizeof(szbufa), &erra);

		char szbufb[256];
		vr::EVROverlayError errb;
		uint32_t b_ret = b->ovi->GetOverlayKey(0x22, szbufb, sizeof(szbufb), &errb);
		assert(erra != vr::VROverlayError_None);
		assert(errb != vr::VROverlayError_None);
		assert(erra == errb);	// strict mode
		assert(a_ret == b_ret);

	}

	// try an invalid name - expect error code
	{
		vr::VROverlayHandle_t overlay_handle_a;
		vr::EVROverlayError errora = a->ovi->FindOverlay("invalid", &overlay_handle_a);

		vr::VROverlayHandle_t overlay_handle_b;
		vr::EVROverlayError errorb = b->ovi->FindOverlay("invalid", &overlay_handle_b);

		assert(errora == errorb);
		assert(overlay_handle_a == overlay_handle_b);
	}

	// try deleting an existing handle ('a') and query - expect error code
	{
		vr::EVROverlayError destroyerrora = a->ovi->DestroyOverlay(a_handles[0]);
		char szbufa[256];
		vr::EVROverlayError queryerra;
		uint32_t a_ret = a->ovi->GetOverlayKey(a_handles[0], szbufa, sizeof(szbufa), &queryerra); // should be invalid

		vr::EVROverlayError destroyerrorb = b->ovi->DestroyOverlay(b_handles[0]);

		ia->Refresh();			// give b a chance to notice it's gone
		ib->Refresh();

		char szbufb[256];
		vr::EVROverlayError queryerrb;
		uint32_t b_ret = b->ovi->GetOverlayKey(b_handles[0], szbufb, sizeof(szbufb), &queryerrb); // should be invalid
		assert(a_ret == b_ret);
		assert(queryerra == queryerrb);
	}

	{
		// add it back in
		vr::VROverlayHandle_t overlay_handle_a;
		vr::VROverlayHandle_t overlay_handle_b;
		std::string friendly_name = std::string(c.overlay_keys_to_sample[0]) + "friendly";
		vr::EVROverlayError errora = a->ovi->CreateOverlay(c.overlay_keys_to_sample[0], friendly_name.c_str(), &overlay_handle_a);
		vr::EVROverlayError errorb = b->ovi->CreateOverlay(c.overlay_keys_to_sample[0], friendly_name.c_str(), &overlay_handle_b);
		assert(errora == errorb);


		vr::VROverlayHandle_t found_overlay_handle_a;
		vr::EVROverlayError found_errora = a->ovi->FindOverlay(c.overlay_keys_to_sample[0], &found_overlay_handle_a);

		// give b a chance to detect the new overlay
		ia->Refresh();
		ib->Refresh();

		vr::VROverlayHandle_t found_overlay_handle_b;
		vr::EVROverlayError found_errorb = b->ovi->FindOverlay(c.overlay_keys_to_sample[0], &found_overlay_handle_b);
		assert(found_errora == found_errorb);
		assert(found_overlay_handle_a == found_overlay_handle_b);

		a_handles[0] = found_overlay_handle_a;
		b_handles[0] = found_overlay_handle_b;
	}

	// check they can find their names
	for (int i = 0; i < c.num_overlays_to_sample; i++)
	{
		char szbufa[256];
		vr::EVROverlayError erra;
		uint32_t a_ret = a->ovi->GetOverlayName(a_handles[i], szbufa, sizeof(szbufa), &erra);

		char szbufb[256];
		vr::EVROverlayError errb;
		uint32_t b_ret = b->ovi->GetOverlayName(b_handles[i], szbufb, sizeof(szbufb), &errb);

		assert(erra == errb);
		assert(strcmp(szbufa, szbufb) == 0);
		assert(a_ret == b_ret);
	}

	// test with no textures assigned
	{
		for (int i = 0; i < c.num_overlays_to_sample; i++)
		{
			uint32_t aw=99, ah=99;
			vr::EVROverlayError aimg_err = a->ovi->GetOverlayImageData(a_handles[i], nullptr, 0, &aw, &ah);

			uint32_t bw = 99, bh = 99;
			vr::EVROverlayError bimg_err = b->ovi->GetOverlayImageData(b_handles[i], nullptr, 0, &bw, &bh);
			assert(aw == bw);
			assert(ah == bh);
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

		for (int i = 0; i < c.num_overlays_to_sample; i++)
		{
			vr::EVROverlayError erra = a->ovi->SetOverlayRaw(a_handles[i], tex, texture_w, texture_h, depth);
			vr::EVROverlayError errb = b->ovi->SetOverlayRaw(b_handles[i], tex, texture_w, texture_h, depth);
			ia->Refresh();
			ib->Refresh();
			uint32_t a_wout;
			uint32_t a_hout;
			vr::EVROverlayError geterra = a->ovi->GetOverlayImageData(a_handles[i], texa, tex_size, &a_wout, &a_hout);
			uint32_t b_wout;
			uint32_t b_hout;
			vr::EVROverlayError geterrb = b->ovi->GetOverlayImageData(b_handles[i], texb, tex_size, &b_wout, &b_hout);

			assert(geterra == geterrb);
			assert(a_wout = b_wout);
			assert(a_hout = b_hout);
			assert(memcmp(tex, texa, tex_size) == 0);
			assert(memcmp(tex, texb, tex_size) == 0);
		}

		free(tex);
		free(texa);
		free(texb);
	}

	for (int i = 0; i < c.num_overlays_to_sample; i++)
	{
		vr::VROverlayHandle_t handle_a = a_handles[i];
		vr::VROverlayHandle_t handle_b = b_handles[i];

		compare_per_overlay_handles(handle_a, handle_b, &ia->Get(), &ib->Get());
	}

}

void compare_compi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();
	
	ia->Refresh();
	ib->Refresh();

	{
		vr::ETrackingUniverseOrigin aret = a->compi->GetTrackingSpace();
		vr::ETrackingUniverseOrigin bret = b->compi->GetTrackingSpace();
		assert(aret == bret);
	}
	
	{
		vr::TrackedDevicePose_t render_posesa[vr::k_unMaxTrackedDeviceCount];
		vr::TrackedDevicePose_t game_posesa[vr::k_unMaxTrackedDeviceCount];

		vr::EVRCompositorError rca = a->compi->GetLastPoses(
										render_posesa, vr::k_unMaxTrackedDeviceCount,
										game_posesa, vr::k_unMaxTrackedDeviceCount);

		vr::TrackedDevicePose_t render_posesb[vr::k_unMaxTrackedDeviceCount];
		vr::TrackedDevicePose_t game_posesb[vr::k_unMaxTrackedDeviceCount];

		vr::EVRCompositorError rcb = b->compi->GetLastPoses(
										render_posesb, vr::k_unMaxTrackedDeviceCount,
										game_posesb, vr::k_unMaxTrackedDeviceCount);

		assert(rca == rcb);
		assert(softcompare_is_similar(render_posesa, render_posesb, vr::k_unMaxTrackedDeviceCount) == true);
		assert(softcompare_is_similar(game_posesa, game_posesb, vr::k_unMaxTrackedDeviceCount) == true);

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
		assert(rca == rcb);
		assert(softcompare_is_similar(timing_a, timing_b));
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
		assert(reta == retb);
		for (int i = 0; i < (int)c.frame_timings_num_frames; i++)
		{
			assert(softcompare_is_similar(atimings[i], btimings[i]));
		}
		free(atimings);
		free(btimings);
	}

	{
		ia->Refresh();
		ib->Refresh();
		float aret = a->compi->GetFrameTimeRemaining();
		float bret = b->compi->GetFrameTimeRemaining();
		assert(softcompare_is_similar(aret, bret, 0.5f));
	}

	{
		vr::Compositor_CumulativeStats a_stats;
		vr::Compositor_CumulativeStats b_stats;
		a->compi->GetCumulativeStats(&a_stats, sizeof(vr::Compositor_CumulativeStats));
		b->compi->GetCumulativeStats(&b_stats, sizeof(vr::Compositor_CumulativeStats));
		assert(softcompare_is_similar(a_stats, b_stats));
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
			assert(softcompare_is_similar(color_a, color_b, 0.01f));
		}
		{
			vr::HmdColor_t color_a = a->compi->GetCurrentFadeColor(true);
			vr::HmdColor_t color_b = b->compi->GetCurrentFadeColor(true);
			assert(softcompare_is_similar(color_a, color_b, 0.01f));
		}

		float alpha_a = a->compi->GetCurrentGridAlpha();
		float alpha_b = b->compi->GetCurrentGridAlpha();
		assert(softcompare_is_similar(alpha_a, alpha_b, .01f));
	}
	{
		bool rca = a->compi->IsFullscreen();
		bool rcb = b->compi->IsFullscreen();
		assert(rca == rcb);
	}
	{
		uint32_t rca = a->compi->GetCurrentSceneFocusProcess();
		uint32_t rcb = b->compi->GetCurrentSceneFocusProcess();
		assert(rca == rcb);
	}

	{
		uint32_t rca = a->compi->GetLastFrameRenderer();
		uint32_t rcb = b->compi->GetLastFrameRenderer();
		assert(rca == rcb);
	}

	{
		bool rca = a->compi->CanRenderScene();
		bool rcb = b->compi->CanRenderScene();
		assert(rca == rcb);
	}

	{
		bool rca = a->compi->IsMirrorWindowVisible();
		bool rcb = b->compi->IsMirrorWindowVisible();
		assert(rca == rcb);
	}

	{
		bool rca = a->compi->ShouldAppRenderWithLowResources();
		bool rcb = b->compi->ShouldAppRenderWithLowResources();
		assert(rca == rcb);
	}
}

void compare_chapi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	{
		vr::ChaperoneCalibrationState reta = a->chapi->GetCalibrationState();
		vr::ChaperoneCalibrationState retb = b->chapi->GetCalibrationState();
		assert(reta == retb);
	}
	
	{
		float sizexa = 0;
		float sizeza = 0;
		bool reta = a->chapi->GetPlayAreaSize(&sizexa, &sizeza);
		
		float sizexb = 0;
		float sizezb = 0;
		bool retb = b->chapi->GetPlayAreaSize(&sizexb, &sizezb);
		assert(reta == retb);
		assert(sizexa == sizexb);
		assert(sizeza == sizezb);
	}

	{
		vr::HmdQuad_t quada;
		bool reta = a->chapi->GetPlayAreaRect(&quada);

		vr::HmdQuad_t quadb;
		bool retb = b->chapi->GetPlayAreaRect(&quadb);
		assert(reta == retb);
		assert(quada == quadb);
	}


	{
		vr::HmdColor_t *color_array_a = (vr::HmdColor_t *)malloc(sizeof(vr::HmdColor_t) * c.num_bounds_colors);
		vr::HmdColor_t camera_a;
		a->chapi->GetBoundsColor(color_array_a, c.num_bounds_colors, c.collision_bounds_fade_distance, &camera_a);

		ib->Refresh();

		vr::HmdColor_t *color_array_b = (vr::HmdColor_t *)malloc(sizeof(vr::HmdColor_t) * c.num_bounds_colors);
		vr::HmdColor_t camera_b;
		b->chapi->GetBoundsColor(color_array_b, c.num_bounds_colors, c.collision_bounds_fade_distance, &camera_b);

		assert(softcompare_is_similar(color_array_a, color_array_b, c.num_bounds_colors, 0.01f));
		assert(softcompare_is_similar(camera_a, camera_b, 0.01f));

		free(color_array_a);
		free(color_array_b);
	}

	{
		bool rca = a->chapi->AreBoundsVisible();
		bool rcb = b->chapi->AreBoundsVisible();
		assert(rca == rcb);
	}

	{
		a->chapi->ForceBoundsVisible(true);
		b->chapi->ForceBoundsVisible(true);
		ia->Refresh();
		ib->Refresh();
		bool rca = a->chapi->AreBoundsVisible();
		bool rcb = b->chapi->AreBoundsVisible();
		assert(rca == rcb);

		vr::HmdColor_t *color_array_a = (vr::HmdColor_t *)malloc(sizeof(vr::HmdColor_t) * c.num_bounds_colors);
		vr::HmdColor_t camera_a;
		a->chapi->GetBoundsColor(color_array_a, c.num_bounds_colors, c.collision_bounds_fade_distance, &camera_a);

		vr::HmdColor_t *color_array_b = (vr::HmdColor_t *)malloc(sizeof(vr::HmdColor_t) * c.num_bounds_colors);
		vr::HmdColor_t camera_b;
		b->chapi->GetBoundsColor(color_array_b, c.num_bounds_colors, c.collision_bounds_fade_distance, &camera_b);
		
		assert(softcompare_is_similar(color_array_a, color_array_b, c.num_bounds_colors, 0.01f));
		assert(softcompare_is_similar(camera_a, camera_b, 0.01f));
		free(color_array_a);
		free(color_array_b);
	}
}

void compare_chapsi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();
	
	{
		float sizexa = 0;
		float sizeza = 0;
		vr::HmdQuad_t working_recta;
		bool rca = a->chapsi->GetWorkingPlayAreaSize(&sizexa, &sizeza);
		bool rca1 = a->chapsi->GetWorkingPlayAreaRect(&working_recta);

		float sizexb = 0;
		float sizezb = 0;
		vr::HmdQuad_t working_rectb;
		bool rcb = b->chapsi->GetWorkingPlayAreaSize(&sizexb, &sizezb);
		bool rcb1 = b->chapsi->GetWorkingPlayAreaRect(&working_rectb);

		assert(rca == rcb);
		assert(rcb1 == rca1);
		assert(sizexa == sizexb);
		assert(sizeza == sizezb);
		assert(working_rectb == working_recta);


		a->chapsi->SetWorkingPlayAreaSize(20.0f, 40.0f);
		b->chapsi->SetWorkingPlayAreaSize(20.0f, 40.0f);

		ia->Refresh();
		ib->Refresh();
		float sizexaa;
		float sizezaa;
		bool rcaa = a->chapsi->GetWorkingPlayAreaSize(&sizexaa, &sizezaa);

		float sizexbb;
		float sizezbb;
		bool rcbb = b->chapsi->GetWorkingPlayAreaSize(&sizexbb, &sizezbb);

		assert(rcaa == rcbb);
		assert(sizexaa == sizexbb);
		assert(sizezaa == sizezbb);
		assert(sizexaa == 20.0f);	// if this fails check your vrheadset
		assert(sizezbb == 40.0f);
	}


	{
		vr::HmdMatrix34_t seated2rawa;
		memset(&seated2rawa, 0, sizeof(seated2rawa));
		bool rca = a->chapsi->GetWorkingSeatedZeroPoseToRawTrackingPose(&seated2rawa);

		vr::HmdMatrix34_t seated2rawb;
		memset(&seated2rawb, 0, sizeof(seated2rawb));
		bool rcb = a->chapsi->GetWorkingSeatedZeroPoseToRawTrackingPose(&seated2rawb);

		assert(rca == rcb);
		assert(memcmp(&seated2rawa, &seated2rawb, sizeof(seated2rawa)) == 0);
	}

	{
		vr::HmdMatrix34_t standing2rawa;
		memset(&standing2rawa, 0, sizeof(standing2rawa));
		bool rca = a->chapsi->GetWorkingStandingZeroPoseToRawTrackingPose(&standing2rawa);

		vr::HmdMatrix34_t standing2rawb;
		memset(&standing2rawb, 0, sizeof(standing2rawb));
		bool rcb = a->chapsi->GetWorkingStandingZeroPoseToRawTrackingPose(&standing2rawb);

		assert(rca == rcb);
		assert(memcmp(&standing2rawa, &standing2rawb, sizeof(standing2rawa)) == 0);
	}

	for (int i = 0; i < 2; i++)
	{
		if (i == 1)
		{
			vr::HmdQuad_t quad;
			memset(&quad, 0, sizeof(quad));
			a->chapsi->SetWorkingCollisionBoundsInfo(&quad, 1);
			ia->Refresh();
			ib->Refresh();
		}

		uint32_t quads_counta = 0;
		bool rca = a->chapsi->GetWorkingCollisionBoundsInfo(nullptr, &quads_counta);
		uint32_t quads_countb = 0;
		bool rcb = b->chapsi->GetWorkingCollisionBoundsInfo(nullptr, &quads_countb);
		assert(rca == rcb);

		if (rca)
		{
			vr::HmdQuad_t *quadsa = (vr::HmdQuad_t *)calloc(quads_counta, sizeof(vr::HmdQuad_t));
			bool rca1 = a->chapsi->GetWorkingCollisionBoundsInfo(quadsa, &quads_counta);
			vr::HmdQuad_t *quadsb = (vr::HmdQuad_t *)calloc(quads_countb, sizeof(vr::HmdQuad_t));
			bool rcb1 = b->chapsi->GetWorkingCollisionBoundsInfo(quadsb, &quads_countb);
			assert(rca1 == rcb1);
			assert(quads_counta == quads_countb);
			free(quadsa);
			free(quadsb);
		}
	}

	{	
		uint32_t quad_counta = 0;
		bool rca = a->chapsi->GetLiveCollisionBoundsInfo(nullptr, &quad_counta);

		// insane count and nullptr
		uint32_t quad_countaa = 99;
		bool rca1 = b->chapsi->GetLiveCollisionBoundsInfo(nullptr, &quad_countaa);

		vr::HmdQuad_t *quadsa = (vr::HmdQuad_t *)calloc(quad_counta, sizeof(vr::HmdQuad_t));

		bool rcaa = a->chapsi->GetLiveCollisionBoundsInfo(quadsa, &quad_counta); // this function seems foobared - does not return count

		uint32_t quad_countb = 0;
		bool rcb = b->chapsi->GetLiveCollisionBoundsInfo(nullptr, &quad_countb);

		// give an insane count and nullptr 
		uint32_t quad_countbb = 99;
		bool rcb1 = b->chapsi->GetLiveCollisionBoundsInfo(nullptr, &quad_countbb);

		vr::HmdQuad_t *quadsb = (vr::HmdQuad_t *)calloc(quad_counta, sizeof(vr::HmdQuad_t));
		bool rcbb = a->chapsi->GetLiveCollisionBoundsInfo(quadsb, &quad_counta);

		assert(rca == rcb);
		assert(rcaa == rcbb);
		assert(quad_countaa == quad_countbb);
		assert(quad_counta == quad_countb);
		assert(memcmp(quadsa, quadsb, (sizeof(vr::HmdQuad_t) * quad_counta)) == 0);
		free(quadsa);
		free(quadsb);
	}

	{
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

		assert(rca == rcb);
		assert(rcaa == rcbb);
		assert(quad_countaa == quad_countbb);
		assert(quad_counta == quad_countb);
		assert(memcmp(quadsa, quadsb, (sizeof(vr::HmdQuad_t) * quad_counta)) == 0);
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

		assert(rca == rcb);
		assert(rcaa == rcbb);
		assert(uint_countaa == uint_countbb);
		assert(uint_counta == uint_countb);
		assert(memcmp(uintsa, uintsb, (sizeof(uint8_t) * uint_counta)) == 0);
		free(uintsa);
		free(uintsb);
	}

}

// test settings interfaces by getting and setting values to see that a interfaces behave the same as b interfaces
void compare_seti_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
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
		vr::EVRSettingsError errora;
		float fa = a->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &errora);

		vr::EVRSettingsError errorb;
		float fb = b->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &errorb);

		assert(fa == fb);
		assert(errora == errorb);
		
		// change it
		float new_val = fa + 1.0f;

		vr::EVRSettingsError write1errora;
		vr::EVRSettingsError write1errorb;
		a->seti->SetFloat(vr::k_pch_SteamVR_Section, key, new_val, &write1errora);
		b->seti->SetFloat(vr::k_pch_SteamVR_Section, key, new_val, &write1errorb);	// writes to stub
		ia->Refresh();
		ib->Refresh();
		// read it again
		vr::EVRSettingsError read1errora;
		vr::EVRSettingsError read1errorb;
		float fa1 = a->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &read1errora);
		float fb1 = b->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &read1errorb);	// reads from snapshot of a


		// write the old version back
		vr::EVRSettingsError write2errora;
		vr::EVRSettingsError write2errorb;
		a->seti->SetFloat(vr::k_pch_SteamVR_Section, key, fa, &write2errora);
		b->seti->SetFloat(vr::k_pch_SteamVR_Section, key, fa, &write2errorb);	// writes to stub
		ia->Refresh();
		ib->Refresh();

		vr::EVRSettingsError read2errora;
		vr::EVRSettingsError read2errorb;
		// check it again
		float finala = a->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &read2errora);	
		float finalb = b->seti->GetFloat(vr::k_pch_SteamVR_Section, key, &read2errorb);	// reads from snapshot of a

		assert(write1errora == write1errorb);
		assert(read1errora == read1errorb);
		assert(read1errora == read1errorb);
		assert(fa1 == fb1);
		assert(fa1 == new_val);

		assert(write2errora == write2errorb);
		assert(read2errora == read2errorb);

		assert(finala == finalb);
		assert(finala == fa);
	}
}

// broadly tests all get interfaces to make sure they behave
// the same 
void compare_appi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

	{
		bool ba = a->appi->IsApplicationInstalled(nullptr);
		bool bb = b->appi->IsApplicationInstalled(nullptr);
		assert(ba == bb);
	}

	ia->Refresh();
	ib->Refresh();

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
		assert(succeeded);	
	}

	{

		uint32_t weirda = a->appi->GetApplicationCount();

		bool ba = a->appi->IsApplicationInstalled("smursmur");
		bool bb = b->appi->IsApplicationInstalled("smursmur");
		assert(ba == bb);
		
		uint32_t weirdb = a->appi->GetApplicationCount();
		assert(weirda == weirdb);
		uint32_t weirdc = a->appi->GetApplicationCount();
		assert(weirdc == weirdb);


	}

	ia->Refresh();
	ib->Refresh();

	{
		uint32_t counta = a->appi->GetApplicationCount();
		uint32_t countb = b->appi->GetApplicationCount();
		//assert(counta == countb);
	}

	{
		for (int i = 0; i < (int)a->appi->GetApplicationCount(); i++)
		{
			vr::EVRApplicationError errora = a->appi->GetApplicationKeyByIndex(i, nullptr, 0);
			vr::EVRApplicationError errorb = b->appi->GetApplicationKeyByIndex(i, nullptr, 0);
			assert(errora == errorb);
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
			assert(errora == errorb);
			assert(bufa[0] == bufb[0]);
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


			assert(errora == errorb);
			assert(strcmp(bufa, bufb) == 0);
			assert(aprocess_id == bprocess_id);
			assert(autolaunch_a == autolaunch_b);
			assert(mimesupportboola == mimesupportboolb);
			assert(strcmp(mimesupportastr, mimesupportbstr) == 0);

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
					char outa[256]; outa[0] = 0;
					char outb[256]; outb[0] = 0;
					vr::EVRApplicationError errora;
					vr::EVRApplicationError errorb;
					uint32_t reta = a->appi->GetApplicationPropertyString(bufa, prop, outa, sizeof(outa), &errora);
					uint32_t retb = b->appi->GetApplicationPropertyString(bufb, prop, outb, sizeof(outb), &errorb);
					assert(reta == retb);
					assert(errora == errorb);
					assert(strcmp(outa, outb) == 0);
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
					vr::EVRApplicationError errora;
					vr::EVRApplicationError errorb;
					bool reta = a->appi->GetApplicationPropertyBool(bufa, prop, &errora);
					bool retb = b->appi->GetApplicationPropertyBool(bufb, prop, &errorb);
					assert(reta == retb);
					assert(errora == errorb);
				}
			}
			{
				std::vector<vr::EVRApplicationProperty> uint64_properties =
				{
					vr::VRApplicationProperty_LastLaunchTime_Uint64,					
				};

				for (auto prop : uint64_properties)
				{
					vr::EVRApplicationError errora;
					vr::EVRApplicationError errorb;
					uint64_t reta = a->appi->GetApplicationPropertyUint64(bufa, prop, &errora);
					uint64_t retb = b->appi->GetApplicationPropertyUint64(bufb, prop, &errorb);
					assert(reta == retb);
					assert(errora == errorb);					
				}
			}
		}
	}
	
	{
		vr::EVRApplicationTransitionState sta = a->appi->GetTransitionState();
		vr::EVRApplicationTransitionState stb = b->appi->GetTransitionState();
		assert(sta == stb);
	}

	{
		uint32_t currenta = a->appi->GetCurrentSceneProcessId();
		uint32_t currentb = b->appi->GetCurrentSceneProcessId();
		assert(currenta == currentb);
	}
}

void compare_sysi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

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

			ia->Refresh();
			ib->Refresh();

			vr::HmdMatrix34_t eha = a->sysi->GetEyeToHeadTransform(vr::EVREye(i));
			vr::HmdMatrix34_t ehb = b->sysi->GetEyeToHeadTransform(vr::EVREye(i));
			assert(softcompare_is_similar(eha, ehb));

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
		ia->Refresh();
		ib->Refresh();
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
				assert(softcompare_is_similar(posea[i],poseb[i]));
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
		ia->Refresh();
		ib->Refresh();
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
				assert(softcompare_is_similar(posea, poseb));
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

void compare_trackedcamera_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();

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


void compare_remi_strange_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib)
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
		assert(counta == countb);
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

		assert(counta == countb);
		assert((rca == false) && (rcb == false));
	}

	
	// GetLiveSeatedZeroPoseToRawTrackingPose
	{	
		bool ba = a->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(nullptr);
		bool bb = b->chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(nullptr);
		assert(ba == bb);

		vr::HmdMatrix34_t posea;
		vr::HmdMatrix34_t poseb;
		memset(&posea, 0, sizeof(posea));
		memset(&poseb, 0, sizeof(posea));
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
		
		assert(softcompare_is_similar(gposesa[0], gposesb[0]));
		assert(softcompare_is_similar(rposesa[0], rposesb[0]));
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



void interactive_component_state_test(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib)
{
	openvr_broker::open_vr_interfaces *a = &ia->Get();
	openvr_broker::open_vr_interfaces *b = &ib->Get();


	int controller_index = 3;
	char rendermodelname[256];
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