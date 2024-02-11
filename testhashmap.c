#include <stdio.h>
#include "hashmap.h"

int main() {

    HashMap hash_map = create_hash_map(NULL, NULL);
    hash_map->put(hash_map, (void*)"aaaaaa", (void*)"bbbbbbbb");
    char* value = (char*)hash_map->get(hash_map, (void*)"aaaaaa");
    printf("%s",value);
}