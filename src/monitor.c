/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mariaalm <mariaalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 16:38:02 by mariaalm          #+#    #+#             */
/*   Updated: 2026/09/21 16:49:57 by mariaalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	stop_simulation(t_table *table)
{
	pthread_mutex_lock(&table->simulation_mutex);
	table->simulation_over = 1;
	pthread_mutex_unlock(&table->simulation_mutex);
	pthread_mutex_lock(&table->scheduler_mutex);
	pthread_cond_broadcast(&table->scheduler_cond);
	pthread_mutex_unlock(&table->scheduler_mutex);
	wake_up_all_coders(table);
}

int	check_bournout(t_table *table)
{
	long	now;
	long	start;
	int		i;
	t_coder	*coder;

	i = 0;
	now = get_timestamp_ms();
	while (i < table->config->number_of_coders)
	{
		coder = &table->coders[i];
		pthread_mutex_lock(&coder->mutex_compile);
		start = coder->last_compile_start;
		pthread_mutex_unlock(&coder->mutex_compile);
		if (now - start > table->config->time_to_burnout)
		{
			log_state(table, coder->id_coder, "burned out");
			return (1);
		}
		i++;
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_table	*table;
	int		over;

	table = (t_table *)arg;
	pthread_mutex_lock(&table->simulation_mutex);
	over = table->simulation_over;
	pthread_mutex_unlock(&table->simulation_mutex);
	while (over == 0)
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
		usleep(1000);
		pthread_mutex_lock(&table->simulation_mutex);
		over = table->simulation_over;
		pthread_mutex_unlock(&table->simulation_mutex);
	}
	return (NULL);
}

int	all_compiled_enough(t_table *table)
{
	int	i;
	int	finished;

	i = 0;
	while (i < table->config->number_of_coders)
	{
		pthread_mutex_lock(&table->coders[i].mutex_compile);
		finished = table->coders[i].compiles_finish;
		pthread_mutex_unlock(&table->coders[i].mutex_compile);
		if (finished < table->config->number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

void	wake_up_all_coders(t_table *table)
{
	int		i;
	t_coder	*coder;

	i = 0;
	while (i < table->config->number_of_coders)
	{
		coder = &table->coders[i];
		pthread_mutex_lock(&coder->mutex_compile);
		pthread_cond_broadcast(&coder->cond_compile);
		pthread_mutex_unlock(&coder->mutex_compile);
		i++;
	}
}
