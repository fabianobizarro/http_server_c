#include "query_string.h"
#include <http.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define HTTP_METHODS_COUNT 9
#define HTTP_METHODS { "GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH" }

const http_method_record HTTP_METHODS_DICT[] = {
    { HTTP_METHOD_GET, "GET" },
    { HTTP_METHOD_HEAD, "HEAD" },
    { HTTP_METHOD_POST, "POST" },
    { HTTP_METHOD_PUT, "PUT" },
    { HTTP_METHOD_DELETE, "DELETE" },
    { HTTP_METHOD_CONNECT, "CONNECT" },
    { HTTP_METHOD_OPTIONS, "OPTIONS" },
    { HTTP_METHOD_TRACE, "TRACE" },
    { HTTP_METHOD_PATCH, "PATCH" },
};

http_parse_e parse_http_request(int socket_fd, http_request* request)
{
    ssize_t bytes_read = read(socket_fd, request->buffer, sizeof(request->buffer) - 1);

    if (bytes_read <= 0) {
        return HTTP_PARSE_INVALID;
    }

    request->buffer[bytes_read] = '\0';

    // parse method, target and protocol
    if (sscanf(request->buffer, "%7s %2047s %15s", request->method, request->target, request->protocol) != 3) {
        return HTTP_PARSE_INVALID;
    }

    request->method_e = parse_http_method_e(request->method);

    if (parse_request_target(request->target, HTTP_REQUEST_TARGET_MAX_LEN, request) != HTTP_PARSE_OK) {
        return HTTP_PARSE_INVALID;
    };

    if (parse_query_string(request) != HTTP_PARSE_OK) {
        return HTTP_PARSE_INVALID;
    }

    if (parse_request_body(request->buffer, bytes_read, &request->body) != HTTP_PARSE_OK) {
        return HTTP_PARSE_INVALID;
    }

    return HTTP_PARSE_OK;
}

http_parse_e parse_request_headers(const char* raw_request, http_request* request)
{
    char* line_start = strstr(raw_request, "\r\n");
    if (!line_start)
        return HTTP_PARSE_INVALID;

    line_start += 2;

    while (line_start && *line_start && *line_start != '\r' && *line_start != '\n') {
        const char* line_end = strstr(line_start, "\r\n");
        if (!line_end)
            return HTTP_PARSE_INVALID;

        size_t line_length = line_end - line_start;
        char line[1024] = { 0 };
        strncpy(line, line_start, line_length);

        char* colon_pos = strchr(line, ':');
        if (colon_pos) {
            *colon_pos = '\0';
            const char* key = line;
            const char* value = colon_pos + 1;

            while (*value == ' ')
                value++;

            request->headers = realloc(request->headers, sizeof(http_header_t) * (request->headers_count + 1));
            if (!request->headers) {
                perror("Failed to allocate memory for headers");
                exit(EXIT_FAILURE);
            }

            strncpy(request->headers[request->headers_count].key, key, sizeof(request->headers[request->headers_count].key) - 1);
            strncpy(request->headers[request->headers_count].value, value, sizeof(request->headers[request->headers_count].value) - 1);

            request->headers_count++;
        }

        line_start = (char*)line_end + 2;
    }

    return HTTP_PARSE_OK;
}

void free_request_headers(http_request* request)
{
    free(request->headers);
    request->headers = NULL;
    request->headers_count = 0;
}

void add_respose_header(http_response* response, const char* key, const char* value)
{
    response->headers = realloc(response->headers, sizeof(http_header_t) * (response->header_count + 1));
    if (!response->headers) {
        perror("Failed to allocate memory for headers");
        exit(EXIT_FAILURE);
    }

    strncpy(response->headers[response->header_count].key, key, sizeof(response->headers[response->header_count].key) - 1);
    strncpy(response->headers[response->header_count].value, value, sizeof(response->headers[response->header_count].value) - 1);
    response->header_count++;
}

void init_http_response(http_response* response)
{
    response->status_code = 200; // OK
    strncpy(response->reason_phrase, "OK", sizeof(response->reason_phrase) - 1);
    response->headers = NULL;
    response->header_count = 0;
    response->body = NULL;
    response->body_length = 0;
}

void free_http_response(http_response* response)
{
    free(response->headers);
    response->headers = NULL;
    response->header_count = 0;
    free(response->body);
    response->body = NULL;
    response->body_length = 0;
}

char* construct_http_response(const http_response* response, size_t* response_length)
{
    size_t buffer_size = 1024;
    char* buffer = malloc(buffer_size);

    if (!buffer) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // snprintf will write to the buffer the buffer_size and
    // print the HTTP/1.1 200 OK content
    // following the format: HTTP/1.1 %d %s\r\n
    size_t offset = snprintf(buffer, buffer_size, "HTTP/1.1 %d %s\r\n", response->status_code, response->reason_phrase);

    for (size_t i = 0; i < response->header_count; i++) {
        // get the length of the formatted header key: value with the \r\n
        size_t header_len = snprintf(NULL, 0, "%s: %s\r\n", response->headers[i].key, response->headers[i].value);

        while (offset + header_len + 1 > buffer_size) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                puts("Failed to reallocate memory for the response header");
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        // write the header to the buffer
        offset += snprintf(
            buffer + offset,
            buffer_size - offset,
            "%s: %s\r\n",
            response->headers[i].key, response->headers[i].value);
    }

    offset += snprintf(buffer + offset, buffer_size - offset, "\r\n");

    if (response->body) {
        while (offset + response->body_length + 1 > buffer_size) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                puts("Failed to reallocate memory for the response body");
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        memcpy(buffer + offset, response->body, response->body_length + 1);
        offset += response->body_length;
    }

    *response_length = offset;
    return buffer;
}

void send_http_response(int client_fd, const http_response* response)
{
    size_t response_length = 0;
    char* response_data = construct_http_response(response, &response_length);

    size_t total_sent = 0;
    while (total_sent < response_length) {
        ssize_t bytes_sent = send(client_fd, response_data + total_sent, response_length - total_sent, 0);
        if (bytes_sent <= 0) {
            puts("Failed to send response");
            perror("send");
            break;
        }
        total_sent += bytes_sent;
    }

    free(response_data);
}

void set_response_body(http_response* response, const char* content)
{
    if (response == NULL)
        return;

    response->body_length = strlen(content); // null terminator \0?
    response->body = malloc(response->body_length);
    strncpy(response->body, content, response->body_length);
}

sanitize_result_e sanitize_path(const char* root, const char* requested_path, char* sanitized_path, size_t buffer_size)
{
    snprintf(sanitized_path, buffer_size, "%s%s", root, requested_path);

    if (strstr(sanitized_path, "..")) {
        // attempt to access ../ folder on the server
        return SANITIZE_ERROR;
    }

    return SANITIZE_OK;
}

bool serve_file(const char* path, http_response* response)
{
    FILE* file = fopen(path, "rb+");
    if (!file) {
        return false;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // allocate memory to serve the file
    response->body = malloc(file_size + 1);
    if (!response->body) {
        perror("Failed to allocate memory for file content");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fread(response->body, 1, file_size, file);
    fclose(file);
    response->body[file_size] = '\0';
    response->body_length = file_size;

    if (strstr(path, ".html")) {
        add_respose_header(response, "Content-Type", "text/html");
    } else if (strstr(path, ".css")) {
        add_respose_header(response, "Content-Type", "text/css");
    } else if (strstr(path, ".js")) {
        add_respose_header(response, "Content-Type", "application/javascrispt");
    } else if (strstr(path, ".png")) {
        add_respose_header(response, "Content-Type", "image/png");
    } else {
        add_respose_header(response, "Content-Type", "application/octet-stream");
    }

    // adding content to the response
    char content_length[32] = { 0 };
    snprintf(content_length, sizeof(content_length), "%zu", file_size);
    add_respose_header(response, "Content-Length", content_length);

    return true;
}

http_method_e parse_http_method_e(char* method)
{
    if (!method)
        return -1;

    for (int i = 0; i < HTTP_METHODS_COUNT; i++) {
        if (strcmp(HTTP_METHODS_DICT[i].method, method) == 0) {
            return HTTP_METHODS_DICT[i].method_e;
        }
    }

    return -1;
}

http_parse_e parse_request_target(const char* request_target, size_t size, http_request* request)
{
    if (!request_target)
        return HTTP_PARSE_INVALID;

    if (!request)
        return HTTP_PARSE_INVALID;

    if (size > HTTP_REQUEST_TARGET_MAX_LEN)
        return HTTP_PARSE_INVALID;

    char* source = calloc(size, sizeof(char));
    if (!source) {
        puts("Failed to allocate memory when parsing request target");
        exit(EXIT_FAILURE);
    }

    strncpy(source, request_target, size);

    char* path = strtok(source, "?");
    char* query = strtok(NULL, "?");

    strncpy(request->path, path, HTTP_PATH_MAX_LEN);

    if (query) {
        strncpy(request->query, query, HTTP_QUERY_MAX_LEN);
    } else {
        strcpy(request->query, "");
    }

    free(source);
    source = NULL;
    path = NULL;
    query = NULL;

    return HTTP_PARSE_OK;
}

http_parse_e parse_query_string(http_request* request)
{
    if (!request)
        return HTTP_PARSE_INVALID;

    request->query_string = qs_init();

    char* source = calloc(strlen(request->query), HTTP_QUERY_MAX_LEN);
    strncpy(source, request->query, strlen(request->query));

    char *keyvalue = NULL, *key = NULL, *value = NULL;
    char* separator = NULL;
    int key_size = 0;
    int value_size = 0;

    for (keyvalue = strtok(source, "&"); keyvalue != NULL; keyvalue = strtok(NULL, "&")) {
        separator = strchr(keyvalue, '=');
        if (separator) {
            key_size = separator - keyvalue;
            value_size = strlen(separator) - 1;
        } else {
            key_size = strlen(keyvalue);
            value_size = 0;
        }

        key = calloc(key_size, sizeof(char));
        value = calloc(value_size, sizeof(char));

        memcpy(key, keyvalue, key_size);
        if (value_size > 0)
            memcpy(value, separator + 1, value_size);
        else
            memset(value, 0, value_size);

        qs_add(request->query_string, key, value);

        free(key);
        free(value);
        key = NULL;
        value = NULL;
    }

    return HTTP_PARSE_OK;
}

http_parse_e parse_request_body(
    const char* raw_request,
    size_t request_length,
    http_request_body* body)
{
    if (!raw_request || !*raw_request) {
        return HTTP_PARSE_INVALID;
    }

    char* headers = memmem(raw_request, request_length, "\r\n\r\n", 4);
    if (!headers) {
        body->length = 0;
        body->content = NULL;

        return HTTP_PARSE_INVALID;
    }

    const char* start = headers + 4;
    size_t header_len = start - raw_request;
    size_t body_len = request_length - header_len;

    body->content = start;
    body->length = body_len;

    return HTTP_PARSE_OK;
}
