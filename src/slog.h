//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

// Special logging features

#ifndef KETAMINE_SLOG_H
#define KETAMINE_SLOG_H

#define HANDLE_ERRORS(tag, err) \
        if ((err) < 0) {   \
                printf("[Error(%s)]: %s\n", tag, strerror(errno));            \
                return -1; \
        }

#define HANDLE_ERRORS_NORETURN(tag, err) \
        if ((err) < 0) {   \
                printf("[Error(%s)]: %s\n", tag, strerror(errno));            \
        }

#define CONNECTION_ERROR(conn, ...) \
        printf("[ERR: %s | %d] ", conn->ip_addr, conn->conn_id);                      \
        printf(__VA_ARGS__);

#define CONNECTION_LOG(conn, ...) \
        printf("[LOG: %s | %d] ", conn->ip_addr, conn->conn_id);                      \
        printf(__VA_ARGS__);

#endif //KETAMINE_SLOG_H
