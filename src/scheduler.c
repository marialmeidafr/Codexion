#include "codexion.h"

void heap_insert_sift_up(t_heap *heap, t_request *add_request)
{
    int index;
    int parent_node;

    if (heap_insert_add_back(heap, add_request) == 1)
    {
        index = heap->queue_len - 1;
        while (index > 0)
        {
            parent_node = (index - 1) / 2;
            if(compare_index(heap, parent_node, index) == 1)
            {
                swap_nodes(heap, parent_node, index);
                index = parent_node;
            }
            else
                break;
        }
    }
}