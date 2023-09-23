//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#include "../sdk/module_sdk.h"

void internal_ketamine_function();

__ketamine_register("SDKTest")

__ketamine__init
{
        internal_ketamine_function();
}