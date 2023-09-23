//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#ifndef KETAMINE_BASE_H
#define KETAMINE_BASE_H

#include <stdio.h>

#define ModuleInit void kModule_Init()
#define MODULE_INIT_FUNCTION "kModule_Init"

#define ModuleRouteOverride(param) char *kModule_RouteOverride(char *param)
#define MODULE_ROUTE_OVERRIDE "kModule_RouteOverride"

#endif //KETAMINE_BASE_H
