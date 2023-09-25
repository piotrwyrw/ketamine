//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#ifndef KETAMINE_MIDDLEWARE_H
#define KETAMINE_MIDDLEWARE_H

#include "../http/http.h"

char *resolve_route_override(char *src);

int run_get_hook(http_unit *req, http_unit *resp);

#endif //KETAMINE_MIDDLEWARE_H
