#include "server.h"
#include "routing.h"
#include "cfgroute.h"

int main(void)
{
        load_routes_file(ROUTES_FILE_NAME);
        run_server();
        return 0;
}
