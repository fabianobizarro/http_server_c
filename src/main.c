#include "http.h"
#include "server.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define WEB_ROOT "./www"

void hello_handler(http_request* _, http_response* res)
{
    puts("EXECUTING HELLO HANDLER");

    res->status_code = 200;

    printf("From request handler Path: %s\n", _->path);
    printf("From request handler  Query: %s\n", _->query);

    if (!res->body) {
        res->body = malloc(64);
    }

    strcpy(res->body, "Hello, World!");
    res->body_length = 13;

    add_respose_header(res, "Content-Length", "13");
}

int main()
{
    Server* server;

    server = init_server(PORT);

    if (!server) {
        puts("Failed to initialize server");
        exit(EXIT_FAILURE);
    }

    set_wwwroot(server, WEB_ROOT);

    register_route(server, HTTP_METHOD_GET, "/", &hello_handler);
    register_route(server, HTTP_METHOD_GET, "/hello", &hello_handler);
    register_route(server, HTTP_METHOD_POST, "/hello", &hello_handler);

    start_server(server);

    free_server(server);
    server = NULL;

    return 0;
}
