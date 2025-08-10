#include "kernel.h"
#include <stdio.h>
#include "../fs/fs.h"
#include "../gui/gui.h"
#include "../apps/apps.h"

// 内核全局状态
static int kernel_running = 1;
static u32 kernel_tick = 0;

// 内核初始化
void kernel_init(void) {
    printf("[%s] 初始化内核版本 %s\n", KERNEL_NAME, KERNEL_VERSION);
    
    // 初始化内存管理
    printf("初始化内存管理...\n");
    
    // 初始化中断系统
    printf("初始化中断系统...\n");
    interrupt_init();
    
    // 初始化文件系统
    printf("初始化文件系统...\n");
    fs_init();
    
    // 初始化GUI系统
    printf("初始化图形界面...\n");
    gui_init();
    
    printf("内核初始化完成\n");
    
    // 启动桌面应用程序
    printf("启动桌面应用程序...\n");
    launch_desktop_apps();
}

// 内核主循环
void kernel_main(void) {
    printf("[%s] 内核主循环启动\n", KERNEL_NAME);
    
    while (kernel_running) {
        kernel_tick++;
        
        // 进程调度
        schedule();
        
        // 处理中断
        // interrupt_process();
        
        // 更新GUI
        gui_update();
        
        // 简单的时间片轮转
        sleep(10);
    }
    
    printf("[%s] 内核主循环结束\n", KERNEL_NAME);
}

// 内核关闭
void kernel_shutdown(void) {
    printf("[%s] 系统关闭中...\n", KERNEL_NAME);
    
    // 关闭GUI
    gui_shutdown();
    
    // 同步文件系统
    fs_sync();
    
    // 关闭中断
    // interrupt_disable();
    
    kernel_running = 0;
    printf("[%s] 系统已安全关闭\n", KERNEL_NAME);
}

// 简单的内存分配实现
void* kmalloc(size_t size) {
    // 这里应该实现真正的内存分配器
    // 暂时使用简单的实现
    static u8 memory_pool[1024 * 1024]; // 1MB 内存池
    static size_t pool_offset = 0;
    
    if (pool_offset + size > sizeof(memory_pool)) {
        return NULL; // 内存不足
    }
    
    void* ptr = &memory_pool[pool_offset];
    pool_offset += size;
    return ptr;
}

void kfree(void* ptr) {
    // 简单实现，不释放内存
    // 实际应该实现真正的内存释放
}

// 进程调度实现
int create_process(const char* name, void (*entry)(void)) {
    printf("创建进程: %s\n", name);
    // 这里应该实现真正的进程创建
    return 0;
}

void schedule(void) {
    // 简单的调度器实现
    // 实际应该实现时间片轮转或优先级调度
}

void sleep(int ms) {
    // 简单的睡眠实现
    // 实际应该使用定时器中断
}

// 中断处理实现
void interrupt_init(void) {
    printf("中断系统初始化\n");
}

void interrupt_handler(int irq) {
    printf("处理中断: %d\n", irq);
}