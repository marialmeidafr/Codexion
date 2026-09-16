#include "codexion.h"

int ft_isdigit(int c)
{
    if (c >= '0' && c <= '9')
        return (1);
    else
        return (0);
}

int ft_strcmp(char *s1, char *s2)
{
    int i;
    int j;

    i = 0;
    j = 0;

    while (s1[i] && (s1[i] == s2[j]))
    {
        i++;
        j++;
    }
    return (s1[i] - s2[j]);
}

long ft_atol(const char *str)
{
    long result;
    int i;
    int signal;

    i = 0;
    result = 0;
    signal = 1;

    while(str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
        i++;
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            signal *= -1;
        i++;

    }
    while(str[i] >= '0' && str[i] <= '9')
    {
        result = (result * 10) + (str[i] - '0');
        i++;
    }
    return(signal * result);
}

long get_timestamp_ms(void)
{
    struct timeval tv;
    long total_em_ms;

    gettimeofday(&tv, NULL);
    total_em_ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    return (total_em_ms);
}

void log_state(t_table *table, int coder_id, char *msg)
{
    long timestamp;

    pthread_mutex_lock(&table->write_mutex);
    timestamp = get_timestamp_ms() - table->start_time;
    printf("%ld %d %s\n", timestamp, coder_id, msg);
    pthread_mutex_unlock(&table->write_mutex);
}
