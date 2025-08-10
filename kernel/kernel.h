#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

// 内核版本信息
#define KERNEL_VERSION "1.0.0"
#define KERNEL_NAME "QiYuanOS"

// 基本数据类型定义
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

// 系统调用号
#define SYS_READ    0
#define SYS_WRITE   1
#define SYS_OPEN    2
#define SYS_CLOSE   3
#define SYS_SEEK    4
#define SYS_MMAP    5
#define SYS_MUNMAP  6
#define SYS_FORK    7
#define SYS_EXEC    8
#define SYS_EXIT    9
#define SYS_WAIT   10

// 内核初始化函数
void kernel_init(void);
void kernel_main(void);
void kernel_shutdown(void);

// 内存管理函数
void* kmalloc(size_t size);
void kfree(void* ptr);

// 进程管理函数
int create_process(const char* name, void (*entry)(void));
void schedule(void);
void sleep(int ms);

// 中断处理
void interrupt_init(void);
void interrupt_handler(int irq);

#endif // KERNEL_H