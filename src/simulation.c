#include "codexion.h"

int start_simulation(t_table *table)
{
    pthread_t scheduler_thread;
    pthread_t monitor_thread;
    int       i;

    table->start_time = get_timestamp_ms();
    i = 0;
    while (i < table->config->number_of_coders)
    {
        table->coders[i].last_compile_start = table->start_time;
        i++;
    }
    if (pthread_create(&scheduler_thread, NULL, scheduler_routine, table) != 0)
        return (0);
    if (pthread_create(&monitor_thread, NULL, monitor_routine, table) != 0)
        return (0);
    i = 0;
    while (i < table->config->number_of_coders)
    {
        if (pthread_create(&table->coders[i].thread_coder, NULL, coder_routine, &table->coders[i]) != 0)
            return (0);
        i++;
    }
    pthread_join(scheduler_thread, NULL);
    pthread_join(monitor_thread, NULL);
    i = 0;
    while (i < table->config->number_of_coders)
    {
        pthread_join(table->coders[i].thread_coder, NULL);
        i++;
    }
    return (1);
}