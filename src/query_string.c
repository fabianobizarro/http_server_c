#include "query_string.h"
#include "dynamic_array.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

QueryString* qs_init()
{
    QueryString* p = malloc(sizeof(QueryString));

    p->size = 0;
    p->nodes = NULL;

    return p;
}

void qs_free(QueryString* qs)
{
    free(qs->nodes);
    qs->nodes = NULL;
}

long _find_index(QueryString* qs, const char* key)
{
    for (size_t i = 0; i < qs->size; i++) {
        if (strcmp(qs->nodes[i]->key, key) == 0) {
            return i;
        }
    }

    return -1;
}

QSNode* init_array_node(const char* key)
{
    QSNode* p = malloc(sizeof(QSNode));
    memset(p, 0, sizeof(QSNode));

    // p->key = key;
    // init key pointer
    unsigned long s = strlen(key);
    p->key = malloc(strlen(key) * sizeof(char));
    strncpy((char*)p->key, key, strlen(key));
    //

    p->arrayvalues = malloc(sizeof(DynamicArray));
    memset(p->arrayvalues, 0, sizeof(DynamicArray));

    return p;
}

void qs_add(QueryString* qs, const char* key, const char* value)
{
    if (!qs)
        return;

    long index = _find_index(qs, key);

    if (index == -1) {
        qs->nodes = realloc(qs->nodes, sizeof(qs->nodes) * qs->size + 1);

        qs->nodes[qs->size] = init_array_node(key);

        da_append(qs->nodes[qs->size]->arrayvalues, value);
        qs->size++;
    } else {
        da_append(qs->nodes[index]->arrayvalues, value);
    }
}

qs_result qs_get(QueryString* qs, const char* key, QSNode* node)
{
    long index = _find_index(qs, key);

    if (index == -1) {
        node = NULL;
        return QS_KEY_NOT_FOUND;
    }

    *node = *qs->nodes[index];

    return QS_OK;
}
