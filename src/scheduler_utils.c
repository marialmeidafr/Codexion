#include "codexion.h"

int heap_insert_add_back(t_heap *heap, t_request *new_request)
{
    if (heap->queue_len >= heap->queue_limit)
        return (0);
    heap->requests[heap->queue_len].id_coders = new_request->id_coders;
    heap->requests[heap->queue_len].priority_request = new_request->priority_request;
    heap->requests[heap->queue_len].sequence = new_request->sequence;
    heap->requests[heap->queue_len].fail_count = new_request->fail_count;
    heap->queue_len++;
    return (1);
}

int compare_index(t_heap *heap, int a, int b)
{
    if (heap->requests[a].fail_count != heap->requests[b].fail_count)
        return (heap->requests[a].fail_count < heap->requests[b].fail_count);
    if (heap->requests[a].priority_request == heap->requests[b].priority_request)
        return (heap->requests[a].sequence > heap->requests[b].sequence);
    return (heap->requests[a].priority_request > heap->requests[b].priority_request);
}

void swap_nodes(t_heap *heap, int a, int b)
{
    t_request tmp;

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