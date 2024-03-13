#pragma once

#include <types.h>

#include <mm/vmm/vmm.h>

#include <lib/string/string.h>

#define elf64_addr u64
#define elf64_half u16
#define elf64_off u64
#define elf64_sword u32
#define elf64_word u32
#define elf64_xword u64
#define elf64_sxword u64

typedef struct {
    unsigned char ident[16];
    elf64_half type;
    elf64_half isa;
    elf64_word elf_version;
    elf64_addr entry;
    elf64_off phoff;
    elf64_off shoff;
    elf64_word flags;
    elf64_half hdr_size;
    elf64_half entry_ph_size;
    elf64_half entry_ph_count;
    elf64_half entry_sh_size;
    elf64_half entry_sh_count;
    elf64_half sh_names;
} elf_header;

typedef struct {
    u32 type;
    u32 flags;
    u64 offset;
    u64 vaddr;
    u64 paddr;
    u64 file_size;
    u64 mem_size;
    u64 align;
} elf_ph;

u64 elf_load(char* addr, page_map* pm);