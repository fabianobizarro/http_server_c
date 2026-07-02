#include "server.h"
#include "http.h"
#include "tcp.h"
#include <stddef.h>
#include <stdio.h>
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
    server->www_root = NULL;

    return server;
}

size_t register_route(Server* server, http_method_e method, const char* path, void (*handler)(http_request* req, http_response* res))
{
    int pos = server->routes_len;
    server->routes_len++;
    server->routes = realloc(server->routes, sizeof(Route) * server->routes_len);

    if (!server->routes) {
        puts("Failed to allocate memory for the server routes.");
        exit(EXIT_FAILURE);
    }

    server->routes[pos].method = method;
    server->routes[pos].handler = handler;
    strcpy(server->routes[pos].path, path);

    return server->routes_len;
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

void free_server(Server* server)
{
    free(server->routes);
    server->routes = NULL;

    free(server->www_root);
    server->www_root = NULL;

    free(server);
    server = NULL;
}

bool handle_request(Server* server, http_request* request, http_response* response)
{

    for (int i = 0; i < server->routes_len; i++) {
        
        printf("%d %s\n", server->routes[i].method, server->routes[i].path);
        
        if (strcmp(server->routes[i].path, request->path) == 0 && server->routes[i].method == request->method_e) {
            server->routes[i].handler(request, response);
            return true;
        }
    }

    return false;
}

bool handle_request2(Server* server, http_request* request, http_response* response)
{
    if (handle_request(server, request, response))
        return true;

    char sanitized_path[1024] = { 0 };
    sanitize_path(server->www_root, request->path, sanitized_path, sizeof(sanitized_path));

    if (serve_file(sanitized_path, response))
        return true;

    response->status_code = 404;
    strncpy(response->reason_phrase, "Not Found", sizeof(response->reason_phrase) - 1);

    return false;
}

server_status_e start_server(Server* server)
{
    tcp_server tcp_server = { 0 };
    server_status_e status = bind_tcp_port(&tcp_server, server->port);
    if (status != SERVER_OK) {
        puts("Server initialization failed");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        puts("Waiting for client....");
        int client_fd = accept_client(tcp_server.socket_fd);
        if (client_fd == -1) {
            puts("Failed to accept client connection");
            continue;
        }

        puts("Client connected");
        http_response response = { 0 };
        http_request request = { 0 };

        init_http_response(&response);

        if (parse_http_request(client_fd, &request) != HTTP_PARSE_OK) {
            puts("Failed to parse request");
            close(client_fd);
            return 0;
        }

        if (parse_request_headers(request.buffer, &request) != HTTP_PARSE_OK) {
            puts("Failed to parse headers");
            close(client_fd);
            return 0;
        }

        handle_request2(server, &request, &response);

        // char sanitized_path[1024] = { 0 };
        // sanitize_path(request.path, sanitized_path, sizeof(sanitized_path));

        // // printf("Sanitized Path: %s\n", sanitized_path);

        // if (!handle_request(server, &request, &response))
        //     serve_file(sanitized_path, &response);

        send_http_response(client_fd, &response);
        free_http_response(&response);

        close(client_fd);
        puts("Response sent to client.");
    }

    close(tcp_server.socket_fd);

    return 0;
}
