#include "codexion.h"

int main(int ac, char *av[])
{
    t_config config;

    if (parser(ac, av, &config) == 0)
    {
        //if(config.number_of_coders == 0 || config.number_of_compiles_required == 0)
            //return (0);
        fprintf(stderr, "Error: invalid arguments\n");
        return (1);
    }
    return(0);
}