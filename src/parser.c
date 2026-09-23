/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mariaalm <mariaalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 16:38:05 by mariaalm          #+#    #+#             */
/*   Updated: 2026/09/23 12:27:06 by mariaalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	config_parser(char *av[], t_config *config);

int	parser(int ac, char *av[], t_config *config)
{
	int	i;
	int	j;

	if (av == NULL || ac != 9)
		return (0);
	j = 1;
	while (j < 8)
	{
		i = 0;
		if (av[j][i] == '\0')
			return (0);
		while (av[j][i])
		{
			if (!ft_isdigit(av[j][i]))
				return (0);
			i++;
		}
		j++;
	}
	if (ft_strcmp(av[8], "fifo") != 0 && ft_strcmp(av[8], "edf")
		&& ft_strcmp(av[8], "lifo") != 0)
		return (0);
	if (config_parser(av, config) == 0)
		return (0);
	return (1);
}

static int	config_parser(char *av[], t_config *config)
{
	config->number_of_coders = (int)ft_atol(av[1]);
	config->time_to_burnout = ft_atol(av[2]);
	config->time_to_compile = ft_atol(av[3]);
	config->time_to_debug = ft_atol(av[4]);
	config->time_to_refactor = ft_atol(av[5]);
	config->number_of_compiles_required = (int)ft_atol(av[6]);
	config->dongle_cooldown = ft_atol(av[7]);
	if (ft_strcmp(av[8], "fifo") == 0)
		config->scheduler = FIFO;
	if (ft_strcmp(av[8], "edf") == 0)
		config->scheduler = EDF;
	if (ft_strcmp(av[8], "lifo") == 0)
		config->scheduler = LIFO;
	if (config->number_of_coders < 0 || config->time_to_burnout < 0
		|| config->time_to_compile < 0 || config->time_to_debug < 0
		|| config->time_to_refactor < 0
		|| config->number_of_compiles_required < 0
		|| config->dongle_cooldown < 0)
		return (0);
	return (1);
}
