cmake_minimum_required(VERSION 3.26.0)

project(WebServer C)
add_executable(server src/main.c
        src/parse.c
        src/settings.h
        src/defs.h
        src/global.h
        src/global.c
        src/fio.h
        src/fio.c
        src/util.h
        src/util.c
        src/server.h
        src/server.c
        src/routing.h
        src/client.h
        src/client.c
        src/routing.c)