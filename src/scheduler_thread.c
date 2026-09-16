#include "codexion.h"

void *scheduler_routine(void *arg)
{
    t_table *table;

    table = (t_table *)arg;
    pthread_mutex_lock(&table->scheduler_mutex);
    while(table->simulation_over == 0)
    {
        scheduler_dispatch(table); //to do
        pthread_cond_wait(&table->scheduler_cond, &table->scheduler_mutex);
    }
    pthread_mutex_unlock(&table->scheduler_mutex);
    return (NULL);
}

void scheduler_dispatch(t_table *table)
{
    int i;
    int dispatched_someone;

    dispatched_someone = 1;
    while (dispatched_someone == 1)
    {
        dispatched_someone = 0;
        i = 0;
        while (i < table->scheduler_queue.queue_len)
        {
            try_take_dongles(coder); //to do
            if(try_take_dongles)
            {
                remove_coder_on_heap();
                dispatched_someone = 1;
                i--;
            }
            else
                i++;
        }
    }
}