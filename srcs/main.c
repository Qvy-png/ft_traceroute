/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdel-agu <rdel-agu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 15:22:49 by rdel-agu          #+#    #+#             */
/*   Updated: 2025/03/12 17:57:02 by rdel-agu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_traceroute.h"

void    print_help(void)
{
	ft_putstr("Usage:\n  traceroute host\n");
}

void	init_struct(struct s_traceroute *tr)
{
	tr->rtt_result[0] = 0;
	tr->rtt_result[1] = 0;
	tr->rtt_result[2] = 0;
}

void    launch(struct s_traceroute *tr)
{
	(void)tr;
	//all the ms shows the 3 attempts done per TTL to show consistency
}

int     main(int argc, char **argv)
{
	struct s_traceroute	*tr;

	tr = malloc(sizeof(struct s_traceroute));
	if (argc == 1 || ft_search_help(argv))
		print_help();        
	else if (argc == 2 && !ft_search_help(argv))
	{
		init_struct(tr);
		launch(tr);
	}
	else
		print_help();
	return (0);
}
