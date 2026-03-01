/**
 * @file        client.c
 * @brief       UDP client to communicate with game server
 */
#include "client.h"

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

int client_init(udp_client* client, const char* host, int port){

    client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client->sockfd < 0) {
        fprintf(stderr, "Failed to create UDP socket");
        return -1;
    }

    memset(&client->server_addr, 0, sizeof(client->server_addr));
    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(port);
    client->server_addr.sin_addr.s_addr = inet_addr(host);

    // Connect
    if (connect(client->sockfd, (struct sockaddr*) &client->server_addr, sizeof(client->server_addr)) < 0) {
        fprintf(stderr, "Failed to connect to UDP socket");
        return -1;
    }

    // Make socket non-blocking
    fcntl(client->sockfd, F_SETFL, O_NONBLOCK);

    return 0;
}

int client_send(udp_client* client, const void* data, int size)
{
    return send(client->sockfd, data, size, 0);
}

int client_receive(udp_client* client, void* buffer, int buffer_size)
{
    return recv(client->sockfd, buffer, buffer_size, 0);
}

void client_destroy(udp_client* client)
{
    close(client->sockfd);
}