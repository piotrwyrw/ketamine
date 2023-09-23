//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#ifndef KETAMINE_MODULE_SDK_H
#define KETAMINE_MODULE_SDK_H

#define __ketamine_register(id) \
        const char *kModule_name() { \
                return id;                        \
        }

#define MODULE_NAME_FUNCTION "kModule_name"

#define __ketamine__init void kModule_init()
#define MODULE_INIT_FUNCTION "kModule_init"

#endif //KETAMINE_MODULE_SDK_H
