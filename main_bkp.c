// example

int main2()
{
    tcp_server server = { 0 };
    server_status_e status = bind_tcp_port(&server, PORT);
    if (status != SERVER_OK) {
        puts("Server initialization failed");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        puts("Waiting for client....");
        int client_fd = accept_client(server.socket_fd);
        if (client_fd == -1) {
            puts("Failed to accept client connection");
            continue;
        }

        puts("Client connected");

        http_response response = { 0 };
        http_request request = { 0 };

        init_http_response(&response);

        register_route(HTTP_METHOD_GET, "/hello", hello_handler);

        if (read_http_request(client_fd, &request) != HTTP_PARSE_OK) {
            puts("Failed to parse request");
            close(client_fd);
            return 0;
        }

        if (parse_http_headers(request.buffer, &request) != HTTP_PARSE_OK) {
            puts("Failed to parse headers");
            close(client_fd);
            return 0;
        }

        char sanitized_path[1024] = { 0 };
        sanitize_path(request.path, sanitized_path, sizeof(sanitized_path));

        // printf("Sanitized Path: %s\n", sanitized_path);

        if (!handle_request(&request, &response))
            serve_file(sanitized_path, &response);

        send_http_response(client_fd, &response);
        free_http_response(&response);

        close(client_fd);
        puts("Response sent to client.");
    }

    close(server.socket_fd);

    return 0;
}
