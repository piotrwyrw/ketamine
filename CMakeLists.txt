cmake_minimum_required(VERSION 3.26.0)

project(ketamine C)
set(KETAMINE_SOURCES src/main.c
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
        src/modules/modules.h
        src/modules/modules.c
        src/example.c
        example_plugin/plugin.c
        sdk/module_sdk.h)

add_executable(ketamine ${KETAMINE_SOURCES})

add_library(libketamine STATIC ${KETAMINE_SOURCES})

add_library(sample_plugin SHARED example_plugin/plugin.c)
target_link_libraries(sample_plugin libketamine)
