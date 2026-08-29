#include "dynamic_array.h"
#include "http.h"
#include "query_string.h"
#include "server.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define WEB_ROOT "./www"

void ensure_buffer_allocation(void* buffer, size_t* buffer_size, size_t additional_size)
{
    while (additional_size + 1 > *buffer_size) {
        *buffer_size *= 2;
        buffer = realloc(buffer, *buffer_size);
        if (!buffer) {
            puts("Failed to reallocate memory for the response header");
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }
}

void hello_handler(http_request* req, http_response* res)
{
    puts("EXECUTING HELLO HANDLER");

    res->status_code = 200;

    size_t buffer_size = 200;
    size_t offset = 0;
    char* buffer = calloc(buffer_size, sizeof(char));

    offset = snprintf(buffer, buffer_size, "Hello!\n");

    for (size_t i = 0; i < req->query_string->size; i++) {

        DynamicArray* darray = req->query_string->nodes[i]->arrayvalues;
        size_t additional_length = snprintf(
            NULL,
            0,
            "[%s] => ",
            req->query_string->nodes[i]->key);

        ensure_buffer_allocation(buffer, &buffer_size, offset + additional_length);

        offset += snprintf(buffer + offset, buffer_size - offset, "[%s] => ", req->query_string->nodes[i]->key);

        for (size_t j = 0; j < darray->count; j++) {
            additional_length += snprintf(NULL, 0, "%s ", darray->values[j]);
            ensure_buffer_allocation(buffer, &buffer_size, offset + additional_length);

            offset += snprintf(buffer + offset, buffer_size - offset, "%s ", darray->values[j]);
        }

        additional_length += snprintf(NULL, 0, "\n");
        ensure_buffer_allocation(buffer, &buffer_size, offset + additional_length);
        offset += snprintf(buffer + offset, buffer_size - offset, "\n");
    }

    res->body = malloc(buffer_size);
    strncpy(res->body, buffer, buffer_size);
    res->body_length = offset;
    res->body_length = strlen(buffer);

    char content_length[32] = { 0 };
    snprintf(content_length, sizeof(content_length), "%zu", offset);
    add_respose_header(res, "Content-Length", content_length);
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
