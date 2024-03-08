# User controllable C flags.
CFLAGS = -m64 -I../libs -I../../kernel/src/inc/usr/include -ffreestanding -std=gnu99 -Wall \
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
LD_FLAGS = -Tlinker.ld -nostdlib

CFILES = $(shell find -L . -type f -name '*.c')
OBJ = $(CFILES:.c=.o)

all: clean
	clang -static $(CFLAGS) -c terminal.c -o terminal.o
	ld -static terminal.o ../libs/blazlib.a $(LD_FLAGS) -o ../out/terminal
	@echo all done.

%.o: %.c
	clang -static $(CFLAGS) -c $< -o $@
	ld -static $@ ../libs/blazlib.a $(LD_FLAGS) -o ../out/$(basename $@)

clean:
	-rm -f $(OBJ)
