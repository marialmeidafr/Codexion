#include "codexion.h"

void stop_simulation(t_table *table)
{
    table->simulation_over = 1;
    pthread_mutex_lock(&table->scheduler_mutex);
    pthread_cond_broadcast(&table->scheduler_cond);
    pthread_mutex_unlock(&table->scheduler_mutex);
    wake_up_all_coders(table);
}

int check_bournout(t_table *table)
{
    long now;
    int i;
    t_coder *coder;

    i = 0;
    now = get_timestamp_ms();
    while (i < table->config->number_of_coders)
    {
        coder = &table->coders[i];
        if (now - coder->last_compile_start > table->config->time_to_burnout)
        {
            log_state(table, coder->id_coder, "burned out");
            return (1);
        }
        i++;
    }
    return (0);
}
void *monitor_routine(void *arg)
{
    t_table *table;

    table = (t_table *)arg;
    while (table->simulation_over == 0)
    {   
        if (check_bournout(table))
        {
            stop_simulation(table);
            return (NULL);
        }
        if (all_compiled_enough(table))
        {
            stop_simulation(table);
            return (NULL);
        }
        usleep (1000);
    }
    return (NULL);
}

int all_compiled_enough(t_table *table)
{
    int i;

    i = 0;
    while (i < table->config->number_of_coders)
    {
        if(table->coders[i].compiles_finish < table->config->number_of_compiles_required)
            return (0);
        i++;
    }
    return (1);
}

void wake_up_all_coders(t_table *table)
{
    int i;
    t_coder *coder;

    i = 0;
    while(i < table->config->number_of_coders)
    {
        coder = &table->coders[i];
        pthread_mutex_lock(&coder->mutex_compile);
        pthread_cond_broadcast(&coder->cond_compile);
        pthread_mutex_unlock(&coder->mutex_compile);
        i++;
    }
}