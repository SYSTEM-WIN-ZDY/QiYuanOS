#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

// 引导信息结构
typedef struct {
    uint32_t magic;          // 引导魔术字
    uint32_t flags;          // 引导标志
    uint32_t mem_lower;      // 低内存大小 (KB)
    uint32_t mem_upper;      // 高内存大小 (KB)
    uint32_t boot_device;    // 引导设备
    uint32_t cmdline;        // 命令行指针
    uint32_t mods_count;     // 模块数量
    uint32_t mods_addr;      // 模块地址
    uint32_t syms[4];        // 符号表信息
    uint32_t mmap_length;    // 内存映射长度
    uint32_t mmap_addr;      // 内存映射地址
    uint32_t drives_length;  // 驱动器长度
    uint32_t drives_addr;    // 驱动器地址
    uint32_t config_table;   // 配置表
    uint32_t boot_loader_name; // 引导加载程序名称
    uint32_t apm_table;      // APM 表
    uint32_t vbe_control_info; // VBE 控制信息
    uint32_t vbe_mode_info;  // VBE 模式信息
    uint32_t vbe_mode;       // VBE 模式
    uint32_t vbe_interface_seg; // VBE 接口段
    uint32_t vbe_interface_off; // VBE 接口偏移
    uint32_t vbe_interface_len; // VBE 接口长度
} boot_info_t;

// 内存映射条目
typedef struct {
    uint32_t size;           // 条目大小
    uint64_t base_addr;      // 基地址
    uint64_t length;         // 长度
    uint32_t type;           // 类型
} mmap_entry_t;

// 内存类型
#define MMAP_TYPE_AVAILABLE   1
#define MMAP_TYPE_RESERVED   2
#define MMAP_TYPE_ACPI       3
#define MMAP_TYPE_NVS        4
#define MMAP_TYPE_BADRAM     5

// 引导魔术字
#define BOOT_MAGIC            0x2BADB002

// 引导标志
#define BOOT_FLAG_MEM_INFO    0x00000001
#define BOOT_FLAG_BOOT_DEV    0x00000002
#define BOOT_FLAG_CMDLINE     0x00000004
#define BOOT_FLAG_MODS        0x00000008
#define BOOT_FLAG_AOUT        0x00000010
#define BOOT_FLAG_ELF         0x00000020
#define BOOT_FLAG_MMAP        0x00000040
#define BOOT_FLAG_DRIVES      0x00000080
#define BOOT_FLAG_CONFIG      0x00000100
#define BOOT_FLAG_VBE         0x00000200
#define BOOT_FLAG_FB          0x00000400

// 引导函数声明
void boot_main(void);
void boot_init(void);
void boot_print(const char* str);
void boot_print_hex(uint32_t value);
void boot_print_dec(uint32_t value);
void boot_delay(uint32_t ms);
void boot_reboot(void);
void boot_shutdown(void);

// 内核入口点声明
void kernel_main(void);
void kernel_init(void);

// 内核映像信息
#define KERNEL_LOAD_ADDR      0x100000  // 1MB
#define KERNEL_STACK_SIZE     0x10000   // 64KB

#endif // BOOT_H