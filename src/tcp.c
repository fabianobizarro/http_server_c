#include <stdio.h>
#include <string.h>
#include <tcp.h>

#define BACKLOG 5

server_status_e bind_tcp_port(tcp_server* server, int port)
{
    int opt = 1;
    memset(server, 0, sizeof(*server));
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server->socket_fd == -1) {
        puts("Socket creation failed");
        return SERVER_SOCKET_ERROR;
    }

    // making the socket port reusable
    if (setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        return SERVER_SOCKET_ERROR;
    }

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(port);

    if (bind(server->socket_fd, (struct sockaddr*)&server->address, sizeof(server->address)) < 0) {
        printf("Bind failed: ");
        perror("bind");
        close(server->socket_fd);
        return SERVER_BIND_ERROR;
    }

    if (listen(server->socket_fd, BACKLOG) < 0) {
        puts("Listen failed");
        close(server->socket_fd);
        return SERVER_LISTEN_ERROR;
    }

    printf("Server running and listening at port %d\n", port);
    return SERVER_OK;
}

int accept_client(int server_fd)
{
    struct sockaddr_in client_addr = { 0 };
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        puts("Accept failed");
        return -1;
    }

    return client_fd;
}
