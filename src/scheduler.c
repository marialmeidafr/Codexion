#include "codexion.h"

void insert_on_heap_sift_up(t_heap *heap, t_request *add_request)
{
    if (heap_insert_add_back(heap, add_request) == 1)
        sift_up(heap, heap->queue_len - 1);
}

void find_smallest(t_heap *heap, int current)
{
    int smallest;
    int left;
    int right;

    while (1)
    {
        smallest = current;
        left = (2 * current + 1);
        right = (2 * current + 2);
        if (left < heap->queue_len && compare_index(heap, smallest, left) == 1)
			smallest = left;
        if (right < heap->queue_len && compare_index(heap, smallest, right) == 1)
			smallest = right;
        if (smallest != current)
        {
            swap_nodes(heap, current, smallest);
            current = smallest;
        }
        else
            break;
    }
}

void sift_up(t_heap *heap, int index)
{
    int parent_node;

    while (index > 0)
    {
        parent_node = (index - 1) / 2;
        if (compare_index(heap, parent_node, index) == 1)
        {
            swap_nodes(heap, parent_node, index);
            index = parent_node;
        }
        else
            break;
    }
}

void remove_coder_on_heap(t_heap *heap, int id_coders)
{
    int i;
    int last;

    i = 0;
    while (i < heap->queue_len && heap->requests[i].id_coders != id_coders)
        i++;
    if (i == heap->queue_len)
        return; 
    last = heap->queue_len - 1;
    heap->requests[i] = heap->requests[last];
    heap->queue_len--;
    if (i < heap->queue_len)
    {
        sift_up(heap, i);
		find_smallest(heap, i);
    }
}