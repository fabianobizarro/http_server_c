#include "http.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    printf("Test 6 - %s\n", target);

    http_request req = { 0 };

    http_parse_e result = parse_request_target(target, HTTP_REQUEST_TARGET_MAX_LEN, &req);

    printf("Path: %s\n", req.path);
    printf("Query: %s\n", req.query);

    assert(result == HTTP_PARSE_OK);
    assert(strcmp(req.path, "/") == 0);
    assert(strncmp(req.query, expected_query_array, HTTP_QUERY_MAX_LEN) == 0);

    printf("OK\n\n");
}

int main()
{
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    test_6();
    test_7();

    return 0;
}
