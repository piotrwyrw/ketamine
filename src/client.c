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

// Respond to a HTTP GET request
request_status respond_get(int sockd, char *req, server_connection *conn)
{
#ifdef WITH_DEBUG_FEATURES
        if (strncmp(req, "close-server", 12) == 0) {
                return EXIT;
        }
#endif

        char *file = parse_request(req);

        if (!file) {
                CONNECTION_ERROR(conn, "Could not parse incoming request\n");
                return ERR;
        }

        size_t file_len;

        if ((file_len = strnlen(file, MAX_STRING_LENGTH)) == 0) {
                CONNECTION_ERROR(conn, "An error occurred while parsing the GET request\n");
                return ERR;
        }

        // Note: This function may return NULL, if no explicit routes are present
        char *route_target = resolve_route(file);

        if (route_target) {
                CONNECTION_LOG(conn, "Routing \"%s\" -> \"%s\"\n", file, route_target)
                file = route_target;
        }

        // Special cases if no routes were resolved
        if (!route_target) {

                // Route to the default index document on '/'
                if (file_len == 1 && file[0] == '/') {
                        file = INDEX_FILE_NAME;
                }

                // Skip the initial '/' if possible
                if (file[0] == '/' && file_len > 1) {
                        file++;
                }

        }

        int status = read_file(file);
        if (status < 0) {
                CONNECTION_LOG(conn, "GET \"%s\" -> Resource unavailable\n", file);

                status = send(sockd, http_not_found, strnlen(http_not_found, MAX_STRING_LENGTH), 0) >= 0 ? 0 : -1;
                HANDLE_ERRORS("send(404)", status)

                return GET_ERR;
        }

        CONNECTION_LOG(conn, "GET \"%s\" -> Resource found\n", file);

        size_t response_length = file_size + 100;
        char *http_response = calloc(response_length, sizeof(char));
        if (!http_response) {
                ERROR_LOG("Failed to allocate memory for HTTP response.\n")
                return ERR;
        }
        sprintf(http_response,
                "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nAccept-Ranges: bytes\r\nConnection: Closed\r\n\r\n%s",
                file_size,
                file_buffer);


        status = send(sockd, http_response, strnlen(http_response, response_length), 0) >= 0 ? 0 : -1;
        HANDLE_ERRORS("send", status)

        free(http_response);

        return GET_OK;
}

void *handle_client_connection(void *param)
{
        server_connection *conn = (server_connection *) param;

        int client_socket = conn->client_sockd;
        char buffer[1001] = {0};

        CONNECTION_LOG(conn, "New connection established\n");

        ssize_t req_size;

        receive_req:
        req_size = recv(client_socket, buffer, 1000, 0);

        if (req_size < 0) {
                if (!running) {
                        CONNECTION_LOG(conn, "Server thread was requested to terminate.\n");
                        goto close_session;
                }
                if ((errno == EWOULDBLOCK || errno == EAGAIN)) {
                        goto receive_req;
                }
                printf("Could not receive data.\n");
                return NULL;
        }

        if (req_size == 0) {
//                CONNECTION_LOG(conn, "No incoming requests.\n")
                goto close_session;
        }

        request_status req = respond_get(client_socket, buffer, conn);

#ifdef WITH_DEBUG_FEATURES
        if (req == EXIT) {
                CONNECTION_LOG(conn, "Client requested server shutdown.\n")
        }
#endif

        close_session:;
        free_file_buffer();
        close(client_socket);
        CONNECTION_LOG(conn, "Disconnected\n");
        free_connection(conn);

#ifdef WITH_DEBUG_FEATURES
        if (req == EXIT) {
                running = false;
        }
#endif

        running_threads--;

        INFO_LOG("---- %ld Thread%s still running ----\n", running_threads, (running_threads == 1) ? "" : "s")

        return NULL;
}