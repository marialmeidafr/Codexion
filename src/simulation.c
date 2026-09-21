/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mariaalm <mariaalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 16:38:20 by mariaalm          #+#    #+#             */
/*   Updated: 2026/09/21 16:38:23 by mariaalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_burnout_clocks(t_table *table)
{
	int	i;

	table->start_time = get_timestamp_ms();
	i = 0;
	while (i < table->config->number_of_coders)
	{
		table->coders[i].last_compile_start = table->start_time;
		i++;
	}
}

static int	create_all_threads(t_table *table, t_threads *th)
{
	int	i;

	if (pthread_create(&th->scheduler, NULL, scheduler_routine, table) != 0)
		return (0);
	if (pthread_create(&th->monitor, NULL, monitor_routine, table) != 0)
		return (0);
	if (pthread_create(&th->ticker, NULL, ticker_routine, table) != 0)
		return (0);
	i = 0;
	while (i < table->config->number_of_coders)
	{
		if (pthread_create(&table->coders[i].thread_coder, NULL, coder_routine,
				&table->coders[i]) != 0)
			return (0);
		i++;
	}
	return (1);
}

static void	join_all_threads(t_table *table, t_threads *th)
{
	int	i;

	pthread_join(th->scheduler, NULL);
	pthread_join(th->monitor, NULL);
	pthread_join(th->ticker, NULL);
	i = 0;
	while (i < table->config->number_of_coders)
	{
		pthread_join(table->coders[i].thread_coder, NULL);
		i++;
	}
}

int	start_simulation(t_table *table)
{
	t_threads	th;

	init_burnout_clocks(table);
	if (create_all_threads(table, &th) == 0)
		return (0);
	join_all_threads(table, &th);
	return (1);
}
