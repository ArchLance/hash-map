#include <stdio.h>
#include "hashmap.h"
#include <string.h>
#include <assert.h>

bool put_get_test() {
    HashMap hash_map = create_hash_map(NULL, NULL);
    // 扩容前测试
    hash_map->put(hash_map, "aa", "aaa");
    char* value1 = (char*)hash_map->get(hash_map, "aa");
    assert(strcmp(value1, (char*)"aaa") == 0);
    // 放8个以上看看扩容情况
    hash_map->put(hash_map, "bb", "bbb");
    hash_map->put(hash_map, "cc", "ccc");
    hash_map->put(hash_map, "dd", "ddd");
    hash_map->put(hash_map, "ee", "eee");
    hash_map->put(hash_map, "ff", "fff");
    char* value3 = (char*)hash_map->get(hash_map, "ff");
    hash_map->put(hash_map, "gg", "ggg");
    hash_map->put(hash_map, "hh", "hhh");
    hash_map->put(hash_map, "ii", "iii");
    // 扩容后测试
    char* value2 = (char*)hash_map->get(hash_map, "gg");
    assert(strcmp(value2, (char*)"ggg") == 0);
    hash_map->put(hash_map, "jj", "jjj");
    // 扩容前get扩容后使用测试
    assert(strcmp(value3, (char*)"fff") == 0);

    // NULL测试
    char* value4 = hash_map->get(hash_map, "kk");
    assert(value4 == NULL);

    // 覆盖测试
    int size = hash_map->size;
    hash_map->put(hash_map, "ff", "ffff");
    assert(size == hash_map->size);
    char* value5 = hash_map->get(hash_map, "ff");
    assert(strcmp(value5, "ffff") == 0);

    size = hash_map->size;
    hash_map->put(hash_map, "jj", "jjjj");
    assert(size == hash_map->size);
    char* value6 = hash_map->get(hash_map, "jj");
    assert(strcmp(value6, "jjjj") == 0);
    // 不存在且该index位置没有放入元素
    char* value7 = hash_map->get(hash_map, "1fdsf");
    assert(value7 == NULL);
    // 测试迭代器
    HashMapIterator iterator = create_hash_map_iterator(hash_map);
    while (has_next_hash_map_iterator(iterator)) {
        iterator = next_hash_map_iterator(iterator);
        printf("{ key: %s, key: %s, hashcode: %d }\n",
            (char *)iterator->entry->key, (char *)iterator->entry->value, iterator->hash_code);
    }
    printf("put get test passed\n");
    free_hash_map_iterator(&iterator);
    hash_map->free(hash_map);
    return true;
}

bool remove_exist_test() {
    HashMap hash_map = create_hash_map(NULL, NULL);

    hash_map->put(hash_map, "aa", "aaa");
    hash_map->put(hash_map, "bb", "bbb");
    hash_map->put(hash_map, "cc", "ccc");
    hash_map->put(hash_map, "dd", "ddd");
    hash_map->put(hash_map, "ee", "eee");
    hash_map->put(hash_map, "ff", "fff");
    hash_map->put(hash_map, "gg", "ggg");
    hash_map->put(hash_map, "hh", "hhh");
    // 扩容前测试remove
    // 删除冲突列表外的
    int size = hash_map->size;
    char* value1 = (char*)hash_map->get(hash_map, "aa");
    assert(!strcmp(value1, "aaa"));
    hash_map->remove(hash_map, "aa");
    value1 = (char*)hash_map->get(hash_map, "aa");
    assert(value1 == NULL);
    assert(hash_map->size == size-1);
    hash_map->put(hash_map, "aa", "aaa");
    // 删除冲突列表中的
    size = hash_map->size;
    char* value2 = (char*)hash_map->get(hash_map, "cc");
    assert(!strcmp(value2, "ccc"));
    hash_map->remove(hash_map, "cc");
    value2 = (char*)hash_map->get(hash_map, "cc");
    assert(value2 == NULL);
    assert(hash_map->size == size-1);
    hash_map->put(hash_map, "cc", "ccc");
    // 测试自动调整内存
    hash_map->put(hash_map, "ii", "iii");
    int list_size = hash_map->list_size;  // 16
    char* value3 = (char*)hash_map->get(hash_map, "ii");
    assert(!strcmp(value3, "iii"));
    hash_map->remove(hash_map, "ii");
    value3 = (char*)hash_map->get(hash_map, "ii");
    assert(value3 == NULL);
    hash_map->remove(hash_map, "hh");
    assert(list_size == hash_map->list_size * 2);

    // exist测试
    bool exist1 = hash_map->exist(hash_map, "1fdsf");
    bool exist2 = hash_map->exist(hash_map, "aa");
    bool exist3 = hash_map->exist(hash_map, "ii");
    assert(exist1 == false);
    assert(exist2 == true);
    assert(exist3 == false);
    // iterator测试
    HashMapIterator iterator = create_hash_map_iterator(hash_map);
    while (has_next_hash_map_iterator(iterator)) {
        iterator = next_hash_map_iterator(iterator);
        printf("{ key: %s, key: %s, hashcode: %d }\n",
            (char *)iterator->entry->key, (char *)iterator->entry->value, iterator->hash_code);
    }
    printf("remove test passed\n");
    free_hash_map_iterator(&iterator);
    hash_map->free(hash_map);
    return true;
}

int main() {
    int passed_count = 0,total_count = 0;
    passed_count += put_get_test();
    passed_count += remove_exist_test();
    total_count += 2;
    printf("passed_test/total_test is %d/%d\n", passed_count, total_count);
}