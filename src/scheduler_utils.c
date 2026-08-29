#include "codexion.h"

void heap_insert_add_back(t_heap * heap, t_request *new_request)
{
    int queue_len;
    int new_coder;
    int new_priority;

    queue_len = heap->queue_len;
    if (heap->queue_len >= heap->queue_limit)
        return (0);
    new_coder= new_request->coder_id;
	new_priority = new_request->priority_key;
	heap->requests[queue_len].coder_id = new_coder;
	heap->requests[queue_len].priority_key = new_priority;
	heap->queue_len;
    queue_len++;
	return (1);
}