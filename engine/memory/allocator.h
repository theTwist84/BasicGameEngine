#ifndef __ENGINE_MEMORY_ALLOCATOR_H
#define __ENGINE_MEMORY_ALLOCATOR_H

#include "../engine_definitions.h"

namespace engine
{
	class c_allocator
	{
	public:
		virtual void* allocate(const int64 size) const = 0;
		virtual void free(void* const ptr) const = 0;
	};

	c_allocator* g_heap_allocator();
	c_allocator* g_virtual_allocator();
}

#endif