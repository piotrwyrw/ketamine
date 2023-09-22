//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#include "server.h"

#include "settings.h"
#include "defs.h"
#include "client.h"
#include "global.h"

#include <stdbool.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

const char *http_not_found = "HTTP/1.1 404 NOT-FOUND\r\nConnection: Closed\r\n\r\n";

int server_sockd;
int connection_ids = 0;

server_connection *new_connection(int sockd, struct sockaddr_in addr, socklen_t addr_len)
{
        server_connection *conn = malloc(sizeof(server_connection));

        if (!conn) {
                return NULL;
        }

        conn->conn_id = connection_ids;
        conn->client_sockd = sockd;
        conn->addr = addr;
        conn->addr_len = addr_len;
        conn->ip_addr = inet_ntoa(addr.sin_addr);
        return conn;
}

void free_connection(server_connection *conn)
{
        if (!conn) {
                return;
        }

        free(conn);
}

int config_server()
{
        int _true = true;

        server_sockd = socket(AF_INET, SOCK_STREAM, 0);
        HANDLE_ERRORS("socket", server_sockd)

        int status = setsockopt(server_sockd, SOL_SOCKET, SO_REUSEADDR, &_true, sizeof(int));
        HANDLE_ERRORS("setsockopt", status)

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(LISTEN_PORT);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        status = bind(server_sockd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
        HANDLE_ERRORS("bind", status)

        status = listen(server_sockd, 10);
        HANDLE_ERRORS("listen", status)

        status = fcntl(server_sockd, F_SETFL, O_NONBLOCK);
        HANDLE_ERRORS("fcntl", status)

        return 0;
}

int run_server()
{
        if (config_server() < 0) {
                return -1;
        }

        struct sockaddr_in in_addr;
        socklen_t addr_len;

        int status;
        int client_socket;

        INFO_LOG("Server is UP AND RUNNING on http://localhost:%d\n\n", LISTEN_PORT)

        while (true) {

                if (!running) {
                        INFO_LOG("---- Server shutdown requested. Waiting for all threads to terminate ----\n")
                        while (running_threads > 0) {}
                        goto return_all;
                }

                status = accept(server_sockd, (struct sockaddr *) &in_addr, &addr_len);

                if (status < 0) {
                        if (errno == EWOULDBLOCK || errno == EAGAIN) {
                                continue;
                        }
                }

                HANDLE_ERRORS("accept", status)
                client_socket = status;

                if (in_addr.sin_family != AF_INET) {
                        status = close(client_socket);
                        HANDLE_ERRORS("close", status)

                        continue;
                }

                server_connection *conn = new_connection(status, in_addr, addr_len);

                if (!conn) {
                        ERROR_LOG("Failed to allocate a connection structure\n")

                        status = close(client_socket);
                        HANDLE_ERRORS("close", status)

                        return -1;
                }

                running_threads++;

                // Handle this client in an async thread
                pthread_t thread;
                pthread_create(&thread, NULL, handle_client_connection, conn);
                pthread_detach(thread);

                connection_ids++;
        }

        return_all:
        INFO_LOG("Server shutting down ...\n");

        return 0;
}