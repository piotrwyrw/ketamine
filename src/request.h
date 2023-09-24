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
        char header[MAX_STRING_LENGTH];
        char value[MAX_STRING_LENGTH];
} http_header;

typedef struct {
        http_method method;
        http_header headers[MAX_HEADER_COUNT];

        unsigned long data_length;
        char *data;
} http_request;

http_method get_method(char *str);

char *parse_request(char *req, client_handle *handle);

#endif //KETAMINE_REQUEST_H
