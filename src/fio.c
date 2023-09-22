//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#include "fio.h"
#include "global.h"

#include <stdio.h>
#include <stdlib.h>

int read_file(char *path)
{
        FILE *f = fopen(path, "r");
        if (!f) {
                return -1;
        }

        fseek(f, 0, SEEK_END);
        long len = ftell(f);

        if (len < 0) {
                return -1;
        }

        rewind(f);

        file_buffer = calloc(len + 1, sizeof(char));

        if (fread(file_buffer, sizeof(char), len, f) == 0) {
                return -1;
        }

        file_size = len;

        rewind(f);

        return 0;
}

void free_file_buffer()
{
        if (file_buffer == NULL) {
                goto final;
        }

        free(file_buffer);
        file_buffer = NULL;

        final:
        file_size = 0;
}