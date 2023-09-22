//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#ifndef KETAMINE_DEFS_H
#define KETAMINE_DEFS_H

#define HANDLE_ERRORS(tag, err) \
        if (err < 0) {   \
                printf("[Error(%s)]: %s\n", tag, strerror(errno));            \
                return -1; \
        }

#define CONNECTION_ERROR(conn, ...) \
        printf("[ERR: %s | %d] ", conn->ip_addr, conn->conn_id);                      \
        printf(__VA_ARGS__);

#define CONNECTION_LOG(conn, ...) \
        printf("[LOG: %s | %d] ", conn->ip_addr, conn->conn_id);                      \
        printf(__VA_ARGS__);

#define INFO_LOG(...) \
        printf("[LOG] "); \
        printf(__VA_ARGS__);

#define ERROR_LOG(...) \
        printf("[ERR] "); \
        printf(__VA_ARGS__);

#endif //KETAMINE_DEFS_H
