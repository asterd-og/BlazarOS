# User controllable C flags.
CFLAGS = -m64 -I../lib -ffreestanding -std=gnu99
LD_FLAGS = -Tlinker.ld -nostdlib

CFILES = $(shell find -L . -type f -name '*.c')
OBJ = $(CFILES:.c=.o)

all: $(OBJ) clean
	@echo all done.

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@
	gcc $@ $(LD_FLAGS) -o ../out/$(basename $@)

clean:
	-rm -f $(OBJ)
