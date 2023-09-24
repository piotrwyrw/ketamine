//
// Created by Piotr Krzysztof Wyrwas on 24.09.23.
//

#ifndef KETAMINE_PARSE_H
#define KETAMINE_PARSE_H

#define IS_LETTER(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

extern char identifier_buffer[];

char *identifier(const char *str);

inline _Bool identifier_is(const char *str);

#endif //KETAMINE_PARSE_H
