#include "codexion.h"

void	do_compile(t_coder *coder)
{
	request_compile(coder);
	if (coder->table->simulation_over != 0)
		return ;
	log_state(coder->table, coder->id_coder, "is compiling");
	coder->last_compile_start = get_timestamp_ms();
	usleep(coder->config->time_to_compile * 1000);
	release_dongles(coder);
    coder->compiles_finish++;
}

void	do_debug(t_coder *coder)
{
	log_state(coder->table, coder->id_coder, "is debugging");
    usleep(coder->config->time_to_debug * 1000);
}

void 	do_refactor(t_coder *coder)
{
	log_state(coder->table, coder->id_coder, "is refactoring");
    usleep(coder->config->time_to_refactor * 1000);
}

int must_stop(t_coder *coder)
{
	return (coder->table->simulation_over != 0
		|| coder->compiles_finish >= coder->config->number_of_compiles_required);
}

void *coder_routine(void *arg)
{
	t_coder *coder;
	
	coder = (t_coder*)arg;
	while (!must_stop(coder))
	{
		do_compile(coder);
		if (must_stop(coder))
			break;
		do_debug(coder);
		if (must_stop(coder))
			break;
		do_refactor(coder);
	}
	return NULL;
}