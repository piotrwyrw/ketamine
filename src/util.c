//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void padding(unsigned int count)
{
        char *s = calloc(count + 1, sizeof(char));
        memset(s, '\t', count);
        printf("%s", s);
        free(s);
}