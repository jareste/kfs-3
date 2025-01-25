NAME = jareste_kfs.iso
RELEASE_NAME = jareste_kfs_release.iso

BIN_NAME = ./iso/boot/kernel.bin

CC = gcc
AS = nasm
CFLAGS = -m32 -ffreestanding -nostdlib -nodefaultlibs -fno-builtin -fno-exceptions -fno-stack-protector -O3
ASFLAGS = -f elf
LDFLAGS = -m elf_i386

SRC_DIR = srcs
BOOT_DIR = srcs/boot
LINKER_DIR = linker
OBJ_DIR = objs
ISO_DIR = iso
GRUB_DIR = $(ISO_DIR)/boot/grub

vpath %.c $(SRC_DIR) $(SRC_DIR)/utils $(SRC_DIR)/display $(SRC_DIR)/keyboard $(SRC_DIR)/gdt \
			$(SRC_DIR)/idt $(SRC_DIR)/kshell $(SRC_DIR)/io $(SRC_DIR)/timers $(SRC_DIR)/memory \
			$(SRC_DIR)/syscalls

vpath %.asm $(BOOT_DIR) $(SRC_DIR)/keyboard $(SRC_DIR)/gdt $(SRC_DIR)/utils $(SRC_DIR)/memory

C_SOURCES = kernel.c strcmp.c strlen.c printf.c putc.c puts.c keyboard.c \
			idt.c itoa.c gdt.c put_hex.c kdump.c kshell.c memset.c strtol.c \
			hatoi.c get_stack_pointer.c kpanic.c dump_registers_c.c \
			io.c init_timers.c memory.c put_zu.c pmm.c memcpy.c memcmp.c \
			interrupts.c signals.c syscalls.c get_line.c layouts.c

ASM_SOURCES = boot.asm handler.asm gdt_asm.asm dump_registers.asm \
			  clear_registers.asm memory_asm.asm

SRC = $(C_SOURCES) $(ASM_SOURCES)

OBJ = $(addprefix $(OBJ_DIR)/, $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.o))

DEP = $(addsuffix .d, $(basename $(OBJ)))

all: $(NAME)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -MMD $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.asm
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) -o $@ $<

$(NAME): $(OBJ) $(LINKER_DIR)/link.ld
	@mkdir -p $(GRUB_DIR)
	ld $(LDFLAGS) -T $(LINKER_DIR)/link.ld -o kernel.bin $(OBJ)
	cp kernel.bin $(ISO_DIR)/boot/
	cp srcs/boot/grub.cfg $(GRUB_DIR)/.
	grub-mkrescue -o $(NAME) $(ISO_DIR)/

release: $(OBJ) $(LINKER_DIR)/link.ld
	@mkdir -p $(GRUB_DIR)
	ld $(LDFLAGS) -T $(LINKER_DIR)/link.ld -o kernel.bin $(OBJ)
	cp kernel.bin $(ISO_DIR)/boot/
	cp srcs/boot/grub.cfg $(GRUB_DIR)/.
	grub-mkrescue -o $(RELEASE_NAME) $(ISO_DIR) --compress=xz
	
clean:
	rm -f $(OBJ) $(DEP)
	rm -rf $(OBJ_DIR)
	@echo "OBJECTS REMOVED"

fclean: clean
	rm -f kernel.bin $(NAME) $(RELEASE_NAME)
	rm -rf $(ISO_DIR)
	@echo "EVERYTHING REMOVED"

re: fclean all

run:
	qemu-system-i386 -kernel $(BIN_NAME) -m 4096

run_grub:
	qemu-system-i386 -cdrom $(NAME)

run_release: release
	qemu-system-i386 -cdrom $(RELEASE_NAME)

debug:
	qemu-system-i386 -cdrom $(NAME) -d int,cpu_reset

xorriso:
	xorriso -indev $(NAME) -ls /boot/grub/

.PHONY: all clean fclean re run xorriso release run_release

-include $(DEP)
