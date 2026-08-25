#ifndef CODEXION_H
# define CODEXION_H

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef enum e_sched
{
	FIFO,
	EDF
}						t_sched;

typedef struct s_table	t_table;
typedef struct s_coder	t_coder;

typedef struct s_config
{
    int     number_of_coders;
    long    time_to_burnout;
    long    time_to_compile;
    long    time_to_debug;
    long    time_to_refactor;
    int     number_of_compiles_required;
    long    dongle_cooldown;
    t_sched scheduler;      //t_sched é um tipo de dado personalizado 
    // criado com um enum para representar qual o algoritmo de agendamento 
    // definido na linha de comandos.
}           t_config;

typedef struct s_request
{
    int id_coders;
    long priority_request;
}                       t_request;

typedef struct s_heap
{
    t_request           *requests;
    int                 queue_len;
    int                 queue_limit;
}                       t_heap;

typedef struct s_dongle
{
    int                 id_dongle;
    pthread_mutex_t		mutex_dongle;
    int                 in_use;
    pthread_cond_t      cond_dongle; //esperar sem busy-wait
    long				table_return_time; //qnd pode ser pego de novo

}                       t_dongle;

typedef struct s_table
{
    t_config *config;
    t_dongle *dongles;
    t_coder *coders;
    long    start_time;
    int					simulation_over;
	pthread_mutex_t		write_mutex;
	pthread_mutex_t		simulation_mutex;
	pthread_cond_t		simulation_cond;
	pthread_mutex_t		scheduler_mutex;
	pthread_cond_t		scheduler_cond;
	t_heap				scheduler_queue;
}                       t_table;

typedef struct s_coder
{
    int                 id_coder;
    int                 compiles_finish;
    long                last_compile_start;
    t_dongle            *left_dongle;
    t_dongle            *right_dongle;
    t_config            *config;
    t_table			    *table;
    pthread_t		    thread_coder;
    pthread_mutex_t		mutex_compile;
    pthread_cond_t      cond_compile;
    int					compile_authorized;
}                       t_coder;

//utils
int ft_isdigit(int c);
int ft_strcmp(char *s1, char *s2);
long ft_atol(const char *str);

//parser
int parser(int ac, char *av[], t_config *config);

#endif