
// expected behaviour
// at startup
//	base_snapshot = take_snapshot(timestamp);
//
// in loop
//	last_snapshot = take_snapshot(last_snapshot);
//
// afterwards
//	snapshot_replay(events | pose)
//
// snapshot_diff(base_snapshot, final_snapshot)
//
// snapshot_save();
// snapshot_load();
//
// snapshot_step();
//

// design
// vrwalker - quickly walk though state and check for diffs
//					every struct needs a diff routine
//			  could use stronger enum stuff
//					enum walker
//			  could use improved struct stuff
//
// a step is something that has a timestamp and a list of changes
//
// a change is a keyword and value
//
// I think the entire vr state can be made flat
//

// I'd like to know the events that occured as well
// so it becomes more of a journal?

//-- leave that out of scope - caller can hook to an event recorder between snapshots

#include "openvr_broker.h"
//
// todo
//	1. test the basic structure by adding in a pose
//  2. add a log to print out when something changed or not
//		with a filter - ie a boolean flag to enable something
//  3. add a history to walk forwards through changes
//	4. figure out how many MB/sec it uses.
//		* adding a dumber encoder might be the easiest way to make this smaller and faster - ie no formatting/spaces just csv and no enum 2 string


struct TrackerConfig
{
	float nearz;
	float farz;

	float distortionU;
	float distortionV;

	float predicted_seconds_to_photon;

	int num_bounds_colors;
	float collision_bounds_fade_distance;

	void set_default()
	{
		nearz = 0.02f;
		farz = 100.0f;
		distortionU = 0.0f;
		distortionV = 0.0f;
		predicted_seconds_to_photon = 0.0f;
		num_bounds_colors = 10;
		collision_bounds_fade_distance = 2.0f;
	}
};

// should the tracker control write interfaces? TBD!
// pros:
//	* event processing could be intercepted
//  * nearz/farz and distortion queries (ie TrackerConfig) could be intercepted
//  * api calls could be logged automatically
//
// cons:
//	* I like that the caller can control when snapshots are taken
//  * I like that the system tries to not be dependent on tracking all the writes to capture state
// 


typedef void *vr_state_tracker_t;
vr_state_tracker_t create_vr_state_tracker(TrackerConfig c = { 0.1f, 100.0f, 0.0f, 0.0f, 0.0f });
void capture_vr_state(vr_state_tracker_t h,
						openvr_broker::open_vr_interfaces &interfaces);

void get_frame_range(vr_state_tracker_t, int *first_frame, int *last_frame);

void capture_vr_event(vr_state_tracker_t h, const vr::VREvent_t &event);

void dump_vr_history(vr_state_tracker_t h);

void dump_vr_state(vr_state_tracker_t h, openvr_broker::open_vr_interfaces &interfaces);

void dump_vr_current(vr_state_tracker_t h, openvr_broker::open_vr_interfaces &interfaces);

void save_vrstate_to_file(vr_state_tracker_t h, const char *filename, bool binary);
vr_state_tracker_t load_vrstate_from_file(const char *filename);

void destroy_vr_state_tracker(vr_state_tracker_t);


typedef void *vr_cursor_t;
vr_cursor_t create_cursor(vr_state_tracker_t h);
openvr_broker::open_vr_interfaces &get_cursor_interfaces(vr_cursor_t h);
void set_cursor_frame(vr_cursor_t h, int framenumber);
void destroy_cursor(vr_cursor_t h);


// this is for the old tree view
void img_gui_update(vr_state_tracker_t h);

typedef void *gui_context_t;
gui_context_t create_gui_context();
void gui_context_update(gui_context_t);
void destroy_gui_context(gui_context_t);

typedef void *timeline_grid_t;
struct grid_config
{
	const char *title;
	const char *include_filter;
	const char *exclude_filter;
};
timeline_grid_t create_timeline_grid(gui_context_t g, vr_state_tracker_t h, const grid_config &config);
void timeline_grid_update(vr_state_tracker_t h);
void destroy_timeline_grid(timeline_grid_t);
