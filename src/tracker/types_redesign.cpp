#include <openvr.h>
#include "openvr_string_std.h"
#include "openvr_operators.h"
#include "openvr_serialization.h"
#include "openvr_softcompare.h"
#include "openvr_cppstub.h"
#include "vrdelta.h"
#include "dprintf.h"

#include <chrono>
#include <type_traits>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <forward_list>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <ctime>

#ifdef HAVE_IMG_GUI
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <imgui.h>
#endif


static void sleep_ms(unsigned long ms)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1ms);
}

// coordinate spaces
// overlays. can be Absolute, TrackedDeviceRelative,SystemOverlay, TrackedComponent
//	overlay_schema keyboard
// components
// controllers
// tracked cameras
#ifdef TIMERS_ENABLED
#include "Timer.h"
static int TWRAP_NUM_TIMERS = 0;
static const int TWRAP_MAX_TIMERS = 132;
static const int TWRAP_MAX_SAMPLES = 1;
Timer TWRAP_timers[TWRAP_MAX_TIMERS][TWRAP_MAX_SAMPLES];
int TWRAP_num_samples[TWRAP_MAX_TIMERS];
static const char *TWRAP_timer_names[TWRAP_MAX_TIMERS];

struct twrap
{
	twrap(const char *name_in)
		:
		timer_id(TWRAP_NUM_TIMERS++)
	{
		assert(timer_id < TWRAP_MAX_TIMERS);
		if (timer_id < TWRAP_MAX_TIMERS && TWRAP_num_samples[timer_id] < TWRAP_MAX_SAMPLES)
		{
			sample_id = TWRAP_num_samples[timer_id];
			TWRAP_timer_names[timer_id] = name_in;
			TWRAP_timers[timer_id][sample_id].start();
		}
	}

	~twrap()
	{
		if (TWRAP_num_samples[timer_id] < TWRAP_MAX_SAMPLES)
		{
			TWRAP_timers[timer_id][sample_id].stop();
			TWRAP_num_samples[timer_id]++;
		}

	}
	int timer_id;
	int sample_id;
	Timer t;
};
#else
struct twrap
{
	twrap(const char *) {}
};
#endif

#define TBL_SIZE(t) (sizeof(t)/sizeof(t[0]))

template <typename U>
struct property_row
{
	U enum_val;
	const char *name;
};

using device_property_row = property_row<vr::ETrackedDeviceProperty>;
using app_property_row = property_row<vr::EVRApplicationProperty>;

using namespace vr;
#include "openvr_delta_cfgtables.cpp"

struct AlwaysAndForever
{};

inline bool operator == (const AlwaysAndForever &lhs, const AlwaysAndForever &rhs)
{
	return true;
}

namespace openvr_string
{
	const char *GetAsString(AlwaysAndForever)
	{
		return "Always";
	}

	const char *GetAsString(const bool b)
	{
		if (b == true)
		{
			return "true";
		}
		else
		{
			return "false";
		}
	}
};

// the following type traits for enums used by the history logic to detect if an error code implies that the value
// part is valid
template <typename T> 
struct PresenceValue
{
	static const T is_present;
};

// specializations
template <> struct PresenceValue <EVRSettingsError> 
{ static const EVRSettingsError is_present = VRSettingsError_None; };

template <> struct PresenceValue <ETrackedPropertyError> 
{static const ETrackedPropertyError is_present = TrackedProp_Success; };

template <> struct PresenceValue <bool>
{ static const bool is_present = true; };

template <> struct PresenceValue <EVRTrackedCameraError>
{ static const bool is_present = VRTrackedCameraError_None; };

template <> struct PresenceValue <EVRCompositorError>
{ static const bool is_present = VRCompositorError_None;};

template <> struct PresenceValue <EVRRenderModelError>
{ static const bool is_present = VRCompositorError_None;};

template <> struct PresenceValue <EVRApplicationError>
{
	static const bool is_present = VRApplicationError_None;
};

template <> struct PresenceValue <EVROverlayError>
{
	static const bool is_present = VROverlayError_None;
};

template <typename T, typename U>
struct stringifier
{
	static std::string stringify(const std::vector<T, U> &v)
	{
		using namespace std;
		using namespace openvr_string;

		std::string s;
		int i = 0;
		for (auto iter = v.begin(); iter != v.end(); iter++)
		{
			i++;
			s += to_string(i) + to_string(*iter) + "\n";
		}
		return s;
	}
};

// specialize for vectors of char
template <typename U>
struct stringifier<char, U>
{
	static std::string stringify(const std::vector<char, U> &v)
	{
		std::string s(v.begin(), v.end());
		return s;
	}
};

template <typename U>
struct stringifier<uint8_t, U>
{
	static std::string stringify(const std::vector<uint8_t, U> &v)
	{
		std::string s(v.begin(), v.end());
		return s;
	}
};

template <typename T, typename U>
std::string to_string(const std::vector<T, U> &v)
{
	return stringifier<T, U>::stringify(v);
}

static void indented_output(std::ostream &ofs, std::string s, int indent_level)
{
	for (int i = 0; i < indent_level; i++)
	{
		ofs << "   ";
	}
	ofs << s;
}

namespace vrtypes
{
struct slab
{
	static int slab_num_slabs;
	static int slab_total_slab_page_allocs;
	static int slab_total_slab_page_frees;
	static int slab_num_alloc_calls;

	std::forward_list<char *> pages;
	int		page_size;
	int64_t current_page_pos;

	slab(int page_size_in)
		: page_size(page_size_in)
	{
		slab_num_slabs++;
		current_page_pos = 0;
		slab_alloc(0);
	}

	~slab()
	{
		for (auto iter = pages.begin(); iter != pages.end(); iter++)
		{
			char *mem = *iter;
			free(mem);
			slab_total_slab_page_frees += 1;
		}
		pages.clear();
		slab_num_slabs--;
	}

	void *slab_alloc(size_t size)
	{
		assert((int)size < page_size);
		size = (size + 3) & ~0x3;
		slab_num_alloc_calls += 1;

		// critical start
		if (pages.empty() || (int)size + current_page_pos > page_size)
		{
			current_page_pos = 0;
			slab_total_slab_page_allocs += 1;
			char *page = (char *)malloc(page_size);
			pages.push_front(page);
		}

		char *ret = pages.front();
		ret += current_page_pos;
		current_page_pos += size;
		// critical end

		return ret;
	}
};


struct slab_allocator_base
{
	static int slab_allocators_constructed;
	static int slab_allocators_destroyed;
	static int slab_allocators_leaks;
};

template <class T>
struct slab_allocator : slab_allocator_base {
public:
	typedef T		   value_type;
	typedef T*         pointer;
	typedef const T&   const_reference;

	slab* m_slab;

	// stl likes to make these evil ones for some reason
	slab_allocator()
		: m_slab(nullptr)
	{
		slab_allocators_constructed++;
	}

	slab_allocator(slab *slab)
	{
		m_slab = slab;
		slab_allocators_constructed++;
	}

	slab_allocator(const slab_allocator &rhs)
	{
		m_slab = rhs.m_slab;
		slab_allocators_constructed++;
	}

	template<typename X, typename Y>
	struct rebind { using other = slab_allocator<T>; };

	~slab_allocator()
	{
		slab_allocators_destroyed++;
	}

	template <class U> slab_allocator(const slab_allocator<U>& other)
	{
		m_slab = other.m_slab;
		slab_allocators_constructed++;
	}

	T* allocate(std::size_t n)
	{
		T* p = (T*)m_slab->slab_alloc(n * sizeof(T));
		return (T*)p;
	}
	void deallocate(T* p, std::size_t n)
	{
		slab_allocators_leaks++;
	}
};
}

#define ALLOCATOR_TYPE vrtypes::slab_allocator<char>
#define ALLOCATOR_DECL ALLOCATOR_TYPE &allocator
#define INIT(var_name)					var_name( #var_name, allocator)

namespace vrtypes
{
	template <typename T, typename P, typename AllocatorT = ALLOCATOR_TYPE>
	struct history_entry_base
	{
		history_entry_base() = default;

		template<typename... Args>
		history_entry_base(int frame_number_in, P presence_in, Args&&... args)
			: presence(presence_in),
			frame_number(frame_number_in),
			val(std::forward<Args>(args)...)
		{
		}
		history_entry_base(int frame_number_in, P presence_in, const T& val_in)
			:
			presence(presence_in),
			frame_number(frame_number_in),
			val(val_in)
		{}

		bool is_present() const
		{
			return (PresenceValue<P>::is_present == presence);
		}

		std::string GetChangeDescriptionString()
		{
			using namespace openvr_string;
			using namespace std;
			std::string s;

			if (PresenceValue<P>::is_present == presence)
			{
				s += to_string(val);
			}
			else
			{
				s += to_string(presence) + "\n";
			}
			return s;
		}

		void dump(std::ostream &ofs, int indent_level)
		{
			using namespace openvr_string;
			using namespace std;
			indented_output(ofs, std::string("frame_number:") + to_string(frame_number) + "\n", indent_level);
			indented_output(ofs, std::string("presence:") + to_string(presence) + "\n", indent_level);
			if (PresenceValue<P>::is_present == presence)
			{
				indented_output(ofs, to_string(val), indent_level);
			}
			ofs << "\n";
		}
		int frame_number;
		P presence;
		T val;
	};

	template <typename T, typename AllocatorT>
	struct history_entry_base <T, AlwaysAndForever, AllocatorT>
	{
		history_entry_base() = default;

		template<typename... Args>
		history_entry_base(int frame_number_in, Args&&... args)
			:
			frame_number(frame_number_in),
			val(std::forward<Args>(args)...)
		{}

		history_entry_base(int frame_number_in, const T& val_in)
			:
			frame_number(frame_number_in),
			val(val_in)
		{}

		bool is_present() const
		{
			return true;
		}

		std::string GetChangeDescriptionString()
		{
			using namespace openvr_string;
			using namespace std;
			std::string s;
			s += to_string(val);
			return s;
		}

		void dump(std::ostream &ofs, int indent_level)
		{
			using namespace openvr_string;
			using namespace std;
			indented_output(ofs, std::string("frame_number:") + to_string(frame_number) + "\n", indent_level);
			indented_output(ofs, to_string(val), indent_level);
			ofs << "\n";
		}
		int frame_number;
		T val;
	};

	// represent the history of a value
	// 
	template <typename T, typename P, typename AllocatorT = ALLOCATOR_TYPE>
	struct history_base
	{
		typedef T val_type;
		using history_entry = history_entry_base<T, P, AllocatorT>;

		typedef typename std::forward_list<history_entry, AllocatorT>::iterator iter_type;

		history_base(const AllocatorT& alloc) : name(nullptr), values(alloc)
		{}

		history_base(const char* name_in, const AllocatorT& alloc)
			: name(name_in), values(alloc)
		{}

		bool empty() const { return values.empty(); }

		bool more_than_2_values() const
		{
			int count = 0;
			for (auto iter = values.cbegin(); iter != values.end(); iter++)
			{
				count++;
				if (count > 1)
				{
					return true;
				}
			}
			return false;
		}
		const history_entry & front() const { return values.front(); }
		const T& latest() const
		{
			return values.front().val;
		}

		template<typename... Args>
		void base_emplace_front(int frame_number, Args&&... args)
		{
			values.emplace_front(frame_number, std::forward<Args>(args)...);
		}

		void dump(std::ostream &ofs, int indent_level)
		{
			indented_output(ofs, "history " + std::string(name) + "\n", indent_level);
			for (auto iter = values.begin(); iter != values.end(); iter++)
			{
				iter->dump(ofs, indent_level);
			}
			ofs << "\n";
		}
		const char *name;
		std::forward_list<history_entry, AllocatorT> values;
	};

	// history with Presence values
	template <typename T, typename P, typename AllocatorT = ALLOCATOR_TYPE>
	struct history : public history_base<T, P, AllocatorT>
	{

		history(const AllocatorT& alloc) : history_base<T,P,AllocatorT>(alloc)
		{}

		history(const char* name_in, const AllocatorT& alloc)
			: history_base<T, P, AllocatorT>(name_in, alloc)
		{}

		const P& latest_P() const
		{
			return this->front().presence;
		}

		template<typename... Args>
		void emplace_front(int frame_number, P presence, Args&&... args)
		{
			base_emplace_front(frame_number, presence, std::forward<Args>(args)...);
			//values.emplace_front();
		}

	};

	// history with no Presence values
	template <typename T, typename AllocatorT>
	struct history<T, AlwaysAndForever, AllocatorT> : public history_base<T, AlwaysAndForever, AllocatorT>
	{
		history(const AllocatorT& alloc) : history_base<T, AlwaysAndForever, AllocatorT>(alloc)
		{}

		history(const char* name_in, const AllocatorT& alloc)
			: history_base<T, AlwaysAndForever, AllocatorT>(name_in, alloc)
		{}

		template<typename... Args>
		void emplace_front(int frame_number, Args&&... args)
		{
			base_emplace_front(frame_number, std::forward<Args>(args)...);
			//values.emplace_front(frame_number, std::forward<Args>(args)...);
		}

	};
}
	


template <typename T, typename P, typename allocatorT>
struct serializer_base
{
	typedef typename vrtypes::history<T, P, allocatorT> history_scalar_node_type;
	typedef vrtypes::history<std::vector<T, allocatorT>, P, allocatorT> history_vector_node_type;

	static void encode_history(history_scalar_node_type &h, EncodeStream &e)
	{
		encode(h.name, e); // leak: the vrstate sets the names at construction time

		int entry_count = 0;
		for (auto iter = h.values.begin(); iter != h.values.end(); iter++)
		{
			entry_count++;
		}
		encode(entry_count, e);

		for (auto iter = h.values.begin(); iter != h.values.end(); iter++)
		{
			const typename history_scalar_node_type::history_entry &val = *iter;
			e.memcpy_out_to_stream(&val, sizeof(val));
		}
	}
};

template <typename T, typename P, typename allocatorT>
struct serializer : serializer_base<T, P, allocatorT>
{
	static void vector_encode_history(typename serializer_base<T, P, allocatorT>::history_vector_node_type &h, EncodeStream &e)
	{
		encode(h.name, e);
		
		// num entries
		int entry_count = 0;
		for (auto iter = h.values.begin(); iter != h.values.end(); iter++)
		{
			entry_count++;
		}
		encode(entry_count, e);

		// writes from newest to oldest
		for (auto iter = h.values.begin(); iter != h.values.end(); iter++)
		{
			auto &history_entry = *iter;

			// write the frame_number
			encode(history_entry.frame_number, e);
			encode(history_entry.presence, e);

			// write the vector size
			int inner_entry_count = (int)history_entry.val.size();
			encode(inner_entry_count, e);
			if (inner_entry_count > 0)
			{
				// write the simple type
				e.memcpy_out_to_stream(&history_entry.val[0], 
					sizeof(typename serializer_base<T, P, allocatorT>::history_vector_node_type::val_type::value_type) * inner_entry_count); // this will work for all basic types (scalar_history)
			}
		}
	}
};

// name | num_history_entries | [ frame_number, {Prescence,} vec_size, [DATA]* ]*

template <typename T, typename allocatorT>
struct serializer<T,AlwaysAndForever,allocatorT> : serializer_base<T, AlwaysAndForever, allocatorT>
{
	static void vector_encode_history(typename serializer_base<T, AlwaysAndForever, allocatorT>::history_vector_node_type &h, EncodeStream &e)
	{
		encode(h.name, e);

		// num entries
		int entry_count = 0;
		for (auto iter = h.values.begin(); iter != h.values.end(); iter++)
		{
			entry_count++;
		}
		encode(entry_count, e);
	
		// writes from newest to oldest
		for (auto iter = h.values.begin(); iter != h.values.end(); iter++)
		{
			auto &history_entry = *iter;

			// write the frame_number
			encode(history_entry.frame_number, e);

			// write the vector size
			int inner_entry_count = (int)history_entry.val.size();
			encode(inner_entry_count, e);
			if (inner_entry_count > 0)
			{
				// write the simple type
				e.memcpy_out_to_stream(&history_entry.val[0], 
					sizeof(typename serializer_base<T, AlwaysAndForever, allocatorT>::history_vector_node_type::val_type::value_type) * inner_entry_count); // this will work for all basic types (scalar_history)
			}
		}
	}
};

template <typename T, typename P, typename allocatorT>
struct deserializer_base
{
	typedef vrtypes::history<T, P, allocatorT> history_scalar_node_type;
	typedef vrtypes::history<std::vector<T, allocatorT>, P, allocatorT> history_vector_node_type;

	static void decode_history(history_scalar_node_type &h, EncodeStream &e, allocatorT &allocator)
	{
		char buf[256];
		decode_str(buf, e);
		assert(strcmp(buf, h.name) == 0);
		//decode(h.name, e, allocator); // leak: the vrstate sets the names at construction time

		int entry_count;
		decode(entry_count, e);
		for (int i = 0; i < entry_count; i++)
		{
			typename history_scalar_node_type::history_entry val;
			e.memcpy_from_stream(&val, sizeof(val));
			h.values.emplace_front(val);
		}
		h.values.reverse();
	}
};

// name | num_history_entries | [ frame_number, {Prescence,} vec_count, [DATA]* ]*
template <typename T, typename P, typename allocatorT>
struct deserializer : public deserializer_base<T,P, allocatorT>
{
	typedef typename deserializer_base<T, P, allocatorT>::history_vector_node_type base_node_type;
	typedef typename deserializer_base<T, P, allocatorT>::history_vector_node_type::val_type::value_type base_val_type;

	static void vector_decode_history(base_node_type &h, EncodeStream &e, allocatorT &allocator)
	{
		char buf[256];
		decode_str(buf, e);
		assert(strcmp(buf, h.name) == 0);

		int outer_entry_count;				// the number of history entries
		decode(outer_entry_count, e);

		// do the inner values				// every history entry is going to have an inner vector of values
		for (int i = 0; i < outer_entry_count; i++)
		{
			int frame_number;
			decode(frame_number, e);	// there might be a presence value here - based on the T type

			P presence_val;
			decode(presence_val, e);

			int vec_count;
			decode(vec_count, e);

			// how do I construct a history entry that holds a vector from a  buffer
			char *raw_char_ptr = &e.encoded_buf[e.buf_pos];
			base_val_type *raw_array = (base_val_type *)(raw_char_ptr);
			e.buf_pos += vec_count * sizeof(base_val_type);

			// super constructor
			// emplace a history node:
			//	{frame_number, 
			//	 presence,
			//   vector(allocator)
			h.values.emplace_front(frame_number, presence_val, raw_array, raw_array+vec_count, allocator);
			//h.values.emplace_front(frame_number, presence_val, (size_t) vec_count, raw_array[0], allocator);
			//for (int j = 0; j < vec_count; j++)
			//{
			//	h.values.front().val.emplace_back(allocator);
			//}
			//h.values.front().val.resize(vec_count);
			//memcpy(&h.values.front().val[0], raw_char_ptr, vec_count * sizeof(history_vector_node_type::val_type::value_type));
		}
		h.values.reverse();
	}
};

template <typename T,typename allocatorT>
struct deserializer <T, AlwaysAndForever, allocatorT>  : public deserializer_base<T, AlwaysAndForever, allocatorT>
{
	typedef typename deserializer_base<T, AlwaysAndForever, allocatorT>::history_scalar_node_type base_scalar_node_type;

	typedef typename deserializer_base<T, AlwaysAndForever, allocatorT>::history_vector_node_type base_vector_node_type;
	typedef typename deserializer_base<T, AlwaysAndForever, allocatorT>::history_vector_node_type::val_type::value_type base_val_type;

	static void decode_history(base_scalar_node_type &h, EncodeStream &e, allocatorT &allocator)
	{
		char buf[256];
		decode_str(buf, e);
		assert(strcmp(buf, h.name) == 0);

		//assert(strcmp(buf, h.name) == 0);
		//decode(h.name, e, allocator); // leak: the vrstate sets the names at construction time

		int entry_count;
		decode(entry_count, e);
		for (int i = 0; i < entry_count; i++)
		{
			typename base_scalar_node_type::history_entry val;
			e.memcpy_from_stream(&val, sizeof(val));
			h.values.emplace_front(val);
		}
		h.values.reverse();
	}

	static void vector_decode_history(base_vector_node_type &h, EncodeStream &e, allocatorT &allocator)
	{
		char buf[256];
		decode_str(buf, e);
		assert(strcmp(buf, h.name) == 0);

		//decode(h.name, e, allocator);

		int outer_entry_count;				// the number of history entries
		decode(outer_entry_count, e);

		// do the inner values				// every history entry is going to have an inner vector of values
		for (int i = 0; i < outer_entry_count; i++)
		{
			int frame_number;
			decode(frame_number, e);	// there might be a presence value here - based on the T type

			int vec_count;
			decode(vec_count, e);
			// how do I construct a history entry that holds a vector from a  buffer
			char *raw_char_ptr = &e.encoded_buf[e.buf_pos];
			base_val_type *raw_array = (base_val_type *)(raw_char_ptr);

			e.buf_pos += vec_count * sizeof(base_val_type);

			// super constructor
			// emplace a history node:
			//	{frame_number, 
			//	 presence,
			//   vector(allocator)
			h.values.emplace_front(frame_number, raw_array, raw_array + vec_count, allocator);
		}
		h.values.reverse();
	}
};

using namespace vrtypes;

int vrtypes::slab_allocator_base::slab_allocators_constructed;
int vrtypes::slab_allocator_base::slab_allocators_destroyed;
int vrtypes::slab_allocator_base::slab_allocators_leaks;

int vrtypes::slab::slab_num_slabs;
int vrtypes::slab::slab_total_slab_page_allocs;
int vrtypes::slab::slab_total_slab_page_frees;
int vrtypes::slab::slab_num_alloc_calls;

struct StringPool
{
	static const int TMP_BUF_BYTES = vr::k_unMaxPropertyStringSize;

	StringPool()
		:	string_pool_mallocs(0),
			string_pool_blocking_calls(0),
			string_pool_non_blocking_calls(0)
	{}

	char *AllocOne()
	{
		char *ret;
		if (pool_mutex.try_lock())
		{
			// got lock;
			string_pool_non_blocking_calls++;
		}
		else
		{
			string_pool_blocking_calls++;
			pool_mutex.lock();
		}

		if (string_pool.empty())
		{
			ret = (char *)malloc(TMP_BUF_BYTES);
			assert(ret);
			string_pool_mallocs++;
		}
		else
		{
			int last_index = string_pool.size() - 1;
			char *pch = string_pool[last_index];
			ret = pch;
			string_pool.pop_back();
		}

		pool_mutex.unlock();

		return ret;
	}

	void FreeOne(char *s)
	{
		if (pool_mutex.try_lock())
		{
			// got lock;
		}
		else
		{
			string_pool_blocking_calls++;
			pool_mutex.lock();
		}

		for (int i = 0; i < (int)string_pool.size(); i++)
		{
			// make sure duplicates don't appear back in the string pool
			assert(string_pool[i] != (char *)s);
		}
		string_pool.push_back((char *)s);

		pool_mutex.unlock();
	}
private:
	std::mutex pool_mutex;
	std::vector<char *> string_pool;
	int string_pool_mallocs;
	int string_pool_blocking_calls;
	int string_pool_non_blocking_calls;
};

template <typename T>
struct TMP 
{
	StringPool *p;
	T *s;

	static T* make_s(StringPool *p)
	{
		T *ret = (T*)p->AllocOne();
		return ret;
	}

	TMP(StringPool *p_in)
	{
		p = p_in;
		s = make_s(p);
	}

	TMP(const TMP&rhs)
	{
		assert(0);  // this assert is here to debug problems in uper level code which are not meant to use this
		s = make_s();
		memcpy(s, rhs.s, StringPool::TMP_BUF_BYTES);	// this is nasty because I have no idea how big the actual contents are
		p = rhs->p;
	}

	TMP &operator = (const TMP &rhs)
	{
		assert(0); // this assert is here to debug problems in uper level code which are not meant to use this
		memcpy(s, rhs.s, StringPool::TMP_BUF_BYTES);
		p = rhs->p;
		return *this;
	}

	void clear()
	{
		if (s)
		{
			p->FreeOne((char *)s);
			s = nullptr;
		}
	}

	~TMP()
	{
		clear();
	}

#if 0
	void reserve(unsigned int i)
	{
		assert(i < size());
	}
#endif
	T *buf()
	{
		return s;
	}

	const T *buf() const
	{
		return s;
	}

	unsigned int max_count() const 
	{
		return vr::k_unMaxPropertyStringSize/sizeof(T);
	}
};

// scalar result
template <typename T, typename ResultType>
struct scalar_result
{
	scalar_result() = default;

	scalar_result(const T&val_in, const ResultType &result_code_in)
		: val(val_in), result_code(result_code_in)
	{}

	~scalar_result()
	{}

	scalar_result(scalar_result &rhs)
	{
		val = rhs.val;
		result_code = rhs.result_code;
	}

	scalar_result &operator = (const scalar_result &rhs)
	{
		val = rhs.val;
		result_code = rhs.result_code;
		return *this;
	}

	scalar_result &operator = (scalar_result &&rhs)
	{
		val = std::move(rhs.val);
		result_code = rhs.result_code;
		return *this;
	}

	scalar_result(scalar_result &&rhs)
		: val(std::move(rhs.val))
	{
		result_code = rhs.result_code;
	}

	inline bool is_present() { return (PresenceValue<ResultType>::is_present == result_code); }

	T val;
	ResultType result_code;
};

// specialize for when there is never a result code associated with a value
template <typename T>
struct scalar_result<T, AlwaysAndForever>
{
	scalar_result() = default;

	scalar_result(const T&val_in)
		:val(val_in)
	{}

	~scalar_result()
	{}

	scalar_result(scalar_result &rhs)
	{
		val = rhs.val;
	}

	scalar_result &operator = (const scalar_result &rhs)
	{
		val = rhs.val;
		return *this;
	}

	scalar_result &operator = (scalar_result &&rhs)
	{
		val = std::move(rhs.val);
		return *this;
	}

	scalar_result(scalar_result &&rhs)
		: val(std::move(rhs.val))
	{}

	T val;
	
};

template <typename T>
using scalar = scalar_result<T, AlwaysAndForever>;

template <typename T, typename R = AlwaysAndForever>
scalar_result<T,R>  make_scalar(T val) {
	return scalar_result<T,R>(val);
}

template <typename T, typename R>
scalar_result<T, R>  make_scalar(T val, R result) {
	return scalar_result<T, R>(val, result);
}

// no result code
template <typename T, typename ResultType = AlwaysAndForever>
struct vector_result
{
	vector_result(StringPool *p)
	: s(p), count(0)
	{}

	~vector_result()
	{
	}

	vector_result(const vector_result &rhs)
		: s(rhs.s.p)
	{
		assert(rhs.s.p);
		result_code = rhs.result_code;
		count = rhs.count;
		if (rhs.count == 0)
		{
		}
		else
		{
			//
			memcpy(s.buf(), rhs.s.buf(), rhs.count * sizeof(T));
			// vs unchecked warning
			//std::copy(rhs.s.buf(), rhs.s.buf() + rhs.count, s.buf());
		}
		assert(s.p);
	}

	vector_result(vector_result &&rhs)
		: s(rhs.s.p)
	{
		assert(rhs.s.p);
		std::swap(s.s, rhs.s.s);
		count = rhs.count;
		result_code = rhs.result_code;
		assert(s.p);
	}

	vector_result& operator = (vector_result &&rhs)
	{
		assert(rhs.s.p);
		assert(this != &rhs);
		s.clear();
		std::swap(s.s, rhs.s.s);
		count = rhs.count;
		result_code = rhs.result_code;
		assert(s.p);
		return *this;
	}

	vector_result& operator = (const vector_result &rhs)
	{
		assert(rhs.s.p);
		if (this != &rhs)
		{
			if (rhs.count > 0)
			{
				std::copy(rhs.s.buf(), rhs.s.buf() + rhs.count, s.buf());
			}
			s.p = rhs.s.p;
			count = rhs.count;
			result_code = rhs.result_code;
		}
		assert(s.p);
		return *this;
	}
	
	inline bool is_present() { return (PresenceValue<ResultType>::is_present == result_code); }

	TMP<T> s;
	int count;
	ResultType result_code;
};

template <typename T>
struct vector_result<T, AlwaysAndForever>
{
	vector_result(StringPool *p)
	: s(p), count(0)
	{
		assert(s.p);
	}

	~vector_result()
	{}

	vector_result(const vector_result &rhs)
		: s(rhs.s.p)
	{
		assert(rhs.s.p);
		count = rhs.count;
		if (rhs.count != 0)
		{
			memcpy(s.buf(), rhs.s.buf(), rhs.count * sizeof(T));
			//// unchecked iterator working
			//std::copy(rhs.s.buf(), rhs.s.buf() + rhs.count, s.buf());
			count = 0;
		}
		assert(s.p);
	}

	vector_result(vector_result &&rhs)
		: s(rhs.s.p)
	{
		assert(rhs.s.p);
		std::swap(s.s, rhs.s.s);
		count = rhs.count;
		assert(s.p);
	}

	vector_result& operator = (vector_result &&rhs)
	{
		assert(rhs.s.p);
		assert(this != &rhs);
		s.clear();
		std::swap(s.s, rhs.s.s);
		count = rhs.count;
		assert(s.p);
		return *this;
	}

	vector_result& operator = (const vector_result &rhs)
	{
		assert(rhs.s.p);
		if (this != &rhs)
		{
			count = rhs.count;
			if (rhs.count != 0)
			{
				std::copy(rhs.s.buf(), rhs.s.buf() + rhs.count, s.buf());
			}
			s.p = rhs.s.p;
		}
		assert(s.p);
		return *this;
	}

	inline bool is_present() { return true; }

	TMP<T> s;
	int count;
};

//		             Inputs: 1,2
//		ERROR DETAILS PARAM: 1
//		        RETURN CODE: count
template<typename T, typename ResultType, typename InterfaceHandle, typename FunctionPtr,typename ...Params>
inline void query_vector_rccount(vector_result<T,ResultType> *result, InterfaceHandle *ifh, FunctionPtr function_ptr, Params... params)
{
	result->count = (ifh->*function_ptr)(params..., result->s.buf(), result->s.max_count(), &result->result_code);
	assert(result->count < (int)result->s.max_count());
}

//		             Inputs: 0,1
//		ERROR DETAILS PARAM: 0
//		        RETURN CODE: count
template<typename T, typename InterfaceHandle, typename FunctionPtr, typename ...Params>
void query_vector_rccount(vector_result<T> *result, InterfaceHandle *ifh, FunctionPtr function_ptr, Params... params)
{
	result->count = (ifh->*function_ptr)(params..., result->s.buf(), result->s.max_count());
	assert(result->count < (int)result->s.max_count());
}

template<typename T, typename InterfaceHandle, typename FunctionPtr, typename ...Params>
inline void query_vector_zero_means_not_present(vector_result<T, bool> *result, InterfaceHandle *ifh, FunctionPtr function_ptr, Params... params)
{
	result->count = (ifh->*function_ptr)(params..., result->s.buf(), result->s.max_count());
	result->result_code = result->count != 0;
	assert(result->count < (int)result->s.max_count());
}


// rccount with a wrinkle: the count is a pointer - so you can call it as a query interface
// virtual bool GetWorkingCollisionBoundsInfo(VR_OUT_ARRAY_COUNT(punQuadsCount) HmdQuad_t *pQuadsBuffer, uint32_t* punQuadsCount) = 0;
// virtual bool GetLiveCollisionBoundsInfo(VR_OUT_ARRAY_COUNT(punQuadsCount) HmdQuad_t *pQuadsBuffer, uint32_t* punQuadsCount) = 0;
// virtual bool GetLiveCollisionBoundsTagsInfo(VR_OUT_ARRAY_COUNT(punTagCount) uint8_t *pTagsBuffer, uint32_t *punTagCount) = 0;
// virtual bool ExportLiveToBuffer(VR_OUT_STRING() char *pBuffer, uint32_t *pnBufferLength) = 0;
template<typename T, typename InterfaceHandle, typename FunctionPtr, typename ...Params>
void query_vector_rcbool_wrinkle_form(vector_result<T, bool> *result, InterfaceHandle *ifh, FunctionPtr function_ptr, Params... params)
{
	// ask how big it is
	uint32_t instance_count = 0;  // it is important that this is set to zero
	/*bool rc =*/ (ifh->*function_ptr)(params..., nullptr, &instance_count);
	{
		// I don't expect the following assert to be hit, since the tmp buffer is currently 16Kb
		// however put it here in case I need to handle this case
		assert(instance_count < result->s.max_count());
		uint32_t count = instance_count;
		bool rc = (ifh->*function_ptr)(params..., result->s.buf(), &count);
		result->result_code = rc;
		result->count = count;
	}
}

//
// virtual EVRApplicationError GetApplicationKeyByIndex(uint32_t unApplicationIndex, VR_OUT_STRING() char *pchAppKeyBuffer, uint32_t unAppKeyBufferLen) = 0;

//
// FunctionPtr:
//		 ERROR DETAILS: 0
//		   RETURN CODE: Error
template<typename T, typename ResultType, typename InterfaceHandle, typename FunctionPtr, typename ...Params>
void query_vector_rcerror(vector_result<T, ResultType> *result, InterfaceHandle *ifh, FunctionPtr function_ptr, Params... params)
{
	result->result_code = (ifh->*function_ptr)(params..., result->s.buf(), result->s.max_count());
	result->count = (int)strlen(result->s.buf()) + 1; // gross because it's assuming char and I want a null byte
}

// getString returns void
//virtual void GetString(
//	const char *pchSection,
//	const char *pchSettingsKey,
//	uint32_t unValueLen,
//	EVRSettingsError *peError = nullptr) = 0;
template<typename T, typename ResultType, typename InterfaceHandle, typename FunctionPtr, typename ...Params>
void query_vector_rcvoid(vector_result<T, ResultType> *result, InterfaceHandle *ifh, FunctionPtr function_ptr, Params... params)
{
	(ifh->*function_ptr)(params..., result->s.buf(), result->s.max_count(), &result->result_code);
	result->count = (int)strlen(result->s.buf()) + 1; // gross because it's assuming char  and I want a null byte
}

// returns void and takes no params ... guaranteed to work
// virtual void GetDeviceToAbsoluteTrackingPose(ETrackingUniverseOrigin eOrigin, float fPredictedSecondsToPhotonsFromNow,
// VR_ARRAY_COUNT(unTrackedDevicePoseArrayCount) TrackedDevicePose_t *pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount) = 0;
template<typename T, typename InterfaceHandle, typename FunctionPtr, typename ...Params>
void query_vector_rcvoid(vector_result<T> *result, InterfaceHandle *ifh, FunctionPtr function_ptr, Params... params)
{
	(ifh->*function_ptr)(params..., result->s.buf(), result->s.max_count());
	result->count = (int)strlen(result->s.buf()) + 1; // gross because it's assuming char  and I want a null byte
}

// scalar query
// virtual EVROverlayError GetOverlaySortOrder(VROverlayHandle_t ulOverlayHandle, uint32_t *punSortOrder) = 0;
template<typename T, typename ResultType, typename InterfaceHandle, typename FunctionPtr, typename ...Params> 
void query_scalar_rcerror(scalar_result<T,ResultType> *result, InterfaceHandle *ifh, FunctionPtr function_ptr, Params... params)
{
	result->result_code = (ifh->*function_ptr)(params..., &result->val);
}

#define ALLOCATOR_TYPE vrtypes::slab_allocator<char>
#define ALLOCATOR_DECL ALLOCATOR_TYPE &allocator
#define INIT(var_name)					var_name( #var_name, allocator)

#define VSDECL(var_name, presence_type, type_name)	std::vector<HistoryNodeOrIterator<type_name, presence_type, bIsIterator>, ALLOCATOR_TYPE> var_name

//std::vector<vrtypes::history<std::vector<char, ALLOCATOR_TYPE>, EVRSettingsError, ALLOCATOR_TYPE>, ALLOCATOR_TYPE> nodes;
#define VVDECL(var_name, presence_type, type_name)	std::vector<HistoryVectorNodeOrIterator<type_name, presence_type, bIsIterator>, ALLOCATOR_TYPE> var_name

// single value history
#define SDECL(var_name, presence_type, type_name)	HistoryNodeOrIterator<type_name, presence_type, bIsIterator> var_name
// vector of values history
#define VDECL(var_name, presence_type, type_name) HistoryVectorNodeOrIterator<type_name, presence_type, bIsIterator> var_name


// to relate the followng 4 c
struct HistoryNode{};
struct HistoryIterator{};

template<typename T, typename P, bool IsIterator>
struct HistoryNodeOrIterator : HistoryIterator
{
	HistoryNodeOrIterator(const char* name_in, ALLOCATOR_DECL)
		: iter_frame(-1)
	{}

	typename vrtypes::history<T, P, ALLOCATOR_TYPE>::iter_type item;
	int iter_frame;
};

template<typename T, typename P>
struct HistoryNodeOrIterator<T, P, false> : HistoryNode
{
	HistoryNodeOrIterator(const char* name_in, ALLOCATOR_DECL)
		: item(name_in, allocator) {}
	vrtypes::history<T, P, ALLOCATOR_TYPE> item;
};

// history of vectors (as opposed to history of scalars)
template<typename T, typename P, bool IsIterator>
struct HistoryVectorNodeOrIterator : HistoryIterator
{
	HistoryVectorNodeOrIterator(const char* name_in, ALLOCATOR_DECL)
		: iter_frame(-1)
	{}

	typedef vrtypes::history<std::vector<T, ALLOCATOR_TYPE>, P, ALLOCATOR_TYPE> node_type;
	typename node_type::iter_type item;
	int iter_frame;
};

template<typename T, typename P>
struct HistoryVectorNodeOrIterator<T, P, false> : HistoryNode
{
	HistoryVectorNodeOrIterator(const char* name_in, ALLOCATOR_DECL)
		: item(name_in, allocator) {}

	typedef vrtypes::history<std::vector<T, ALLOCATOR_TYPE>, P, ALLOCATOR_TYPE> node_type;
	node_type item;
};


template <bool bIsIterator>
struct vrschema
{
	// T is the value type (eg float, bool) 
	// PropertyIDT is the id type in the row (e.g. ETrackedDeviceProperty)
	// PropertyErrorT is the error return type
	template <typename T, typename PropertyIDT, typename PropertyErrorT>
	struct properties_subtable
	{
		properties_subtable(const property_row<PropertyIDT> *tbl_in, int tbl_size_in, ALLOCATOR_DECL)
			:
			tbl(tbl_in),
			tbl_size(tbl_size_in),
			props(allocator)
		{
			props.reserve(tbl_size);
			for (int i = 0; i < tbl_size; i++)
			{
				// create an entry in the vector table per known property name
				props.emplace_back(tbl[i].name, allocator);
			}
		}

		// property tables aren't too long so maybe linear search isn't that bad
		int get_index_for_property(PropertyIDT key)
		{
			for (int i = 0; i < tbl_size; i++)
			{
				if (tbl[i].enum_val == key)
				{
					return i;
				}
			}
			return -1;
		}

		const property_row<PropertyIDT> *tbl;
		const int tbl_size;
		VSDECL(props, PropertyErrorT, T);
	};

	struct hidden_mesh_schema
	{
		hidden_mesh_schema(ALLOCATOR_DECL)
			:
			INIT(hidden_mesh_triangle_count),
			INIT(hidden_mesh_vertices)
		{}

		SDECL(hidden_mesh_triangle_count, AlwaysAndForever, uint32_t);
		VDECL(hidden_mesh_vertices, AlwaysAndForever, HmdVector2_t);
	};

	struct eye_schema
	{
		eye_schema(ALLOCATOR_DECL)
			:
			INIT(projection),
			INIT(projection_raw),
			INIT(distortion),
			INIT(eye2head),
			hidden_meshes(allocator)
		{}

		SDECL(projection, AlwaysAndForever, HmdMatrix44_t);
		SDECL(projection_raw, AlwaysAndForever, HmdVector4_t);
		SDECL(distortion, bool, DistortionCoordinates_t);
		SDECL(eye2head, AlwaysAndForever, HmdMatrix34_t);
		std::vector<hidden_mesh_schema, ALLOCATOR_TYPE> hidden_meshes;
	};

	struct component_on_controller_schema
	{
		component_on_controller_schema(ALLOCATOR_DECL)
			: 
			INIT(transforms),
			INIT(transforms_scroll_wheel)
		{}

		SDECL(transforms, bool, RenderModel_ComponentState_t);
		SDECL(transforms_scroll_wheel, bool, RenderModel_ComponentState_t);
	};
	
	struct system_controller_schema
	{
		system_controller_schema(ALLOCATOR_DECL)
			:
			INIT(raw_tracking_pose),
			INIT(standing_tracking_pose),
			INIT(seated_tracking_pose),
			INIT(activity_level),
			INIT(controller_role),
			INIT(device_class),
			INIT(connected),
			INIT(controller_state),
			INIT(synced_seated_pose),
			INIT(synced_standing_pose),
			INIT(synced_raw_pose),
			string_props(device_string_properties_table, TBL_SIZE(device_string_properties_table), allocator),
			bool_props(device_bool_properties_table, TBL_SIZE(device_bool_properties_table), allocator),
			float_props(device_float_properties_table, TBL_SIZE(device_float_properties_table), allocator),
			mat34_props(device_mat34_properties_table, TBL_SIZE(device_mat34_properties_table), allocator),
			int32_props(device_int32_properties_table, TBL_SIZE(device_int32_properties_table), allocator),
			uint64_props(device_uint64_properties_table, TBL_SIZE(device_uint64_properties_table), allocator),
			components(allocator)
		{}

		SDECL(raw_tracking_pose, AlwaysAndForever, TrackedDevicePose_t);
		SDECL(seated_tracking_pose, AlwaysAndForever, TrackedDevicePose_t);
		SDECL(standing_tracking_pose, AlwaysAndForever, TrackedDevicePose_t);
		SDECL(activity_level, AlwaysAndForever, EDeviceActivityLevel);
		SDECL(controller_role, AlwaysAndForever, ETrackedControllerRole);
		SDECL(device_class, AlwaysAndForever, ETrackedDeviceClass);
		SDECL(connected, AlwaysAndForever, bool);
		SDECL(controller_state, bool, VRControllerState_t);
		SDECL(synced_seated_pose, bool, TrackedDevicePose_t);
		SDECL(synced_standing_pose, bool, TrackedDevicePose_t);
		SDECL(synced_raw_pose, bool, TrackedDevicePose_t);

		properties_subtable<std::vector<char, ALLOCATOR_TYPE>, vr::ETrackedDeviceProperty, ETrackedPropertyError>	string_props;
		properties_subtable<bool, vr::ETrackedDeviceProperty, ETrackedPropertyError>				bool_props;
		properties_subtable<float,vr::ETrackedDeviceProperty, ETrackedPropertyError>				float_props;
		properties_subtable<HmdMatrix34_t,vr::ETrackedDeviceProperty, ETrackedPropertyError>		mat34_props;
		properties_subtable<int32_t, vr::ETrackedDeviceProperty, ETrackedPropertyError>				int32_props;
		properties_subtable<uint64_t, vr::ETrackedDeviceProperty, ETrackedPropertyError>			uint64_props;

		std::vector<component_on_controller_schema, ALLOCATOR_TYPE> components;
	};

	struct spatial_sort_schema
	{
		spatial_sort_schema(ALLOCATOR_DECL)
			:
			INIT(hmds_sorted),
			INIT(controllers_sorted),
			INIT(trackers_sorted),
			INIT(reference_sorted)
		{}

		VDECL(hmds_sorted, AlwaysAndForever, TrackedDeviceIndex_t);
		VDECL(controllers_sorted, AlwaysAndForever, TrackedDeviceIndex_t);
		VDECL(trackers_sorted, AlwaysAndForever, TrackedDeviceIndex_t);
		VDECL(reference_sorted, AlwaysAndForever, TrackedDeviceIndex_t);
	};

	struct system_schema
	{
		system_schema(ALLOCATOR_DECL)
			:
			INIT(vr_init_token),
			INIT(recommended_target_size),
			INIT(seconds_since_last_vsync),
			INIT(frame_counter_since_last_vsync),
			INIT(is_display_on_desktop),
			INIT(seated2standing),
			INIT(raw2standing),
			INIT(num_hmd),
			INIT(num_controller),
			INIT(num_tracking),
			INIT(num_reference),
			INIT(input_focus_captured_by_other),
			INIT(d3d9_adapter_index),
			INIT(dxgi_output_info)
			, eyes(allocator)
			, controllers(allocator)
			, spatial_sorts(allocator)
		{}

		SDECL(vr_init_token, AlwaysAndForever, uint32_t);
		SDECL(recommended_target_size, AlwaysAndForever, Uint32Size);

		SDECL(seconds_since_last_vsync, bool, float);
		SDECL(frame_counter_since_last_vsync, bool, uint64_t);
		SDECL(is_display_on_desktop, AlwaysAndForever, bool);
		SDECL(seated2standing, AlwaysAndForever, HmdMatrix34_t);
		SDECL(raw2standing, AlwaysAndForever, HmdMatrix34_t);
		SDECL(num_hmd, AlwaysAndForever, uint32_t);
		SDECL(num_controller, AlwaysAndForever, uint32_t);
		SDECL(num_tracking, AlwaysAndForever, uint32_t);
		SDECL(num_reference, AlwaysAndForever, uint32_t);
		SDECL(input_focus_captured_by_other, AlwaysAndForever, bool);
		SDECL(d3d9_adapter_index, AlwaysAndForever, int32_t);
		SDECL(dxgi_output_info, AlwaysAndForever, int32_t);

		std::vector<eye_schema, ALLOCATOR_TYPE> eyes;
		std::vector<system_controller_schema, ALLOCATOR_TYPE> controllers;
		std::vector<spatial_sort_schema, ALLOCATOR_TYPE> spatial_sorts;  // 0is absolute +1 for relative to each controller
	};

	struct application_schema
	{
		application_schema(ALLOCATOR_DECL)
			:
			INIT(application_key),
			INIT(is_installed),
			INIT(auto_launch),
			INIT(supported_mime_types),
			INIT(process_id),
			INIT(application_launch_arguments),
			string_props(application_string_properties_table,TBL_SIZE(application_string_properties_table), allocator),
			bool_props(application_bool_properties_table,TBL_SIZE(application_bool_properties_table), allocator),
			uint64_props(application_uint64_properties_table,TBL_SIZE(application_uint64_properties_table), allocator)
		{}

		VDECL(application_key, AlwaysAndForever, char);
		SDECL(is_installed, AlwaysAndForever, bool);
		SDECL(auto_launch, AlwaysAndForever, bool); 
		VDECL(supported_mime_types, bool, char);
		SDECL(process_id, AlwaysAndForever, uint32_t);
		VDECL(application_launch_arguments, AlwaysAndForever, char);

		properties_subtable<std::vector<char, ALLOCATOR_TYPE>, vr::EVRApplicationProperty, EVRApplicationError>	string_props;
		properties_subtable<bool, vr::EVRApplicationProperty, EVRApplicationError>		bool_props;
		properties_subtable<uint64_t, vr::EVRApplicationProperty, EVRApplicationError>	uint64_props;
	};

	struct mime_type_schema
	{
		mime_type_schema(ALLOCATOR_DECL)
			:
			INIT(mime_type),
			INIT(default_application),
			INIT(applications_that_support_mime_type)
		{}

		VDECL(mime_type, AlwaysAndForever, char);
		VDECL(default_application, bool, char);
		VDECL(applications_that_support_mime_type, AlwaysAndForever, char);
	};

	struct applications_schema
	{
		applications_schema(ALLOCATOR_DECL)
			:
			INIT(starting_application),
			INIT(transition_state),
			INIT(is_quit_user_prompt),
			INIT(current_scene_process_id),
			INIT(active_application_indexes),
			mime_types(allocator),
			applications(allocator)
		{}

		VDECL(starting_application, EVRApplicationError, char);
		SDECL(transition_state, AlwaysAndForever, EVRApplicationTransitionState);
		SDECL(is_quit_user_prompt, AlwaysAndForever, bool);
		SDECL(current_scene_process_id, AlwaysAndForever, uint32_t);
		VDECL(active_application_indexes, AlwaysAndForever, int);
		
		std::vector<mime_type_schema, ALLOCATOR_TYPE> mime_types;
		std::vector<application_schema, ALLOCATOR_TYPE> applications;
	};

	template <typename T>
	struct setting_subtable
	{
		setting_subtable(const char *const *tbl_in, int tbl_size_in, ALLOCATOR_DECL)
			: tbl(tbl_in),
			tbl_size(tbl_size_in),
			nodes(allocator)
		{
			nodes.reserve(tbl_size);
			for (int i = 0; i < tbl_size; i++)
			{
				// create an entry in the vector table per known property name
				nodes.emplace_back(tbl[i], allocator);
			}
		}
		const char *const*tbl;
		const int tbl_size;
		VSDECL(nodes, EVRSettingsError, T);
	};

	template <>
	struct setting_subtable<char>
	{
		setting_subtable(const char *const *tbl_in, int tbl_size_in, ALLOCATOR_DECL)
			: tbl(tbl_in),
			tbl_size(tbl_size_in),
			nodes(allocator)
		{
			nodes.reserve(tbl_size);
			for (int i = 0; i < tbl_size; i++)
			{
				// create an entry in the vector table per known property name
				nodes.emplace_back(tbl[i], allocator);
			}
		}
		const char *const*tbl;
		const int tbl_size;
		VVDECL(nodes, EVRSettingsError, char);
	};

	struct settings_schema
	{
		struct section_schema
		{
			const char *section_name;
			setting_subtable<bool>		bool_settings;
			setting_subtable<char>		string_settings;
			setting_subtable<float>		float_settings;
			setting_subtable<int32_t>	int32_settings;

			section_schema(const section_def_t& def, ALLOCATOR_DECL)
				:
				section_name(def.section_name),
				bool_settings(def.bool_settings_ary, def.bool_size, allocator),
				string_settings(def.stri_settings_ary, def.stri_size, allocator),
				float_settings(def.floa_settings_ary, def.floa_size, allocator),
				int32_settings(def.int32_settings_ary, def.int3_size, allocator)
			{}
		};

		settings_schema(ALLOCATOR_DECL)
			:sections(allocator)
		{
			sections.reserve(defs_tbl_size);
			// section_defs and devs_tbl_size are global constants
			for (int i = 0; i < defs_tbl_size; i++)
			{
				sections.emplace_back(section_defs[i], allocator);
			}
		}
		std::vector<section_schema, ALLOCATOR_TYPE> sections;
	};

	struct chaperone_schema
	{
		chaperone_schema(ALLOCATOR_DECL)
			:
			INIT(calibration_state),
			INIT(play_area_size),
			INIT(play_area_rect),
			INIT(bounds_visible),
			INIT(bounds_colors),
			INIT(camera_color)
		{}
		SDECL(calibration_state, AlwaysAndForever, ChaperoneCalibrationState);
		SDECL(play_area_size, bool, HmdVector2_t);
		SDECL(play_area_rect, bool, HmdQuad_t);
		SDECL(bounds_visible, AlwaysAndForever, bool);
		VDECL(bounds_colors, AlwaysAndForever, HmdColor_t);
		SDECL(camera_color, AlwaysAndForever, HmdColor_t);
	};

	struct chaperonesetup_schema
	{
		chaperonesetup_schema(ALLOCATOR_DECL)
			:
			INIT(working_play_area_size),
			INIT(working_play_area_rect),
			INIT(working_collision_bounds_info),
			INIT(live_collision_bounds_info),
			INIT(working_seated2rawtracking),
			INIT(working_standing2rawtracking),
			INIT(live_collision_bounds_tags_info),
			INIT(live_seated2rawtracking),
			INIT(live_physical_bounds_info)
		{}

		SDECL(working_play_area_size, bool, HmdVector2_t);
		SDECL(working_play_area_rect, bool, HmdQuad_t);
		VDECL(working_collision_bounds_info, bool, HmdQuad_t);
		VDECL(live_collision_bounds_info, bool, HmdQuad_t);
		SDECL(working_seated2rawtracking, bool, HmdMatrix34_t);
		SDECL(working_standing2rawtracking, bool, HmdMatrix34_t);
		VDECL(live_collision_bounds_tags_info, bool, uint8_t);
		SDECL(live_seated2rawtracking, bool, HmdMatrix34_t);
		VDECL(live_physical_bounds_info, bool, HmdQuad_t);
	};

	struct compositor_controller_schema
	{
		compositor_controller_schema(ALLOCATOR_DECL)
			:
			INIT(last_render_pose),
			INIT(last_game_pose)
		{}

		SDECL(last_render_pose, EVRCompositorError, TrackedDevicePose_t);
		SDECL(last_game_pose, EVRCompositorError, TrackedDevicePose_t);
	};

	struct compositor_schema
	{
		compositor_schema(ALLOCATOR_DECL)
			:
			INIT(tracking_space),
			INIT(frame_timing),
			INIT(frame_timings),
			INIT(frame_time_remaining),
			INIT(cumulative_stats),
			INIT(foreground_fade_color),
			INIT(background_fade_color),
			INIT(grid_alpha),
			INIT(is_fullscreen),
			INIT(current_scene_focus_process),
			INIT(last_frame_renderer),
			INIT(can_render_scene),
			INIT(is_mirror_visible),
			INIT(should_app_render_with_low_resource),
			INIT(instance_extensions_required),
			controllers(allocator)
		{}

		SDECL(tracking_space, AlwaysAndForever, ETrackingUniverseOrigin);
		SDECL(frame_timing, bool, Compositor_FrameTiming);
		VDECL(frame_timings, AlwaysAndForever, Compositor_FrameTiming);
		SDECL(frame_time_remaining, AlwaysAndForever, float);
		SDECL(cumulative_stats, AlwaysAndForever, Compositor_CumulativeStats);
		SDECL(foreground_fade_color, AlwaysAndForever, HmdColor_t);
		SDECL(background_fade_color, AlwaysAndForever, HmdColor_t);
		SDECL(grid_alpha, AlwaysAndForever, float);
		SDECL(is_fullscreen, AlwaysAndForever, bool);
		SDECL(current_scene_focus_process, AlwaysAndForever, uint32_t);
		SDECL(last_frame_renderer, AlwaysAndForever, uint32_t);
		SDECL(can_render_scene, AlwaysAndForever, bool);
		SDECL(is_mirror_visible, AlwaysAndForever, bool);
		SDECL(should_app_render_with_low_resource, AlwaysAndForever, bool);
		VDECL(instance_extensions_required, AlwaysAndForever, char);

		std::vector<compositor_controller_schema, ALLOCATOR_TYPE> controllers;
	};

	struct per_overlay_state
	{
		per_overlay_state(ALLOCATOR_DECL)
			:
			INIT(overlay_key),
			INIT(overlay_handle),
			INIT(overlay_name),
			INIT(overlay_image_width),
			INIT(overlay_image_height),
			INIT(overlay_image_data),
			INIT(overlay_rendering_pid),
			INIT(overlay_flags),
			INIT(overlay_color),
			INIT(overlay_alpha),
			INIT(overlay_texel_aspect),
			INIT(overlay_sort_order),
			INIT(overlay_width_in_meters),
			INIT(overlay_auto_curve_range_in_meters),
			INIT(overlay_texture_color_space),
			INIT(overlay_texture_bounds),
			INIT(overlay_transform_type),
			INIT(overlay_transform_absolute),
			INIT(overlay_transform_device_relative),
			INIT(overlay_transform_component_relative_device_index),
			INIT(overlay_transform_component_relative_name),
			INIT(overlay_input_method),
			INIT(overlay_mouse_scale),
			INIT(overlay_is_hover_target),
			INIT(overlay_is_visible),
			INIT(overlay_is_active_dashboard),
			INIT(overlay_texture_size),
			INIT(overlay_dashboard_scene_process),
			INIT(events)
		{}

		VDECL(overlay_key, AlwaysAndForever, char);
		SDECL(overlay_handle, EVROverlayError, vr::VROverlayHandle_t);   // i'm assuming keys are unique, not handles - handles can be reused
		VDECL(overlay_name, EVROverlayError, char);
		
		SDECL(overlay_image_width, EVROverlayError, uint32_t);
		SDECL(overlay_image_height, EVROverlayError, uint32_t);
		VDECL(overlay_image_data, EVROverlayError, uint8_t);

		SDECL(overlay_rendering_pid, AlwaysAndForever, uint32_t);
		SDECL(overlay_flags, EVROverlayError, uint32_t);
		SDECL(overlay_color, EVROverlayError, RGBColor);
		SDECL(overlay_alpha, EVROverlayError, float);
		SDECL(overlay_texel_aspect, EVROverlayError, float);
		SDECL(overlay_sort_order, EVROverlayError, uint32_t);
		SDECL(overlay_width_in_meters, EVROverlayError, float);
		SDECL(overlay_auto_curve_range_in_meters, EVROverlayError, FloatRange);
		SDECL(overlay_texture_color_space, EVROverlayError, EColorSpace);
		SDECL(overlay_texture_bounds, EVROverlayError, VRTextureBounds_t);
		SDECL(overlay_transform_type, EVROverlayError, VROverlayTransformType);
		SDECL(overlay_transform_absolute, EVROverlayError, AbsoluteTransform);
		SDECL(overlay_transform_device_relative, EVROverlayError, TrackedDeviceRelativeTransform);
		SDECL(overlay_transform_component_relative_device_index, EVROverlayError, TrackedDeviceIndex_t);
		VDECL(overlay_transform_component_relative_name, EVROverlayError, char);

		SDECL(overlay_input_method, EVROverlayError, VROverlayInputMethod);
		SDECL(overlay_mouse_scale, EVROverlayError, HmdVector2_t);
		SDECL(overlay_is_hover_target, AlwaysAndForever, bool);
		SDECL(overlay_is_visible, AlwaysAndForever, bool);
		SDECL(overlay_is_active_dashboard, AlwaysAndForever, bool);
		SDECL(overlay_texture_size, EVROverlayError, Uint32Size);
		SDECL(overlay_dashboard_scene_process, EVROverlayError, uint32_t);
		SDECL(events, AlwaysAndForever, VREvent_t);	// experiment

	};

	struct overlay_schema
	{
		overlay_schema(ALLOCATOR_DECL)
			:
			INIT(gamepad_focus_overlay),
			INIT(primary_dashboard_device),
			INIT(is_dashboard_visible),
			INIT(active_overlay_indexes),
			INIT(keyboard_text),
			overlays(allocator)
		{}

		SDECL(gamepad_focus_overlay, AlwaysAndForever, vr::VROverlayHandle_t);
		SDECL(primary_dashboard_device, AlwaysAndForever, TrackedDeviceIndex_t);
		SDECL(is_dashboard_visible, AlwaysAndForever, bool);
		VDECL(active_overlay_indexes, AlwaysAndForever, int);
		VDECL(keyboard_text, AlwaysAndForever, char);
		
		std::vector<per_overlay_state, ALLOCATOR_TYPE> overlays;
	};

	struct rendermodel_component_schema
	{
		rendermodel_component_schema(ALLOCATOR_DECL)
			:
			INIT(component_name),
			INIT(button_mask),
			INIT(render_model_name)
		{}

		VDECL(component_name, AlwaysAndForever, char);
		SDECL(button_mask, AlwaysAndForever, uint64_t);
		VDECL(render_model_name, bool, char);
	};

	struct rendermodel_schema
	{
		rendermodel_schema(ALLOCATOR_DECL)
			:
			INIT(render_model_name),
			INIT(thumbnail_url),
			INIT(original_path),
			INIT(vertex_data),
			INIT(index_data),
			INIT(texture_width),
			INIT(texture_height),
			INIT(texture_map_data),
			components(allocator)
		{}

		VDECL(render_model_name, AlwaysAndForever, char);
		VDECL(thumbnail_url, EVRRenderModelError, char);
		VDECL(original_path, EVRRenderModelError, char);
		VDECL(vertex_data, EVRRenderModelError, RenderModel_Vertex_t);
		VDECL(index_data, EVRRenderModelError, uint16_t);

		SDECL(texture_width, EVRRenderModelError, uint16_t);
		SDECL(texture_height, EVRRenderModelError, uint16_t);
		VDECL(texture_map_data, EVRRenderModelError, uint8_t);

		std::vector<rendermodel_component_schema, ALLOCATOR_TYPE> components;
	};

	struct rendermodels_schema
	{
		rendermodels_schema(ALLOCATOR_DECL)
			: models(allocator)
		{}

		std::vector<rendermodel_schema, ALLOCATOR_TYPE> models;
	};

	struct cameraframetype_schema
	{
		cameraframetype_schema(ALLOCATOR_DECL)
			:
			INIT(frame_size),
			INIT(intrinsics),
			INIT(projection),
			INIT(video_texture_size)			
		{}

		SDECL(frame_size, EVRTrackedCameraError, CameraFrameSize_t);
		SDECL(intrinsics, EVRTrackedCameraError, CameraFrameIntrinsics_t);
		SDECL(projection, EVRTrackedCameraError, HmdMatrix44_t);
		SDECL(video_texture_size, EVRTrackedCameraError, VideoStreamTextureSize_t);
	};

	struct trackedcamera_schema
	{
		struct controller_camera_schema
		{
			controller_camera_schema(ALLOCATOR_DECL)
				:
				INIT(has_camera),
				cameraframetypes(allocator)
			{}

			SDECL(has_camera, EVRTrackedCameraError, bool);
			std::vector<cameraframetype_schema, ALLOCATOR_TYPE> cameraframetypes;
		};

		trackedcamera_schema(ALLOCATOR_DECL)
			: controllers(allocator)
		{}

		std::vector<controller_camera_schema, ALLOCATOR_TYPE> controllers;
	};

	struct extendeddisplay_schema
	{
		extendeddisplay_schema(ALLOCATOR_DECL)
			: 
			INIT(window_bounds),
			INIT(left_output_viewport),
			INIT(right_output_viewport)
		{}

		SDECL(window_bounds, AlwaysAndForever, WindowBounds_t);
		SDECL(left_output_viewport, AlwaysAndForever, ViewPort_t);
		SDECL(right_output_viewport, AlwaysAndForever, ViewPort_t);
	};

	struct resource_schema
	{
		resource_schema(ALLOCATOR_DECL)
			:
			INIT(resource_name),
			INIT(resource_directory),
			INIT(resource_full_path),
			INIT(resource_data)
		{}

		VDECL(resource_name, AlwaysAndForever, char);
		VDECL(resource_directory, AlwaysAndForever, char);
		VDECL(resource_full_path, AlwaysAndForever, char);
		VDECL(resource_data, AlwaysAndForever, uint8_t);
	};
		
	struct resources_schema
	{
		resources_schema(ALLOCATOR_DECL)
			:
			resources(allocator)
		{}

		std::vector<resource_schema, ALLOCATOR_TYPE> resources;
	};

	vrschema(ALLOCATOR_DECL)
		:	
		system_node(allocator),
		applications_node(allocator),
		settings_node(allocator),
		chaperone_node(allocator),
		chaperone_setup_node(allocator),
		compositor_node(allocator),
		overlay_node(allocator),
		rendermodels_node(allocator),
		extendeddisplay_node(allocator),
		trackedcamera_node(allocator),
		resources_node(allocator)
	{}

	system_schema			system_node;		// schema is the type. node is the instace
	applications_schema		applications_node;
	settings_schema			settings_node;
	chaperone_schema		chaperone_node;
	chaperonesetup_schema	chaperone_setup_node;
	compositor_schema		compositor_node;
	overlay_schema			overlay_node;
	rendermodels_schema		rendermodels_node;
	extendeddisplay_schema	extendeddisplay_node;
	trackedcamera_schema	trackedcamera_node;
	resources_schema        resources_node;
};

// instantiate the scheme (false means not-iterator);
using vrstate		= vrschema<false>;
using vriterator	= vrschema<true>;

struct vrstate_visitor{};

struct lhs_only_visitor : public vrstate_visitor
{
	inline bool visit_openvr() { return false; }
#if 0
	template <typename T, typename P, typename allocatorT>
	void visit_node(history<std::vector<T, allocatorT>, P, allocatorT> &node, const T *sample_val, int len)
	{
		assert(0); 
	}
#endif

	// since P is passed as a param, this is the one that'll be chosen for history with P
	template <typename T, typename P, typename allocatorT>
	void visit_node(history<std::vector<T, allocatorT>, P, allocatorT> &node, const T *sample_val, P sample_presence, int len)
	{
		assert(0); 
	} 
};

#define EMPTY_RHS_TEMPLATE()\
template <typename T, typename P, typename allocatorT>\
void visit_node(history<std::vector<T, allocatorT>, P, allocatorT> &node, const T *sample_val, int len)\
{\
	assert(0);\
}\
\
template <typename T, typename P, typename allocatorT>\
void visit_node(history<std::vector<T, allocatorT>, P, allocatorT> &node, const T *sample_val, P sample_presence, int len)\
{\
	assert(0);\
}\
template <typename T, typename P, typename allocatorT>\
void visit_node(history<T, P, allocatorT> &node, const scalar_result<T, P> &scalar)\
{\
	assert(0);\
}

// assert no LHS-only side - ie simplified params cause assert
#define EMPTY_LHS_TEMPLATE()\
template <typename T, typename P, typename allocatorT>\
void visit_node(history<std::vector<T, allocatorT>, P, allocatorT> &node)\
{\
	assert(0);\
}\
template <typename T, typename P, typename allocatorT>\
void visit_node(history<T, P, allocatorT> &node)\
{\
	assert(0);\
}

struct update_history_visit_fn : public vrstate_visitor
{
	update_history_visit_fn(int frame_number)
		: m_frame_number(frame_number),
		m_update_counter(0), m_visit_counter(0)
	{}

	int m_frame_number;
	int m_update_counter;
	int m_visit_counter;

	inline bool visit_openvr() { return true; }
	inline bool reload_render_models() { return false; }

	// non leaf.  group is identified by it's name and index
	inline void start_group_node(const char *group_id_name, int group_id_index)
	{}

	inline void end_group_node(const char *group_id_name, int group_id_index)
	{}

	// this should give the history half and the openvr half
	// because if openvr resizes something, then the history visitor should have a chance to resize
	// as well
	template <typename T>
	inline void start_vector(const char *vector_name, T &vec)
	{}

	template <typename T>
	inline void end_vector(const char *vector_name, T &vec)
	{}

	// use partial specialiation to consider two cases: histories with Presence/result_code and histories with no Present/result_code
	// general case - P is defined
	template <typename T, typename P, typename allocatorT>
	struct updater
	{
		static int do_update(history<T, P, allocatorT> &node, const scalar_result<T, P> &sample_val, int frame_number)
		{
			if (node.empty() || node.latest_P() != sample_val.result_code || 
				((PresenceValue<P>::is_present == sample_val.result_code) && node.latest() != sample_val.val))
			{
				node.emplace_front(frame_number, sample_val.result_code, sample_val.val);
				return 1;
			}
			return 0;
		}
	};

	// partial specialization with no presence
	template <typename T, typename allocatorT>
	struct updater <T,AlwaysAndForever,allocatorT>
	{
		static int do_update(history<T, AlwaysAndForever, allocatorT> &node, const scalar_result<T, AlwaysAndForever> &sample_val, int frame_number)
		{
			if (node.empty() || node.latest() != sample_val.val)
			{
				node.emplace_front(frame_number, sample_val.val);
				return 1;
			}
			return 0;
		}
	};

	// call the right do_update based on P
	template <typename T, typename P, typename allocatorT>
	void visit_node(history<T, P, allocatorT> &node, const scalar_result<T, P> &scalar)
	{
		m_visit_counter++;
		m_update_counter += updater<T, P, allocatorT>::do_update(node, scalar, m_frame_number);
	}

	template <typename T, typename P, typename allocatorT>
	bool not_equal_val(history<std::vector<T, allocatorT>, P, allocatorT> &node, const T *sample_val, int len)
	{
		if (node.latest().size() != len)
			return true;

		if (node.latest().size() == 0 && len == 0)
			return false;

		if (memcmp(&node.latest()[0], sample_val, sizeof(T) * len))
			return true;

		return false;
	}

	template <typename T, typename P, typename allocatorT>
	void visit_node(history<std::vector<T, allocatorT>, P, allocatorT> &node, const T *sample_val, int len)
	{
		m_visit_counter++;
		if (node.empty() || not_equal_val(node, sample_val, len))
		{
			ALLOCATOR_TYPE allocator = node.values.get_allocator();
			node.emplace_front(m_frame_number, sample_val, sample_val + len, allocator);
			m_update_counter += 1;
		}
	}
	
	// since P is passed as a param, this is the one that'll be chosen for history with P
	template <typename T, typename P, typename allocatorT>
	void visit_node(history<std::vector<T, allocatorT>,P,allocatorT> &node, const T *sample_val, P sample_presence, int len)
	{
		m_visit_counter++;
		if (node.empty() || node.latest_P() != sample_presence || 
			(PresenceValue<P>::is_present == sample_presence && not_equal_val(node, sample_val, len)))
		{
			node.emplace_front(m_frame_number, sample_presence, sample_val, sample_val + len, node.values.get_allocator());
			m_update_counter += 1;
		}
	}
	EMPTY_LHS_TEMPLATE()
};

#ifdef HAVE_IMG_GUI
#include <map>

class guinodebase
{
public:
	guinodebase()
	{}

	std::string unique_name;
};

template <typename T, typename P, typename allocatorT>
class guinode : public guinodebase
{
public:
	guinode() {}

};

// traverses vr state to build a graph of grid_nodes for use by the guy
struct img_visit_fn : public lhs_only_visitor
{
	int depth;
	std::map<int, bool> enabled;
	std::map<int, std::string> depth2name;
	std::map<std::string, bool> selected;
	std::map<void*, guinodebase*> historynode2guinode;

	img_visit_fn()
		: depth(0)
	{}

	// make non-history-node name / ie a grouping
	std::string make_group_name(const char *name, int index)
	{
		std::string parent_name;
		if (depth > 1)
		{
			parent_name = depth2name[depth - 1];
		}

		std::string unique_name;
		if (index != -1)
		{
			unique_name = std::string(name) + std::string("[") +
				std::to_string(index) +
				std::string("]") +
				std::string("##")
				+ parent_name + std::string(name) + std::string(name) + std::to_string(index);
		}
		else
		{
			unique_name = std::string(name) + std::string("##")
				+ parent_name + std::string(name) + std::string(name);
		}
		return unique_name;
	}
	
	inline void push_group(const char *name, int index)
	{
		depth++;
		if (depth == 1 || enabled[depth-1])
		{
			std::string group_name = make_group_name(name, index);
			depth2name[depth] = group_name;
			int flags = 0;
			if (ImGui::TreeNodeEx(group_name.c_str(), flags))
			{
				enabled[depth] = true;
			}
			else
			{
				enabled[depth] = false;
			}
		}
		else
		{
			enabled[depth] = false;
		}
	}

	void pop_group()
	{
		if (enabled[depth])
		{
			ImGui::TreePop();
		}
		depth--;
	}

	inline void start_group_node(const char *group_id_name, int group_id_index)
	{
		push_group(group_id_name, group_id_index);
	}

	inline void end_group_node(const char *group_id_name, int group_id_index)	
	{
		pop_group();
	}

	template <typename T>
	inline void start_vector(const char *vector_name, T &vec)
	{
		push_group(vector_name, -1);
	}
	
	template <typename T>
	inline void end_vector(const char *vector_name, T &vec)
	{
		pop_group();
	}

	static bool visit_openvr() { return false; }
	inline bool reload_render_models() { return true; }

	template <typename T, typename P, typename allocatorT>
	struct node_printer
	{
		static std::string get_node_string(history<T, P, allocatorT> &node)
		{
			using namespace std;
			using namespace openvr_string;
			std::string details;
			if (node.front().is_present())
			{
				return to_string(node.latest());
			}
			else
			{
				return to_string(node.latest_P());
			}
		}
	};

	template <typename T, typename allocatorT>
	struct node_printer<T, AlwaysAndForever, allocatorT>
	{
		static std::string get_node_string(history<T, AlwaysAndForever, allocatorT> &node)
		{
			using namespace std;
			using namespace openvr_string;
			return to_string(node.latest());
		}
	};

	template <typename T, typename P, typename allocatorT>
	void push_history_node(history<T, P, allocatorT> &node)
	{
		guinodebase *gnode;
		auto cache_iter = historynode2guinode.find(&node);
		if (cache_iter == historynode2guinode.end())
		{
			gnode = new guinode<T,P,allocatorT>();
			historynode2guinode[&node] = gnode;
		}
		else
		{
			gnode = cache_iter->second;
		}

		if (enabled[depth])	// is the current depth open in the tree view?
		{
			char *id = (char *)&node + 1;

			ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
			ImGui::PushID(id++);
			ImGui::Text(node.name);
			ImGui::PopID();

			static int x;
			ImGui::PushID(id++);
			ImGui::SliderInt("", &x,0,100);
			ImGui::PopID();

			ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
			ImGui::PushID(id++);
			std::string s = node_printer<T, P, allocatorT>::get_node_string(node);
			ImGui::Text(s.c_str());
			ImGui::PopID();

			ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing() * 2);
		}
	}

	template <typename T, typename P, typename allocatorT>
	void visit_node(history<T, P, allocatorT> &node)
	{
		push_history_node(node);
	}

	template <typename T, typename P, typename allocatorT>
	void visit_node(history<std::vector<T, allocatorT>, P, allocatorT> &node)
	{
		push_history_node(node);
	}

	EMPTY_RHS_TEMPLATE()
};
#endif

// traverse vr state to dump values to stdout
struct dump_history_visit_fn : public vrstate_visitor
{
	std::ostream &ofs;
	int depth; 

	dump_history_visit_fn(std::ostream &ofs_in)
		:	ofs(ofs_in),
		depth(0)
	{}

	inline void start_group_node(const char *group_id_name, int group_id_index)
	{
		depth++;
		indented_output(ofs, std::string("start of ") + group_id_name + " " + std::to_string(group_id_index) + "\n", depth);
	}
	inline void end_group_node(const char *group_id_name, int group_id_index)	
	{
		indented_output(ofs, std::string("end of ") + group_id_name + " " + std::to_string(group_id_index) + "\n", depth);
		indented_output(ofs, std::string("==================================================\n"), depth);
		depth--;
	}

	template <typename T>
	inline void start_vector(const char *vector_name, T &vec)
	{}

	template <typename T>
	inline void end_vector(const char *vector_name, T &vec)
	{}

	static bool visit_openvr() { return false; }
	inline bool reload_render_models() { return true; }

	template <typename T, typename P, typename allocatorT>
	void visit_node(history<T, P, allocatorT> &node)
	{
		node.dump(ofs,depth);
	}

	template <typename T, typename P, typename allocatorT>
	void visit_node(history<std::vector<T, allocatorT>, P, allocatorT> &node)
	{
		node.dump(ofs,depth);
	}

	EMPTY_RHS_TEMPLATE()
};

// for saving vrstate to a file
// traverse vrstate and encode values into the EncodeStream
struct encoder_visitor : public lhs_only_visitor
{
	EncodeStream *e;
	encoder_visitor(EncodeStream *e_in)
		: e(e_in)
	{}
	encoder_visitor(encoder_visitor&) = delete;

	
	inline bool reload_render_models() { return true; }

	inline void start_group_node(const char *group_id_name, int group_id_index) {}
	inline void end_group_node(const char *group_id_name, int group_id_index) {}

	template <typename T>
	inline void start_vector(const char *vector_name, T &vec)
	{
		// given a vector - encode it's size onto the stream
		encode(vec.size(), *e);
	}

	template <typename T>
	inline void end_vector(const char *vector_name, T &vec)
	{}

	template <typename T, typename P, typename allocatorT>
	void visit_node(history<std::vector<T, allocatorT>, P, allocatorT> &node)
	{
		serializer<T, P, allocatorT>::vector_encode_history(node, *e);
	}

	template <typename T, typename P, typename allocatorT>
	void visit_node(history<T, P, allocatorT> &node)
	{
		serializer<T, P, allocatorT>::encode_history(node, *e);
	}

	EMPTY_RHS_TEMPLATE();
};


// for reading from a stream
// traverses the vrstate and then reads values from the stream
struct decoder_visitor : public vrstate_visitor
{
	EncodeStream *e;
	ALLOCATOR_TYPE my_allocator;

	decoder_visitor(EncodeStream *e_in, ALLOCATOR_DECL)
		: e(e_in),
		my_allocator(allocator)
	{}
	decoder_visitor(decoder_visitor&) = delete;

	inline bool visit_openvr() { return false; }
	inline bool reload_render_models() { return true; }

	inline void start_group_node(const char *group_id_name, int group_id_index) {}
	inline void end_group_node(const char *group_id_name, int group_id_index) {}

	template <typename T>
	inline void start_vector(const char *vector_name, T &vec)
	{
		// read size from stream and make sure vec is that size
		int size;
		decode(size, *e);
		vec.reserve(size);
		while ((int)vec.size() < size)
		{
			vec.emplace_back(my_allocator);	// goofiness to create empty values with the correct allocator
		}
	}

	template <typename T>
	inline void end_vector(const char *vector_name, T &vec)
	{}

	template <typename T, typename P, typename allocatorT>
	void visit_node(history<std::vector<T, allocatorT>, P, allocatorT> &node)
	{
		deserializer<T,P,allocatorT>::vector_decode_history(node, *e, my_allocator);
	}
	
	template <typename T, typename P, typename allocatorT>
	void visit_node(history<T, P, allocatorT> &node)
	{
		deserializer<T, P, allocatorT>::decode_history(node, *e, my_allocator);
	}

	EMPTY_RHS_TEMPLATE();

};

// construct a scalar only wrapper function with no index
#define SCALAR_WRAP(handle_type, handle, function) \
	inline scalar<decltype(((handle_type*)nullptr)->function())> function() \
{\
	return make_scalar(handle->function());\
}

// construct a scalar only wrapper function indexed by index_type
#define SCALAR_WRAP_INDEXED(handle_type, handle, return_type, function, index_type)\
	inline scalar<return_type> function(index_type index) \
	{ \
		return make_scalar(handle->function(index)); \
	}


// each of the openvr interfaces are wrapped 
// * this is to make the interfaces more consistent to the rest of the 
//   system - specifically the notion of scalar values (normal values) 
//   and scalar_result - values might not be present
struct SystemWrapper
{
	SystemWrapper(IVRSystem *sysi_in, StringPool *string_pool_in)
		: sysi(sysi_in), string_pool(string_pool_in)
	{}

	inline scalar<uint32_t> GetInitToken()
	{
		return 0; // VR_GetInitToken();
	}

	inline scalar<Uint32Size> GetRecommendedRenderTargetSize()
	{
		scalar<Uint32Size> s;
		sysi->GetRecommendedRenderTargetSize(&s.val.width, &s.val.height);
		return s;
	}

	inline scalar<bool> GetIsDisplayOnDesktop()
	{
		return make_scalar(sysi->IsDisplayOnDesktop());
	}

	inline void GetTimeSinceLastVsync(
			scalar_result<float, bool>  *seconds_since_last_vsync,
			scalar_result<uint64_t, bool>  *frame_counter_since_last_vsync)
	{
		bool r = sysi->GetTimeSinceLastVsync(
			&seconds_since_last_vsync->val,
			&frame_counter_since_last_vsync->val);

		seconds_since_last_vsync->result_code = r;
		frame_counter_since_last_vsync->result_code = r;
	}

	inline scalar<int32_t> GetD3D9AdapterIndex()
	{
		return make_scalar(sysi->GetD3D9AdapterIndex());
	}

	inline scalar<int32_t> GetDXGIOutputInfo()
	{
		int32_t i = 0;
		sysi->GetDXGIOutputInfo(&i);
		return make_scalar(i);
	}

	inline vector_result<TrackedDeviceIndex_t> &GetSortedTrackedDeviceIndicesOfClass(
												ETrackedDeviceClass device_class,
												vr::TrackedDeviceIndex_t unRelativeToTrackedDeviceIndex,
											    vector_result<TrackedDeviceIndex_t> *result)
	{
		result->count = sysi->GetSortedTrackedDeviceIndicesOfClass(
			device_class, result->s.buf(),
			result->s.max_count(),
			unRelativeToTrackedDeviceIndex);
		return *result;
	}

	inline scalar<uint32_t> CountDevicesOfClass(ETrackedDeviceClass device_class)
	{
		return make_scalar(sysi->GetSortedTrackedDeviceIndicesOfClass(device_class, nullptr, 0));
	}

	SCALAR_WRAP(IVRSystem, sysi, IsInputFocusCapturedByAnotherProcess);
	SCALAR_WRAP(IVRSystem, sysi, GetSeatedZeroPoseToStandingAbsoluteTrackingPose);
	SCALAR_WRAP(IVRSystem, sysi, GetRawZeroPoseToStandingAbsoluteTrackingPose);

	inline scalar<HmdMatrix44_t> GetProjectionMatrix(EVREye eEye, float fNearZ, float fFarZ)
	{
		return scalar<HmdMatrix44_t>(sysi->GetProjectionMatrix(eEye, fNearZ, fFarZ));
	}
	
	inline scalar<HmdMatrix34_t> GetEyeToHeadTransform(EVREye eEye)
	{
		return make_scalar(sysi->GetEyeToHeadTransform(eEye));
	}

	inline scalar<HmdVector4_t> GetProjectionRaw(EVREye eEye)
	{
		HmdVector4_t raw;
		sysi->GetProjectionRaw(eEye, &raw.v[0], &raw.v[1], &raw.v[2], &raw.v[3]);
		return make_scalar(raw);
	}

	inline scalar_result<DistortionCoordinates_t, bool> ComputeDistortion(EVREye eEye, float fU, float fV)
	{
		scalar_result<DistortionCoordinates_t, bool> rc;
		rc.result_code = sysi->ComputeDistortion(eEye, fU, fV, &rc.val);
		return rc;
	}

	inline void GetControllerStateWithPose(
		ETrackingUniverseOrigin origin, 
		TrackedDeviceIndex_t unDeviceIndex, 
		scalar_result<VRControllerState_t, bool> *controller_state,
		scalar_result<TrackedDevicePose_t, bool> *synced_pose)
	{
		bool rc = sysi->GetControllerStateWithPose(
							origin, unDeviceIndex, &controller_state->val, sizeof(controller_state->val),
							&synced_pose->val);
		controller_state->result_code = rc;
		synced_pose->result_code = rc;
	}

	inline scalar<EDeviceActivityLevel> GetTrackedDeviceActivityLevel(TrackedDeviceIndex_t unDeviceIndex)
	{
		return make_scalar(sysi->GetTrackedDeviceActivityLevel(unDeviceIndex));
	}

	inline scalar<ETrackedControllerRole> GetControllerRoleForTrackedDeviceIndex(TrackedDeviceIndex_t unDeviceIndex)
	{
		return make_scalar(sysi->GetControllerRoleForTrackedDeviceIndex(unDeviceIndex));
	}

	inline scalar<ETrackedDeviceClass> GetTrackedDeviceClass(TrackedDeviceIndex_t unDeviceIndex)
	{
		return make_scalar(sysi->GetTrackedDeviceClass(unDeviceIndex));
	}

	inline scalar<bool> IsTrackedDeviceConnected(TrackedDeviceIndex_t unDeviceIndex)
	{
		return make_scalar(sysi->IsTrackedDeviceConnected(unDeviceIndex));
	}

	template <typename T>
	inline scalar_result<T, ETrackedPropertyError> GetTrackedDeviceProperty(TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop);

	template <>
	inline scalar_result<bool, ETrackedPropertyError> GetTrackedDeviceProperty(TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop)
	{
		scalar_result<bool, ETrackedPropertyError> result;
		result.val = sysi->GetBoolTrackedDeviceProperty(unDeviceIndex, prop, &result.result_code);
		return result;
	}

	template <>
	inline scalar_result<float, ETrackedPropertyError> GetTrackedDeviceProperty<float>(TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop)
	{
		scalar_result<float, ETrackedPropertyError> result;
		result.val = sysi->GetFloatTrackedDeviceProperty(unDeviceIndex, prop, &result.result_code);
		return result;
	}

	template <>
	inline scalar_result<int32_t, ETrackedPropertyError> GetTrackedDeviceProperty(TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop)
	{
		scalar_result<int32_t, ETrackedPropertyError> result;
		result.val = sysi->GetInt32TrackedDeviceProperty(unDeviceIndex, prop, &result.result_code);
		return result;
	}
	template <>
	inline scalar_result<uint64_t, ETrackedPropertyError> GetTrackedDeviceProperty(TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop)
	{
		scalar_result<uint64_t, ETrackedPropertyError> result;
		result.val = sysi->GetUint64TrackedDeviceProperty(unDeviceIndex, prop, &result.result_code);
		return result;
	}
	template <>
	inline scalar_result<HmdMatrix34_t, ETrackedPropertyError> GetTrackedDeviceProperty(TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop)
	{
		scalar_result<HmdMatrix34_t, ETrackedPropertyError> result;
		result.val = sysi->GetMatrix34TrackedDeviceProperty(unDeviceIndex, prop, &result.result_code);
		return result;
	}

	inline void GetStringTrackedDeviceProperty(TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, 
		vector_result<char, ETrackedPropertyError> *result)
	{
		query_vector_rccount(result, sysi, &IVRSystem::GetStringTrackedDeviceProperty, unDeviceIndex, prop);
	}

	IVRSystem *sysi;
	StringPool *string_pool;
};

struct ApplicationsWrapper
{
	ApplicationsWrapper(IVRApplications *appi_in, StringPool *string_pool_in)
		: appi(appi_in), string_pool(string_pool_in)
	{}

	SCALAR_WRAP(IVRApplications, appi, GetApplicationCount);
	SCALAR_WRAP(IVRApplications, appi, GetTransitionState);
	SCALAR_WRAP(IVRApplications, appi, IsQuitUserPromptRequested);
	SCALAR_WRAP(IVRApplications, appi, GetCurrentSceneProcessId);

	SCALAR_WRAP_INDEXED(IVRApplications, appi, bool, IsApplicationInstalled, const char *);
	SCALAR_WRAP_INDEXED(IVRApplications, appi, bool, GetApplicationAutoLaunch, const char *);
	SCALAR_WRAP_INDEXED(IVRApplications, appi, uint32_t, GetApplicationProcessId, const char *);

	inline vector_result<char, EVRApplicationError> GetStartingApplication()
	{
		vector_result<char, EVRApplicationError> result(string_pool);
		query_vector_rcerror(&result, appi, &IVRApplications::GetStartingApplication);
		return result;
	}

	inline vector_result<char, EVRApplicationError> &GetApplicationKeyByIndex(uint32_t app_index, vector_result<char, EVRApplicationError> &result)
	{
		query_vector_rcerror(&result, appi, &IVRApplications::GetApplicationKeyByIndex, app_index);
		return result;
	}

	inline vector_result<char, bool> GetApplicationSupportedMimeTypes(const char *index)
	{
		vector_result<char, bool> result(string_pool);
		result.s.buf()[0] = 0;
		query_vector_rcerror(&result, appi, &IVRApplications::GetApplicationSupportedMimeTypes, index);
		return result;
	}

	inline vector_result<char> GetApplicationLaunchArguments(uint32_t unHandle)
	{
		vector_result<char> result(string_pool);
		result.s.buf()[0] = 0;
		query_vector_rccount(&result, appi, &IVRApplications::GetApplicationLaunchArguments, unHandle);
		return result;
	}

	inline vector_result<char> GetApplicationsThatSupportMimeType(const char *mime_type)
	{
		vector_result<char> result(string_pool);
		query_vector_rccount(&result, appi, &IVRApplications::GetApplicationsThatSupportMimeType, mime_type);
		return result;
	}

	inline vector_result<char, bool> GetDefaultApplicationForMimeType(const char *mime_type)
	{
		vector_result<char,bool> result(string_pool);
		query_vector_rcerror(&result, appi, &IVRApplications::GetDefaultApplicationForMimeType, mime_type);
		return result;
	}

	template <typename T>
	inline scalar_result<T, vr::EVRApplicationError> GetProperty(const char *appkey, EVRApplicationProperty prop);
	

	template <>
	inline scalar_result<bool, EVRApplicationError> GetProperty(const char *appkey, EVRApplicationProperty prop)
	{
		scalar_result<bool, EVRApplicationError> result;
		result.val = appi->GetApplicationPropertyBool(appkey, prop, &result.result_code);
		return result;
	}
	
	template <>
	inline scalar_result<uint64_t, EVRApplicationError> GetProperty(const char *appkey, EVRApplicationProperty prop)
	{
		scalar_result<uint64_t, EVRApplicationError> result;
		result.val = appi->GetApplicationPropertyUint64(appkey, prop, &result.result_code);
		return result;
	}

	inline void GetStringProperty(const char *appkey, EVRApplicationProperty prop,
		vector_result<char, EVRApplicationError> *result)
	{
		query_vector_rccount(result, appi, &IVRApplications::GetApplicationPropertyString, appkey, prop);
	}

	IVRApplications *appi;
	StringPool *string_pool;
};

struct SettingsWrapper
{
	SettingsWrapper(IVRSettings *setti_in, StringPool *string_pool_in)
		: setti(setti_in), string_pool(string_pool_in)
	{}

	template <typename T>
	inline scalar_result<T, EVRSettingsError> GetSetting(const char *pchSection, const char *pchSettingsKey);

	template <>
	inline scalar_result<bool, EVRSettingsError> GetSetting(const char *pchSection, const char *pchSettingsKey)
	{
		scalar_result<bool, EVRSettingsError> result;
		result.val = setti->GetBool(pchSection, pchSettingsKey, &result.result_code);
		return result;
	}

	template <>
	inline scalar_result<float, EVRSettingsError> GetSetting(const char *pchSection, const char *pchSettingsKey)
	{
		scalar_result<float, EVRSettingsError> result;
		result.val = setti->GetFloat(pchSection, pchSettingsKey, &result.result_code);
		return result;
	}

	template <>
	inline scalar_result<int32_t, EVRSettingsError> GetSetting(const char *pchSection, const char *pchSettingsKey)
	{
		scalar_result<int32_t, EVRSettingsError> result;
		result.val = setti->GetInt32(pchSection, pchSettingsKey, &result.result_code);
		return result;
	}

	inline vector_result<char, EVRSettingsError> GetStringSetting(const char *pchSection, const char *pchSettingsKey)
	{
		vector_result<char, EVRSettingsError> result(string_pool);
		query_vector_rcvoid(&result, setti, &IVRSettings::GetString, pchSection, pchSettingsKey);
		return result;
	}

	IVRSettings *setti;
	StringPool *string_pool;
};

struct ChaperoneWrapper
{
	ChaperoneWrapper(IVRChaperone *chapi_in, StringPool *string_pool_in)
		: chapi(chapi_in), string_pool(string_pool_in)
	{}

	SCALAR_WRAP(IVRChaperone, chapi, GetCalibrationState);
	SCALAR_WRAP(IVRChaperone, chapi, AreBoundsVisible);

	inline scalar_result<HmdVector2_t, bool> GetPlayAreaSize()
	{
		scalar_result<HmdVector2_t, bool> result;
		result.result_code = chapi->GetPlayAreaSize(&result.val.v[0], &result.val.v[1]);
		return result;
	}

	inline scalar_result<HmdQuad_t,bool> GetPlayAreaRect()
	{
		scalar_result<HmdQuad_t, bool> result;
		result.result_code = chapi->GetPlayAreaRect(&result.val);
		return result;
	}


	inline void GetBoundsColor(vector_result<HmdColor_t>* colors,
		int num_output_colors,
		float fade_distance,
		scalar<HmdColor_t> *camera_color)
	{
		assert((int)colors->s.max_count() >= num_output_colors);
		num_output_colors = std::min(num_output_colors, (int)colors->s.max_count());
		chapi->GetBoundsColor(colors->s.buf(), num_output_colors, fade_distance, &camera_color->val);
		colors->count = num_output_colors;
	}


	IVRChaperone *chapi;
	StringPool *string_pool;
};

struct ChaperoneSetupWrapper
{
	ChaperoneSetupWrapper(IVRChaperoneSetup *chapsi_in, StringPool *string_pool_in)
		: chapsi(chapsi_in), string_pool(string_pool_in)
	{}

	inline scalar_result<HmdVector2_t, bool> GetWorkingPlayAreaSize()
	{
		scalar_result<HmdVector2_t, bool> result;
		result.result_code = chapsi->GetWorkingPlayAreaSize(&result.val.v[0], &result.val.v[1]);
		return result;
	}

	inline scalar_result<HmdQuad_t, bool> GetWorkingPlayAreaRect()
	{
		scalar_result<HmdQuad_t, bool> result;
		result.result_code = chapsi->GetWorkingPlayAreaRect(&result.val);
		return result;
	}

	inline vector_result<HmdQuad_t, bool> GetWorkingCollisionBoundsInfo()
	{
		vector_result<HmdQuad_t, bool> result(string_pool);
		query_vector_rcbool_wrinkle_form(&result, chapsi, &vr::IVRChaperoneSetup::GetWorkingCollisionBoundsInfo);
		return result;
	}

	inline vector_result<HmdQuad_t, bool> GetLiveCollisionBoundsInfo()
	{
		vector_result<HmdQuad_t, bool> result(string_pool);
		query_vector_rcbool_wrinkle_form(&result, chapsi, &vr::IVRChaperoneSetup::GetLiveCollisionBoundsInfo);
		return result;
	}

	inline scalar_result<HmdMatrix34_t, bool> GetWorkingSeatedZeroPoseToRawTrackingPose()
	{
		scalar_result<HmdMatrix34_t,bool> result;
		result.result_code = chapsi->GetWorkingSeatedZeroPoseToRawTrackingPose(&result.val);
		return result;
	}

	inline scalar_result<HmdMatrix34_t, bool> GetWorkingStandingZeroPoseToRawTrackingPose()
	{
		scalar_result<HmdMatrix34_t,bool> result;
		result.result_code = chapsi->GetWorkingStandingZeroPoseToRawTrackingPose(&result.val);
		return result;
	}

	inline vector_result<uint8_t, bool> GetLiveCollisionBoundsTagsInfo()
	{
		vector_result<uint8_t, bool> result(string_pool);
		query_vector_rcbool_wrinkle_form(&result, chapsi, &vr::IVRChaperoneSetup::GetLiveCollisionBoundsTagsInfo);
		return result;
	}

	inline scalar_result<HmdMatrix34_t, bool> GetLiveSeatedZeroPoseToRawTrackingPose()
	{
		scalar_result<HmdMatrix34_t, bool> result;
		result.result_code = chapsi->GetLiveSeatedZeroPoseToRawTrackingPose(&result.val);
		return result;
	}

	inline vector_result<HmdQuad_t, bool>GetLivePhysicalBoundsInfo()
	{
		vector_result<HmdQuad_t, bool> result(string_pool);
		query_vector_rcbool_wrinkle_form(&result, chapsi, &vr::IVRChaperoneSetup::GetLivePhysicalBoundsInfo);
		return result;
	}
	IVRChaperoneSetup *chapsi;
	StringPool *string_pool;
};

struct CompositorWrapper
{
	CompositorWrapper(IVRCompositor *compi_in, StringPool *string_pool_in)
		: compi(compi_in), string_pool(string_pool_in)
	{}

	inline scalar_result<Compositor_FrameTiming, bool> GetFrameTiming(uint32_t frames_ago)
	{
		scalar_result<Compositor_FrameTiming, bool> result;
		vr::Compositor_FrameTiming *p = &result.val;
		p->m_nSize = sizeof(vr::Compositor_FrameTiming);
		result.result_code = compi->GetFrameTiming(&result.val, frames_ago);
		return result;
	}

	inline void GetFrameTimings(uint32_t num_frames, vector_result<Compositor_FrameTiming> *timings)
	{
		assert((int)timings->s.max_count() >= num_frames);
		num_frames = std::min(num_frames, timings->s.max_count());
		vr::Compositor_FrameTiming *p = timings->s.buf();
		for (int i = 0; i < (int)num_frames; i++)
		{
			p->m_nSize = sizeof(vr::Compositor_FrameTiming);
			p++;
		}
		timings->count = compi->GetFrameTimings(timings->s.buf(), num_frames);
	}

	inline scalar<float> GetFrameTimeRemaining()
	{
		return make_scalar(compi->GetFrameTimeRemaining());
		scalar_result<Compositor_FrameTiming, bool> result;
	}

	inline scalar<Compositor_CumulativeStats> GetCumulativeStats()
	{
		scalar<Compositor_CumulativeStats> result;
		compi->GetCumulativeStats(&result.val, sizeof(result.val));
		return result;
	}

	inline scalar<HmdColor_t> GetForegroundFadeColor()
	{
		return make_scalar(compi->GetCurrentFadeColor(false));
	}

	inline scalar<HmdColor_t> GetBackgroundFadeColor()
	{
		return make_scalar(compi->GetCurrentFadeColor(true));
	}

	inline vector_result<char> GetVulkanInstanceExtensionsRequired()
	{
		vector_result<char> result(string_pool);
		query_vector_rcvoid(&result, compi, &IVRCompositor::GetVulkanInstanceExtensionsRequired);
		return result;
	}

	SCALAR_WRAP(IVRCompositor, compi, GetTrackingSpace);
	SCALAR_WRAP(IVRCompositor, compi, GetCurrentGridAlpha);
	SCALAR_WRAP(IVRCompositor, compi, IsFullscreen);
	SCALAR_WRAP(IVRCompositor, compi, GetCurrentSceneFocusProcess);
	SCALAR_WRAP(IVRCompositor, compi, GetLastFrameRenderer);
	SCALAR_WRAP(IVRCompositor, compi, CanRenderScene);
	SCALAR_WRAP(IVRCompositor, compi, IsMirrorWindowVisible);
	SCALAR_WRAP(IVRCompositor, compi, ShouldAppRenderWithLowResources);

	inline void GetLastPoseForTrackedDeviceIndex(TrackedDeviceIndex_t unDeviceIndex,
		scalar_result<TrackedDevicePose_t, EVRCompositorError> *render_pose,
		scalar_result<TrackedDevicePose_t, EVRCompositorError> *game_pose)
	{
		EVRCompositorError e = compi->GetLastPoseForTrackedDeviceIndex(unDeviceIndex, 
			&render_pose->val, 
			&game_pose->val);
		render_pose->result_code = e;
		game_pose->result_code = e;
	}

	IVRCompositor *compi;
	StringPool *string_pool;
};

struct RenderModelWrapper
{
	RenderModelWrapper(IVRRenderModels *remi_in, StringPool *string_pool_in)
		: remi(remi_in), string_pool(string_pool_in)
	{}

	SCALAR_WRAP(IVRRenderModels, remi, GetRenderModelCount);

	inline vector_result<char> GetRenderModelName(uint32_t unRenderModelIndex)
	{
		vector_result<char> render_model_name(string_pool);
		query_vector_rccount(&render_model_name, remi, &IVRRenderModels::GetRenderModelName, unRenderModelIndex);
		return render_model_name;
	}

	inline vector_result<char, EVRRenderModelError> GetRenderModelThumbnailURL(const char *render_model_name)
	{
		vector_result<char, EVRRenderModelError> result(string_pool);
		query_vector_rccount(&result, remi, &IVRRenderModels::GetRenderModelThumbnailURL, render_model_name);
		return result;
	}

	inline vector_result<char, EVRRenderModelError> GetRenderModelOriginalPath(const char *render_model_name)
	{
		vector_result<char, EVRRenderModelError> result(string_pool);
		query_vector_rccount(&result, remi, &IVRRenderModels::GetRenderModelOriginalPath, render_model_name);
		return result;
	}
	
	inline int GetComponentCount(const char *render_model_name)
	{
		return remi->GetComponentCount(render_model_name);
	}

	inline vector_result<char> GetComponentModelName(
		const char *render_model_name, uint32_t unComponentIndex)
	{
		vector_result<char> model_name(string_pool);
		query_vector_rccount(&model_name, remi, &IVRRenderModels::GetComponentName, render_model_name, unComponentIndex);
		return model_name;
	}

	inline scalar<uint64_t> GetComponentButtonMask(const char *render_model_name, const char *component_name)
	{
		return make_scalar(remi->GetComponentButtonMask(render_model_name, component_name));
	}

	inline vector_result<char,bool>GetComponentRenderModelName(const char *render_model_name, const char *component_name)
	{
		vector_result<char,bool> model_name(string_pool);
		query_vector_zero_means_not_present(&model_name, remi, &IVRRenderModels::GetComponentRenderModelName, render_model_name, component_name);
		return model_name;
	}

	inline void GetComponentState(const char *render_model_name, const char *component_name,
		const VRControllerState_t &controller_state,
		bool scroll_wheel_visible,
		scalar_result<RenderModel_ComponentState_t, bool> *transforms)
	{
		RenderModel_ControllerMode_State_t controller_mode_state;
		controller_mode_state.bScrollWheelVisible = scroll_wheel_visible;
		
		bool rc = remi->GetComponentState(
			render_model_name,
			component_name,
			&controller_state,
			&controller_mode_state,
			&transforms->val);

		transforms->result_code = rc;
	}

	EVRRenderModelError LoadRenderModel(
		const char *render_model_name,
		RenderModel_t **pRenderModel,
		RenderModel_TextureMap_t **pTexture)
	{
		EVRRenderModelError error;

		RenderModel_t *model = nullptr;
		RenderModel_TextureMap_t *texture = nullptr;
		while (1)
		{
			error = remi->LoadRenderModel_Async(render_model_name, &model);
			if (error != vr::VRRenderModelError_Loading)
				break;
			sleep_ms(1);
		}
		if (error == VRRenderModelError_None)
		{
			while (1)
			{
				assert(model->diffuseTextureId >= 0); // are there models with no textures?

				error = remi->LoadTexture_Async(model->diffuseTextureId, &texture);
				if (error != vr::VRRenderModelError_Loading)
					break;
				sleep_ms(1);
			}
		}
		if (error == VRRenderModelError_None)
		{
			*pRenderModel = model;
			*pTexture = texture;
		}
		else
		{
			*pRenderModel = nullptr;
			*pTexture = nullptr;
		}
		return error;
	}

	void FreeRenderModel(
		RenderModel_t *pRenderModel,
		RenderModel_TextureMap_t *pTexture)
	{
		remi->FreeRenderModel(pRenderModel);
		remi->FreeTexture(pTexture);
	}

	IVRRenderModels *remi;
	StringPool *string_pool;
};

// construct a scalar only wrapper function indexed by index_type
// function that returns an error code, with value in a
// parameter.
//
// e.g. virtual EVROverlayError GetOverlayAlpha(VROverlayHandle_t ulOverlayHandle, float *pfAlpha) = 0;
//
#define SCALAR_RESULT_WRAP_INDEXED(handle_type, handle, error_type, function, index_type, return_type)\
	inline scalar_result<return_type, error_type> function(index_type index) \
	{ \
		scalar_result<return_type,error_type> rc;\
		rc.result_code = handle->function(index, &rc.val);\
		return rc; \
	}

struct OverlayWrapper
{
	OverlayWrapper(IVROverlay *ovi_in, StringPool *string_pool_in)
		: ovi(ovi_in), string_pool(string_pool_in)
	{}
	
	SCALAR_WRAP(IVROverlay, ovi, GetPrimaryDashboardDevice);
	SCALAR_WRAP(IVROverlay, ovi, GetHighQualityOverlay);
	SCALAR_WRAP(IVROverlay, ovi, IsDashboardVisible);
	SCALAR_WRAP(IVROverlay, ovi, GetGamepadFocusOverlay);

	inline vector_result<char> GetKeyboardText()
	{
		vector_result<char> keyboard_text(string_pool);
		query_vector_rccount(&keyboard_text, ovi, &IVROverlay::GetKeyboardText);
		return keyboard_text;
	}


	inline vector_result<char, EVROverlayError> GetOverlayKey(VROverlayHandle_t h)
	{
		vector_result<char,EVROverlayError> overlay_key(string_pool);
		query_vector_rccount(&overlay_key, ovi, &IVROverlay::GetOverlayKey,h);
		return overlay_key;
	}

	inline void GetOverlayName(VROverlayHandle_t h, vector_result<char, vr::EVROverlayError> *result)
	{
		query_vector_rccount(result, ovi, &IVROverlay::GetOverlayName,h);
	}

	inline scalar_result<VROverlayHandle_t, EVROverlayError> GetOverlayHandle(const char *key)
	{
		scalar_result<VROverlayHandle_t, EVROverlayError> rc;
		rc.result_code = ovi->FindOverlay(key, &rc.val);
		return rc;
	}

	vr::EVROverlayError GetImageData(VROverlayHandle_t ulOverlayHandle,
									scalar_result<uint32_t, EVROverlayError> *width_out,
									scalar_result<uint32_t, EVROverlayError> *height_out,
									uint8_t **ptr_out, uint32_t *size_out)
	{
		uint32_t width_query;
		uint32_t height_query;
		vr::EVROverlayError rc;
		vr::EVROverlayError err = ovi->GetOverlayImageData(ulOverlayHandle, nullptr, 0, &width_query, &height_query);

		if (err != vr::VROverlayError_ArrayTooSmall)
		{
			*width_out = make_scalar<uint32_t, EVROverlayError>(0, err);
			*height_out = make_scalar<uint32_t, EVROverlayError>(0, err);
			*ptr_out = nullptr;
			*size_out = 0;
			rc = err;
		}
		else
		{
			uint32_t size = width_query * height_query * 4;
			uint8_t *ptr = (uint8_t *)malloc(size);
			vr::EVROverlayError err = ovi->GetOverlayImageData(ulOverlayHandle, ptr, size, &width_query, &height_query);
			if (err != vr::VROverlayError_None)
			{
				free(ptr);
				*width_out = make_scalar<uint32_t, EVROverlayError>(0, err);
				*height_out = make_scalar<uint32_t, EVROverlayError>(0, err);
				*ptr_out = nullptr;
				*size_out = 0;
				rc = err;
			}
			else
			{
				*width_out = make_scalar<uint32_t, EVROverlayError>(width_query, err);
				*height_out = make_scalar<uint32_t, EVROverlayError>(height_query, err);
				*ptr_out = ptr;
				*size_out = size;
				rc = err;
			}
		}
		return rc;
	}

	void FreeImageData(void *ptr)
	{
		free(ptr);
	}

	SCALAR_WRAP_INDEXED(IVROverlay, ovi, uint32_t, GetOverlayRenderingPid,	VROverlayHandle_t);
	SCALAR_WRAP_INDEXED(IVROverlay, ovi, bool, IsOverlayVisible,			VROverlayHandle_t);
	SCALAR_WRAP_INDEXED(IVROverlay, ovi, bool, IsHoverTargetOverlay,		VROverlayHandle_t);
	SCALAR_WRAP_INDEXED(IVROverlay, ovi, bool, IsActiveDashboardOverlay,	VROverlayHandle_t);

	inline scalar_result<RGBColor, EVROverlayError> GetOverlayColor(VROverlayHandle_t h)
	{
		scalar_result<RGBColor, EVROverlayError> result;
		result.result_code = ovi->GetOverlayColor(h, &result.val.r, &result.val.g, &result.val.b);
		return result;
	}
	SCALAR_RESULT_WRAP_INDEXED(IVROverlay, ovi, EVROverlayError, GetOverlayAlpha, VROverlayHandle_t, float);
	SCALAR_RESULT_WRAP_INDEXED(IVROverlay, ovi, EVROverlayError, GetOverlayTexelAspect,		VROverlayHandle_t , float);
	SCALAR_RESULT_WRAP_INDEXED(IVROverlay, ovi, EVROverlayError, GetOverlaySortOrder,		VROverlayHandle_t, uint32_t);
	SCALAR_RESULT_WRAP_INDEXED(IVROverlay, ovi, EVROverlayError, GetOverlayWidthInMeters,	VROverlayHandle_t, float);
	SCALAR_RESULT_WRAP_INDEXED(IVROverlay, ovi, EVROverlayError, GetOverlayTextureColorSpace, VROverlayHandle_t, EColorSpace);
	SCALAR_RESULT_WRAP_INDEXED(IVROverlay, ovi, EVROverlayError, GetOverlayTextureBounds,	VROverlayHandle_t, VRTextureBounds_t);
	SCALAR_RESULT_WRAP_INDEXED(IVROverlay, ovi, EVROverlayError, GetOverlayTransformType,	VROverlayHandle_t, VROverlayTransformType);
	SCALAR_RESULT_WRAP_INDEXED(IVROverlay, ovi, EVROverlayError, GetOverlayInputMethod,		VROverlayHandle_t, VROverlayInputMethod);
	SCALAR_RESULT_WRAP_INDEXED(IVROverlay, ovi, EVROverlayError, GetOverlayMouseScale,		VROverlayHandle_t, HmdVector2_t);
	SCALAR_RESULT_WRAP_INDEXED(IVROverlay, ovi, EVROverlayError, GetDashboardOverlaySceneProcess, VROverlayHandle_t,uint32_t);
	SCALAR_RESULT_WRAP_INDEXED(IVROverlay, ovi, EVROverlayError, GetOverlayFlags, VROverlayHandle_t, uint32_t);

	scalar_result<Uint32Size, EVROverlayError> GetOverlayTextureSize(VROverlayHandle_t h)
	{
		scalar_result<Uint32Size, EVROverlayError> rc;
		rc.result_code = ovi->GetOverlayTextureSize(h, &rc.val.width, &rc.val.height);
		return rc;
	}

	scalar_result<FloatRange, EVROverlayError> GetOverlayAutoCurveDistanceRangeInMeters(VROverlayHandle_t h)
	{
		scalar_result<FloatRange, EVROverlayError> rc;
		rc.result_code = ovi->GetOverlayAutoCurveDistanceRangeInMeters(h, &rc.val.min, &rc.val.max);
		return rc;
	}

	scalar_result<AbsoluteTransform, EVROverlayError> GetOverlayTransformAbsolute(VROverlayHandle_t h)
	{
		scalar_result<AbsoluteTransform, EVROverlayError> rc;
		rc.result_code = ovi->GetOverlayTransformAbsolute(h, &rc.val.tracking_origin, &rc.val.origin2overlaytransform);
		return rc;
	}

	scalar_result<TrackedDeviceRelativeTransform, EVROverlayError> GetOverlayTransformTrackedDeviceRelative(VROverlayHandle_t h)
	{
		scalar_result<TrackedDeviceRelativeTransform, EVROverlayError> rc;
		rc.result_code = ovi->GetOverlayTransformTrackedDeviceRelative(h, &rc.val.tracked_device, &rc.val.device2overlaytransform);
		return rc;
	}

	void GetOverlayTransformTrackedDeviceComponent(VROverlayHandle_t h,
		scalar_result<TrackedDeviceIndex_t, EVROverlayError> *device_index,
		vector_result<char, EVROverlayError> *component)
	{
		EVROverlayError rc = ovi->GetOverlayTransformTrackedDeviceComponent(h, &device_index->val,
			component->s.buf(), component->s.max_count());
		device_index->result_code = rc;
		component->result_code = rc;
	}

	IVROverlay *ovi;
	StringPool *string_pool;
};

struct ExtendedDisplayWrapper
{
	ExtendedDisplayWrapper(IVRExtendedDisplay *edi_in, StringPool *string_pool_in)
		: edi(edi_in), string_pool(string_pool_in)
	{}

	inline scalar<WindowBounds_t> GetWindowBounds()
	{
		scalar<WindowBounds_t> ret;
		edi->GetWindowBounds(&ret.val.i[0], &ret.val.i[1], &ret.val.u[0], &ret.val.u[1]);
		return ret;
	}

	inline scalar<ViewPort_t> GetEyeOutputViewport(vr::EVREye eye)
	{
		scalar<ViewPort_t> ret;
		edi->GetEyeOutputViewport(eye, &ret.val.u[0], &ret.val.u[1], &ret.val.u[2], &ret.val.u[3]);
		return ret;
	}

	IVRExtendedDisplay *edi;
	StringPool *string_pool;
};

struct TrackedCameraWrapper
{
	TrackedCameraWrapper(IVRTrackedCamera *taci_in, StringPool *string_pool_in)
		: taci(taci_in), string_pool(string_pool_in)
	{}

	inline scalar_result<bool, EVRTrackedCameraError> HasCamera(vr::TrackedDeviceIndex_t nDeviceIndex)
	{
		scalar_result<bool, EVRTrackedCameraError> result;
		result.result_code = taci->HasCamera(nDeviceIndex, &result.val);
		return result;
	}

	inline scalar_result<CameraFrameSize_t, EVRTrackedCameraError> &GetCameraFrameSize(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType frame_type,
		scalar_result<CameraFrameSize_t, EVRTrackedCameraError> *frame_size)
	{
		EVRTrackedCameraError rc = taci->GetCameraFrameSize( 
											nDeviceIndex, 
											frame_type,
											&frame_size->val.width,
											&frame_size->val.height,
											&frame_size->val.size);

		frame_size->result_code = rc;		
		return *frame_size;
	}

	inline 
		scalar_result<CameraFrameIntrinsics_t, EVRTrackedCameraError> &
		GetCameraIntrinsics(
				vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType frame_type,
				scalar_result<CameraFrameIntrinsics_t, EVRTrackedCameraError> *intrinsics)
	{
		intrinsics->result_code = taci->GetCameraIntrinsics(
			nDeviceIndex,
			frame_type,
			&intrinsics->val.focal_length,
			&intrinsics->val.center);
		return *intrinsics;
	}

	inline scalar_result<HmdMatrix44_t, EVRTrackedCameraError> &GetCameraProjection(
		TrackedDeviceIndex_t device_index,
		EVRTrackedCameraFrameType frame_type,
		float flZNear,
		float flZFar,
		scalar_result<HmdMatrix44_t, EVRTrackedCameraError> *projection)
	{
		projection->result_code = taci->GetCameraProjection(device_index, frame_type, flZNear, flZFar, &projection->val);
		return *projection;
	}

	inline  scalar_result<VideoStreamTextureSize_t, EVRTrackedCameraError>&
		GetVideoStreamTextureSize(
		TrackedDeviceIndex_t device_index,
		EVRTrackedCameraFrameType frame_type,
		scalar_result<VideoStreamTextureSize_t, EVRTrackedCameraError> *size
	)
	{
		size->result_code = taci->GetVideoStreamTextureSize(device_index, frame_type,
			&size->val.texture_bounds,
			&size->val.width,
			&size->val.height);
		return *size;
	}

	IVRTrackedCamera *taci;
	StringPool *string_pool;
};

struct ResourcesWrapper
{
	ResourcesWrapper(IVRResources *resi_in, StringPool *string_pool_in)
		: resi(resi_in), string_pool(string_pool_in)
	{}

	void GetFullPath(const char *filename, const char *directory, vector_result<char> *result)
	{
		query_vector_rccount(result, resi, &IVRResources::GetResourceFullPath, filename, directory);
	}

	uint32_t GetImageData(const char *joinedfilename, uint8_t **ret)
	{
		uint32_t image_size = resi->LoadSharedResource(joinedfilename, nullptr, 0);
		if (image_size > 0)
		{
			char *buf = (char *)malloc(image_size);
			if (buf)
			{
				image_size = resi->LoadSharedResource(joinedfilename, buf, image_size);
			}
			*ret = (uint8_t *)buf;
		}
		else
		{
			*ret = nullptr;
		}
		return image_size;
	}

	void FreeImageData(uint8_t *data)
	{
		if (data)
		{
			free(data);
		}
	}
	
	IVRResources *resi;
	StringPool *string_pool;
};

// create internal index for a key
// overlays: keyed on string,handle->index
// apps: keyed on string->index
// resources: name,dir/name/full_path->index
// unArgsHandle: uint32_t->index
class InternalIndexer
{


};

static void write_string_vector_to_stream(EncodeStream &s, std::vector<std::string> &v)
{
	uint32_t count = v.size();
	encode(count, s);
	
	for (int i = 0; i < (int)count; i++)
	{
		encode(v[i].c_str(), s);
	}
}

static void read_string_vector_from_stream(EncodeStream &s, std::vector<std::string> &v)
{
	uint32_t count;
	decode(count, s);
	v.reserve(count);
	for (int i = 0; i < (int)count; i++)
	{
		char szBuf[256];
		decode_str(szBuf, s);
		v.emplace_back(szBuf);
	}
}

// knows how to map indexes to application keys
class ApplicationsIndexer
{
public:
	ApplicationsIndexer()
	{
	}

	void WriteToStream(EncodeStream &s)
	{
		write_string_vector_to_stream(s, app_keys);
	}

	void ReadFromStream(EncodeStream &s)
	{
		app_keys.clear();
		app_keys2index.clear();
		read_string_vector_from_stream(s, app_keys);
		for (int i = 0; i < (int)app_keys.size(); i++)
		{
			app_keys2index.insert({ app_keys[i], i });
		}
	}

	void update(std::vector<int> *active_indexes, ApplicationsWrapper &ow)
	{
		scalar<uint32_t> count = ow.GetApplicationCount();
		active_indexes->reserve(count.val);
		for (int i = 0; i < (int)count.val; i++)
		{
			vector_result<char, EVRApplicationError> result(ow.string_pool);
			ow.GetApplicationKeyByIndex(i, result);
			std::string key(result.s.buf());	// todo get rid of stupid string
			int index = get_index_for_key(key);	// automatically populates keys
			active_indexes->push_back(index);
		}
	}
	// number of applications ever seen
	int get_num_applications()
	{
		return app_keys.size();
	}

	// index to key mapping
	// say
	// To
	// appa,appb,appc
	// T1
	// appa,appc
	// 
	// the appb disappeared - the key is gone from OpenVR - so the AppHelper will hold it

	const char *get_key_for_index(uint32_t app_index, int *count)
	{
		auto &ref = app_keys[app_index];
		*count = ref.size() + 1;
		return ref.c_str();
	}

	// return a valid index for a key (ie has to be in the active_indexes_set)
	// blargh - overlay doesn't check it versus the active_indexes - and relies on
	// the presence state to indicate to the caller if the value is dead.
	int get_index_for_key(const char *key)
	{
		int ret = -1;
		if (key)
		{
			std::string skey(key);	// todo blargh, just use char *
			auto iter = app_keys2index.find(skey);
			if (iter != app_keys2index.end())
			{
				ret = iter->second;
			}
		}
		return ret;
	}


private:
	// return an index.  if the key doesn't exist yet add it.
	int get_index_for_key(const std::string &key)
	{
		int rc;
		auto iter = app_keys2index.find(key);
		if (iter != app_keys2index.end())
		{
			rc = iter->second;
		}
		else
		{
			app_keys.push_back(key);					// update caches
			rc = app_keys.size() - 1;
			app_keys2index.insert(iter, { key, rc });
		}
		return rc;
	}

	std::vector<std::string> app_keys;
	std::unordered_map<std::string, int> app_keys2index;
};


class ResourcesIndexer
{
public:
	ResourcesIndexer(const char **resource_filenames, const char **initial_resources_dirs, int num_names)
	{
		for (int i = 0; i < num_names; i++)
		{
			m_resource_directories.emplace_back(initial_resources_dirs[i]);
			m_resource_filenames.emplace_back(resource_filenames[i]);
		}
	}

	void WriteToStream(EncodeStream &s)
	{
		write_string_vector_to_stream(s, m_resource_directories);
		write_string_vector_to_stream(s, m_resource_filenames);
	}

	void ReadFromStream(EncodeStream &s)
	{
		m_resource_directories.clear();
		m_resource_filenames.clear();
		
		read_string_vector_from_stream(s, m_resource_directories);
		read_string_vector_from_stream(s, m_resource_filenames);
	}

	const char *get_filename_for_index(int index, int *fname_size)
	{
		*fname_size = m_resource_filenames[index].size() + 1;
		return m_resource_filenames[index].c_str();
	}

	const char * get_directoryname_for_index(int index, int *dname_size)
	{
		*dname_size = m_resource_directories[index].size() + 1;
		return m_resource_directories[index].c_str();
	}

	int get_num_resources() {
		return m_resource_filenames.size();
	}

	std::vector<std::string> m_resource_directories;
	std::vector<std::string> m_resource_filenames;
};

// the idea of the overlay helper is to make it possible to index this thing using integer indexes
// despite the fact that openvr interfaces index using both overlay handles AND string keys
//
class OverlayIndexer
{
public:
	OverlayIndexer(const char **initial_overlay_names, int num_names)
	{
		for (int i = 0; i < num_names; i++)
		{
			std::string s(initial_overlay_names[i]);
			get_overlay_index_for_key(s); // adds and assigns
		}
	}

	void WriteToStream(EncodeStream &s)
	{
		write_string_vector_to_stream(s, overlay_keys);
	}

	void ReadFromStream(EncodeStream &s)
	{
		overlay_keys.clear();
		overlay_keys2index.clear();
		overlay_handle2index.clear();
		read_string_vector_from_stream(s, overlay_keys);
		for (int i = 0; i < (int)overlay_keys.size(); i++)
		{
			overlay_keys2index.insert({ overlay_keys[i], i });
		}
	}


	// walks through known overlays and updates index set
	void update(std::vector<int> *active_indexes, OverlayWrapper &ow)
	{	
		for (int i = 0; i < (int)overlay_keys.size(); i++)
		{
			vr::VROverlayHandle_t handle;
			vr::EVROverlayError e = ow.ovi->FindOverlay(overlay_keys[i].c_str(), &handle);
			if (e == vr::VROverlayError_None)
			{
				// this overlay is active.
				// lookup it's index
				int index = get_overlay_index_for_key(overlay_keys[i]);
				active_indexes->push_back(index);
				overlay_handle2index.insert({ handle, index });	// cache it's handle
			}
		}
	}

	int get_index_for_key(const char *key)
	{
		int ret = -1;
		std::string skey(key);	// todo blargh, just use char *
		auto iter = overlay_keys2index.find(skey);
		if (iter != overlay_keys2index.end())
		{
			ret = iter->second;
		}
		return ret;
	}
	int get_index_for_handle(VROverlayHandle_t h)
	{
		int ret = -1;
		auto iter = overlay_handle2index.find(h);
		if (iter != overlay_handle2index.end())
		{
			ret = iter->second;
		}
		return ret;
	}

	const std::string &get_overlay_key_for_index(const uint32_t overlay_index)
	{
		return overlay_keys[overlay_index];
	}

	int get_num_overlays() const
	{
		return overlay_keys.size();
	}

private:
	// return an index.  if the key doesn't exist yet add it.
	int get_overlay_index_for_key(const std::string &key)
	{
		int rc;
		auto iter = overlay_keys2index.find(key);
		if (iter != overlay_keys2index.end())
		{
			rc = iter->second;
		}
		else
		{
			overlay_keys.push_back(key);					// update caches
			rc = overlay_keys.size() - 1;
			overlay_keys2index.insert(iter, { key, rc });
		}
		return rc;
	}

	std::vector<std::string> overlay_keys;
	std::unordered_map<std::string, int> overlay_keys2index;
	std::unordered_map<VROverlayHandle_t, int> overlay_handle2index;
};


//
// resources outside of the bare openvr interfaces that need to be 
// tracked 
//
struct AdditionalResourceKeys
{
	OverlayIndexer	&GetOverlayIndexer()			{ return m_overlay_indexer; }
	ApplicationsIndexer &GetApplicationsIndexer()	{ return m_applications_indexer; }
	ResourcesIndexer &GetResourcesIndexer()			{ return m_resources_indexer; }
	
	AdditionalResourceKeys(const TrackerConfig &c, ALLOCATOR_DECL)
		:	m_overlay_indexer(c.overlay_keys_to_sample, c.num_overlays_to_sample),
			m_resources_indexer(c.resource_filenames_to_sample, c.resource_directories_to_sample, c.num_resources_to_sample)
	{
		m_data.nearz = c.nearz;
		m_data.farz = c.farz;
		m_data.distortionU = c.distortionU;
		m_data.distortionV = c.distortionV;
		m_data.predicted_seconds_to_photon = c.predicted_seconds_to_photon;
		m_data.num_bounds_colors = c.num_bounds_colors;
		m_data.collision_bounds_fade_distance = c.collision_bounds_fade_distance;
		m_data.frame_timing_frames_ago = c.frame_timing_frames_ago;
		m_data.frame_timings_num_frames = c.frame_timings_num_frames;
	}

	void write_to_stream(EncodeStream &stream)
	{
		stream.memcpy_out_to_stream(&m_data, sizeof(m_data));
		m_overlay_indexer.WriteToStream(stream);
		m_applications_indexer.WriteToStream(stream);
		m_resources_indexer.WriteToStream(stream);
	}

	void read_from_stream(EncodeStream &stream)
	{
		stream.memcpy_from_stream(&m_data, sizeof(m_data));
		m_overlay_indexer.ReadFromStream(stream);
		m_applications_indexer.ReadFromStream(stream);
		m_resources_indexer.ReadFromStream(stream);
	}

	uint64_t GetEncodedSize()
	{
		EncodeStream counter(nullptr, 0, true);
		write_to_stream(counter);
		return counter.buf_pos;		
	}

	void Encode(char *buf, uint64_t buf_size)
	{
		EncodeStream encoder(buf, buf_size, false);
		write_to_stream(encoder);
	}

	void Decode(char *buf, uint64_t buf_size)
	{
		EncodeStream decoder(buf, buf_size, false);
		read_from_stream(decoder);
	}

	float GetNearZ() const { return m_data.nearz; }
	float GetFarZ() const { return m_data.farz; }
	float GetDistortionU() const { return m_data.distortionU; }
	float GetDistortionV() const { return m_data.distortionV; }
	float GetPredictedSecondsToPhoton() const { return m_data.predicted_seconds_to_photon; }
	int GetNumBoundsColors() const { return m_data.num_bounds_colors; }
	float GetCollisionBoundsFadeDistance() const { return m_data.collision_bounds_fade_distance; }
	uint32_t GetFrameTimingFramesAgo() const { return m_data.frame_timing_frames_ago; }
	uint32_t GetFrameTimingsNumFrames() const { return m_data.frame_timings_num_frames; }

private:
	struct {
		float nearz;
		float farz;
		float distortionU;
		float distortionV;
		float predicted_seconds_to_photon;
		int num_bounds_colors;
		float collision_bounds_fade_distance;
		uint32_t frame_timing_frames_ago;
		uint32_t frame_timings_num_frames;
	} m_data;

	

	OverlayIndexer m_overlay_indexer;
	ApplicationsIndexer m_applications_indexer;
	ResourcesIndexer m_resources_indexer;
};


#define START_VECTOR(vector_name) \
visitor.start_vector(#vector_name, ss->vector_name)

#define END_VECTOR(vector_name) \
visitor.end_vector(#vector_name, ss->vector_name)


#define LEAF_SCALAR(member_name, wrapper_call)\
if (visitor.visit_openvr())\
{\
	decltype(wrapper_call) member_name(wrapper_call); \
	visitor.visit_node(ss->member_name.item, member_name);\
}\
else\
{\
	visitor.visit_node(ss->member_name.item);\
}


// leaf 0 because there is no presence/error code type
#define LEAF_VECTOR0(member_name, wrapper_call)\
if (visitor.visit_openvr())\
{\
	decltype(wrapper_call) member_name(wrapper_call); \
	visitor.visit_node(ss->member_name.item, member_name.s.buf(), member_name.count);\
}\
else\
{\
	visitor.visit_node(ss->member_name.item);\
}

// for when there IS a presence type
// LEAF_VECTOR1(supported_mime_types, appw.GetApplicationSupportedMimeTypes(app_key));
// the first case creates member_name from invoking the wrapper.
//
// the second case is broken - to keep the same visit_node() call it creates
// a fake object - even though that side should never be read (since visit_openvr() is false)
//
#define LEAF_VECTOR1(member_name, wrapper_call)\
if (visitor.visit_openvr())\
{\
	decltype(wrapper_call) member_name(wrapper_call); \
	visitor.visit_node(ss->member_name.item, member_name.s.buf(), member_name.result_code, member_name.count);\
}\
else\
{\
	visitor.visit_node(ss->member_name.item); \
}

template <typename visitor_fn, typename T>
static void visit_properties(visitor_fn &visitor, 
	vrstate::properties_subtable<T, vr::TrackedDeviceProperty,vr::ETrackedPropertyError> &sub_table,
	SystemWrapper sysw, vr::TrackedDeviceIndex_t device_index)
{
	for (int i = 0; i < sub_table.tbl_size; i++)
	{
		auto &node = sub_table.props[i];
		scalar_result<T, ETrackedPropertyError> result;
		if (visitor.visit_openvr())
		{
			result = sysw.GetTrackedDeviceProperty<T>(device_index, sub_table.tbl[i].enum_val);
			visitor.visit_node(node.item, result);
		}
		else
		{
			visitor.visit_node(node.item);
		}
	}
}

template <typename visitor_fn, typename T>
static void visit_properties(visitor_fn &visitor,
	vrstate::properties_subtable<T, vr::EVRApplicationProperty, vr::EVRApplicationError> &sub_table,
	ApplicationsWrapper appw, const char *app_key)
{
	for (int i = 0; i < sub_table.tbl_size; i++)
	{
		auto &node = sub_table.props[i];
		scalar_result<T, EVRApplicationError> result;
		if (visitor.visit_openvr())
		{
			result = appw.GetProperty<T>(app_key, sub_table.tbl[i].enum_val);
			visitor.visit_node(node.item, result);
		}
		else
		{
			visitor.visit_node(node.item);
		}
	}
}

template <typename visitor_fn, typename T>
static void visit_string_properties(visitor_fn &visitor,
	vrstate::properties_subtable<T, vr::EVRApplicationProperty, vr::EVRApplicationError> &sub_table,
	ApplicationsWrapper wrap, const char *app_key) 
{
	if (visitor.visit_openvr())
	{
		vector_result<char, EVRApplicationError> result(wrap.string_pool);
		for (int i = 0; i < sub_table.tbl_size; i++)
		{
			auto &node = sub_table.props[i];
			wrap.GetStringProperty(app_key, sub_table.tbl[i].enum_val, &result);
			visitor.visit_node(node.item, result.s.buf(), result.result_code, result.count);
		}
	}
	else
	{
		for (int i = 0; i < sub_table.tbl_size; i++)
		{
			auto &node = sub_table.props[i];
			visitor.visit_node(node.item);
		}
	}
}

template <typename visitor_fn, typename T>
static void visit_string_properties(visitor_fn &visitor, 
	vrstate::properties_subtable<T, vr::TrackedDeviceProperty, vr::ETrackedPropertyError> &sub_table,
	SystemWrapper wrap, vr::TrackedDeviceIndex_t device_index)
{
	if (visitor.visit_openvr())
	{
		vector_result<char, ETrackedPropertyError> result(wrap.string_pool);
		for (int i = 0; i < sub_table.tbl_size; i++)
		{
			auto &node = sub_table.props[i];
			wrap.GetStringTrackedDeviceProperty(device_index, sub_table.tbl[i].enum_val, &result);
			visitor.visit_node(node.item, result.s.buf(), result.result_code, result.count);
		}
	}
	else
	{
		for (int i = 0; i < sub_table.tbl_size; i++)
		{
			auto &node = sub_table.props[i];
			visitor.visit_node(node.item);
		}
	}
}

template <typename visitor_fn>
static void visit_hidden_mesh(visitor_fn &visitor, 
							vrstate::hidden_mesh_schema *ss, 
							vr::EVREye eEye,
							EHiddenAreaMeshType mesh_type,
							IVRSystem *sysi, SystemWrapper wrap)
{
	if (visitor.visit_openvr())
	{
		scalar<uint32_t> hidden_mesh_triangle_count;
		const HmdVector2_t *vertex_data = nullptr;
		uint32_t vertex_data_count = 0;

		vr::HiddenAreaMesh_t mesh = sysi->GetHiddenAreaMesh(eEye, mesh_type);
		vertex_data = mesh.pVertexData;
		hidden_mesh_triangle_count.val = mesh.unTriangleCount;
		if (mesh_type == vr::k_eHiddenAreaMesh_LineLoop)
		{
			vertex_data_count = mesh.unTriangleCount;  // from openvr.h LineLoop->"triangle count" is vertex count
		}
		else
		{
			vertex_data_count = mesh.unTriangleCount * 3;
		} 
		

		visitor.visit_node(ss->hidden_mesh_triangle_count.item, hidden_mesh_triangle_count);
		visitor.visit_node(ss->hidden_mesh_vertices.item, vertex_data, vertex_data_count);
	}
	else
	{
		visitor.visit_node(ss->hidden_mesh_triangle_count.item);
		visitor.visit_node(ss->hidden_mesh_vertices.item);
	}
}

template <typename visitor_fn>
static void visit_eye_state(visitor_fn &visitor, 
							vrstate::eye_schema *ss, 
							vr::EVREye eEye, 
							IVRSystem *sysi, SystemWrapper wrap,
							const AdditionalResourceKeys &c,
							ALLOCATOR_DECL)
{
	visitor.start_group_node("eye", eEye);
	{
		twrap t("   system_eye_scalars");
		LEAF_SCALAR(projection, wrap.GetProjectionMatrix(eEye, c.GetNearZ(), c.GetFarZ()));
		LEAF_SCALAR(eye2head, wrap.GetEyeToHeadTransform(eEye));
		LEAF_SCALAR(projection_raw, wrap.GetProjectionRaw(eEye));
		LEAF_SCALAR(distortion, wrap.ComputeDistortion(eEye, c.GetDistortionU(), c.GetDistortionV()));
	}

	if (ss->hidden_meshes.size() < 3)
	{
		ss->hidden_meshes.reserve(3);
		ss->hidden_meshes.emplace_back(allocator);
		ss->hidden_meshes.emplace_back(allocator);
		ss->hidden_meshes.emplace_back(allocator);
	}
	START_VECTOR(hidden_meshes);
	for (int i = 0; i < 3; i++)
	{
		visit_hidden_mesh(visitor, &ss->hidden_meshes[i], eEye, EHiddenAreaMeshType(i), sysi, wrap);
	}
	END_VECTOR(hidden_meshes);

	visitor.end_group_node("eye", eEye);
}
          
template <typename visitor_fn>
static void visit_component_on_controller_schema(
	visitor_fn &visitor, vrstate::component_on_controller_schema *ss, RenderModelWrapper wrap,
	const char *render_model_name, 
	scalar_result<VRControllerState_t, bool> &controller_state, 
	uint32_t component_index)
{
	visitor.start_group_node("component", component_index);

	scalar_result<RenderModel_ComponentState_t,bool> transforms;
	scalar_result<RenderModel_ComponentState_t, bool> transforms_scroll_wheel;

	if (visitor.visit_openvr() && controller_state.is_present())
	{
		vector_result<char> component_name(wrap.string_pool);
		component_name = wrap.GetComponentModelName(render_model_name, component_index);
		if (component_name.count > 0)
		{
			wrap.GetComponentState(
				render_model_name,
				component_name.s.buf(),
				controller_state.val,		
				false,
				&transforms);

			wrap.GetComponentState(
				render_model_name,
				component_name.s.buf(),
				controller_state.val,		
				true,							// scroll_wheel set to true
				&transforms_scroll_wheel);
		}
		else
		{
			transforms.result_code = false;
			transforms_scroll_wheel.result_code = false;
		}
	}
	if (visitor.visit_openvr())
	{
		visitor.visit_node(ss->transforms.item, transforms);
		visitor.visit_node(ss->transforms_scroll_wheel.item, transforms_scroll_wheel);
	}
	else
	{
		visitor.visit_node(ss->transforms.item);
		visitor.visit_node(ss->transforms_scroll_wheel.item);
	}
	visitor.end_group_node("component", component_index);
}

template <typename visitor_fn>
static void visit_controller_state(visitor_fn &visitor, vrstate::system_controller_schema *ss,
	SystemWrapper wrap, RenderModelWrapper rmw, int controller_index, ALLOCATOR_DECL)
{
	{
		twrap t("   system_controller_scalars");
		LEAF_SCALAR(activity_level, wrap.GetTrackedDeviceActivityLevel(controller_index));
		LEAF_SCALAR(controller_role, wrap.GetControllerRoleForTrackedDeviceIndex(controller_index));
		LEAF_SCALAR(device_class, wrap.GetTrackedDeviceClass(controller_index));
		LEAF_SCALAR(connected, wrap.IsTrackedDeviceConnected(controller_index));
	}

	// used in two places:
	scalar_result<VRControllerState_t, bool> controller_state;
	{
		twrap t("   system_controller_state_with_pose");
		
		
		if (visitor.visit_openvr())
		{
			scalar_result<TrackedDevicePose_t, bool> synced_pose;

			wrap.GetControllerStateWithPose(
				vr::TrackingUniverseSeated, controller_index, &controller_state, &synced_pose);
			visitor.visit_node(ss->synced_seated_pose.item, synced_pose);

			wrap.GetControllerStateWithPose(
				vr::TrackingUniverseStanding, controller_index, &controller_state, &synced_pose);
			visitor.visit_node(ss->synced_standing_pose.item, synced_pose);
					
			wrap.GetControllerStateWithPose(
				vr::TrackingUniverseRawAndUncalibrated, controller_index, &controller_state, &synced_pose);
			visitor.visit_node(ss->synced_raw_pose.item, synced_pose);

			visitor.visit_node(ss->controller_state.item, controller_state);
		}
		else
		{
			visitor.visit_node(ss->synced_seated_pose.item);
			visitor.visit_node(ss->synced_standing_pose.item);
			visitor.visit_node(ss->synced_raw_pose.item);
			visitor.visit_node(ss->controller_state.item);
		}		
	}

	{
		twrap t("   system_controller_properties");
		visit_string_properties(visitor, ss->string_props, wrap, controller_index);
		visit_properties(visitor, ss->bool_props, wrap, controller_index);
		visit_properties(visitor, ss->float_props, wrap, controller_index);
		visit_properties(visitor, ss->mat34_props, wrap, controller_index);
		visit_properties(visitor, ss->int32_props, wrap, controller_index);
		visit_properties(visitor, ss->uint64_props, wrap, controller_index);
	}
	// update the component states on this controller
	// based on: component name
	//           render model name
	//           controller state

	// render model name comes from a property.  to avoid coupling to visit_string_properties, 
	// just look it up again

	{
		twrap t("   system_controller_components");
		vector_result<char, ETrackedPropertyError> render_model(wrap.string_pool);
		int component_count = 0;
		if (visitor.visit_openvr())
		{
			wrap.GetStringTrackedDeviceProperty(controller_index, vr::Prop_RenderModelName_String, &render_model);
			if (render_model.is_present())
			{
				component_count = rmw.GetComponentCount(render_model.s.buf());
				ss->components.reserve(component_count);
				while ((int)ss->components.size() <component_count)
				{
					ss->components.emplace_back(allocator);
				}
			}
		}
		else
		{
			component_count = (int)ss->components.size();
		}

		START_VECTOR(components);
		for (int i = 0; i < (int)ss->components.size(); i++)
		{
			visit_component_on_controller_schema(visitor, &ss->components[i], rmw, render_model.s.buf(), controller_state, i);
		}
		END_VECTOR(components);
	}
}

template <typename visitor_fn>
static void visit_system_node(
								visitor_fn &visitor, 
								vrstate::system_schema *ss, 
								IVRSystem *sysi, SystemWrapper sysw, 
								RenderModelWrapper rmw, 
								const AdditionalResourceKeys &resource_keys,
								ALLOCATOR_DECL)
{
	visitor.start_group_node("system", -1);
	{
		twrap t(" system_node_ scalars");
		LEAF_SCALAR(vr_init_token, sysw.GetInitToken());
		LEAF_SCALAR(recommended_target_size, sysw.GetRecommendedRenderTargetSize());
		LEAF_SCALAR(is_display_on_desktop, sysw.GetIsDisplayOnDesktop());
		LEAF_SCALAR(seated2standing, sysw.GetSeatedZeroPoseToStandingAbsoluteTrackingPose());
		LEAF_SCALAR(raw2standing, sysw.GetRawZeroPoseToStandingAbsoluteTrackingPose());

		LEAF_SCALAR(num_hmd, sysw.CountDevicesOfClass(vr::TrackedDeviceClass_HMD));
		LEAF_SCALAR(num_controller, sysw.CountDevicesOfClass(vr::TrackedDeviceClass_Controller));
		LEAF_SCALAR(num_tracking, sysw.CountDevicesOfClass(vr::TrackedDeviceClass_GenericTracker));
		LEAF_SCALAR(num_reference, sysw.CountDevicesOfClass(TrackedDeviceClass_TrackingReference));

		LEAF_SCALAR(input_focus_captured_by_other, sysw.IsInputFocusCapturedByAnotherProcess());

		
		if (visitor.visit_openvr())
		{
			scalar_result<float, bool>		seconds_since_last_vsync;
			scalar_result<uint64_t, bool>	frame_counter_since_last_vsync;
			sysw.GetTimeSinceLastVsync(&seconds_since_last_vsync,
				&frame_counter_since_last_vsync);
			visitor.visit_node(ss->seconds_since_last_vsync.item, seconds_since_last_vsync);
			visitor.visit_node(ss->frame_counter_since_last_vsync.item, frame_counter_since_last_vsync);
		}
		else
		{
			visitor.visit_node(ss->seconds_since_last_vsync.item);
			visitor.visit_node(ss->frame_counter_since_last_vsync.item);
		}

		LEAF_SCALAR(d3d9_adapter_index, sysw.GetD3D9AdapterIndex());
		LEAF_SCALAR(dxgi_output_info, sysw.GetDXGIOutputInfo());
	}

	//
	// eyes
	//
	{
		twrap t(" system_node_eyes");
		if (ss->eyes.size() < 2)
		{
			ss->eyes.reserve(2);
			ss->eyes.emplace_back(allocator);
			ss->eyes.emplace_back(allocator);
		}
		START_VECTOR(eyes);
		for (int i = 0; i < 2; i++)
		{
			vrstate::eye_schema *es = &ss->eyes[i];
			vr::EVREye eEye = (i == 0) ? vr::Eye_Left : vr::Eye_Right;
			visit_eye_state(visitor, &ss->eyes[i], eEye, sysi, sysw, resource_keys, allocator);
		}
		END_VECTOR(eyes);
	}

	//
	// controllers
	//
	{
		twrap t(" system_node_controller poses");
		ss->controllers.reserve(vr::k_unMaxTrackedDeviceCount);
		while (ss->controllers.size() < vr::k_unMaxTrackedDeviceCount)
		{
			ss->controllers.emplace_back(allocator);
		}
		START_VECTOR(controllers);
		{
			TrackedDevicePose_t raw_pose_array[vr::k_unMaxTrackedDeviceCount];
			TrackedDevicePose_t standing_pose_array[vr::k_unMaxTrackedDeviceCount];
			TrackedDevicePose_t seated_pose_array[vr::k_unMaxTrackedDeviceCount];

			if (visitor.visit_openvr())
			{
				memset(raw_pose_array, 0, sizeof(raw_pose_array));	// 2/6/2017 - on error this stuff should be zero
				memset(standing_pose_array, 0, sizeof(standing_pose_array));
				memset(seated_pose_array, 0, sizeof(seated_pose_array));
				sysi->GetDeviceToAbsoluteTrackingPose(TrackingUniverseRawAndUncalibrated, 
									resource_keys.GetPredictedSecondsToPhoton(), raw_pose_array, vr::k_unMaxTrackedDeviceCount);

				sysi->GetDeviceToAbsoluteTrackingPose(TrackingUniverseStanding, 
									resource_keys.GetPredictedSecondsToPhoton(), standing_pose_array, vr::k_unMaxTrackedDeviceCount);

				sysi->GetDeviceToAbsoluteTrackingPose(TrackingUniverseSeated, 
									resource_keys.GetPredictedSecondsToPhoton(), seated_pose_array, vr::k_unMaxTrackedDeviceCount);
			}
			for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
			{
				visitor.start_group_node("controller", i);
				if (visitor.visit_openvr())
				{
					visitor.visit_node(ss->controllers[i].raw_tracking_pose.item, make_scalar(raw_pose_array[i]));
					visitor.visit_node(ss->controllers[i].standing_tracking_pose.item, make_scalar(standing_pose_array[i]));
					visitor.visit_node(ss->controllers[i].seated_tracking_pose.item, make_scalar(seated_pose_array[i]));
				}
				else
				{
					visitor.visit_node(ss->controllers[i].raw_tracking_pose.item);
					visitor.visit_node(ss->controllers[i].standing_tracking_pose.item);
					visitor.visit_node(ss->controllers[i].seated_tracking_pose.item);
				}
				visit_controller_state(visitor, &ss->controllers[i], sysw, rmw, i, allocator);
				visitor.end_group_node("controller", i);
			}
		}
		END_VECTOR(controllers);
	}

	//
	// spatial sorts
	//
	ss->spatial_sorts.reserve(vr::k_unMaxTrackedDeviceCount+1);
	while (ss->spatial_sorts.size() < vr::k_unMaxTrackedDeviceCount+1)
	{
		ss->spatial_sorts.emplace_back(allocator);
	}
	START_VECTOR(spatial_sorts);
	for (unsigned i = 0; i < k_unMaxTrackedDeviceCount + 1; i++)
	{
		unsigned unRelativeToTrackedDeviceIndex = -1 + i;
		if (visitor.visit_openvr())
		{
			vector_result<TrackedDeviceIndex_t> result(sysw.string_pool);

			sysw.GetSortedTrackedDeviceIndicesOfClass(TrackedDeviceClass_HMD, unRelativeToTrackedDeviceIndex, &result);
			visitor.visit_node(ss->spatial_sorts[i].hmds_sorted.item, result.s.buf(), result.count);

			sysw.GetSortedTrackedDeviceIndicesOfClass(TrackedDeviceClass_Controller, unRelativeToTrackedDeviceIndex, &result);
			visitor.visit_node(ss->spatial_sorts[i].controllers_sorted.item, result.s.buf(), result.count);

			sysw.GetSortedTrackedDeviceIndicesOfClass(TrackedDeviceClass_GenericTracker, unRelativeToTrackedDeviceIndex, &result);
			visitor.visit_node(ss->spatial_sorts[i].trackers_sorted.item, result.s.buf(), result.count);

			sysw.GetSortedTrackedDeviceIndicesOfClass(TrackedDeviceClass_TrackingReference, unRelativeToTrackedDeviceIndex, &result);
			visitor.visit_node(ss->spatial_sorts[i].reference_sorted.item, result.s.buf(), result.count);
		}
		else
		{
			visitor.visit_node(ss->spatial_sorts[i].hmds_sorted.item);
			visitor.visit_node(ss->spatial_sorts[i].controllers_sorted.item);
			visitor.visit_node(ss->spatial_sorts[i].trackers_sorted.item);
			visitor.visit_node(ss->spatial_sorts[i].reference_sorted.item);
		}
	}

	END_VECTOR(spatial_sorts);

	visitor.end_group_node("system_node", 0);
}


template <typename visitor_fn>
void visit_application_state(visitor_fn &visitor, vrstate::application_schema *ss,
	ApplicationsWrapper wrap, uint32_t app_index, ApplicationsIndexer *helper)
{
	visitor.start_group_node("app_state", app_index);

	const char *app_key = nullptr;
	if (visitor.visit_openvr())
	{
		int count;
		app_key = helper->get_key_for_index(app_index, &count);
		visitor.visit_node(ss->application_key.item, app_key, count);
		scalar<uint32_t> process_id = wrap.GetApplicationProcessId(app_key);
		LEAF_SCALAR(process_id, process_id);
		LEAF_VECTOR0(application_launch_arguments, wrap.GetApplicationLaunchArguments(process_id.val));
	}
	else
	{
		visitor.visit_node(ss->application_key.item);
		visitor.visit_node(ss->process_id.item);
		visitor.visit_node(ss->application_launch_arguments.item);
	}

	LEAF_SCALAR(is_installed, wrap.IsApplicationInstalled(app_key));
	LEAF_SCALAR(auto_launch, wrap.GetApplicationAutoLaunch(app_key));
	LEAF_VECTOR1(supported_mime_types, wrap.GetApplicationSupportedMimeTypes(app_key));
	
	visit_string_properties(visitor, ss->string_props, wrap, app_key);
	visit_properties(visitor, ss->bool_props, wrap, app_key);
	visit_properties(visitor, ss->uint64_props, wrap, app_key);

	visitor.end_group_node("app_state", app_index);
}

template <typename visitor_fn>
void visit_mime_type_schema(visitor_fn &visitor, vrstate::mime_type_schema *ss,
	ApplicationsWrapper wrap, uint32_t mime_index)
{
	const char *mime_type = mime_types[mime_index].name;
	if (visitor.visit_openvr())
	{
		visitor.visit_node(ss->mime_type.item, mime_types[mime_index].name, (int)strlen(mime_type) + 1);
	}
	else
	{
		visitor.visit_node(ss->mime_type.item);
	}

	LEAF_VECTOR1(default_application, wrap.GetDefaultApplicationForMimeType(mime_type));
	LEAF_VECTOR0(applications_that_support_mime_type, wrap.GetApplicationsThatSupportMimeType(mime_type));
}


template <typename visitor_fn>
static void visit_applications_node(visitor_fn &visitor, vrstate::applications_schema *ss, ApplicationsWrapper wrap, 
	 AdditionalResourceKeys &resource_keys, ALLOCATOR_DECL)
{
	visitor.start_group_node("app", -1);

	if (visitor.visit_openvr())
	{
		std::vector<int> active_indexes;
		resource_keys.GetApplicationsIndexer().update(&active_indexes, wrap);

		int *ptr = nullptr;
		if (active_indexes.size() > 0)
		{
			ptr = &active_indexes.at(0);
		}
		visitor.visit_node(ss->active_application_indexes.item, ptr, active_indexes.size());
	}
	else
	{
		visitor.visit_node(ss->active_application_indexes.item);
	}

	if (resource_keys.GetApplicationsIndexer().get_num_applications() > (int)ss->applications.size())
	{
		ss->applications.reserve(resource_keys.GetApplicationsIndexer().get_num_applications());
		while ((int)ss->applications.size() < resource_keys.GetApplicationsIndexer().get_num_applications())
		{
			ss->applications.emplace_back(allocator);
		}
	}

	LEAF_VECTOR1(starting_application, wrap.GetStartingApplication());
	LEAF_SCALAR(transition_state, wrap.GetTransitionState());
	LEAF_SCALAR(is_quit_user_prompt, wrap.IsQuitUserPromptRequested());
	LEAF_SCALAR(current_scene_process_id, wrap.GetCurrentSceneProcessId());

	ss->mime_types.reserve(mime_tbl_size);
	while (ss->mime_types.size() < mime_tbl_size)
	{
		ss->mime_types.emplace_back(allocator);
	}

	START_VECTOR(mime_types);
	for (int i = 0; i < mime_tbl_size; i++)
	{
		visit_mime_type_schema(visitor, &ss->mime_types[i], wrap, i);
	}
	END_VECTOR(mime_types);

	START_VECTOR(applications);
	for (int i = 0; i < (int)ss->applications.size(); i++)
	{
		visit_application_state(visitor, &ss->applications[i], wrap, i, &resource_keys.GetApplicationsIndexer());
	}
	END_VECTOR(applications);

	visitor.end_group_node("app", -1);
}

template <typename visitor_fn, typename T>
static void visit_subtable(visitor_fn &visitor, vrstate::setting_subtable<T> &subtable, 
							SettingsWrapper sw, const char *section_name)
{
	for (auto iter = subtable.nodes.begin(); iter != subtable.nodes.end(); iter++)
	{
		scalar_result<T, EVRSettingsError> result;
		if (visitor.visit_openvr())
		{
			// the name of the node is the setting name
			const char *setting_name = iter->item.name;
			result = sw.GetSetting<T>(section_name, setting_name);
			visitor.visit_node(iter->item, result);
		}
		else
		{
			visitor.visit_node(iter->item);
		}
	}
}

template <typename visitor_fn, typename T>
static void visit_string_subtable(
									visitor_fn &visitor, 
									vrstate::setting_subtable<T> &subtable,
									SettingsWrapper &wrap, 
									const char *section_name)
{
	for (auto iter = subtable.nodes.begin(); iter != subtable.nodes.end(); iter++)
	{
		if (visitor.visit_openvr())
		{
			// the name of the node is the setting name
			const char *setting_name = iter->item.name;
			vector_result<char, EVRSettingsError> result = wrap.GetStringSetting(section_name, setting_name);
			visitor.visit_node(iter->item, result.s.buf(), result.result_code, result.count);
		}
		else
		{
			visitor.visit_node(iter->item);
		}
	}
}

template <typename visitor_fn>
static void visit_section(
							visitor_fn &visitor, 
							vrstate::settings_schema::section_schema *s,
							SettingsWrapper &wrap)
{
	visit_subtable<visitor_fn, bool>(visitor, s->bool_settings, wrap, s->section_name);
	visit_string_subtable(visitor, s->string_settings, wrap, s->section_name);
	visit_subtable<visitor_fn, float>(visitor, s->float_settings, wrap, s->section_name);
	visit_subtable<visitor_fn, int32_t>(visitor, s->int32_settings, wrap, s->section_name);
}

template <typename visitor_fn>
static void visit_settings_node(
									visitor_fn &visitor, 
									vrstate::settings_schema *ss, 
									SettingsWrapper& wrap)
{
	visitor.start_group_node("settings_schema", -1);
		for (auto iter = ss->sections.begin(); iter != ss->sections.end(); iter++)
		{
			visit_section(visitor, &(*iter), wrap);
		}
	visitor.end_group_node("settings_schema", -1);
}

template <typename visitor_fn>
static void visit_chaperone_node(
									visitor_fn &visitor, 
									vrstate::chaperone_schema *ss,
									ChaperoneWrapper &wrap,
									const AdditionalResourceKeys &resource_keys)
{
	visitor.start_group_node("chaperone_schema", -1);
		LEAF_SCALAR(calibration_state, wrap.GetCalibrationState());
		LEAF_SCALAR(bounds_visible,   wrap.AreBoundsVisible());
		LEAF_SCALAR(play_area_rect, wrap.GetPlayAreaRect());
		LEAF_SCALAR(play_area_size, wrap.GetPlayAreaSize());

		if (visitor.visit_openvr())
		{
			vector_result<HmdColor_t> colors(wrap.string_pool);
			scalar<HmdColor_t> camera_color;

			wrap.GetBoundsColor(&colors, resource_keys.GetNumBoundsColors(), 
						resource_keys.GetCollisionBoundsFadeDistance(), 
						&camera_color);

			visitor.visit_node(ss->bounds_colors.item, colors.s.buf(), colors.count);
			visitor.visit_node(ss->camera_color.item, camera_color);
		}
		else
		{
			visitor.visit_node(ss->bounds_colors.item);
			visitor.visit_node(ss->camera_color.item);
		}

	visitor.end_group_node("chaperone_schema", -1);
}

template <typename visitor_fn>
static void visit_chaperone_setup_node(visitor_fn &visitor, vrstate::chaperonesetup_schema *ss, ChaperoneSetupWrapper wrap)
{
	visitor.start_group_node("chaperone_setup", -1);
		LEAF_SCALAR(working_play_area_size , wrap.GetWorkingPlayAreaSize());
		LEAF_SCALAR(working_play_area_rect , wrap.GetWorkingPlayAreaRect());
		LEAF_VECTOR1(working_collision_bounds_info , wrap.GetWorkingCollisionBoundsInfo());
		LEAF_VECTOR1(live_collision_bounds_info, wrap.GetLiveCollisionBoundsInfo());
		LEAF_SCALAR(working_seated2rawtracking , wrap.GetWorkingSeatedZeroPoseToRawTrackingPose());
		LEAF_SCALAR(working_standing2rawtracking , wrap.GetWorkingStandingZeroPoseToRawTrackingPose());
		LEAF_VECTOR1(live_collision_bounds_tags_info , wrap.GetLiveCollisionBoundsTagsInfo());
		LEAF_SCALAR(live_seated2rawtracking , wrap.GetLiveSeatedZeroPoseToRawTrackingPose());
		LEAF_VECTOR1(live_physical_bounds_info , wrap.GetLivePhysicalBoundsInfo());
	visitor.end_group_node("chaperone_setup", -1);
}

template <typename visitor_fn>
static void visit_compositor_controller(visitor_fn &visitor, 
										vrstate::compositor_controller_schema *ss,
										CompositorWrapper cw, TrackedDeviceIndex_t unDeviceIndex)
{
	visitor.start_group_node("controller", unDeviceIndex);
	if (visitor.visit_openvr())
	{
		scalar_result<TrackedDevicePose_t, EVRCompositorError> last_render_pose;
		scalar_result<TrackedDevicePose_t, EVRCompositorError> last_game_pose;
		cw.GetLastPoseForTrackedDeviceIndex(unDeviceIndex, &last_render_pose, &last_game_pose);
		visitor.visit_node(ss->last_render_pose.item, last_render_pose);
		visitor.visit_node(ss->last_game_pose.item, last_game_pose);
	}
	else
	{
		visitor.visit_node(ss->last_render_pose.item);
		visitor.visit_node(ss->last_game_pose.item);
	}
	
	visitor.end_group_node("controller", unDeviceIndex);
}

template <typename visitor_fn>
static void visit_compositor_state(visitor_fn &visitor, 
									vrstate::compositor_schema *ss, CompositorWrapper wrap, 
									AdditionalResourceKeys &config, ALLOCATOR_DECL)
{
	visitor.start_group_node("compositor_schema", -1);
	{
		twrap t(" compositor_schema scalars");

		LEAF_SCALAR(tracking_space, wrap.GetTrackingSpace());
		LEAF_SCALAR(frame_timing, wrap.GetFrameTiming(config.GetFrameTimingFramesAgo()));
		LEAF_SCALAR(frame_time_remaining, wrap.GetFrameTimeRemaining());
		LEAF_SCALAR(cumulative_stats, wrap.GetCumulativeStats());
		LEAF_SCALAR(foreground_fade_color, wrap.GetForegroundFadeColor());
		LEAF_SCALAR(background_fade_color, wrap.GetBackgroundFadeColor());
		LEAF_SCALAR(grid_alpha, wrap.GetCurrentGridAlpha());
		LEAF_SCALAR(is_fullscreen, wrap.IsFullscreen());
		LEAF_SCALAR(current_scene_focus_process, wrap.GetCurrentSceneFocusProcess());
		LEAF_SCALAR(last_frame_renderer, wrap.GetLastFrameRenderer());
		LEAF_SCALAR(can_render_scene, wrap.CanRenderScene());
		LEAF_SCALAR(is_mirror_visible, wrap.IsMirrorWindowVisible());
		LEAF_SCALAR(should_app_render_with_low_resource, wrap.ShouldAppRenderWithLowResources());
	}

	if (visitor.visit_openvr())
	{
		vector_result<Compositor_FrameTiming> frame_timings(wrap.string_pool);
		wrap.GetFrameTimings(config.GetFrameTimingsNumFrames(), &frame_timings);
		visitor.visit_node(ss->frame_timings.item, frame_timings.s.buf(), frame_timings.count);
	}
	else
	{
		visitor.visit_node(ss->frame_timings.item);
	}
		

	{
		twrap t(" compositor_schema controllers");
		ss->controllers.reserve(vr::k_unMaxTrackedDeviceCount);
		while (ss->controllers.size() < vr::k_unMaxTrackedDeviceCount)
		{
			ss->controllers.emplace_back(allocator);
		}

		START_VECTOR(controllers);
		for (int i = 0; i < (int)ss->controllers.size(); i++)
		{
			visit_compositor_controller(visitor, &ss->controllers[i], wrap, i);
		}
		END_VECTOR(controllers);
	}

	{
		twrap t(" compositor_schema vulkan");
		LEAF_VECTOR0(instance_extensions_required, wrap.GetVulkanInstanceExtensionsRequired());
	}

	visitor.end_group_node("compositor_schema", -1);
}

template <typename visitor_fn>
static void visit_permodelcomponent(
	visitor_fn &visitor, 
	vrstate::rendermodel_component_schema *ss, RenderModelWrapper wrap,
	const char *pchRenderModelName, uint32_t component_index)
{
	visitor.start_group_node("component", component_index);
	
	if (visitor.visit_openvr())
	{
		vector_result<char> component_model_name
				= wrap.GetComponentModelName(pchRenderModelName, component_index);
		visitor.visit_node(ss->component_name.item, component_model_name.s.buf(), component_model_name.count);
		LEAF_SCALAR(button_mask, wrap.GetComponentButtonMask(pchRenderModelName, component_model_name.s.buf()));
		LEAF_VECTOR1(render_model_name, wrap.GetComponentRenderModelName(pchRenderModelName, component_model_name.s.buf()));
	}
	else
	{
		visitor.visit_node(ss->component_name.item);
		visitor.visit_node(ss->button_mask.item);
		visitor.visit_node(ss->render_model_name.item);
	}
	
	visitor.end_group_node("component", component_index);
}

template <typename visitor_fn>
static void visit_rendermodel(visitor_fn &visitor, 
					vrstate::rendermodel_schema *ss, RenderModelWrapper wrap,
					uint32_t unRenderModelIndex, ALLOCATOR_DECL)
{
	
	visitor.start_group_node("model", unRenderModelIndex);
	if (visitor.visit_openvr())
	{
		vector_result<char> render_model_name_result(wrap.string_pool);
		render_model_name_result = wrap.GetRenderModelName(unRenderModelIndex);
		visitor.visit_node(ss->render_model_name.item, render_model_name_result.s.buf(), render_model_name_result.count);
		LEAF_VECTOR1(thumbnail_url, wrap.GetRenderModelThumbnailURL(render_model_name_result.s.buf()));
		LEAF_VECTOR1(original_path, wrap.GetRenderModelOriginalPath(render_model_name_result.s.buf()));
	}
	else
	{
		visitor.visit_node(ss->render_model_name.item);
		visitor.visit_node(ss->thumbnail_url.item);
		visitor.visit_node(ss->original_path.item);
	}

	// every body wants the render model name
	const char *render_model_name = &ss->render_model_name.item.latest().at(0);
	
	RenderModel_t *pRenderModel = nullptr;
	RenderModel_TextureMap_t *pTexture = nullptr;
	EVRRenderModelError rc = VRRenderModelError_None;
	const RenderModel_Vertex_t *rVertexData=nullptr;	// Vertex data for the mesh
	uint32_t unVertexCount = 0;						// Number of vertices in the vertex data
	const uint16_t *rIndexData=nullptr;
	uint32_t unTriangleCount = 0;
	uint16_t unWidth, unHeight; // width and height of the texture map in pixels
	unWidth = unHeight = 0;
	const uint8_t *rubTextureMapData=nullptr;
	if (visitor.visit_openvr())
	{
		if (visitor.reload_render_models() || ss->vertex_data.item.empty())
		{
			rc = wrap.LoadRenderModel(render_model_name, &pRenderModel, &pTexture);
			if (pRenderModel)
			{
				rVertexData = pRenderModel->rVertexData;
				unVertexCount = pRenderModel->unVertexCount;
				rIndexData = pRenderModel->rIndexData;
				unTriangleCount = pRenderModel->unTriangleCount;
			}
			if (pTexture)
			{
				unWidth = pTexture->unWidth;
				unHeight = pTexture->unHeight;
				rubTextureMapData = pTexture->rubTextureMapData;
			}
			visitor.visit_node(ss->vertex_data.item, rVertexData, rc, unVertexCount);
			visitor.visit_node(ss->index_data.item, rIndexData, rc, unTriangleCount * 3);
			visitor.visit_node(ss->texture_map_data.item, rubTextureMapData, rc, unWidth * unHeight);

			scalar_result<uint16_t, EVRRenderModelError> height(unHeight, rc);
			scalar_result<uint16_t, EVRRenderModelError> width(unWidth, rc);
			visitor.visit_node(ss->texture_height.item, height);
			visitor.visit_node(ss->texture_width.item, width);

			// Note: TextureID_t::diffuseTextureId is not stored because the whole texture is stored.

			if (pRenderModel)
			{
				wrap.FreeRenderModel(pRenderModel, pTexture);
			}
		}
	}
	else
	{
		visitor.visit_node(ss->vertex_data.item);
		visitor.visit_node(ss->index_data.item);
		visitor.visit_node(ss->texture_map_data.item);
		visitor.visit_node(ss->texture_height.item);
		visitor.visit_node(ss->texture_width.item);
	}

	// set default
	int component_count = (int)ss->components.size();
	if (visitor.visit_openvr())
	{
		component_count = (int)wrap.GetComponentCount(render_model_name);
		ss->components.reserve(component_count);
		while ((int)ss->components.size() < component_count)
		{
			ss->components.emplace_back(allocator);
		}
	}
	// consider the decoder - he needs to know how many componets are going to
	// be sent to him
	START_VECTOR(components);
	for (int i = 0; i < (int)ss->components.size(); i++)
	{
		visit_permodelcomponent(visitor, &ss->components[i], wrap, render_model_name, i);
	}
	END_VECTOR(components);
	visitor.end_group_node("model", unRenderModelIndex);
}

template <typename visitor_fn>
static void visit_per_overlay(
	visitor_fn &visitor, 
	vrstate::overlay_schema *overlay_state, 
	OverlayWrapper wrap, 
	uint32_t overlay_index,
	AdditionalResourceKeys &config,
	ALLOCATOR_DECL)
{
	visitor.start_group_node("overlay", overlay_index);

	vrstate::per_overlay_state *ss = &overlay_state->overlays[overlay_index];
	vr::VROverlayHandle_t handle = 0;

	if (visitor.visit_openvr())
	{
		const std::string &key = config.GetOverlayIndexer().get_overlay_key_for_index(overlay_index);
		scalar_result<VROverlayHandle_t, EVROverlayError> handle_result = wrap.GetOverlayHandle(key.c_str());
		handle = handle_result.val;
		vector_result<char, vr::EVROverlayError> name(wrap.string_pool);
		wrap.GetOverlayName(handle, &name);

		visitor.visit_node(ss->overlay_key.item, key.c_str(), key.size() + 1);
		visitor.visit_node(ss->overlay_handle.item, handle_result);
		visitor.visit_node(ss->overlay_name.item, name.s.buf(), name.result_code, name.count);

		scalar_result<uint32_t, EVROverlayError> width;
		scalar_result<uint32_t, EVROverlayError> height;
		uint8_t *ptr;
		uint32_t size;
		EVROverlayError err = wrap.GetImageData(handle,&width, &height, &ptr, &size);
		visitor.visit_node(ss->overlay_image_width.item, width);
		visitor.visit_node(ss->overlay_image_height.item, height);
		visitor.visit_node(ss->overlay_image_data.item, ptr, err, size);
		wrap.FreeImageData(ptr);
	}
	else
	{
		visitor.visit_node(ss->overlay_key.item);
		visitor.visit_node(ss->overlay_handle.item);
		visitor.visit_node(ss->overlay_name.item);

		visitor.visit_node(ss->overlay_image_width.item);
		visitor.visit_node(ss->overlay_image_height.item);
		visitor.visit_node(ss->overlay_image_data.item);
	}

	LEAF_SCALAR(overlay_rendering_pid,				wrap.GetOverlayRenderingPid(handle));
	LEAF_SCALAR(overlay_flags,						wrap.GetOverlayFlags(handle));
	LEAF_SCALAR(overlay_color,						wrap.GetOverlayColor(handle));
	LEAF_SCALAR(overlay_alpha,						wrap.GetOverlayAlpha(handle));
	LEAF_SCALAR(overlay_texel_aspect,				wrap.GetOverlayTexelAspect(handle));
	LEAF_SCALAR(overlay_sort_order,					wrap.GetOverlaySortOrder(handle));
	LEAF_SCALAR(overlay_width_in_meters,			wrap.GetOverlayWidthInMeters(handle));
	LEAF_SCALAR(overlay_auto_curve_range_in_meters, wrap.GetOverlayAutoCurveDistanceRangeInMeters(handle));
	LEAF_SCALAR(overlay_texture_color_space,		wrap.GetOverlayTextureColorSpace(handle));
	LEAF_SCALAR(overlay_texture_bounds,				wrap.GetOverlayTextureBounds(handle));
	LEAF_SCALAR(overlay_transform_type,				wrap.GetOverlayTransformType(handle));
	LEAF_SCALAR(overlay_transform_absolute,			wrap.GetOverlayTransformAbsolute(handle));
	LEAF_SCALAR(overlay_transform_device_relative,  wrap.GetOverlayTransformTrackedDeviceRelative(handle));

	LEAF_SCALAR(overlay_input_method,				wrap.GetOverlayInputMethod(handle));
	LEAF_SCALAR(overlay_mouse_scale,				wrap.GetOverlayMouseScale(handle));
	LEAF_SCALAR(overlay_is_hover_target,			wrap.IsHoverTargetOverlay(handle));
	LEAF_SCALAR(overlay_is_visible,					wrap.IsOverlayVisible(handle));
	LEAF_SCALAR(overlay_is_active_dashboard,		wrap.IsActiveDashboardOverlay(handle));
	LEAF_SCALAR(overlay_dashboard_scene_process,	wrap.GetDashboardOverlaySceneProcess(handle));
	LEAF_SCALAR(overlay_texture_size,				wrap.GetOverlayTextureSize(handle));
	

	if (visitor.visit_openvr())
	{
		scalar_result<TrackedDeviceIndex_t, EVROverlayError> device_index;
		vector_result<char, EVROverlayError> name(wrap.string_pool);
		wrap.GetOverlayTransformTrackedDeviceComponent(handle, &device_index, &name);

		visitor.visit_node(ss->overlay_transform_component_relative_device_index.item, device_index);
		visitor.visit_node(ss->overlay_transform_component_relative_name.item, name.s.buf(), name.result_code, name.count);
	}
	else
	{
		visitor.visit_node(ss->overlay_transform_component_relative_device_index.item);
		visitor.visit_node(ss->overlay_transform_component_relative_name.item);
	}

	visitor.end_group_node("overlay", overlay_index);
}

template <typename visitor_fn>
static void visit_overlay_state(visitor_fn &visitor, vrstate::overlay_schema *ss, 
								OverlayWrapper ow, 
								AdditionalResourceKeys &config,
								ALLOCATOR_DECL)
{
	visitor.start_group_node("overlays", -1);

	LEAF_SCALAR(gamepad_focus_overlay, ow.GetGamepadFocusOverlay());
	LEAF_SCALAR(primary_dashboard_device, ow.GetPrimaryDashboardDevice());

	if (visitor.visit_openvr())
	{
		std::vector<int> active_indexes;
		
		config.GetOverlayIndexer().update(&active_indexes, ow);

		int *ptr = nullptr;
		if (active_indexes.size() > 0)
		{
			ptr = &active_indexes.at(0);
		}
		visitor.visit_node(ss->active_overlay_indexes.item, ptr, active_indexes.size());
	}
	else
	{
		visitor.visit_node(ss->active_overlay_indexes.item);
	}

	if (config.GetOverlayIndexer().get_num_overlays() > (int)ss->overlays.size())
	{
		ss->overlays.reserve(config.GetOverlayIndexer().get_num_overlays());
		while ((int)ss->overlays.size() < config.GetOverlayIndexer().get_num_overlays())
		{
			ss->overlays.emplace_back(allocator);
		}
	}

	// history traversal always goes through the complete set
	// see "How to track applications and overlays.docx"
	START_VECTOR(overlays);
	for (int i = 0; i < (int)ss->overlays.size(); i++)
	{
		visit_per_overlay(visitor, ss, ow, i, config, allocator);
	}
	END_VECTOR(overlays);

	visitor.end_group_node("overlays", -1);
}

template <typename visitor_fn>
static void visit_rendermodel_state(visitor_fn &visitor, vrstate::rendermodels_schema *ss, RenderModelWrapper rmw, ALLOCATOR_DECL)
{
	visitor.start_group_node("render_model", -1);
	
	if (visitor.visit_openvr())
	{
		scalar<uint32_t> current_rendermodels = rmw.GetRenderModelCount();
		int num_render_models = current_rendermodels.val;
		ss->models.reserve(num_render_models);
		while ((int)ss->models.size() < num_render_models)
		{
			ss->models.emplace_back(allocator);
		}
	}

	START_VECTOR(models);
	for (int i = 0; i < (int)ss->models.size(); i++)
	{
		visit_rendermodel(visitor, &ss->models[i], rmw, i, allocator);
	}
	END_VECTOR(models);
	visitor.end_group_node("render_model", -1);
}

template <typename visitor_fn>
static void visit_extended_display_state(visitor_fn &visitor, vrstate::extendeddisplay_schema *ss, ExtendedDisplayWrapper ew)
{
	visitor.start_group_node("extended_display", -1);
	LEAF_SCALAR(window_bounds, ew.GetWindowBounds());
	LEAF_SCALAR(left_output_viewport, ew.GetEyeOutputViewport(vr::Eye_Left));
	LEAF_SCALAR(right_output_viewport, ew.GetEyeOutputViewport(vr::Eye_Right));
	visitor.end_group_node("extended_display", -1);
}

static const char *FrameTypeToGroupName(EVRTrackedCameraFrameType f)
{
	switch (f)
	{
		case VRTrackedCameraFrameType_Distorted:	return "distorted";
		case VRTrackedCameraFrameType_Undistorted: return "undistorted";
		case VRTrackedCameraFrameType_MaximumUndistorted: return "max_undistorted";
	}
	return "unknown";
}

template <typename visitor_fn>
static void visit_cameraframetype_schema(visitor_fn &visitor,
	vrstate::cameraframetype_schema *ss, TrackedCameraWrapper tcw,
	int device_index, 
	EVRTrackedCameraFrameType frame_type,
	AdditionalResourceKeys &config)
{
	visitor.start_group_node(FrameTypeToGroupName(frame_type),-1);

	scalar_result<CameraFrameSize_t, EVRTrackedCameraError> f;
	scalar_result<CameraFrameIntrinsics_t, EVRTrackedCameraError> intrinsics;
	scalar_result<HmdMatrix44_t, EVRTrackedCameraError> projection;
	scalar_result<VideoStreamTextureSize_t, EVRTrackedCameraError> video_texture_size;
	
	LEAF_SCALAR(frame_size, tcw.GetCameraFrameSize(device_index, frame_type, &f));
	LEAF_SCALAR(intrinsics, tcw.GetCameraIntrinsics(device_index, frame_type, &intrinsics));
	LEAF_SCALAR(projection, tcw.GetCameraProjection(device_index, frame_type, config.GetNearZ(), config.GetFarZ(), &projection));
	LEAF_SCALAR(video_texture_size, tcw.GetVideoStreamTextureSize(device_index, frame_type, &video_texture_size));

	visitor.end_group_node("cameraframetypes", device_index);
}

template <typename visitor_fn>
static void visit_per_controller_state(visitor_fn &visitor, 
		vrstate::trackedcamera_schema::controller_camera_schema *ss, TrackedCameraWrapper tcw, 
		int device_index, AdditionalResourceKeys &resource_keys, ALLOCATOR_DECL)
{
	visitor.start_group_node("controller", device_index);
	LEAF_SCALAR(has_camera, tcw.HasCamera(device_index));
	
	if (ss->cameraframetypes.size() < 3)
	{
		ss->cameraframetypes.reserve(3);
		ss->cameraframetypes.emplace_back(allocator);
		ss->cameraframetypes.emplace_back(allocator);
		ss->cameraframetypes.emplace_back(allocator);
	}
	START_VECTOR(cameraframetypes);
	for (int i = 0; i < (int)ss->cameraframetypes.size(); i++)
	{
		visit_cameraframetype_schema(visitor, &ss->cameraframetypes[i], tcw, device_index, (EVRTrackedCameraFrameType)i, resource_keys);
	}
	END_VECTOR(cameraframetypes);
	visitor.end_group_node("controller", device_index);
}

template <typename visitor_fn>
static void visit_trackedcamera_state(visitor_fn &visitor, 
									vrstate::trackedcamera_schema *ss, TrackedCameraWrapper tcw, 
									AdditionalResourceKeys &resource_keys,
									ALLOCATOR_DECL)
{
	visitor.start_group_node("camera", -1);
	ss->controllers.reserve(vr::k_unMaxTrackedDeviceCount);
	while (ss->controllers.size() < vr::k_unMaxTrackedDeviceCount)
	{
		ss->controllers.emplace_back(allocator);
	}

	START_VECTOR(controllers);
	for (int i = 0; i < (int)ss->controllers.size(); i++)
	{
		visit_per_controller_state(visitor, &ss->controllers[i], tcw, i, resource_keys, allocator);
	}
	END_VECTOR(controllers);

	visitor.end_group_node("camera", -1);
}

template <typename visitor_fn>
static void visit_per_resource(visitor_fn &visitor,
	vrstate::resources_schema *ss, ResourcesWrapper &wrap,
	int i, AdditionalResourceKeys &resource_keys,
	ALLOCATOR_DECL)
{
	visitor.start_group_node("resource", i);
	if (visitor.visit_openvr())
	{
		int fname_size;
		const char *fname = resource_keys.GetResourcesIndexer().get_filename_for_index(i, &fname_size);
		int dname_size;
		const char *dname = resource_keys.GetResourcesIndexer().get_directoryname_for_index(i, &dname_size);
		visitor.visit_node(ss->resources[i].resource_name.item, fname, fname_size);
		visitor.visit_node(ss->resources[i].resource_directory.item, dname, dname_size);

		vector_result<char> full_path(wrap.string_pool);
		wrap.GetFullPath(
			fname,
			dname,
			&full_path);

		visitor.visit_node(ss->resources[i].resource_full_path.item, full_path.s.buf(), full_path.count);

		uint8_t *data;
		uint32_t size = wrap.GetImageData(full_path.s.buf(), &data);
		visitor.visit_node(ss->resources[i].resource_data.item, data, size);
		wrap.FreeImageData(data);
	}
	else
	{
		visitor.visit_node(ss->resources[i].resource_full_path.item);
		visitor.visit_node(ss->resources[i].resource_data.item);
	}
	visitor.end_group_node("resource", i);
}

template <typename visitor_fn>
static void visit_resources_state(visitor_fn &visitor,
	vrstate::resources_schema *ss, ResourcesWrapper &wrap,
	AdditionalResourceKeys &resource_keys,
	ALLOCATOR_DECL)
{
	visitor.start_group_node("resources", -1);

	if ((int)ss->resources.size() < resource_keys.GetResourcesIndexer().get_num_resources())
	{
		ss->resources.reserve(resource_keys.GetResourcesIndexer().get_num_resources());
		while ((int)ss->resources.size() < resource_keys.GetResourcesIndexer().get_num_resources())
		{
			ss->resources.emplace_back(allocator);
		}
	}

	START_VECTOR(resources);
	for (int i = 0; i < (int)ss->resources.size(); i++)
	{
		visit_per_resource(visitor, ss, wrap, i, resource_keys, allocator);
	}
	END_VECTOR(resources);

	visitor.end_group_node("resources", -1);
}

static void encode_events(EncodeStream *stream, const std::forward_list<FrameNumberedEvent, ALLOCATOR_TYPE> &events)
{
	for (auto iter = events.begin(); iter != events.end(); iter++)
	{
		const FrameNumberedEvent &event = *iter;
		stream->memcpy_out_to_stream(&event, sizeof(event));
	}
}

static void encode_timestamps(EncodeStream *stream, std::forward_list<int64_t, ALLOCATOR_TYPE> &timestamps)
{
	for (auto iter = timestamps.begin(); iter != timestamps.end(); iter++)
	{
		uint64_t timestamp = *iter;
		stream->memcpy_out_to_stream(&timestamp, sizeof(timestamp));
	}
}

static void decode_events(EncodeStream *stream, std::forward_list<FrameNumberedEvent, ALLOCATOR_TYPE> &events, int num_events)
{
	for (int i = 0; i < num_events; i++)
	{
		FrameNumberedEvent event;
		stream->memcpy_from_stream(&event,sizeof(event));
		events.emplace_front(event);
	}
	events.reverse();
}

static void decode_timestamps(EncodeStream *stream, std::forward_list<int64_t, ALLOCATOR_TYPE> &timestamps, int num_timestamps)
{
	for (int i = 0; i < num_timestamps; i++)
	{
		uint64_t timestamp;
		stream->memcpy_from_stream(&timestamp, sizeof(timestamp));
		timestamps.emplace_front(timestamp);
	}
	timestamps.reverse();
}

// stuff recorded at time of save
struct save_summary
{
	int num_frames;
	char date_string[64];
};

//
// the root of all vr state
//
struct tracker
{
	slab *m_slab;
	slab_allocator<char> m_allocator;
	StringPool m_string_pool;
	int m_frame_number;
	std::chrono::time_point<std::chrono::steady_clock> start_time;

	std::mutex update_mutex;
	int blocking_update_calls;
	int non_blocking_update_calls;
	save_summary save_info;
	AdditionalResourceKeys additional_resource_keys;
	
	vrstate m_state;
	
	std::forward_list<FrameNumberedEvent, ALLOCATOR_TYPE>  m_events;
	std::forward_list<int64_t, ALLOCATOR_TYPE>  m_timestamps;

	tracker(slab *slab, const TrackerConfig &c)
		:
		m_slab(slab),
		m_allocator(slab),
		m_frame_number(0),
		blocking_update_calls(0),
		non_blocking_update_calls(0),
		additional_resource_keys(c, m_allocator),
		m_state(m_allocator),
		m_events(m_allocator),
		m_timestamps(m_allocator)
	{
		memset(&save_info, 0, sizeof(save_summary));
	}
};

template <typename visitor_fn>
static void traverse_history_graph_sequential(visitor_fn &visitor, tracker *outer_state,
	openvr_broker::open_vr_interfaces &interfaces)
{
	SystemWrapper			system_wrapper(			interfaces.sysi,	&outer_state->m_string_pool);
	ApplicationsWrapper		application_wrapper(	interfaces.appi,	&outer_state->m_string_pool);
	SettingsWrapper			settings_wrapper(		interfaces.seti,	&outer_state->m_string_pool);
	ChaperoneWrapper		chaperone_wrapper(		interfaces.chapi,	&outer_state->m_string_pool);
	ChaperoneSetupWrapper	chaperone_setup_wrapper(interfaces.chapsi,	&outer_state->m_string_pool);
	CompositorWrapper		compositor_wrapper(		interfaces.compi,	&outer_state->m_string_pool);
	OverlayWrapper			overlay_wrapper(		interfaces.ovi,		&outer_state->m_string_pool);
	RenderModelWrapper		rendermodel_wrapper(	interfaces.remi,	&outer_state->m_string_pool);
	ExtendedDisplayWrapper	extended_display_wrapper(interfaces.exdi,	&outer_state->m_string_pool);
	TrackedCameraWrapper	tracked_camera_wrapper(	interfaces.taci,	&outer_state->m_string_pool);
	ResourcesWrapper		resources_wrapper(		interfaces.resi,	&outer_state->m_string_pool);

	vrstate *s = &outer_state->m_state;
	ALLOCATOR_TYPE &allocator = outer_state->m_allocator;

#ifdef TIMERS_ENABLED	
	// reset at the start
	TWRAP_NUM_TIMERS = 0;
#endif

	{
		twrap t("system_node");
		visit_system_node(visitor, &s->system_node, interfaces.sysi, system_wrapper, rendermodel_wrapper, 
							outer_state->additional_resource_keys, allocator);
	}
	
	{
		twrap t("applications_node");
		visit_applications_node(visitor, &s->applications_node, application_wrapper, outer_state->additional_resource_keys, allocator);
	}
	
	{
		twrap t("settings_node");
		visit_settings_node(visitor, &s->settings_node, settings_wrapper);
	}

	{
		twrap t("chaperone_node");
		visit_chaperone_node(visitor, &s->chaperone_node, chaperone_wrapper, outer_state->additional_resource_keys);
	}

	{
		twrap t("chaperone_setup_node");
		visit_chaperone_setup_node(visitor, &s->chaperone_setup_node, chaperone_setup_wrapper);
	}

	{
		twrap t("compositor_node");
		visit_compositor_state(visitor, &s->compositor_node, compositor_wrapper, outer_state->additional_resource_keys, allocator);
	}

	{
		twrap t("overlay_node");
		visit_overlay_state(visitor, &s->overlay_node, overlay_wrapper, outer_state->additional_resource_keys, allocator);
	}
	
	{
		twrap t("rendermodels_node");
		visit_rendermodel_state(visitor, &s->rendermodels_node, rendermodel_wrapper, allocator);
	}

	{
		twrap t("extendeddisplay_node");
		visit_extended_display_state(visitor, &s->extendeddisplay_node, extended_display_wrapper);
	}
	
	{
		twrap t("trackedcamera_node");
		visit_trackedcamera_state(visitor, &s->trackedcamera_node, tracked_camera_wrapper, 
										outer_state->additional_resource_keys, allocator);
	}

	{
		twrap t("resources_node");
		visit_resources_state(visitor, &s->resources_node, resources_wrapper, outer_state->additional_resource_keys, allocator);
	}



#ifdef TIMERS_ENABLED
	static bool once;
	if (!once && TWRAP_num_samples[0] == TWRAP_MAX_SAMPLES)
	{
		once = true;
		for (int i = 0; i < TWRAP_NUM_TIMERS; i++)
		{
			dprintf("%s: ", TWRAP_timer_names[i]);
			for (int j = 0; j < TWRAP_MAX_SAMPLES; j++)
			{
				dprintf("%f, ", TWRAP_timers[i][j].getElapsedTimeInMilliSec());
			}
			dprintf("\n");
		}
		dprintf("\n");
	}
#endif
}

// wrapper function for when you want to traverse the "left sides" (the history nodes) 
// and not the right sides (the openvr current state).
template <typename visitor_fn>
static void traverse_history_graph_sequential(visitor_fn &visitor, tracker *s)
{
	openvr_broker::open_vr_interfaces null_interfaces;
	openvr_broker::acquire_interfaces("null", &null_interfaces, nullptr);
	traverse_history_graph_sequential(visitor, s, null_interfaces);
}

vr_state_tracker_t create_vr_state_tracker(TrackerConfig c)
{
	slab *the_slab = new slab(8 * 1024 * 1024);	// textures can be > 4MB 
	void *some_space_for_vrstate = the_slab->slab_alloc(sizeof(tracker));
	tracker *ret = new(some_space_for_vrstate)tracker(the_slab, c); // everyone is setup and pointing into the slab
	return ret;
}

void destroy_vr_state_tracker(vr_state_tracker_t t)
{
	tracker *s = static_cast<tracker*>(t);
	slab *slab = s->m_slab;
	s->~tracker(); // since I allocated the vrstate inside the slab - don't call delete - just call the destructor
	delete slab;
}

static void update_timestamp(tracker *s)
{
	std::chrono::time_point<std::chrono::steady_clock> cur_time;
	if (s->m_frame_number == 0)
	{
		s->start_time = std::chrono::steady_clock::now();
		cur_time = s->start_time;
	}
	else
	{
		cur_time = std::chrono::steady_clock::now();
	}
	using ns = std::chrono::duration<__int64, std::ratio<1, 1000000000>>;
	ns duration = cur_time - s->start_time;
	s->m_timestamps.emplace_front(duration.count());
	s->m_frame_number++;
}

// START OF CURSOR TEST
// START OF CURSOR TEST
// START OF CURSOR TEST
static int util_char_vector_cmp(const char *pch, std::vector<char, ALLOCATOR_TYPE> &v)
{
	if (v.size() == 0)
	{
		if (*pch == '\0')
			return 0;
		else
			return 1;
	}
	else
	{
		return strncmp(pch, &v[0], v.size());
	}
}



template <typename T>
static bool util_vector_to_return_buf_rc(
	std::vector<T, ALLOCATOR_TYPE> *p,
	T *pRet,
	uint32_t unBufferCount,
	uint32_t *rc)
{
	bool big_enough = false;
	uint32_t required_count = p->size();

	if (pRet && unBufferCount > 0 && required_count > 0)
	{
		uint32_t bytes_to_write = std::min(unBufferCount, required_count) * sizeof(T);
		memcpy(pRet, &p->at(0), bytes_to_write);
		if (unBufferCount >= required_count)
		{
			big_enough = true;
		}
	}

	if (rc)
	{
		*rc = required_count;
	}
	return big_enough;
}

bool util_char_to_return_buf_rc(const char *val, size_t required_size, char *pRet, uint32_t unBufferCount, uint32_t *rc)
{
	bool big_enough = true;
	
	if (pRet && unBufferCount > 0)
	{
		if (required_size == 0)
		{
			pRet[0] = 0;
		}
		else
		{
			uint32_t bytes_to_write = std::min(unBufferCount, required_size);
			memcpy(pRet, val, bytes_to_write);
			if (bytes_to_write < required_size)
			{
				big_enough = false;
				pRet[unBufferCount - 1] = 0;
			}
		}
	}
	else
	{
		big_enough = false;
	}

	if (rc)
	{
		*rc = required_size;
	}
	return big_enough;
}

// specialized for char to terminate the buffer
template <>
static bool util_vector_to_return_buf_rc<char>(
	std::vector<char, ALLOCATOR_TYPE> *p,
	char *pRet,
	uint32_t unBufferCount,
	uint32_t *rc)
{
	uint32_t required_size = p->size(); // size to hold the string including the trailing null
										// note that p is not an 'std::string' its a vector and it include the trailing null

	assert(p->size() == 0 || p->at(p->size() - 1) == 0); // (proof that p always has the trailing null)

	char *ptr = nullptr;
	if (p->size() > 0)
	{
		ptr = &p->at(0);
	}

	return util_char_to_return_buf_rc(ptr, p->size(), pRet, unBufferCount, rc);
}

struct CursorContext
{
	CursorContext(
		int my_current_frame,
		vriterator *my_iterators,
		vrstate *my_state,
		AdditionalResourceKeys *tracking_set,
		ALLOCATOR_DECL)
		:
		current_frame(my_current_frame),
		iterators(my_iterators),
		state(my_state),
		m_tracking_set(tracking_set),
		m_allocator(allocator)
	{}

	int current_frame;
	vriterator *iterators;
	vrstate *state;
	AdditionalResourceKeys *m_tracking_set;
	ALLOCATOR_TYPE m_allocator;
};

template <typename T, typename U>
static void update_iter(T& cached_iterator, U &history_node, int cursor_frame)
{
	static_assert(
		std::is_base_of<HistoryIterator, T>::value,
		"T must be a descendant of HistoryIterator"
		);

	static_assert(
		std::is_base_of<HistoryNode, U>::value,
		"U must be a descendant of HistoryNode"
		);

	auto &iter = cached_iterator.item;
	auto &node = history_node.item;


	// update cache when my cached frame id is not the current cursor frame
	if (cached_iterator.iter_frame != cursor_frame)
	{
		for (iter = node.values.begin(); iter != node.values.end(); iter++)
		{
			if (iter->frame_number <= cursor_frame)
			{
				// found the value for this frame
				break;
			}
		}
		cached_iterator.iter_frame = cursor_frame;
		cached_iterator.item = iter;
	}
}

template <typename T, typename PropertyIDT, typename PropertyErrorT>
static bool lookup_subtable_property(
	vrstate::properties_subtable<T, PropertyIDT, PropertyErrorT> &subtable,
	vriterator::properties_subtable<T, PropertyIDT, PropertyErrorT> &subtable_iter,
	int current_frame,
	PropertyIDT prop_enum,
	T **ret,
	PropertyErrorT * pError)
{
	int index = subtable.get_index_for_property(prop_enum);
	if (index >= 0)
	{
		update_iter(subtable_iter.props[index], subtable.props[index], current_frame);
		*ret = &subtable_iter.props[index].item->val;

		if (pError)
		{
			*pError = subtable_iter.props[index].item->presence;
		}

		return subtable_iter.props[index].item->is_present();
	}
	return false;
}

class VRSystemCursor : public VRSystemCppStub
{
public:
	void GetRecommendedRenderTargetSize(uint32_t * pnWidth, uint32_t * pnHeight) override;
	struct vr::HmdMatrix44_t GetProjectionMatrix(vr::EVREye eEye, float fNearZ, float fFarZ) override;
	void GetProjectionRaw(vr::EVREye eEye, float * pfLeft, float * pfRight, float * pfTop, float * pfBottom) override;
	bool ComputeDistortion(vr::EVREye eEye, float fU, float fV, struct vr::DistortionCoordinates_t * pDistortionCoordinates) override;
	struct vr::HmdMatrix34_t GetEyeToHeadTransform(vr::EVREye eEye) override;
	bool GetTimeSinceLastVsync(float * pfSecondsSinceLastVsync, uint64_t * pulFrameCounter) override;
	int32_t GetD3D9AdapterIndex() override;
	void GetDXGIOutputInfo(int32_t * pnAdapterIndex) override;
	bool IsDisplayOnDesktop() override;
	void GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin eOrigin, float fPredictedSecondsToPhotonsFromNow, struct vr::TrackedDevicePose_t * pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount) override;
	struct vr::HmdMatrix34_t GetSeatedZeroPoseToStandingAbsoluteTrackingPose() override;
	struct vr::HmdMatrix34_t GetRawZeroPoseToStandingAbsoluteTrackingPose() override;
	uint32_t GetSortedTrackedDeviceIndicesOfClass(vr::ETrackedDeviceClass eTrackedDeviceClass, vr::TrackedDeviceIndex_t * punTrackedDeviceIndexArray, uint32_t unTrackedDeviceIndexArrayCount, vr::TrackedDeviceIndex_t unRelativeToTrackedDeviceIndex) override;
	vr::EDeviceActivityLevel GetTrackedDeviceActivityLevel(vr::TrackedDeviceIndex_t unDeviceId) override;
	void ApplyTransform(struct vr::TrackedDevicePose_t * pOutputPose, const struct vr::TrackedDevicePose_t * pTrackedDevicePose, const struct vr::HmdMatrix34_t * pTransform) override;
	vr::TrackedDeviceIndex_t GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole unDeviceType) override;
	vr::ETrackedControllerRole GetControllerRoleForTrackedDeviceIndex(vr::TrackedDeviceIndex_t unDeviceIndex) override;
	vr::ETrackedDeviceClass GetTrackedDeviceClass(vr::TrackedDeviceIndex_t unDeviceIndex) override;
	bool IsTrackedDeviceConnected(vr::TrackedDeviceIndex_t unDeviceIndex) override;
	bool GetBoolTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError) override;
	float GetFloatTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError) override;
	int32_t GetInt32TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError) override;
	uint64_t GetUint64TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError) override;
	struct vr::HmdMatrix34_t GetMatrix34TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError) override;
	uint32_t GetStringTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, vr::ETrackedDeviceProperty prop, char * pchValue, uint32_t unBufferSize, vr::ETrackedPropertyError * pError) override;
	const char * GetPropErrorNameFromEnum(vr::ETrackedPropertyError error) override;
	bool PollNextEvent(struct vr::VREvent_t * pEvent, uint32_t uncbVREvent) override;
	bool PollNextEventWithPose(vr::ETrackingUniverseOrigin eOrigin, struct vr::VREvent_t * pEvent, uint32_t uncbVREvent, vr::TrackedDevicePose_t * pTrackedDevicePose) override;
	const char * GetEventTypeNameFromEnum(vr::EVREventType eType) override;
	struct vr::HiddenAreaMesh_t GetHiddenAreaMesh(vr::EVREye eEye, vr::EHiddenAreaMeshType type) override;
	bool GetControllerState(vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t * pControllerState, uint32_t unControllerStateSize) override;
	bool GetControllerStateWithPose(vr::ETrackingUniverseOrigin eOrigin, vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t * pControllerState, uint32_t unControllerStateSize, struct vr::TrackedDevicePose_t * pTrackedDevicePose) override;
	const char * GetButtonIdNameFromEnum(vr::EVRButtonId eButtonId) override;
	const char * GetControllerAxisTypeNameFromEnum(vr::EVRControllerAxisType eAxisType) override;
	bool IsInputFocusCapturedByAnotherProcess() override;
	
	CursorContext *m_context;
	vrstate::system_schema &state_ref;
	vriterator::system_schema &iter_ref;

	VRSystemCursor(CursorContext *context)
		:
		m_context(context),
		state_ref(m_context->state->system_node),
		iter_ref(m_context->iterators->system_node)
	{

		// synchronize eyes, controllers/components
		for (int eyes_index = 0; eyes_index < (int)state_ref.eyes.size(); eyes_index++)
		{
			iter_ref.eyes.emplace_back(m_context->m_allocator);
			for (int meshes_index = 0;
				meshes_index < (int)state_ref.eyes[eyes_index].hidden_meshes.size();
				meshes_index++)
			{
				iter_ref.eyes[eyes_index].hidden_meshes.emplace_back(m_context->m_allocator);
			}
		}

		for (int controllers_index = 0; controllers_index < (int)state_ref.controllers.size(); controllers_index++)
		{
			iter_ref.controllers.emplace_back(m_context->m_allocator);
			for (int components_index = 0;
				components_index < (int)state_ref.controllers[controllers_index].components.size();
				components_index++)
			{
				iter_ref.controllers[controllers_index].components.emplace_back(m_context->m_allocator);
			}
		}

		for (int spatial_sorts_index = 0; spatial_sorts_index < (int)state_ref.spatial_sorts.size(); spatial_sorts_index++)
		{
			iter_ref.spatial_sorts.emplace_back(m_context->m_allocator);
		}
	}

	bool VRSystemCursor::IsValidDeviceIndex(vr::TrackedDeviceIndex_t unDeviceIndex);
};

bool VRSystemCursor::IsValidDeviceIndex(vr::TrackedDeviceIndex_t unDeviceIndex)
{
	if (unDeviceIndex >= 0 && unDeviceIndex <= m_context->state->system_node.controllers.size())
		return true;
	else
		return false;
}

#define SYNC_SYSTEM_STATE(local_name, variable_name) \
auto local_name ## iter = iter_ref.variable_name;\
update_iter(local_name ## iter,\
	state_ref.variable_name,\
	m_context->current_frame);\
auto & local_name = local_name ## iter ## .item;

void VRSystemCursor::GetRecommendedRenderTargetSize(uint32_t * pnWidth, uint32_t * pnHeight)
{
	LOG_ENTRY("CppStubGetRecommendedRenderTargetSize");

	SYNC_SYSTEM_STATE(size, recommended_target_size);

	*pnWidth	= size->val.width;
	*pnHeight	= size->val.height;

	LOG_EXIT("CppStubGetRecommendedRenderTargetSize");
}

struct vr::HmdMatrix44_t VRSystemCursor::GetProjectionMatrix(vr::EVREye eEye, float fNearZ, float fFarZ)
{
	LOG_ENTRY("CppStubGetProjectionMatrix");
	
	SYNC_SYSTEM_STATE(proj, eyes[int(eEye)].projection);

	LOG_EXIT_RC(proj->val, "CppStubGetProjectionMatrix");
}

void VRSystemCursor::GetProjectionRaw(vr::EVREye eEye, float * pfLeft, float * pfRight, float * pfTop, float * pfBottom)
{
	LOG_ENTRY("CppStubGetProjectionRaw");

	SYNC_SYSTEM_STATE(v4, eyes[int(eEye)].projection_raw);

	*pfLeft = v4->val.v[0];
	*pfRight = v4->val.v[1];
	*pfTop = v4->val.v[2];
	*pfBottom = v4->val.v[3];

	LOG_EXIT("CppStubGetProjectionRaw");
}

bool VRSystemCursor::ComputeDistortion(vr::EVREye eEye, float fU, float fV, struct vr::DistortionCoordinates_t * pDistortionCoordinates)
{
	LOG_ENTRY("CppStubComputeDistortion");
	
	auto iter = m_context->iterators->system_node.eyes[int(eEye)].distortion;
	update_iter(iter, m_context->state->system_node.eyes[int(eEye)].distortion, m_context->current_frame);

	if (iter.item->is_present() && pDistortionCoordinates)
	{
		*pDistortionCoordinates = iter.item->val;
	}

	LOG_EXIT_RC(iter.item->presence, "CppStubComputeDistortion");
}

struct vr::HmdMatrix34_t VRSystemCursor::GetEyeToHeadTransform(vr::EVREye eEye)
{
	LOG_ENTRY("CppStubGetEyeToHeadTransform");
	SYNC_SYSTEM_STATE(tx, eyes[int(eEye)].eye2head);
	LOG_EXIT_RC(tx->val, "CppStubGetEyeToHeadTransform");
}

bool VRSystemCursor::GetTimeSinceLastVsync(float * pfSecondsSinceLastVsync, uint64_t * pulFrameCounter)
{
	LOG_ENTRY("CppStubGetTimeSinceLastVsync");

	auto iter1 = m_context->iterators->system_node.seconds_since_last_vsync;
	update_iter(iter1, m_context->state->system_node.seconds_since_last_vsync, m_context->current_frame);

	auto iter2 = m_context->iterators->system_node.frame_counter_since_last_vsync;
	update_iter(iter2, m_context->state->system_node.frame_counter_since_last_vsync, m_context->current_frame);

	if (iter1.item->is_present())
	{
		if (pfSecondsSinceLastVsync)
		{
			*pfSecondsSinceLastVsync = iter1.item->val;
		}
		if (pulFrameCounter)
		{
			*pulFrameCounter = iter2.item->val;
		}
	}

	LOG_EXIT_RC(iter1.item->presence, "CppStubGetTimeSinceLastVsync");
}

int32_t VRSystemCursor::GetD3D9AdapterIndex()
{
	LOG_ENTRY("CppStubGetD3D9AdapterIndex");
	SYNC_SYSTEM_STATE(d3d9_adapter_index, d3d9_adapter_index);
	LOG_EXIT_RC(d3d9_adapter_index->val, "CppStubGetD3D9AdapterIndex");
}

void VRSystemCursor::GetDXGIOutputInfo(int32_t * pnAdapterIndex)
{
	LOG_ENTRY("CppStubGetDXGIOutputInfo");
	SYNC_SYSTEM_STATE(dxgi_output_info, dxgi_output_info);
	if (pnAdapterIndex)
	{
		*pnAdapterIndex = dxgi_output_info->val;
	}
	LOG_EXIT("CppStubGetDXGIOutputInfo");
}

bool VRSystemCursor::IsDisplayOnDesktop()
{
	LOG_ENTRY("CppStubIsDisplayOnDesktop");
	SYNC_SYSTEM_STATE(is_display_on_desktop, is_display_on_desktop);
	LOG_EXIT_RC(is_display_on_desktop->val, "CppStubIsDisplayOnDesktop");
}

void VRSystemCursor::GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin eOrigin, 
	float fPredictedSecondsToPhotonsFromNow, 
	struct vr::TrackedDevicePose_t * pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount)
{
	LOG_ENTRY("CppStubGetDeviceToAbsoluteTrackingPose");

	assert(fPredictedSecondsToPhotonsFromNow == 0);

	unTrackedDevicePoseArrayCount = std::max(unTrackedDevicePoseArrayCount, vr::k_unMaxTrackedDeviceCount);
	unTrackedDevicePoseArrayCount = std::max(unTrackedDevicePoseArrayCount, m_context->state->system_node.controllers.size());

	if (eOrigin == vr::TrackingUniverseSeated)
	{
		for (int i = 0; i < (int)unTrackedDevicePoseArrayCount; i++)
		{
			SYNC_SYSTEM_STATE(seated_tracking_pose, controllers[i].seated_tracking_pose);
			pTrackedDevicePoseArray[i] = seated_tracking_pose->val;
		}
	}
	else if (eOrigin == vr::TrackingUniverseStanding)
	{
		for (int i = 0; i < (int)unTrackedDevicePoseArrayCount; i++)
		{
			SYNC_SYSTEM_STATE(standing_tracking_pose, controllers[i].standing_tracking_pose);
			pTrackedDevicePoseArray[i] = standing_tracking_pose->val;
		}
	}
	else
	{
		for (int i = 0; i < (int)unTrackedDevicePoseArrayCount; i++)
		{
			SYNC_SYSTEM_STATE(raw_tracking_pose, controllers[i].raw_tracking_pose);
			pTrackedDevicePoseArray[i] = raw_tracking_pose->val;
		}
	}
	
	LOG_EXIT("CppStubGetDeviceToAbsoluteTrackingPose");
}

struct vr::HmdMatrix34_t VRSystemCursor::GetSeatedZeroPoseToStandingAbsoluteTrackingPose()
{
	LOG_ENTRY("CppStubGetSeatedZeroPoseToStandingAbsoluteTrackingPose");
	SYNC_SYSTEM_STATE(seated2standing, seated2standing)
	LOG_EXIT_RC(seated2standing->val, "CppStubGetSeatedZeroPoseToStandingAbsoluteTrackingPose");
}

struct vr::HmdMatrix34_t VRSystemCursor::GetRawZeroPoseToStandingAbsoluteTrackingPose()
{
	LOG_ENTRY("CppStubGetRawZeroPoseToStandingAbsoluteTrackingPose");
	SYNC_SYSTEM_STATE(raw2standing, raw2standing);
	LOG_EXIT_RC(raw2standing->val, "CppStubGetRawZeroPoseToStandingAbsoluteTrackingPose");
}

uint32_t VRSystemCursor::GetSortedTrackedDeviceIndicesOfClass(
		vr::ETrackedDeviceClass eTrackedDeviceClass, 
		vr::TrackedDeviceIndex_t * punTrackedDeviceIndexArray, 
		uint32_t unTrackedDeviceIndexArrayCount, 
		vr::TrackedDeviceIndex_t unRelativeToTrackedDeviceIndex)
{
	LOG_ENTRY("CppStubGetSortedTrackedDeviceIndicesOfClass");

	unsigned int spatial_sort_index = unRelativeToTrackedDeviceIndex + 1;

	TrackedDeviceIndex_t *indices = nullptr;
	uint32_t count = 0;

	switch (eTrackedDeviceClass)
	{
	case TrackedDeviceClass_Invalid:
		return 0;
		break;
	case TrackedDeviceClass_HMD:
	{
		SYNC_SYSTEM_STATE(hmds_sorted, spatial_sorts[spatial_sort_index].hmds_sorted);
		util_vector_to_return_buf_rc(&hmds_sorted->val, punTrackedDeviceIndexArray, unTrackedDeviceIndexArrayCount, &count);
	}
		break;

	case TrackedDeviceClass_Controller:
	{
		SYNC_SYSTEM_STATE(controllers_sorted, spatial_sorts[spatial_sort_index].controllers_sorted);
		util_vector_to_return_buf_rc(&controllers_sorted->val, punTrackedDeviceIndexArray, unTrackedDeviceIndexArrayCount, &count);
	}
		break;

	case TrackedDeviceClass_GenericTracker:
	{
		SYNC_SYSTEM_STATE(trackers_sorted, spatial_sorts[spatial_sort_index].trackers_sorted);
		util_vector_to_return_buf_rc(&trackers_sorted->val, punTrackedDeviceIndexArray, unTrackedDeviceIndexArrayCount, &count);
	}
		break;

	case TrackedDeviceClass_TrackingReference:
	{
		SYNC_SYSTEM_STATE(reference_sorted, spatial_sorts[spatial_sort_index].reference_sorted);
		util_vector_to_return_buf_rc(&reference_sorted->val, punTrackedDeviceIndexArray, unTrackedDeviceIndexArrayCount, &count);
	}
		break;
	default:
		assert(0);
	}

	LOG_EXIT_RC(count, "CppStubGetSortedTrackedDeviceIndicesOfClass");
}

vr::EDeviceActivityLevel VRSystemCursor::GetTrackedDeviceActivityLevel(vr::TrackedDeviceIndex_t unDeviceId)
{
	LOG_ENTRY("CppStubGetTrackedDeviceActivityLevel");
	SYNC_SYSTEM_STATE(activity_level, controllers[unDeviceId].activity_level);
	LOG_EXIT_RC(activity_level->val, "CppStubGetTrackedDeviceActivityLevel");
}

void VRSystemCursor::ApplyTransform(
	struct vr::TrackedDevicePose_t * pOutputPose, 
	const struct vr::TrackedDevicePose_t * pTrackedDevicePose, 
	const struct vr::HmdMatrix34_t * pTransform)
{
	LOG_ENTRY("CppStubApplyTransform");
	assert(0); // todo 
	LOG_EXIT("CppStubApplyTransform");
}

vr::TrackedDeviceIndex_t VRSystemCursor::GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole unDeviceType)
{
	LOG_ENTRY("CppStubGetTrackedDeviceIndexForControllerRole");
	
	vr::TrackedDeviceIndex_t rc = k_unTrackedDeviceIndexInvalid;
	for (unsigned int i = 0; i < m_context->state->system_node.controllers.size(); i++)
	{
		auto iter1 = m_context->iterators->system_node.controllers[i].controller_role;
		update_iter(iter1, m_context->state->system_node.controllers[i].controller_role, m_context->current_frame);
		if (iter1.item->val == unDeviceType)
		{
			rc = i;
			break;
		}
	}
	LOG_EXIT_RC(rc, "CppStubGetTrackedDeviceIndexForControllerRole");
}

vr::ETrackedControllerRole VRSystemCursor::GetControllerRoleForTrackedDeviceIndex(vr::TrackedDeviceIndex_t unDeviceIndex)
{
	LOG_ENTRY("CppStubGetControllerRoleForTrackedDeviceIndex");
	vr::ETrackedControllerRole rc = TrackedControllerRole_Invalid;
	if (IsValidDeviceIndex(unDeviceIndex))
	{
		auto iter1 = m_context->iterators->system_node.controllers[unDeviceIndex].controller_role;
		update_iter(iter1, m_context->state->system_node.controllers[unDeviceIndex].controller_role, m_context->current_frame);
		rc = iter1.item->val;
	}
	LOG_EXIT_RC(rc, "CppStubGetControllerRoleForTrackedDeviceIndex");
}

vr::ETrackedDeviceClass VRSystemCursor::GetTrackedDeviceClass(vr::TrackedDeviceIndex_t unDeviceIndex)
{
	LOG_ENTRY("CppStubGetTrackedDeviceClass");
	vr::ETrackedDeviceClass rc = TrackedDeviceClass_Invalid;
	if (IsValidDeviceIndex(unDeviceIndex))
	{
		SYNC_SYSTEM_STATE(device_class, controllers[unDeviceIndex].device_class);
		rc = device_class->val;
	}
	LOG_EXIT_RC(rc, "CppStubGetTrackedDeviceClass");
}

bool VRSystemCursor::IsTrackedDeviceConnected(vr::TrackedDeviceIndex_t unDeviceIndex)
{
	LOG_ENTRY("CppStubIsTrackedDeviceConnected");
	bool rc = false;
	if (IsValidDeviceIndex(unDeviceIndex))
	{
		SYNC_SYSTEM_STATE(connected, controllers[unDeviceIndex].connected);
		rc = connected->val;
	}
	LOG_EXIT_RC(rc, "CppStubIsTrackedDeviceConnected");
}

bool VRSystemCursor::GetBoolTrackedDeviceProperty(
				vr::TrackedDeviceIndex_t unDeviceIndex, 
				vr::ETrackedDeviceProperty prop_enum, 
				vr::ETrackedPropertyError * pError)
{
	LOG_ENTRY("CppStubGetBoolTrackedDeviceProperty");
	bool rc = false;
	if (IsValidDeviceIndex(unDeviceIndex))
	{
		bool *p;
		lookup_subtable_property(
			m_context->state->system_node.controllers[unDeviceIndex].bool_props, 
			m_context->iterators->system_node.controllers[unDeviceIndex].bool_props,
			m_context->current_frame,
			prop_enum, &p, pError);
		rc = *p;
	}
	LOG_EXIT_RC(rc, "CppStubGetBoolTrackedDeviceProperty");
}

float VRSystemCursor::GetFloatTrackedDeviceProperty(
				vr::TrackedDeviceIndex_t unDeviceIndex, 
				vr::ETrackedDeviceProperty prop_enum, 
				vr::ETrackedPropertyError * pError)
{
	LOG_ENTRY("CppStubGetFloatTrackedDeviceProperty");

	float rc = 0.0f;
	if (IsValidDeviceIndex(unDeviceIndex))
	{
		float *p;
		lookup_subtable_property(
			m_context->state->system_node.controllers[unDeviceIndex].float_props,
			m_context->iterators->system_node.controllers[unDeviceIndex].float_props,
			m_context->current_frame,
			prop_enum, &p, pError);
		rc = *p;
	}
	
	LOG_EXIT_RC(rc, "CppStubGetFloatTrackedDeviceProperty");
}

int32_t VRSystemCursor::GetInt32TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, 
					vr::ETrackedDeviceProperty prop_enum, vr::ETrackedPropertyError * pError)
{
	LOG_ENTRY("CppStubGetInt32TrackedDeviceProperty");

	int32_t rc = 0;
	if (IsValidDeviceIndex(unDeviceIndex))
	{
		int32_t *p;
		lookup_subtable_property(
			m_context->state->system_node.controllers[unDeviceIndex].int32_props,
			m_context->iterators->system_node.controllers[unDeviceIndex].int32_props,
			m_context->current_frame,
			prop_enum, &p, pError);
		rc = *p;
	}
	
	LOG_EXIT_RC(rc, "CppStubGetInt32TrackedDeviceProperty");
}

uint64_t VRSystemCursor::GetUint64TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, 
				vr::ETrackedDeviceProperty prop_enum, vr::ETrackedPropertyError * pError)
{
	LOG_ENTRY("CppStubGetUint64TrackedDeviceProperty");
	
	uint64_t rc = 0;
	if (IsValidDeviceIndex(unDeviceIndex))
	{
		uint64_t *p;
		lookup_subtable_property(
			m_context->state->system_node.controllers[unDeviceIndex].uint64_props,
			m_context->iterators->system_node.controllers[unDeviceIndex].uint64_props,
			m_context->current_frame,
			prop_enum, &p, pError);
		rc = *p;
	}

	LOG_EXIT_RC(rc, "CppStubGetUint64TrackedDeviceProperty");
}

struct vr::HmdMatrix34_t VRSystemCursor::GetMatrix34TrackedDeviceProperty(
				vr::TrackedDeviceIndex_t unDeviceIndex, 
				vr::ETrackedDeviceProperty prop_enum, vr::ETrackedPropertyError * pError)
{
	LOG_ENTRY("CppStubGetMatrix34TrackedDeviceProperty");
	
	vr::HmdMatrix34_t rc;
	if (IsValidDeviceIndex(unDeviceIndex))
	{
		vr::HmdMatrix34_t *p;
		lookup_subtable_property(
			m_context->state->system_node.controllers[unDeviceIndex].mat34_props,
			m_context->iterators->system_node.controllers[unDeviceIndex].mat34_props,
			m_context->current_frame,
			prop_enum, &p, pError);
		rc = *p;
	}
	else
	{
		memset(&rc, 0, sizeof(rc));
	}
	
	LOG_EXIT_RC(rc, "CppStubGetMatrix34TrackedDeviceProperty");
}

bool GetIndexForMimeType(const char *pchMimeType, int *index)
{
	for (int i = 0; i < mime_tbl_size; i++)
	{
		if (strcmp(mime_types[i].name, k_pch_MimeType_HomeApp) == 0)
		{
			*index = i;
			return true;
		}
	}
	return false;
}

uint32_t VRSystemCursor::GetStringTrackedDeviceProperty(
		vr::TrackedDeviceIndex_t unDeviceIndex, 
		vr::ETrackedDeviceProperty prop_enum, 
		char * pchValue, uint32_t unBufferSize, vr::ETrackedPropertyError * pError)
{
	LOG_ENTRY("CppStubGetStringTrackedDeviceProperty");
	
	uint32_t rc = 0;
	if (IsValidDeviceIndex(unDeviceIndex))
	{
		std::vector<char, ALLOCATOR_TYPE> *p;
		if (lookup_subtable_property(
			m_context->state->system_node.controllers[unDeviceIndex].string_props,
			m_context->iterators->system_node.controllers[unDeviceIndex].string_props,
			m_context->current_frame,
			prop_enum, &p, pError))
		{
			util_vector_to_return_buf_rc(p, pchValue, unBufferSize, &rc);
		}
	}
	
	LOG_EXIT_RC(rc, "CppStubGetStringTrackedDeviceProperty");
}

const char * VRSystemCursor::GetPropErrorNameFromEnum(vr::ETrackedPropertyError error)
{
	LOG_ENTRY("CppStubGetPropErrorNameFromEnum");
	const char * rc = openvr_string::ETrackedPropertyErrorToString(error);
	LOG_EXIT_RC(rc, "CppStubGetPropErrorNameFromEnum");
}

bool VRSystemCursor::PollNextEvent(struct vr::VREvent_t * pEvent, uint32_t uncbVREvent)
{
	LOG_ENTRY("CppStubPollNextEvent");
	assert(0); // todo to do after I've figured out read vs write interfaces
	static bool rc = false;
	LOG_EXIT_RC(rc, "CppStubPollNextEvent");
}

bool VRSystemCursor::PollNextEventWithPose(vr::ETrackingUniverseOrigin eOrigin, struct vr::VREvent_t * pEvent, uint32_t uncbVREvent, vr::TrackedDevicePose_t * pTrackedDevicePose)
{
	LOG_ENTRY("CppStubPollNextEventWithPose");
	assert(0); // todo to do after I've figured out read vs write interfaces
	static bool rc = true;
	LOG_EXIT_RC(rc, "CppStubPollNextEventWithPose");
}

const char * VRSystemCursor::GetEventTypeNameFromEnum(vr::EVREventType eType)
{
	LOG_ENTRY("CppStubGetEventTypeNameFromEnum");
	const char * rc = openvr_string::EVREventTypeToString(eType);
	LOG_EXIT_RC(rc, "CppStubGetEventTypeNameFromEnum");
}

struct vr::HiddenAreaMesh_t VRSystemCursor::GetHiddenAreaMesh(vr::EVREye eEye, vr::EHiddenAreaMeshType type)
{
	LOG_ENTRY("CppStubGetHiddenAreaMesh");

	SYNC_SYSTEM_STATE(vertices, eyes[int(eEye)].hidden_meshes[(int)type].hidden_mesh_vertices);
	SYNC_SYSTEM_STATE(triangle_count, eyes[int(eEye)].hidden_meshes[(int)type].hidden_mesh_triangle_count);
	
	vr::HiddenAreaMesh_t ret;
	ret.pVertexData = &vertices->val[0];
	ret.unTriangleCount = triangle_count->val;

	LOG_EXIT_RC(ret, "CppStubGetHiddenAreaMesh");
}

bool VRSystemCursor::GetControllerState(
	vr::TrackedDeviceIndex_t unControllerDeviceIndex, 
	vr::VRControllerState_t * pControllerState, uint32_t unControllerStateSize)
{
	LOG_ENTRY("CppStubGetControllerState");

	bool rc = false;
	if (IsValidDeviceIndex(unControllerDeviceIndex) && pControllerState)
	{
		SYNC_SYSTEM_STATE(controller_state, controllers[unControllerDeviceIndex].controller_state);
		*pControllerState = controller_state->val;
		rc = true;
	}
	
	LOG_EXIT_RC(rc, "CppStubGetControllerState");
}

bool VRSystemCursor::GetControllerStateWithPose(
	vr::ETrackingUniverseOrigin eOrigin, 
	vr::TrackedDeviceIndex_t unControllerDeviceIndex, 
	vr::VRControllerState_t * pControllerState, uint32_t unControllerStateSize, 
	struct vr::TrackedDevicePose_t * pTrackedDevicePose)
{
	LOG_ENTRY("CppStubGetControllerStateWithPose");

	bool rc = false;
	if (IsValidDeviceIndex(unControllerDeviceIndex) && pControllerState)
	{
		SYNC_SYSTEM_STATE(controller_state, controllers[unControllerDeviceIndex].controller_state);
		if (controller_state->is_present())
		{
			rc = true;
			if (pControllerState)
			{
				*pControllerState = controller_state->val;
			}
		}
		
		if (rc == true && pTrackedDevicePose)
		{
			switch(eOrigin)
			{
				case TrackingUniverseSeated:
				{
					SYNC_SYSTEM_STATE(pose, controllers[unControllerDeviceIndex].synced_seated_pose);
					if (pose->is_present())
					{
						*pTrackedDevicePose = pose->val;
					}
					else
					{
						rc = false;
					}
					break;
				}
				case TrackingUniverseStanding:
				{
					SYNC_SYSTEM_STATE(pose, controllers[unControllerDeviceIndex].synced_standing_pose);
					if (pose->is_present())
					{
						*pTrackedDevicePose = pose->val;
					}
					else
					{
						rc = false;
					}
					break;
				}
				case TrackingUniverseRawAndUncalibrated:
				{
					SYNC_SYSTEM_STATE(pose, controllers[unControllerDeviceIndex].synced_raw_pose);
					if (pose->is_present())
					{
						*pTrackedDevicePose = pose->val;
					}
					else
					{
						rc = false;
					}
					
					break;
				}
			}
		}
	}
	
	LOG_EXIT_RC(rc, "CppStubGetControllerStateWithPose");
}

const char * VRSystemCursor::GetButtonIdNameFromEnum(vr::EVRButtonId eButtonId)
{
	LOG_ENTRY("CppStubGetButtonIdNameFromEnum");
	const char * rc = openvr_string::EVRButtonIdToString(eButtonId);
	LOG_EXIT_RC(rc, "CppStubGetButtonIdNameFromEnum");
}

const char * VRSystemCursor::GetControllerAxisTypeNameFromEnum(vr::EVRControllerAxisType eAxisType)
{
	LOG_ENTRY("CppStubGetControllerAxisTypeNameFromEnum");
	const char * rc = openvr_string::EVRControllerAxisTypeToString(eAxisType);
	LOG_EXIT_RC(rc, "CppStubGetControllerAxisTypeNameFromEnum");
}

bool VRSystemCursor::IsInputFocusCapturedByAnotherProcess()
{
	LOG_ENTRY("CppStubIsInputFocusCapturedByAnotherProcess");
	SYNC_SYSTEM_STATE(input_focus_captured_by_other, input_focus_captured_by_other);
	LOG_EXIT_RC(input_focus_captured_by_other->val, "CppStubIsInputFocusCapturedByAnotherProcess");
}

struct VRApplicationsCursor : public VRApplicationsCppStub
{

	CursorContext *m_context;
	vrstate::applications_schema &state_ref;
	vriterator::applications_schema &iter_ref;

	VRApplicationsCursor(CursorContext *context)
		:
		m_context(context),
		state_ref(m_context->state->applications_node),
		iter_ref(m_context->iterators->applications_node)
	{
		SynchronizeChildVectors();
	}

	void SynchronizeChildVectors()
	{
		while (iter_ref.applications.size() < state_ref.applications.size())
		{
			iter_ref.applications.emplace_back(m_context->m_allocator);
		}
		
		while (iter_ref.mime_types.size() < state_ref.mime_types.size())
		{
			iter_ref.mime_types.emplace_back(m_context->m_allocator);
		}
	}
	
	bool IsApplicationInstalled(const char * pchAppKey) override;
	uint32_t GetApplicationCount() override;
	vr::EVRApplicationError GetApplicationKeyByIndex(uint32_t unApplicationIndex, char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen) override;
	vr::EVRApplicationError GetApplicationKeyByProcessId(uint32_t unProcessId, char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen) override;
	uint32_t GetApplicationProcessId(const char * pchAppKey) override;
	const char * GetApplicationsErrorNameFromEnum(vr::EVRApplicationError error) override;
	uint32_t GetApplicationPropertyString(const char * pchAppKey, vr::EVRApplicationProperty eProperty, char * pchPropertyValueBuffer, uint32_t unPropertyValueBufferLen, vr::EVRApplicationError * peError) override;
	bool GetApplicationPropertyBool(const char * pchAppKey, vr::EVRApplicationProperty eProperty, vr::EVRApplicationError * peError) override;
	uint64_t GetApplicationPropertyUint64(const char * pchAppKey, vr::EVRApplicationProperty eProperty, vr::EVRApplicationError * peError) override;
	bool GetApplicationAutoLaunch(const char * pchAppKey) override;
	bool GetDefaultApplicationForMimeType(const char * pchMimeType, char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen) override;
	bool GetApplicationSupportedMimeTypes(const char * pchAppKey, char * pchMimeTypesBuffer, uint32_t unMimeTypesBuffer) override;
	uint32_t GetApplicationsThatSupportMimeType(const char * pchMimeType, char * pchAppKeysThatSupportBuffer, uint32_t unAppKeysThatSupportBuffer) override;
	uint32_t GetApplicationLaunchArguments(uint32_t unHandle, char * pchArgs, uint32_t unArgs) override;
	vr::EVRApplicationError GetStartingApplication(char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen) override;
	vr::EVRApplicationTransitionState GetTransitionState() override;
	const char * GetApplicationsTransitionStateNameFromEnum(vr::EVRApplicationTransitionState state) override;
	bool IsQuitUserPromptRequested() override;
	uint32_t GetCurrentSceneProcessId() override;

	bool GetInternalIndexForAppKey(const char *pchKey, int *index);

};

#define SYNC_APP_STATE(local_name, variable_name) \
SynchronizeChildVectors();\
auto local_name ## iter = m_context->iterators->applications_node.variable_name;\
update_iter(local_name ## iter,\
	m_context->state->applications_node.variable_name,\
	m_context->current_frame);\
auto & local_name = local_name ## iter ## .item;

bool VRApplicationsCursor::GetInternalIndexForAppKey(const char *key, int *index_ret)
{
	int index = m_context->m_tracking_set->GetApplicationsIndexer().get_index_for_key(key);
	if (index != -1)
	{
		*index_ret = index;
		return true;
	}
	return false;
}

bool VRApplicationsCursor::IsApplicationInstalled(const char * pchAppKey)
{
	LOG_ENTRY("CppStubIsApplicationInstalled");
	bool rc = false;
	int index;
	if (GetInternalIndexForAppKey(pchAppKey, &index))
	{
		SYNC_APP_STATE(is_app_installed, applications[index].is_installed);
		if (is_app_installed->is_present() && is_app_installed->val)
		{
			rc = true;
		}
	}
	LOG_EXIT_RC(rc, "CppStubIsApplicationInstalled");
}

uint32_t VRApplicationsCursor::GetApplicationCount()
{
	LOG_ENTRY("CppStubGetApplicationCount");
	
	SYNC_APP_STATE(active_application_indexes, active_application_indexes);
	uint32_t rc = active_application_indexes->val.size();
	LOG_EXIT_RC(rc, "CppStubGetApplicationCount");
}

template <typename T>
vr::EVRApplicationError return_app_key(T &app_key, char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
	vr::EVRApplicationError rc;
	if (app_key->is_present())
	{
		if (util_vector_to_return_buf_rc(&app_key->val, pchAppKeyBuffer, unAppKeyBufferLen, nullptr))
		{
			rc = VRApplicationError_None;
		}
		else
		{
			rc = VRApplicationError_BufferTooSmall;
		}
	}
	else
	{
		rc = vr::VRApplicationError_InvalidApplication;
	}
	return rc;
}

// since caller is asking for unApplicationIndex, need to map from External to internal index
vr::EVRApplicationError VRApplicationsCursor::GetApplicationKeyByIndex(uint32_t unExternalApplicationIndex, char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
	LOG_ENTRY("CppStubGetApplicationKeyByIndex");

	vr::EVRApplicationError rc = VRApplicationError_InvalidIndex;
	
	SYNC_APP_STATE(active_application_indexes, active_application_indexes);
	if (unExternalApplicationIndex < active_application_indexes->val.size())
	{
		int internal_index = active_application_indexes->val[unExternalApplicationIndex];
		int count;
		const char *app_key = m_context->m_tracking_set->GetApplicationsIndexer().get_key_for_index(internal_index, &count);
		if (util_char_to_return_buf_rc(app_key, count, pchAppKeyBuffer, unAppKeyBufferLen, nullptr))
		{
			rc = VRApplicationError_None;
		}
		else
		{
			rc = VRApplicationError_BufferTooSmall;
		}
	}
	LOG_EXIT_RC(rc, "CppStubGetApplicationKeyByIndex");
}

vr::EVRApplicationError VRApplicationsCursor::GetApplicationKeyByProcessId(
				uint32_t unProcessId, 
				char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
	LOG_ENTRY("CppStubGetApplicationKeyByProcessId");
	
	vr::EVRApplicationError rc = VRApplicationError_InvalidParameter;
	for (int i = 0; i < (int)m_context->iterators->applications_node.applications.size(); i++)
	{
		SYNC_APP_STATE(app_key, applications[i].application_key);
		SYNC_APP_STATE(process_id, applications[i].process_id);

		if (process_id->is_present() && process_id->val == unProcessId)
		{
			rc = return_app_key(app_key, pchAppKeyBuffer, unAppKeyBufferLen);
			break;
		}
	}

	LOG_EXIT_RC(rc, "CppStubGetApplicationKeyByProcessId");
}

uint32_t VRApplicationsCursor::GetApplicationProcessId(const char * pchAppKey)
{
	LOG_ENTRY("CppStubGetApplicationProcessId");

	uint32_t rc = 0;
	int internal_index;
	if (GetInternalIndexForAppKey(pchAppKey, &internal_index))
	{
		SYNC_APP_STATE(process_id, applications[internal_index].process_id);
		if (process_id->is_present())
		{
			rc = process_id->val;
		}
	}

	LOG_EXIT_RC(rc, "CppStubGetApplicationProcessId");
}

const char * VRApplicationsCursor::GetApplicationsErrorNameFromEnum(vr::EVRApplicationError error)
{
	LOG_ENTRY("CppStubGetApplicationsErrorNameFromEnum");
	const char *rc = openvr_string::EVRApplicationErrorToString(error);
	LOG_EXIT_RC(rc, "CppStubGetApplicationsErrorNameFromEnum");
}

uint32_t VRApplicationsCursor::GetApplicationPropertyString(
		const char * pchAppKey, 
		vr::EVRApplicationProperty prop_enum, 
		char * pchValue, uint32_t unBufferSize,
		vr::EVRApplicationError * pError)
{
	LOG_ENTRY("CppStubGetApplicationPropertyString");
	
	uint32_t rc = 1;	// 2/1/2017 - default is to return a 1 for empty string
	if (unBufferSize > 0)
	{
		pchValue[0] = '\0';
	}

	int internal_index;
	if (GetInternalIndexForAppKey(pchAppKey, &internal_index))
	{
		std::vector<char, ALLOCATOR_TYPE> *p;
		if (lookup_subtable_property(
				m_context->state->applications_node.applications[internal_index].string_props,
				m_context->iterators->applications_node.applications[internal_index].string_props,
				m_context->current_frame,
				prop_enum, &p, pError))
		{
			util_vector_to_return_buf_rc(p, pchValue, unBufferSize, &rc);
		}
	}
	LOG_EXIT_RC(rc, "CppStubGetApplicationPropertyString");
}

bool VRApplicationsCursor::GetApplicationPropertyBool(
			const char * pchAppKey, 
			vr::EVRApplicationProperty prop_enum, vr::EVRApplicationError * pError)
{
	LOG_ENTRY("CppStubGetApplicationPropertyBool");
	bool rc = false;
	int internal_index;
	if (GetInternalIndexForAppKey(pchAppKey, &internal_index))
	{
		bool *p;
		lookup_subtable_property(
				m_context->state->applications_node.applications[internal_index].bool_props,
				m_context->iterators->applications_node.applications[internal_index].bool_props,
				m_context->current_frame,
			prop_enum, &p, pError);
		rc = *p;
	}
	
	LOG_EXIT_RC(rc, "CppStubGetApplicationPropertyBool");
}

uint64_t VRApplicationsCursor::GetApplicationPropertyUint64(const char * pchAppKey,
			vr::EVRApplicationProperty prop_enum, vr::EVRApplicationError * pError)
{
	LOG_ENTRY("CppStubGetApplicationPropertyUint64");
	
	uint64_t rc = 0;
	int internal_index;
	if (GetInternalIndexForAppKey(pchAppKey, &internal_index))
	{
		uint64_t *p;
		lookup_subtable_property(
			m_context->state->applications_node.applications[internal_index].uint64_props,
			m_context->iterators->applications_node.applications[internal_index].uint64_props,
			m_context->current_frame,
			prop_enum, &p, pError);
		rc = *p;
	}

	LOG_EXIT_RC(rc, "CppStubGetApplicationPropertyUint64");
}

bool VRApplicationsCursor::GetApplicationAutoLaunch(const char * pchAppKey)
{
	LOG_ENTRY("CppStubGetApplicationAutoLaunch");
	bool rc = false;
	int internal_index;
	if (GetInternalIndexForAppKey(pchAppKey, &internal_index))
	{
		SYNC_APP_STATE(auto_launch, applications[internal_index].auto_launch);
		rc = auto_launch->is_present() && auto_launch->val;
	}
	LOG_EXIT_RC(rc, "CppStubGetApplicationAutoLaunch");
}

#define MIME_LOOKUP(field_name)\
bool rc; \
int index;\
if (GetIndexForMimeType(pchMimeType, &index))\
{\
	SYNC_APP_STATE(default_application, mime_types[index].default_application);\
	if (default_application->is_present())\
	{\
		if (util_vector_to_return_buf_rc(&default_application->val, Buffer, Len, nullptr))\
		{\
			rc = true;\
		}\
	}\
}\
rc = false;

bool VRApplicationsCursor::GetDefaultApplicationForMimeType(
				const char * pchMimeType, char * Buffer, uint32_t Len)
{
	LOG_ENTRY("CppStubGetDefaultApplicationForMimeType");
	MIME_LOOKUP(default_application);
	LOG_EXIT_RC(rc, "CppStubGetDefaultApplicationForMimeType");
}

bool VRApplicationsCursor::GetApplicationSupportedMimeTypes(const char * pchAppKey, 
					char * Buffer, uint32_t Len)
{
	LOG_ENTRY("CppStubGetApplicationSupportedMimeTypes");
	
	bool rc = false;
	int internal_index;
	if (GetInternalIndexForAppKey(pchAppKey, &internal_index))
	{
		SYNC_APP_STATE(supported_mime_types, applications[internal_index].supported_mime_types);
		if (supported_mime_types->is_present())
		{
			rc = util_vector_to_return_buf_rc(&supported_mime_types->val, Buffer, Len, nullptr);
		}
	}
	
	LOG_EXIT_RC(rc, "CppStubGetApplicationSupportedMimeTypes");
}

uint32_t VRApplicationsCursor::GetApplicationsThatSupportMimeType(const char * pchMimeType, char * Buffer, uint32_t Len)
{
	LOG_ENTRY("CppStubGetApplicationsThatSupportMimeType");
	MIME_LOOKUP(applications_that_support_mime_type);
	LOG_EXIT_RC(rc, "CppStubGetApplicationsThatSupportMimeType");
}

uint32_t VRApplicationsCursor::GetApplicationLaunchArguments(uint32_t unHandle, char * pchArgs, uint32_t unArgs)
{
	LOG_ENTRY("CppStubGetApplicationLaunchArguments");

	// This is a todo because it relies on the args handle that comes from one of theses events:
	// struct VREvent_ApplicationLaunch_t
	//{
	//	uint32_t pid;
	//	uint32_t unArgsHandle;
	//};


	assert(0);  // todo after i figure out app launch arguments 
	uint32_t rc = 2;
	LOG_EXIT_RC(rc, "CppStubGetApplicationLaunchArguments");
}

vr::EVRApplicationError VRApplicationsCursor::GetStartingApplication(char * pchAppKeyBuffer, uint32_t unAppKeyBufferLen)
{
	LOG_ENTRY("CppStubGetStartingApplication");
	
	vr::EVRApplicationError rc;
	SYNC_APP_STATE(starting_application, starting_application);

	if (starting_application->is_present())
	{
		if (!util_vector_to_return_buf_rc(&starting_application->val, pchAppKeyBuffer, unAppKeyBufferLen, nullptr))
		{
			rc = VRApplicationError_BufferTooSmall;
		}
		else
		{
			rc = starting_application->presence;
		}
	}
	else
	{
		rc = starting_application->presence;
	}
	LOG_EXIT_RC(rc, "CppStubGetStartingApplication");
}

vr::EVRApplicationTransitionState VRApplicationsCursor::GetTransitionState()
{
	LOG_ENTRY("CppStubGetTransitionState");
	SYNC_APP_STATE(transition_state, transition_state);
	vr::EVRApplicationTransitionState rc = transition_state->val;
	LOG_EXIT_RC(rc, "CppStubIsQuitUserPromptRequested");
}

const char * VRApplicationsCursor::GetApplicationsTransitionStateNameFromEnum(vr::EVRApplicationTransitionState state)
{
	LOG_ENTRY("CppStubGetApplicationsTransitionStateNameFromEnum");
	const char * rc = openvr_string::EVRApplicationTransitionStateToString(state);
	LOG_EXIT_RC(rc, "CppStubGetApplicationsTransitionStateNameFromEnum");
}

bool VRApplicationsCursor::IsQuitUserPromptRequested()
{
	LOG_ENTRY("CppStubIsQuitUserPromptRequested");
	bool rc;
	SYNC_APP_STATE(is_quit_user_prompt, is_quit_user_prompt);
	rc = is_quit_user_prompt->is_present() && is_quit_user_prompt->val;
	LOG_EXIT_RC(rc, "CppStubIsQuitUserPromptRequested");
}

uint32_t VRApplicationsCursor::GetCurrentSceneProcessId()
{
	LOG_ENTRY("CppStubGetCurrentSceneProcessId");
	uint32_t rc;
	SYNC_APP_STATE(current_scene_process_id, current_scene_process_id);
	rc = current_scene_process_id->val;
	LOG_EXIT_RC(rc, "CppStubGetCurrentSceneProcessId");
}

class VRSettingsCursor : public VRSettingsCppStub
{
public:
	VRSettingsCursor(CursorContext *context)
		:
		m_context(context)
	{	}

	const char * GetSettingsErrorNameFromEnum(vr::EVRSettingsError eError) override;
	bool GetBool(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * peError) override;
	int32_t GetInt32(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * peError) override;
	float GetFloat(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * peError) override;
	void GetString(const char * pchSection, const char * pchSettingsKey, char * pchValue, uint32_t unValueLen, vr::EVRSettingsError * peError) override;

	bool GetIndexForSection(const char * pchSection, int *index);
	CursorContext *m_context;
};

bool VRSettingsCursor::GetIndexForSection(const char * pchSection, int *index)
{
	bool rc = false;
	for (int i = 0; i < (int)m_context->iterators->settings_node.sections.size(); i++)
	{
		if (strcmp(pchSection, m_context->iterators->settings_node.sections[i].section_name) == 0)
		{
			*index = i;
			rc = true;
			break;
		}
	}
	return rc;
}

static bool GetIndexForSubSection(const char *settings_key, const char *const*tbl, const int tbl_size, int *index)
{
	bool rc = false;
	for (int i = 0; i < tbl_size; i++)
	{
		if (strcmp(settings_key, tbl[i]) == 0)
		{
		*index = i;
		rc = true;
		break;
		}
	}
	return rc;
}

const char * VRSettingsCursor::GetSettingsErrorNameFromEnum(vr::EVRSettingsError eError)
{
	LOG_ENTRY("CppStubGetSettingsErrorNameFromEnum");
	const char * rc = openvr_string::EVRSettingsErrorToString(eError);
	LOG_EXIT_RC(rc, "CppStubGetSettingsErrorNameFromEnum");
}

#define GET_SETTING(settings_field_name) \
int index;\
if (GetIndexForSection(pchSection, &index))\
{\
	int subsection_index;\
	if (GetIndexForSubSection(pchSettingsKey,\
		m_context->iterators->settings_node.sections[index].settings_field_name.tbl,\
		m_context->iterators->settings_node.sections[index].settings_field_name.tbl_size,\
		&subsection_index))\
	{\
		auto iter = m_context->iterators->settings_node.sections[index].settings_field_name.nodes[subsection_index];\
		auto state = m_context->state->settings_node.sections[index].settings_field_name.nodes[subsection_index];\
		update_iter(iter, state, m_context->current_frame);\
\
		if (pError)\
		{\
			*pError = iter.item->presence;\
		}\
			\
		if (iter.item->is_present())\
		{\
			rc = iter.item->val;\
		}\
	}\
}

bool VRSettingsCursor::GetBool(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * pError)
{
	LOG_ENTRY("CppStubGetBool");
	bool rc = false;
	GET_SETTING(bool_settings);
	LOG_EXIT_RC(rc, "CppStubGetBool");
}

int32_t VRSettingsCursor::GetInt32(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * pError)
{
	LOG_ENTRY("CppStubGetInt32");
	int32_t rc = 0;
	GET_SETTING(int32_settings);
	LOG_EXIT_RC(rc, "CppStubGetInt32");
}

float VRSettingsCursor::GetFloat(const char * pchSection, const char * pchSettingsKey, vr::EVRSettingsError * pError)
{
	LOG_ENTRY("CppStubGetFloat");
	float rc = 0.0f;
	GET_SETTING(float_settings);
	LOG_EXIT_RC(rc, "CppStubGetFloat");
}

void VRSettingsCursor::GetString(const char * pchSection, const char * pchSettingsKey, 
					char * Buffer, uint32_t Len, vr::EVRSettingsError * pError)
{
	LOG_ENTRY("CppStubGetString");

	bool rc = false; 
	int index; 
	if (GetIndexForSection(pchSection, &index))
	{
		int subsection_index;
		if (GetIndexForSubSection(pchSettingsKey,
			m_context->iterators->settings_node.sections[index].string_settings.tbl,
			m_context->iterators->settings_node.sections[index].string_settings.tbl_size,
			&subsection_index))
		{
			auto iter = m_context->iterators->settings_node.sections[index].string_settings.nodes[subsection_index];
			auto state = m_context->state->settings_node.sections[index].string_settings.nodes[subsection_index];
			update_iter(iter, state, m_context->current_frame);

			if (pError)
			{
				*pError = iter.item->presence;
			}

			if (iter.item->is_present())
			{
				util_vector_to_return_buf_rc(&iter.item->val, Buffer, Len, nullptr);
			}

			rc = iter.item->is_present();
		}
	}
	LOG_EXIT("CppStubGetString");
}

struct VRChaperoneCursor : public VRChaperoneCppStub
{
	VRChaperoneCursor(CursorContext *context) : m_context(context) {}
	vr::ChaperoneCalibrationState GetCalibrationState() override;
	bool GetPlayAreaSize(float * pSizeX, float * pSizeZ) override;
	bool GetPlayAreaRect(struct vr::HmdQuad_t * rect) override;
	bool AreBoundsVisible() override;
	void GetBoundsColor(struct vr::HmdColor_t * pOutputColorArray, int nNumOutputColors, float flCollisionBoundsFadeDistance, struct vr::HmdColor_t * pOutputCameraColor) override;
	CursorContext *m_context;
};

#define SYNC_CHAP_STATE(local_name, variable_name) \
auto local_name ## iter = m_context->iterators->chaperone_node.variable_name;\
update_iter(local_name ## iter,\
	m_context->state->chaperone_node.variable_name,\
	m_context->current_frame);\
auto & local_name = local_name ## iter ## .item;

vr::ChaperoneCalibrationState VRChaperoneCursor::GetCalibrationState()
{
	LOG_ENTRY("CppStubGetCalibrationState");
	vr::ChaperoneCalibrationState rc;
	SYNC_CHAP_STATE(calibration_state, calibration_state);
	rc = calibration_state->val;
	LOG_EXIT_RC(rc, "CppStubGetCalibrationState");
}

bool VRChaperoneCursor::GetPlayAreaSize(float * pSizeX, float * pSizeZ)
{
	LOG_ENTRY("CppStubGetPlayAreaSize");
	SYNC_CHAP_STATE(play_area_size, play_area_size);
	if (play_area_size->is_present())
	{
		if (pSizeX)
			*pSizeX = play_area_size->val.v[0];
		if (pSizeZ)
			*pSizeZ = play_area_size->val.v[1];
	}

	bool rc = play_area_size->is_present();
	LOG_EXIT_RC(rc, "CppStubGetPlayAreaSize");
}

bool VRChaperoneCursor::GetPlayAreaRect(struct vr::HmdQuad_t * rect)
{
	LOG_ENTRY("CppStubGetPlayAreaRect");
	SYNC_CHAP_STATE(play_area_rect, play_area_rect);
	if (play_area_rect->is_present() && rect)
	{
		*rect = play_area_rect->val;
	}
	bool rc = play_area_rect->is_present();
	LOG_EXIT_RC(rc, "CppStubGetPlayAreaRect");
}

bool VRChaperoneCursor::AreBoundsVisible()
{
	LOG_ENTRY("CppStubAreBoundsVisible");
	SYNC_CHAP_STATE(bounds_visible, bounds_visible);
	bool rc = bounds_visible->is_present() && bounds_visible->val;
	LOG_EXIT_RC(rc, "CppStubAreBoundsVisible");
}

void VRChaperoneCursor::GetBoundsColor(struct vr::HmdColor_t * pOutputColorArray, int nNumOutputColors,
	float flCollisionBoundsFadeDistance, struct vr::HmdColor_t * pOutputCameraColor)
{
	LOG_ENTRY("CppStubGetBoundsColor");
	if (pOutputColorArray)
	{
		SYNC_CHAP_STATE(bounds_colors, bounds_colors);
		if (bounds_colors->val.size() == 0)
		{
			memset(pOutputColorArray, 0, sizeof(vr::HmdColor_t) * nNumOutputColors);
		}
		else
		{
			int elements_to_copy = std::min(nNumOutputColors, (int)bounds_colors->val.size());
			memcpy(pOutputColorArray, &bounds_colors->val.at(0), sizeof(vr::HmdColor_t) * elements_to_copy);
			while (elements_to_copy < nNumOutputColors)
			{
				pOutputColorArray[elements_to_copy++] = bounds_colors->val.back();
			}
		}
	}

	if (pOutputCameraColor)
	{
		SYNC_CHAP_STATE(camera_color, camera_color);
		*pOutputCameraColor = camera_color->val;
	}

	LOG_EXIT("CppStubGetBoundsColor");
}

class VRChaperoneSetupCursor : public VRChaperoneSetupCppStub
{
public:
	VRChaperoneSetupCursor(CursorContext *context) : m_context(context) {}

	bool GetWorkingPlayAreaSize(float * pSizeX, float * pSizeZ) override;
	bool GetWorkingPlayAreaRect(struct vr::HmdQuad_t * rect) override;
	bool GetWorkingCollisionBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t * punQuadsCount) override;
	bool GetLiveCollisionBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t * punQuadsCount) override;
	bool GetWorkingSeatedZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pmatSeatedZeroPoseToRawTrackingPose) override;
	bool GetWorkingStandingZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pmatStandingZeroPoseToRawTrackingPose) override;
	bool GetLiveSeatedZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pmatSeatedZeroPoseToRawTrackingPose) override;
	bool GetLiveCollisionBoundsTagsInfo(uint8_t * pTagsBuffer, uint32_t * punTagCount) override;
	bool GetLivePhysicalBoundsInfo(struct vr::HmdQuad_t * pQuadsBuffer, uint32_t * punQuadsCount) override;

	CursorContext *m_context;
};

#define SYNC_CHAP_SETUP_STATE(local_name, variable_name) \
auto local_name ## iter = m_context->iterators->chaperone_setup_node.variable_name;\
update_iter(local_name ## iter,\
	m_context->state->chaperone_setup_node.variable_name,\
	m_context->current_frame);\
auto & local_name = local_name ## iter ## .item;

bool VRChaperoneSetupCursor::GetWorkingPlayAreaSize(float * pSizeX, float * pSizeZ)
{
	LOG_ENTRY("CppStubGetWorkingPlayAreaSize");
	SYNC_CHAP_SETUP_STATE(working_play_area_size, working_play_area_size);

	if (working_play_area_size->is_present())
	{
		if (pSizeX)
			*pSizeX = working_play_area_size->val.v[0];
		if (pSizeZ)
			*pSizeZ = working_play_area_size->val.v[1];
	}

	bool rc = working_play_area_size->presence;
	LOG_EXIT_RC(rc, "CppStubGetWorkingPlayAreaSize");
}

bool VRChaperoneSetupCursor::GetWorkingPlayAreaRect(struct vr::HmdQuad_t * rect)
{
	LOG_ENTRY("CppStubGetWorkingPlayAreaRect");
	SYNC_CHAP_SETUP_STATE(working_play_area_rect, working_play_area_rect);
	if (working_play_area_rect->is_present())
	{
		if (rect)
		{
			*rect = working_play_area_rect->val;
		}
	}
	bool rc = working_play_area_rect->presence;
	LOG_EXIT_RC(rc, "CppStubGetWorkingPlayAreaRect");
}

// return semantics with a wrinkle
#define GetWrinkleBounds(param_name)\
bool rc = false; \
SYNC_CHAP_SETUP_STATE(param_name, param_name);\
if (param_name->is_present())\
{\
	if (pBuffer == nullptr && !punCount || (punCount && *punCount))\
	{\
		rc = false;\
	}\
	else\
	{\
		rc = util_vector_to_return_buf_rc(&param_name->val, pBuffer, *punCount, punCount);\
	}\
}

bool VRChaperoneSetupCursor::GetWorkingCollisionBoundsInfo(struct vr::HmdQuad_t * pBuffer, uint32_t * punCount)
{
	LOG_ENTRY("CppStubGetWorkingCollisionBoundsInfo");

	GetWrinkleBounds(working_collision_bounds_info);

	LOG_EXIT_RC(rc, "CppStubGetWorkingCollisionBoundsInfo");
}

bool VRChaperoneSetupCursor::GetLiveCollisionBoundsInfo(struct vr::HmdQuad_t * pBuffer, uint32_t * punCount)
{
	LOG_ENTRY("CppStubGetLiveCollisionBoundsInfo");

	GetWrinkleBounds(live_collision_bounds_info);

	LOG_EXIT_RC(rc, "CppStubGetLiveCollisionBoundsInfo");
}

#define GETPOSE(param_name)\
bool rc;\
\
SYNC_CHAP_SETUP_STATE(param_name, param_name);\
if (pose_ret && param_name->is_present())\
{\
	*pose_ret = param_name->val;\
	rc = true;\
}\
else\
{\
	rc = false;\
}\

bool VRChaperoneSetupCursor::GetWorkingSeatedZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pose_ret)
{
	LOG_ENTRY("CppStubGetWorkingSeatedZeroPoseToRawTrackingPose");
	GETPOSE(working_seated2rawtracking);
	LOG_EXIT_RC(rc, "CppStubGetWorkingSeatedZeroPoseToRawTrackingPose");
}

bool VRChaperoneSetupCursor::GetWorkingStandingZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pose_ret)
{
	LOG_ENTRY("CppStubGetWorkingStandingZeroPoseToRawTrackingPose");
	GETPOSE(working_standing2rawtracking);
	LOG_EXIT_RC(rc, "CppStubGetWorkingStandingZeroPoseToRawTrackingPose");
}

bool VRChaperoneSetupCursor::GetLiveSeatedZeroPoseToRawTrackingPose(struct vr::HmdMatrix34_t * pose_ret)
{
	LOG_ENTRY("CppStubGetLiveSeatedZeroPoseToRawTrackingPose");
	GETPOSE(live_seated2rawtracking);
	LOG_EXIT_RC(rc, "CppStubGetLiveSeatedZeroPoseToRawTrackingPose");
}

bool VRChaperoneSetupCursor::GetLiveCollisionBoundsTagsInfo(uint8_t * pBuffer, uint32_t * punCount)
{
	LOG_ENTRY("CppStubGetLiveCollisionBoundsTagsInfo");
	GetWrinkleBounds(live_collision_bounds_tags_info);
	LOG_EXIT_RC(rc, "CppStubGetLiveCollisionBoundsTagsInfo");
}

bool VRChaperoneSetupCursor::GetLivePhysicalBoundsInfo(struct vr::HmdQuad_t * pBuffer, uint32_t * punCount)
{
	LOG_ENTRY("CppStubGetLivePhysicalBoundsInfo");
	GetWrinkleBounds(live_physical_bounds_info);
	LOG_EXIT_RC(rc, "CppStubGetLivePhysicalBoundsInfo");
}

class VRCompositorCursor : public VRCompositorCppStub
{
public:
	VRCompositorCursor(CursorContext *context) : m_context(context) 
	{
		// synchronize controllers vector
		while (m_context->iterators->compositor_node.controllers.size() <
			m_context->state->compositor_node.controllers.size())
		{
			m_context->iterators->compositor_node.controllers.emplace_back(m_context->m_allocator);
		}
	}

	vr::ETrackingUniverseOrigin GetTrackingSpace() override;
	vr::EVRCompositorError GetLastPoses(struct vr::TrackedDevicePose_t * pRenderPoseArray, uint32_t unRenderPoseArrayCount, struct vr::TrackedDevicePose_t * pGamePoseArray, uint32_t unGamePoseArrayCount) override;
	vr::EVRCompositorError GetLastPoseForTrackedDeviceIndex(vr::TrackedDeviceIndex_t unDeviceIndex, struct vr::TrackedDevicePose_t * pOutputPose, struct vr::TrackedDevicePose_t * pOutputGamePose) override;
	bool GetFrameTiming(struct vr::Compositor_FrameTiming * pTiming, uint32_t unFramesAgo) override;
	uint32_t GetFrameTimings(struct vr::Compositor_FrameTiming * pTiming, uint32_t nFrames) override;
	float GetFrameTimeRemaining() override;
	void GetCumulativeStats(struct vr::Compositor_CumulativeStats * pStats, uint32_t nStatsSizeInBytes) override;
	struct vr::HmdColor_t GetCurrentFadeColor(bool bBackground) override;
	float GetCurrentGridAlpha() override;
	bool IsFullscreen() override;
	uint32_t GetCurrentSceneFocusProcess() override;
	uint32_t GetLastFrameRenderer() override;
	bool CanRenderScene() override;
	bool IsMirrorWindowVisible() override;
	bool ShouldAppRenderWithLowResources() override;
	uint32_t GetVulkanInstanceExtensionsRequired(char * pchValue, uint32_t unBufferSize) override;
	uint32_t GetVulkanDeviceExtensionsRequired(struct VkPhysicalDevice_T * pPhysicalDevice, char * pchValue, uint32_t unBufferSize) override;

	CursorContext *m_context;
};

#define SYNC_COMP_STATE(local_name, variable_name) \
auto local_name ## iter = m_context->iterators->compositor_node.variable_name;\
update_iter(local_name ## iter,\
	m_context->state->compositor_node.variable_name,\
	m_context->current_frame);\
auto & local_name = local_name ## iter ## .item;

vr::ETrackingUniverseOrigin VRCompositorCursor::GetTrackingSpace()
{
	LOG_ENTRY("CppStubGetTrackingSpace");
	SYNC_COMP_STATE(tracking_space, tracking_space);
	vr::ETrackingUniverseOrigin rc = tracking_space->val;
	LOG_EXIT_RC(rc, "CppStubGetTrackingSpace");
}

vr::EVRCompositorError VRCompositorCursor::GetLastPoses(
		struct vr::TrackedDevicePose_t * pRenderPoseArray, uint32_t unRenderPoseArrayCount, 
		struct vr::TrackedDevicePose_t * pGamePoseArray, uint32_t unGamePoseArrayCount)
{
	LOG_ENTRY("CppStubGetLastPoses");

	vr::EVRCompositorError rc = vr::VRCompositorError_None;
	
	// rebuild the arrays from the controllers
	for (int i = 0; 
		i < (int)unRenderPoseArrayCount && 
		i < (int)unGamePoseArrayCount &&
		i < (int)m_context->iterators->compositor_node.controllers.size();
		i++)
	{
		if (i < (int)unRenderPoseArrayCount)
		{
			SYNC_COMP_STATE(last_render_pose, controllers[i].last_render_pose);
			if (last_render_pose->is_present())
			{
				pRenderPoseArray[i] = last_render_pose->val;
			}
			else
			{
				rc = last_render_pose->presence;
				break;
			}
		}
		if (i < (int)unGamePoseArrayCount)
		{
			SYNC_COMP_STATE(last_game_pose, controllers[i].last_game_pose);
			if (last_game_pose->is_present())
			{
				pGamePoseArray[i] = last_game_pose->val;
			}
			else
			{
				rc = last_game_pose->presence;
				break;
			}
		}
	}

	LOG_EXIT_RC(rc, "CppStubGetLastPoses");
}

vr::EVRCompositorError VRCompositorCursor::GetLastPoseForTrackedDeviceIndex(
	vr::TrackedDeviceIndex_t unDeviceIndex, 
	struct vr::TrackedDevicePose_t * pOutputPose, 
	struct vr::TrackedDevicePose_t * pOutputGamePose)
{
	LOG_ENTRY("CppStubGetLastPoseForTrackedDeviceIndex");

	vr::EVRCompositorError rc = vr::VRCompositorError_None;

	if (unDeviceIndex >= m_context->iterators->compositor_node.controllers.size())
	{
		rc = VRCompositorError_IndexOutOfRange;
	}
	else
	{
		if (pOutputPose)
		{
			SYNC_COMP_STATE(last_render_pose, controllers[unDeviceIndex].last_render_pose);
			if (last_render_pose->is_present())
			{
				*pOutputPose = last_render_pose->val;
			}
			else
			{
				rc = last_render_pose->presence;
			}
		}
		if (pOutputGamePose && rc == vr::VRCompositorError_None)
		{
			SYNC_COMP_STATE(last_game_pose, controllers[unDeviceIndex].last_game_pose);
			if (last_game_pose->is_present())
			{
				*pOutputGamePose = last_game_pose->val;
			}
			else
			{
				rc = last_game_pose->presence;
			}
		}
	}
	LOG_EXIT_RC(rc, "CppStubGetLastPoseForTrackedDeviceIndex");
}

bool VRCompositorCursor::GetFrameTiming(struct vr::Compositor_FrameTiming * pTiming, uint32_t unFramesAgo)
{
	LOG_ENTRY("CppStubGetFrameTiming");
	bool rc = false;
	SYNC_COMP_STATE(frame_timing, frame_timing);	// TODO : I can actually walk backwards and figure unFramesAgo out
	if (pTiming && frame_timing->is_present())		//        OR - I could cache the frame timing table out of
													//        band - e.g. like the events and timestamps
	{
		*pTiming = frame_timing->val;
		rc = true;
	}
	LOG_EXIT_RC(rc, "CppStubGetFrameTiming");
}

uint32_t VRCompositorCursor::GetFrameTimings(struct vr::Compositor_FrameTiming * pTiming, uint32_t nFrames)
{
	LOG_ENTRY("CppStubGetFrameTimings");				// TODO/Notes: See GetFrameTiming comment above
	uint32_t rc = 0;
	SYNC_COMP_STATE(frame_timings, frame_timings);
	if (pTiming && frame_timings->is_present() && frame_timings->val.size() > 0)
	{
		nFrames = std::min(frame_timings->val.size(), nFrames);
		memcpy(pTiming, &frame_timings->val.at(0), sizeof(vr::Compositor_FrameTiming)*nFrames);
		rc = nFrames;
	}
	LOG_EXIT_RC(rc, "CppStubGetFrameTimings");
}

float VRCompositorCursor::GetFrameTimeRemaining()
{
	LOG_ENTRY("CppStubGetFrameTimeRemaining");
	SYNC_COMP_STATE(frame_time_remaining, frame_time_remaining);
	float rc = frame_time_remaining->val;
	LOG_EXIT_RC(rc, "CppStubGetFrameTimeRemaining");
}

void VRCompositorCursor::GetCumulativeStats(struct vr::Compositor_CumulativeStats * pStats, uint32_t nStatsSizeInBytes)
{
	LOG_ENTRY("CppStubGetCumulativeStats");

	SYNC_COMP_STATE(cumulative_stats, cumulative_stats);
	if (pStats && nStatsSizeInBytes == sizeof(vr::Compositor_CumulativeStats))
	{
		*pStats = cumulative_stats->val;
	}

	LOG_EXIT("CppStubGetCumulativeStats");
}

struct vr::HmdColor_t VRCompositorCursor::GetCurrentFadeColor(bool bBackground)
{
	LOG_ENTRY("CppStubGetCurrentFadeColor");

	struct vr::HmdColor_t rc;
	if (bBackground == false)
	{
		SYNC_COMP_STATE(foreground_fade_color, foreground_fade_color);
		rc = foreground_fade_color->val;
	}
	else
	{
		SYNC_COMP_STATE(background_fade_color, background_fade_color);
		rc = background_fade_color->val;
	}

	LOG_EXIT_RC(rc, "CppStubGetCurrentFadeColor");
}

float VRCompositorCursor::GetCurrentGridAlpha()
{
	LOG_ENTRY("CppStubGetCurrentGridAlpha");
	SYNC_COMP_STATE(grid_alpha, grid_alpha);
	float rc = grid_alpha->val;
	LOG_EXIT_RC(rc, "CppStubGetCurrentGridAlpha");
}

bool VRCompositorCursor::IsFullscreen()
{
	LOG_ENTRY("CppStubIsFullscreen");
	SYNC_COMP_STATE(is_fullscreen, is_fullscreen);
	bool rc = is_fullscreen->val;
	LOG_EXIT_RC(rc, "CppStubIsFullscreen");
}

uint32_t VRCompositorCursor::GetCurrentSceneFocusProcess()
{
	LOG_ENTRY("CppStubGetCurrentSceneFocusProcess");
	SYNC_COMP_STATE(current_scene_focus_process, current_scene_focus_process);
	uint32_t rc = current_scene_focus_process->val;
	LOG_EXIT_RC(rc, "CppStubGetCurrentSceneFocusProcess");
}

uint32_t VRCompositorCursor::GetLastFrameRenderer()
{
	LOG_ENTRY("CppStubGetLastFrameRenderer");
	SYNC_COMP_STATE(last_frame_renderer, last_frame_renderer);
	uint32_t rc = last_frame_renderer->val;
	LOG_EXIT_RC(rc, "CppStubGetLastFrameRenderer");
}

bool VRCompositorCursor::CanRenderScene()
{
	LOG_ENTRY("CppStubCanRenderScene");
	SYNC_COMP_STATE(can_render_scene, can_render_scene);
	bool rc = can_render_scene->val;
	LOG_EXIT_RC(rc, "CppStubCanRenderScene");
}

bool VRCompositorCursor::IsMirrorWindowVisible()
{
	LOG_ENTRY("CppStubIsMirrorWindowVisible");
	SYNC_COMP_STATE(is_mirror_visible, is_mirror_visible);
	bool rc = is_mirror_visible->val;
	LOG_EXIT_RC(rc, "CppStubIsMirrorWindowVisible");
}

bool VRCompositorCursor::ShouldAppRenderWithLowResources()
{
	LOG_ENTRY("CppStubShouldAppRenderWithLowResources");
	SYNC_COMP_STATE(should_app_render_with_low_resource, should_app_render_with_low_resource);
	bool rc = should_app_render_with_low_resource->val;
	LOG_EXIT_RC(rc, "CppStubShouldAppRenderWithLowResources");
}

uint32_t VRCompositorCursor::GetVulkanInstanceExtensionsRequired(char * pchValue, uint32_t unBufferSize)
{
	LOG_ENTRY("CppStubGetVulkanInstanceExtensionsRequired");
	uint32_t rc = 0;
	SYNC_COMP_STATE(instance_extensions_required, instance_extensions_required);
	if (instance_extensions_required->is_present())
	{
		util_vector_to_return_buf_rc(&instance_extensions_required->val, pchValue, unBufferSize,&rc);
	}
	LOG_EXIT_RC(rc, "CppStubGetVulkanInstanceExtensionsRequired");
}

uint32_t VRCompositorCursor::GetVulkanDeviceExtensionsRequired(struct VkPhysicalDevice_T * pPhysicalDevice, char * pchValue, uint32_t unBufferSize)
{
	LOG_ENTRY("CppStubGetVulkanDeviceExtensionsRequired");
	static uint32_t rc = 0; // todo
	LOG_EXIT_RC(rc, "CppStubGetVulkanDeviceExtensionsRequired");
}

class VRNotificationsCursor : public VRNotificationsCppStub
{
public:
	VRNotificationsCursor(CursorContext *context)
	{
	}
};

class VROverlayCursor : public VROverlayCppStub
{
public:

	CursorContext *m_context;
	vrstate::overlay_schema &state_ref;
	vriterator::overlay_schema &iter_ref;

	VROverlayCursor(CursorContext *context)
		:
		m_context(context),
		state_ref(m_context->state->overlay_node),
		iter_ref(m_context->iterators->overlay_node)
	{
		SynchronizeChildVectors();
	}

	void SynchronizeChildVectors()
	{
		while (iter_ref.overlays.size() < state_ref.overlays.size())
		{
			iter_ref.overlays.emplace_back(m_context->m_allocator);
		}
	}

	vr::EVROverlayError GetOverlayIndexForHandle(vr::VROverlayHandle_t ulOverlayHandle, int *index);

	vr::TrackedDeviceIndex_t GetPrimaryDashboardDevice() override;
	bool IsDashboardVisible() override;
	
	EVROverlayError FindOverlay(const char *pchOverlayKey, VROverlayHandle_t * pOverlayHandle) override;
	uint32_t GetOverlayKey(vr::VROverlayHandle_t ulOverlayHandle, char * pchValue, uint32_t unBufferSize, vr::EVROverlayError * pError) override;
	uint32_t GetOverlayName(vr::VROverlayHandle_t ulOverlayHandle, char * pchValue, uint32_t unBufferSize, vr::EVROverlayError * pError) override;
	vr::EVROverlayError GetOverlayImageData(vr::VROverlayHandle_t ulOverlayHandle, void * pvBuffer, uint32_t unBufferSize, uint32_t * punWidth, uint32_t * punHeight) override;

	uint32_t GetOverlayRenderingPid(VROverlayHandle_t ulOverlayHandle) override;
	bool IsOverlayVisible(VROverlayHandle_t ulOverlayHandle) override;
	bool IsHoverTargetOverlay(VROverlayHandle_t ulOverlayHandle) override;
	EVROverlayError GetOverlayFlag(VROverlayHandle_t ulOverlayHandle, VROverlayFlags eOverlayFlag, bool *pbEnabled) override;

	vr::EVROverlayError GetOverlayColor(vr::VROverlayHandle_t ulOverlayHandle, float * pfRed, float * pfGreen, float * pfBlue) override;
	vr::EVROverlayError GetOverlayAlpha(vr::VROverlayHandle_t ulOverlayHandle, float * pfAlpha) override;
	vr::EVROverlayError GetOverlayTexelAspect(vr::VROverlayHandle_t ulOverlayHandle, float * pfTexelAspect) override;
	vr::EVROverlayError GetOverlaySortOrder(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * punSortOrder) override;
	vr::EVROverlayError GetOverlayWidthInMeters(vr::VROverlayHandle_t ulOverlayHandle, float * pfWidthInMeters) override;
	vr::EVROverlayError GetOverlayAutoCurveDistanceRangeInMeters(vr::VROverlayHandle_t ulOverlayHandle, float * pfMinDistanceInMeters, float * pfMaxDistanceInMeters) override;
	vr::EVROverlayError GetOverlayTextureColorSpace(vr::VROverlayHandle_t ulOverlayHandle, vr::EColorSpace * peTextureColorSpace) override;
	vr::EVROverlayError GetOverlayTextureBounds(vr::VROverlayHandle_t ulOverlayHandle, struct vr::VRTextureBounds_t * pOverlayTextureBounds) override;
	vr::EVROverlayError GetOverlayTransformType(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayTransformType * peTransformType) override;
	vr::EVROverlayError GetOverlayTransformAbsolute(vr::VROverlayHandle_t ulOverlayHandle, vr::ETrackingUniverseOrigin * peTrackingOrigin, struct vr::HmdMatrix34_t * pmatTrackingOriginToOverlayTransform) override;
	vr::EVROverlayError GetOverlayTransformTrackedDeviceRelative(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t * punTrackedDevice, struct vr::HmdMatrix34_t * pmatTrackedDeviceToOverlayTransform) override;
	vr::EVROverlayError GetOverlayTransformTrackedDeviceComponent(vr::VROverlayHandle_t ulOverlayHandle, vr::TrackedDeviceIndex_t * punDeviceIndex, char * pchComponentName, uint32_t unComponentNameSize) override;
	vr::EVROverlayError GetOverlayInputMethod(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayInputMethod * peInputMethod) override;
	vr::EVROverlayError GetOverlayMouseScale(vr::VROverlayHandle_t ulOverlayHandle, struct vr::HmdVector2_t * pvecMouseScale) override;
	vr::VROverlayHandle_t GetGamepadFocusOverlay() override;
	vr::EVROverlayError GetOverlayTextureSize(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * pWidth, uint32_t * pHeight) override;
	
	bool IsActiveDashboardOverlay(vr::VROverlayHandle_t ulOverlayHandle) override;
	vr::EVROverlayError GetDashboardOverlaySceneProcess(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * punProcessId) override;
	uint32_t GetKeyboardText(char * pchText, uint32_t cchText) override;
	vr::EVROverlayError GetOverlayFlags(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * pFlags) override;
};

#define SYNC_OVERLAY_STATE(local_name, variable_name) \
SynchronizeChildVectors();\
auto local_name ## iter = iter_ref.variable_name;\
update_iter(local_name ## iter,\
	state_ref.variable_name,\
	m_context->current_frame);\
auto & local_name = local_name ## iter ## .item;

vr::TrackedDeviceIndex_t VROverlayCursor::GetPrimaryDashboardDevice()
{
	LOG_ENTRY("CppStubGetPrimaryDashboardDevice");

	vr::TrackedDeviceIndex_t rc;
	SYNC_OVERLAY_STATE(primary_dashboard_device, primary_dashboard_device);
	rc = primary_dashboard_device->val;
	LOG_EXIT_RC(rc, "CppStubGetPrimaryDashboardDevice");
}

bool VROverlayCursor::IsDashboardVisible()
{
	LOG_ENTRY("CppStubIsDashboardVisible");
	bool rc;
	SYNC_OVERLAY_STATE(dashboard_visible, is_dashboard_visible);
	rc = dashboard_visible->val;
	LOG_EXIT_RC(rc, "CppStubIsDashboardVisible");
}

vr::EVROverlayError VROverlayCursor::FindOverlay(const char *pchOverlayKey, VROverlayHandle_t * pOverlayHandle)
{
	vr::EVROverlayError rc = VROverlayError_UnknownOverlay;
	OverlayIndexer *h = &m_context->m_tracking_set->GetOverlayIndexer();

	// regardless of result, pOverlayHandle return value is set 2/6/2017
	if (pOverlayHandle)
	{
		*pOverlayHandle = 0;
	}

	int index = h->get_index_for_key(pchOverlayKey);			// find index
	if (index >= 0)
	{
		SYNC_OVERLAY_STATE(handle, overlays[index].overlay_handle); // synchronize time.  overlay_handles will change
		if (handle->is_present())
		{
			*pOverlayHandle = handle->val;
		}
		else
		{
			*pOverlayHandle = k_ulOverlayHandleInvalid;
		}
		rc = handle->presence;
	}
	return rc;
}


vr::EVROverlayError 
VROverlayCursor::GetOverlayIndexForHandle(vr::VROverlayHandle_t ulOverlayHandle, int *index_ret)
{
	bool found_it = false;
	SYNC_OVERLAY_STATE(active_overlay_indexes, active_overlay_indexes);	// what overlays are currently present

	for (auto iter = active_overlay_indexes->val.begin(); iter != active_overlay_indexes->val.end(); iter++)
	{
		int index = *iter;
		SYNC_OVERLAY_STATE(handle, overlays[index].overlay_handle);
		if (handle->is_present())
		{
			if (handle->val == ulOverlayHandle)
			{
				*index_ret = index;
				found_it = true;
			}
		}
	}
	vr::EVROverlayError rc;
	if (found_it)
	{		
		rc = vr::VROverlayError_None;
	}
	else
	{
		rc = vr::VROverlayError_InvalidHandle;	// 2/6/2017: this is what's returned for non existant handles
	}
	return rc;
}

#define LOOKUP_OVERLAY_STRING(VAR_NAME)\
int index;\
vr::EVROverlayError found_it = GetOverlayIndexForHandle(ulOverlayHandle, &index);\
uint32_t rc = 0;\
\
if (found_it == vr::VROverlayError_None)\
{\
	SYNC_OVERLAY_STATE(key, overlays[index].VAR_NAME);\
	util_vector_to_return_buf_rc(&key->val, pchValue, unBufferSize, &rc);\
}\
\
if (pError)\
{\
	*pError = found_it;\
}

uint32_t VROverlayCursor::GetOverlayKey(vr::VROverlayHandle_t ulOverlayHandle, char * pchValue, uint32_t unBufferSize, vr::EVROverlayError * pError)
{
	LOG_ENTRY("CppStubGetOverlayKey");
	LOOKUP_OVERLAY_STRING(overlay_key);
	LOG_EXIT_RC(rc, "CppStubGetOverlayKey");
}

uint32_t VROverlayCursor::GetOverlayName(vr::VROverlayHandle_t ulOverlayHandle, char * pchValue, uint32_t unBufferSize, vr::EVROverlayError * pError)
{
	LOG_ENTRY("CppStubGetOverlayName");
	LOOKUP_OVERLAY_STRING(overlay_name);
	LOG_EXIT_RC(rc, "CppStubGetOverlayName");
}

uint32_t VROverlayCursor::GetOverlayRenderingPid(VROverlayHandle_t ulOverlayHandle)
{
	uint32_t rc = 0;
	int index;
	vr::EVROverlayError found_it = GetOverlayIndexForHandle(ulOverlayHandle, &index); 
	if (found_it == vr::VROverlayError_None)
	{
		SYNC_OVERLAY_STATE(key, overlays[index].overlay_rendering_pid);
		rc = key->val;
	}
	return rc;
}

#define OVERLAY_BOOL_LOOKUP(var_name)\
bool rc = 0;\
int index;\
vr::EVROverlayError found_it = GetOverlayIndexForHandle(ulOverlayHandle, &index);\
if (found_it == vr::VROverlayError_None)\
{\
	SYNC_OVERLAY_STATE(key, overlays[index].var_name);\
	rc = key->val;\
}\
return rc;

bool VROverlayCursor::IsOverlayVisible(VROverlayHandle_t ulOverlayHandle)
{
	OVERLAY_BOOL_LOOKUP(overlay_is_visible);
}

bool VROverlayCursor::IsHoverTargetOverlay(VROverlayHandle_t ulOverlayHandle)
{
	OVERLAY_BOOL_LOOKUP(overlay_is_hover_target);
}

bool VROverlayCursor::IsActiveDashboardOverlay(vr::VROverlayHandle_t ulOverlayHandle)
{
	LOG_ENTRY("CppStubIsActiveDashboardOverlay");
	OVERLAY_BOOL_LOOKUP(overlay_is_active_dashboard);
	LOG_EXIT_RC(rc, "CppStubIsActiveDashboardOverlay");
}

EVROverlayError 
VROverlayCursor::GetOverlayFlag(VROverlayHandle_t ulOverlayHandle, VROverlayFlags eOverlayFlag, bool *pbEnabled)
{
	EVROverlayError err = vr::VROverlayError_UnknownOverlay;
	bool rc = 0; 
	int index; 
	vr::EVROverlayError found_it = GetOverlayIndexForHandle(ulOverlayHandle, &index); 
	if (found_it == vr::VROverlayError_None)
	{
		SYNC_OVERLAY_STATE(key, overlays[index].overlay_flags); 
		err = key->presence;
		if (key->is_present())
		{
			int bit_pos = (int)eOverlayFlag;
			uint32_t mask = 0x1 << (bit_pos);	// weird in the lowest bit isn't used
			uint32_t flags = key->val;
			if ((mask & flags) != 0)
			{
				if (pbEnabled)
					*pbEnabled = true;
			}
			else
			{
				if (pbEnabled)
					*pbEnabled = false;
			}
		}
	}
	return err;
}

vr::EVROverlayError VROverlayCursor::GetOverlayImageData(
	vr::VROverlayHandle_t ulOverlayHandle, void * pvBuffer, uint32_t unBufferSize, uint32_t * punWidth, uint32_t * punHeight)
{
	LOG_ENTRY("CppStubGetOverlayImageData");
	
	int index; 
	vr::EVROverlayError rc = GetOverlayIndexForHandle(ulOverlayHandle, &index); 
	
	if (rc == vr::VROverlayError_None)
	{
		SYNC_OVERLAY_STATE(height, overlays[index].overlay_image_height); 
		SYNC_OVERLAY_STATE(width, overlays[index].overlay_image_width);
		SYNC_OVERLAY_STATE(data, overlays[index].overlay_image_data);

		if (data->is_present())
		{
			if (punWidth)
			{
				*punWidth = width->val;
			}
			if (punHeight)
			{
				*punHeight = height->val;
			}

			uint8_t *ptr = (uint8_t*)pvBuffer;
			if (!util_vector_to_return_buf_rc(&data->val, ptr, unBufferSize, nullptr))
			{
				rc = VROverlayError_ArrayTooSmall;
			}
		}
		else
		{
			rc = data->presence;
		}
	}

	LOG_EXIT_RC(rc, "CppStubGetOverlayImageData");
}

vr::EVROverlayError VROverlayCursor::GetOverlayColor(vr::VROverlayHandle_t ulOverlayHandle, float * pfRed, float * pfGreen, float * pfBlue)
{
	LOG_ENTRY("CppStubGetOverlayColor");
	int index;
	vr::EVROverlayError rc = GetOverlayIndexForHandle(ulOverlayHandle, &index);
	if (rc == vr::VROverlayError_None)
	{
		SYNC_OVERLAY_STATE(color, overlays[index].overlay_color);
		if (color->is_present())
		{
			if (pfRed)
			{
				*pfRed = color->val.r;		// 2/7/2017: openvr writes to pfRed only if valid
			}
			if (pfGreen)
			{
				*pfGreen = color->val.g;
			}
			if (pfBlue)
			{
				*pfBlue = color->val.b;
			}
		}
	}
	LOG_EXIT_RC(rc, "CppStubGetOverlayColor");
}

#define OVERLAY_VAL_LOOKUP(schema_name, param_name) \
int index;\
vr::EVROverlayError rc = GetOverlayIndexForHandle(ulOverlayHandle, &index);\
if (rc == vr::VROverlayError_None)\
{\
	SYNC_OVERLAY_STATE(state_val, overlays[index].schema_name);\
	if (state_val->is_present()) \
	{\
		*param_name = state_val->val;\
	}\
	rc = state_val->presence;\
}

vr::EVROverlayError VROverlayCursor::GetOverlayAlpha(vr::VROverlayHandle_t ulOverlayHandle, float * pfAlpha)
{
	LOG_ENTRY("CppStubGetOverlayAlpha");

	OVERLAY_VAL_LOOKUP(overlay_alpha, pfAlpha);
	
	LOG_EXIT_RC(rc, "CppStubGetOverlayAlpha");
}

vr::EVROverlayError VROverlayCursor::GetOverlayTexelAspect(vr::VROverlayHandle_t ulOverlayHandle, float * pfTexelAspect)
{
	LOG_ENTRY("CppStubGetOverlayTexelAspect");
	OVERLAY_VAL_LOOKUP(overlay_texel_aspect, pfTexelAspect);
	LOG_EXIT_RC(rc, "CppStubGetOverlayTexelAspect");
}

vr::EVROverlayError VROverlayCursor::GetOverlaySortOrder(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * punSortOrder)
{
	LOG_ENTRY("CppStubGetOverlaySortOrder");
	OVERLAY_VAL_LOOKUP(overlay_sort_order, punSortOrder);
	LOG_EXIT_RC(rc, "CppStubGetOverlaySortOrder");
}

vr::EVROverlayError VROverlayCursor::GetOverlayWidthInMeters(vr::VROverlayHandle_t ulOverlayHandle, float * pfWidthInMeters)
{
	LOG_ENTRY("CppStubGetOverlayWidthInMeters");
	OVERLAY_VAL_LOOKUP(overlay_width_in_meters, pfWidthInMeters);
	LOG_EXIT_RC(rc, "CppStubGetOverlayWidthInMeters");
}

// take a struct and split it into two outgoing params
#define OVERLAY_2_STRUCT_VAL_LOOKUP(schema_name, schema_field1, schema_field2, param_name1, param_name2) \
int index;\
vr::EVROverlayError rc = GetOverlayIndexForHandle(ulOverlayHandle, &index);\
if (rc == vr::VROverlayError_None)\
{\
	SYNC_OVERLAY_STATE(state_val, overlays[index].schema_name);\
	if (state_val->is_present())\
	{\
		if (param_name1)\
		{\
			*param_name1 = state_val->val.schema_field1;\
		}\
		if (param_name2)\
		{\
			*param_name2 = state_val->val.schema_field2;\
		}\
	}\
	rc = state_val->presence;\
}

vr::EVROverlayError VROverlayCursor::GetOverlayAutoCurveDistanceRangeInMeters(vr::VROverlayHandle_t ulOverlayHandle, 
	float * pfMinDistanceInMeters, float * pfMaxDistanceInMeters)
{
	LOG_ENTRY("CppStubGetOverlayAutoCurveDistanceRangeInMeters");
	OVERLAY_2_STRUCT_VAL_LOOKUP(overlay_auto_curve_range_in_meters, min, max, pfMinDistanceInMeters, pfMaxDistanceInMeters);
	LOG_EXIT_RC(rc, "CppStubGetOverlayAutoCurveDistanceRangeInMeters");
}

vr::EVROverlayError VROverlayCursor::GetOverlayTextureColorSpace(vr::VROverlayHandle_t ulOverlayHandle, vr::EColorSpace * peTextureColorSpace)
{
	LOG_ENTRY("CppStubGetOverlayTextureColorSpace");
	OVERLAY_VAL_LOOKUP(overlay_texture_color_space, peTextureColorSpace);
	LOG_EXIT_RC(rc, "CppStubGetOverlayTextureColorSpace");
}

vr::EVROverlayError VROverlayCursor::GetOverlayTextureBounds(vr::VROverlayHandle_t ulOverlayHandle, 
	struct vr::VRTextureBounds_t * pOverlayTextureBounds)
{
	LOG_ENTRY("CppStubGetOverlayTextureBounds");
	OVERLAY_VAL_LOOKUP(overlay_texture_bounds, pOverlayTextureBounds);
	LOG_EXIT_RC(rc, "CppStubGetOverlayTextureBounds");
}

vr::EVROverlayError VROverlayCursor::GetOverlayTransformType(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayTransformType * peTransformType)
{
	LOG_ENTRY("CppStubGetOverlayTransformType");
	OVERLAY_VAL_LOOKUP(overlay_transform_type, peTransformType);
	LOG_EXIT_RC(rc, "CppStubGetOverlayTransformType");
}

vr::EVROverlayError VROverlayCursor::GetOverlayTransformAbsolute(
		vr::VROverlayHandle_t ulOverlayHandle, 
		vr::ETrackingUniverseOrigin * peTrackingOrigin, 
		struct vr::HmdMatrix34_t * pmatTrackingOriginToOverlayTransform)
{
	LOG_ENTRY("CppStubGetOverlayTransformAbsolute");
	OVERLAY_2_STRUCT_VAL_LOOKUP(overlay_transform_absolute, tracking_origin, origin2overlaytransform, peTrackingOrigin, pmatTrackingOriginToOverlayTransform);
	LOG_EXIT_RC(rc, "CppStubGetOverlayTransformAbsolute");
}

vr::EVROverlayError VROverlayCursor::GetOverlayTransformTrackedDeviceRelative(
		vr::VROverlayHandle_t ulOverlayHandle, 
		vr::TrackedDeviceIndex_t * punTrackedDevice, 
		struct vr::HmdMatrix34_t * pmatTrackedDeviceToOverlayTransform)
{
	LOG_ENTRY("CppStubGetOverlayTransformTrackedDeviceRelative");
	OVERLAY_2_STRUCT_VAL_LOOKUP(overlay_transform_device_relative, 
								tracked_device, device2overlaytransform, 
								punTrackedDevice, pmatTrackedDeviceToOverlayTransform);
	LOG_EXIT_RC(rc, "CppStubGetOverlayTransformTrackedDeviceRelative");
}

vr::EVROverlayError VROverlayCursor::GetOverlayTransformTrackedDeviceComponent(
					vr::VROverlayHandle_t ulOverlayHandle, 
					vr::TrackedDeviceIndex_t * punDeviceIndex, 
					char * pchComponentName, uint32_t unComponentNameSize)
{
	LOG_ENTRY("CppStubGetOverlayTransformTrackedDeviceComponent");
	
	int index; 
	vr::EVROverlayError rc = GetOverlayIndexForHandle(ulOverlayHandle, &index); 
	if (rc == vr::VROverlayError_None)
	{
		SYNC_OVERLAY_STATE(device_index, overlays[index].overlay_transform_component_relative_device_index);
		SYNC_OVERLAY_STATE(name, overlays[index].overlay_transform_component_relative_name);

		if (device_index->is_present() && name->is_present())
		{
			if (punDeviceIndex)
			{
				*punDeviceIndex = device_index->val;
			}
			if (!util_vector_to_return_buf_rc(&name->val, pchComponentName, unComponentNameSize, nullptr))
			{
				rc = VROverlayError_ArrayTooSmall; // TODO: verify this is what happens if the buffer is too small
			}
		}
	}

	LOG_EXIT_RC(rc, "CppStubGetOverlayTransformTrackedDeviceComponent");
}


vr::EVROverlayError VROverlayCursor::GetOverlayInputMethod(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayInputMethod * peInputMethod)
{
	LOG_ENTRY("CppStubGetOverlayInputMethod");
	OVERLAY_VAL_LOOKUP(overlay_input_method, peInputMethod);
	LOG_EXIT_RC(rc, "CppStubGetOverlayInputMethod");
}

vr::EVROverlayError VROverlayCursor::GetOverlayMouseScale(vr::VROverlayHandle_t ulOverlayHandle, struct vr::HmdVector2_t * pvecMouseScale)
{
	LOG_ENTRY("CppStubGetOverlayMouseScale");
	OVERLAY_VAL_LOOKUP(overlay_mouse_scale, pvecMouseScale);
	LOG_EXIT_RC(rc, "CppStubGetOverlayMouseScale");
}


vr::VROverlayHandle_t VROverlayCursor::GetGamepadFocusOverlay()
{
	LOG_ENTRY("CppStubGetGamepadFocusOverlay");
	vr::VROverlayHandle_t rc;
	SYNC_OVERLAY_STATE(gamepad_focus_overlay, gamepad_focus_overlay);
	rc = gamepad_focus_overlay->val;
	LOG_EXIT_RC(rc, "CppStubGetGamepadFocusOverlay");
}

vr::EVROverlayError VROverlayCursor::GetOverlayTextureSize(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * pWidth, uint32_t * pHeight)
{
	LOG_ENTRY("CppStubGetOverlayTextureSize");
	OVERLAY_2_STRUCT_VAL_LOOKUP(overlay_texture_size,
		width, height,
		pWidth, pHeight);
	LOG_EXIT_RC(rc, "CppStubGetOverlayTextureSize");
}

vr::EVROverlayError VROverlayCursor::GetDashboardOverlaySceneProcess(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * punProcessId)
{
	LOG_ENTRY("CppStubGetDashboardOverlaySceneProcess");
	OVERLAY_VAL_LOOKUP(overlay_dashboard_scene_process, punProcessId);
	LOG_EXIT_RC(rc, "CppStubGetDashboardOverlaySceneProcess");
}

uint32_t VROverlayCursor::GetKeyboardText(char * pchText, uint32_t cchText)
{
	LOG_ENTRY("CppStubGetKeyboardText");
	uint32_t rc;
	SYNC_OVERLAY_STATE(keyboard_text, keyboard_text);
	util_vector_to_return_buf_rc(&keyboard_text->val, pchText, cchText, &rc);
	LOG_EXIT_RC(rc, "CppStubGetKeyboardText");
}

vr::EVROverlayError VROverlayCursor::GetOverlayFlags(vr::VROverlayHandle_t ulOverlayHandle, uint32_t * pFlags)
{
	LOG_ENTRY("CppStubGetOverlayFlags");
	OVERLAY_VAL_LOOKUP(overlay_flags, pFlags);
	LOG_EXIT_RC(rc, "CppStubGetOverlayFlags");
}

class VRRenderModelsCursor : public VRRenderModelsCppStub
{
public:
	CursorContext *m_context;
	vrstate::rendermodels_schema &state_ref;
	vriterator::rendermodels_schema &iter_ref;

	VRRenderModelsCursor(CursorContext *context) 
		: 
		m_context(context),
		state_ref(m_context->state->rendermodels_node),
		iter_ref(m_context->iterators->rendermodels_node)
	{
		// synchronize rendermodel_schema vector
		for (int model_index = 0; model_index < (int)state_ref.models.size(); model_index++)
		{
			iter_ref.models.emplace_back(m_context->m_allocator);
			while (iter_ref.models[model_index].components.size() < (int)state_ref.models[model_index].components.size())
			{
				iter_ref.models[model_index].components.emplace_back(m_context->m_allocator);
			}
		}
	}

	vr::EVRRenderModelError LoadRenderModel_Async(
				const char * pchRenderModelName, 
				struct vr::RenderModel_t ** ppRenderModel) override;

	void FreeRenderModel(struct vr::RenderModel_t * pRenderModel) override;
	
	vr::EVRRenderModelError LoadTexture_Async(	vr::TextureID_t textureId, 
												struct vr::RenderModel_TextureMap_t ** ppTexture) override;

	void FreeTexture(struct vr::RenderModel_TextureMap_t * pTexture) override;
	
	// TODO?
	vr::EVRRenderModelError LoadTextureD3D11_Async(vr::TextureID_t textureId, void * pD3D11Device, void ** ppD3D11Texture2D) override;
	vr::EVRRenderModelError LoadIntoTextureD3D11_Async(vr::TextureID_t textureId, void * pDstTexture) override;
	
	uint32_t GetRenderModelName(uint32_t unRenderModelIndex, char * pchRenderModelName, uint32_t unRenderModelNameLen) override;
	uint32_t GetRenderModelCount() override;
	uint32_t GetComponentCount(const char * pchRenderModelName) override;
	uint32_t GetComponentName(const char * pchRenderModelName, uint32_t unComponentIndex, char * pchComponentName, uint32_t unComponentNameLen) override;
	uint64_t GetComponentButtonMask(const char * pchRenderModelName, const char * pchComponentName) override;
	uint32_t GetComponentRenderModelName(const char * pchRenderModelName, const char * pchComponentName, char * pchComponentRenderModelName, uint32_t unComponentRenderModelNameLen) override;
	bool GetComponentState(const char * pchRenderModelName, const char * pchComponentName, const vr::VRControllerState_t * pControllerState, const struct vr::RenderModel_ControllerMode_State_t * pState, struct vr::RenderModel_ComponentState_t * pComponentState) override;
	bool RenderModelHasComponent(const char * pchRenderModelName, const char * pchComponentName) override;
	uint32_t GetRenderModelThumbnailURL(const char * pchRenderModelName, char * pchThumbnailURL, uint32_t unThumbnailURLLen, vr::EVRRenderModelError * peError) override;
	uint32_t GetRenderModelOriginalPath(const char * pchRenderModelName, char * pchOriginalPath, uint32_t unOriginalPathLen, vr::EVRRenderModelError * peError) override;
	const char * GetRenderModelErrorNameFromEnum(vr::EVRRenderModelError error) override;

	bool GetIndexForRenderModelName(const char *pchRenderModelName, int *index);

	bool GetIndexForRenderModelAndComponent(const char *pchRenderModelName, const char *pchComponentName,
		int *rendermodel_index, int *component_index);
	
	void GetControllerIndicesMatchingRenderModel(const char *pchRenderModelName, std::vector<int> *indices);
};

#define SYNC_RENDERMODELS_STATE(local_name, variable_name) \
auto local_name ## iter = iter_ref.variable_name;\
update_iter(local_name ## iter,\
	state_ref.variable_name,\
	m_context->current_frame);\
auto & local_name = local_name ## iter ## .item;


// TODO: look for other getindexes and see if there is a collapsible pattern
// TODO: also the syncing of vectors (SynchronizeChildVectors()) between state and cursor is also a common pattern
bool VRRenderModelsCursor::GetIndexForRenderModelName(const char *pchRenderModelName, int *index)
{
	bool rc = false;
	for (int i = 0; i < (int)iter_ref.models.size(); i++)
	{
		SYNC_RENDERMODELS_STATE(model, models[i].render_model_name);

		if (model->is_present() && util_char_vector_cmp(pchRenderModelName, model->val) == 0)
		{
			*index = i;
			rc = true;
			break;
		}
	}
	return rc;
}

bool VRRenderModelsCursor::GetIndexForRenderModelAndComponent(
			const char *pchRenderModelName, const char *pchComponentName,
			int *rendermodel_index, int *component_index)
{
	bool rc = false;
	int render_model_index;
	if (GetIndexForRenderModelName(pchRenderModelName, &render_model_index))
	{
		for (int i = 0; i < (int)iter_ref.models[render_model_index].components.size(); i++)
		{
			SYNC_RENDERMODELS_STATE(component_name, models[render_model_index].components[i].component_name);
			if (component_name->is_present() && util_char_vector_cmp(pchComponentName, component_name->val) == 0)
			{
				*rendermodel_index = render_model_index;
				*component_index = i;
				rc = true;
				break;
			}
		}
	}
	return rc;
}

vr::EVRRenderModelError VRRenderModelsCursor::LoadRenderModel_Async(const char * pchRenderModelName, 
				struct vr::RenderModel_t ** ppRenderModel)
{
	LOG_ENTRY("CppStubLoadRenderModel_Async");

	if (!ppRenderModel)
	{
		return vr::VRRenderModelError_InvalidArg;
	}

	// 1/25/2017 passing in null returns no error and a 0 value
	if (!pchRenderModelName)
	{
		ppRenderModel = nullptr;
		return vr::VRRenderModelError_None;
	}

	vr::EVRRenderModelError rc;
	int index;
	if (GetIndexForRenderModelName(pchRenderModelName, &index))
	{
		// build the return value from vertex and index data
		SYNC_RENDERMODELS_STATE(vertex_data, models[index].vertex_data);
		SYNC_RENDERMODELS_STATE(index_data, models[index].index_data);

		if (vertex_data->is_present())
		{
			vr::RenderModel_t *m = new RenderModel_t;						// allocation to return to apps. caller calls FreeRenderModel
			m->rIndexData = &index_data->val.at(0);
			m->unTriangleCount = index_data->val.size() / 3;
			m->rVertexData = &vertex_data->val.at(0);
			m->unVertexCount = vertex_data->val.size();
			m->diffuseTextureId = index + 1000;	// we'll fake out the texture ids as indices
			*ppRenderModel = m;
		}
		rc = vertex_data->presence;
	}
	else
	{
		// test shows this is what is returned for invalid names
		// (eventually - it returns Loading and then the final result)
		 rc = vr::VRRenderModelError_InvalidModel;												
	}

	LOG_EXIT_RC(rc, "CppStubLoadRenderModel_Async");
}

void VRRenderModelsCursor::FreeRenderModel(struct vr::RenderModel_t * pRenderModel)
{
	LOG_ENTRY("CppStubFreeRenderModel");
	
	if (pRenderModel)
	{
		delete pRenderModel;
	}

	LOG_EXIT("CppStubFreeRenderModel");
}

vr::EVRRenderModelError VRRenderModelsCursor::LoadTexture_Async(
					vr::TextureID_t textureId, 
					struct vr::RenderModel_TextureMap_t ** ppTexture)
{
	LOG_ENTRY("CppStubLoadTexture_Async");
	
	if (!ppTexture)
	{
		return vr::VRRenderModelError_InvalidArg;
	}

	vr::EVRRenderModelError rc;
	int index = (int)textureId - 1000;
	if (index >= 0 && index < (int)iter_ref.models.size())
	{
		// build the return value from vertex and index data
		SYNC_RENDERMODELS_STATE(texture_width, models[index].texture_width);
		SYNC_RENDERMODELS_STATE(texture_height, models[index].texture_height);
		SYNC_RENDERMODELS_STATE(texture_map_data, models[index].texture_map_data);

		if (texture_map_data->is_present())
		{
			RenderModel_TextureMap_t *t = new RenderModel_TextureMap_t;
			t->unWidth = texture_width->val;
			t->unHeight = texture_height->val;
			t->rubTextureMapData = &texture_map_data->val.at(0);
			*ppTexture = t;
		}
		rc = texture_map_data->presence;
	}
	else
	{
		rc = VRRenderModelError_InvalidTexture;
	}

	LOG_EXIT_RC(rc, "CppStubLoadTexture_Async");
}

void VRRenderModelsCursor::FreeTexture(struct vr::RenderModel_TextureMap_t * pTexture)
{
	LOG_ENTRY("CppStubFreeTexture");
	if (pTexture)
	{
		delete pTexture;
	}
	LOG_EXIT("CppStubFreeTexture");
}

vr::EVRRenderModelError VRRenderModelsCursor::LoadTextureD3D11_Async(
		vr::TextureID_t textureId, void * pD3D11Device, void ** ppD3D11Texture2D)
{
	LOG_ENTRY("CppStubLoadTextureD3D11_Async");
	assert(0);	// todo
	vr::EVRRenderModelError rc = vr::VRRenderModelError_None;

	LOG_EXIT_RC(rc, "CppStubLoadTextureD3D11_Async");
}

vr::EVRRenderModelError VRRenderModelsCursor::LoadIntoTextureD3D11_Async(
			vr::TextureID_t textureId, void * pDstTexture)
{
	LOG_ENTRY("CppStubLoadIntoTextureD3D11_Async");
	assert(0);	// todo
	static vr::EVRRenderModelError rc;
	LOG_EXIT_RC(rc, "CppStubLoadIntoTextureD3D11_Async");
}

uint32_t VRRenderModelsCursor::GetRenderModelName(
		uint32_t unRenderModelIndex, char * pchRenderModelName, uint32_t unRenderModelNameLen)
{
	LOG_ENTRY("CppStubGetRenderModelName");

	uint32_t rc = 0; 
	if (unRenderModelIndex >= 0 && unRenderModelIndex < iter_ref.models.size())
	{
		SYNC_RENDERMODELS_STATE(model, models[unRenderModelIndex].render_model_name);
		if (model->is_present())
		{
			util_vector_to_return_buf_rc(&model->val, pchRenderModelName, unRenderModelNameLen, &rc);
		}
	}

	LOG_EXIT_RC(rc, "CppStubGetRenderModelName");
}

uint32_t VRRenderModelsCursor::GetRenderModelCount()
{
	LOG_ENTRY("CppStubGetRenderModelCount");
	uint32_t rc = iter_ref.models.size();
	LOG_EXIT_RC(rc, "CppStubGetRenderModelCount");
}

uint32_t VRRenderModelsCursor::GetComponentCount(const char * pchRenderModelName)
{
	LOG_ENTRY("CppStubGetComponentCount");

	uint32_t rc = 0;
	int index;
	if (GetIndexForRenderModelName(pchRenderModelName, &index))
	{
		rc = iter_ref.models[index].components.size();
	}
	
	LOG_EXIT_RC(rc, "CppStubGetComponentCount");
}


uint32_t VRRenderModelsCursor::GetComponentName(
		const char * pchRenderModelName, 
		uint32_t unComponentIndex, char * pchComponentName, uint32_t unComponentNameLen)
{
	LOG_ENTRY("CppStubGetComponentName");
	
	uint32_t rc = 0;
	int model_index;
	if (GetIndexForRenderModelName(pchRenderModelName, &model_index))
	{
		if (unComponentIndex >= 0 && unComponentIndex < iter_ref.models[model_index].components.size())
		{
			SYNC_RENDERMODELS_STATE(component_name, models[model_index].components[unComponentIndex].component_name);
			util_vector_to_return_buf_rc(&component_name->val, pchComponentName, unComponentNameLen, &rc);
		}	
	}
	LOG_EXIT_RC(rc, "CppStubGetComponentName");
}

uint64_t VRRenderModelsCursor::GetComponentButtonMask(
			const char * pchRenderModelName, 
			const char * pchComponentName)
{
	LOG_ENTRY("CppStubGetComponentButtonMask");
	
	uint64_t rc = 0;
	int model_index;
	int component_index;
	if (GetIndexForRenderModelAndComponent(pchRenderModelName, pchComponentName, &model_index, &component_index))
	{
		SYNC_RENDERMODELS_STATE(button_mask, models[model_index].components[component_index].button_mask);
		if (button_mask->is_present())
		{
			rc = button_mask->val;
		}
	}

	LOG_EXIT_RC(rc, "CppStubGetComponentButtonMask");
}

uint32_t VRRenderModelsCursor::GetComponentRenderModelName(
		const char * pchRenderModelName, 
		const char * pchComponentName, char * pchComponentRenderModelName, uint32_t unComponentRenderModelNameLen)
{
	LOG_ENTRY("CppStubGetComponentRenderModelName");
	
	uint32_t rc = 0;
	int model_index;
	int component_index;
	if (GetIndexForRenderModelAndComponent(pchRenderModelName, pchComponentName, &model_index, &component_index))
	{
		SYNC_RENDERMODELS_STATE(render_model_name, models[model_index].components[component_index].render_model_name);

		if (render_model_name->is_present())
		{
			util_vector_to_return_buf_rc(&render_model_name->val, pchComponentRenderModelName, unComponentRenderModelNameLen, &rc);
		}
	}

	LOG_EXIT_RC(rc, "CppStubGetComponentRenderModelName");
}

// let the render model cursor poke at the system state cursors
#define REMI_SYNC_SYSTEM_STATE(local_name, variable_name) \
auto local_name ## iter = m_context->iterators->system_node.variable_name;\
update_iter(local_name ## iter,\
	m_context->state->system_node.variable_name,\
	m_context->current_frame);\
auto & local_name = local_name ## iter ## .item;

void VRRenderModelsCursor::GetControllerIndicesMatchingRenderModel(
	const char *pchRenderModelName, std::vector<int> *controller_indices)
{
	vrstate::system_schema		&system_node = m_context->state->system_node;
	vriterator::system_schema	&system_iter  = m_context->iterators->system_node;

	int prop_index = - 1;

	for (int i = 0; i < (int)system_iter.controllers.size(); i++)
	{
		if (prop_index == -1)
		{
			prop_index = system_iter.controllers[i].string_props.get_index_for_property(Prop_RenderModelName_String);
		}
		REMI_SYNC_SYSTEM_STATE(render_model_name, controllers[i].string_props.props[prop_index]);
		if (util_char_vector_cmp(pchRenderModelName, render_model_name->val) == 0)
		{
			controller_indices->push_back(i);
		}
	}
}

bool VRRenderModelsCursor::GetComponentState(
			const char * pchRenderModelName, 
			const char * pchComponentName, 
			const vr::VRControllerState_t * pControllerState,					// input
			const struct vr::RenderModel_ControllerMode_State_t * pState,		// input
			struct vr::RenderModel_ComponentState_t * pComponentState)          // return value
{
	LOG_ENTRY("CppStubGetComponentState");
	bool rc = false;
	if (pchRenderModelName && pchComponentName && pControllerState && pState && pComponentState)
	{
		// TODO / Review the following after its up and running and think if there is a better way
		//
		// GetComponentState is close to a pure function. its parameteized by, controller state,
		// and pstate.  the unpure part is that it depends on the component.  Because of the pstate and controller state
		// parameters it is not possible to capture all possible return values.
		// 
		// The 'reasonable' thing that is captured is the last active controller
		// states.  Because that is likely to be of interest.
		//
		// So currently the behaviour is to find all the controllers with this model and component name
		// and choose the one with the closest controller state
		//  

		std::vector<int> controller_indices;
		GetControllerIndicesMatchingRenderModel(pchRenderModelName, &controller_indices);

		if (controller_indices.size() > 0)
		{
			int best_controller_index = controller_indices[0];
			if (controller_indices.size() > 1)
			{
				int best_score = std::numeric_limits<int>::max();
				for (int i = 0; i < (int)controller_indices.size() && best_score != 0; i++)
				{
					int controller_index = controller_indices[i];
					REMI_SYNC_SYSTEM_STATE(controller_state, controllers[controller_index].controller_state);
					int cur_score = softcompare_controllerstates(pControllerState, &controller_state->val);
					if (cur_score < best_score)
					{
						best_controller_index = controller_index;
						best_score = cur_score;
					}
				}
			}
			// ok now got controller, return the captured component state	
			// what's the component index again?

			int render_model_index;
			int component_index;

			if (GetIndexForRenderModelAndComponent(pchRenderModelName, pchComponentName,
				&render_model_index, &component_index))
			{
				if (pState->bScrollWheelVisible)
				{
					REMI_SYNC_SYSTEM_STATE(transforms, controllers[best_controller_index].components[component_index].transforms_scroll_wheel);
					if (transforms->is_present())
					{
						*pComponentState = transforms->val;
						rc = true;
					}
				}
				else
				{
					REMI_SYNC_SYSTEM_STATE(transforms, controllers[best_controller_index].components[component_index].transforms);
					if (transforms->is_present())
					{
						*pComponentState = transforms->val;
						rc = true;
					}
				}
			}
		}
	}
	LOG_EXIT_RC(rc, "CppStubGetComponentState");
}

bool VRRenderModelsCursor::RenderModelHasComponent(const char * pchRenderModelName, const char * pchComponentName)
{
	LOG_ENTRY("CppStubRenderModelHasComponent");

	bool rc = false;
	int model_index;
	int component_index;
	if (GetIndexForRenderModelAndComponent(pchRenderModelName, pchComponentName, &model_index, &component_index))
	{
		rc = true;
	}

	LOG_EXIT_RC(rc, "CppStubRenderModelHasComponent");
}

uint32_t VRRenderModelsCursor::GetRenderModelThumbnailURL(
		const char * pchRenderModelName, 
		char * pchThumbnailURL, 
		uint32_t unThumbnailURLLen, vr::EVRRenderModelError * peError)
{
	LOG_ENTRY("CppStubGetRenderModelThumbnailURL");
	
	uint32_t rc = 1;	// 1/26/2017 - default is to return a 1 for empty string
	if (unThumbnailURLLen > 0)
	{
		pchThumbnailURL[0] = '\0';
	}
	int model_index;
	if (GetIndexForRenderModelName(pchRenderModelName, &model_index))
	{
		SYNC_RENDERMODELS_STATE(url, models[model_index].thumbnail_url);
		if (url->is_present())
		{
			util_vector_to_return_buf_rc(&url->val, pchThumbnailURL, unThumbnailURLLen, &rc);	
		}
		if (peError)
		{
			*peError = url->presence;
		}
	}
	else
	{
		if (peError)
		{
			*peError = VRRenderModelError_InvalidModel;
		}
	}

	LOG_EXIT_RC(rc, "CppStubGetRenderModelThumbnailURL");
}

uint32_t VRRenderModelsCursor::GetRenderModelOriginalPath(
			const char * pchRenderModelName, 
			char * pchOriginalPath, 
			uint32_t unOriginalPathLen, 
			vr::EVRRenderModelError * peError)
{
	LOG_ENTRY("CppStubGetRenderModelOriginalPath");

	uint32_t rc = 0;
	int model_index;
	if (GetIndexForRenderModelName(pchRenderModelName, &model_index))
	{
		SYNC_RENDERMODELS_STATE(original_path, models[model_index].original_path);
		if (original_path->is_present())
		{
			util_vector_to_return_buf_rc(&original_path->val, pchOriginalPath, unOriginalPathLen, &rc);
		}
		if (peError)
		{
			*peError = original_path->presence;
		}
	}
	else
	{
		if (peError)
		{
			*peError = VRRenderModelError_InvalidModel;
		}
	}
	
	LOG_EXIT_RC(rc, "CppStubGetRenderModelOriginalPath");
}

const char * VRRenderModelsCursor::GetRenderModelErrorNameFromEnum(vr::EVRRenderModelError error)
{
	LOG_ENTRY("CppStubGetRenderModelErrorNameFromEnum");
	const char * rc = openvr_string::EVRRenderModelErrorToString(error);
	LOG_EXIT_RC(rc, "CppStubGetRenderModelErrorNameFromEnum");
}


class VRExtendedDisplayCursor : public VRExtendedDisplayCppStub
{
public:
	CursorContext *m_context;
	vrstate::extendeddisplay_schema &state_ref;
	vriterator::extendeddisplay_schema &iter_ref;

	VRExtendedDisplayCursor(CursorContext *context)
		:
		m_context(context),
		state_ref(m_context->state->extendeddisplay_node),
		iter_ref(m_context->iterators->extendeddisplay_node)
	{
	}

	void GetWindowBounds(int32_t * pnX, int32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight) override;
	void GetEyeOutputViewport(vr::EVREye eEye, uint32_t * pnX, uint32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight) override;
};

#define SYNC_EXTENDEDDISPLAY_STATE(local_name, variable_name) \
auto local_name ## iter = iter_ref.variable_name;\
update_iter(local_name ## iter,\
	state_ref.variable_name,\
	m_context->current_frame);\
auto & local_name = local_name ## iter ## .item;


void VRExtendedDisplayCursor::GetWindowBounds(int32_t * pnX, int32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight)
{
	LOG_ENTRY("CppStubGetWindowBounds");

	SYNC_EXTENDEDDISPLAY_STATE(window_bounds, window_bounds);

	// edi->GetWindowBounds(&ret.val.i[0], &ret.val.i[1], &ret.val.u[0], &ret.val.u[1]);

	if (pnX)
	{
		*pnX = window_bounds->val.i[0];
	}

	if (pnY)
	{
		*pnY = window_bounds->val.i[1];
	}

	if (pnWidth)
	{
		*pnWidth = window_bounds->val.u[0];
	}

	if (pnHeight)
	{
		*pnHeight = window_bounds->val.u[1];
	}

	LOG_EXIT("CppStubGetWindowBounds");
}

void VRExtendedDisplayCursor::GetEyeOutputViewport(vr::EVREye eEye, uint32_t * pnX, uint32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight)
{
	LOG_ENTRY("CppStubGetEyeOutputViewport");

	ViewPort_t *vp;
	if (eEye == vr::Eye_Left)
	{
		SYNC_EXTENDEDDISPLAY_STATE(left_output_viewport, left_output_viewport);
		vp = &left_output_viewport->val;
	}
	else
	{
		SYNC_EXTENDEDDISPLAY_STATE(right_output_viewport, right_output_viewport);
		vp = &right_output_viewport->val;
	}

	if (pnX)
	{
		*pnX = vp->u[0];
	}

	if (pnY)
	{
		*pnY = vp->u[1];
	}

	if (pnWidth)
	{
		*pnWidth = vp->u[2];
	}

	if (pnHeight)
	{
		*pnHeight = vp->u[3];
	}

	LOG_EXIT("CppStubGetEyeOutputViewport");
}

class VRTrackedCameraCursor : public VRTrackedCameraCppStub
{
public:
	CursorContext *m_context;
	vrstate::trackedcamera_schema &state_ref;
	vriterator::trackedcamera_schema &iter_ref;

	VRTrackedCameraCursor(CursorContext *context)
		:
		m_context(context),
		state_ref(m_context->state->trackedcamera_node),
		iter_ref(m_context->iterators->trackedcamera_node)
	{
		for (int controller_index = 0; controller_index < (int)state_ref.controllers.size(); controller_index++)
		{
			iter_ref.controllers.emplace_back(m_context->m_allocator);
			for (int frame_type_index = 0; frame_type_index < (int)state_ref.controllers[controller_index].cameraframetypes.size();
				frame_type_index++)
			{
				iter_ref.controllers[controller_index].cameraframetypes.emplace_back(m_context->m_allocator);
			}
		}
	}

	const char * GetCameraErrorNameFromEnum(vr::EVRTrackedCameraError eCameraError) override;
	vr::EVRTrackedCameraError HasCamera(vr::TrackedDeviceIndex_t nDeviceIndex, bool * pHasCamera) override;
	vr::EVRTrackedCameraError GetCameraFrameSize(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType eFrameType, uint32_t * pnWidth, uint32_t * pnHeight, uint32_t * pnFrameBufferSize) override;
	vr::EVRTrackedCameraError GetCameraIntrinsics(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType eFrameType, vr::HmdVector2_t * pFocalLength, vr::HmdVector2_t * pCenter) override;
	vr::EVRTrackedCameraError GetCameraProjection(vr::TrackedDeviceIndex_t nDeviceIndex, vr::EVRTrackedCameraFrameType eFrameType, float flZNear, float flZFar, vr::HmdMatrix44_t * pProjection) override;
};

#define SYNC_TRACKEDCAMERA_STATE(local_name, variable_name) \
auto local_name ## iter = iter_ref.variable_name;\
update_iter(local_name ## iter,\
	state_ref.variable_name,\
	m_context->current_frame);\
auto & local_name = local_name ## iter ## .item;

const char * VRTrackedCameraCursor::GetCameraErrorNameFromEnum(vr::EVRTrackedCameraError eCameraError)
{
	LOG_ENTRY("CursorGetCameraErrorNameFromEnum");

	const char * rc = openvr_string::EVRTrackedCameraErrorToString(eCameraError);

	LOG_EXIT_RC(rc, "CursorGetCameraErrorNameFromEnum");
}

vr::EVRTrackedCameraError VRTrackedCameraCursor::HasCamera(vr::TrackedDeviceIndex_t nDeviceIndex, bool * pHasCamera)
{
	LOG_ENTRY("CursorHasCamera");

	vr::EVRTrackedCameraError rc; 
	bool has_camera_ret = false;

	if (nDeviceIndex < iter_ref.controllers.size())
	{
		SYNC_TRACKEDCAMERA_STATE(has_camera, controllers[nDeviceIndex].has_camera);
		if (has_camera->is_present())
		{
			has_camera_ret = has_camera->val;
		}
		rc = has_camera->presence;
	}
	else
	{
		rc = VRTrackedCameraError_NotSupportedForThisDevice; // 1/26/2017 - invalid device index returns this and sets pHasCamera to false
	}

	if (pHasCamera)
	{
		*pHasCamera = has_camera_ret;
	}

	LOG_EXIT_RC(rc, "CursorHasCamera");
}

vr::EVRTrackedCameraError VRTrackedCameraCursor::GetCameraFrameSize(
	vr::TrackedDeviceIndex_t nDeviceIndex, 
	vr::EVRTrackedCameraFrameType eFrameType, uint32_t * pnWidth, uint32_t * pnHeight, uint32_t * pnFrameBufferSize)
{
	LOG_ENTRY("CursorGetCameraFrameSize");

	vr::EVRTrackedCameraError rc;
	if (nDeviceIndex < iter_ref.controllers.size())
	{
		SYNC_TRACKEDCAMERA_STATE(size, controllers[nDeviceIndex].cameraframetypes[(int)eFrameType].frame_size);
		
		// 2/10/2017: regardless of presence, write the stuffed value back to the caller
		//if (size->is_present())
		{
			if (pnWidth)
				*pnWidth = size->val.width;
			if (pnHeight)
				*pnHeight = size->val.height;
			if (pnFrameBufferSize)
				*pnFrameBufferSize = size->val.size;
		}
		rc = size->presence;
	}
	else
	{
		rc = VRTrackedCameraError_NotSupportedForThisDevice;
	}

	LOG_EXIT_RC(rc, "CursorGetCameraFrameSize");
}

vr::EVRTrackedCameraError VRTrackedCameraCursor::GetCameraIntrinsics(
		vr::TrackedDeviceIndex_t nDeviceIndex, 
		vr::EVRTrackedCameraFrameType eFrameType, 
		vr::HmdVector2_t * pFocalLength, 
		vr::HmdVector2_t * pCenter)
{
	LOG_ENTRY("CursorGetCameraIntrinsics");
	vr::EVRTrackedCameraError rc;
	if (nDeviceIndex < iter_ref.controllers.size())
	{
		SYNC_TRACKEDCAMERA_STATE(intrinsics, controllers[nDeviceIndex].cameraframetypes[(int)eFrameType].intrinsics);
		if (intrinsics->is_present())
		{
			if (pFocalLength)
				*pFocalLength = intrinsics->val.focal_length;
			if (pCenter)
				*pCenter = intrinsics->val.center;
		}
		rc = intrinsics->presence;
	}
	else
	{
		rc = VRTrackedCameraError_NotSupportedForThisDevice;
	}
	LOG_EXIT_RC(rc, "CursorGetCameraIntrinsics");
}

vr::EVRTrackedCameraError VRTrackedCameraCursor::GetCameraProjection(vr::TrackedDeviceIndex_t nDeviceIndex, 
	vr::EVRTrackedCameraFrameType eFrameType, float flZNear, float flZFar, vr::HmdMatrix44_t * pProjection)
{
	LOG_ENTRY("CursorGetCameraProjection");
	vr::EVRTrackedCameraError rc;
	if (nDeviceIndex < iter_ref.controllers.size())
	{
		SYNC_TRACKEDCAMERA_STATE(projection, controllers[nDeviceIndex].cameraframetypes[(int)eFrameType].projection);
		if (projection->is_present())
		{
			if (pProjection)
				*pProjection = projection->val;
		}
		rc = projection->presence;
	}
	else
	{
		rc = VRTrackedCameraError_NotSupportedForThisDevice;
	}
	LOG_EXIT_RC(rc, "CursorGetCameraProjection");
}

class VRScreenshotsCursor : public VRScreenshotsCppStub
{
public:
	VRScreenshotsCursor(CursorContext *context)
	{}
};

class VRResourcesCursor : public VRResourcesCppStub
{
	CursorContext *m_context;
	vrstate::resources_schema &state_ref;
	vriterator::resources_schema &iter_ref;

public:
	VRResourcesCursor(CursorContext *context)
		:
		m_context(context),
		state_ref(m_context->state->resources_node),
		iter_ref(m_context->iterators->resources_node)
	{}

	void SynchronizeChildVectors()
	{
		while (iter_ref.resources.size() < state_ref.resources.size())
		{
			iter_ref.resources.emplace_back(m_context->m_allocator);
		}
	}

	bool GetIndexForResourceName(const char *pchResourceName, int *index);
	bool GetIndexForResourceNameAndDirectory(const char *pchResourceName, const char *pchDirectoryName, int *index);
	bool GetIndexForFullPath(const char *pchResourceName, int *index);
	bool GetIndexWithFallbacks(const char * pchResourceName, int *index);

	uint32_t LoadSharedResource(const char * pchResourceName, char * pchBuffer, uint32_t unBufferLen) override;
	uint32_t GetResourceFullPath(const char * pchResourceName, const char * pchResourceTypeDirectory, char * pchPathBuffer, uint32_t unBufferLen) override;
};

#define SYNC_RESOURCE_STATE(local_name, variable_name) \
SynchronizeChildVectors();\
auto local_name ## iter = iter_ref.variable_name;\
update_iter(local_name ## iter,\
	state_ref.variable_name,\
	m_context->current_frame);\
auto & local_name = local_name ## iter ## .item;

bool VRResourcesCursor::GetIndexForResourceName(const char *pchResourceName, int *index)
{
	bool rc = false;
	for (int i = 0; i < (int)iter_ref.resources.size(); i++)
	{
		SYNC_RESOURCE_STATE(name, resources[i].resource_name);

		if (name->is_present() && util_char_vector_cmp(pchResourceName, name->val) == 0)
		{
			*index = i;
			rc = true;
			break;
		}
	}
	return rc;
}

bool VRResourcesCursor::GetIndexForResourceNameAndDirectory(
		const char *pchResourceName, 
		const char *pchDirectoryName, int *index)
{
	bool rc = false;
	SynchronizeChildVectors(); // dont erase this since this is a lookup 
	for (int i = 0; i < (int)iter_ref.resources.size(); i++)
	{
		SYNC_RESOURCE_STATE(name, resources[i].resource_name);
		SYNC_RESOURCE_STATE(directory, resources[i].resource_directory);

		if ( (name->is_present() && util_char_vector_cmp(pchResourceName, name->val) == 0) &&
			 (directory->is_present() && util_char_vector_cmp(pchDirectoryName, directory->val) == 0))
		{
			*index = i;
			rc = true;
			break;
		}
	}
	return rc;
}

// search based on the full path
bool VRResourcesCursor::GetIndexForFullPath(const char *pchResourceName, int *index)
{
	bool rc = false;
	SynchronizeChildVectors();
	for (int i = 0; i < (int)iter_ref.resources.size(); i++)
	{
		SYNC_RESOURCE_STATE(full, resources[i].resource_full_path);

		if ((full->is_present() && util_char_vector_cmp(pchResourceName, full->val) == 0))
		{
			*index = i;
			rc = true;
			break;
		}
	}
	return rc;
}

bool VRResourcesCursor::GetIndexWithFallbacks(const char * pchResourceName, int *index_ret)
{
	bool rc = false;

	if (GetIndexForResourceName(pchResourceName, index_ret))
	{
		rc = true;
	}
	
	if (rc == false)
	{
		// resource name might have a directory embedded in it: e.g. "icons/banana.txt"
		const char *slash = nullptr;
		slash = strchr(pchResourceName, '/');
		if (!slash)
		{
			slash = strchr(pchResourceName, '\\');
		}
		if (slash)
		{
			std::string dir(pchResourceName, slash - pchResourceName);
			if (GetIndexForResourceNameAndDirectory(slash+1, dir.c_str(), index_ret))
			{
				rc = true;
			}
		}
	}

	// might be the full path
	if (rc == false)
	{
		rc = GetIndexForFullPath(pchResourceName, index_ret);
	}
	
	return rc;
}


uint32_t VRResourcesCursor::LoadSharedResource(const char * pchResourceName, char * pchBuffer, uint32_t unBufferLen)
{
	LOG_ENTRY("CursorLoadSharedResource");

	int resource_index;
	uint32_t rc = 0; 
	if (GetIndexWithFallbacks(pchResourceName, &resource_index))
	{
		SYNC_RESOURCE_STATE(data, resources[resource_index].resource_data);
		if (data->is_present())
		{
			util_vector_to_return_buf_rc(&data->val, (uint8_t*)pchBuffer, unBufferLen, &rc);
		}
	}

	LOG_EXIT_RC(rc, "CursorLoadSharedResource");
}

uint32_t VRResourcesCursor::GetResourceFullPath(const char * pchResourceName, const char * pchResourceTypeDirectory, 
	char * pchPathBuffer, uint32_t unBufferLen)
{
	LOG_ENTRY("CursorGetResourceFullPath");
	
	int resource_index;
	uint32_t rc = 0;
	if (GetIndexForResourceNameAndDirectory(pchResourceName, pchResourceTypeDirectory, &resource_index))
	{
		SYNC_RESOURCE_STATE(data, resources[resource_index].resource_full_path);
		if (data->is_present())
		{
			util_vector_to_return_buf_rc(&data->val, pchPathBuffer, unBufferLen, &rc);
		}
	}
	
	LOG_EXIT_RC(rc, "CursorGetResourceFullPath");
}

//
// VRcursor has:
//		a pointer to the state tracker
//      a set of iterators into that state tracker.
//
//
// There can be more than one cursor pointing into the same state tracker
//
struct VRCursor
{
	VRCursor(tracker *tracker, ALLOCATOR_DECL)
		:
		iterators(allocator),
		m_context(tracker->m_frame_number, &iterators, &tracker->m_state, &tracker->additional_resource_keys, allocator),
		m_system_cursor(&m_context),
		m_applications_cursor(&m_context),
		m_settings_cursor(&m_context),
		m_chaperone_cursor(&m_context),
		m_chaperone_setup_cursor(&m_context),
		m_compositor_cursor(&m_context),
		m_notifications_cursor(&m_context),
		m_overlay_cursor(&m_context),
		m_rendermodels_cursor(&m_context),
		m_extendeddisplay_cursor(&m_context),
		m_trackedcamera_cursor(&m_context),
		m_screenshots_cursor(&m_context),
		m_resources_cursor(&m_context)
	{
		m_interfaces.sysi = &m_system_cursor;
		m_interfaces.appi = &m_applications_cursor;
		m_interfaces.seti = &m_settings_cursor;
		m_interfaces.chapi = &m_chaperone_cursor;
		m_interfaces.chapsi = &m_chaperone_setup_cursor;
		m_interfaces.compi = &m_compositor_cursor;
		m_interfaces.noti = &m_notifications_cursor;
		m_interfaces.ovi = &m_overlay_cursor;
		m_interfaces.remi = &m_rendermodels_cursor;
		m_interfaces.exdi = &m_extendeddisplay_cursor;	
		m_interfaces.taci = &m_trackedcamera_cursor;
		m_interfaces.screeni = &m_screenshots_cursor;
		m_interfaces.resi = &m_resources_cursor;
	}

	vriterator iterators;
	CursorContext m_context;
		
	VRSystemCursor			m_system_cursor;
	VRApplicationsCursor	m_applications_cursor;
	VRSettingsCursor		m_settings_cursor;
	VRChaperoneCursor		m_chaperone_cursor;
	VRChaperoneSetupCursor	m_chaperone_setup_cursor;
	VRCompositorCursor		m_compositor_cursor;
	VRNotificationsCursor	m_notifications_cursor;
	VROverlayCursor			m_overlay_cursor;
	VRRenderModelsCursor	m_rendermodels_cursor;
	VRExtendedDisplayCursor m_extendeddisplay_cursor;
	VRTrackedCameraCursor	m_trackedcamera_cursor;
	VRScreenshotsCursor		m_screenshots_cursor;
	VRResourcesCursor       m_resources_cursor;

	openvr_broker::open_vr_interfaces m_interfaces;
};

vr_cursor_t create_cursor(vr_state_tracker_t h)
{
	tracker *s = static_cast<tracker*>(h);
	VRCursor *cursor = new VRCursor(s, s->m_allocator);
	return cursor;
}

void set_cursor_frame(vr_cursor_t h, int framenumber)
{
	VRCursor *cursor = static_cast<VRCursor*>(h);
	cursor->m_context.current_frame = framenumber;
}

openvr_broker::open_vr_interfaces &get_cursor_interfaces(vr_cursor_t h)
{
	VRCursor *cursor = static_cast<VRCursor*>(h);
	return cursor->m_interfaces;
}


void destroy_cursor(vr_cursor_t h)
{
	VRCursor *cursor = static_cast<VRCursor*>(h);
	delete cursor;
}

// END OF CURSOR TEST
// END OF CURSOR TEST
// END OF CURSOR TEST

// public interface
void capture_vr_state(vr_state_tracker_t h, openvr_broker::open_vr_interfaces &interfaces)
{
	tracker *s = static_cast<tracker*>(h);

	if (s->update_mutex.try_lock())
	{
		// got lock;
		s->non_blocking_update_calls++;
	}
	else
	{
		s->blocking_update_calls++;
		s->update_mutex.lock();
	}

	update_timestamp(s);
	update_history_visit_fn visitor(s->m_frame_number);
	traverse_history_graph_sequential(visitor, s, interfaces);
	s->update_mutex.unlock();
}


void get_frame_range(vr_state_tracker_t h, int *first_frame, int *last_frame)
{
	tracker *s = static_cast<tracker*>(h);
	// frame 0 is empty frame - nothing there

	*first_frame = 1;
	*last_frame = s->m_frame_number;
}


// public interface
void capture_vr_event(vr_state_tracker_t h, const VREvent_t &event_in)
{
	tracker *s = static_cast<tracker*>(h);
	update_timestamp(s);
	s->m_events.emplace_front(s->m_frame_number, event_in);
}

void capture_vr_overlay_event(vr_state_tracker_t h, vr::VROverlayHandle_t overlay_handle, const VREvent_t &event_in)
{
	tracker *s = static_cast<tracker*>(h);
	update_timestamp(s);

	// helper and overlay must be present or I can't insert the events
	int index = s->additional_resource_keys.GetOverlayIndexer().get_index_for_handle(overlay_handle);
	assert(index >= 0);
	s->m_state.overlay_node.overlays[index].events.item.emplace_front(s->m_frame_number, event_in);
}


// public interface
void dump_vr_history(vr_state_tracker_t h)
{
	tracker *s = static_cast<tracker*>(h);
	dump_history_visit_fn visitor(std::cout);
	traverse_history_graph_sequential(visitor, s);
}

// file format starts with a header:
struct header
{
	uint64_t magic;
	uint64_t summary_offset;
	uint64_t summary_size;
	uint64_t state_offset;
	uint64_t state_size;
	uint64_t resource_keys_offset;
	uint64_t resource_keys_size;
	uint64_t event_offset;
	uint64_t event_size;
	uint64_t event_count;
	uint64_t timestamp_offset;
	uint64_t timestamp_size;
	uint64_t timestamp_count;
};

// public interface
void save_vrstate_to_file(vr_state_tracker_t h, const char *filename, bool binary)
{
	tracker *s = static_cast<tracker*>(h);

	if (binary)
	{
		s->update_mutex.lock();
		EncodeStream count_stream(nullptr, 0, true);
		encoder_visitor counter(&count_stream);

		// count state size
		traverse_history_graph_sequential(counter, s);
		uint64_t state_size = count_stream.buf_pos+1;
		// pad it
		uint64_t padded_state_size = (state_size + 3) & ~0x3;

		//  
		uint64_t resource_keys_size = s->additional_resource_keys.GetEncodedSize();
		uint64_t padded_resource_keys_size = (resource_keys_size + 3) & ~0x3;				

		// count event size
		int num_events = 0;
		for (auto iter = s->m_events.begin(); iter != s->m_events.end(); iter++)
		{
			num_events++;
		}
		int event_size = num_events * sizeof(FrameNumberedEvent);
		uint64_t padded_event_size = (event_size + 3) & ~0x3;
		
		// count timestamps size
		int num_timestamps = s->m_frame_number;
		int timestamp_size = num_timestamps * sizeof(uint64_t);
		
		// allocate a buffer
		uint64_t total_size =	sizeof(header) + 
								sizeof(save_summary) + 
								padded_state_size + 
								padded_resource_keys_size +
								padded_event_size +
								timestamp_size;
		assert(size_t(total_size) == total_size);
		char *buf = (char*)malloc(static_cast<size_t>(total_size));

		//
		// write header
		//
		header* h = (header *)buf;
		h->magic = 7;
		h->summary_offset = sizeof(header);
		h->summary_size = sizeof(save_summary);
		h->state_offset = sizeof(save_summary) + h->summary_offset;
		h->state_size = state_size;

		h->resource_keys_offset = h->state_offset + padded_state_size;
		h->resource_keys_size = resource_keys_size;

		h->event_count = num_events;
		h->event_size = event_size;
		h->event_offset = h->resource_keys_offset + padded_resource_keys_size;

		h->timestamp_count = num_timestamps;
		h->timestamp_size = timestamp_size;
		h->timestamp_offset = h->event_offset + padded_event_size;

		//
		// write summary
		// 
		save_summary *sum = (save_summary*)(buf + h->summary_offset);
		sum->num_frames = s->m_frame_number;
		std::time_t start = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		errno_t e = ctime_s(sum->date_string, sizeof(sum->date_string), &start);
		assert(e == 0);

		//
		// write state
		// 
		EncodeStream state_stream(buf + h->state_offset, state_size, false);
		encoder_visitor encoder(&state_stream);
		traverse_history_graph_sequential(encoder, s);

		//
		// write resource keys
		//
		s->additional_resource_keys.Encode(buf+h->resource_keys_offset, resource_keys_size);

		// write events
		// 
		EncodeStream event_stream(buf + h->event_offset, event_size, false);
		encode_events(&event_stream, s->m_events);
		
		//
		// write timestamps
		// 
		EncodeStream timestamp_stream(buf + h->timestamp_offset, timestamp_size, false);
		encode_timestamps(&timestamp_stream, s->m_timestamps);

		s->update_mutex.unlock();

		// write buffer to a file
		std::fstream fs;
		fs.open(filename, std::fstream::out | std::fstream::binary);
		fs.write(buf, total_size);
		fs.close();
		free(buf);
	}
	else
	{
		std::fstream fs;
		fs.open(filename, std::fstream::out);
		dump_history_visit_fn dump_history(fs);
		traverse_history_graph_sequential(dump_history, s);
		fs.close();
	}
}

// public interface
vr_state_tracker_t load_vrstate_from_file(const char *filename)
{
	vr_state_tracker_t handle = nullptr; 

	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	if (size > 0)
	{
		handle = create_vr_state_tracker();
		tracker *s = static_cast<tracker*>(handle);

		file.seekg(0, std::ios::beg);

		assert(size_t(size) == size);
		std::vector<char> buffer(static_cast<size_t>(size));
		if (file.read(buffer.data(), size))
		{
			char *buf = &buffer[0];
			// read the header
			header *h = (header *)buf;

			// read the summary
			save_summary *sum = (save_summary*)(buf + h->summary_offset);
			s->m_frame_number = sum->num_frames + 1;
			memcpy(&s->save_info, sum, sizeof(save_summary));

			// read the state
			EncodeStream decode_stream(buf + h->state_offset, h->state_size, false);
			decoder_visitor decoder(&decode_stream, s->m_allocator);
			openvr_broker::open_vr_interfaces null_interfaces;
			openvr_broker::acquire_interfaces("null", &null_interfaces, nullptr);
			traverse_history_graph_sequential(decoder, s, null_interfaces);

			// read the resource keys
			s->additional_resource_keys.Decode(buf + h->resource_keys_offset, h->resource_keys_size);

			// read the events
			EncodeStream event_stream(buf + h->event_offset, h->event_size, false);
			assert((int)h->event_count == h->event_count);
			decode_events(&event_stream, s->m_events, static_cast<int>(h->event_count));

			// read the timestamps
			EncodeStream timestamp_stream(buf + h->timestamp_offset, h->timestamp_size, false);
			assert((int)h->timestamp_count == h->timestamp_count);
			decode_timestamps(&timestamp_stream, s->m_timestamps, static_cast<int>(h->timestamp_count));

			file.close();
		}
		else
		{
			destroy_vr_state_tracker(handle);
			handle = nullptr;
		}
	}
	else
	{
		destroy_vr_state_tracker(handle);
		handle = nullptr;
	}
	return handle;
}

// public_interface
void img_gui_update(vr_state_tracker_t h)
{
#ifdef HAVE_IMG_GUI
	tracker *s = static_cast<tracker*>(h);
	static img_visit_fn img_gui;
	traverse_history_graph_sequential(img_gui, s);
#endif
}

#ifdef HAVE_IMG_GUI
using namespace ImGui;
struct grid_node;

struct change_of_state
{
	virtual int GetFrameNumber() const { return -1;  }
	virtual int GetPrevFrameNumber() { return -1; }
	virtual bool GetHovered() const { return false; }
	virtual void *unique_id() = 0;
	virtual std::string GetChangeDescriptionString() = 0;
	virtual std::string GetPrevDescriptionString() = 0;
}; 

struct grid_node_data_if
{
	virtual const char *GetLabel() = 0;
	virtual const char *GetLabel(int *size) = 0;
	virtual const char *GetPopupMenuLabel() = 0;
	virtual std::string GetChangeDescriptionString() = 0;
	virtual bool has_changes() = 0;
	virtual bool start_iterator(int start_frame, int end_frame) = 0;
	virtual change_of_state* get_next() = 0;
	virtual change_of_state* peek_next() = 0;
	virtual change_of_state* get_latest() = 0;
};

// represents a data value / aka history node / aka a row in the grid
struct grid_node
{
	grid_node(grid_node_data_if *data_in)
		: data(data_in), 
		  pos_y(0),
		  height(0),
			label_selected(false), marked_for_freezing(false), marked_for_unfreezing(false)
	{
		text_size = CalcTextSize(data->GetLabel());
	}

	bool has_changes() {
		return data->has_changes();
	}
	
	inline bool start_iterator(int start_frame, int end_frame)
	{
		return data->start_iterator(start_frame, end_frame);
	}

	change_of_state* get_next()
	{
		return data->get_next();
	}

	change_of_state* peek_next()
	{
		return data->peek_next();
	}

	change_of_state* get_latest()
	{
		return data->get_latest();
	}

	const char *GetLabel() { return data->GetLabel(); }
	const char *GetLabel(int *size) { return data->GetLabel(size); }
	const char *GetPopupMenuLabel() { return data->GetPopupMenuLabel(); }
	const ImVec2 &GetTextSize() { return text_size; }
	const char *PopupMenuLabel() { return data->GetPopupMenuLabel(); }

	std::string GetChangeDescriptionString()
	{
		return data->GetChangeDescriptionString();
	}
	grid_node_data_if *data;
	// gui specific
	float			pos_y;
	float			height;
	ImVec2			text_size;
	bool label_selected;
	bool marked_for_freezing;
	bool marked_for_unfreezing;
};

// information dialog box data
struct box_data_struct
{
	int box_frame_number;
	int box_prev_number;
	std::string box_label, box_prev, box_cur;
};

// draw information dialog box
static void draw_box_data(const box_data_struct &box_data)
{
	Text(box_data.box_label.c_str());
	if (box_data.box_prev.size() > 0)
	{
		Columns(2);
		TextDisabled("frame %d", box_data.box_prev_number);
		TextDisabled(box_data.box_prev.c_str());
		NextColumn();
		Text("frame %d", box_data.box_frame_number);
		Text(box_data.box_cur.c_str());
	}
	else
	{
		Text(box_data.box_cur.c_str());
	}
}

// change of state info_dialogs are persistent
// map from a change of state to a info_dialog:
struct info_dialog
{
	info_dialog(void * id_in, const box_data_struct &box_data)
		:	id(id_in),
			m_box_data(box_data),
			up(true)
	{
		name = m_box_data.box_label;
		if (m_box_data.box_prev_number != -1)
		{
			m_box_data.box_label += " " + std::to_string(m_box_data.box_prev_number) + " vs " + std::to_string(m_box_data.box_frame_number);
		}
		else
		{
			m_box_data.box_label += " " + std::to_string(m_box_data.box_frame_number);
		}
		name = m_box_data.box_label;

		anim = 0;
		start_pos = GetMousePos();
		end_pos.y = start_pos.y - 150;
		end_pos.x = start_pos.x + 50;
		start_size.x = 50;
		start_size.y = 50;
		end_size.x = 200;
		end_size.y = 200;
	}
	void *id;
	std::string name;
	box_data_struct m_box_data;
	bool up;

	float anim;
	ImVec2 start_pos;
	ImVec2 end_pos;
	ImVec2 start_size;
	ImVec2 end_size;
};

static ImVec2 lerp(const ImVec2 &a, const ImVec2 &b, float f)
{
	ImVec2 result;
	result.x = a.x + (b.x - a.x) * f;
	result.y = a.y + (b.y - a.y) * f;
	return result;
}

// the grid nodes refer to data inside of the vr state
// which are mostly 'forward lists' 
// 
// so this wrapper keeps that reference and also a flag to indicate if it's the 
// end of the list
template <typename forward_iterator_type>
struct forward_ref
{
	forward_iterator_type m_iter;	// reference to data
	bool is_end;					// is this the end of the list - ie GetPrevFrameNumber should return -1

	forward_ref()
	{}

	int GetPrevFrameNumber() 
	{
		if (!is_end)
		{
			forward_iterator_type prev = m_iter;
			prev++;
			return prev->frame_number;
		}
		return -1;
	}

	std::string GetPrevDescriptionString() 
	{
		if (!is_end)
		{
			std::string s;
			forward_iterator_type prev = m_iter;
			prev++;
			s += prev->GetChangeDescriptionString();
			return s;
		}
		return "";
	}

	std::string GetChangeDescriptionString() 
	{
		std::string s;
		s += m_iter->GetChangeDescriptionString();
		return s;
	}

	int GetFrameNumber() const 
	{
		return m_iter->frame_number;
	}

	void *unique_id() 
	{
		void *unique = &(*m_iter);
		return unique;
	}
};

// this code is common between the history node iter and teh event iter
//
// todo: but templates make it tricky - so copy pasta for now
//
// cursors are created and managed by their 'grid-node-data-if'.  in this case
// the event_node_if
struct event_node_cursor : change_of_state
{
	typedef typename std::forward_list<FrameNumberedEvent, ALLOCATOR_TYPE>::iterator event_iter;
	forward_ref<event_iter> m_ref;

	int GetPrevFrameNumber() override
	{
		return m_ref.GetPrevFrameNumber();
	}

	virtual std::string GetPrevDescriptionString() override
	{
		return m_ref.GetPrevDescriptionString();
	}

	virtual std::string GetChangeDescriptionString() override
	{
		return m_ref.GetChangeDescriptionString();
	}

	virtual int GetFrameNumber() const override
	{
		return m_ref.GetFrameNumber();
	}
	virtual void *unique_id() override
	{
		return m_ref.unique_id();
	}
};


template <typename T, typename P, typename AllocatorT>
struct history_node_cos : change_of_state
{
	typedef history_base<T, P, AllocatorT> history_node;
	typedef typename history_node::iter_type history_iter;

	forward_ref<history_iter> m_ref;

	history_node_cos()
	{}

	int GetPrevFrameNumber() override
	{
		return m_ref.GetPrevFrameNumber();
	}

	virtual std::string GetPrevDescriptionString() override
	{
		return m_ref.GetPrevDescriptionString();
	}

	virtual std::string GetChangeDescriptionString() override
	{
		return m_ref.GetChangeDescriptionString();
	}
	virtual int GetFrameNumber() const override
	{
		return m_ref.GetFrameNumber();
	}
	virtual void * unique_id() override
	{
		return m_ref.unique_id();
	}
};

// caches iterators that represent cells along a row
template <typename iterator_type>
struct cached_iterator
{
	iterator_type iter_cur;	// initialized in start_iterator() and updated in get_next
	int start_frame_id;
	int cache_start_frame_id;
	int cache_end_frame_id;
	bool cache_rc;

	iterator_type cache_end_frame_iter;

	cached_iterator()
		:
		start_frame_id(-1),
		cache_end_frame_id(-1),
		cache_start_frame_id(-1)
	{
	}

	inline bool start_iterator(int start_frame_in, int end_frame_in, const iterator_type &begin, const iterator_type &end)
	{
		start_frame_id = start_frame_in; // keep this so the get_next() knows when to return null

		bool rc = false;
		if (end_frame_in == cache_end_frame_id && start_frame_in == cache_start_frame_id)
		{
			iter_cur = cache_end_frame_iter;
			rc = cache_rc;
		}
		else
		{
			// if we are searching a frame that is less than the cached frame
			// (and the cache is valid), then we can search back from that
			if (end_frame_in < cache_end_frame_id && cache_end_frame_id != -1)
			{
				iter_cur = cache_end_frame_iter;
			}
			else
			{
				// rats: start from the beginning then
				iter_cur = begin;
				//dprintf("reset to begin\n");
			}

			while (iter_cur != end)
			{
				if (iter_cur->frame_number > end_frame_in)
				{
					iter_cur++; // current frame is too big - keep looking
					//dprintf("to big\n");
				}
				else if (iter_cur->frame_number < start_frame_in)
				{
					// the current frame number is less than the interval - return false. cache this result
					cache_start_frame_id = start_frame_in;
					cache_end_frame_id = end_frame_in;
					cache_end_frame_iter = iter_cur;
					cache_rc = false;
					rc = false;
					break;
				}
				else
				{
					// found first one in range, break from while
					cache_start_frame_id = start_frame_in;
					cache_end_frame_id = end_frame_in;
					cache_end_frame_iter = iter_cur;
					rc = true;
					cache_rc = true;
					break;
				}
			}
		}
		return rc;
	}

	bool get_next(iterator_type *next, bool *next_is_end, const iterator_type& end) 
	{
		if (iter_cur != end 
			&& iter_cur->frame_number >= start_frame_id)
		{
			*next = iter_cur;
			iter_cur++;
			*next_is_end = (iter_cur == end);
			return true;
		}
		else
		{
			return false;
		}
	}
	bool peek_next(iterator_type *next, bool *next_is_end, const iterator_type& end)
	{
		if (iter_cur != end
			&& iter_cur->frame_number >= start_frame_id)
		{
			*next = iter_cur;
			*next_is_end = (iter_cur == end);
			return true;
		}
		else
		{
			return false;
		}
	}
};

struct event_node_if : grid_node_data_if
{
	tracker *s;
	typedef std::forward_list<FrameNumberedEvent, ALLOCATOR_TYPE>::iterator event_iterator;
	cached_iterator <event_iterator> c_iter;
	event_node_cursor current_event;	// this is the 'current event'

	event_node_if(tracker *s_in)
		: s(s_in)
	{
	}
	virtual const char *GetLabel()
	{
		return "events";
	}
	virtual const char *GetLabel(int *size)
	{
		*size = 6;
		return "events";
	}

	virtual const char *GetPopupMenuLabel()
	{
		return "events";
	}
	virtual std::string GetChangeDescriptionString()
	{
		if (!s->m_events.empty())
		{
			return s->m_events.front().GetChangeDescriptionString();
		}
		return std::string("");
	}
	virtual bool has_changes()
	{
		return true;
	}

	virtual bool start_iterator(int start_frame_in, int end_frame_in)
	{
		return c_iter.start_iterator(start_frame_in, end_frame_in, s->m_events.begin(), s->m_events.end());
	}

	// warning: modifies current_cos
	virtual change_of_state* get_next() override
	{
		event_iterator iter;
		bool next_is_end;
		if (c_iter.get_next(&iter, &next_is_end, s->m_events.end()))
		{
			current_event.m_ref.m_iter = iter;
			current_event.m_ref.is_end = next_is_end;
			return &current_event;
		}
		else
		{
			return nullptr;
		}
	}

	virtual change_of_state* peek_next() override
	{
		event_iterator iter;
		bool next_is_end;
		if (c_iter.peek_next(&iter, &next_is_end, s->m_events.end()))
		{
			current_event.m_ref.m_iter = iter;
			current_event.m_ref.is_end = next_is_end;
			return &current_event;
		}
		else
		{
			return nullptr;
		}
	}

	// warning: modifies current_cos
	virtual change_of_state* get_latest() override
	{
		event_iterator iter = s->m_events.begin();
		if (iter != s->m_events.end())
		{
			current_event.m_ref.m_iter = iter;
			iter++;
			current_event.m_ref.is_end = (s->m_events.end() == iter);
			return &current_event;
		}
		return nullptr;
	}
};

template <typename T, typename P, typename AllocatorT>
struct history_node_if : grid_node_data_if
{
	typedef history_base<T, P, AllocatorT> history_node;
	typedef typename history_node::iter_type history_iter;

	history_node *m_history_node;

	std::string m_path;
	std::string popup_menu_label;
	std::string label;

	cached_iterator <history_iter> c_iter;
	history_node_cos<T, P, AllocatorT > current_cos;	// this is the 'current change of state

	history_node_if(const std::string &path, history_node *base)
		:	m_path(path),
			m_history_node(base),
			popup_menu_label(base->name)
	{
	}

	virtual const char *GetLabel( int *size ) override
	{
		*size = (int)m_path.size();
		return m_path.c_str();
	}

	virtual const char *GetLabel() override
	{
		return m_path.c_str();
	}

	virtual const char *GetPopupMenuLabel() override
	{
		return popup_menu_label.c_str();
	}

	virtual bool has_changes() override
	{
		return m_history_node->more_than_2_values();
	}

	virtual std::string GetChangeDescriptionString()
	{
		if (!m_history_node->values.empty())
		{
			return m_history_node->values.front().GetChangeDescriptionString();
		}
		return "";
	}

	virtual bool start_iterator(int start_frame_in, int end_frame_in)
	{
		return c_iter.start_iterator(start_frame_in, end_frame_in, m_history_node->values.begin(), m_history_node->values.end());
	}

	// warning: modifies current_cos
	virtual change_of_state* get_next() override
	{
		history_iter iter;
		bool next_is_end; 
		if (c_iter.get_next(&iter, &next_is_end, m_history_node->values.end()))
		{
			current_cos.m_ref.m_iter = iter;
			current_cos.m_ref.is_end = next_is_end;
			return &current_cos;
		}
		else
		{
			return nullptr;
		}
	}

	virtual change_of_state* peek_next() override
	{
		history_iter iter;
		bool next_is_end;
		if (c_iter.peek_next(&iter, &next_is_end, m_history_node->values.end()))
		{
			current_cos.m_ref.m_iter = iter;
			current_cos.m_ref.is_end = next_is_end;
			return &current_cos;
		}
		else
		{
			return nullptr;
		}
	}

	// warning: modifies current_cos
	virtual change_of_state* get_latest() override
	{
		history_iter iter = m_history_node->values.begin();
		if (iter != m_history_node->values.end())
		{
			current_cos.m_ref.m_iter = iter;
			iter++;
			current_cos.m_ref.is_end = (m_history_node->values.end() == iter);
			return &current_cos;
		}
		return nullptr;
	}
};

// this index is very important as it versions the grid
// node sets.  it's used for stuff that caches state to know
// if their data is valid_or_not
static int unique_grid_node_id;

// this is a vector of grid_nodes with a version number
// the version number must change if the elements contents of the set change.
// (the idea being that caches can use this id to know when they are invalidated)
struct grid_node_set
{
	int version_id;
	std::vector<grid_node*> nodes;
};

// constructs a vector of grid_nodes from the vr state
struct grid_visitor
{
	std::vector<std::string>path_stack;
	std::vector<grid_node*> *m_vec;
	
	grid_visitor(std::vector<grid_node*> *vec)
		: m_vec(vec)
	{}

	template <typename T, typename P, typename AllocatorT>
	void add_grid_node(history_base<T,P,AllocatorT> *b)
	{
		std::string path;
		for (auto iter = path_stack.begin(); iter != path_stack.end(); iter++)
		{
			path += *iter;
			path += ".";
		}
		path += b->name;
		grid_node_data_if* data_if = new history_node_if<T,P,AllocatorT>(path, b);
		grid_node *node = new grid_node(data_if);
		m_vec->push_back(node);
	}

	inline bool visit_openvr() { return false; }
	inline bool reload_render_models() { return true; }

	inline void start_group_node(const char *group_id_name, int group_id_index) 
	{
		if (group_id_index != -1)
		{
			path_stack.push_back(std::string(group_id_name) + std::to_string(group_id_index));
		}
		else
		{
			path_stack.push_back(std::string(group_id_name));
		}
	}
	inline void end_group_node(const char *group_id_name, int group_id_index) 
	{
		path_stack.pop_back();
	}

	template <typename T>
	inline void start_vector(const char *vector_name, T &vec)
	{
	}

	template <typename T>
	inline void end_vector(const char *vector_name, T &vec)
	{}


	// since P is passed as a param, this is the one that'll be chosen for history with P
	template <typename T, typename P, typename allocatorT>
	void visit_node(history<std::vector<T, allocatorT>, P, allocatorT> &node)
	{
		add_grid_node(&node);
	}

	template <typename T, typename P, typename allocatorT>
	void visit_node(history<T, P, allocatorT> &node)
	{
		add_grid_node(&node);
	}

	EMPTY_RHS_TEMPLATE()
};

// called at initialization time - creates the initial grid node set
static void get_grid_nodes(grid_node_set &grid_nodes, int *num_frames, tracker *s)
{
	*num_frames = s->m_frame_number - 1;
	grid_nodes.version_id = unique_grid_node_id++;

	// add event row
	grid_node_data_if* event_data_if = new event_node_if(s);
	grid_node *node = new grid_node(event_data_if);
	grid_nodes.nodes.push_back(node);

	// add row for each of the nodes in the vr state
	grid_visitor visitor(&grid_nodes.nodes);
	openvr_broker::open_vr_interfaces null_interfaces;
	openvr_broker::acquire_interfaces("null", &null_interfaces, nullptr);
	traverse_history_graph_sequential(visitor, s, null_interfaces);
}

// user interface uses a couple regular expressions. 
// this class wraps the interface to pcre
struct re_context
{
	pcre2_compile_context *compile_context;
	pcre2_match_context *match_context;
	pcre2_match_data *match_data;
	pcre2_code *code;

	re_context()
	{
		const int OFFSET_SIZE = 33;
		compile_context = pcre2_compile_context_create(NULL);
		match_context = pcre2_match_context_create(NULL);
		match_data = pcre2_match_data_create(OFFSET_SIZE, NULL);
		code = nullptr;
	}

	~re_context()
	{
		pcre2_compile_context_free(compile_context);
		pcre2_match_context_free(match_context);
		pcre2_match_data_free(match_data);
	}

	bool compile(const char *buffer)
	{
		if (code)
		{
			pcre2_code_free(code);
		}
		int options = PCRE2_CASELESS;
		int errcode = 0;
		size_t erroffset = 0;
		code = pcre2_compile((PCRE2_SPTR8)buffer, PCRE2_ZERO_TERMINATED, options, &errcode,
			&erroffset, compile_context);
		return code != nullptr;
	}

	inline bool match(const char *buf, int len)
	{
		int rc = pcre2_match(code, (PCRE2_SPTR)buf, len, 0, 0, match_data, NULL);
		return rc >= 0;
	}
};

// 
// caches a frame number (last_frame_cache) and a list of frames that have data
// the purpose is to be able to collapse columns in the user interface for periods
// were there is no new data.
//
// 
//
struct FramesWithDataCache
{
	int cached_grid_set_version;	
	int cached_last_frame_id;
	std::vector<int> cached_frames_with_data;

	FramesWithDataCache()
		: 
			cached_grid_set_version(-1),
			cached_last_frame_id(0)
	{
		cached_frames_with_data.reserve(2000);
	}

	void Reset()
	{
		cached_last_frame_id = 0;
		cached_grid_set_version = -1;
		cached_frames_with_data.clear();
	}

	// 
	// I can cached based on 
	//		grid_nodes, and a frame_number - since the presence of frames
	//                                    does not change in the past
	//                                    (assuming grid_nodes set is the same)
	//		
	//
	void UpdateCache(int latest_frame_number,  grid_node_set &grid_nodes)
	{
		//
		// check the span from[cached_last_frame_id + 1, latest_frame_number]
		//

		// break if either I've checked all my nodes 
		// or if I've found the maximum number of hits in this span
		std::unordered_set<int> hits;
		int max_possible_hits = latest_frame_number - cached_last_frame_id;

		for (auto iter = grid_nodes.nodes.begin(); iter != grid_nodes.nodes.end() && 
							(int)hits.size() < max_possible_hits; iter++)
		{
			grid_node *node = *iter;
			// create an iterator to see if there is any data 
			if (node->start_iterator(cached_last_frame_id + 1, latest_frame_number) == false)
			{
			}
			change_of_state *change;
			while (change = node->get_next())
			{
				int frame_number = change->GetFrameNumber();
				auto iter = hits.find(frame_number);
				if (iter == hits.end())
				{	// unique: add it:
					hits.insert(iter, frame_number);
				}
			}
		}

		cached_frames_with_data.reserve(cached_frames_with_data.size() + hits.size());
		for (int i = cached_last_frame_id + 1; i <= latest_frame_number; i++)
		{
			if (hits.find(i) != hits.end())
			{
				cached_frames_with_data.push_back(i);
			}
		}

		cached_grid_set_version = grid_nodes.version_id;
		cached_last_frame_id = latest_frame_number;
	}

	//
	// input: 
	//		latest_frame_number: the lastest frame known
	//      grid_nodes:       the rows to consider
	//      max_visible_requested:   the maximum number of frames to return in 'visible_frames'
	//
	// output:
	//	a vector of frames that have data 
	//
	void GetLastFrameWithData(int latest_frame_number,
		grid_node_set &grid_nodes,
		int max_visible_requested,
		std::vector<int> *visible_frames)
	{
		if (grid_nodes.version_id != cached_grid_set_version)
		{
			Reset();	// the rows have changed, better reset
		}

		if (latest_frame_number > cached_last_frame_id)
		{
			// some columns have been added- update
			//Reset();
			UpdateCache(latest_frame_number, grid_nodes); // update the cache to track all frames with data
		}

		// return 
		int num_frames_to_return = max_visible_requested;
		if (num_frames_to_return > (int)cached_frames_with_data.size())
		{
			num_frames_to_return = (int)cached_frames_with_data.size();
			
		}
		if (num_frames_to_return > 0)
		{
			visible_frames->reserve(num_frames_to_return);
			int start = (int)cached_frames_with_data.size() - num_frames_to_return;
			if (start < 0)
			{
				start = 0;
			}
			for (int i = start; i < (int)cached_frames_with_data.size(); i++)
			{
				visible_frames->push_back(cached_frames_with_data[i]);
			}
		}
	}
};


// making colums look the way i want to --- to be able to skip frames -- is complex
// the following interface is to be able to separate the column rendering complexity
// away from the rest of the drawing code
class ColumnSetup
{
public:
	ColumnSetup()
	{
		left_margin = 50.0f;
		pixels_per_frame = 20.0f;
	}

	virtual int GetFirstVisibleFrame() = 0;
	virtual int GetLastVisibleFrame() = 0;
	virtual int GetVisibleFrame(int index) = 0;
	virtual int GetNumVisibleFrames() = 0;
	virtual float GetPixelOffsetOfFrame(int index) = 0;
	virtual void Update(float plot_width, float pixels_per_frame, int last_frame, grid_node_set &pane_nodes_in) = 0;
protected:
	float pixels_per_frame;
	float left_margin;
	float max_visible_frames;
};

// ForceRightColumnSetup
// layout colums so that the latest frame is on the right and any
// columns(frames) that do not have data are trimmed
//
class ForceRightColumnSetup : public ColumnSetup
{
	FramesWithDataCache data_cache;
	std::vector<int> visible_frames;
	std::unordered_map<int, float> offset_of_frame;
public:
	ForceRightColumnSetup()
	{}

	virtual int GetFirstVisibleFrame()
	{
		return visible_frames.front();
	}

	virtual int GetLastVisibleFrame()
	{
		return visible_frames.back();
	}

	virtual int GetVisibleFrame(int index) override
	{
		return visible_frames[index];
	}
	virtual int GetNumVisibleFrames()
	{
		return (int)visible_frames.size();
	}
	virtual float GetPixelOffsetOfFrame(int frame_number) override
	{
		return offset_of_frame[frame_number];
	}

	//
	// update the visible frames caches
	//
	void Update(float plot_width, float pixels_per_frame, int last_frame, grid_node_set &nodes)
	{
		// clear the old states because we will reset them 
		visible_frames.clear();		
		offset_of_frame.clear();	
		max_visible_frames = (plot_width - left_margin) / pixels_per_frame;

		int int_max_visible_frames = static_cast<int>(max_visible_frames + 0.5f);
		// figure out which frames actually have data
		data_cache.GetLastFrameWithData(last_frame, nodes, int_max_visible_frames, &visible_frames);

		// convenience lookup for rendering columns:
		for (int i = 0; i < (int)visible_frames.size(); i++)
		{
			offset_of_frame[visible_frames[i]] = left_margin + i*pixels_per_frame;
		}
	}
};

template <size_t count>
void strcpy_safe(char(&s)[count], const char* pSrc)
{
#pragma warning(push)
#pragma warning( disable : 4996)
strncpy(s, pSrc, count);
#pragma warning(pop)
// Ensure null-termination.
s[count-1] = 0;
}

struct vr_gui_context
{
	std::unordered_map<void *, info_dialog*> info_dialogs;
	std::vector<void*> info_dialog_reaper;

	void reap_info_dialogs()
	{
		for (auto iter = info_dialog_reaper.begin(); iter != info_dialog_reaper.end(); iter++)
		{
			auto pop_iter = info_dialogs.find(*iter);
			info_dialog* p = pop_iter->second;
			info_dialogs.erase(pop_iter);
			delete p;
		}
		info_dialog_reaper.clear();
	}

	void draw_info_dialog(info_dialog *p)
	{
		if (p->anim < 1.0f)
		{
			ImGui::SetNextWindowSize(lerp(p->start_size, p->end_size, p->anim), ImGuiSetCond_Always);
			ImGui::SetNextWindowPos(lerp(p->start_pos, p->end_pos, p->anim), ImGuiSetCond_Always);
			if (p->anim < 0.25)
			{
				p->anim += 0.05f;
			}
			else
			{
				p->anim += 0.25f;
			}
		}

		if (ImGui::Begin(p->name.c_str(), &p->up))
		{
			ImGui::PushID(p->id);
			draw_box_data(p->m_box_data);
			ImGui::PopID();
		}
		else
		{
			// collapsed - don't draw stuff
		}
		ImGui::End();
		if (!p->up)
		{
			info_dialog_reaper.push_back(p->id);
		}
	}

	void draw_info_dialogs()
	{
		for (auto iter = info_dialogs.begin(); iter != info_dialogs.end(); iter++)
		{
			draw_info_dialog(iter->second);
		}
	}

	void update()
	{
		reap_info_dialogs();
		draw_info_dialogs();
	}
};


struct timeline_grid
{
	int visible_row_start;
	int visible_frame_start;
	int last_frame_with_data;
	int first_frame_with_data;
	int max_visible_frames;
	int last_visible_frame;
	float max_text_width;
	float max_text_height;
	int last_frame;
	int num_frames;
	float pixels_per_frame = 20.0f;
	grid_node_set all_nodes;
	grid_node_set filtered_nodes;
	re_context inclusive_regex;
	re_context exclusive_regex;
	box_data_struct box_data;
	ForceRightColumnSetup force_right_setup;
	int prev_all_nodes_version_id;

	bool calculated_text_widths;
	bool change_once;
	ImVec2 scroll_pane_content_size;
	char include_filter_buf[128];
	char exclusive_filter_buf[128];
	tracker *m_vrstate;
	vr_gui_context *m_gui_context;

	timeline_grid(vr_gui_context *gui_context, tracker *s, const grid_config &c)
		: 
			visible_row_start(0),
			visible_frame_start(0),
			prev_all_nodes_version_id(-1),
			calculated_text_widths(false),
			change_once(true),
			m_vrstate(s),
			m_gui_context(gui_context)
	{
		strcpy_safe(include_filter_buf, c.include_filter);
		strcpy_safe(exclusive_filter_buf, c.exclude_filter);
		get_grid_nodes(all_nodes, &num_frames, s);
	}

	void calculate_text_widths()
	{
		max_text_width = 0;
		max_text_height = 0;
		for (int i = 0; i < (int)all_nodes.nodes.size(); i++)
		{
			float w = all_nodes.nodes[i]->GetTextSize().x;
			if (w > max_text_width)
			{
				max_text_width = w;
			}
			if (all_nodes.nodes[i]->GetTextSize().y > max_text_height)
			{
				max_text_height = all_nodes.nodes[i]->GetTextSize().y;
			}
		}
		if (max_text_width > 200)
		{
			max_text_width = 200;
		}
		calculated_text_widths = true;
	}

	void draw_label_and_pane(
		const char *label_name, const char *pane_name,
		grid_node_set &pane_nodes_in, float pane_height,
		ColumnSetup &column_setup
	)
	{
		PushID(label_name);

		float ypos_start_of_list = ImGui::GetCursorPosY();
		ImGui::SetCursorPosX(20);
		int child_flags = ImGuiWindowFlags_NoScrollbar;
		
		// visible row_start
		float plot_start_x = 20 + max_text_width + 150;
		float plot_width = GetWindowWidth() - plot_start_x - 25;
		
		column_setup.Update(plot_width, pixels_per_frame, last_frame, pane_nodes_in);
		if (pane_height == 0)
		{
			pane_height = 350;
		}
		
		// walk though the data and add rows starting from visible row start AND have visible data
		float line_height = GetTextLineHeightWithSpacing();
		int max_visible_rows = static_cast<int>(pane_height / line_height + 0.5f);
		std::vector<grid_node*> pane_nodes_with_data_in_range;
		
		if (column_setup.GetNumVisibleFrames() > 0)
		{
			int first_visible = column_setup.GetFirstVisibleFrame();
			int last_visible = column_setup.GetLastVisibleFrame();
			for (auto iter = pane_nodes_in.nodes.begin(); 
					iter != pane_nodes_in.nodes.end(); iter++)
			{
				grid_node *node = *iter;
				if (node->start_iterator(first_visible, last_visible))
				{
					pane_nodes_with_data_in_range.push_back(node);
				}
			}
		}

		// if simultaneous updates are enabled use peek_next 
		// to get values conistent with the start_iterator

#if 0
		std::sort(pane_nodes_with_data_in_range.begin(), pane_nodes_with_data_in_range.end(), [](grid_node* a, grid_node *b) {
			int frame_number_a = a->peek_next()->GetFrameNumber();
			int frame_number_b = b->peek_next()->GetFrameNumber();
			if (frame_number_a != frame_number_b)
			{
				return frame_number_a > frame_number_b;
			}
			else
			{
				return a > b;
			}
		});
#endif

		int visible_rows = std::min(max_visible_rows, (int)pane_nodes_with_data_in_range.size() - visible_row_start);
		std::vector<grid_node*> pane_nodes;
		pane_nodes.reserve(visible_rows);
		for (int i = visible_row_start; i < (int)pane_nodes_with_data_in_range.size()
			&& i < visible_row_start+max_visible_rows; i++)
		{
			pane_nodes.push_back(pane_nodes_with_data_in_range[i]);
		}

		ImVec2 label_content_size(max_text_width, pane_nodes_in.nodes.size()*line_height);
		SetNextWindowContentSize(label_content_size);
		BeginChild(label_name, ImVec2(max_text_width + 150, pane_height), true, child_flags);

		// update visible row start
		visible_row_start = static_cast<int>(GetScrollY() / line_height);
		
		bool freeze_selected = false;
		bool unfreeze_selected = false;
		float child_start_y = GetScrollY();
		SetCursorPosY(child_start_y);
		ImGui::NewLine();
		ImGui::NewLine();

		float prev_pos_y = GetCursorPosY();
		// draw labels
		for (auto iter = pane_nodes.begin(); iter != pane_nodes.end(); iter++)
		{
			grid_node *node = *iter;
			bool prev = node->label_selected;
			ImGui::Selectable(node->GetLabel(), &node->label_selected, 0, node->GetTextSize());

			if (IsItemHovered())
			{
				change_of_state *change = node->get_latest();
				if (change)
				{
					box_data.box_frame_number = change->GetFrameNumber();
					box_data.box_prev_number = change->GetPrevFrameNumber();
					box_data.box_label = node->GetLabel();
					box_data.box_prev = change->GetPrevDescriptionString();
					box_data.box_cur = change->GetChangeDescriptionString().c_str();
				}
			}

			if (BeginPopupContextItem(node->GetPopupMenuLabel() , 1))
			{
				EndPopup();
			}
			node->pos_y = ImGui::GetCursorPosY();
			node->height = node->pos_y - prev_pos_y;
			prev_pos_y = node->pos_y;
		}

		EndChild();

		float ypos_end_of_list = ImGui::GetCursorPosY();

		// 
		//BeginChild - main plot area
		// 
		
		ImGui::SetCursorPos(ImVec2(plot_start_x, ypos_start_of_list));
		ImVec2 grid_content_size(plot_width, label_content_size.y);
		SetNextWindowContentSize(grid_content_size);
		BeginChild(pane_name, ImVec2(plot_width, pane_height), true, child_flags);    // begin a scrolling region. size==0.0f: use remaining window size, size<0.0f: use remaining window size minus abs(size). size>0.0f: fixed size. each axis can use a different mode, e.g. ImVec2(0,pane_height).
		
		SetScrollY(child_start_y);
		SetCursorPosY(child_start_y);
	
		// draw ruler (vertical lines)
		for (int i = 0; i < column_setup.GetNumVisibleFrames(); i++)
		{
			int frame_id = column_setup.GetVisibleFrame(i);
			float offset_x = column_setup.GetPixelOffsetOfFrame(frame_id);
			ImGui::SetCursorPosX(offset_x);
			ImVec2 v2 = GetCursorScreenPos();
			v2.y += 6;
			ImVec2 v2end = v2;
			v2.y += 20;
			ImColor line_color(1.0f, 1.0f, 1.0f, 0.4f);

			// draw frame numbers
			char szbuf[256];
			if (i+1 <= column_setup.GetNumVisibleFrames())
			{
				GetWindowDrawList()->AddLine(v2, v2end, line_color);
				snprintf(szbuf, sizeof(szbuf), "%d", frame_id);
			}
			else
			{
				szbuf[0] = '\0';	// this extra bit is to stretch the content pane so panning works correctly
			}
			v2end.x += 1;
			GetWindowDrawList()->AddText(v2end, line_color, szbuf);
		}

		bool item_hovered = false;
		bool item_clicked = false;
		// draw nodes
		for (auto iter = pane_nodes.begin(); iter != pane_nodes.end(); iter++)
		{
			grid_node *node = *iter;
			//  horizontal lines
			SetCursorPosX(0);
			ImGui::SetCursorPosY(node->pos_y - node->height / 2);
			ImVec2 v2 = GetCursorScreenPos();
			ImVec2 v2end = v2;
			v2end.x += (num_frames + 2)*pixels_per_frame;
			GetWindowDrawList()->AddLine(v2, v2end, ImColor(1.0f, 1.0f, 1.0f, 0.2f));
			if (column_setup.GetNumVisibleFrames() > 0)
			{
 				node->start_iterator(column_setup.GetFirstVisibleFrame(), column_setup.GetLastVisibleFrame());
				int i = 0;
				while (change_of_state *change = node->get_next())
				{
					PushID(change->unique_id());
					float width = pixels_per_frame * .7f;
					float offset_x = column_setup.GetPixelOffsetOfFrame(change->GetFrameNumber());

					ImGui::SetCursorPosX(offset_x - width / 2.0f);
					ImGui::SetCursorPosY(node->pos_y - node->height + 1);

					bool colors_pushed = false;
					auto info_dialog_iter = m_gui_context->info_dialogs.find(change->unique_id());

					// if there is a info_dialog AND the info_dialog is hovered, then my color should
					// indicate this
					if ( info_dialog_iter != m_gui_context->info_dialogs.end())
					{
						colors_pushed = true;
						ImColor col;
						if (change->GetHovered())
						{
							col = GetColorU32(ImGuiCol_TitleBgActive);
							float h, s, v;
							ColorConvertRGBtoHSV(col.Value.x, col.Value.y, col.Value.z, h, s, v);
							col.SetHSV(h, s, v*1.2f, 1);
						}
						else
						{
							col = GetColorU32(ImGuiCol_TitleBgActive);
						}

						PushStyleColor(ImGuiCol_Button, col);
						PushStyleColor(ImGuiCol_ButtonActive, col);
						PushStyleColor(ImGuiCol_ButtonHovered, col);
					}
					Button("", ImVec2(width, node->height - 1));	// witdh scaled by pixels per frame

					if (colors_pushed)
					{
						PopStyleColor(3);
					}

					if (IsItemHovered())
					{
						item_hovered = true;
						box_data.box_frame_number = change->GetFrameNumber();
						box_data.box_prev_number = change->GetPrevFrameNumber();
						box_data.box_label = node->GetLabel();
						box_data.box_prev = change->GetPrevDescriptionString();
						box_data.box_cur = change->GetChangeDescriptionString().c_str();
					}
					if (IsItemClicked())
					{
						if (info_dialog_iter != m_gui_context->info_dialogs.end())
						{
						}
						else
						{
							info_dialog *p = new info_dialog(change->unique_id(), box_data);
							m_gui_context->info_dialogs[change->unique_id()] = p;
						}
						
						item_clicked = true;
					}
					PopID();
				}
			}
		}

		if (!item_clicked && ImGui::IsMouseHoveringWindow())
		{
			ImGuiIO &io = GetIO();
			pixels_per_frame += io.MouseWheel;
			if (pixels_per_frame < 1)
			{
				pixels_per_frame = 1;
			}
			ImVec2 drag = ImGui::GetMouseDragDelta(0, 0.0f);
			SetScrollX(GetScrollX() - drag.x);
		}
		
		EndChild();

		//
		// draw bottom
		//
		ImGui::SetCursorPosX(20);
		BeginChild("bottominfobox", ImVec2(GetWindowWidth() - 45, 0), true, child_flags);
		draw_box_data(box_data);
		EndChild();

		PopID(); // (label_name);
	}

	bool inclusive_regex_is_empty(const char *buf)
	{
		if (buf[0] == 0 || 
			(buf[0] == '.' && ((buf[1] == '*' && buf[2] == '\0')|| buf[1] == '\0')))
		{
			return true;
		}
		return false;
	}

	void draw()
	{
		last_frame = m_vrstate->m_frame_number;
		num_frames = m_vrstate->m_frame_number + 1;

		
		Value("frames", num_frames );
		if (m_vrstate->save_info.date_string[0])
		{
			Text("%s", m_vrstate->save_info.date_string);
		}

		
		NewLine();

		bool rebuild_filter = false;
		if (all_nodes.version_id != prev_all_nodes_version_id)
		{
			rebuild_filter = true;
			calculate_text_widths();
		}
		prev_all_nodes_version_id = all_nodes.version_id;

		ImGui::SetCursorPosX(20);
		if (rebuild_filter || InputText("Include", include_filter_buf, sizeof(include_filter_buf)))
		{
			inclusive_regex.compile(include_filter_buf);
			rebuild_filter = true;
		}
		ImGui::SetCursorPosX(20);
		if (rebuild_filter || InputText("Exclude##exclusive", exclusive_filter_buf, sizeof(exclusive_filter_buf)))
		{
			exclusive_regex.compile(exclusive_filter_buf);
			rebuild_filter = true;
		}
		
		if (rebuild_filter)
		{
			filtered_nodes.version_id = unique_grid_node_id++;
			filtered_nodes.nodes.clear();

			for (auto iter = all_nodes.nodes.begin(); iter != all_nodes.nodes.end(); iter++)
			{
				// walk through each row, read the label.  if the label 
				// passes the filters, then push it onto the filtered_nodes vector
				int size;
				const char *label = (*iter)->GetLabel(&size);
				bool include = inclusive_regex_is_empty(include_filter_buf) || inclusive_regex.match(label, size);
				bool exclude = exclusive_filter_buf[0] != '\0' && exclusive_regex.match(label, size);
				if (include && !exclude)
				{
					filtered_nodes.nodes.push_back(*iter);
				}
			}
		}

		if (rebuild_filter)
		{
			visible_row_start = 0;
		}
	
		draw_label_and_pane("l1", "p1", filtered_nodes, 0, force_right_setup);
	}
};
#endif // HAVE_IMG_GUI


gui_context_t create_gui_context()
{
#ifdef HAVE_IMG_GUI
	vr_gui_context *context = new vr_gui_context();
	return context;
#else
	return nullptr;
#endif
}

void gui_context_update(gui_context_t g)
{
#ifdef HAVE_IMG_GUI
	vr_gui_context *context = (vr_gui_context *)g;
	context->update();
#endif
}

void destroy_gui_context(gui_context_t g)
{
#ifdef HAVE_IMG_GUI
	vr_gui_context *context = (vr_gui_context *)g;
	delete g;
#endif 
}

timeline_grid_t create_timeline_grid(gui_context_t g, vr_state_tracker_t h, const grid_config &config)
{
#ifdef HAVE_IMG_GUI
	vr_gui_context *context = (vr_gui_context *)g;
	tracker *s = static_cast<tracker*>(h);
	timeline_grid *grid = new timeline_grid(context, s, config);
	return grid;
#else
	return nullptr;
#endif
}

void timeline_grid_update(timeline_grid_t g)
{
#ifdef HAVE_IMG_GUI
	timeline_grid *grid = (timeline_grid*)g;
	grid->draw();
#endif
}

void destroy_timeline_grid(timeline_grid_t g)
{
#ifdef HAVE_IMG_GUI
	timeline_grid *grid = (timeline_grid*)g;
	delete grid;
#endif
}