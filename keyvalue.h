#ifndef KEYVALUELIB_H
#define KEYVALUELIB_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DEFAULT_SIZE 16
#define DEFAULT_LOAD_FACTOR 70
#define MAX_STR_LEN 64

struct bucket {
    struct linked_keyvalue* root;
};

struct linked_keyvalue {
    // data part of key is expected to be ascii string
    struct container* key;
    struct container* value;
    struct linked_keyvalue* next;
};

struct keyvalue {
    char* key;
    void* value;
    int value_size;
};

struct container {
    int size;
    void* data;
};

struct store {
    struct bucket* buckets;
    unsigned long count;
    unsigned int store_size;
    int load_factor;
};

enum STORE_ERROR {
    OK = 1,
    MEMERR = -1,
    RESIZEERR = -2,
    INSERTERR = -3,
    DELETEERR = -4,
    UPDATEERR = -5,
    KEYERR = -6,
};

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

void free_container(struct container* cont)
{
    if (cont) {
        free(cont->data);
        free(cont);
    }
}
// Djb2 hash function
unsigned long _hash(char* key, int len)
{
    unsigned long hash = 5381;
    int c;
    char* bytes = key;
    for (size_t i = 0; i < len; i++) {
        c = bytes[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

int _store_resize(struct store* kv);
struct container* store_get(struct store* kv, char* key);

int _store_insert(struct store* kv, struct container* key, struct container* value)
{
    if (kv->count * 100 / kv->store_size >= kv->load_factor) {
        if (_store_resize(kv) != OK)
            return RESIZEERR;
    }
    struct container* duplicate = store_get(kv, key->data);
    if (duplicate != NULL) {
        return INSERTERR;
    }
    unsigned long hsh = _hash(key->data, key->size) % kv->store_size;
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
int _get_keylen(char* key)
{
    int key_size = strnlen(key, MAX_STR_LEN);
    if (key_size == MAX_STR_LEN)
        return KEYERR;
    return key_size + 1;
}

int store_insert(struct store* kv, char* key, void* value, int value_size)
{
    int key_size = _get_keylen(key);
    if (key_size == KEYERR)
        return INSERTERR;
    struct container key_container = { .data = key, .size = key_size };
    struct container value_container = { .data = value, .size = value_size };
    return _store_insert(kv, &key_container, &value_container);
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
            _store_insert(kv, node->key, node->value);
            node = node->next;
        }
        _store_free_linked_keyvalue(buckets[i].root);
    }
    free(buckets);
    return OK;
}

int store_remove(struct store* kv, char* key)
{
    int key_size = _get_keylen(key);
    if (key_size == KEYERR)
        return DELETEERR;
    unsigned long hsh = _hash(key, key_size) % kv->store_size;
    struct linked_keyvalue* node = kv->buckets[hsh].root;
    struct linked_keyvalue** last_node_address = &kv->buckets[hsh].root;
    while (node != NULL) {
        if (key_size == node->key->size && memcmp(node->key->data, key, key_size) == 0) {
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

struct container* store_get(struct store* kv, char* key)
{

    int key_size = _get_keylen(key);
    if (key_size == KEYERR)
        return NULL;
    unsigned long hsh = _hash(key, key_size) % kv->store_size;
    struct linked_keyvalue* node = kv->buckets[hsh].root;
    while (node != NULL) {
        if (key_size == node->key->size && memcmp(node->key->data, key, key_size) == 0) {
            struct container* result = (struct container*)malloc(sizeof(struct container));
            result->data = malloc(node->value->size);
            memcpy(result->data, node->value->data, node->value->size);
            result->size = node->value->size;
            return result;
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
#endif
