#include "keyvalue.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct test {
    bool (*run_test)(void);
    char description[36];
};

bool test_insert()
{
    struct store* kv = store_init();
    struct container key, value;
    key.data = (char*)calloc(1, sizeof(char) * 10);
    key.size = 10;
    value.data = (char*)calloc(1, sizeof(char) * 10);
    value.size = 10;
    strncpy(key.data, "super key", 10);
    strncpy(value.data, "supervalue", 10);
    int result = store_insert(kv, &key, &value);
    store_destroy(kv);
    free(key.data);
    free(value.data);
    return result == OK;
}
bool test_get()
{
    struct store* kv = store_init();
    struct container key, value;
    key.data = (char*)calloc(1, sizeof(char) * 10);
    key.size = 10;
    value.data = (char*)calloc(1, sizeof(char) * 10);
    value.size = 10;
    strncpy(key.data, "super key", 10);
    strncpy(value.data, "supervalue", 10);
    store_insert(kv, &key, &value);
    struct linked_keyvalue* result = store_get(kv, &key);
    bool ret = result != NULL && strncmp(value.data, result->value->data, value.size) == 0;
    store_destroy(kv);
    free(key.data);
    free(value.data);

    return ret;
}

bool test_remove()
{
    struct store* kv = store_init();
    struct container key, value;
    key.data = (char*)calloc(1, sizeof(char) * 10);
    key.size = 10;
    value.data = (char*)calloc(1, sizeof(char) * 10);
    value.size = 10;
    strncpy(key.data, "super key", 10);
    strncpy(value.data, "supervalue", 10);
    store_insert(kv, &key, &value);
    int result = store_remove(kv, &key);
    store_destroy(kv);
    free(key.data);
    free(value.data);

    return result == OK;
}

bool test_remove_nonexistant()
{
    struct store* kv = store_init();
    struct container key;
    key.data = (char*)calloc(1, sizeof(char) * 10);
    strncpy(key.data, "super key", 10);
    key.size = 10;
    int ret = store_remove(kv, &key);
    store_destroy(kv);
    free(key.data);

    return ret == DELETEERR;
}

bool test_destroy_empty_store()
{
    struct store* kv = store_init();
    store_destroy(kv);
    return true;
}
bool test_resize()
{
    struct store* kv = store_init();
    struct container key, value;
    key.data = calloc(1, sizeof(int));
    key.size = sizeof(int);
    value.data = calloc(1, sizeof(int));
    value.size = sizeof(int);
    for (int i = 0; i < 16; i++) {
        memcpy(key.data, &i, sizeof(int));
        memcpy(value.data, &i, sizeof(int));
        store_insert(kv, &key, &value);
    }
    bool result = kv->store_size == 32;
    store_destroy(kv);
    free(key.data);
    free(value.data);
    return result;
}

bool test_append()
{
    struct store* kv = store_init();
    struct container key, value;
    key.data = (char*)calloc(1, sizeof(char) * 10);
    key.size = 10;
    value.data = (char*)calloc(1, sizeof(char) * 10);
    value.size = 10;
    strncpy(key.data, "super key", 10);
    strncpy(value.data, "supervalue", 10);
    store_insert(kv, &key, &value);
    store_append(kv, &key, &value);
    struct linked_keyvalue* result = store_get(kv, &key);
    bool ret = result != NULL && strncmp(result->value->data, "supervaluesupervalue", result->value->size) == 0;
    store_destroy(kv);
    free(key.data);
    free(value.data);

    return ret;
}

int main(int argc, char const* argv[])
{
    struct test tests[7] = {
        { test_insert, "insert test" }, { test_get, "get test" },
        { test_remove, "remove test" }, { test_destroy_empty_store, "destroy empty store test" },
        { test_resize, "resize test" }, { test_remove_nonexistant, "remove nonexistant test" },
        { test_append, "append test" },
    };
    int result = 0;
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        bool test_succeed = tests[i].run_test();
        if (test_succeed) {
            fprintf(stdout, "TEST SUCCESS: %s\n", tests[i].description);
        } else {
            fprintf(stderr, "TEST FAIL: %s\n", tests[i].description);
            result = 255;
        }
    }
    return result;
}
