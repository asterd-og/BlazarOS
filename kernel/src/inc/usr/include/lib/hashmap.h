#pragma once

#include <types.h>

#include <mm/heap/heap.h>
#include <lib/string/string.h>

typedef struct {
    char* key;
    u8* value;
} hashmap_value;

typedef struct {
    hashmap_value* entries;
    int collisions;
} hashmap_info;

// The hashmap table will be a table with hasmap infos inside it
// if we end up hashing to the same index twice (or more)
// we count that as a collision

typedef struct {
    hashmap_info* table;
    size_t item_size;
    u64 size;
    u64 max_collisions;
    u64 max_items;
} hashmap_table;

hashmap_table* hashmap_init(int size, int item_size, int max_collisions);
void hashmap_push(hashmap_table* table, char* key, u8* value);
u8* hashmap_get(hashmap_table* table, char* key);