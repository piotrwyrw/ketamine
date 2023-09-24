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

#define Get(request) char *kModule_GetHook(http_request *request)
#define MODULE_GET_HOOK "kModule_GetHook"

#define HTTP_NOT_FOUND(...) "HTTP/1.1 404 NOT_FOUND\r\nContent-Type: text/html\r\n\r\n" __VA_ARGS__

#define HTTP_OK(...) "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" __VA_ARGS__
#define HTTP_OK_JSON(...) "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n" __VA_ARGS__

#endif //KETAMINE_BASE_H
