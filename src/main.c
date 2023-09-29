#include "server.h"
#include "cfgroute.h"
#include "global.h"
#include "modules/cfgmodule.h"
#include "modules/modules.h"
#include "cli.h"
#include "gplogging.h"

#include <stdio.h>
#include <errno.h>

int apply_port(char *port)
{
        unsigned long lport = strtol(port, NULL, 0);
        switch (errno) {
                case ERANGE:
                case EINVAL:;
                        ERROR_LOG("Could not parse port number \"%s\".\n", port)
                        return -1;
        }

        short _port = (short) lport;
        listen_port = _port;
        return 0;
}

int parse_cli_args(int argc, char **argv)
{
        PARAM_ITER {
                BRANCH_IF("--port", "-p", apply_port, default_cli_error, true)
        }

        return 0;
}

int main(int argc, char **argv)
{
        printf("%s\n", banner);

        if (parse_cli_args(argc, argv) < 0) {
                return 0;
        }

        load_routes_file(ROUTES_FILE_NAME);
        load_modules_file(MODULES_FILE_NAME);
        modules_init_all();
        run_server();
        return 0;
}
