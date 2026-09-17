#include "codexion.h"

void request_compile(t_coder *coder)
{
	long 		prioriy;
	t_request 	request;

	if (coder->config->scheduler == FIFO)
		prioriy = get_timestamp_ms();
	else
		prioriy = coder->last_compile_start
		+ coder->config->time_to_burnout;
	pthread_mutex_lock(&coder->scheduler_mutex);
	request.id_coders = coder->id_coder;
	request.priority_request = prioriy;
	insert_on_heap_sift_up(&coder->scheduler_queue, &request);
	pthread_cond_signal(&coder->table->scheduler_cond);
	pthread_mutex_unlock(&coder->scheduler_mutex);
	pthread_mutex_lock(&coder->mutex_compile);
	while (coder->compile_authorized == 0 && coder->table->simulation_over == 0)
		pthread_cond_wait(&coder->cond_compile, &coder->mutex_compile);
	pthread_mutex_unlock(&coder->mutex_compile);
}

void release_dongles(t_coder *coder)
{
	long now;

	pthread_mutex_lock(&coder->table->scheduler_mutex);
	now = get_timestamp_ms();
	coder->left_dongle->in_use = 0;
    coder->left_dongle->table_return_time = now;
    coder->right_dongle->in_use = 0;
    coder->right_dongle->table_return_time = now;
	pthread_cond_signal(&coder->table->scheduler_cond);
	pthread_mutex_unlock(&coder->table->scheduler_mutex);
}