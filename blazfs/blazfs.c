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
    size_t file_count = argc - 1;

    blazfs_file_info* files = (blazfs_file_info*)malloc(sizeof(blazfs_file_info) * file_count);
    uint64_t addr = (sizeof(blazfs_file_info) * file_count) + sizeof(blazfs_head_info);

    FILE* fp;

    for (int i = 0; i < file_count; i++) {
        files[i].magic = 0xbadcafee;
        strcpy(files[i].name, argv[i + 1]);

        fp = fopen(argv[i + 1], "r");
        if (fp ==NULL) {
            printf("Couldn't open file '%s'\n", argv[i + 1]);
            return 1;
        }
        fseek(fp, 0L, SEEK_END);

        files[i].size = ftell(fp);

        rewind(fp);
        fclose(fp);

        files[i].address = addr;

        addr += files[i].size;
    }

    fp = fopen("./out/blazfs.fs", "w");

    blazfs_head_info head_info;

    strcpy(head_info.magic, "BLAZ");
    head_info.file_count = file_count;

    fwrite(&head_info, sizeof(blazfs_head_info), 1, fp);
    fwrite(files, sizeof(blazfs_file_info), file_count, fp);

    FILE* stream;
    
    char* tmp_buf;

    for (int i = 0; i < file_count; i++) {
        printf("Saving file '%s'.\n", files[i].name);

        stream = fopen(argv[i + 1], "r");
        tmp_buf = (char*)malloc(files[i].size);

        fread(tmp_buf, 1, files[i].size, stream);
        fwrite(tmp_buf, 1, files[i].size, fp);

        fclose(stream);
        free(tmp_buf);
    }

    fclose(fp);
    return 0;
}