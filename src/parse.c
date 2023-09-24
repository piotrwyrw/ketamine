//
// Created by Piotr Krzysztof Wyrwas on 24.09.23.
//

#include "parse.h"
#include "global.h"
#include "gplogging.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char identifier_buffer[MAX_STRING_LENGTH] = {0};

/**
 * @param   str - The string to be parsed
 * @return  <b>On Success</b> - A pointer to the next character after the identifier.
 *          <b>On Failure</b> - NULL, if <b>str</b> is NULL.
 */
char *identifier(const char *str)
{
        // Clear the identifier buffer before scanning a new one
        memset(identifier_buffer, 0, MAX_STRING_LENGTH);

        if (!str) {
                return NULL;
        }

        char *ptr = identifier_buffer;

        while (IS_LETTER(*str)) {
                *(ptr++) = *(str++);
                if (ptr - identifier_buffer > MAX_STRING_LENGTH) {
                        ERROR_LOG("Failed while parsing identifier as it exceeds the allowed string length bounds.\n")
                        return NULL;
                }
        }

        return ptr;
}

inline _Bool identifier_is(const char *str)
{
        return strncmp(identifier_buffer, str, MAX_STRING_LENGTH) == 0;
}
