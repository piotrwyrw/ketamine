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

// Parse and respond to a HTTP request after calling GET hooks, resolving routes, including considering potential route
// overrides from the modules. A lot of stuff in a single function. Will need to break this up at some point to make it
// a bit more readable.
request_status respond_get(int sockd, char *req, client_handle *handle)
{
        if (!req || !handle) {
                return RSTATUS_ERR;
        }

#ifdef WITH_DEBUG_FEATURES
        if (strncmp(req, "close-server", 12) == 0) {
                return RSTATUS_EXIT;
        }
#endif

        request_status exit_code = RSTATUS_DEFAULT;
        http_unit request = {.type = UNIT_REQUEST};
        http_unit response = {.type = UNIT_RESPONSE};
        char *response_str;

        // Parse the incoming request, store output in `request`
        if (parse_request(req, handle, &request) < 0) {
                CONNECTION_ERROR(handle, "Could not parse incoming request\n")
                return RSTATUS_ERR;
        }

        // We cannot respond to anything that is not a GET request
        if (request.unit.request.method != METHOD_GET) {
                return RSTATUS_OK;
        }

        // Data needed to respond to a GET: Target path name, length of the path string
        char *file = request.unit.request.path;
        size_t file_len;

        // If the target path is empty, there's not much we can do
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
        if (run_get_hook(&request, &response) == 0) {
                exit_code = RSTATUS_OK;
                goto send_and_close;
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

        // Prefix the file with a root directory (if it needs prefixing)
        char prefixed_file[MAX_STRING_LENGTH] = {0};

        if (route_directory_set) {
                snprintf(prefixed_file, MAX_STRING_LENGTH, "%s/%s", route_directory, file);
                file = prefixed_file;
        }

        // Read the requested file
        int status = read_file(file, handle);

        /* Case a) Reading did not succeed */
        if (status < 0) {
                CONNECTION_LOG(handle, "GET \"%s\" -> Resource unavailable\n", file)

                // Respond with a simple 404 "NOT_FOUND" HTTP response
                simple_http_response(&response, 404, "NOT_FOUND");
                goto send_and_close;
        }

        /* Case b) The file was read properly */

        CONNECTION_LOG(handle, "GET \"%s\" -> Resource found\n", file)

        // Respond with the contents of the requested file
        full_http_response(&response, 200, "OK", handle->file_buffer, handle->file_size);

        send_and_close:
        response_str = http_response_string(&response);
        status = send(sockd, response_str, http_response_length(&response), 0) >= 0 ? 0 : -1;

        HANDLE_ERRORS("send", status)

        free_resources:
        unit_dealloc(&request);
        unit_dealloc(&response);
        free(response_str);

        return exit_code;

        return_error:
        unit_dealloc(&request);
        return RSTATUS_ERR;
}

void *handle_client_connection(void *param)
{
        client_handle *handle = (client_handle *) param;

        int client_socket = handle->client_sockd;
        char buffer[1001] = {0};

        CONNECTION_LOG(handle, "New connection established\n")

        ssize_t req_size;
        handle->req = RSTATUS_DEFAULT;

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
        if (handle->req == RSTATUS_EXIT) {
                CONNECTION_LOG(handle, "Client requested server shutdown.\n")
        }
#endif

        close_session:
        close(client_socket);
        CONNECTION_LOG(handle, "Disconnected\n")
        request_status tmp_req = handle->req;
        free_connection(handle);

#ifdef WITH_DEBUG_FEATURES
        if (tmp_req == RSTATUS_EXIT) {
                request_server_shutdown();
        }
#endif

        decrement_running_threads();

        unsigned long tmp_running_threads = get_running_threads();

        INFO_LOG("---- %ld Thread%s still running ----\n", tmp_running_threads, (tmp_running_threads == 1) ? "" : "s")

        return NULL;
}