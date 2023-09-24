//
// Created by Piotr Krzysztof Wyrwas on 24.09.23.
//

#include "parse.h"
#include "global.h"
#include "gplogging.h"

#include <string.h>
#include <stdio.h>

/**
 * @param   str - The string to be parsed
 * @return  <b>On Success</b> - A pointer to the next character after the identifier.
 *          <b>On Failure</b> - NULL, if <b>str</b> is NULL.
 */
char *identifier(char *str, client_handle *handle)
{
        // Clear the identifier buffer before scanning a new one
        memset(handle->parser_buffer, 0, MAX_STRING_LENGTH);

        if (!str) {
                return NULL;
        }

        char *ptr = handle->parser_buffer;

        while (IS_LETTER(*str)) {
                *(ptr++) = *(str++);
                if (ptr - handle->parser_buffer > MAX_STRING_LENGTH) {
                        ERROR_LOG("Failed while parsing identifier as it exceeds the allowed string length bounds.\n")
                        return NULL;
                }
        }

        return str;
}

void remove_leading_spaces(char **str)
{
        if (!str) {
                return;
        }

        if (!*str) {
                return;
        }

        unsigned long length;

        if ((length = strnlen(*str, MAX_STRING_LENGTH)) == 0) {
                return;
        }

        while (IS_SPACE(**str)) {
                (*str)++;

                // Ensure we don't go out of range
                length--;
                if (length == 0) {
                        break;
                }
        }
}