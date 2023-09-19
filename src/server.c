//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#include "server.h"

#include "parse.h"
#include "settings.h"
#include "defs.h"
#include "global.h"
#include "fio.h"
#include "util.h"

#include <stdbool.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int respond_get(int sockd, char *file)
{
        int status = read_file(file);
        if (status < 0) {
                SRVLOG("Failed while trying to read file \"%s\"\n", file);
                return -1;
        }

        SRVLOG("Resource found - Responding with data\n");

        status = send(sockd, file_buffer, file_size, 0) >= 0 ? 0 : -1;
        HANDLE("write", status)

        return 0;
}

int run_server()
{
        int _true = true;

        int sockd = socket(AF_INET, SOCK_STREAM, 0);
        HANDLE("socket", sockd)

        int status = setsockopt(sockd, SOL_SOCKET, SO_REUSEADDR, &_true, sizeof(int));
        HANDLE("setsockopt", status)

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(LISTEN_PORT);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        status = bind(sockd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
        HANDLE("bind", status)

        status = listen(sockd, 10);
        HANDLE("listen", status)

        struct sockaddr_in in_addr;
        socklen_t addr_len;

        char buffer[1001] = {0};
        char *ip_addr;
        char *http_req;
        int client_socket;

        LOG("Server is UP AND RUNNING on port %d\n\n", LISTEN_PORT)

        while ((status = accept(sockd, (struct sockaddr *) &in_addr, &addr_len))) {
                HANDLE("accept", status)
                client_socket = status;

                if (in_addr.sin_family != AF_INET) {
                        close(client_socket);
                        continue;
                }

                ip_addr = inet_ntoa(in_addr.sin_addr);

                SRVLOG(">>>>>> %s\n", ip_addr);

                indent++;

                ssize_t req_size = recv(client_socket, buffer, 1000, 0);

                if (req_size == 0) {
                        SRVLOG("No incoming HTTP requests.\n");
                        goto close_session;
                }

                http_req = parse_request(buffer);

                if (!http_req) {
                        SRVLOG("Failed to parse incoming request.\n");
                        goto close_session;
                }

                SRVLOG("GET \"%s\"\n", http_req);

                respond_get(client_socket, http_req);


                close_session:
                free_file_buffer();
                status = close(client_socket);
                HANDLE("close", status);

                indent--;
                SRVLOG("<<<<<< %s\n\n", ip_addr);
        }

        return 0;
}