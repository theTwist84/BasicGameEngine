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
	struct s_datum_header
	{
		int32 salt;
	};


	struct s_data_array
	{

		int64 capacity;
		int64 datum_size;
		char name[k_maximum_string_length];
		int64* active_pages;
		char* data;

	private:

		enum
		{
			k_block_per_page = 64,
			k_page_index_bit_shift = 6
		};

		static void initialize_data_array(void* allocated_memory, std::string name, int64 maximum_count, int64 datum_size);


	public:
		// first attempt at allocating memory in an array format. Arrays are not automatically garbage collected.
		static s_data_array* create_new_data_array(std::string name, int64 maximum_count, int64 datum_size);

		int64 datum_delete(int32 datum_index);

	};
	static_assert(sizeof(s_data_array) == 0x40, "");
	

	

}


#endif