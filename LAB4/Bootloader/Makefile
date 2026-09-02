CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

CFLAGS = -fno-stack-protector

SRCS_S = $(wildcard *.S)
SRCS_C = $(wildcard *.c)

OBJ = $(SRCS_S:.S=.o) $(SRCS_C:.c=.o)

.PHONY: all clean

all: kernel8.img

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.elf: $(OBJ)
	$(LD) -T linker.ld -o kernel8.elf $(OBJ)

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

clean:
	rm *.o kernel8.elf kernel8.img