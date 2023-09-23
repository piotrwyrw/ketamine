//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#ifndef KETAMINE_GLOBAL_H
#define KETAMINE_GLOBAL_H

#define WITH_DEBUG_FEATURES

#define INDEX_FILE_NAME "index.html"
#define ROUTES_FILE_NAME "routes"
#define LISTEN_PORT 80
#define MAX_STRING_LENGTH 100
#define CONNECTION_LIMIT 20

#include <stdbool.h>

extern unsigned long running_threads;
extern _Bool running;

extern const char *banner;

#endif //KETAMINE_GLOBAL_H
