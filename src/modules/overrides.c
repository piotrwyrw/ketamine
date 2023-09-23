//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#include "overrides.h"
#include "modules.h"

char *resolve_route_override(char *src)
{
        for (unsigned long i = 0; i < modules_count; i++) {
                char *(*plugin_override)(char *) = loaded_modules[i].route_override;

                if (!plugin_override) {
                        continue;
                }

                char *override = (*plugin_override)(src);

                if (!override) {
                        continue;
                }

                return override;
        }

        return NULL;
}