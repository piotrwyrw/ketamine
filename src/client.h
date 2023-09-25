//
// Created by Piotr Krzysztof Wyrwas on 21.09.23.
//

#ifndef KETAMINE_CLIENT_H
#define KETAMINE_CLIENT_H

typedef enum {
        RSTATUS_DEFAULT,
        RSTATUS_RESOURCE_UNAVAILABLE,

        RSTATUS_OK,
        RSTATUS_EXIT,
        RSTATUS_ERR = -1
} request_status;



void *handle_client_connection(void *param);

#endif //KETAMINE_CLIENT_H
