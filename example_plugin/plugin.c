//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#define KetamineModuleName "TestPlugin"

#include "../sdk/sdk.h"

ModuleRouteOverride(path)
{
        ModuleLog("A client requested the resource \"%s\"\n", path)
        return NULL;
}

ModuleInit
{
        ModuleLog("Initializing testing plugin.\n")
}