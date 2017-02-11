#pragma once
#include <vrdelta.h>
#include <vector>

struct OpenVRInterfaceUnderTest
{
	virtual openvr_broker::open_vr_interfaces &Get() = 0;
	virtual void Refresh() {};
	virtual void PushOverlayEvent(vr::VROverlayHandle_t ulOverlayHandle, vr::VREvent_t *pEvent) {}
};

//
// query interface a and query interface b an make report on any differences
//
class InterfaceAuditor
{
public:
	void InterfaceAuditor::AuditInterfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c,
		bool do_interactive = false);

private:
	void ReportFailure(const char *string, const char *file, int line);
	void compare_sysi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
	void compare_appi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
	void compare_seti_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
	void compare_chapi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
	void compare_chapsi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib);
	void compare_compi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
	void compare_remi_strange_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib);
	void compare_ovi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
	void compare_exdi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
	void compare_taci_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
	void compare_screeni_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
	void compare_resi_interfaces(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib, const TrackerConfig &c);
	void interactive_component_state_test(OpenVRInterfaceUnderTest *ia, OpenVRInterfaceUnderTest *ib);

	void compare_per_overlay_handles(vr::VROverlayHandle_t overlay_handle_a,
		vr::VROverlayHandle_t overlay_handle_b,
		openvr_broker::open_vr_interfaces *a,
		openvr_broker::open_vr_interfaces *b);

	void process_overlay_events_on_handles(
		OpenVRInterfaceUnderTest *ia, std::vector<vr::VROverlayHandle_t> *h,
		OpenVRInterfaceUnderTest *ib, std::vector<vr::VROverlayHandle_t> *h2);

	void compare_apps(vr::IVRApplications *a_appi, vr::IVRApplications *b_appi);
};