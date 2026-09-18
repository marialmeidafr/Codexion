#include "codexion.h"

int main(int ac, char *av[])
{
    t_config config;
    t_table table;

    if (parser(ac, av, &config) == 0)
    {
        fprintf(stderr, "Error: invalid arguments\n");
        return (1);
    }
    if (init_table(&table, &config) == 0)
    {
        fprintf(stderr, "Error: initialization failed\n");
        return (1);
    }
    start_simulation(&table);
    free_table(&table);
    return(0);
}