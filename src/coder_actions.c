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
    coder->compiles_done++;
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