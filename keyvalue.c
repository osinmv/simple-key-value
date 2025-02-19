#include "keyvalue.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int _store_init(struct store* kv, int size)
{
    kv->store_size = size;
    kv->load_factor = DEFAULT_LOAD_FACTOR;
    kv->buckets = (struct bucket*)calloc(size, sizeof(struct bucket));
    if (kv->buckets == NULL) {
        return MEMERR;
    }
    kv->count = 0;
    return OK;
}
struct store* store_init()
{
    struct store* kv = (struct store*)calloc(1, sizeof(struct store));
    if (kv == NULL || _store_init(kv, DEFAULT_SIZE) != OK)
        return NULL;
    return kv;
}
// Djb2 hash function
unsigned long _hash(struct container* key)
{
    unsigned long hash = 5381;
    int c;
    char* bytes = (char*)key->data;
    for (size_t i = 0; i < key->size; i++) {
        c = bytes[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

// Assumed that bucket and key will not be freed by anybody else
int store_insert(struct store* kv, struct container* key, struct container* value)
{
    if (kv->count * 100 / kv->store_size >= kv->load_factor) {
        if (_store_resize(kv) != OK)
            return RESIZEERR;
    }
    unsigned long hsh = _hash(key) % kv->store_size;
    struct bucket* bucket = &kv->buckets[hsh];
    struct linked_keyvalue* node = (struct linked_keyvalue*)calloc(1, sizeof(struct linked_keyvalue));

    node->next = bucket->root;
    bucket->root = node;

    node->key = (struct container*)calloc(1, sizeof(struct container));
    node->value = (struct container*)calloc(1, sizeof(struct container));
    node->key->data = calloc(1, key->size);
    node->value->data = calloc(1, value->size);
    node->key->size = key->size;
    node->value->size = value->size;
    memcpy(node->key->data, key->data, key->size);
    memcpy(node->value->data, value->data, value->size);

    kv->count++;

    return OK;
}
void _store_free_linked_keyvalue(struct linked_keyvalue* node)
{
    struct linked_keyvalue* last_node = node;
    while (node != NULL) {
        free(node->key->data);
        free(node->value->data);
        free(node->key);
        free(node->value);
        node = node->next;
        free(last_node);
        last_node = node;
    }
}
int _store_resize(struct store* kv)
{
    struct bucket* buckets = kv->buckets;
    int size = kv->store_size;
    if (_store_init(kv, kv->store_size * 2) != OK) {
        kv->buckets = buckets;
        fprintf(stderr, "COULDN'T RESIZE, NOT ENOUGH MEMORY");
        return RESIZEERR;
    }
    for (size_t i = 0; i < size; i++) {
        struct linked_keyvalue* node = buckets[i].root;
        while (node != NULL) {
            store_insert(kv, node->key, node->value);
            node = node->next;
        }
        _store_free_linked_keyvalue(buckets[i].root);
    }
    free(buckets);
    return OK;
}
int store_remove(struct store* kv, struct container* key)
{
    unsigned long hsh = _hash(key) % kv->store_size;
    struct linked_keyvalue* node = kv->buckets[hsh].root;
    struct linked_keyvalue** last_node_address = &kv->buckets[hsh].root;
    while (node != NULL) {
        if (key->size == node->key->size && memcmp(node->key->data, key->data, key->size) == 0) {
            *last_node_address = node->next;
            node->next = NULL;
            _store_free_linked_keyvalue(node);
            kv->count--;
            return OK;
        }
        last_node_address = &node->next;
        node = node->next;
    }
    return DELETEERR;
}

struct linked_keyvalue* store_get(struct store* kv, struct container* key)
{
    unsigned long hsh = _hash(key) % kv->store_size;
    struct linked_keyvalue* node = kv->buckets[hsh].root;
    while (node != NULL) {
        if (key->size == node->key->size && memcmp(node->key->data, key->data, key->size) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}
int store_destroy(struct store* kv)
{
    struct bucket* tmp;
    struct bucket* next;
    for (size_t i = 0; i < kv->store_size; i++) {
        if (kv->buckets[i].root != NULL)
            _store_free_linked_keyvalue(kv->buckets[i].root);
    }
    free(kv->buckets);
    free(kv);
    return OK;
}
