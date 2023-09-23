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
#include "slog.h"
#include "gplogging.h"
#include "global.h"
#include "parse.h"
#include "fio.h"
#include "client.h"
#include "routing.h"
#include "threadsafe.h"

// Respond to a HTTP GET request
request_status respond_get(int sockd, char *req, client_handle *handle)
{
        if (!req || !handle) {
                return ERR;
        }

#ifdef WITH_DEBUG_FEATURES
        if (strncmp(req, "close-server", 12) == 0) {
                return EXIT;
        }
#endif

        char *file = parse_request(req, handle);

        if (!file) {
                CONNECTION_ERROR(handle, "Could not parse incoming request\n");
                return ERR;
        }

        size_t file_len;

        if ((file_len = strnlen(file, MAX_STRING_LENGTH)) == 0) {
                CONNECTION_ERROR(handle, "An error occurred while parsing the GET request\n");
                return ERR;
        }

        // Note: This function may return NULL, if no explicit routes are present
        char *route_target = resolve_route(file);

        if (route_target) {
                CONNECTION_LOG(handle, "Routing \"%s\" -> \"%s\"\n", file, route_target)
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

        char prefixed_file[MAX_STRING_LENGTH] = {0};

        if (route_directory_set) {
                snprintf(prefixed_file, MAX_STRING_LENGTH, "%s/%s", route_directory, file);
                file = prefixed_file;
        }

        int status = read_file(file, handle);
        if (status < 0) {
                CONNECTION_LOG(handle, "GET \"%s\" -> Resource unavailable\n", file);

                status = send(sockd, http_not_found, strnlen(http_not_found, MAX_STRING_LENGTH), 0) >= 0 ? 0 : -1;
                HANDLE_ERRORS("send(404)", status)

                return RESOURCE_UNAVAILABLE;
        }

        CONNECTION_LOG(handle, "GET \"%s\" -> Resource found\n", file);

        size_t response_length = handle->file_size + 100;
        char *http_response = calloc(response_length, sizeof(char));

        if (!http_response) {
                ERROR_LOG("Failed to allocate memory for HTTP response.\n")
                return ERR;
        }

        snprintf(http_response, response_length,
                 "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nAccept-Ranges: bytes\r\nConnection: Closed\r\n\r\n%s",
                 handle->file_size,
                 handle->file_buffer);


        status = send(sockd, http_response, strnlen(http_response, response_length), 0) >= 0 ? 0 : -1;
        HANDLE_ERRORS("send", status)

        free(http_response);

        return GET_OK;
}

void *handle_client_connection(void *param)
{
        client_handle *handle = (client_handle *) param;

        int client_socket = handle->client_sockd;
        char buffer[1001] = {0};

        CONNECTION_LOG(handle, "New connection established\n");

        ssize_t req_size;
        handle->req = DEFAULT;

        receive_req:
        req_size = recv(client_socket, buffer, 1000, 0);

        if (req_size < 0) {
                if (!is_running()) {
                        CONNECTION_LOG(handle, "Server thread was requested to terminate.\n");
                        goto close_session;
                }
                if ((errno == EWOULDBLOCK || errno == EAGAIN)) {
                        goto receive_req;
                }
                goto close_session;
        }

        if (req_size == 0) {
                goto close_session;
        }

        handle->req = respond_get(client_socket, buffer, handle);

#ifdef WITH_DEBUG_FEATURES
        if (handle->req == EXIT) {
                CONNECTION_LOG(handle, "Client requested server shutdown.\n")
        }
#endif

        close_session:
        close(client_socket);
        CONNECTION_LOG(handle, "Disconnected\n");
        request_status tmp_req = handle->req;
        free_connection(handle);

#ifdef WITH_DEBUG_FEATURES
        if (tmp_req == EXIT) {
                request_server_shutdown();
        }
#endif

        decrement_running_threads();

        unsigned long tmp_running_threads = get_running_threads();

        INFO_LOG("---- %ld Thread%s still running ----\n", tmp_running_threads, (tmp_running_threads == 1) ? "" : "s")

        return NULL;
}