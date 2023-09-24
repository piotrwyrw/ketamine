//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#include "middleware.h"
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

char *run_get_hook(http_request *req)
{
        for (unsigned long i = 0; i < modules_count; i++) {
                char *(*get_hook)(http_request *) = loaded_modules[i].get_hook;

                if (!get_hook) {
                        continue;
                }

                char *override = (*get_hook)(req);

                if (!override) {
                        continue;
                }

                return override;
        }

        return NULL;
}