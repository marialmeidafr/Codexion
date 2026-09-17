#include "codexion.h"

int main(int ac, char *av[])
{
    t_config config;

    if (parser(ac, av, &config) == 0)
    {
        fprintf(stderr, "Error: invalid arguments\n");
        return (1);
    }
    return(0);
}