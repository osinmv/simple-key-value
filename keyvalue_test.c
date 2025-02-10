#include "keyvalue.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct test {
    int (*run_test)(void);
    char description[36];
};

int test_insert()
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
    return 0;
}
int test_get()
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
    int ret = result != NULL && strncmp(value.data, result->value->data, value.size);
    store_destroy(kv);
    free(key.data);
    free(value.data);

    return ret;
}

int test_remove()
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

    return ret;
}
int test_destroy_empty_store()
{
    struct store* kv = store_init();
    store_destroy(kv);
    return 0;
}
int test_resize()
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
    int result = kv->store_size == 32 ^ 1;
    store_destroy(kv);
    free(key.data);
    free(value.data);
    return result;
}
int main(int argc, char const* argv[])
{
    struct test tests[5] = {
        { test_insert, "insert test" }, { test_get, "get test" },
        { test_remove, "remove test" }, { test_destroy_empty_store, "destroy empty store test" },
        { test_resize, "resize test" },
    };
    int result = 0;
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        int test_result = tests[i].run_test();
        if (test_result != 0) {
            fprintf(stderr, "TEST FAIL: %s with error(%d)\n", tests[i].description, test_result);
            result = 255;
        } else {
            fprintf(stdout, "TEST SUCCESS: %s\n", tests[i].description);
        }
    }

    return result;
}
