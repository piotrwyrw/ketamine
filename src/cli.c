//
// Created by Piotr Krzysztof Wyrwas on 29.09.23.
//

#include "gplogging.h"

#include <stdio.h>

void default_cli_error(const char *_long, const char *_short)
{
        if (!_long) {
                ERROR_LOG("The long version string of the parameter is not supposed to be NULL.\n")
        }

        if (!_short) {
                ERROR_LOG("The short version string of the parameter is not supposed to be NULL.\n")
        }

        if (_long && _short) {
                ERROR_LOG("No value specified for parameter that needs a value (%s; %s)\n", _long, _short);
        }
}
