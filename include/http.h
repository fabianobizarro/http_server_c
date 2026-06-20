#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>
#include <stdbool.h>

#define HTTP_MAX_REQUEST_LEN 8192 * 4
#define HTTP_METHOD_MAX_LEN 8
#define HTTP_PATH_MAX_LEN 2048
#define HTTP_PROTOCOL_MAX_LEN 16

#define HTTP_MAX_HEADER_KEY_LEN 256
#define HTTP_MAX_HEADER_VALUE_LEN 256

typedef enum {
    HTTP_PARSE_OK,
    HTTP_PARSE_INVALID
} http_parse_e;

typedef enum {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_OPTIONS,
    HTTP_METHOD_TRACE,
    HTTP_METHOD_CONNECT,
    HTTP_METHOD_PATCH,
    HTTP_METHOD_UNKNOWN
} http_method_e;

typedef struct {
    char key[HTTP_MAX_HEADER_KEY_LEN];
    char value[HTTP_MAX_HEADER_VALUE_LEN];
} http_header_t;

typedef struct {
    char method[HTTP_METHOD_MAX_LEN];
    http_method_e method_e;
    char path[HTTP_PATH_MAX_LEN];
    char protocol[HTTP_PROTOCOL_MAX_LEN];

    http_header_t* headers;
    size_t headers_count;
    char buffer[HTTP_MAX_REQUEST_LEN];
} http_request;

typedef struct {
    int status_code;
    char reason_phrase[64];
    http_header_t* headers;
    size_t header_count;
    char* body;
    size_t body_length;
} http_response;

http_parse_e read_http_request(int socket_fd, http_request* request);

http_parse_e parse_http_headers(const char* raw_request, http_request* request);
void free_http_headers(http_request* request);
void add_http_header(http_response* response, const char* key, const char* value);

void free_http_response(http_response* response);
void init_http_response(http_response* response);

char* construct_http_response(const http_response* response, size_t* response_length);
void set_response_body(http_response* response, const char* content);
void send_http_response(int client_fd, const http_response* response);

void serve_file(const char* path, http_response* response);
void sanitize_path(const char* requested_path, char* sanitized_path, size_t buffer_size);

bool handle_request(http_request* request, http_response* response);

#endif
