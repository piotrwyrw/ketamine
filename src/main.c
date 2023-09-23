#include "server.h"
#include "cfgroute.h"
#include "global.h"
#include "modules/modules.h"

#include <stdio.h>

int main(void)
{
        printf("%s\n", banner);
        load_routes_file(ROUTES_FILE_NAME);
        load_module("sample", "libsample_plugin.dylib");
        run_server();
        return 0;
}
