#include "data_array.h"

namespace engine
{

	void s_data_array::initialize_data_array(void* allocated_memory, std::string name, int64 capacity, int64 datum_size)
	{
		memset(allocated_memory, 0, sizeof(s_data_array));
		s_data_array* new_array = reinterpret_cast<s_data_array*>(allocated_memory);

		strncpy_s(new_array->name, name.c_str(), k_maximum_string_length);
		new_array->capacity = capacity;
		new_array->datum_size = datum_size;
	}

	s_data_array* s_data_array::create_new_data_array(std::string name, int64 capacity, int64 datum_size)
	{
		int64 total_memory_size, data_memory_size, active_page_memory_size;

		data_memory_size = capacity * datum_size;
		active_page_memory_size = sizeof(int64) * ((capacity + (k_block_per_page - 1)) >> k_page_index_bit_shift);
		total_memory_size = data_memory_size + sizeof(s_data_array) + active_page_memory_size;

		// TODO: add option for custom allocators such that create_new_data_array does not depent on malloc and friends
		void* allocated_memory = malloc(total_memory_size);

		if (allocated_memory != nullptr)
		{
			initialize_data_array(allocated_memory, name, capacity, datum_size);
			s_data_array* new_array = reinterpret_cast<s_data_array*>(allocated_memory);
			// signal array has been allocated properly
			// TODO add flags 
			return new_array;
		}
		else
			return nullptr;
	}

	int64 s_data_array::datum_delete(int32 datum_index)
	{
		char* data = &this->data[datum_index * this->datum_size];

		// TODO: add flag for debug fill
		memset(data, k_debug_fill_data, this->datum_size);


		int64* page = &this->active_pages[datum_index >> k_page_index_bit_shift];
		*page &= ~(1 << (datum_index & (k_block_per_page - 1)));

		return -1;
	}
}