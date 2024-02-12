#include <stdio.h>
#include "hashmap.h"
#include <string.h>
#include <assert.h>

bool put_get_test(HashMap hash_map) {
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
    printf("put get test passed\n");
    return true;
}


int main() {
    int passed_count = 0,total_count = 0;
    HashMap hash_map = create_hash_map(NULL, NULL);
    passed_count += put_get_test(hash_map);
    total_count += 1;
    printf("passed_test/total_test is %d/%d\n", passed_count, total_count);
}