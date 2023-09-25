//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#include "modules.h"
#include "../gplogging.h"
#include "../../sdk/base.h"

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
                ERROR_LOG("Failed to load dynamic module: Number of loaded modules would exceed the allowed range.\n")
                return -1;
        }

        void *handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);

        if (!handle) {
                ERROR_LOG("Failed to load dynamic module \"%s\" at \"%s\": %s\n", name, path, dlerror())
                return -1;
        }

        dynamic_module *mod = &(loaded_modules[modules_count]);
        mod->handle = handle;
        strncpy(mod->import_name, name, MAX_STRING_LENGTH);

        if (load_module_symbols(mod) < 0) {
                ERROR_LOG("Failed to load module \"%s\" (%s)\n", name, path)
                return -1;
        }

        INFO_LOG("Module loaded: %s (%s)\n", name, path)

        modules_count++;

        return 0;
}

static int load_module_symbol(void *handle, void **target, char *symbol, _Bool required)
{
        if (!handle || !target) {
                return -1;
        }

        void *sym = dlsym(handle, symbol);

        if (!sym) {
                if (required) {
                        ERROR_LOG("Module symbol not found: %s\n", symbol)
                }
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
                ERROR_LOG("Could not load module callbacks: Module is NULL.\n")
                return -1;
        }

        TRY(load_module_symbol(module->handle, (void **) &(module->init), MODULE_INIT_FUNCTION, true))
        load_module_symbol(module->handle, (void **) &(module->route_override), MODULE_ROUTE_OVERRIDE, false);
        load_module_symbol(module->handle, (void **) &(module->get_hook), MODULE_GET_HOOK, false);

        return 0;
}

#undef TRY

void modules_init_all()
{
        for (unsigned long i = 0; i < modules_count; i++) {
                dynamic_module *mod = &(loaded_modules[i]);
                (*mod->init)();
        }
}

void modules_unload_all()
{
        if (modules_count == 0) {
                INFO_LOG("No dynamic modules to unload.\n")
                return;
        }

        for (unsigned long i = 0; i < modules_count; i++) {
                dynamic_module *mod = &(loaded_modules[i]);

                if (!mod->handle) {
                        continue;
                }

                if (dlclose(mod->handle) == 0) {
                        INFO_LOG("Unloaded module \"%s\"\n", mod->import_name)
                } else {
                        ERROR_LOG("Could not unload module \"%s\": %s\n", mod->import_name, dlerror())
                }
        }

        modules_count = 0;
}