/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdel-agu <rdel-agu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 15:31:16 by rdel-agu          #+#    #+#             */
/*   Updated: 2025/03/12 17:41:45 by rdel-agu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_traceroute.h"

int     ft_search_help(char **tab)
{
	int i;

	i = 0;
	while (tab[i] != NULL)
	{
		if (ft_strcmp(tab[i], "--help") == 0)
			return (1);
		i++;
	}
	return (0);
}

int	    ft_strcmp(char *s1, char *s2)
{
	int i;

	i = 0;
	while (s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0')
		i++;
	return (s1[i] - s2[i]);
}

int     ft_strlen(char *str)
{
	int i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void    ft_putstr(char *str)
{
	write(1, str, ft_strlen(str));
}
