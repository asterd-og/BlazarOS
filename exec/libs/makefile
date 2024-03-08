# User controllable C flags.
CFLAGS = -static -I../../kernel/src/inc/usr/include -m64 -ffreestanding -nostdlib -std=gnu99 -Wall \
    -Wextra \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -fPIE \
    -march=x86-64 \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone

override NASMFLAGS += \
    -Wall \
    -f elf64

CFILES = $(shell find -L . -type f -name '*.c') 
ASFILES = $(shell find -L . -type f -name '*.asm') 
OBJS = $(CFILES:.c=.o)
OBJS += $(ASFILES:.asm=.o)

all: $(OBJS) lib
	rm $(OBJS)
	@echo all done.

.PHONY: lib
lib:
	ar rcs blazlib.a $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.o: %.asm
	nasm $(NASMFLAGS) $< -o $@

clean:
	rm $(OBJS) *.a