#pragma once
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

// Demo ideas
//	* hellopenvr 
//			- link to openvrdelta.lib
//  * on startup 
//			- record option
//			means to request interfaces that:
//			    snapshot at startup
//				pass though everything to the real openvr
//				periodically snapshot 
//          or
//              request a separate snapshot interface
//				take snapshot
//
//   on restart 
//			- playback option
//			  loads on startup
//            all read requests from the openvr stubs
//			  
// playback forces event and waitforposes stepping
//  e.g. if event pending, don't advance time until it is gone.
//       if waitforpose pending, dont advance time until it is gone.
//
//		 if profiler is up allow pausing
//

//
// passthru monitor
// 

// how to test such a system
//
// a monitor that watches transactions and marks desyncs: 
//			// # times that there is a mismatch.
//			// # times there is a mismatch that isn't corrected by refreshing
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

// tracker concepts/responsibilities
// Wrappers - wrap the openvr interfaces and export optional<style> interfaces to the caller
// Visitors functions - walk the graph.  
// HistoryNode/Presence - detects changes of optional<style> values and logs that history
// vrschema is the data model which can be instantiated as history notes or cursors into that schema

// TrackerConfigInternal - is the internal ResourcesOfInterest Database
//	its used by the OverlaysHelper to know which overlays to track
//  its used by the resources visitors to know which resources to query
//  its used by all visitors to fill in some of the small config values

// productizations
//	spy
//		costs:	add call tracing
//				add area of interest monitoring
//
//	debug:
//		is a superset of spy.
//		add breakpoints
//      add expressions
//
//	replay
//		figure out how the caller can rewind as well
//		event stepping
//	
// money is in process. 
//		make this invaluable for a developer
//      make this invaluable for platform people
//
//


// need to pipe in callers areas of interest:
// caller assets: resources, screenshot handles, overlays, 
// caller specific queries: nearz, farz, seconds to photon queries, etc:
// unless he tells me, then I need to "spy" it out
//
// its meant to be more of a hint than part of the database - ie don't add dependencies to this
struct TrackerConfig	// aka Hints
{
	float nearz;					// intercept: GetProjectionMatrix
	float farz;

	int  distortion_sample_width;		
	int  distortion_sample_height;

	float predicted_seconds_to_photon; // intercept: GetDeviceToAbsoluteTrackingPose

	int num_bounds_colors;					// intercept: GetBoundsColor
	float collision_bounds_fade_distance;	// ""

	uint32_t frame_timing_frames_ago;		// intercept GetFrameTiming
	uint32_t frame_timings_num_frames;		// intercept GetFrameTimings

	int num_overlays;				// intercept 30-50 overlay functions
	const char **overlay_keys;

	int num_resources;			// intercept GetResourceFullPath and GetResourceFullPath
	const char **resource_directories;
	const char **resource_filenames;

	// custom settings
	struct {
		int num_bool_settings;
		const char **bool_sections;
		const char **bool_names;

		int num_int32_settings;
		const char **int32_sections;
		const char **int32_names;

		int num_string_settings;
		const char **string_sections;
		const char **string_names;

		int num_float_settings;
		const char **float_sections;
		const char **float_names;
	} custom_settings;

	// custom tracked device properties
	struct {
		int num_bool_properties;
		const char **bool_names;
		int *bool_values;

		int num_string_properties;
		const char **string_names;
		int *string_values;

		int num_uint64_properties;
		const char **uint64_names;
		int *uint64_values;

		int num_int32_properties;
		const char **int32_names;
		int *int32_values;

		int num_mat34_properties;
		const char **mat34_names;
		int *mat34_values;

		int num_float_properties;
		const char **float_names;
		int *float_values;
	} custom_tracked_device_properties;
	
	void set_default();
	
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

// should the tracker always return cursors for all interfaces?
// NO.  sometimes I don't want the old renderstatistics.  I want to replay
// and record new render statistics.

typedef void *vr_state_tracker_t;
vr_state_tracker_t create_vr_state_tracker(const TrackerConfig &c);
void capture_vr_state(vr_state_tracker_t h,
						openvr_broker::open_vr_interfaces &interfaces);

// return the first and last frames in the current tracker state
void get_frame_range(vr_state_tracker_t, int *first_frame, int *last_frame);

void capture_vr_event(vr_state_tracker_t h, const vr::VREvent_t &event);
void capture_vr_overlay_event(vr_state_tracker_t h, vr::VROverlayHandle_t overlay_handle, const vr::VREvent_t &event);

void update_vr_config_near_far(vr_state_tracker_t h, float nearz, float farz);
void update_vr_config_bool_setting(vr_state_tracker_t h, const char *section, const char *key);
void update_vr_config_int32_setting(vr_state_tracker_t h, const char *section, const char *key);
void update_vr_config_float_setting(vr_state_tracker_t h, const char *section, const char *key);
void update_vr_config_string_setting(vr_state_tracker_t h, const char *section, const char *key);


// explicit tracking adds
//void add_tracking_on_resource(resource_directory, resource_filename);
//void add_tracking_on_args(resource_directory, resource_filename);

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
void set_cursor_to_start_frame(vr_state_tracker_t hstate, vr_cursor_t hcursor);
void set_cursor_to_end_frame(vr_state_tracker_t hstate, vr_cursor_t hcursor);
void advance_cursor_one_frame(vr_state_tracker_t hstate, vr_cursor_t hcursor);
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
