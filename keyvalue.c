#include "keyvalue.h"
#include <stdlib.h>
#include <string.h>

struct store* store_init()
{
    struct store* kv = (struct store*)calloc(1, sizeof(struct store));
    kv->store_size = DEFAULT_SIZE;
    kv->load_factor = DEFAULT_LOAD_FACTOR;
    kv->buckets = (struct value*)calloc(DEFAULT_SIZE, sizeof(struct value));
    return kv;
}
// Djb2 hash function
unsigned long _hash(char* key)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

// Assumed that value and key will not be freed by anybody else
int store_insert(struct store* kv, char* key, char* value)
{
    if (kv->count * 100 / kv->store_size >= kv->load_factor) {
        _store_resize(kv);
    }
    unsigned long hsh = _hash(key) % kv->store_size;
    struct value* bucket = &kv->buckets[hsh];
    if (bucket->key != NULL) {
        while (bucket->next != NULL) {
            bucket = bucket->next;
        }
    }

    struct value* new = (struct value*)calloc(1, sizeof(struct value));
    new->key = key;
    new->value = value;
    new->next = NULL;
    bucket->next = new;
    kv->count++;
    return 0;
}

int _store_resize(struct store* old_kv)
{
    struct store* new_kv = store_init();
    new_kv->buckets = (struct value*)calloc(old_kv->store_size * 2, sizeof(struct value));
    new_kv->store_size *= 2;
    for (size_t i = 0; i < old_kv->store_size / 2; i++) {
        struct value* current = &old_kv->buckets[i];
        while (current->value != NULL) {
            store_insert(new_kv, current->key, current->value);
            current = current->next;
        }
    }
    store_destroy(old_kv);
    old_kv = new_kv;
    return 0;
}

int store_remove(struct store* kv, char* key)
{
    unsigned long hsh = _hash(key) % kv->store_size;
    struct value* current = &kv->buckets[hsh];
    struct value* last = current;
    while (current && !(memcmp(current->key, key, DEFAULT_MEM_LIMIT) == 0)) {
        last = current;
        current = current->next;
    }
    if (current == NULL)
        return INSERTERR;
    free(current->key);
    free(current->value);
    if (current == last) {
        current->key = NULL;
        current->value = NULL;
        kv->buckets[hsh].next = current->next;
    } else {
        last->next = current->next;
        free(current);
    }
    kv->count--;
    return 0;
}
struct value* store_get(struct store* kv, char* key)
{
    unsigned long hsh = _hash(key) % kv->store_size;
    struct value* current = &kv->buckets[hsh];
    while (current && !memcmp(current->key, key, DEFAULT_MEM_LIMIT))
        current = current->next;
    if (current == NULL)
        return NULL;
    else
        return current;
}
int store_destroy(struct store* kv)
{
    struct value* tmp;
    for (size_t i = 0; i < kv->count; i++) {
        if (kv->buckets[i].value != NULL) {
            free(kv->buckets[i].value);
        }
        struct value* next = kv->buckets[i].next;
        while (next != NULL) {
            tmp = next;
            free(next->value);
            next = next->next;
            free(tmp);
        }
    }
    free(kv->buckets);
    free(kv);
    return 1;
}