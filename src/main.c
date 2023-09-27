#include "server.h"
#include "cfgroute.h"
#include "global.h"
#include "modules/cfgmodule.h"
#include "modules/modules.h"

#include <stdio.h>

int main(void)
{
        printf("%s\n", banner);
        load_routes_file(ROUTES_FILE_NAME);
        load_modules_file(MODULES_FILE_NAME);
        modules_init_all();
        run_server();
        return 0;
}
