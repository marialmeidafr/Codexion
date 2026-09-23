/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mariaalm <mariaalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 16:37:59 by mariaalm          #+#    #+#             */
/*   Updated: 2026/09/22 18:53:00 by mariaalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int ac, char *av[])
{
	t_config	config;
	t_table		table;

	if (parser(ac, av, &config) == 0)
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (1);
	}
	if (config.number_of_coders == 0)
		return (0);
	if (init_table(&table, &config) == 0)
	{
		fprintf(stderr, "Error: initialization failed\n");
		return (1);
	}
	start_simulation(&table);
	free_table(&table);
	return (0);
}
