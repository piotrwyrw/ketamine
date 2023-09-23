//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#ifndef KETAMINE_LOGGING_H
#define KETAMINE_LOGGING_H

#define ModuleLog(...) \
        printf("[LOG: %s] ", KetamineModuleName); \
        printf(__VA_ARGS__);

#define ModuleError(...) \
        printf("[ERR: %s] ", KetamineModuleName); \
        printf(__VA_ARGS__);

#endif //KETAMINE_LOGGING_H
