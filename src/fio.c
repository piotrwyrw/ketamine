//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#include "fio.h"
#include "gplogging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_file_to(char *path, char **buffer, unsigned long *length)
{
        if (!path || !buffer || !length) {
                return -1;
        }

        FILE *f = fopen(path, "r");
        if (!f) {
                return -1;
        }

        fseek(f, 0, SEEK_END);
        long len = ftell(f);

        if (len <= 0) {
                fclose(f);
                return -1;
        }

        rewind(f);

        *buffer = calloc(len + 1, sizeof(char));

        if (!*buffer) {
                ERROR_LOG("Failed to allocate file buffer.\n")
                return -1;
        }

        if (fread(*buffer, sizeof(char), len, f) == 0) {
                free(*buffer);
                *buffer = NULL;
                return -1;
        }

        *length = len;

        fclose(f);

        return 0;
}

int read_file(char *path, client_handle *handle)
{
        return read_file_to(path, &handle->file_buffer, &handle->file_size);
}