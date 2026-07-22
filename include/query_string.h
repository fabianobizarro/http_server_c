#ifndef QUERY_STRING_H
#define QUERY_STRING_H

#include "dynamic_array.h"
#include <stdlib.h>

typedef enum {
    QS_OK,
    QS_KEY_NOT_FOUND,
} qs_result;

typedef struct {
    const char* key;
    DynamicArray* arrayvalues;
} QSNode;

typedef struct {
    QSNode** nodes;
    size_t size;
} QueryString;

QueryString* qs_init();
void qs_free(QueryString* qs);

QSNode* init_array_node(const char* key);

void qs_add(QueryString* qs, const char* key, const char* value);

qs_result qs_get(QueryString* qs, const char* key, QSNode* value);

#endif
