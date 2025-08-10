#!/bin/bash

# QiYuanOS 启动脚本
# 用于快速构建和运行操作系统

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查依赖
check_dependencies() {
    print_info "检查构建依赖..."
    
    # 检查 GCC
    if ! command -v gcc &> /dev/null; then
        print_error "GCC 未安装"
        exit 1
    fi
    
    # 检查 NASM
    if ! command -v nasm &> /dev/null; then
        print_error "NASM 未安装"
        exit 1
    fi
    
    # 检查 QEMU
    if ! command -v qemu-system-i386 &> /dev/null; then
        print_warning "QEMU 未安装，无法运行模拟器"
        QEMU_AVAILABLE=false
    else
        QEMU_AVAILABLE=true
    fi
    
    # 检查 GRUB
    if ! command -v grub-mkrescue &> /dev/null && ! command -v grub2-mkrescue &> /dev/null; then
        print_warning "GRUB 未安装，无法创建ISO镜像"
        GRUB_AVAILABLE=false
    else
        GRUB_AVAILABLE=true
    fi
    
    print_success "依赖检查完成"
}

# 清理构建文件
clean_build() {
    print_info "清理构建文件..."
    make clean
    print_success "清理完成"
}

# 构建内核
build_kernel() {
    print_info "构建内核..."
    make all
    if [ $? -eq 0 ]; then
        print_success "内核构建完成"
    else
        print_error "内核构建失败"
        exit 1
    fi
}

# 创建ISO镜像
create_iso() {
    if [ "$GRUB_AVAILABLE" = false ]; then
        print_warning "GRUB 不可用，跳过ISO创建"
        return
    fi
    
    print_info "创建ISO镜像..."
    make iso
    if [ $? -eq 0 ]; then
        print_success "ISO镜像创建完成"
    else
        print_warning "ISO镜像创建失败"
    fi
}

# 运行QEMU
run_qemu() {
    if [ "$QEMU_AVAILABLE" = false ]; then
        print_error "QEMU 不可用，无法运行模拟器"
        exit 1
    fi
    
    print_info "启动QEMU模拟器..."
    make run
}

# 运行QEMU (ISO模式)
run_qemu_iso() {
    if [ "$QEMU_AVAILABLE" = false ]; then
        print_error "QEMU 不可用，无法运行模拟器"
        exit 1
    fi
    
    if [ "$GRUB_AVAILABLE" = false ]; then
        print_error "GRUB 不可用，无法创建ISO镜像"
        exit 1
    fi
    
    print_info "从ISO启动QEMU模拟器..."
    make run-iso
}

# 调试模式
debug_qemu() {
    if [ "$QEMU_AVAILABLE" = false ]; then
        print_error "QEMU 不可用，无法运行模拟器"
        exit 1
    fi
    
    print_info "启动QEMU调试模式..."
    print_info "使用 GDB 连接到 localhost:1234 进行调试"
    make debug
}

# 显示帮助
show_help() {
    echo "QiYuanOS 启动脚本"
    echo "=================="
    echo ""
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  build      - 构建内核"
    echo "  clean      - 清理构建文件"
    echo "  iso        - 创建ISO镜像"
    echo "  run        - 在QEMU中运行内核"
    echo "  run-iso    - 在QEMU中从ISO启动"
    echo "  debug      - 在QEMU中调试内核"
    echo "  all        - 构建并运行 (默认)"
    echo "  help       - 显示此帮助信息"
    echo ""
    echo "示例:"
    echo "  $0 build && $0 run"
    echo "  $0 all"
    echo "  $0 debug"
}

# 主函数
main() {
    case "${1:-all}" in
        "build")
            check_dependencies
            build_kernel
            ;;
        "clean")
            clean_build
            ;;
        "iso")
            check_dependencies
            build_kernel
            create_iso
            ;;
        "run")
            check_dependencies
            build_kernel
            run_qemu
            ;;
        "run-iso")
            check_dependencies
            build_kernel
            create_iso
            run_qemu_iso
            ;;
        "debug")
            check_dependencies
            build_kernel
            debug_qemu
            ;;
        "all")
            check_dependencies
            build_kernel
            create_iso
            if [ "$QEMU_AVAILABLE" = true ]; then
                run_qemu
            else
                print_success "构建完成，但QEMU不可用"
            fi
            ;;
        "help"|"-h"|"--help")
            show_help
            ;;
        *)
            print_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
}

# 运行主函数
main "$@"