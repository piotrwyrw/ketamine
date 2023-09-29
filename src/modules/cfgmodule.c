//
// Created by Piotr Krzysztof Wyrwas on 26.09.23.
//

#include "cfgmodule.h"
#include "../gplogging.h"
#include "../fio.h"
#include "modules.h"

#include <stdio.h>
#include <string.h>

int load_modules_file(char *path)
{
        // Read the file into memory
        if (!path) {
                ERROR_LOG("Path for the modules file is empty.\n")
                return -1;
        }

        char *base_buffer; // Warning: This is a dynamically allocated  buffer - will need explicit free()-ing
        char *buffer;
        unsigned long length;

        if (read_file_to(path, &base_buffer, &length) < 0) {
                ERROR_LOG("Could not read modules file: \"%s\"\n", path)
                return -1;
        }

        buffer = base_buffer;

        // -- Parse the modules file --

        char *line;

        // Loop through all lines
        while ((line = strsep(&buffer, "\n"))) {

                char *module_path = line;

                int route_status = load_module(module_path);

                if (route_status < 0) {
                        ERROR_LOG("Modules: Could not load module \"%s\"\n", module_path)
                        continue;
                }
        }

        INFO_LOG("~~ Done reading modules file ~~\n")

        free(base_buffer);
        return 0;

}
