//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#include "parse.h"
#include "settings.h"

#include <string.h>

// Parse a HTTP 'GET' request
char *parse_request(char *req)
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

        file = strsep(&tok, " ");

        if (file == NULL) {
                return NULL;
        }

        return file;
}