//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#ifndef KETAMINE_BASE_H
#define KETAMINE_BASE_H

#include <stdio.h>

#include "../src/server.h"
#include "../src/http/http.h"

#define ModuleInit void kModule_Init()
#define MODULE_INIT_FUNCTION "kModule_Init"

#define ModuleRouteOverride(param) char *kModule_RouteOverride(char *param)
#define MODULE_ROUTE_OVERRIDE "kModule_RouteOverride"

#define Get(request, response) int kModule_GetHook(http_request *request, http_response *response)
#define MODULE_GET_HOOK "kModule_GetHook"

#define MapRoute(route, request, response, identifier) \
        if (strncmp(request->path, route, strnlen(route, MAX_STRING_LENGTH)) == 0) { \
                return kRouteMapping_##identifier(request, response); \
        }

#define ForceRoute(request, response, identifier) \
        return kRouteMapping_##identifier(request, response);

#define RouteMapping(identifier, request, response) \
        int kRouteMapping_##identifier(http_request *request, http_response *response)

#endif //KETAMINE_BASE_H
