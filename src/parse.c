//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#include "parse.h"
#include "settings.h"
#include "defs.h"

#include <string.h>
#include <stdio.h>

// Parse a HTTP 'GET' request
char *parse_request(char *req, client_handle *handle)
{
        if (strnlen(req, MAX_STRING_LENGTH) == 0) {
                return NULL;
        }

        char *tok = strsep(&req, "\n\r");

        if (tok == NULL) {
                return NULL;
        }

        char *file = strsep(&tok, " ");

        if (file == NULL) {
                return NULL;
        }

        if (strncmp(file, "GET", 3) != 0) {
                CONNECTION_ERROR(handle, "Not a GET request.\n");
                return NULL;
        }

        file = strsep(&tok, " ");

        if (file == NULL) {
                return NULL;
        }

        if (strnlen(file, MAX_STRING_LENGTH) == 0) {
                CONNECTION_LOG(handle, "The requested resource path is empty.\n");
                return NULL;
        }

        return file;
}