//
// Created by Piotr Krzysztof Wyrwas on 21.09.23.
//

#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "server.h"
#include "defs.h"
#include "global.h"
#include "parse.h"
#include "fio.h"
#include "client.h"
#include "routing.h"
#include "settings.h"

request_status respond_get(int sockd, char *req, server_connection *conn)
{
        if (strcmp(req, "close-server") == 0) {
                return EXIT;
        }

        char *file = parse_request(req);

        if (!file) {
                CONNECTION_ERROR(conn, "Could not parse incoming request\n");
                return ERR;
        }

        char *route_target = resolve_route(file);

        if (route_target) {
                CONNECTION_LOG(conn, "Routing \"%s\" -> \"%s\"\n", file, route_target)
                file = route_target;
        }

        if (strcmp(file, "/") == 0) {
                file = INDEX_FILE_NAME;
        }

        if (!route_target && file[0] == '/') {
                file++;
        }

        int status = read_file(file);
        if (status < 0) {
                CONNECTION_LOG(conn, "GET \"%s\" -> Resource unavailable\n", file);

                status = send(sockd, http_not_found, strlen(http_not_found), 0) >= 0 ? 0 : -1;
                HANDLE_ERRORS("send(404)", status)

                return GET_ERR;
        }

        CONNECTION_LOG(conn, "GET \"%s\" -> Resource found\n", file);

        char *http_success = calloc(file_size + 100, sizeof(char));
        sprintf(http_success,
                "HTTP/1.1 200 KETAMINE-OK\r\nContent-Length: %ld\r\nAccept-Ranges: bytes\r\nConnection: Closed\r\n\r\n%s",
                file_size,
                file_buffer);


        status = send(sockd, http_success, strlen(http_success), 0) >= 0 ? 0 : -1;
        HANDLE_ERRORS("send", status)

        free(http_success);

        return GET_OK;
}

void *handle_client_connection(void *param)
{
        server_connection *conn = (server_connection *) param;

        int client_socket = conn->client_sockd;
        char buffer[1001] = {0};

        CONNECTION_LOG(conn, "New connection established\n");

        indent++;

        ssize_t req_size;

        receive_req:
        req_size = recv(client_socket, buffer, 1000, 0);

        if (req_size < 0) {
                if ((errno == EWOULDBLOCK || errno == EAGAIN)) {
                        goto receive_req;
                }
                printf("Could not receive data.\n");
                return NULL;
        }

        if (req_size == 0) {
                CONNECTION_LOG(conn, "No incoming requests.\n");
                goto close_session;
        }

        request_status req = respond_get(client_socket, buffer, conn);

        if (req == EXIT) {
                running = false;
        }

        close_session:
        free_file_buffer();
        close(client_socket);

        indent--;
        CONNECTION_LOG(conn, "Disconnected\n");

        free_connection(conn);

        return NULL;
}