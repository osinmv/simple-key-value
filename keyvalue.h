#define DEFAULT_SIZE 16
#define DEFAULT_LOAD_FACTOR 70
#define DEFAULT_MEM_LIMIT 32
struct bucket {
    struct bucket* next;
    struct container* value;
    struct container* key;
};

struct container {
    int size;
    char* data;
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
    DELETEERR = -4
};
struct store* store_init();
unsigned long _hash(struct container* key);
int store_insert(struct store* kv, struct container* key, struct container* value);
int store_remove(struct store* kv, struct container* key);
int store_destroy(struct store* kv);
int _store_resize(struct store* old_kv);
struct bucket* store_get(struct store* kv, struct container* key);
