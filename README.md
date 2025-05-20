## Description 

A minimal, header-only key-value store written in C. It supports insert, get, remove, and automatic resizing when the number of keys grows.

## Installation

Just copy `keyvalue.h` into your project

## Example

```c
#include "keyvalue.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    struct store* kv = store_init();
    store_insert(kv, "key", "value", strlen("value") + 1);
    store_insert(kv, "second key", malloc(1024), 1024);
    struct container* value = store_get(kv, "key");
    printf("Value: %s of size %d\n", (char*)value->data, value->size);
    free_container(value);
    store_destroy(kv);
    return 0;
}
```