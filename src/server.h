//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#ifndef WEBSERVER_SERVER_H
#define WEBSERVER_SERVER_H

#include <stdlib.h>
#include <sys/socket.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

extern const char *http_not_found;
extern int server_sockd;
extern int connection_ids;

typedef struct {
        int conn_id;
        int client_sockd;
        struct sockaddr_in addr;
        socklen_t addr_len;
        char *ip_addr;
} server_connection;

server_connection *new_connection(int sockd, struct sockaddr_in addr, socklen_t addr_len);
void free_connection(server_connection *conn);

int run_server();

#endif //WEBSERVER_SERVER_H
