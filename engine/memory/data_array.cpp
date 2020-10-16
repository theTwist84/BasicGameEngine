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

	void initialize_data_array(void* allocated_memory, std::string name, int64 capacity, int64 datum_size)
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

	s_data_array* create_new_data_array(std::string name, int64 capacity, int64 datum_size)
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

	int64 s_data_array::allocation_size()
	{
		return sizeof(s_data_array) + this->capacity * datum_size + sizeof(int64) * ((this->capacity + (k_block_per_page - 1)) >> k_page_index_bit_shift);
	}

	void s_data_array::make_invalid()
	{
		this->flags &= ~_data_array_flags_is_valid;
	}

	void s_data_array::make_valid()
	{
		this->flags |= _data_array_flags_is_valid;
		delete_all();
	}

	void dispose(s_data_array* data_array)
	{	
		memset(data_array, 0, sizeof(s_data_array));
		// TODO: use custom allocator
		free(data_array);
	}

	void s_data_array::delete_all()
	{
		this->first_unallocated_index = 0;
		this->active_count = 0;
		this->next_index = 0;
		this->next_salt = 0x80000000;

		if ((this->flags & _data_array_flags_debug_fill) > 0)
			memset(this->data, k_debug_fill_data, this->datum_size * this->capacity);

		for (int64 i = 0; i < this->capacity; i++)
		{
			(*(s_datum_header*)(this->data + i * this->datum_size)).salt = 0;
		}
		memset(this->active_pages, 0, sizeof(int64) * ((capacity + (k_block_per_page - 1)) >> k_page_index_bit_shift));
	}

	void s_data_array::datum_delete(int32 datum_index)
	{
		char* data = &this->data[datum_index * this->datum_size];

		if((this->flags & _data_array_flags_debug_fill) > 0)
			memset(data, k_debug_fill_data, this->datum_size);

		int64* page = &this->active_pages[datum_index >> k_page_index_bit_shift];
		*page &= ~(1 << (datum_index & (k_block_per_page - 1)));

		// clear the salt from the data, very important
		(*(s_datum_header*)data).salt = 0;

		if (datum_index < this->next_index)
			this->next_index = datum_index;

		if (datum_index + 1 == this->first_unallocated_index)
		{
			char* other_salt = data;
			do
			{
				this->first_unallocated_index -= 1;
				other_salt -= this->datum_size;
			} 
			while (this->first_unallocated_index > 0 && (*(s_datum_header*)other_salt).salt == 0);
		}
		this->active_count -= 1;
	}

	datum_handle s_data_array::datum_new()
	{
		int32 current_free_index = this->first_unallocated_index;
		int32 datum_index = this->next_index;


		if (datum_index >= current_free_index)
		{
			goto VERIFY_FREE_INDEX;
		}
		else
		{
			// while block is active
			while (1i64 << (datum_index & (k_block_per_page - 1)) & this->active_pages[datum_index >> k_page_index_bit_shift])
			{
				datum_index += 1;
				if (datum_index >= current_free_index)
					goto VERIFY_FREE_INDEX;
			}
		}

		if (datum_index == -1)
		{
		VERIFY_FREE_INDEX:
			if (current_free_index >= this->capacity)
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
		int64* page = &this->active_pages[datum_index >> k_page_index_bit_shift];
		*page |= 1i64 << (datum_index & (k_block_per_page - 1));

		// clear memory
		char* data = &this->data[datum_index * this->datum_size];
		memset(data, 0, this->datum_size);

		this->active_count++;
		this->next_index++;

		if (this->first_unallocated_index <= datum_index)
			this->first_unallocated_index = datum_index + 1;

		// circle back to first salt and set datum salt
		if (this->next_salt == -1)
			this->next_salt = 0x80000000u;

		int32 new_salt = this->next_salt;
		(*(s_datum_header*)data).salt = new_salt;

		this->next_salt++;

		datum_handle new_handle = datum_index | ((int64)new_salt << k_salt_size);

		return new_handle;
	}
}