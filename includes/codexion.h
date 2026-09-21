/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mariaalm <mariaalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 16:38:46 by mariaalm          #+#    #+#             */
/*   Updated: 2026/09/21 18:20:51 by mariaalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <time.h>
# include <unistd.h>

typedef enum e_sched
{
	FIFO,
	EDF,
	LIFO
}						t_sched;

typedef struct s_table	t_table;
typedef struct s_coder	t_coder;

typedef struct s_config
{
	int					number_of_coders;
	long				time_to_burnout;
	long				time_to_compile;
	long				time_to_debug;
	long				time_to_refactor;
	int					number_of_compiles_required;
	long				dongle_cooldown;
	t_sched				scheduler;
}						t_config;

typedef struct s_threads
{
	pthread_t			scheduler;
	pthread_t			monitor;
	pthread_t			ticker;
}						t_threads;

typedef struct s_request
{
	int					id_coders;
	long				priority_request;
	long				sequence;
	int					fail_count;
}						t_request;

typedef struct s_heap
{
	t_request			*requests;
	int					queue_len;
	int					queue_limit;
}						t_heap;

typedef struct s_dongle
{
	int					id_dongle;
	pthread_mutex_t		mutex_dongle;
	int					in_use;
	pthread_cond_t		cond_dongle;
	long				table_return_time;

}						t_dongle;

typedef struct s_table
{
	t_config			*config;
	t_dongle			*dongles;
	t_coder				*coders;
	long				start_time;
	long				request_sequence;
	int					simulation_over;
	pthread_mutex_t		write_mutex;
	pthread_mutex_t		simulation_mutex;
	pthread_cond_t		simulation_cond;
	pthread_mutex_t		scheduler_mutex;
	pthread_cond_t		scheduler_cond;
	t_heap				scheduler_queue;
}						t_table;

typedef struct s_coder
{
	int					id_coder;
	int					compiles_finish;
	long				last_compile_start;
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	t_config			*config;
	t_table				*table;
	pthread_t			thread_coder;
	pthread_mutex_t		mutex_compile;
	pthread_cond_t		cond_compile;
	int					compile_authorized;
}						t_coder;

// utils
int						ft_isdigit(int c);
int						ft_strcmp(char *s1, char *s2);
long					ft_atol(const char *str);
long					get_timestamp_ms(void);
void					log_state(t_table *table, int coder_id, char *msg);

// parser
int						parser(int ac, char *av[], t_config *config);

// scheduler
void					insert_on_heap_sift_up(t_heap *heap,
							t_request *add_request);
void					find_smallest(t_heap *heap, int current);
void					sift_up(t_heap *heap, int index);
void					remove_coder_on_heap(t_heap *heap, int id_coders);
int						heap_insert_add_back(t_heap *heap,
							t_request *new_request);
int						compare_index(t_heap *heap, int a, int b);
void					swap_nodes(t_heap *heap, int a, int b);
t_request				heap_pop_min(t_heap *heap);

// scheduler thread
void					*scheduler_routine(void *arg);
void					scheduler_dispatch(t_table *table);
int						try_take_dongles(t_coder *coder);
void					*ticker_routine(void *arg);

// init
void					free_table(t_table *table);
int						init_table(t_table *table, t_config *config);

int	init_sync(t_table *table);
int	init_queue(t_table *table, t_config *config);
int	init_allocations(t_table *table, t_config *config);

// coder actions - utils
void					request_compile(t_coder *coder);
void					release_dongles(t_coder *coder);

// coder actions
void					do_compile(t_coder *coder);
void					do_debug(t_coder *coder);
void					do_refactor(t_coder *coder);
int						must_stop(t_coder *coder);
void					*coder_routine(void *arg);

// monitor
void					stop_simulation(t_table *table);
int						check_bournout(t_table *table);
void					*monitor_routine(void *arg);
int						all_compiled_enough(t_table *table);
void					wake_up_all_coders(t_table *table);

// simulation
int						start_simulation(t_table *table);

#endif