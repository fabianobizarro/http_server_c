#include "dynamic_array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DynamicArray* da_init(void)
{
    DynamicArray* p = malloc(sizeof(DynamicArray));
    if (!p) {
        exit(EXIT_FAILURE);
    }

    p->count = 0;
    p->capacity = 0;
    p->values = NULL;

    return p;
}

void da_append(DynamicArray* array, const char* value)
{
    if (!array)
        return;

    char* dest;

    if (array->count >= array->capacity) {
        array->capacity = array->capacity == 0
            ? DA_INITIAL_CAPACITY
            : array->capacity * 2;

        array->values = realloc(
            array->values,
            array->capacity * sizeof(array->values));

        if (!array->values) {
            puts("Failed to allocate memory for Dynamic Array");
            exit(EXIT_FAILURE);
        }
    }
    dest = malloc(strlen(value) * sizeof(char));
    strncpy(dest, value, strlen(value));
    array->values[array->count++] = dest;
    dest = NULL;
}

void free_da(DynamicArray* a)
{
    if (a == NULL)
        return;

    free(a->values);
    a->values = NULL;
    a->capacity = 0;
    a->capacity = 0;
}
