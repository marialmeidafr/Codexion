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
                pthread_cond_signal(&coder->cond_compile);
                pthread_mutex_unlock(&coder->mutex_compile);
            }
            else
            {
                fprintf(stderr, "DEBUG: dispatch failed for coder %d (in queue, dongles busy)\n", coder_id);
                i++;
            }
        }
    }
}

int try_take_dongles(t_coder *coder)
{
    long now;
    int left_dongle_ok;
    int right_dongle_ok;

    if (coder->left_dongle == coder->right_dongle)
        return (0);
    now = get_timestamp_ms();
    left_dongle_ok = (coder->left_dongle->in_use == 0)
                    && (now >= coder->left_dongle->table_return_time
                        + coder->config->dongle_cooldown);
    right_dongle_ok = (coder->right_dongle->in_use == 0)
                    && (now >= coder->right_dongle->table_return_time
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
