#include "codexion.h"

static void build_timeout(struct timespec *timeout)
{
    clock_gettime(CLOCK_REALTIME, timeout);
    timeout->tv_nsec += 5000000;
    if (timeout->tv_nsec >= 1000000000)
    {
        timeout->tv_sec += 1;
        timeout->tv_nsec -= 1000000000;
    }
}

void *scheduler_routine(void *arg)
{
    t_table         *table;
    struct timespec  timeout;
    int              over;

    table = (t_table *)arg;
    pthread_mutex_lock(&table->simulation_mutex);
    over = table->simulation_over;
    pthread_mutex_unlock(&table->simulation_mutex);
    pthread_mutex_lock(&table->scheduler_mutex);
    while (over == 0)
    {
        scheduler_dispatch(table);
        build_timeout(&timeout);
        pthread_cond_timedwait(&table->scheduler_cond, &table->scheduler_mutex, &timeout);
        pthread_mutex_lock(&table->simulation_mutex);
        over = table->simulation_over;
        pthread_mutex_unlock(&table->simulation_mutex);
    }
    pthread_mutex_unlock(&table->scheduler_mutex);
    return (NULL);
}

static void grant_request(t_coder *coder)
{
    pthread_mutex_lock(&coder->mutex_compile);
    coder->compile_authorized = 1;
    pthread_cond_signal(&coder->cond_compile);
    pthread_mutex_unlock(&coder->mutex_compile);
}

static void requeue_pending(t_table *table, t_heap *pending)
{
    t_request req;

    while (pending->queue_len > 0)
    {
        req = heap_pop_min(pending);
        insert_on_heap_sift_up(&table->scheduler_queue, &req);
    }
}

void scheduler_dispatch(t_table *table)
{
    t_heap      pending;
    t_request   req;
    t_coder     *coder;

    pending.requests = malloc(sizeof(t_request) * table->scheduler_queue.queue_limit);
    pending.queue_len = 0;
    pending.queue_limit = table->scheduler_queue.queue_limit;
    while (table->scheduler_queue.queue_len > 0)
    {
        req = heap_pop_min(&table->scheduler_queue);
        coder = &table->coders[req.id_coders - 1];
        if (try_take_dongles(coder) == 1)
            grant_request(coder);
        else
            insert_on_heap_sift_up(&pending, &req);
    }
    requeue_pending(table, &pending);
    free(pending.requests);
}