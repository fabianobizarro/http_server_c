#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdlib.h>

const static short DA_INITIAL_CAPACITY = 20;

typedef struct {
    const char** values;
    size_t count;
    size_t capacity;
} DynamicArray;

DynamicArray* da_init(void);

void da_append(DynamicArray* array, const char* value);

void free_da(DynamicArray* a);

#endif
