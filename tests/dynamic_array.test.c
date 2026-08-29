#include "dynamic_array.h"
#include <assert.h>
#include <stdio.h>

void print_dynamic_array(const DynamicArray* array)
{
    puts("State");
    printf("Count: %lu\n", array->count);
    printf("Capacity: %lu\n", array->capacity);
}

void append(DynamicArray* a, const char* v)
{
    printf("[Array: count %lu cap %lu]Appending %s...\n", a->count, a->capacity, v);
    da_append(a, "Fabiano");
}

void default_test(void)
{
    DynamicArray names = { 0 };

    puts("Default test - It should behave as expected");

    print_dynamic_array(&names);

    append(&names, "Fabiano");
    append(&names, "Augusto");
    append(&names, "Silva");
    append(&names, "Bizarro");

    append(&names, "Mayara");
    append(&names, "Fernandes");
    append(&names, "Souza");

    print_dynamic_array(&names);

    assert(names.capacity > 0);
    assert(names.count == 7);
    assert(names.values != NULL);

    puts("OK");
}

void init_test(void)
{
    DynamicArray* arr = NULL;

    puts("Init test - It should initialize the pointer as expected");

    arr = da_init();

    assert(arr != NULL);
    assert(arr->capacity == 0);
    assert(arr->count == 0);
    assert(arr->values == NULL);

    puts("OK");
    free_da(arr);
}

int main()
{
    init_test();
    default_test();

    return 0;
}
