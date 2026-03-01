/**
 * @file        server.c
 * @brief       UDP game server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "packets.h"

#define PORT 5000

int server(void)
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        game_packet packet;

        socklen_t len = sizeof(cliaddr);

        ssize_t recvlen = recvfrom(
            sockfd,
            &packet,
            sizeof(packet),
            0,
            (struct sockaddr*)&cliaddr,
            &len
        );

        if (recvlen < 0) {
            perror("recvfrom");
            continue;
        }

        if (recvlen != sizeof(game_packet)) {
            printf("Warning: unexpected packet size (%ld bytes)\n", recvlen);
            continue;
        }

        printf("Packet received:\n");
        printf("  From: %s:%d\n",
               inet_ntoa(cliaddr.sin_addr),
               ntohs(cliaddr.sin_port));

        printf("  Player ID: %d\n", packet.id);
        printf("  Input: %d\n", packet.input);
        printf("  Time: %d\n", packet.time);
        printf("\n");
    }

    return 0;
}