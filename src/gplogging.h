//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#ifndef KETAMINE_GPLOGGING_H
#define KETAMINE_GPLOGGING_H

// General purpose logging

#define INFO_LOG(...) \
        printf("[LOG] "); \
        printf(__VA_ARGS__);

#define ERROR_LOG(...) \
        printf("[RSTATUS_ERR] "); \
        printf(__VA_ARGS__);

#endif //KETAMINE_GPLOGGING_H
