#include <iostream>
#include <slack/slack.h>
#include <mongoose/mongoose.h>
#include <future>

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
    switch (ev) {
    case MG_AUTH: return MG_TRUE;
    case MG_REQUEST:
        if (!strcmp(conn->uri, "/auth")) {
            char n1[100], n2[100];

            mg_get_var(conn, "n1", n1, sizeof(n1));
            mg_get_var(conn, "n2", n2, sizeof(n2));

            mg_printf_data(conn, "{ \"result\": %lf }", strtod(n1, NULL) + strtod(n2, NULL));

            return MG_TRUE;
        }
        mg_send_file(conn, "auth.html", NULL);
        return MG_MORE;
    default: return MG_FALSE;
    }
}


int main(int argc, char **argv)
{
    //TODO THIS IS SUPER-DUPER BRAIN-DEAD!
    std::async(std::launch::async, []{
        //create a Mongoose HTTP server
        struct mg_server *server;
        server = mg_create_server(NULL, ev_handler);

        mg_set_option(server, "listening_port", "8000");

        printf("Starting on port %s\n", mg_get_option(server, "listening_port"));

        while(1) mg_poll_server(server, 1000);

        // Cleanup, and free server instance
        mg_destroy_server(&server);
    });


    return 0;
}