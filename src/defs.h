//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#ifndef WEBSERVER_DEFS_H
#define WEBSERVER_DEFS_H

#define HANDLE(tag, err) \
        if (err < 0) {   \
                padding(indent); \
                printf("(Error : %s): %s\n", tag, strerror(errno));            \
                return -1; \
        }

#define SRVLOG(...) \
        padding(indent); \
        printf(__VA_ARGS__);

#define LOG(...) \
        printf("[Server] "); \
        printf(__VA_ARGS__);


#endif //WEBSERVER_DEFS_H
