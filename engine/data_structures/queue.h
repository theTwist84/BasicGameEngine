#ifndef __ENGINE_DATA_STRUCTURES_QUEUE_H
#define __ENGINE_DATA_STRUCTURES_QUEUE_H

#include "../engine_definitions.h"
#include "../memory/allocator.h"

namespace engine
{
	enum e_queue_flags
	{
		_queue_flags_is_valid = 1 << 0,
		_queue_flags_is_initialized = 1 << 1,
		_queue_flags_debug_fill = 1 << 2,
	};

	struct s_queue
	{
		int64 capacity;
		int64 element_size;
		int64 active_count;
		int64 front_index;
		int64 back_index;
		int64 flags;
		c_allocator* allocator;
		char* data;
	};

	s_queue* create_new_queue(int64 capacity, int64 element_size, c_allocator* const allocator);
	void dispose_queue(s_queue* queue);

	char* queue_front(s_queue* queue);
	char* queue_back(s_queue* queue);

	bool queue_is_empty(s_queue* queue);
	bool queue_is_full(s_queue* queue);
	void queue_reset(s_queue* queue);

	int64 queue_put(s_queue* queue, char* element);
	void queue_pop(s_queue* queue);

	template <typename t_element>
	class c_queue : public s_queue
	{
	public:

		inline void dispose() { engine::dispose_queue(this); }

		inline t_element* front() { return (t_element*)engine::queue_front(this); }

		inline t_element* back() { return (t_element*)engine::queue_back(this); }

		inline bool is_empty() { return engine::queue_is_empty(this); }

		inline bool is_full() { return engine::queue_is_full(this); }

		inline void reset() { engine::queue_reset(this); }

		inline int64 put(t_element* element) { return engine::queue_put(this, (char*)element); }

		inline void pop() { engine::queue_pop(this); }
	};

	template <typename t_element>
	inline c_queue<t_element>* create_new_queue(int64 capacity, c_allocator* const allocator)
	{
		return (c_queue<t_element>*)engine::create_new_queue(capacity, sizeof(t_element), allocator);
	}
}

#endif