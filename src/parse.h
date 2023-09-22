//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#ifndef WEBSERVER_PARSE_H
#define WEBSERVER_PARSE_H

#include "server.h"

char *parse_request(char *req, client_handle *handle);

#endif //WEBSERVER_PARSE_H
