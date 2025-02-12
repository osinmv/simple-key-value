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
    for (size_t i = 0; i < key->size; i++) {
        c = key->data[i];
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
    struct bucket* new;
    if (bucket->key != NULL) {
        while (bucket->next != NULL) {
            bucket = bucket->next;
        }
        new = (struct bucket*)calloc(1, sizeof(struct bucket));
        bucket->next = new;
    } else {
        new = &kv->buckets[hsh];
    }
    new->key = (struct container*)calloc(1, sizeof(struct container));
    new->value = (struct container*)calloc(1, sizeof(struct container));
    new->key->data = (char*)calloc(1, key->size);
    new->value->data = (char*)calloc(1, value->size);
    new->key->size = key->size;
    new->value->size = value->size;
    memcpy(new->key->data, key->data, key->size);
    memcpy(new->value->data, value->data, value->size);
    kv->count++;
    return OK;
}
void _store_free_bucket(struct bucket* bucket, bool free_self)
{
    free(bucket->key->data);
    free(bucket->value->data);
    free(bucket->key);
    free(bucket->value);
    if (free_self)
        free(bucket);
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
        struct bucket* current = &buckets[i];
        struct bucket* last = current;
        bool is_first = true;
        while (current != NULL && current->value != NULL) {
            last = current;
            store_insert(kv, current->key, current->value);
            current = current->next;
            _store_free_bucket(last, !is_first);
            is_first = false;
        }
    }
    free(buckets);
    return OK;
}
int store_remove(struct store* kv, struct container* key)
{
    unsigned long hsh = _hash(key) % kv->store_size;
    struct bucket* current = &kv->buckets[hsh];
    struct bucket* last = current;
    while (current != NULL && current->key != NULL) {
        if (key->size == current->key->size && memcmp(current->key->data, key->data, key->size) == 0) {
            _store_free_bucket(current, false);
            // we treat first item from linkedlist differently, cause we can't really free it
            if (current == last) {
                current->key = NULL;
                current->value = NULL;
                kv->buckets[hsh].next = current->next;
            } else {
                last->next = current->next;
                free(current);
            }
            kv->count--;
            return OK;
        }

        last = current;
        current = current->next;
    }
    return DELETEERR;
}

struct bucket* store_get(struct store* kv, struct container* key)
{
    unsigned long hsh = _hash(key) % kv->store_size;
    struct bucket* current = &kv->buckets[hsh];
    while (current) {
        if (key->size == current->key->size && memcmp(current->key->data, key->data, key->size) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
int store_destroy(struct store* kv)
{
    struct bucket* tmp;
    struct bucket* next;
    for (size_t i = 0; i < kv->store_size; i++) {
        if (kv->buckets[i].value != NULL)
            _store_free_bucket(&kv->buckets[i], false);
        next = kv->buckets[i].next;
        while (next != NULL) {
            tmp = next;
            next = next->next;
            _store_free_bucket(tmp, true);
        }
    }
    free(kv->buckets);
    free(kv);
    return OK;
}
