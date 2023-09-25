//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#define KetamineModuleName "TestPlugin"

#include "../sdk/sdk.h"

#include <string.h>

RouteMapping(Greeting, req, resp)
{
        char *message = "Hello, World!\n";
        full_http_response(resp, 200, "OK", message, strnlen(message, MAX_STRING_LENGTH));
        return 0;
}

RouteMapping(Default, req, resp)
{
        char *message = "Nothing interesting here.";
        full_http_response(resp, 200, "ERR", message, strnlen(message, MAX_STRING_LENGTH));
        return 0;
}

Get(req, resp)
{
        MapRoute("/greeting", req, resp, Greeting)
        ForceRoute(req, resp, Default)
}

ModuleInit
{
        ModuleLog("Initializing testing plugin.\n")
}