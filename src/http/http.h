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

typedef enum {
        UNIT_INVALID,
        UNIT_REQUEST,
        UNIT_RESPONSE
} http_unit_type;

typedef struct {
        http_unit_type type;

        http_header headers[MAX_HEADER_COUNT];
        unsigned int header_count;

        unsigned long data_length;
        char *data;

        union {
                struct {
                        char path[MAX_STRING_LENGTH];
                        http_method method;
                } request;

                struct {
                        unsigned int status_code;
                        char status_message[MAX_STATUS_MESSAGE_LENGTH];

                        /**
                         * Now that the header length is also a thing with requests, i.e. that they also have a header
                         * that has a defined length. In this case, however, we don't really care about the header length
                         * of the request - the parser will do just fine without this information.
                         */
                        unsigned long header_length;
                } response;
        } unit;
} http_unit;

http_method get_method(char *str);

int parse_request(char *req, client_handle *handle, http_unit *target);

http_header *request_find_header(http_unit *request, char *key);

int simple_http_response(http_unit *target, unsigned int code, const char *msg);

int full_http_response(http_unit *target, unsigned int code, const char *msg, char *body, unsigned int body_length);

void unit_dealloc(http_unit *response);

char *http_unit_string(http_unit *resp);

unsigned int http_response_length(http_unit *resp);

#endif //KETAMINE_HTTP_H
