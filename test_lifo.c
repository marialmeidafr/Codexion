#include "codexion.h"
#include <stdio.h>

static void print_order(t_heap *heap)
{
    t_request req;

    printf("Ordem de despacho (LIFO simulado):\n");
    while (heap->queue_len > 0)
    {
        req = heap_pop_min(heap);
        printf("  coder_id=%d (priority=%ld)\n", req.id_coders, req.priority_request);
    }
}

int main(void)
{
    t_heap heap;
    t_request req;

    heap.requests = malloc(sizeof(t_request) * 3);
    heap.queue_len = 0;
    heap.queue_limit = 3;

    /* Simula 3 pedidos chegando em ordem: A (t=100), B (t=150), C (t=200) */
    /* Prioridade LIFO = -timestamp */

    req.id_coders = 1; /* representa "A", chegou em t=100 */
    req.priority_request = -100;
    insert_on_heap_sift_up(&heap, &req);

    req.id_coders = 2; /* representa "B", chegou em t=150 */
    req.priority_request = -150;
    insert_on_heap_sift_up(&heap, &req);

    req.id_coders = 3; /* representa "C", chegou em t=200 (o mais recente) */
    req.priority_request = -200;
    insert_on_heap_sift_up(&heap, &req);

    printf("Esperado: coder_id=3 primeiro (chegou por ultimo), depois 2, depois 1\n\n");
    print_order(&heap);

    free(heap.requests);
    return (0);
}