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
    value.data = (char*)calloc(1, sizeof(char) * 11);
    value.size = 11;
    strncpy(key.data, "super key", 10);
    strncpy(value.data, "supervalue", 11);
    store_insert(kv, &key, &value);
    store_destroy(kv);
    free(key.data);
    free(value.data);
    return true;
}
bool test_get()
{
    struct store* kv = store_init();
    struct container key, value;
    key.data = (char*)calloc(1, sizeof(char) * 10);
    key.size = 10;
    value.data = (char*)calloc(1, sizeof(char) * 11);
    value.size = 11;
    strncpy(key.data, "super key", 10);
    strncpy(value.data, "supervalue", 11);
    store_insert(kv, &key, &value);
    struct bucket* result = store_get(kv, &key);
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
    value.data = (char*)calloc(1, sizeof(char) * 11);
    value.size = 11;
    strncpy(key.data, "super key", 10);
    strncpy(value.data, "supervalue", 11);
    store_insert(kv, &key, &value);
    int ret = store_remove(kv, &key);
    store_destroy(kv);
    free(key.data);
    free(value.data);

    return ret == 0;
}

bool test_remove_nonexistant()
{
    struct store* kv = store_init();
    struct container key, value;
    key.data = (char*)calloc(1, sizeof(char) * 10);
    key.size = 10;
    int ret = store_remove(kv, &key);
    store_destroy(kv);
    free(key.data);
    free(value.data);

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
    key.data = (char*)calloc(1, sizeof(char) * 10);
    key.size = 10;
    value.data = (char*)calloc(1, sizeof(char) * 11);
    value.size = 11;
    strncpy(key.data, "super key", 10);
    strncpy(value.data, "supervalue", 11);
    for (int i = 0; i < 16; i++)
        store_insert(kv, &key, &value);
    bool result = kv->store_size == 32;
    store_destroy(kv);
    free(key.data);
    free(value.data);
    return result;
}
int main(int argc, char const* argv[])
{
    struct test tests[6] = {
        { test_insert, "insert test" }, { test_get, "get test" },
        { test_remove, "remove test" }, { test_destroy_empty_store, "destroy empty store test" },
        { test_resize, "resize test" }, { test_remove_nonexistant, "remove nonexistant test" },
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
