#include "http.h"
#include <stdio.h>

int main()
{
    http_response response;
    const char* body = "<html><body><h1>Hello, world!</h1></body></html>";
    init_http_response(&response);

    puts("Test #1 - Setting response body from const char*");

    set_response_body(&response, body);
    printf("Setting value %s to the response body.\n", body);

    printf("Response body value: %s\n", response.body);
    printf("Response body length: %lu\n", response.body_length);

    puts("Test #2 - Setting response body from string literal");

    set_response_body(&response, "<p>Content from string literal</p>");
    printf("Setting value %s to the response body.\n", "<p>Content from string literal</p>");

    printf("Response body value: %s\n", response.body);
    printf("Response body length: %lu\n", response.body_length);

    return 0;
}
