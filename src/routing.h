//
// Created by Piotr Krzysztof Wyrwas on 20.09.23.
//

#ifndef KETAMINE_ROUTING_H
#define KETAMINE_ROUTING_H

#include <stdbool.h>

#include "global.h"

#define MAX_ROUTES 100
#define DEFAULT_ROUTE NULL

typedef struct {
        char src[MAX_STRING_LENGTH];
        char target[MAX_STRING_LENGTH];
} route_entry;

extern route_entry routes[];
extern unsigned long route_count;

_Bool route(char *src, char *target);

char *resolve_route_raw(char *src);

char *resolve_route(char *src);

#endif //KETAMINE_ROUTING_H
