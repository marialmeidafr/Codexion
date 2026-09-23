/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mariaalm <mariaalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 16:17:26 by mariaalm          #+#    #+#             */
/*   Updated: 2026/09/23 12:00:55 by mariaalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	do_compile(t_coder *coder)
{
	request_compile(coder);
	pthread_mutex_lock(&coder->table->simulation_mutex);
	if (coder->table->simulation_over != 0)
	{
		pthread_mutex_unlock(&coder->table->simulation_mutex);
		return ;
	}
	pthread_mutex_unlock(&coder->table->simulation_mutex);
	usleep(coder->config->time_to_compile * 1000);
	release_dongles(coder);
	pthread_mutex_lock(&coder->mutex_compile);
	coder->compiles_finish++;
	pthread_mutex_unlock(&coder->mutex_compile);
}

void	do_debug(t_coder *coder)
{
	log_state(coder->table, coder->id_coder, "is debugging");
	usleep(coder->config->time_to_debug * 1000);
}

void	do_refactor(t_coder *coder)
{
	log_state(coder->table, coder->id_coder, "is refactoring");
	usleep(coder->config->time_to_refactor * 1000);
}

int	must_stop(t_coder *coder)
{
	int	over;
	int	finished;

	pthread_mutex_lock(&coder->table->simulation_mutex);
	over = coder->table->simulation_over;
	pthread_mutex_unlock(&coder->table->simulation_mutex);
	pthread_mutex_lock(&coder->mutex_compile);
	finished = coder->compiles_finish;
	pthread_mutex_unlock(&coder->mutex_compile);
	return (over != 0
		|| finished >= coder->config->number_of_compiles_required);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!must_stop(coder))
	{
		do_compile(coder);
		if (must_stop(coder))
			break ;
		do_debug(coder);
		if (must_stop(coder))
			break ;
		do_refactor(coder);
	}
	return (NULL);
}
