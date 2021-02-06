#pragma once

#include <openvr.h>


#include <string> // blargh

// consider exposing direct references to the nodes to 
// the ui - let the ui decide what level of abstraction it wants.
#if 0
VRVariant
{
	union
	{
		history_base<HmdMatrix34_t, AlwaysAndForever> *mat34;
	};

}


// I'd like tracker node if to expose raw types and not strings
// TrackerNodeIF

// TrackerNodeIF already has the history node inside it
{
	enum ValType
	{
		MAT_34
	};
	ValType getValType()
	&getValAsMat_34;

	
	enum RCType
	{
		AlwaysAndForever,
		etc
	}
	&RCType getRCType();

}

//I'd like to be able to walk backwards in time
// change of state iterator?
	cos = start_iterator;
	cos.get_next();
	cos.peek_next();
	cos.get_latest();


struct VAL2
{
	int *GetFrameTime();
	GetPresenceValueAsVRSettingsError();
	
	GetValueAsMat34();
	GetValueAs..();
};

struct ChangeOfStateIter2
{
	virtual Mat34* getValAsMat34();

	virtual change_of_state* get_next() = 0;
	virtual change_of_state* peek_next() = 0;
	virtual change_of_state* get_latest() = 0;
};

struct TrackerNodeIF2
{
	virtual const char *GetLabel() = 0;
	virtual const char *GetLabel(int *size) = 0;

	enum ValType
	{
		MAT_34
	};
	ValType getValType()

	virtual int GetChildCount() = 0;
	virtual TrackerNodeIF *GetChild(int index) = 0;
	virtual TrackerNodeIF *GetParent() = 0;
	virtual int GetMyIndexInParent() = 0;

	virtual ChangeOfStateIterator *start_iterator(int start_frame, int end_frame) = 0;
};

#endif





//
// a view into the data
// 
struct change_of_state
{
	virtual int GetFrameNumber() = 0;
	virtual int GetPrevFrameNumber() = 0;
	virtual void *unique_id() = 0;
	virtual std::string GetChangeDescriptionString() = 0;
	virtual std::string GetPrevDescriptionString() = 0;
};

struct TrackerNodeIF
{
	virtual const char *GetLabel() = 0;
	virtual const char *GetLabel(int *size) = 0;

	virtual int GetChildCount() = 0;
	virtual TrackerNodeIF *GetChild(int index) = 0;
	virtual TrackerNodeIF *GetParent() = 0;
	virtual int GetMyIndexInParent() = 0;

	virtual const char *GetPopupMenuLabel() = 0;
	virtual std::string GetChangeDescriptionString() = 0;

	virtual bool has_changes() = 0;
	virtual bool start_iterator(int start_frame, int end_frame) = 0;
	virtual change_of_state* get_next() = 0;
	virtual change_of_state* peek_next() = 0;
	virtual change_of_state* get_latest() = 0;
};

TrackerNodeIF *get_node_if(vr_state_tracker_t);

