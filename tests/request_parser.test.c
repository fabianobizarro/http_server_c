#include "http.h"
#include "query_string.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_values(DynamicArray* array)
{
    for (size_t i = 0; i < array->count; i++) {
        printf("%s - ", array->values[i]);
    }
}

void print_nodes(QueryString* qs)
{
    puts("QueryString key/values:");
    for (size_t i = 0; i < qs->size; i++) {
        printf("[%s] => ", qs->nodes[i]->key);
        print_values(qs->nodes[i]->arrayvalues);
        printf(" \n");
    }
}

void test_1(void)
{
    const char* target = "/path?a=1";
    printf("Test 1 - %s\n", target);

    http_request req = { 0 };

    http_parse_e result = parse_request_target(target, HTTP_REQUEST_TARGET_MAX_LEN, &req);
    // http_parse_e result = parse_request_target(target, strlen(target), &req);

    printf("Path: %s\n", req.path);
    printf("Query: %s\n", req.query);

    assert(result == HTTP_PARSE_OK);
    assert(strcmp(req.path, "/path") == 0);
    assert(strcmp(req.query, "a=1") == 0);
    printf("OK\n\n");
}

void test_2(void)
{
    const char* target = "/path?a=1&b=2";
    printf("Test 2 - %s\n", target);

    http_request req = { 0 };

    http_parse_e result = parse_request_target(target, HTTP_REQUEST_TARGET_MAX_LEN, &req);

    printf("Path: %s\n", req.path);
    printf("Query: %s\n", req.query);

    assert(result == HTTP_PARSE_OK);
    assert(strcmp(req.path, "/path") == 0);
    assert(strcmp(req.query, "a=1&b=2") == 0);

    printf("OK\n\n");
}

void test_3()
{
    const char* target = "/path?x=";
    printf("Test 3 - %s\n", target);

    http_request req = { 0 };

    http_parse_e result = parse_request_target(target, HTTP_REQUEST_TARGET_MAX_LEN, &req);

    printf("Path: %s\n", req.path);
    printf("Query: %s\n", req.query);

    assert(result == HTTP_PARSE_OK);
    assert(strcmp(req.path, "/path") == 0);
    assert(strcmp(req.query, "x=") == 0);

    printf("OK\n\n");
}

void test_4()
{
    const char* target = "/path?flag";
    printf("Test 4 - %s\n", target);

    http_request req = { 0 };

    http_parse_e result = parse_request_target(target, HTTP_REQUEST_TARGET_MAX_LEN, &req);

    printf("Path: %s\n", req.path);
    printf("Query: %s\n", req.query);

    assert(result == HTTP_PARSE_OK);
    assert(strcmp(req.path, "/path") == 0);
    assert(strcmp(req.query, "flag") == 0);

    printf("OK\n\n");
}

void test_5()
{
    const char* target = "/path?";
    const char expected_query_array[HTTP_QUERY_MAX_LEN] = { 0 };
    printf("Test 5 - %s\n", target);

    http_request req = { 0 };

    http_parse_e result = parse_request_target(target, HTTP_REQUEST_TARGET_MAX_LEN, &req);

    printf("Path: %s\n", req.path);
    printf("Query: %s\n", req.query);

    assert(result == HTTP_PARSE_OK);
    assert(strcmp(req.path, "/path") == 0);
    assert(strncmp(req.query, expected_query_array, HTTP_QUERY_MAX_LEN) == 0);

    printf("OK\n\n");
}

void test_6()
{
    const char* target = "/path";
    const char expected_query_array[HTTP_QUERY_MAX_LEN] = { 0 };

    printf("Test 6 - %s\n", target);

    http_request req = { 0 };

    http_parse_e result = parse_request_target(target, HTTP_REQUEST_TARGET_MAX_LEN, &req);

    printf("Path: %s\n", req.path);
    printf("Query: %s\n", req.query);

    assert(result == HTTP_PARSE_OK);
    assert(strcmp(req.path, "/path") == 0);
    assert(strncmp(req.query, expected_query_array, HTTP_QUERY_MAX_LEN) == 0);

    printf("OK\n\n");
}

void test_7()
{
    const char* target = "/";
    const char expected_query_array[HTTP_QUERY_MAX_LEN] = { 0 };

    printf("Test 7 - %s\n", target);

    http_request req = { 0 };

    http_parse_e result = parse_request_target(target, HTTP_REQUEST_TARGET_MAX_LEN, &req);

    printf("Path: %s\n", req.path);
    printf("Query: %s\n", req.query);

    assert(result == HTTP_PARSE_OK);
    assert(strcmp(req.path, "/") == 0);
    assert(strncmp(req.query, expected_query_array, HTTP_QUERY_MAX_LEN) == 0);

    printf("OK\n\n");
}

void test_8()
{
    const char* target = "/path?name=fabiano&a=1&b=2&c=3&d=4&e=5&f=6&g=7&h=8&i=9&j=10&a=5";

    QueryString qs;
    printf("Test 8 - %s\n", target);
    http_request req = { 0 };

    parse_request_target(target, HTTP_REQUEST_TARGET_MAX_LEN, &req);

    printf("Querystring:\n\t%s\n", req.query);

    char* source = calloc(strlen(req.query), sizeof(char));
    strncpy(source, req.query, strlen(req.query));

    char* keyvalue = strtok(source, "&");

    while (keyvalue != NULL) {

        char *key = NULL, *value = NULL;
        char* sep = strchr(keyvalue, '=');
        int sep_pos = sep - keyvalue;

        key = calloc(sep_pos, sizeof(char));
        value = calloc(strlen(sep) - 1, sizeof(char));

        memcpy(key, keyvalue, sep_pos);
        memcpy(value, sep + 1, strlen(sep) - 1);

        qs_add(&qs, key, value);

        keyvalue = strtok(NULL, "&");
    }

    print_nodes(&qs);

    free(source);
    source = NULL;
}

int main()
{
    // test_1();
    // test_2();
    // test_3();
    // test_4();
    // test_5();
    // test_6();
    // test_7();
    test_8();

    return 0;
}
