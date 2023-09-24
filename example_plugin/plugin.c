//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#define KetamineModuleName "TestPlugin"

#include "../sdk/sdk.h"

#include <string.h>

ModuleRouteOverride(path)
{
        ModuleLog("A client requested the resource \"%s\"\n", path)
        return NULL;
}

Get(req)
{
        if (strncmp(req->path, "/json", 4) != 0) {
                return HTTP_NOT_FOUND (
                               "<h1>404 Not found</h1>"
                               "<span>There's nothing on this route.</span>"
                       );
        }

        return HTTP_OK_JSON (
                       "{"
                       "  \"username\": \"iUseMacBtw\","
                       "  \"password\": \"password123\""
                       "}"
               );
}

ModuleInit
{
        ModuleLog("Initializing testing plugin.\n")
}