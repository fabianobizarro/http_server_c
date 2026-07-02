# http_server_c

A minimal HTTP/1.1 server written in C using Berkeley sockets. It supports custom route handlers, static file serving, and basic request/response parsing.

## Features

- TCP socket server with `SO_REUSEADDR`
- HTTP/1.1 request parsing (method, path, protocol, headers)
- Custom route registration with per-route handlers
- Static file serving from a configurable web root
- Basic path sanitization (blocks `..` traversal)
- Dynamic response header and body construction

## Project Structure

```
.
├── include/          # Public headers
│   ├── http.h        # HTTP request/response types and functions
│   ├── server.h      # Server and route API
│   ├── tcp.h         # Low-level TCP socket helpers
│   └── main.h
├── src/              # Source files
│   ├── main.c        # Entry point and example routes
│   ├── server.c      # Server lifecycle and request dispatch
│   ├── http.c        # HTTP parsing and response building
│   └── tcp.c         # Socket bind/accept helpers
├── tests/            # Standalone test programs
├── www/              # Default static web root
│   ├── index.html
│   └── 404.html
├── Makefile
└── README.md
```

## Build

Requires a C compiler such as GCC and a POSIX environment.

```bash
make
```

The compiled binary is written to `bin/httpd`.

To clean build artifacts:

```bash
make clean
```

## Usage

Run the server:

```bash
./bin/httpd
```

By default it listens on port `8080` and serves files from `./www`.

Example requests:

```bash
# Custom route
curl http://localhost:8080/

# Static file
curl http://localhost:8080/index.html

# POST to custom route
curl -X POST http://localhost:8080/hello
```

## Adding Routes

Routes are registered in `src/main.c` with a method, path, and handler function:

```c
void my_handler(http_request* req, http_response* res) {
    res->status_code = 200;
    set_response_body(res, "Hello from my handler!");
    add_respose_header(res, "Content-Length", "22");
}

register_route(server, HTTP_METHOD_GET, "/my-route", &my_handler);
```

If no route matches, the server attempts to serve a matching file from the configured web root and falls back to a `404 Not Found` response.

## Configuration

The default port and web root are defined in `src/main.c`:

```c
#define PORT 8080
#define WEB_ROOT "./www"
```

Edit these values and rebuild to change the listening port or static content directory.

## Tests

Small standalone test programs are located in `tests/`. They are not currently integrated into the Makefile, so compile and run them manually, for example:

```bash
gcc -Iinclude -o tests/headers_test tests/headers_test.c src/http.c src/tcp.c src/server.c
./tests/headers_test
```

## Notes

- The server handles one connection at a time (single-threaded, sequential accept loop).
- Response buffers grow dynamically as headers and body are added.
- Static files are read fully into memory before being sent.
