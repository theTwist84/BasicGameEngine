#include "queue.h"
#include <stdlib.h>
#include <iostream>
#include "../engine_debug.h"

namespace engine
{
	s_queue* create_new_queue(int64 capacity, int64 element_size)
	{
		int64 memory_size = sizeof(s_queue) + capacity * element_size;

		// TODO: use allocator here
		void* allocated_memory = malloc(memory_size);
		
		memset(allocated_memory, 0, sizeof(s_queue));

		if (allocated_memory == nullptr)
			return nullptr;

		s_queue* new_queue = reinterpret_cast<s_queue*>(allocated_memory);
		
		new_queue->data = (char*)allocated_memory + sizeof(s_queue);
		new_queue->capacity = capacity;
		new_queue->element_size = element_size;
		new_queue->flags = _queue_flags_is_initialized;

		if (ENGINE_DEBUG)
			new_queue->flags |= _queue_flags_debug_fill;

		queue_reset(new_queue);

		return new_queue;
	}

	void dispose_queue(s_queue* queue)
	{
		memset(queue, 0, sizeof(s_queue));
		free(queue);
	}

	char* queue_front(s_queue* queue)
	{
		if (queue->active_count > 0 && queue->front_index != -1)
			return queue->data + queue->front_index * queue->element_size;
		else
			return nullptr;
	}
	char* queue_back(s_queue* queue)
	{
		if (queue->active_count > 0 && queue->back_index != -1)
			return queue->data + queue->back_index * queue->element_size;
		else
			return nullptr;
	}

	bool queue_is_empty(s_queue* queue)
	{
		return queue->active_count == 0;
	}

	bool queue_is_full(s_queue* queue)
	{
		return queue->capacity == queue->active_count;
	}

	void queue_reset(s_queue* queue)
	{
		queue->active_count = 0;
		queue->back_index = -1;
		queue->front_index = -1;
		if (queue->flags & _queue_flags_debug_fill)
			memset(queue->data, k_debug_fill_data, queue->element_size * queue->capacity);
	}

	int64 queue_put(s_queue* queue, char* element)
	{
		if (!queue_is_full(queue))
		{
			if (queue->back_index == -1 && queue->front_index == -1)
			{
				queue->back_index = 0;
				queue->front_index = 0;
			}
			else
				queue->back_index = (queue->back_index + 1) % queue->capacity;

			queue->active_count++;
			memcpy(queue->data + (queue->back_index * queue->element_size), element, queue->element_size);
			return queue->back_index;
		}
		else
			return -1;
	}

	void queue_pop(s_queue* queue)
	{
		if (!queue_is_empty(queue))
		{
			if (queue->flags & _queue_flags_debug_fill)
				memset(queue->data + queue->front_index * queue->element_size, k_debug_fill_data, queue->element_size);

			queue->active_count--;
			queue->front_index = (queue->front_index + 1) % queue->capacity;

			if (queue_is_empty(queue))
				queue_reset(queue);
		}
	}
}