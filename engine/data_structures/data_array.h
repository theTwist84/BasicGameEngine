#ifndef __ENGINE_DATA_STRUCTURES_DATA_ARRAY_H
#define __ENGINE_DATA_STRUCTURES_DATA_ARRAY_H

#include "../engine_definitions.h"
#include "../memory/allocator.h"
#include <string>


/*
Fixed size memory pool with array-like properties.

Memory is preallocated on creation and is subdivided into datum of size datum_size.

the main struct contains an index into the next datum to look first and the index of the first unallocated datum. Any index after this one is empty.
When removing blocks, the first unllacoted datum index is updated if possible. The next index is also updated when adding (+1) and deleting (moved to the delete position + 1)
Active blocks are stored in pages and we can fit 64 block per page.
*/

namespace engine
{

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
		int32 capacity;
		int64 datum_size;
		int32 next_index;
		int32 first_unallocated_index;
		int64* active_pages;
		int64 active_count;
		int32 next_salt;
		int32 flags;
		c_allocator* allocator;
		char* data;
	};

	struct s_data_array_iterator
	{
		s_data_array* data_array;
		datum_handle current_datum_handle;
		int32 current_index;
	};

	s_data_array* create_new_data_array(std::string name, int32 maximum_count, int64 datum_size, c_allocator* const allocator);

	void dispose_data_array(s_data_array* const data_array);

	int64 allocation_size(s_data_array* const data_array);

	void delete_all(s_data_array* const data_array);

	void make_invalid(s_data_array* const data_array);
	void make_valid(s_data_array* const data_array);

	void datum_delete(s_data_array* const data_array, datum_handle handle);
	void datum_delete(s_data_array* const data_array, int32 datum_index);

	datum_handle datum_new(s_data_array* const data_array);

	char* datum_get(s_data_array* const data_array, datum_handle handle);
	char* datum_get_absolute(s_data_array* const data_array, int32 index);

	void data_iterator_new(s_data_array_iterator* const iterator, s_data_array* const array);
	char* data_iterator_next(s_data_array_iterator* const iterator);

	template <typename t_datum>
	class c_data_array : public s_data_array
	{
		static_assert(__is_base_of(s_datum_header, t_datum));

	public:

		inline void dispose_data_array()
		{
			engine::dispose_data_array(this);
		}

		inline int64 allocation_size()
		{
			engine::allocation_size(this);
		}

		inline void delete_all()
		{
			engine::delete_all(this);
		}

		inline void make_invalid()
		{
			engine::make_invalid(this);
		}

		inline void make_valid()
		{
			engine::make_valid(this);
		}

		inline void datum_delete(datum_handle handle)
		{
			engine::datum_delete(this, handle);
		}

		inline void datum_delete(int32 datum_index)
		{
			engine::datum_delete(this, datum_index);
		}

		inline datum_handle datum_new()
		{
			return engine::datum_new(this);
		}

		inline t_datum* datum_get(datum_handle handle)
		{
			return (t_datum*)engine::datum_get(this, handle);
		}

		inline t_datum* datum_get_absolute(int32 index)
		{
			return (t_datum*)engine::datum_get_absolute(this, index);
		}

	};

	template <typename t_datum>
	inline c_data_array<t_datum>* create_new_data_array(std::string name, int32 maximum_count, c_allocator* const allocator)
	{
		static_assert(__is_base_of(s_datum_header, t_datum));
		return (c_data_array<t_datum>*)engine::create_new_data_array(name, maximum_count, sizeof(t_datum), allocator);
	}

	template <typename t_datum>
	class c_data_array_iterator : public s_data_array_iterator
	{
		static_assert(__is_base_of(s_datum_header, t_datum));

	public:

		inline c_data_array_iterator(c_data_array<t_datum>* array)
		{
			engine::data_iterator_new(this, array);
		}

		inline t_datum* data_iterator_next()
		{
			return (t_datum*)engine::data_iterator_next(this);
		}

	};
}

#endif