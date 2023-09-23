//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#ifndef KETAMINE_SERVER_H
#define KETAMINE_SERVER_H

#include <stdlib.h>
#include <sys/socket.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "client.h"

extern const char *http_not_found;
extern int server_sockd;
extern int connection_ids;

typedef struct {
        int conn_id;
        int client_sockd;
        struct sockaddr_in addr;
        socklen_t addr_len;
        char *ip_addr;

        char *file_buffer;
        unsigned long file_size;

        request_status req;
} client_handle;

client_handle *new_connection(int sockd, struct sockaddr_in addr, socklen_t addr_len);

void free_connection(client_handle *conn);

int run_server();

#endif //KETAMINE_SERVER_H
