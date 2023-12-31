cmake_minimum_required(VERSION 3.26.0)

project(ketamine C)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

set(KETAMINE_SOURCES src/main.c
        src/http/http.c
        src/global.h
        src/gplogging.h
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
        example_plugin/plugin.c
        sdk/base.h
        src/modules/middleware.h
        src/modules/middleware.c
        src/slog.h
        sdk/sdk.h
        sdk/logging.h
        src/parse.h
        src/parse.c
        src/modules/cfgmodule.h
        src/modules/cfgmodule.c
        src/cli.h
        src/cli.c
)

# Build the ketamine server excutable
add_executable(ketamine ${KETAMINE_SOURCES})

add_library(libketamine STATIC ${KETAMINE_SOURCES})
set_target_properties(libketamine PROPERTIES OUTPUT_NAME "libketamine" PREFIX "")

# Build the project as a shared (dynamic) library and statically link it with ketamine
add_library(plugin SHARED example_plugin/plugin.c)
target_link_libraries(plugin libketamine)
set_target_properties(plugin PROPERTIES OUTPUT_NAME "plugin" PREFIX "")