#include "hashmap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// 默认的hash函数
int default_hash_code(HashMap hash_map, void* key) {
    uint8_t* key_ptr = (uint8_t*)key;
    unsigned long value = 0x811C9DC5;
    unsigned long h = 0;
    while(*key_ptr) {
        h = (h + *key_ptr) * 0x01000193;
        ++key_ptr;
    }
    return h % hash_map->list_size;
}

HashMapIterator create_hash_map_iterator(HashMap hash_map) {
    HashMapIterator iterator = new_hash_map_iterator();
    iterator->hash_map = hash_map;
    iterator->count = 0;
    iterator->hash_code = -1;
    iterator->entry = NULL;
    return iterator;
}

bool has_next_hash_map_iterator(HashMapIterator iterator) {
    return iterator->count < iterator->hash_map->size ? true : false; 
}

HashMapIterator next_hash_map_iterator(HashMapIterator iterator) {
    if(has_next_hash_map_iterator(iterator)) {
        // 如果当前entry冲突地址上的next不为空
        if(iterator->entry != NULL && iterator->entry->next != NULL) {
            ++iterator->count;
            iterator->entry = iterator->entry->next;
            return iterator;
        }
        // 如果为空，说明冲突链表结束了，该找下一个不为空的hash桶中的位置了
        while(++iterator->hash_code < iterator->hash_map->list_size) {
            Entry entry = &iterator->hash_map->list[iterator->hash_code];
            if(entry->key != NULL) {
                ++iterator->count;
                iterator->entry = entry;
                break;
            }
        }
        return iterator;
    }
    // 不存在下一个iterator
    return NULL;
}

void free_hash_map_iterator(HashMapIterator* iterator) {
    free(*iterator);
    // 需要在函数内部释放迭代器的内存，并且确保在函数返回后，
    // 传入的指针不再指向已释放的内存，避免悬垂引用。
    // 在 C 语言中，函数参数是按值传递的，而不是按引用传递的。
    *iterator = NULL;
}

void reset_hash_map(HashMap hash_map, int list_size) {
    if(list_size < 8) return ;
    // 为元素创建临时存储空间，后面转移要用
    Entry temp_list = new_entry_list(list_size);
    // 创建迭代器，方便向后遍历
    HashMapIterator iterator = create_hash_map_iterator(hash_map);
    int length = hash_map->size;
    for(int i = 0;has_next_hash_map_iterator(iterator); ++i) {
        iterator = next_hash_map_iterator(iterator);
        temp_list[i].key = iterator->entry->key;
        temp_list[i].value = iterator->entry->value;
        temp_list[i].next = NULL;
    }
    free_hash_map_iterator(&iterator);

    // 清除原有数据
    for(int i = 0;i < hash_map->list_size; ++i) {
        hash_map->list[i].key = NULL;
        hash_map->list[i].value = NULL;
        // 这个next必须释放，否则会内存泄漏
        while(hash_map->list[i].next != NULL) {
            Entry temp = hash_map->list[i].next->next;
            free(hash_map->list[i].next);
            hash_map->list[i].next = temp;
        }
    }

    // 更改内存大小
    hash_map->list_size = list_size;
    Entry relist = (Entry)realloc(hash_map->list, hash_map->list_size * sizeof(struct entry));
    if(relist != NULL) {
        hash_map->list = relist;
        relist = NULL;
    }
    // 初始化数据
    for(int i = 0;i < hash_map->list_size; ++i) {
        hash_map->list[i].key = NULL;
        hash_map->list[i].value = NULL;
        hash_map->list[i].next = NULL;
    }
    // 转移数据,将原数据转移到新开辟的内存中
    for(int i = 0;i < length; ++i) {
        hash_map->put(hash_map, temp_list[i].key, temp_list[i].value);
    }
    free(temp_list);
}

bool default_equal(void* key1, void* key2) {
    return strcmp((const char*) key1, (const char*) key2) ? false : true;
}

void default_put(HashMap hash_map, void* key, void* value) {
    if(hash_map->auto_assign && hash_map->size >= hash_map->list_size) {
        // 内存扩充至原来的两倍
        // 注意：⚠️扩充考虑的是当前存储元素的数量和hash表的大小之间的关系，而不是
        // 存储空间是否已经存满。
        // 扩充存储空间，对原有键值对进行再次散列，会把冲突的数据再次分散开，加快索引定位速度。
        reset_hash_map(hash_map, hash_map->list_size * 2);
    }
    int index = hash_map->hash_code(hash_map, key);
    if(hash_map->list[index].key == NULL) {
        ++hash_map->size;
        hash_map->list[index].key = key;
        hash_map->list[index].value = value;
        printf("%s %s\n", (char*) key, (char*) value);
    } else {
        Entry current_entry = &hash_map->list[index];
        while(hash_map->list[index].next != NULL) {
            // 如果冲突列表中存在则直接覆盖
            if(hash_map->equal(key, current_entry->key)) {
                current_entry->value = value;
                return ;
            }
            current_entry = current_entry->next;
        }
        // 这里是尾插法
        current_entry->next = new_entry();
        current_entry->next->key = key;
        current_entry->next->value = value;
        current_entry->next->next = NULL;
        ++hash_map->size;
    }
}
// 根据key获取相应value，不存在则返回NULL
void* default_get(HashMap hash_map, void* key) {
    int index = hash_map->hash_code(hash_map, key);
    Entry current_entry = &hash_map->list[index];
    while(current_entry != NULL && !hash_map->equal(current_entry->key, key)) {
        current_entry = current_entry->next;
    }
    return current_entry->value;
}
// 判断hash表中是否存在对应key
bool default_exist(HashMap hash_map, void* key) {
    int index = hash_map->hash_code(hash_map, key);
    Entry current_entry = &hash_map->list[index];
    while(current_entry != NULL && !hash_map->equal(current_entry->key, key)) {
        current_entry = current_entry->next;
    }
    if(current_entry->key == NULL){
        return false;
    }
    return true;
}
// 从hash表中移除key
bool default_remove(HashMap hash_map, void* key) {
    int index = hash_map->hash_code(hash_map, key);
    Entry current_entry = &hash_map->list[index];
    bool result = false;
    // 首先判断hash table中有无key
    if(current_entry->key != NULL) {
        if(hash_map->equal(current_entry->key, key)) {
            --hash_map->size;
            if(current_entry->next != NULL) {
                // 这里需要注意不能释放当前list中的entry，需要先将entry->next中数据转移到entry中
                // 然后释放当前list中entry->next的空间
                Entry next_entry = current_entry->next;
                current_entry->key = next_entry->key;
                current_entry->value = next_entry->value;
                current_entry->next = next_entry->next;
                free(next_entry);
                result = true;
            } else {
                // 首选判断当前entry->next是否为NULL，如果为NULL，则不需要free
                // 后续销毁hash表时会free该entry list
                current_entry->key = NULL;
                current_entry->value = NULL;
                current_entry->next = NULL;
                result = true;
            }
        }
        // 保留一个前节点，直接赋值就可以->next->next是否为NULL并不影响使用
        while(current_entry->next != NULL) {
            if(hash_map->equal(current_entry->next->key, key)) {
                --hash_map->size;
                Entry next2_entry = current_entry->next->next;
                free(current_entry->next);
                current_entry->next = next2_entry;
                result = true;
            }
            current_entry = current_entry->next;
        }
    }
    // 最后一定记得自动分配内存的事情
    // 如果小于一半就释放一半的内存
    if(hash_map->auto_assign && hash_map->size < hash_map->list_size/2) {
        reset_hash_map(hash_map, hash_map->list_size/2);
    }
    return result;
}

void default_clear(HashMap hash_map) {
    for(int i = 0;i < hash_map->list_size; ++i) {
        Entry entry = &hash_map->list[i];
        if(entry != NULL) {
            while(entry->next != NULL) {
                Entry temp = entry->next->next;
                free(entry->next);
                entry->next = temp;
            }
            entry->key = NULL;
            entry->value = NULL;
        }
    }
    free(hash_map->list);
    hash_map->list = NULL;
    hash_map->size = -1;
    hash_map->list_size = 0;
}

HashMap create_hash_map(HashCode hash_code, Equal equal) {
    HashMap hash_map = new_hash_map();
    hash_map->size = 0;
    hash_map->list_size = 8;
    // 将函数指针赋值给结构体
    hash_map->hash_code = hash_code == NULL ? default_hash_code : hash_code;
    hash_map->equal = equal == NULL ? default_equal : equal;
    hash_map->put = default_put;
    hash_map->get = default_get;
    hash_map->remove = default_remove;
    hash_map->clear = default_clear;
    hash_map->exist = default_exist;
    hash_map->auto_assign = true;
    // 为list分配空间
    hash_map->list = new_entry_list(hash_map->list_size);
    // 记得初始化
    for(int i = 0; i < hash_map->list_size; ++i) {
        hash_map->list[i].key = NULL;
        hash_map->list[i].value = NULL;
        hash_map->list[i].next = NULL;
    }
    // 返回hash_map
    return hash_map;
}