//
// Created by Piotr Krzysztof Wyrwas on 22.09.23.
//

#include "routing.h"
#include "global.h"
#include "defs.h"

#include <string.h>
#include <stdio.h>

_Bool route_directory_set = false;
char route_directory[MAX_STRING_LENGTH] = {0};
route_entry routes[MAX_ROUTES];
unsigned long route_count = 0;

_Bool route(char *src, char *target)
{
        if (!src || !target) {
                return false;
        }

        if (strncmp(src, "[root]", 6) == 0) {
                if (route_directory_set) {
                        ERROR_LOG("Route root directory is already set - cannot overwrite.\n")
                        return false;
                }
                strncpy(route_directory, target, MAX_STRING_LENGTH);
                INFO_LOG("Working directory set to \"%s\"\n", target);
                route_directory_set = true;
                return true;
        }

        if (route_count >= MAX_ROUTES) {
                return false;
        }

        if (resolve_route_raw(src)) {
                ERROR_LOG("Attempted to add conflicting route for \"%s\": \"%s\"\n", src, target)
                return false;
        }

        size_t src_len = strnlen(src, MAX_STRING_LENGTH);
        size_t target_len = strnlen(target, MAX_STRING_LENGTH);

        strncpy(routes[route_count].src, src, src_len);
        strncpy(routes[route_count].target, target, target_len);

        route_count++;

        return true;
}

char *resolve_route_raw(char *src)
{
        if (!src) {
                return NULL;
        }

        size_t src_len = strnlen(src, MAX_STRING_LENGTH);

        char *target = NULL;

        for (unsigned long i = 0; i < route_count; i++) {
                size_t route_src_len = strnlen(routes[i].src, MAX_STRING_LENGTH);

                // If the lengths of two strings are not the same, they may not be equal
                if (src_len != route_src_len) {
                        continue;
                }

                if (strncmp(routes[i].src, src, src_len) != 0) {
                        continue;
                }

                target = routes[i].target;
                break;
        }

        return target;
}

// Resolve all routes (including cross-referencing routes)
char *resolve_all_routes(char *src)
{
        char *target = resolve_route_raw(src);
        char *tmp_target;

        while ((tmp_target = resolve_route_raw(target))) {
                target = tmp_target;
        }

        return target;
}

char *resolve_route(char *src)
{
        return resolve_all_routes(src);
}