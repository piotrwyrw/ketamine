//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#include "modules.h"
#include "../defs.h"
#include "../../sdk/module_sdk.h"

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

dynamic_module loaded_modules[MAX_DYNAMIC_MODULES];
unsigned long modules_count = 0;

int load_module(char *name, char *path)
{
        if (!name || !path) {
                return -1;
        }

        if (modules_count + 1 > MAX_DYNAMIC_MODULES) {
                ERROR_LOG("Failed to load dynamic module: Number of loaded modules would exceed the allowed range.\n");
                return -1;
        }

        void *handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);

        if (!handle) {
                ERROR_LOG("Failed to load dynamic module \"%s\" at \"%s\": %s\n", name, path, dlerror());
                return -1;
        }

        dynamic_module *mod = &(loaded_modules[modules_count]);
        mod->handle = handle;
        strncpy(mod->import_name, name, MAX_STRING_LENGTH);

        if (load_module_symbols(mod) < 0) {
                ERROR_LOG("Failed to load module \"%s\" (%s)\n", name, path)
                return -1;
        }

        const char *logging_identifier = (*mod->name)();

        if (!logging_identifier) {
                ERROR_LOG("Logging identifier of module \"%s\" must not be NULL!\n", name);
                dlclose(handle);
                return -1;
        }

        INFO_LOG("Loaded module from \"%s\" as \"%s\" identified by \"%s\" \n", path, name, (*mod->name)());

        modules_count++;

        return 0;
}

static int load_module_symbol(void *handle, void **target, char *symbol)
{
        if (!handle || !target) {
                return -1;
        }

        void *sym = dlsym(handle, symbol);

        if (!sym) {
                ERROR_LOG("Module symbol not found: %s\n", symbol);
                return -1;
        }

        *target = sym;
        return 0;
}

#define TRY(...) \
        if (__VA_ARGS__ < 0) { \
                return -1;         \
        }

int load_module_symbols(dynamic_module *module)
{
        if (!module) {
                ERROR_LOG("Could not load module callbacks: Module is NULL.\n");
                return -1;
        }

        TRY(load_module_symbol(module->handle, (void **) &(module->init), MODULE_INIT_FUNCTION))
        TRY(load_module_symbol(module->handle, (void **) &module->name, MODULE_NAME_FUNCTION))

        return 0;
}

#undef TRY