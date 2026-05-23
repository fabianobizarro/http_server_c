#include <http.h>
#include <stdio.h>
#include <stdlib.h>
#include <tcp.h>

#define PORT 8080

void handle_client(int client_fd)
{
    http_request request = { 0 };

    if (read_http_request(client_fd, &request) == HTTP_PARSE_INVALID) {
        puts("Failed to parse request");
        close(client_fd);
        return;
    }

    if (parse_http_headers(request.buffer, &request) != HTTP_PARSE_OK) {
        puts("Failed to parse headers");
        close(client_fd);
        return;
    }

    puts("HTTP Request parsed successfully");
    printf("Method: %s\n", request.method);
    printf("Path: %s\n", request.path);
    printf("Protocol: %s\n", request.protocol);
    puts("Headers");
    for (size_t i = 0; i < request.headers_count; i++) {
        printf("%s: %s\n", request.headers[i].key, request.headers[i].value);
    }

    free(request.headers);
}

int main()
{
    tcp_server server = { 0 };
    server_status_e status = bind_tcp_port(&server, PORT);
    if (status != SERVER_OK) {
        puts("Server initialization failed");
        exit(EXIT_FAILURE);
    }

    int client_fd = accept_client(server.socket_fd);
    if (client_fd == -1) {
        puts("Failed to accept client connection");
        close(server.socket_fd);
        exit(EXIT_FAILURE);
    }

    puts("Client connected");

    handle_client(client_fd);

    close(client_fd);
    close(server.socket_fd);

    return 0;
}
