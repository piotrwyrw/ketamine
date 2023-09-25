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
int parse_request(char *req, client_handle *handle, http_request *request)
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


        char *line;

        request->header_count = 0;
        request->method = METHOD_INVALID;
        request->data = NULL;

//        http_request request = {
//                .header_count = 0,
//                .method = METHOD_INVALID,
//                .data = NULL
//        };

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
        if (request->method == METHOD_INVALID) {
                char *nLinePtr;

                // Find the method
                if (!(nLinePtr = identifier(line, handle))) {
                        ERROR_LOG("Failed to parse request: Could not parse method identifier.\n")
                        return -1;
                }

                request->method = get_method(handle->parser_buffer);

                if (request->method == METHOD_INVALID) {
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

                strncpy(request->path, path, MAX_STRING_LENGTH);

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

void request_dealloc(http_request *request)
{
        if (!request->data) {
                return;
        }

        if (!request->data_length) {
                return;
        }

        free(request->data);
}

int simple_http_response(http_response *target, unsigned int code, const char *msg)
{
        if (!target || !msg) {
                return -1;
        }

        strncpy(target->status_message, msg, MAX_STATUS_MESSAGE_LENGTH);
        target->status_code = code;

        target->body = NULL;
        target->body_length = 0;

        return 0;
}

int full_http_response(http_response *target, unsigned int code, const char *msg, char *body, unsigned int body_length)
{
        if (!target || !msg) {
                return -1;
        }

        if (!body || body_length == 0) {
                return simple_http_response(target, code, msg);
        }

        target->status_code = code;
        strncpy(target->status_message, msg, MAX_STATUS_MESSAGE_LENGTH);

        // Yes, the null terminator is totally pointless in case of binary data, but
        // let's leave it in there just in case.
        target->body = calloc(body_length + 1, sizeof(char));

        if (!target->body) {
                ERROR_LOG("Could not allocate memory for HTTP response body.\n")
                return -1;
        }

        memcpy(target->body, body, body_length);
        target->body_length = body_length;

        return 0;
}

void response_dealloc(http_response *response)
{
        if (!response) {
                return;
        }

        if (!response->body) {
                return;
        }

        free(response->body);
}

/**
 * @param resp
 * @return A dynamically allocated HTTP response string or <b>NULL</b> if something were to go wrong
 */
char *http_response_string(http_response *resp)
{
        if (!resp) {
                return NULL;
        }

        if (strnlen(resp->status_message, MAX_STATUS_MESSAGE_LENGTH) == 0) {
                return NULL;
        }

        char *str;

        if (resp->body && resp->body > 0) {
                str = calloc(resp->body_length + MAX_STATUS_MESSAGE_LENGTH + 50, sizeof(char));
                sprintf(str, "HTTP/1.1 %d %s\r\nConnection: Closed\r\nContent-Length: %ld\r\n\r\n", resp->status_code,
                        resp->status_message, resp->body_length);
                resp->header_length = strnlen(str, MAX_STRING_LENGTH);
                memcpy(str + resp->header_length, resp->body, resp->body_length);
                return str;
        }

        str = calloc(MAX_STATUS_MESSAGE_LENGTH + 50, sizeof(char));
        sprintf(str, "HTTP/1.1 %d %s\r\nConnection: Closed\r\n\r\n", resp->status_code, resp->status_message);
        resp->header_length = strnlen(str, MAX_STRING_LENGTH);
        return str;
}

unsigned int http_response_length(http_response *resp)
{
        if (!resp) {
                return 0;
        }

        return resp->body_length + resp->header_length;
}