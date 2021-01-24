// openvr_semantics_unit_test.cpp : Defines the entry point for the console application.
//
#include "InterfaceAuditor.h"
#include <windows.h>
#include <assert.h>
#include <vector>
#include <chrono>
#include <thread>
#include "openvr_softcompare.h"
#include "openvr_broker.h"
#include "dprintf.h"

// for testing/abstraction purposes, package the interfaces in something with
// an Refres method.  This way I can have the 'tracker' based interfaces update their cursor positions
// when writes are being made.
struct NullInterface : public OpenVRInterfaceUnderTest
{
	NullInterface()
		: m_init(false)
	{}
	void Init()
	{
		m_init = true;
		char *error;
		if (!openvr_broker::acquire_interfaces("null", &m_interfaces, &error))
		{
			dprintf("error %s\n", error);
			exit(0);
		}
	}

	virtual openvr_broker::open_vr_interfaces &Get() override { return m_interfaces; }

private:
	bool m_init;
	openvr_broker::open_vr_interfaces m_interfaces;
};


struct RawInterface : public OpenVRInterfaceUnderTest
{
	RawInterface()
		: m_init(false)
	{}
	

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

struct FileBasedInterface : public OpenVRInterfaceUnderTest
{
	FileBasedInterface()
		: m_init(false)
	{}

	~FileBasedInterface()
	{
		if (m_init)
		{
			destroy_cursor(m_cursor);
			destroy_vr_state_tracker(m_tracker);
		}
	}

	openvr_broker::open_vr_interfaces &Get() override { return m_cursor_interfaces; }

	void Init(const char *filename)
	{
		LoadFromFile(filename);
		m_init = true;
	}

	virtual void PushOverlayEvent(vr::VROverlayHandle_t ulOverlayHandle, vr::VREvent_t *pEvent) override
	{
		capture_vr_overlay_event(m_tracker, ulOverlayHandle, *pEvent);
	}

	virtual void SaveToFile(const char *filename, bool binary)
	{
		save_vrstate_to_file(m_tracker, filename, binary);
	}

	virtual void LoadFromFile(const char *filename)
	{
		if (m_init)
		{
			destroy_cursor(m_cursor);
			destroy_vr_state_tracker(m_tracker);
		}
		m_tracker = load_vrstate_from_file(filename);
		m_cursor = create_cursor(m_tracker);
		m_cursor_interfaces = get_cursor_interfaces(m_cursor);
	}

	virtual void Refresh() override
	{		
		// move cursor to the end frame
		int start_frame; int end_frame;
		get_frame_range(m_tracker, &start_frame, &end_frame);
		set_cursor_frame(m_cursor, end_frame);
	}
private:
	bool m_init;
	vr_state_tracker_t m_tracker;
	vr_cursor_t m_cursor;
	openvr_broker::open_vr_interfaces m_cursor_interfaces;
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

	void Init(const TrackerConfig &c, const openvr_broker::open_vr_interfaces &source_interfaces)
	{
		m_init = true;
		m_tracker = create_vr_state_tracker(c);
		m_source_interfaces = source_interfaces;
		m_cursor = create_cursor(m_tracker);
		m_cursor_interfaces = get_cursor_interfaces(m_cursor);
	}

	virtual void PushOverlayEvent(vr::VROverlayHandle_t ulOverlayHandle, vr::VREvent_t *pEvent) override
	{
		capture_vr_overlay_event(m_tracker, ulOverlayHandle, *pEvent);
	}

	virtual void SaveToFile(const char *filename, bool binary)
	{
		save_vrstate_to_file(m_tracker, filename, binary);
	}

	virtual void Refresh() override
	{
		capture_vr_state(m_tracker, m_source_interfaces);
		set_cursor_to_end_frame(m_tracker, m_cursor);
	}
private:
	bool m_init;
	vr_state_tracker_t m_tracker;
	vr_cursor_t m_cursor;
	openvr_broker::open_vr_interfaces m_source_interfaces;
	openvr_broker::open_vr_interfaces m_cursor_interfaces;
};

// create a raw interface
//  create a tracker that points into that interface
//
// walk though the api and make sure the results don't deviate too much
//
void run_raw_vs_tracker_audit_test()
{
	printf("starting raw vs cursor test\n");
	const char *overlay_keys[] = {
		"a","b","c"
	};

	const char *resource_directories[] =
	{
		"icons"
	};

	const char *resource_names[] =
	{
		"base_status_error.png"
	};

	const char *float_setting_sections[] =
	{
		"unittest_section"
	};

	const char *float_setting_names[] =
	{
		"unit_test_key"
	};


	TrackerConfig c;
	c.set_default();
	c.num_overlays = sizeof(overlay_keys)/sizeof(overlay_keys[0]);
	c.overlay_keys = overlay_keys;

	c.num_resources = sizeof(resource_names) / sizeof(resource_names[0]);
	c.resource_directories = resource_directories;
	c.resource_filenames = resource_names;

	c.custom_settings.float_names = float_setting_names;
	c.custom_settings.float_sections = float_setting_sections;
	c.custom_settings.num_float_settings = sizeof(float_setting_names) / sizeof(float_setting_names[0]);

	

	const char *vendor_int32_property_names[] = 
	{
		"vendor_test"
	};
	int vendor_int32_properties_values[] =
	{
		vr::Prop_VendorSpecific_Reserved_Start
	};
	c.custom_tracked_device_properties.int32_names = vendor_int32_property_names;
	c.custom_tracked_device_properties.int32_values = vendor_int32_properties_values;
	c.custom_tracked_device_properties.num_int32_properties = sizeof(vendor_int32_property_names)/sizeof(vendor_int32_property_names[0]);


	RawInterface raw;
	raw.Init();
	CursorBasedInterface cursor;
	cursor.Init(c, raw.Get());

	bool read_only = false;
	bool interactive = false;
	bool large_time_gap_override = false;
	InterfaceAuditor auditor;
	auditor.AuditInterfaces(&raw, &cursor, c, read_only, large_time_gap_override, interactive);
	auditor.PrintResults();
}

void run_serialization_test()
{
	printf("starting saved object vs raw test\n");


	const char *float_setting_sections[] =
	{
		"unittest_section"
	};

	const char *float_setting_names[] =
	{
		"unit_test_key"
	};

	TrackerConfig c;
	c.set_default();
	c.custom_settings.float_names = float_setting_names;
	c.custom_settings.float_sections = float_setting_sections;
	c.custom_settings.num_float_settings = sizeof(float_setting_names) / sizeof(float_setting_names[0]);
	const char *vendor_int32_property_names[] =
	{
		"vendor_test"
	};
	int vendor_int32_properties_values[] =
	{
		vr::Prop_VendorSpecific_Reserved_Start
	};
	c.custom_tracked_device_properties.int32_names = vendor_int32_property_names;
	c.custom_tracked_device_properties.int32_values = vendor_int32_properties_values;
	c.custom_tracked_device_properties.num_int32_properties = sizeof(vendor_int32_property_names) / sizeof(vendor_int32_property_names[0]);


	RawInterface raw;
	raw.Init();

	CursorBasedInterface cursorA;	// cursor A is attached to a Raw Source - so it will wobble
	cursorA.Init(c, raw.Get());
	cursorA.SaveToFile("c:\\vr_streams\\unit_test.bin", true);

	cursorA.Refresh();
	cursorA.SaveToFile("c:\\vr_streams\\unit_test1.bin", true);  // save to a file

	FileBasedInterface fileA;
	fileA.Init("c:\\vr_streams\\unit_test1.bin");  // load the file

	// readonly tests here because since fileA is a file, it's setter functions are 'stubs'  passive and will fail if you try and read and write from it
	bool read_only = true;			

	bool interactive = false;
	bool large_time_gap_override = true;
	InterfaceAuditor auditor;
	auditor.AuditInterfaces(&cursorA, &fileA, c, read_only, large_time_gap_override, interactive);
	auditor.PrintResults();
}

void chain_cursor_object_test()
{
	printf("starting chained object test\n");

	FileBasedInterface cursorA;
	cursorA.Init("c:\\vr_streams\\unit_test1.bin");	// if this fails, make sure you ran the serialization test first
	uint32_t wa;
	uint32_t ha;
	cursorA.Get().sysi->GetRecommendedRenderTargetSize(&wa, &ha);

	TrackerConfig c;
	c.set_default();
	CursorBasedInterface cursorB;
	cursorB.Init(c, cursorA.Get());
	cursorB.Refresh();
	uint32_t wb;
	uint32_t hb;
	cursorB.Get().sysi->GetRecommendedRenderTargetSize(&wb, &hb);

	// check that the chain is working for at least one test point before continuing on
	assert(wa == wb);
	assert(ha == hb);

	InterfaceAuditor auditor;
	bool read_only = true;
	bool interactive = false;
	bool large_time_gap_override = false;
	auditor.AuditInterfaces(&cursorA, &cursorB, c, read_only, large_time_gap_override, interactive);
	auditor.PrintResults();
}

void run_perf_test()
{
	RawInterface raw;
	raw.Init();
	RawInterface raw2;
	raw2.Init();
	TrackerConfig c;
	c.set_default();
	InterfaceAuditor auditor;

	
	auditor.AuditInterfaces(&raw, &raw2, c, true, false, false);

	dprintf("done");

	FileBasedInterface fileA;
	fileA.Init("c:\\vr_streams\\unit_test1.bin");
	FileBasedInterface fileB;
	fileB.Init("c:\\vr_streams\\unit_test1.bin");
	auditor.AuditInterfaces(&fileA, &fileB, c, true, false, false);
	dprintf("done");

	{
		RawInterface raw;
		raw.Init();

		CursorBasedInterface cursorA;	// cursor A is attached to a Raw Source - so it will wobble
		cursorA.Init(c, raw.Get());
		cursorA.SaveToFile("c:\\vr_streams\\unit_test.bin", true);

		cursorA.Refresh();
		cursorA.SaveToFile("c:\\vr_streams\\unit_test1.bin", true);  // save to a file

		FileBasedInterface fileA;
		fileA.Init("c:\\vr_streams\\unit_test1.bin");  // load the file

													   // readonly tests here because since fileA is a file, it's setter functions are 'stubs'  passive and will fail if you try and read and write from it
		bool read_only = true;

		bool interactive = false;
		bool large_time_gap_override = true;
		InterfaceAuditor auditor;
		auditor.AuditInterfaces(&cursorA, &fileA, c, read_only, large_time_gap_override, interactive);
		auditor.PrintResults();
	}
}

int main()
{
#if 0
	run_raw_vs_tracker_audit_test();
	run_serialization_test();
	chain_cursor_object_test();	// has a dependency on run_serialization_test
#endif

	run_perf_test();
	
	return 0;
}

