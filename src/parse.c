//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#include "parse.h"
#include "settings.h"

#include <string.h>

char *parse_request(char *req)
{
        if (strlen(req) == 0) {
                return NULL;
        }

        char *tok = strtok(req, "\n\r");

        if (tok == NULL) {
                return NULL;
        }

        char *file = strtok(tok, " ");

        if (file == NULL) {
                return NULL;
        }

        if (strcmp(file, "GET") != 0) {
                return NULL;
        }

        file = strtok(NULL, " ");

        if (file == NULL) {
                return NULL;
        }

        if (strcmp(file, "/") == 0) {
                return INDEX_FILE_NAME;
        }

        if (file[0] == '/' && strlen(file) > 1) {
                file++;
        }

        return file;
}