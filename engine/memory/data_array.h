#ifndef __ENGINE_MEMORY_DATA_ARRAY_H
#define __ENGINE_MEMORY_DATA_ARRAY_H

#include "../engine_definitions.h"
#include <string>


/*
Fixed size memory pool with array-like properties.

Memory is preallocated on creation and is subdivided into blocks of size datum_size.

Adding a block: find the first unused block and mark it as used.
Removing a block: mark the block unused and update markers.

Active blocks are stored in pages and we can fit 64 block per page.
*/

namespace engine
{
#define DATUM_HANDLE_INDEX(datum_handle) datum_handle & 0xFFFFFFFF
#define DATUM_HANDLE_SALT(datum_handle) (datum_handle >> 32) & 0xFFFFFFFF

	typedef int64 datum_handle;

	enum e_data_array_flags
	{
		_data_array_flags_is_valid = 1 << 0,
		_data_array_flags_is_initialized = 1 << 1,
		_data_array_flags_debug_fill = 1 << 2,
	};

	struct s_datum_header
	{
		int32 salt;
	};

	struct s_data_array
	{
		char name[k_maximum_string_length];
		int64 capacity;
		int64 datum_size;
		int32 next_index;
		int32 first_unallocated_index;
		int64* active_pages;
		int64 active_count;
		int32 next_salt;
		int32 flags;
		char* data;

	public:
		// first attempt at allocating memory in an array format. Arrays are not automatically garbage collected.
		

		int64 allocation_size();
		void delete_all();
		
		void make_invalid();
		void make_valid();


		void datum_delete(int32 datum_index);
		datum_handle datum_new();

		void* datum_get(datum_handle handle);
		void* datum_get_absolute(int64 index);


	};
	static_assert(sizeof(s_data_array) == 0x58, "");

	static s_data_array* create_new_data_array(std::string name, int64 maximum_count, int64 datum_size);
	void dispose(s_data_array* data_array);

}


#endif