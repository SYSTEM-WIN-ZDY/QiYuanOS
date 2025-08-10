/*
 * QiYuanOS 链接器脚本
 * 定义内核的内存布局
 */

OUTPUT_FORMAT("elf32-i386")
ENTRY(boot_main)

SECTIONS
{
    /* 内核加载地址: 1MB */
    . = 0x100000;
    
    /* 内核开始标记 */
    .kernel_start : {
        *(.kernel_start)
    }
    
    /* 代码段 */
    .text : {
        *(.text)
        *(.text.*)
    }
    
    /* 只读数据段 */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* 数据段 */
    .data : {
        *(.data)
        *(.data.*)
    }
    
    /* BSS段 */
    .bss : {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    }
    
    /* 栈段 */
    .stack : {
        __stack_start = .;
        . += 64K;  /* 64KB 栈空间 */
        __stack_end = .;
    }
    
    /* 内核结束标记 */
    .kernel_end : {
        *(.kernel_end)
    }
    
    /* 丢弃不需要的段 */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}

/* 定义符号供内核使用 */
__kernel_start = 0x100000;
__kernel_size = __kernel_end - __kernel_start;
__bss_size = __bss_end - __bss_start;
__stack_size = __stack_end - __stack_start;