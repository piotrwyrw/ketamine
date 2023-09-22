//
// Created by Piotr Krzysztof Wyrwas on 22.09.23.
//

#include "routing.h"

#include <string.h>

route_entry routes[MAX_ROUTES];
unsigned long route_count = 0;

_Bool route(char *src, char *target)
{
        if (route_count >= MAX_ROUTES) {
                return false;
        }

        strcpy(routes[route_count].src, src);
        strcpy(routes[route_count].target, target);

        route_count++;

        return true;
}

char *resolve_route(char *src)
{
        for (unsigned long i = 0; i < route_count; i++) {
                if (strcmp(routes[i].src, src) != 0) {
                        continue;
                }
                return routes[i].target;
        }

        return DEFAULT_ROUTE;
}