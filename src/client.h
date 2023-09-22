//
// Created by Piotr Krzysztof Wyrwas on 21.09.23.
//

#ifndef WEBSERVER_CLIENT_H
#define WEBSERVER_CLIENT_H

typedef enum {
        GET_OK,
        GET_ERR,
        EXIT,
        ERR = -1
} request_status;



void *handle_client_connection(void *param);

#endif //WEBSERVER_CLIENT_H
