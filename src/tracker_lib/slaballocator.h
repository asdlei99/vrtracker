#if 0
// slab and an stl-compatible allocator
#pragma once
#include <list>
#include <memory>

extern int simple_allocator_allocators_constructed;
extern int simple_allocator_allocators_destroyed;

template <class T, typename slabT>
struct SimpleAllocator {
public:

	typedef T*         pointer;
	typedef const T&   const_reference;

	slabT local_slab;
	typedef T value_type;
	SimpleAllocator(/*ctor args*/)
	{
		local_slab = std::make_shared<slabT::element_type>();
		simple_allocator_allocators_constructed++;
	}

	SimpleAllocator(const SimpleAllocator &other)
	{
		local_slab = other.local_slab;
		simple_allocator_allocators_constructed++;

	}

	SimpleAllocator(slabT canidothis)
	{
		local_slab = canidothis;
		simple_allocator_allocators_constructed++;
	}


	template<typename X, typename Y>
	struct rebind { using other = SimpleAllocator<T, slabT>; };

	~SimpleAllocator()
	{
		simple_allocator_allocators_destroyed--;
	}

	template <class U, typename slabT> SimpleAllocator(const SimpleAllocator<U, slabT>& other)
	{
		simple_allocator_allocators_constructed++;
		local_slab = other.local_slab;
	}

	T* allocate(std::size_t n)
	{
		T* p = (T*)local_slab->slab_alloc(n * sizeof(T));
		return (T*)p;
	}
	void deallocate(T* p, std::size_t n)
	{
	}
};

template <class T, class T2, class U, class U2>
bool operator==(const SimpleAllocator<T, T2>&, const SimpleAllocator<U, U2>&)
{
	return true;
}

template <class T, class T2, class U, class U2>
bool operator!=(const SimpleAllocator<T, T2>&, const SimpleAllocator<U, U2>&)
{
	return false;
}



template <typename T>
class listwrap : public std::list<T>
{
public:
	listwrap()
	{
		static int i;
		i++;
	}
};


extern int slab_num_slabs;
extern int slab_total_slab_page_allocs;
extern int slab_total_slab_page_frees;
extern int slab_num_alloc_calls;

template <int page_size>
struct slab
{
	slab()
	{
		slab_num_slabs++;
		current_page_pos = 0;
	}

	~slab()
	{
		for (auto iter = pages.begin(); iter != pages.end(); iter++)
		{
			char *mem = *iter;
			free(mem);
			slab_total_slab_page_frees += 1;
		}
		slab_num_slabs--;
	}
	listwrap<char *> pages;
	int current_page_pos;


	void *slab_alloc(size_t size)
	{
		size = (size + 3) & ~0x3;
		if (pages.empty() || size + current_page_pos > page_size)
		{
			current_page_pos = 0;
			slab_total_slab_page_allocs += 1;
			char *page = (char *)malloc(page_size);
			pages.push_front(page);
		}

		slab_num_alloc_calls += 1;
		char *ret = pages.front();
		ret += current_page_pos;
		current_page_pos += size;
		return ret;
	}
};

#endif