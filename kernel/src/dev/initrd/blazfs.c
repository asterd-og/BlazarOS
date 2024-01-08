#include <dev/initrd/blazfs.h>

blazfs_head_info* blazfs_head;
char* blazfs_addr;

int blazfs_read(const char* file_name, char* buffer) {
    blazfs_file_info* blazfs_file;
    
    for (size_t i = 0; i < blazfs_head->file_count; i++) {
        blazfs_file = (blazfs_file_info*)(blazfs_addr + (sizeof(blazfs_head_info) + (sizeof(blazfs_file_info) * i)));
        if (!strcmp(blazfs_file->name, file_name)) {
            memcpy(buffer, blazfs_addr + blazfs_file->address, blazfs_file->size);
            return 0;
        }
    }

    return BLAZFS_FILE_NOT_FOUND;
}

size_t blazfs_ftell(const char* file_name) {
    blazfs_file_info* blazfs_file;
    
    for (size_t i = 0; i < blazfs_head->file_count; i++) {
        blazfs_file = (blazfs_file_info*)(blazfs_addr + (sizeof(blazfs_head_info) + (sizeof(blazfs_file_info) * i)));
        if (!strcmp(blazfs_file->name, file_name))
            return blazfs_file->size;
    }

    return BLAZFS_FILE_NOT_FOUND;
}

int blazfs_init(void* addr) {
    blazfs_addr = (char*)addr;
    blazfs_head = (blazfs_head_info*)addr;

    if (memcmp(blazfs_head->magic, "BLAZ", 3))
        return BLAZFS_INVALID_HEAD_MAGIC;
    
    return 0;
}