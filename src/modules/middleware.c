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

int run_get_hook(http_request *req, http_response *resp)
{
        for (unsigned long i = 0; i < modules_count; i++) {
                int (*get_hook)(http_request *, http_response *) = loaded_modules[i].get_hook;

                if (!get_hook) {
                        continue;
                }

                if ((*get_hook)(req, resp) == 0) {
                        return 0;
                }
        }

        return -1;
}