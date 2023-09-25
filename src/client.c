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
#include "http/http.h"
#include "fio.h"
#include "client.h"
#include "routing.h"
#include "threadsafe.h"
#include "modules/middleware.h"

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

        http_unit request = {.type = UNIT_REQUEST};
        http_unit response = {.type = UNIT_RESPONSE};
        char *response_str;

        if (parse_request(req, handle, &request) < 0) {
                CONNECTION_ERROR(handle, "Could not parse incoming request\n")
                return ERR;
        }

        char *file = request.unit.request.path;
        size_t file_len;

        if ((file_len = strnlen(file, MAX_STRING_LENGTH)) == 0) {
                CONNECTION_ERROR(handle, "An error occurred while parsing the request\n")
                goto return_error;
        }

        // Note: This function may return NULL, if no explicit routes are present
        char *route_target = resolve_route(file);

        if (route_target) {
                CONNECTION_LOG(handle, "Routing \"%s\" -> \"%s\"\n", file, route_target)
                file = route_target;
        }

        // Run the modular middleware
        if (run_get_hook(&request, &response) < 0) {
                goto continue_regular_flow;
        }

        goto send_and_close;

        continue_regular_flow:

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
                CONNECTION_LOG(handle, "GET \"%s\" -> Resource unavailable\n", file)

                simple_http_response(&response, 404, "NOT_FOUND");
                response_str = http_unit_string(&response);

                status = send(sockd, response_str, http_response_length(&response), 0) >= 0 ? 0 : -1;
                if (status < 0) {
                        unit_dealloc(&request);
                        unit_dealloc(&response);
                        free(response_str);
                }

                HANDLE_ERRORS("send(404)", status)

                unit_dealloc(&request);
                unit_dealloc(&response);
                free(response_str); // No it may not: It's only freed if (status < 0), which is also when the error handler gets triggered

                return RESOURCE_UNAVAILABLE;
        }

        CONNECTION_LOG(handle, "GET \"%s\" -> Resource found\n", file)

        full_http_response(&response, 200, "OK", handle->file_buffer, handle->file_size);

        send_and_close:
        response_str = http_unit_string(&response);
        status = send(sockd, response_str, http_response_length(&response), 0) >= 0 ? 0 : -1;
        if (status < 0) {
                unit_dealloc(&request);
                unit_dealloc(&response);
                free(response_str);
        }
        HANDLE_ERRORS("send", status)

        unit_dealloc(&request);
        unit_dealloc(&response);
        free(response_str); // Again, it may NOT point to deallocated memory.

        return GET_OK;

        return_error:
        unit_dealloc(&request);
        return ERR;
}

void *handle_client_connection(void *param)
{
        client_handle *handle = (client_handle *) param;

        int client_socket = handle->client_sockd;
        char buffer[1001] = {0};

        CONNECTION_LOG(handle, "New connection established\n")

        ssize_t req_size;
        handle->req = DEFAULT;

        receive_req:
        req_size = recv(client_socket, buffer, 1000, 0);

        if (req_size < 0) {
                if (!is_running()) {
                        CONNECTION_LOG(handle, "Server thread was requested to terminate.\n")
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
        CONNECTION_LOG(handle, "Disconnected\n")
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