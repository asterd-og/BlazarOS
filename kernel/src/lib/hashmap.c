#include <lib/hashmap.h>

u64 hashmap_hash(char* key) {
    u64 hash = 5381;
    int c = 0;
    while (c = *key++)
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}

void hashmap_push(hashmap_table* map, char* key, u8* value) {
    u64 key_hashed = hashmap_hash(key) % map->max_items;
    if (map->table[key_hashed].collisions == 0) {
        map->table[key_hashed].entries = (hashmap_value*)kmalloc(sizeof(hashmap_value) * map->max_collisions);
    }
    hashmap_info* entry = &map->table[key_hashed];
    hashmap_value* entry_value = &entry->entries[entry->collisions];
    entry_value->key = (char*)kmalloc(strlen(key));
    entry_value->value = (u8*)kmalloc(map->item_size);
    memcpy(entry_value->key, key, strlen(key));
    memcpy(entry_value->value, value, map->item_size);
    entry->collisions++;
}

u8* hashmap_get(hashmap_table* map, char* key) {
    u64 key_hashed = hashmap_hash(key) % map->max_items;
    hashmap_info entry = map->table[key_hashed];
    if (map->table[key_hashed].collisions == 0) return NULL;
    if (entry.collisions > 1) {
        for (u64 i = 0; i < entry.collisions; i++)
            if (!strcmp(key, entry.entries[i].key))
                return entry.entries[i].value;
        return NULL;
    }
    return entry.entries[0].value;
}

hashmap_table* hashmap_init(int size, int item_size, int max_collisions) {
    hashmap_table* map = (hashmap_table*)kmalloc(sizeof(hashmap_table));
    map->item_size = item_size;
    map->size = size * item_size;
    map->max_collisions = max_collisions;
    map->max_items = size;
    map->table = (hashmap_info*)kmalloc(sizeof(hashmap_info) * size);
    memset(map->table, 0, sizeof(hashmap_info) * size);
    return map;
}
