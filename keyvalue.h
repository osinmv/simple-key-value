#define DEFAULT_SIZE 16
#define DEFAULT_LOAD_FACTOR 70
#define DEFAULT_MEM_LIMIT 32
struct value {
    struct value* next;
    char* value;
    char* key;
};
struct store {
    struct value* buckets;
    unsigned long count;
    unsigned int store_size;
    int load_factor;
};

enum STORE_ERROR {
    MEMERR = -1,
    RESIZEERR = -2,
    INSERTERR = -3,
    DELETEERR = -4
};
struct store* store_init();
unsigned long _hash(char* key);
int store_insert(struct store* kv, char* key, char* value);
int store_remove(struct store* kv, char* key);
int store_destroy(struct store* kv);
int _store_resize(struct store* old_kv);
struct value* store_get(struct store* kv, char* key);
