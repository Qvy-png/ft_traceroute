/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qvy <qvy@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 15:22:49 by rdel-agu          #+#    #+#             */
/*   Updated: 2025/04/15 22:11:48 by qvy              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_traceroute.h"

// void    print_help(void)
// {
// 	ft_putstr("Usage:\n  traceroute host\n");
// }

// void	init_struct(struct s_traceroute *tr)
// {
// 	tr->rtt_result[0] = 0;
// 	tr->rtt_result[1] = 0;
// 	tr->rtt_result[2] = 0;
// }

// void    launch(struct s_traceroute *tr)
// {
// 	(void)tr;
// 	//all the ms shows the 3 attempts done per TTL to show consistency
// }

// int     main(int argc, char **argv)
// {
// 	struct s_traceroute	*tr;

// 	tr = malloc(sizeof(struct s_traceroute));
// 	if (argc == 1 || ft_search_help(argv))
// 		print_help();        
// 	else if (argc == 2 && !ft_search_help(argv))
// 	{
// 		init_struct(tr);
// 		launch(tr);
// 	}
// 	else
// 		print_help();
// 	return (0);
// }


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAX_TTL 30
#define TIMEOUT_SEC 1
#define NUM_PROBES 3

int main(int argc, char *argv[])	{

    if (argc != 2)	{

        printf("Usage: ft_traceroute <destination>\n");
        return EXIT_FAILURE;
    }

    struct addrinfo hints, *res;
    int udp_sock, icmp_sock;
    struct sockaddr_in dest_addr;
    char ip_str[INET_ADDRSTRLEN];

    // Resolve destination
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    if (getaddrinfo(argv[1], NULL, &hints, &res) != 0)	{

        perror("getaddrinfo");
        return EXIT_FAILURE;
    }
    memcpy(&dest_addr, res->ai_addr, sizeof(dest_addr));
    freeaddrinfo(res);

    inet_ntop(AF_INET, &dest_addr.sin_addr, ip_str, sizeof(ip_str));
    printf("traceroute to %s (%s), %d hops max, 60 byte packets\n", argv[1], ip_str, MAX_TTL);

    // Set a valid UDP port (33434 is commonly used for traceroute)
    dest_addr.sin_port = htons(33434);

    // Create UDP socket
    udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_sock < 0)	{

		perror("socket");
        return EXIT_FAILURE;
    }

    // Create raw ICMP socket
    icmp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (icmp_sock < 0)	{

        perror("socket");
        close(udp_sock);
        return EXIT_FAILURE;
    }

    for (int ttl = 1; ttl <= MAX_TTL; ttl++)	{

        struct timeval start_time, end_time;
        int successful_probes = 0;
        char ip_addresses[NUM_PROBES][INET_ADDRSTRLEN];
        double rtts[NUM_PROBES];

		// does 3 "pings" to target host
        for (int probe = 0; probe < NUM_PROBES; probe++)	{
			
            gettimeofday(&start_time, NULL);

            // Set TTL
            if (setsockopt(udp_sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)	{

                perror("setsockopt");
                break;
            }

            // Send UDP packet
            unsigned char packet[1] = {0};  // Minimal packet with 1 byte
            if (sendto(udp_sock, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)	{

                perror("sendto");
                break;
            }

            // Set up select for receiving ICMP response
            fd_set read_fds;
            struct timeval timeout;
            FD_ZERO(&read_fds);
            FD_SET(icmp_sock, &read_fds);
            timeout.tv_sec = TIMEOUT_SEC;
            timeout.tv_usec = 0;

            // Wait for ICMP response
            int ret = select(icmp_sock + 1, &read_fds, NULL, NULL, &timeout);
            if (ret > 0 && FD_ISSET(icmp_sock, &read_fds))	{

                char buf[512];
                struct sockaddr_in recv_addr;
                socklen_t addr_len = sizeof(recv_addr);
                ssize_t len = recvfrom(icmp_sock, buf, sizeof(buf), 0, (struct sockaddr *)&recv_addr, &addr_len);
                if (len > 0)	{

                    char host[NI_MAXHOST];
                    if (getnameinfo((struct sockaddr *)&recv_addr, addr_len, host, sizeof(host), NULL, 0, 0) != 0)
                        strncpy(host, "unknown", sizeof(host));
                    gettimeofday(&end_time, NULL);
                    double rtt = (double) ((end_time.tv_sec * 1000000 + end_time.tv_usec) - (start_time.tv_sec * 1000000 + start_time.tv_usec)) / 1000.0;
                    rtts[probe] = rtt;
                    successful_probes++;

                    // Store IP address for later use
                    inet_ntop(AF_INET, &recv_addr.sin_addr, ip_addresses[probe], sizeof(ip_addresses[probe]));
                }
            } else	{

                rtts[probe] = -1;  // Timeout
                strcpy(ip_addresses[probe], "*");
            }
        }

        // Print hop information
        printf("%2d  ", ttl);
        for (int probe = 0; probe < NUM_PROBES; probe++)	{

            if (rtts[probe] >= 0)	{

                printf("%s (%s)  %.2f ms", (probe == 0) ? ip_addresses[probe] : "", ip_addresses[probe], rtts[probe]);
            } else	{

                printf("*");
            }
            if (probe < NUM_PROBES - 1)	{

                printf("  ");
            }
        }
        printf("\n");

        // If the TTL exceeds the maximum value (maximum hops)
        if (ttl == MAX_TTL) {
            break;
        }
    }

    // Cleanup
    close(udp_sock);
    close(icmp_sock);
    return EXIT_SUCCESS;
}
