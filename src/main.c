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

    puts("Incoming request:");
    printf("\t%s %s | protocol %s\n", request.method, request.path, request.protocol);
    puts("\tHeaders");
    for (size_t i = 0; i < request.headers_count; i++) {
        printf("\t  %s: %s\n", request.headers[i].key, request.headers[i].value);
    }
    puts("");

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

    while (1) {
        puts("Waiting for client....");
        int client_fd = accept_client(server.socket_fd);
        if (client_fd == -1) {
            puts("Failed to accept client connection");
            continue;
        }

        puts("Client connected");

        handle_client(client_fd);

        http_response response;
        init_http_response(&response);
        add_http_header(&response, "Content-Type", "text/html");
        add_http_header(&response, "Connection", "close");
        set_response_body(&response, "<html><body><h1>Hello, world!</h1></body></html>");

        send_http_response(client_fd, &response);
        free_http_response(&response);

        close(client_fd);
        puts("Response sent to client.");
    }

    close(server.socket_fd);

    return 0;
}
