#include <http.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    http_response response;
    init_http_response(&response);

    printf("Initialized HTTP Response:\n");
    printf("Status Code: %d\n", response.status_code);
    printf("Reason Phrase: %s\n", response.reason_phrase);

    add_http_header(&response, "Content-Type", "text/json");
    add_http_header(&response, "Token", "abc123");
    add_http_header(&response, "TTL", "3600");

    printf("Headers:\n");
    for (size_t i = 0; i < response.header_count; i++) {
        printf("%s: %s\n", response.headers[i].key, response.headers[i].value);
    }

    return 0;
}
