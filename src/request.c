//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#include "request.h"
#include "global.h"

#include <string.h>
#include <stdio.h>

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
char *parse_request(char *req, client_handle *handle)
{
        if (!req || !handle) {
                return NULL;
        }

        if (strnlen(req, MAX_STRING_LENGTH) == 0) {
                return NULL;
        }

        char *line, *token;

        next_line:
        line = strsep(&req, "\n\r");

        if (!line) {
                goto done;
        }

        if (strnlen(line, MAX_STRING_LENGTH) > 0) {
                printf("%s\n", line);
        }

        goto next_line;

        done:

        return NULL;
}