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
		}
	}

	openvr_broker::open_vr_interfaces &Get() override { return m_cursor_interfaces; }

	void Init(const char *filename)
	{
		m_init = true;
		LoadFromFile(filename);
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
			//destroy_cursor(m_cursor);
			//destroy_vr_state_tracker(m_tracker);
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

		// move cursor to the end frame
		int start_frame; int end_frame;
		get_frame_range(m_tracker, &start_frame, &end_frame);
		set_cursor_frame(m_cursor, end_frame);
	}
private:
	bool m_init;
	vr_state_tracker_t m_tracker;
	vr_cursor_t m_cursor;
	openvr_broker::open_vr_interfaces m_source_interfaces;
	openvr_broker::open_vr_interfaces m_cursor_interfaces;
};


void run_audit_test()
{

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

	TrackerConfig c;
	c.set_default();
	c.num_overlays_to_sample = sizeof(overlay_keys)/sizeof(overlay_keys[0]);
	c.overlay_keys_to_sample = overlay_keys;

	c.num_resources_to_sample = sizeof(resource_names) / sizeof(resource_names[0]);
	c.resource_directories_to_sample = resource_directories;
	c.resource_filenames_to_sample = resource_names;


	RawInterface raw;
	raw.Init();
	CursorBasedInterface cursor;
	cursor.Init(c, raw.Get());

	InterfaceAuditor auditor;
	auditor.AuditInterfaces(&raw, &cursor, c);
}

void chain_cursor_object_test()
{
	FileBasedInterface cursorA;
	cursorA.Init("c:\\vr_streams\\unit_test.bin");

	TrackerConfig c;
	c.set_default();
	CursorBasedInterface cursorB;
	cursorB.Init(c, cursorA.Get());

	InterfaceAuditor auditor;
	auditor.AuditInterfaces(&cursorA, &cursorB, c);

}

void run_serialization_test()
{
	TrackerConfig c;
	c.set_default();

	RawInterface raw;
	raw.Init();

	CursorBasedInterface cursorA;
	cursorA.Init(c, raw.Get());
	cursorA.SaveToFile("c:\\vr_streams\\unit_test.bin", true);

	FileBasedInterface fileA;
	fileA.Init("c:\\vr_streams\\unit_test.bin");

	
	InterfaceAuditor auditor;
	auditor.AuditInterfaces(&cursorA, &fileA, c);

}

int main()
{
	chain_cursor_object_test();
	run_serialization_test();
	run_audit_test();
	return 0;
}

