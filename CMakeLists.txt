cmake_minimum_required(VERSION 3.26.0)

project(ketamine C)
add_executable(ketamine src/main.c
        src/parse.c
        src/global.h
        src/defs.h
        src/global.c
        src/fio.h
        src/fio.c
        src/server.h
        src/server.c
        src/routing.h
        src/client.h
        src/client.c
        src/routing.c
        src/cfgroute.h
        src/cfgroute.c
        src/threadsafe.c
        src/threadsafe.h
)