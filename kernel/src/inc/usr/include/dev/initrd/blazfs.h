#pragma once

#include <types.h>

#include <lib/string/string.h>

#define BLAZFS_FILE_MAGIC 0xbadcafee

typedef struct {
    char magic[4];
    size_t file_count;
} blazfs_head_info;

typedef struct {
    u64 magic;
    char name[50];
    u64 address;
    size_t size;
} blazfs_file_info;

enum {
    BLAZFS_SUCCESS,
    BLAZFS_INVALID_HEAD_MAGIC,
    BLAZFS_FILE_NOT_FOUND
};

int blazfs_init(void* addr);

int blazfs_read(const char* file_name, u8* buffer);
size_t blazfs_ftell(const char* file_name);