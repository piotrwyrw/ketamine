//
// Created by Piotr Krzysztof Wyrwas on 24.09.23.
//

#ifndef KETAMINE_PARSE_H
#define KETAMINE_PARSE_H

#include "server.h"

#define IS_LETTER(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define IS_SPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')

char *identifier(char *str, client_handle *handle);

void remove_leading_spaces(char **str);

#endif //KETAMINE_PARSE_H
