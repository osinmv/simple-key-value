#include "keyvalue.h"
#include "malloc/malloc.h"
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
    char* key = (char*)calloc(1, sizeof(char) * 32);
    char* value = (char*)calloc(1, sizeof(char) * 32);
    strcpy(key, "super key");
    strcpy(value, "supervalue");
    int result = store_insert(kv, key, value);
    store_destroy(kv);
    return result;
}
int test_get()
{
    struct store* kv = store_init();
    char* key = (char*)calloc(1, sizeof(char) * 32);
    char* value = (char*)calloc(1, sizeof(char) * 32);
    strcpy(key, "super key");
    strcpy(value, "supervalue");
    store_insert(kv, key, value);
    struct value* val = store_get(kv, key);
    store_destroy(kv);
    return -1;

    // return strcmp(val->value, value);
}
int main(int argc, char const* argv[])
{
    struct test tests[2] = {
        { test_insert, "insert test" },
        { test_get, "get test" },
    };
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        int test_result = tests[i].run_test();
        if (test_result != 0)
            fprintf(stderr, "TEST FAIL: %s with error(%d)\n", tests[i].description, test_result);
    }

    return 0;
}
