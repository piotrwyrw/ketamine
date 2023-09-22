#include "server.h"
#include "routing.h"

int main(void)
{
        route("/", "home.html");
        route("/contact", "contact.html");
        run_server();
        return 0;
}
