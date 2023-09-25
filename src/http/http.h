//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#ifndef KETAMINE_HTTP_H
#define KETAMINE_HTTP_H

#include "../server.h"
#include "../global.h"

typedef enum {
        METHOD_INVALID,
        METHOD_GET,
        METHOD_POST
} http_method;

typedef struct {
        char field[MAX_STRING_LENGTH];
        char value[MAX_STRING_LENGTH];
} http_header;

typedef struct {
        char path[MAX_STRING_LENGTH];

        http_method method;
        http_header headers[MAX_HEADER_COUNT];
        unsigned int header_count;

        unsigned long data_length;
        char *data;
} http_request;

typedef struct {
        unsigned int status_code;
        char status_message[MAX_STATUS_MESSAGE_LENGTH];

        unsigned long body_length;
        char *body;

        unsigned long header_length;
} http_response;

http_method get_method(char *str);

int parse_request(char *req, client_handle *handle, http_request *target);

void request_dealloc(http_request *request);

http_header *request_find_header(http_request *request, char *key);

int simple_http_response(http_response *target, unsigned int code, const char *msg);

int full_http_response(http_response *target, unsigned int code, const char *msg, char *body, unsigned int body_length);

void response_dealloc(http_response *response);

char *http_response_string(http_response *resp);

unsigned int http_response_length(http_response *resp);

#endif //KETAMINE_HTTP_H
