#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char magic[4];
    size_t file_count;
} blazfs_head_info;

typedef struct {
    uint64_t magic;
    char name[50];
    uint64_t address;
    size_t size;
} blazfs_file_info;

int main(int argc, char** argv) {
    FILE* fp = fopen(argv[1], "r");
    
    fseek(fp, 0L, SEEK_END);
    size_t size = ftell(fp);

    char* buffer = (char*)malloc(size);
    
    rewind(fp);
    fread(buffer, 1, size, fp);

    blazfs_head_info* blazfs_head = (blazfs_head_info*)buffer;
    
    printf("BlazFS Head Info:\n\t- Magic: '%s'\n\t- File Count: %ld\n\n", blazfs_head->magic, blazfs_head->file_count);
    
    blazfs_file_info* blazfs_file;

    printf("BlazFS Files:");

    if (blazfs_head->file_count > 0) {
        for (int i = 0; i < blazfs_head->file_count; i++) {
            blazfs_file = (blazfs_file_info*)(buffer + sizeof(blazfs_head_info) + (i * sizeof(blazfs_file_info)));
            if (blazfs_file->magic != 0xbadcafee) {
                printf("Invalid File Header.\n");
                fclose(fp);
                free(buffer);
                exit(1);
            }
            printf("\n\t- File Name: '%s'\n\t- File Size: %ld", blazfs_file->name, blazfs_file->size);
        }
    }

    printf("\n");
    
    return 0;
}