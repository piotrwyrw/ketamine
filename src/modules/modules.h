//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#ifndef KETAMINE_MODULES_H
#define KETAMINE_MODULES_H

#include "../global.h"

typedef struct {
        char import_name[MAX_STRING_LENGTH];
        void *handle;

        void (*init)();

        const char *(*name)();
} dynamic_module;

extern dynamic_module loaded_modules[];
extern unsigned long modules_count;

int load_module(char *name, char *path);

int load_module_symbols(dynamic_module *module);

void modules_init_all();

#endif //KETAMINE_MODULES_H
