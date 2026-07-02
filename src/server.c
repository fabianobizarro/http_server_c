#include "server.h"
#include "tcp.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

Server* init_server(int port)
{
    Server* server = malloc(sizeof(Server));

    if (!server) {
        return NULL;
    }

    server->port = port;
    server->routes_len = 0;
    // malloc routes here?
    server->www_root = NULL;

    return server;
}

size_t register_route(Server* server, http_method_e method, const char* path, void (*handler)(http_request* req, http_response* res))
{

    int length = server->routes_len;

    if (length == 0) {
        server->routes = malloc(sizeof(Route));
        server->routes[length].method = method;
        server->routes[length].handler = handler;
        strcpy(server->routes[length].path, path);
        length++;
    } else {
        length++;
        server->routes = realloc(server->routes, sizeof(Route) * length);
        server->routes[length].method = method;
        server->routes[length].handler = handler;
        strcpy(server->routes[length].path, path);
    }

    server->routes_len = length;

    return length;

    // if (route_count < MAX_ROUTES) {
    //     server->routes[route_count].method = method;
    //     strcpy(server->routes[route_count].path, path);
    //     server->routes[route_count].handler = handler;

    //     return ++route_count;
    // }

    // return route_count;
}

server_status_e start_server()
{
    return SERVER_OK;
}

void set_wwwroot(Server* server, const char* path)
{
    if (!server)
        return;

    if (!path)
        return;

    server->www_root = malloc(strlen(path));
    strcpy(server->www_root, path);
}
