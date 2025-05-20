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
    int result = store_insert(kv, "super key", "super value", strlen("super value") + 1);
    store_destroy(kv);
    return result == OK;
}

bool test_get()
{
    struct store* kv = store_init();
    store_insert(kv, "super key", "super value", strlen("super value") + 1);
    struct container* result = store_get(kv, "super key");
    bool ret = result != NULL && strncmp("super value", result->data, strlen("super value") + 1) == 0;
    free_container(result);
    store_destroy(kv);
    return ret;
}

bool test_remove()
{
    struct store* kv = store_init();
    store_insert(kv, "super key", "super value", strlen("super value") + 1);
    int result = store_remove(kv, "super key");
    store_destroy(kv);
    return result == OK;
}

bool test_remove_nonexistant()
{
    struct store* kv = store_init();
    store_insert(kv, "super key", "super value", strlen("super value") + 1);
    int result = store_remove(kv, "meh key");
    store_destroy(kv);
    return result == DELETEERR;
}

bool test_destroy_empty_store()
{
    struct store* kv = store_init();
    return OK == store_destroy(kv);
}

bool test_resize()
{
    struct store* kv = store_init();
    char* keys[32] = { "key0",  "key1",  "key2",  "key3",  "key4",  "key5",  "key6",  "key7",
                       "key8",  "key9",  "key10", "key11", "key12", "key13", "key14", "key15",
                       "key16", "key17", "key18", "key19", "key20", "key21", "key22", "key23",
                       "key24", "key25", "key26", "key27", "key28", "key29", "key30", "key31" };
    for (int i = 0; i < 16; i++) {
        store_insert(kv, keys[i], keys[i], strlen(keys[i]) + 1);
    }
    bool result = kv->store_size == 32;
    store_destroy(kv);
    return result;
}

int main(int argc, char const* argv[])
{
    struct test tests[6] = { { test_insert, "insert test" },
                             { test_get, "get test" },
                             { test_remove, "remove test" },
                             { test_destroy_empty_store, "destroy empty store test" },
                             { test_remove_nonexistant, "remove nonexistant test" },
                             { test_resize, "resize test" } };
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
