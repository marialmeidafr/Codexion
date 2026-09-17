#include "codexion.h"

void *scheduler_routine(void *arg)
{
    t_table *table;

    table = (t_table *)arg;
    pthread_mutex_lock(&table->scheduler_mutex);
    while(table->simulation_over == 0)
    {
        scheduler_dispatch(table);
        pthread_cond_wait(&table->scheduler_cond, &table->scheduler_mutex);
    }
    pthread_mutex_unlock(&table->scheduler_mutex);
    return (NULL);
}

void scheduler_dispatch(t_table *table)
{
    int i;
    int dispatched_someone;
    int coder_id;
    t_coder *coder;

    dispatched_someone = 1;
    while (dispatched_someone == 1)
    {
        dispatched_someone = 0;
        i = 0;
        while (i < table->scheduler_queue.queue_len)
        {
            coder_id = table->scheduler_queue.requests[i].id_coders;
            coder = &table->coders[coder_id - 1];
            if (try_take_dongles(coder) == 1)
            {
                remove_coder_on_heap(&table->scheduler_queue, coder_id);
                dispatched_someone = 1;
                pthread_mutex_lock(&coder->mutex_compile);
                coder->compile_authorized = 1;
                phtread_cond_signal(&coder->cond_compile);
                pthread_mutex_unlock(&coder->mutex_compile);
            }
            else
                i++;
        }
    }
}

int try_take_dongles(t_coder *coder)
{
    long now;
    int left_dongle_ok;
    int right_dongle_ok;

    now = get_timestamp_ms();

    left_dongle_ok = (coder->left_dongle->in_use == 0)
                    && (now >= coder->left_dongle->table_return_time
                        + coder->config->dongle_cooldown);
    
    right_dongle_ok = (coder->right_dongle->in_use == 0)
                    && (now >= coder->left_dongle->table_return_time
                        + coder->config->dongle_cooldown);
    
    if (left_dongle_ok && right_dongle_ok)
    {
        coder->left_dongle->in_use = 1;
        log_state(coder->table, coder->id_coder, "has taken a dongle");
        coder->right_dongle->in_use = 1;
        log_state(coder->table, coder->id_coder, "has taken a dongle");
        return (1);
    }
    return (0);
}