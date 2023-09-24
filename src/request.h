//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#ifndef KETAMINE_REQUEST_H
#define KETAMINE_REQUEST_H

#include "server.h"
#include "global.h"

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

http_method get_method(char *str);

int parse_request(char *req, client_handle *handle, http_request *target);

void request_dealloc(http_request *request);

#endif //KETAMINE_REQUEST_H
