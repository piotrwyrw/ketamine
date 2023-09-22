#include "server.h"
#include "routing.h"
#include "cfgroute.h"
#include "global.h"

#include <stdio.h>

int main(void)
{
        printf("%s\n", banner);
        load_routes_file(ROUTES_FILE_NAME);
        run_server();
        return 0;
}
