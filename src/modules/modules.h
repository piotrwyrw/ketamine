//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#ifndef KETAMINE_MODULES_H
#define KETAMINE_MODULES_H

#include "../global.h"
#include "../http/http.h"

typedef struct {
        char import_name[MAX_STRING_LENGTH];
        void *handle;

        void (*init)();

        char *(*route_override)(char *);

        char *(*get_hook)(http_request *);
} dynamic_module;

extern dynamic_module loaded_modules[];
extern unsigned long modules_count;

int load_module(char *name, char *path);

int load_module_symbols(dynamic_module *module);

void modules_init_all();

void modules_unload_all();

#endif //KETAMINE_MODULES_H
