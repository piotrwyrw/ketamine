//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#include "http.h"
#include "../parse.h"
#include "../gplogging.h"

#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

http_method get_method(char *str)
{
        if (strncmp(str, "GET", 3) == 0) {
                return METHOD_GET;
        }

        if (strncmp(str, "POST", 4) == 0) {
                return METHOD_POST;
        }

        return METHOD_INVALID;
}

// Parse an HTTP request
int parse_request(char *req, client_handle *handle, http_unit *request)
{
        // Sanity checks before parsing
        if (!req || !handle) {
                return -1;
        }

        if (strnlen(req, MAX_STRING_LENGTH) == 0) {
                return -1;
        }

        if (!request) {
                return -1;
        }

        if (request->type != UNIT_REQUEST) {
                return 01;
        }


        char *line;

        request->header_count = 0;
        request->unit.request.method = METHOD_INVALID;
        request->data = NULL;

        // Scan the request line-by-line
        next_line:
        line = strsep(&req, "\n\r");

        if (!line) {
                goto done;
        }

        // We don't care about empty lines tho
        if (strnlen(line, MAX_STRING_LENGTH) == 0) {
                goto next_line;
        }

        // Parse the first request line (e.g. GET / HTTP/1.1)
        if (request->unit.request.method == METHOD_INVALID) {
                char *nLinePtr;

                // Find the method
                if (!(nLinePtr = identifier(line, handle))) {
                        ERROR_LOG("Failed to parse request: Could not parse method identifier.\n")
                        return -1;
                }

                request->unit.request.method = get_method(handle->parser_buffer);

                if (request->unit.request.method == METHOD_INVALID) {
                        ERROR_LOG("Could not recognise method: \"%s\".\n", handle->parser_buffer)
                        return -1;
                }

                if (strnlen(nLinePtr, MAX_STRING_LENGTH) == 0) {
                        ERROR_LOG("Request string ends after method.\n")
                        return -1;
                }

                // Skip the initial whitespace, if present
                if (nLinePtr[0] == ' ') {
                        nLinePtr++;
                }

                // Find the target (path)
                char *path = strsep(&nLinePtr, " ");

                if (!path) {
                        ERROR_LOG("Could not parse request path.\n")
                        return -1;
                }

                if (strnlen(path, MAX_STRING_LENGTH) == 0) {
                        ERROR_LOG("The request path is empty.\n")
                        return -1;
                }

                strncpy(request->unit.request.path, path, MAX_STRING_LENGTH);

                goto next_line;
        }

        if (request->header_count >= MAX_HEADER_COUNT) {
                ERROR_LOG("Failed to parse headers: Maximum allowed number of headers would've been exceeded \n")
                return -1;
        }

        // Parse a request field
        char *key = strsep(&line, ":");

        if (!key) {
                ERROR_LOG("Could not retrieve field key.\n")
                return -1;
        }

        if (strnlen(key, MAX_STRING_LENGTH) == 0) {
                ERROR_LOG("Header key is empty.\n")
                return -1;
        }

        char *value = strsep(&line, ":");

        if (!value) {
                ERROR_LOG("Could not retrieve field value.\n")
                return -1;
        }

        if (strnlen(value, MAX_STRING_LENGTH) == 0) {
                ERROR_LOG("Header value is empty.\n")
                return -1;
        }

        // Remove leading spaces
        if (value[0] == ' ') {
                value++;
        }

        if (request_find_header(request, key)) {
                ERROR_LOG("Conflicting headers: The header \"%s\" was passed more than once.\n", key)
                return -1;
        }

        strncpy(request->headers[request->header_count].field, key, MAX_STRING_LENGTH);
        strncpy(request->headers[request->header_count].value, value, MAX_STRING_LENGTH);
        request->header_count++;

        // If the last header is 'Content-Length', put the provided amount of bytes into the data section of the request
        if (strncmp(key, "Content-Length", 14) != 0) {
                goto next_line;
        }

        remove_leading_spaces(&req);

        request->data_length = strtoul(value, NULL, 10);

        if (request->data_length == ULONG_MAX && (errno == ERANGE || errno == EINVAL)) {
                ERROR_LOG("Could not parse value of 'Content-Length' header as unsigned long.\n")
                return -1;
        }

        // We don't want to end up allocating a buffer just for the null-terminator ...
        if (request->data_length == 0) {
                goto done;
        }

        request->data = calloc(request->data_length + 1, sizeof(char));

        if (!request->data) {
                ERROR_LOG("Could not allocate memory buffer for request data.\n")
                return -1;
        }

        memcpy(request->data, req, request->data_length);

        done:

        return 0;
}

http_header *request_find_header(http_unit *request, char *key)
{
        if (!request || !key) {
                return NULL;
        }

        for (unsigned long i = 0; i < request->header_count; i++) {
                http_header header = request->headers[i];

                if (strncmp(header.field, key, MAX_STRING_LENGTH) == 0) {
                        return &(request->headers[i]);
                }
        }

        return NULL;
}

int simple_http_response(http_unit *target, unsigned int code, const char *msg)
{
        if (!target || !msg) {
                return -1;
        }

        if (target->type != UNIT_RESPONSE) {
                return -1;
        }

        strncpy(target->unit.response.status_message, msg, MAX_STATUS_MESSAGE_LENGTH);
        target->unit.response.status_code = code;

        target->data = NULL;
        target->data_length = 0;

        return 0;
}

int full_http_response(http_unit *target, unsigned int code, const char *msg, char *body, unsigned int body_length)
{
        if (!target || !msg) {
                return -1;
        }

        if (!body || body_length == 0) {
                return simple_http_response(target, code, msg);
        }

        if (target->type != UNIT_RESPONSE) {
                return -1;
        }

        target->unit.response.status_code = code;
        strncpy(target->unit.response.status_message, msg, MAX_STATUS_MESSAGE_LENGTH);

        // Yes, the null terminator is totally pointless in case of binary data, but
        // let's leave it in there just in case.
        target->data = calloc(body_length + 1, sizeof(char));

        if (!target->data) {
                ERROR_LOG("Could not allocate memory for HTTP response body.\n")
                return -1;
        }

        memcpy(target->data, body, body_length);
        target->data_length = body_length;

        return 0;
}

void unit_dealloc(http_unit *response)
{
        if (!response) {
                return;
        }

        if (!response->data) {
                return;
        }

        free(response->data);
}

/**
 * @param resp
 * @return A dynamically allocated HTTP response string or <b>NULL</b> if something were to go wrong
 */
char *http_unit_string(http_unit *resp)
{
        if (!resp) {
                return NULL;
        }

        if (resp->type != UNIT_RESPONSE) {
                return NULL;
        }

        if (strnlen(resp->unit.response.status_message, MAX_STATUS_MESSAGE_LENGTH) == 0) {
                return NULL;
        }

        char *str;

        if (resp->data && resp->data_length > 0) {
                str = calloc(resp->data_length + MAX_STATUS_MESSAGE_LENGTH + 50, sizeof(char));
                sprintf(str, "HTTP/1.1 %d %s\r\nConnection: Closed\r\nContent-Length: %ld\r\n\r\n",
                        resp->unit.response.status_code,
                        resp->unit.response.status_message, resp->data_length);
                resp->unit.response.header_length = strnlen(str, MAX_STRING_LENGTH);
                memcpy(str + resp->unit.response.header_length, resp->data, resp->data_length);
                return str;
        }

        str = calloc(MAX_STATUS_MESSAGE_LENGTH + 50, sizeof(char));
        sprintf(str, "HTTP/1.1 %d %s\r\nConnection: Closed\r\n\r\n", resp->unit.response.status_code,
                resp->unit.response.status_message);
        resp->unit.response.header_length = strnlen(str, MAX_STRING_LENGTH);
        return str;
}

unsigned int http_response_length(http_unit *resp)
{
        if (!resp) {
                return 0;
        }

        if (resp->type != UNIT_RESPONSE) {
                return -1;
        }

        return resp->data_length + resp->unit.response.header_length;
}