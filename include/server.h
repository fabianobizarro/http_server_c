#ifndef SERVER_H
#define SERVER_H

#include "http.h"
#include "tcp.h"

#define MAX_ROUTES 100

// typedef enum {
//     SERVER_STATUS_OK,
//     SERVER_STATUS_ERROR
// } server_status_e;

typedef struct {
    http_method_e method;
    char path[128];
    void (*handler)(http_request* req, http_response* res);
} Route;

typedef struct {
    int port;
    Route* routes;
    int routes_len;
    char* www_root;
} Server;

// typedef void (*handler)(http_request* req, http_response* res);

/**
 * Initializes the Server structure
 */
Server* init_server(int port);

void set_wwwroot(Server* server, const char* path);

size_t register_route(Server* server, http_method_e method, const char* path, void (*handler)(http_request* req, http_response* res));

/**
 * socket, bind, listen, accept, loop
 */
server_status_e start_server(Server* server);

void free_server(Server* server);

#endif
