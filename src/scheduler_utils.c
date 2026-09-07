#include "codexion.h"

int heap_insert_add_back(t_heap *heap, t_request *new_request)
{
    if (heap->queue_len >= heap->queue_limit)
        return (0);
    heap->requests[heap->queue_len].id_coders = new_request->id_coders;
    heap->requests[heap->queue_len].priority_request = new_request->priority_request;
    heap->queue_len++;
    return (1);
}

int compare_index(t_heap *heap, int a, int b)
{
    return (heap->requests[a].priority_request > heap->requests[b].priority_request);
}

void	swap_nodes(t_heap *heap, int a, int b)
{
	t_request	tmp;

	tmp = heap->requests[a];
	heap->requests[a] = heap->requests[b];
	heap->requests[b] = tmp;
}


t_request heap_pop_min(t_heap *heap)
{
    t_request min;

    min = heap->requests[0];
    remove_coder_on_heap(heap, min.id_coders);
    return (min);
}