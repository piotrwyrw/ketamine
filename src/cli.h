//
// Created by Piotr Krzysztof Wyrwas on 29.09.23.
//

#ifndef KETAMINE_CLI_H
#define KETAMINE_CLI_H

#include <string.h>

#define BRANCH_IF(a, b, fn, err, needsValue) \
        if (!a || !b || cliParamIdx + 1 >= argc) { \
                err(a, b);                 \
                return -1;                           \
        } else if (strncmp(argv[cliParamIdx], a, MAX_STRING_LENGTH) == 0 || strncmp(argv[cliParamIdx], b, MAX_STRING_LENGTH) == 0) { \
                if (cliParamIdx + 1 >= argc) {     \
                        if (needsValue) {                   \
                                err(a, b); \
                                return -1;\
                        }                  \
                        continue;                           \
                } else { \
                        cliParamIdx ++; \
                        if (fn(argv[cliParamIdx]) < 0) {      \
                                return -1;                     \
                        }     \
                        continue; \
                } \
        }

#define PARAM_ITER \
        for (unsigned long cliParamIdx = 1; cliParamIdx < argc; cliParamIdx ++)

void default_cli_error(char *_long, char *_short);

#endif //KETAMINE_CLI_H
