//
// Created by Piotr Krzysztof Wyrwas on 21.09.23.
//

#ifndef KETAMINE_CLIENT_H
#define KETAMINE_CLIENT_H

typedef enum {
        GET_OK,
        GET_ERR,
        EXIT,
        ERR = -1
} request_status;



void *handle_client_connection(void *param);

#endif //KETAMINE_CLIENT_H
