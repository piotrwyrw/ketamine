//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#include "fio.h"
#include "cfgroute.h"
#include "routing.h"
#include "defs.h"

#include <string.h>
#include <stdio.h>

int load_routes_file(char *path)
{
        // Read the file into memory
        if (!path) {
                return -1;
        }

        char *base_buffer; // Warning: This is a dynamic buffer - will need explicit free()-ing
        char *buffer;
        unsigned long length;

        if (read_file_to(path, &base_buffer, &length) < 0) {
                return -1;
        }

        buffer = base_buffer;

        // -- Parse the routes file --

        char *line;

        // Loop through all lines
        while ((line = strsep(&buffer, "\n"))) {

                char *left, *right;

                if (!(left = strsep(&line, ":"))) {
                        ERROR_LOG("Failed while parsing routes: could not retrieve lval (route source)\n")
                        free(base_buffer);
                        return -1;
                }

                if (!(right = strsep(&line, ":"))) {
                        ERROR_LOG("Failed while parsing routes: could not retrieve rval (route target)\n")
                        free(base_buffer);
                        return -1;
                }

                if (route(left, right)) {
                        INFO_LOG("Routes: \"%s\" -> \"%s\"\n", left, right)
                } else {
                        ERROR_LOG("Routes: could not route: \"%s\" -> \"%s\"\n", left, right)
                }
        }

        free(base_buffer);
        return 0;
}
