/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mariaalm <mariaalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 16:37:52 by mariaalm          #+#    #+#             */
/*   Updated: 2026/09/21 18:20:35 by mariaalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongle(t_table *table, t_config *config)
{
	int	i;

	i = 0;
	while (i < config->number_of_coders)
	{
		table->dongles[i].id_dongle = i;
		table->dongles[i].in_use = 0;
		table->dongles[i].table_return_time = 0;
		pthread_mutex_init(&table->dongles[i].mutex_dongle, NULL);
		pthread_cond_init(&table->dongles[i].cond_dongle, NULL);
		i++;
	}
	return (1);
}

static void	init_coder(t_table *table, t_config *config)
{
	int	j;

	j = 0;
	while (j < config->number_of_coders)
	{
		table->coders[j].id_coder = j + 1;
		table->coders[j].compiles_finish = 0;
		table->coders[j].last_compile_start = 0;
		table->coders[j].config = config;
		table->coders[j].table = table;
		table->coders[j].compile_authorized = 0;
		pthread_mutex_init(&table->coders[j].mutex_compile, NULL);
		pthread_cond_init(&table->coders[j].cond_compile, NULL);
		table->coders[j].left_dongle = &table->dongles[j];
		table->coders[j].right_dongle = &table->dongles[(j + 1)
			% config->number_of_coders];
		j++;
	}
}

static void	destroy_table_mutexes(t_table *table)
{
	pthread_mutex_destroy(&table->write_mutex);
	pthread_mutex_destroy(&table->simulation_mutex);
	pthread_cond_destroy(&table->simulation_cond);
	pthread_mutex_destroy(&table->scheduler_mutex);
	pthread_cond_destroy(&table->scheduler_cond);
	if (table->scheduler_queue.requests != NULL)
	{
		free(table->scheduler_queue.requests);
		table->scheduler_queue.requests = NULL;
	}
}

void	free_table(t_table *table)
{
	int	i;

	destroy_table_mutexes(table);
	if (table->dongles != NULL)
	{
		i = -1;
		while (++i < table->config->number_of_coders)
		{
			pthread_mutex_destroy(&table->dongles[i].mutex_dongle);
			pthread_cond_destroy(&table->dongles[i].cond_dongle);
		}
		free(table->dongles);
		table->dongles = NULL;
	}
	if (table->coders != NULL)
	{
		i = -1;
		while (++i < table->config->number_of_coders)
		{
			pthread_mutex_destroy(&table->coders[i].mutex_compile);
			pthread_cond_destroy(&table->coders[i].cond_compile);
		}
		free(table->coders);
		table->coders = NULL;
	}
}

int	init_table(t_table *table, t_config *config)
{
	table->dongles = NULL;
	table->coders = NULL;
	table->scheduler_queue.requests = NULL;
	table->config = config;
	table->start_time = 0;
	table->simulation_over = 0;
	table->request_sequence = 0;
	pthread_mutex_init(&table->write_mutex, NULL);
	pthread_mutex_init(&table->simulation_mutex, NULL);
	pthread_cond_init(&table->simulation_cond, NULL);
	pthread_mutex_init(&table->scheduler_mutex, NULL);
	pthread_cond_init(&table->scheduler_cond, NULL);
	table->scheduler_queue.requests = malloc(sizeof(t_request)
			* config->number_of_coders);
	if (table->scheduler_queue.requests == NULL)
		return (free_table(table), 0);
	table->scheduler_queue.queue_len = 0;
	table->scheduler_queue.queue_limit = config->number_of_coders;
	table->dongles = malloc(sizeof(t_dongle) * config->number_of_coders);
	if (table->dongles == NULL)
		return (free_table(table), 0);
	init_dongle(table, config);
	table->coders = malloc(sizeof(t_coder) * config->number_of_coders);
	if (table->coders == NULL)
		return (free_table(table), 0);
	init_coder(table, config);
	return (1);
}
