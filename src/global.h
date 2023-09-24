//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#ifndef KETAMINE_GLOBAL_H
#define KETAMINE_GLOBAL_H

#include <stdbool.h>
#include <pthread.h>

#define WITH_DEBUG_FEATURES

#define INDEX_FILE_NAME "index.html"
#define ROUTES_FILE_NAME "routes"
#define LISTEN_PORT 80

#define MAX_STATUS_MESSAGE_LENGTH 50
#define MAX_STRING_LENGTH 100
#define MAX_HEADER_COUNT 15
#define CONNECTION_LIMIT 10

#define MAX_ROUTES 100
#define MAX_DYNAMIC_MODULES 100

extern const char *banner;

extern pthread_mutex_t global_mutex;
extern unsigned long running_threads;
extern _Bool running;

#endif //KETAMINE_GLOBAL_H
