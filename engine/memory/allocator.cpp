#include "allocator.h"

#include <cstdlib>
#include "Windows.h"

namespace engine
{
	class c_heap_allocator : c_allocator
	{
	public:
		void* allocate(const int64 size) const override { return std::malloc(size);}
		void free(void* const ptr) const override { std::free(ptr); }
	};


	class c_virtual_allocator : c_allocator
	{
	public:
		void* allocate(const int64 size) const override { return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); }
		void free(void* const ptr) const override { VirtualFree(ptr, 0, MEM_RELEASE); }
	};


	c_heap_allocator heap_allocator;
	c_virtual_allocator virtual_allocator;

	c_allocator* g_heap_allocator() { return (c_allocator*)&heap_allocator; };
	c_allocator* g_virtual_allocator() { return (c_allocator*)&virtual_allocator; };
}
