// #include "route.h"
#include "server.h"
#include <http.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcp.h>

#define PORT 8080
#define WEB_ROOT "./www"

void hello_handler(http_request* _, http_response* res)
{
    res->status_code = 200;

    if (!res->body) {
        res->body = malloc(64);
    }

    strcpy(res->body, "Hello, World!\n");
    res->body_length = 14;

    add_respose_header(res, "Content-Length", "14");
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
    register_route(server, HTTP_METHOD_POST, "/hello", &hello_handler);

    // printf("Server struct\n");
    // printf("Port: %d\n", server->port);
    // printf("www root: %s | does contain \\0? %d \n", server->www_root, server->www_root[strlen(server->www_root)] == '\0');
    // printf("Routes count: %d\n", server->routes_len);

    start_server(server);

    free_server(server);
    server = NULL;

    return 0;
}
