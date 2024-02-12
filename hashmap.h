#ifndef HASHMAP_H
#define HASHMAP_H
#include<stdbool.h>
#define NEW(type) (type*)malloc(sizeof(type))
typedef struct entry {
    void* key;
    void* value;
    struct entry* next;
}*Entry;

#define new_entry() NEW(struct entry)
#define new_entry_list(length) (Entry)malloc(length * sizeof(struct entry))

typedef struct hashMap* HashMap;
#define new_hash_map() NEW(struct hashMap)

typedef int(*HashCode)(HashMap hash_map, void* key);

typedef bool(*Equal)(void* key1, void* key2);

typedef void(*Put)(HashMap hash_map, void* key, void* value);

typedef void*(*Get)(HashMap hash_map, void* key);

typedef bool(*Remove)(HashMap hash_map, void* key);

typedef void(*Free)(HashMap hash_map);

typedef bool(*Exists)(HashMap hash_map, void* key);

typedef struct hashMap {
    int size;                       // 当前大小
    int list_size;                  // 有效空间大小
    HashCode hash_code;             // hash函数
    Equal equal;                    // 比较函数
    Entry list;                     // 存储区域
    Put put;                        // 添加键的函数
    Get get;                        // 获取键对应函数
    Remove remove;                  // 删除键函数
    Free free;                    // 清空hash表
    Exists exist;                    // 判断键是否存在
    bool auto_assign;               // 设定是否根据当前数量动态调整内存大小，默认情况下开启
}* HashMap;

static int default_hash_code(HashMap hash_map, void* key);

static bool default_equal(void* key1, void* key2);

static void default_put(HashMap hash_map, void* key, void* value);

static void* default_get(HashMap hash_map, void* key);

static bool default_remove(HashMap hash_map, void* key);

static void default_free(HashMap hash_map);

static bool default_exist(HashMap hash_map, void* key);

HashMap create_hash_map(HashCode hash_code, Equal equal);

static void reset_hash_map(HashMap hash_map, int list_size);

typedef struct hashMapIterator {
    Entry entry;        // 迭代器当前指向
    int count;          // 迭代次数
    int hash_code;      // 键值对的hash值
    HashMap hash_map;   
}* HashMapIterator;

#define new_hash_map_iterator() NEW(struct hashMapIterator)

HashMapIterator create_hash_map_iterator(HashMap hash_map);

bool has_next_hash_map_iterator(HashMapIterator iterator);

HashMapIterator next_hash_map_iterator(HashMapIterator iterator);

void free_hash_map_iterator(HashMapIterator* iterator);
#endif