#include "data_array.h"
#include "../engine_debug.h"

namespace engine
{
	enum
	{
		k_page_index_bit_shift = 6,
		k_salt_size = 32,
		k_block_per_page = 64,

	};

	void initialize_data_array(void* allocated_memory, std::string name, int32 capacity, int64 datum_size)
	{
		int64 data_memory_size = capacity * datum_size;
		int64 active_page_memory_size = sizeof(int64) * ((capacity + (k_block_per_page - 1)) >> k_page_index_bit_shift);

		memset(allocated_memory, 0, sizeof(s_data_array));
		s_data_array* new_array = reinterpret_cast<s_data_array*>(allocated_memory);

		strncpy_s(new_array->name, name.c_str(), k_maximum_string_length);
		new_array->capacity = capacity;
		new_array->datum_size = datum_size;
		new_array->active_count = 0;
		new_array->next_salt = 0x80000000u;
		new_array->next_index = 0;
		new_array->first_unallocated_index = 0;

		// data is located immediatly after the struct
		new_array->data = (char*)allocated_memory + sizeof(s_data_array);

		// active page table located after the data
		new_array->active_pages = (int64*)((char*)allocated_memory + sizeof(s_data_array) + data_memory_size);
		memset((void*)new_array->active_pages, 0, active_page_memory_size);

		new_array->flags &= ~_data_array_flags_is_valid;

		if (ENGINE_DEBUG)
			new_array->flags |= _data_array_flags_debug_fill;

	}

	s_data_array* create_new_data_array(std::string name, int32 capacity, int64 datum_size)
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
			new_array->flags |= _data_array_flags_is_initialized;
			return new_array;
		}
		else
			return nullptr;
	}

	int64 allocation_size(s_data_array* data_array)
	{
		return sizeof(s_data_array) + data_array->capacity * data_array->datum_size + sizeof(int64) * ((data_array->capacity + (k_block_per_page - 1)) >> k_page_index_bit_shift);
	}

	void make_invalid(s_data_array* data_array)
	{
		data_array->flags &= ~_data_array_flags_is_valid;
	}

	void make_valid(s_data_array* data_array)
	{
		data_array->flags |= _data_array_flags_is_valid;
		delete_all(data_array);
	}

	void dispose_data_array(s_data_array* data_array)
	{	
		memset(data_array, 0, sizeof(s_data_array));
		// TODO: use custom allocator
		free(data_array);
	}

	bool data_array_is_datum_used(s_data_array* data_array, int32 index)
	{
		int64 page = data_array->active_pages[index >> k_page_index_bit_shift];
		int64 page_bit = 1i64 << (index & (k_block_per_page - 1));
		return (page_bit & page) != 0;
	}

	void delete_all(s_data_array* data_array)
	{
		data_array->first_unallocated_index = 0;
		data_array->active_count = 0;
		data_array->next_index = 0;
		data_array->next_salt = 0x80000000;

		if ((data_array->flags & _data_array_flags_debug_fill) > 0)
			memset(data_array->data, k_debug_fill_data, data_array->datum_size * data_array->capacity);

		for (int64 i = 0; i < data_array->capacity; i++)
		{
			(*(s_datum_header*)(data_array->data + i * data_array->datum_size)).salt = 0;
		}
		memset(data_array->active_pages, 0, sizeof(int64) * ((data_array->capacity + (k_block_per_page - 1)) >> k_page_index_bit_shift));
	}

	void datum_delete(s_data_array* data_array, datum_handle handle)
	{
		int32 index = handle & 0xFFFFFFFF;
		if (handle != -1 && index >= 0 && index < data_array->capacity)
		{
			char* data = &data_array->data[index * data_array->datum_size];
			int32 datum_salt = (*(s_datum_header*)data).salt;
			int32 handle_salt = (handle >> 32) & 0xFFFFFFFF;
			if (handle_salt == datum_salt)
				datum_delete(data_array, index);
		}
	}

	void datum_delete(s_data_array* data_array, int32 datum_index)
	{
		char* data = &data_array->data[datum_index * data_array->datum_size];

		if((data_array->flags & _data_array_flags_debug_fill) > 0)
			memset(data, k_debug_fill_data, data_array->datum_size);

		int64* page = &data_array->active_pages[datum_index >> k_page_index_bit_shift];
		*page &= ~(1 << (datum_index & (k_block_per_page - 1)));

		// clear the salt from the data, very important
		(*(s_datum_header*)data).salt = 0;

		if (datum_index < data_array->next_index)
			data_array->next_index = datum_index;

		if (datum_index + 1 == data_array->first_unallocated_index)
		{
			char* other_salt = data;
			do
			{
				data_array->first_unallocated_index -= 1;
				other_salt -= data_array->datum_size;
			} 
			while (data_array->first_unallocated_index > 0 && (*(s_datum_header*)other_salt).salt == 0);
		}
		data_array->active_count -= 1;
	}

	datum_handle datum_new(s_data_array* data_array)
	{
		int32 current_free_index = data_array->first_unallocated_index;
		int32 datum_index = data_array->next_index;


		if (datum_index >= current_free_index)
		{
			goto VERIFY_FREE_INDEX;
		}
		else
		{
			// while block is active
			while (data_array_is_datum_used(data_array, datum_index))
			{
				datum_index += 1;
				if (datum_index >= current_free_index)
					goto VERIFY_FREE_INDEX;
			}
		}

		if (datum_index == -1)
		{
		VERIFY_FREE_INDEX:
			if (current_free_index >= data_array->capacity)
			{
				debug_printf("Failed to add new datum, s_data_array is full.");
				return -1;
			}

			if (current_free_index == -1)
			{
				debug_printf("Failed to add new datum, free index is invalid.");
				return -1;
			}

			datum_index = current_free_index;
		}

		// update page table
		int64* page = &data_array->active_pages[datum_index >> k_page_index_bit_shift];
		*page |= 1i64 << (datum_index & (k_block_per_page - 1));

		// clear memory
		char* data = &data_array->data[datum_index * data_array->datum_size];
		memset(data, 0, data_array->datum_size);

		data_array->active_count++;
		data_array->next_index++;

		if (data_array->first_unallocated_index <= datum_index)
			data_array->first_unallocated_index = datum_index + 1;

		// circle back to first salt and set datum salt
		if (data_array->next_salt == -1)
			data_array->next_salt = 0x80000000u;

		int32 new_salt = data_array->next_salt;
		(*(s_datum_header*)data).salt = new_salt;

		data_array->next_salt++;

		datum_handle new_handle = datum_index | ((int64)new_salt << k_salt_size);

		return new_handle;
	}

	char* datum_get(s_data_array* data_array, datum_handle handle)
	{
		int32 index = handle & 0xFFFFFFFF;
		int32 salt = (handle >> (64 - k_salt_size)) & 0xFFFFFFFF;

		char* result = nullptr;
		if (handle != -1 && index < data_array->first_unallocated_index)
		{
			char* datum = &data_array->data[index * data_array->datum_size];
			if ((*(s_datum_header*)datum).salt == salt)
				result = datum;
		}
		return result;
	}

	char* datum_get_absolute(s_data_array* data_array, int32 index)
	{
		char* result = nullptr;
		if (index != -1 && index >= 0 && index < data_array->first_unallocated_index)
		{
			char* datum = &data_array->data[index * data_array->datum_size];
			if ((*(s_datum_header*)datum).salt != 0)
				result = datum;
		}
		return result;
	}


	int32 data_array_get_index(s_data_array* data_array, int32 index)
	{
		int32 result_index = index;

		if (result_index < 0 || result_index >= data_array->first_unallocated_index)
			return -1;

		// find first index with valid page, starting from specified index.
		while (data_array_is_datum_used(data_array, result_index))
		{
			result_index++;
			if (result_index >= data_array->first_unallocated_index)
				return -1;
		}
		return result_index;
	}

	void data_iterator_new(s_data_array_iterator* iterator, s_data_array* data_array)
	{
		iterator->data_array = data_array;
		iterator->current_datum_handle = -1;
		iterator->current_index = -1;
	}

	char* data_iterator_next(s_data_array_iterator* iterator)
	{
		s_data_array* data_array = iterator->data_array;
		int32 index = data_array_get_index(data_array, iterator->current_index + 1);
		char* result;

		if (index == -1)
		{
			result = nullptr;
			iterator->current_datum_handle = -1;
			iterator->current_index = data_array->capacity;
		}
		else
		{
			result = &data_array->data[data_array->datum_size * index];
			int32 salt = (*(s_datum_header*)result).salt;
			iterator->current_datum_handle = index | ((int64)salt << k_salt_size);
			iterator->current_index = index;
		}
		return result;
	}
}