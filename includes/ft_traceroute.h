/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_traceroute.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdel-agu <rdel-agu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 15:27:32 by rdel-agu          #+#    #+#             */
/*   Updated: 2025/03/12 17:55:59 by rdel-agu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>

struct s_traceroute {
    
    int rtt_result[3];
};


// utils
void    ft_putstr(char *str);
int     ft_strlen(char *str);
int     ft_search_help(char **tab);
int     ft_strcmp(char *s1, char *s2);



#endif