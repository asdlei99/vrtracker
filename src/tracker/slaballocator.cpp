#include "slaballocator.h"

int slab_num_slabs;
int slab_total_slab_page_allocs;
int slab_total_slab_page_frees;
int slab_num_alloc_calls;

int simple_allocator_allocators_constructed = 0;
int simple_allocator_allocators_destroyed = 0;
