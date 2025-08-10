# QiYuanOS Makefile
# 操作系统构建配置

# 编译器设置
CC = gcc
LD = ld
ASM = nasm
OBJCOPY = objcopy
STRIP = strip

# 编译标志
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs
CFLAGS += -Wall -Wextra -Werror -O2 -std=c99
CFLAGS += -I./kernel -I./fs -I./gui -I./boot
CFLAGS += -D__KERNEL__ -D__i386__

# 链接标志
LDFLAGS = -m elf_i386 -nostdlib -nodefaultlibs
LDFLAGS += -T linker.ld

# 汇编标志
ASMFLAGS = -f elf32

# 目标文件
KERNEL_OBJS = kernel/kernel.o
FS_OBJS = fs/fs.o
GUI_OBJS = gui/gui.o
BOOT_OBJS = boot/boot.o

# 所有目标文件
ALL_OBJS = $(KERNEL_OBJS) $(FS_OBJS) $(GUI_OBJS) $(BOOT_OBJS)

# 最终目标
TARGET = kernel.bin
ISO_TARGET = qi yuanos.iso

# 默认目标
all: $(TARGET)

# 编译内核文件
kernel/kernel.o: kernel/kernel.c kernel/kernel.h
	@echo "编译内核..."
	@mkdir -p kernel
	$(CC) $(CFLAGS) -c $< -o $@

# 编译文件系统
fs/fs.o: fs/fs.c fs/fs.h
	@echo "编译文件系统..."
	@mkdir -p fs
	$(CC) $(CFLAGS) -c $< -o $@

# 编译GUI系统
gui/gui.o: gui/gui.c gui/gui.h
	@echo "编译GUI系统..."
	@mkdir -p gui
	$(CC) $(CFLAGS) -c $< -o $@

# 编译引导程序
boot/boot.o: boot/boot.c boot/boot.h
	@echo "编译引导程序..."
	@mkdir -p boot
	$(CC) $(CFLAGS) -c $< -o $@

# 链接内核
$(TARGET): $(ALL_OBJS) linker.ld
	@echo "链接内核..."
	$(LD) $(LDFLAGS) -o $@ $(ALL_OBJS)
	@echo "内核构建完成: $(TARGET)"

# 创建ISO镜像
iso: $(TARGET)
	@echo "创建ISO镜像..."
	@mkdir -p isofiles/boot/grub
	@cp $(TARGET) isofiles/boot/
	@echo "set timeout=0" > isofiles/boot/grub/grub.cfg
	@echo "set default=0" >> isofiles/boot/grub/grub.cfg
	@echo "" >> isofiles/boot/grub/grub.cfg
	@echo "menuentry \"QiYuanOS\" {" >> isofiles/boot/grub/grub.cfg
	@echo "    multiboot /boot/kernel.bin" >> isofiles/boot/grub/grub.cfg
	@echo "    boot" >> isofiles/boot/grub/grub.cfg
	@echo "}" >> isofiles/boot/grub/grub.cfg
	@grub-mkrescue -o $(ISO_TARGET) isofiles 2>/dev/null || \
	 grub2-mkrescue -o $(ISO_TARGET) isofiles 2>/dev/null || \
	 echo "警告: 无法创建ISO镜像，请安装grub-mkrescue"
	@echo "ISO镜像创建完成: $(ISO_TARGET)"

# 运行QEMU模拟器
run: $(TARGET)
	@echo "启动QEMU模拟器..."
	@qemu-system-i386 -kernel $(TARGET) -serial stdio

# 运行QEMU模拟器 (调试模式)
debug: $(TARGET)
	@echo "启动QEMU调试模式..."
	@qemu-system-i386 -kernel $(TARGET) -serial stdio -s -S

# 运行QEMU模拟器 (从ISO启动)
run-iso: iso
	@echo "从ISO启动QEMU模拟器..."
	@qemu-system-i386 -cdrom $(ISO_TARGET) -serial stdio

# 清理构建文件
clean:
	@echo "清理构建文件..."
	@rm -f $(ALL_OBJS) $(TARGET) $(ISO_TARGET)
	@rm -rf isofiles
	@echo "清理完成"

# 安装依赖 (Ubuntu/Debian)
deps-ubuntu:
	@echo "安装Ubuntu/Debian依赖..."
	@sudo apt-get update
	@sudo apt-get install -y gcc nasm qemu-system-x86 grub-pc-bin xorriso

# 安装依赖 (Fedora/CentOS)
deps-fedora:
	@echo "安装Fedora/CentOS依赖..."
	@sudo dnf install -y gcc nasm qemu-system-x86 grub2-pc xorriso

# 安装依赖 (Arch Linux)
deps-arch:
	@echo "安装Arch Linux依赖..."
	@sudo pacman -Syyu --noconfirm
	@sudo pacman -S --noconfirm gcc nasm qemu-system-x86 grub xorriso

# 显示帮助
help:
	@echo "QiYuanOS 构建系统"
	@echo "=================="
	@echo ""
	@echo "可用目标:"
	@echo "  all        - 构建内核 (默认)"
	@echo "  iso        - 创建可启动ISO镜像"
	@echo "  run        - 在QEMU中运行内核"
	@echo "  debug      - 在QEMU中调试内核"
	@echo "  run-iso    - 在QEMU中从ISO启动"
	@echo "  clean      - 清理构建文件"
	@echo ""
	@echo "依赖安装:"
	@echo "  deps-ubuntu  - 安装Ubuntu/Debian依赖"
	@echo "  deps-fedora  - 安装Fedora/CentOS依赖"
	@echo "  deps-arch    - 安装Arch Linux依赖"
	@echo ""
	@echo "示例:"
	@echo "  make all && make run"
	@echo "  make iso && make run-iso"

.PHONY: all iso run debug run-iso clean help deps-ubuntu deps-fedora deps-arch