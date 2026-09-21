/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_actions_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mariaalm <mariaalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 16:37:39 by mariaalm          #+#    #+#             */
/*   Updated: 2026/09/21 16:37:42 by mariaalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	push_request(t_coder *coder, long priority)
{
	t_request	request;

	pthread_mutex_lock(&coder->table->scheduler_mutex);
	request.id_coders = coder->id_coder;
	request.priority_request = priority;
	request.sequence = coder->table->request_sequence++;
	request.fail_count = 0;
	insert_on_heap_sift_up(&coder->table->scheduler_queue, &request);
	pthread_cond_signal(&coder->table->scheduler_cond);
	pthread_mutex_unlock(&coder->table->scheduler_mutex);
}

void	request_compile(t_coder *coder)
{
	long	priority;
	int		over;

	if (coder->config->scheduler == FIFO)
		priority = get_timestamp_ms();
	else if (coder->config->scheduler == LIFO)
		priority = -get_timestamp_ms();
	else
		priority = coder->last_compile_start + coder->config->time_to_burnout;
	pthread_mutex_lock(&coder->mutex_compile);
	coder->compile_authorized = 0;
	pthread_mutex_unlock(&coder->mutex_compile);
	push_request(coder, priority);
	pthread_mutex_lock(&coder->mutex_compile);
	pthread_mutex_lock(&coder->table->simulation_mutex);
	over = coder->table->simulation_over;
	pthread_mutex_unlock(&coder->table->simulation_mutex);
	while (coder->compile_authorized == 0 && over == 0)
	{
		pthread_cond_wait(&coder->cond_compile, &coder->mutex_compile);
		pthread_mutex_lock(&coder->table->simulation_mutex);
		over = coder->table->simulation_over;
		pthread_mutex_unlock(&coder->table->simulation_mutex);
	}
	pthread_mutex_unlock(&coder->mutex_compile);
}

void	release_dongles(t_coder *coder)
{
	long	now;

	pthread_mutex_lock(&coder->table->scheduler_mutex);
	now = get_timestamp_ms();
	coder->left_dongle->in_use = 0;
	coder->left_dongle->table_return_time = now;
	coder->right_dongle->in_use = 0;
	coder->right_dongle->table_return_time = now;
	pthread_cond_signal(&coder->table->scheduler_cond);
	pthread_mutex_unlock(&coder->table->scheduler_mutex);
}
