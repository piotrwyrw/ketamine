//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#ifndef WEBSERVER_FIO_H
#define WEBSERVER_FIO_H

#include "server.h"

int read_file_to(char *path, char **buffer, unsigned long *length);

int read_file(char *path, client_handle *handle);

void free_file_buffer(client_handle *handle);

#endif //WEBSERVER_FIO_H
