#include "boot.h"

// 简单的 VGA 文本模式输出
#define VGA_TEXT_ADDR        0xB8000
#define VGA_WIDTH            80
#define VGA_HEIGHT           25

// 颜色定义
#define VGA_COLOR_BLACK      0
#define VGA_COLOR_BLUE       1
#define VGA_COLOR_GREEN      2
#define VGA_COLOR_CYAN       3
#define VGA_COLOR_RED        4
#define VGA_COLOR_MAGENTA    5
#define VGA_COLOR_BROWN      6
#define VGA_COLOR_LIGHT_GRAY 7
#define VGA_COLOR_DARK_GRAY  8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN  11
#define VGA_COLOR_LIGHT_RED   12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_YELLOW     14
#define VGA_COLOR_WHITE      15

// 全局变量
static int vga_x = 0;
static int vga_y = 0;
static boot_info_t boot_info;

// 内联汇编宏
#define PORT_OUTB(port, value) __asm__ __volatile__ ("outb %0, %1" : : "a"(value), "Nd"(port))
#define PORT_INB(port) __asm__ __volatile__ ("inb %1, %0" : "=a"(value) : "Nd"(port))

// VGA 输出函数
static void vga_putc(char c, int color) {
    volatile u16* vga = (volatile u16*)VGA_TEXT_ADDR;
    
    if (c == '\n') {
        vga_x = 0;
        vga_y++;
        if (vga_y >= VGA_HEIGHT) {
            vga_y = 0;
        }
        return;
    }
    
    if (c == '\r') {
        vga_x = 0;
        return;
    }
    
    if (c == '\t') {
        vga_x = (vga_x + 8) & ~7;
        if (vga_x >= VGA_WIDTH) {
            vga_x = 0;
            vga_y++;
            if (vga_y >= VGA_HEIGHT) {
                vga_y = 0;
            }
        }
        return;
    }
    
    vga[vga_y * VGA_WIDTH + vga_x] = (u16)c | (u16)(color << 8);
    vga_x++;
    
    if (vga_x >= VGA_WIDTH) {
        vga_x = 0;
        vga_y++;
        if (vga_y >= VGA_HEIGHT) {
            vga_y = 0;
        }
    }
}

static void vga_clear(void) {
    volatile u16* vga = (volatile u16*)VGA_TEXT_ADDR;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = (u16)' ' | (u16)(VGA_COLOR_LIGHT_GRAY << 8);
    }
    vga_x = 0;
    vga_y = 0;
}

// 引导打印函数
void boot_print(const char* str) {
    while (*str) {
        vga_putc(*str++, VGA_COLOR_LIGHT_GRAY);
    }
}

void boot_print_hex(uint32_t value) {
    const char hex_chars[] = "0123456789ABCDEF";
    boot_print("0x");
    for (int i = 7; i >= 0; i--) {
        vga_putc(hex_chars[(value >> (i * 4)) & 0xF], VGA_COLOR_LIGHT_GRAY);
    }
}

void boot_print_dec(uint32_t value) {
    char buffer[12];
    int i = 0;
    
    if (value == 0) {
        vga_putc('0', VGA_COLOR_LIGHT_GRAY);
        return;
    }
    
    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }
    
    while (i > 0) {
        vga_putc(buffer[--i], VGA_COLOR_LIGHT_GRAY);
    }
}

// 延迟函数
void boot_delay(uint32_t ms) {
    // 简单的忙等待延迟
    for (uint32_t i = 0; i < ms * 1000; i++) {
        __asm__ __volatile__ ("nop");
    }
}

// 内存检测函数
static void detect_memory(void) {
    boot_print("检测内存...\n");
    
    // 简化实现，假设有 64MB 内存
    boot_info.mem_lower = 640;     // 640KB 低内存
    boot_info.mem_upper = 63488;   // 62MB 高内存
    boot_info.flags |= BOOT_FLAG_MEM_INFO;
    
    boot_print("低内存: ");
    boot_print_dec(boot_info.mem_lower);
    boot_print(" KB\n");
    
    boot_print("高内存: ");
    boot_print_dec(boot_info.mem_upper);
    boot_print(" KB\n");
}

// CPU 检测函数
static void detect_cpu(void) {
    boot_print("检测 CPU...\n");
    
    // 检查 CPUID 指令支持
    u32 eax, ebx, ecx, edx;
    __asm__ __volatile__ (
        "pushfl\n"
        "popl %%eax\n"
        "movl %%eax, %%ebx\n"
        "xorl $0x200000, %%eax\n"
        "pushl %%eax\n"
        "popfl\n"
        "pushfl\n"
        "popl %%eax\n"
        "cmpl %%ebx, %%eax\n"
        "jz 1f\n"
        "movl $1, %%eax\n"
        "jmp 2f\n"
        "1:\n"
        "movl $0, %%eax\n"
        "2:\n"
        : "=a"(eax)
        :
        : "ebx", "ecx", "edx"
    );
    
    if (eax) {
        boot_print("支持 CPUID 指令\n");
        
        // 获取 CPU 厂商信息
        __asm__ __volatile__ (
            "cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(0)
        );
        
        boot_print("\nCPU 厂商: ");
        vga_putc((ebx >> 0) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        vga_putc((ebx >> 8) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        vga_putc((ebx >> 16) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        vga_putc((ebx >> 24) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        vga_putc((edx >> 0) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        vga_putc((edx >> 8) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        vga_putc((edx >> 16) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        vga_putc((edx >> 24) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        vga_putc((ecx >> 0) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        vga_putc((ecx >> 8) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        vga_putc((ecx >> 16) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        vga_putc((ecx >> 24) & 0xFF, VGA_COLOR_LIGHT_GRAY);
        boot_print("\n");
    } else {
        boot_print("不支持 CPUID 指令\n");
    }
}

// A20 地址线检测
static void enable_a20(void) {
    boot_print("启用 A20 地址线...\n");
    
    // 使用键盘控制器方法启用 A20
    __asm__ __volatile__ (
        "cli\n"
        "1: inb $0x64, %%al\n"
        "testb $0x02, %%al\n"
        "jnz 1b\n"
        "movb $0xAD, %%al\n"
        "outb %%al, $0x64\n"
        "1: inb $0x64, %%al\n"
        "testb $0x02, %%al\n"
        "jnz 1b\n"
        "movb $0xD0, %%al\n"
        "outb %%al, $0x64\n"
        "inb $0x60, %%al\n"
        "push %%eax\n"
        "1: inb $0x64, %%al\n"
        "testb $0x02, %%al\n"
        "jnz 1b\n"
        "movb $0xD1, %%al\n"
        "outb %%al, $0x64\n"
        "pop %%eax\n"
        "orb $0x02, %%al\n"
        "outb %%al, $0x60\n"
        "1: inb $0x64, %%al\n"
        "testb $0x02, %%al\n"
        "jnz 1b\n"
        "movb $0xAE, %%al\n"
        "outb %%al, $0x64\n"
        "sti\n"
        :
        :
        : "eax"
    );
    
    boot_print("A20 地址线已启用\n");
}

// GDT 设置
static struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) gdt_ptr;

static struct {
    u16 limit_low;
    u16 base_low;
    u8 base_mid;
    u8 access;
    u8 granularity;
    u8 base_high;
} __attribute__((packed)) gdt_entries[5];

static void setup_gdt(void) {
    boot_print("设置 GDT...\n");
    
    // 空描述符
    gdt_entries[0].limit_low = 0;
    gdt_entries[0].base_low = 0;
    gdt_entries[0].base_mid = 0;
    gdt_entries[0].access = 0;
    gdt_entries[0].granularity = 0;
    gdt_entries[0].base_high = 0;
    
    // 内核代码段 (0x08)
    gdt_entries[1].limit_low = 0xFFFF;
    gdt_entries[1].base_low = 0;
    gdt_entries[1].base_mid = 0;
    gdt_entries[1].access = 0x9A; // 存在，可读，DPL=0，代码段
    gdt_entries[1].granularity = 0xCF; // 4KB 粒度，32 位
    gdt_entries[1].base_high = 0;
    
    // 内核数据段 (0x10)
    gdt_entries[2].limit_low = 0xFFFF;
    gdt_entries[2].base_low = 0;
    gdt_entries[2].base_mid = 0;
    gdt_entries[2].access = 0x92; // 存在，可写，DPL=0，数据段
    gdt_entries[2].granularity = 0xCF; // 4KB 粒度，32 位
    gdt_entries[2].base_high = 0;
    
    // 用户代码段 (0x18)
    gdt_entries[3].limit_low = 0xFFFF;
    gdt_entries[3].base_low = 0;
    gdt_entries[3].base_mid = 0;
    gdt_entries[3].access = 0xFA; // 存在，可读，DPL=3，代码段
    gdt_entries[3].granularity = 0xCF; // 4KB 粒度，32 位
    gdt_entries[3].base_high = 0;
    
    // 用户数据段 (0x20)
    gdt_entries[4].limit_low = 0xFFFF;
    gdt_entries[4].base_low = 0;
    gdt_entries[4].base_mid = 0;
    gdt_entries[4].access = 0xF2; // 存在，可写，DPL=3，数据段
    gdt_entries[4].granularity = 0xCF; // 4KB 粒度，32 位
    gdt_entries[4].base_high = 0;
    
    // 加载 GDT
    gdt_ptr.limit = sizeof(gdt_entries) - 1;
    gdt_ptr.base = (u32)&gdt_entries;
    
    __asm__ __volatile__ (
        "lgdt %0\n"
        "movw $0x10, %%ax\n"
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        "movw %%ax, %%ss\n"
        "ljmp $0x08, $1f\n"
        "1:\n"
        :
        : "m"(gdt_ptr)
        : "eax"
    );
    
    boot_print("GDT 设置完成\n");
}

// 引导初始化
void boot_init(void) {
    vga_clear();
    boot_print("QiYuanOS 引导加载程序 v1.0\n");
    boot_print("================================\n");
    
    // 检测硬件
    detect_memory();
    detect_cpu();
    
    // 启用 A20 地址线
    enable_a20();
    
    // 设置 GDT
    setup_gdt();
    
    // 设置引导信息
    boot_info.magic = BOOT_MAGIC;
    boot_info.flags = 0;
    boot_info.boot_device = 0x80; // 第一个硬盘
    boot_info.cmdline = 0;
    boot_info.mods_count = 0;
    boot_info.mods_addr = 0;
    
    boot_print("\n引导初始化完成\n");
    boot_delay(1000);
}

// 系统重启
void boot_reboot(void) {
    boot_print("\n系统重启中...\n");
    boot_delay(1000);
    
    // 使用键盘控制器重启
    __asm__ __volatile__ (
        "cli\n"
        "1: inb $0x64, %%al\n"
        "testb $0x02, %%al\n"
        "jnz 1b\n"
        "movb $0xFE, %%al\n"
        "outb %%al, $0x64\n"
        :
        :
        : "eax"
    );
    
    // 如果失败，死循环
    while (1);
}

// 系统关机
void boot_shutdown(void) {
    boot_print("\n系统关机中...\n");
    boot_delay(1000);
    
    // 使用 APM 关机
    __asm__ __volatile__ (
        "movw $0x5301, %%ax\n"
        "xorw %%bx, %%bx\n"
        "int $0x15\n"
        "movw $0x5307, %%ax\n"
        "movw $0x0001, %%bx\n"
        "movw $0x0003, %%cx\n"
        "int $0x15\n"
        :
        :
        : "eax", "ebx", "ecx"
    );
    
    // 如果失败，死循环
    while (1);
}

// 引导主函数
void boot_main(void) {
    // 初始化引导环境
    boot_init();
    
    boot_print("\n准备加载内核...\n");
    boot_delay(1000);
    
    // 调用内核初始化函数
    boot_print("调用内核初始化...\n");
    kernel_init();
    
    // 调用内核主函数
    boot_print("进入内核主循环...\n");
    kernel_main();
    
    // 如果内核返回，重启系统
    boot_print("\n内核已退出，系统重启\n");
    boot_reboot();
}