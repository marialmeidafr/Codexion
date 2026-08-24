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
}           t_config

#endif