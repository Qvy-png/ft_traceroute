/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qvy <qvy@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 15:22:49 by rdel-agu          #+#    #+#             */
/*   Updated: 2025/05/20 14:03:59 by qvy              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_traceroute.h"

// void    print_help(void)
// {
// 	ft_putstr("Usage:\n  traceroute host\n");
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

#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"
#define CLR "\e[0m"

#define MAX_TTL 30
#define TIMEOUT_SEC 1
#define NUM_PROBES 3

void    s_cpy(char *dst, char *str)
{
    int i;

    i = -1;
    while (str[++i])
        dst[i] = str[i];
}

void    s_clr_addr(char *str)
{
    int i;

    i = -1;
    while (++i < INET_ADDRSTRLEN)
        str[i] = '\0';
}

int main(int argc, char *argv[])	{

    if (argc != 2)	{

        printf("Usage: ft_traceroute <destination>\n");
        return EXIT_FAILURE;
    }

    struct addrinfo hints, *res;
    int udp_sock, icmp_sock;
    struct sockaddr_in dest_addr;
    char ip_str[INET_ADDRSTRLEN];
    char host[NUM_PROBES][512];
    // struct timeval              tv_out;
    int				frag = IP_PMTUDISC_DONT;

    char    prev_address[INET_ADDRSTRLEN];

    s_clr_addr(prev_address);

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

        struct  timeval start_time, end_time;
        // int successful_probes = 0;
        char    ip_addresses[NUM_PROBES][INET_ADDRSTRLEN];
        double  rtts[NUM_PROBES];
        
		// does 3 "pings" to target host
        for (int probe = 0; probe < NUM_PROBES; probe++)	{
			
            gettimeofday(&start_time, NULL);

            // Set TTL
            if (setsockopt(udp_sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)	{

                perror("setsockopt");
                break;
            }
            // // Set timeout
            // memset(&tv_out, 0, sizeof(tv_out)); // initialize memory of tv_out
            // tv_out.tv_usec = 10; // 1s timout
            // tv_out.tv_sec = 0; // 1s timout
            // setsockopt(udp_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof tv_out);

            // Send UDP packet
            unsigned char packet[1] = {0};  // Minimal packet with 1 byte
            if (sendto(udp_sock, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)	{

                perror("sendto");
                break;
            }

            if (setsockopt(udp_sock, IPPROTO_IP, IP_MTU_DISCOVER, &frag, sizeof(frag)) != 0) {
                return -2;
            }

            // Set up select for receiving ICMP response
            fd_set read_fds;
            struct timeval timeout;
            FD_ZERO(&read_fds);
            FD_SET(icmp_sock, &read_fds);
            timeout.tv_sec = 0;
            timeout.tv_usec = 10000;

            // Wait for ICMP response
            int ret = select(icmp_sock + 1, &read_fds, NULL, NULL, &timeout);
            if (ret > 0 && FD_ISSET(icmp_sock, &read_fds))	{

                char buf[512];
                struct sockaddr_in recv_addr;
                socklen_t addr_len = sizeof(recv_addr);
                ssize_t len = recvfrom(icmp_sock, buf, sizeof(buf), 0, (struct sockaddr *)&recv_addr, &addr_len);
                if (len > 0)	{

                    if (getnameinfo((struct sockaddr *)&recv_addr, addr_len, host[probe], sizeof(host[probe]), NULL, 0, 0) != 0)
                        strncpy(host[probe], "unknown", sizeof(host[probe]));
                    gettimeofday(&end_time, NULL);
                    //if (strcmp(host[probe], host[ttl-1]) == 0)
			//break;
		    double rtt = (double) ((end_time.tv_sec * 1000000 + end_time.tv_usec) - (start_time.tv_sec * 1000000 + start_time.tv_usec)) / 1000.0;
                    rtts[probe] = rtt;
                    // successful_probes++;

                    // Store IP address for later use
                    inet_ntop(AF_INET, &recv_addr.sin_addr, ip_addresses[probe], sizeof(ip_addresses[probe]));
                }
            } else	{

                rtts[probe] = -1;  // Timeout
                strcpy(ip_addresses[probe], "*");
            }
        }

        int cmp_ret;
        cmp_ret = strcmp(prev_address, ip_addresses[0]);
        // printf("This is CMP_RET : %d\n", cmp_ret);
        if ( cmp_ret == 0 && prev_address[0] != '*')
        {
            // printf("HELLO THERE YOU SHOULD GO AWAY \n");
            break;
        }
        printf("%s", RED);
        // printf("This is the prev_address : %s\n", prev_address);
        // printf("This is the ip_addresses[0] : %s\n", ip_addresses[0]);
        printf("%s", CLR);
        s_clr_addr(prev_address);
        s_cpy(prev_address, ip_addresses[0]);
        prev_address[ft_strlen(ip_addresses[0])+1] = '\0';

        // Print hop information
        printf("%2d  ", ttl);
        for (int probe = 0; probe < NUM_PROBES; probe++)	{

            if (rtts[probe] >= 0)	{

                printf("%s (%s)  %.2f ms", (probe == 0) ? host[probe] : "", ip_addresses[probe], rtts[probe]);
            } else	{

                printf("*");
            }
            if (probe < NUM_PROBES - 1)	{

                printf("  ");
            }
        }
        printf("\n");

        // If the TTL exceeds the maximum value (maximum hops)
        if (ttl == MAX_TTL)
            break;
        // If the TLL is bigger than the target's TTL
    }

    // Cleanup
    close(udp_sock);
    close(icmp_sock);
    return EXIT_SUCCESS;
}
