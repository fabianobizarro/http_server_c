#include "query_string.h"
#include "dynamic_array.h"
#include <assert.h>
#include <stdio.h>
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

void initial_test()
{
    puts("Initial Test - Adding values to the structure");
    QueryString qs = { 0 };

    QSNode* qsnode;

    qs_add(&qs, "name", "fabiano");
    qs_add(&qs, "name", "augusto");
    qs_add(&qs, "name", "silva");
    qs_add(&qs, "name", "bizarro");
    qs_add(&qs, "phone", "31999043366");
    qs_add(&qs, "age", "32");

    print_nodes(&qs);

    printf("OK\n\n");
}

void instance_test()
{
    QueryString* qs = qs_init();
    QSNode** n = qs->nodes;

    assert(qs != NULL);
    assert(qs->size == 0);

    qs_add(qs, "name", "johan");
    qs_add(qs, "age", "99");

    assert(qs->size == 2);
    assert(qs->nodes != NULL);

    qs_free(qs);
}

void get_test()
{
    QueryString qs = { 0 };
    QSNode node = { 0 };
    qs_result get_result;

    qs_add(&qs, "name", "fabiano");
    qs_add(&qs, "name", "bizarro");
    qs_add(&qs, "age", "99");

    print_nodes(&qs);

    printf("Getting key `name`... ");
    assert(qs_get(&qs, "name", &node) == QS_OK);
    assert(node.key != NULL);
    assert(node.arrayvalues != NULL);
    assert(strcmp(node.key, "name") == 0);
    assert(node.arrayvalues->count == 2);
    printf("OK\n");

    printf("Getting key `age`... ");
    assert(qs_get(&qs, "age", &node) == QS_OK);
    assert(node.key != NULL);
    assert(node.arrayvalues != NULL);
    assert(strcmp(node.key, "age") == 0);
    assert(node.arrayvalues->count == 1);
    printf("OK\n");

    printf("Getting key `test` - it should return QS_KEY_NOT_FOUND... ");
    assert(qs_get(&qs, "test", &node) == QS_KEY_NOT_FOUND);

    printf("OK\n");

    qs_free(&qs);
}

int main()
{
    initial_test();
    instance_test();
    get_test();

    return 0;
}
