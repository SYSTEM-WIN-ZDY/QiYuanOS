#include "fs.h"
#include <string.h>
#include <stdio.h>

// 文件系统全局状态
static filesystem_t* registered_fs[16];
static int fs_count = 0;
static int fs_initialized = 0;

// QiYuanOS 文件系统实现
static int qyfs_mount(const char* device, const char* mount_point) {
    printf("挂载 QYFS 文件系统: %s -> %s\n", device, mount_point);
    return 0;
}

static int qyfs_umount(const char* mount_point) {
    printf("卸载 QYFS 文件系统: %s\n", mount_point);
    return 0;
}

static int qyfs_open(const char* path, int flags) {
    printf("打开文件: %s (flags: %d)\n", path, flags);
    return 1; // 返回文件描述符
}

static int qyfs_close(int fd) {
    printf("关闭文件描述符: %d\n", fd);
    return 0;
}

static ssize_t qyfs_read(int fd, void* buffer, size_t size) {
    printf("读取文件: fd=%d, size=%zu\n", fd, size);
    // 模拟读取一些数据
    memset(buffer, 0, size);
    strcpy(buffer, "Hello from QiYuanOS File System!");
    return strlen(buffer);
}

static ssize_t qyfs_write(int fd, const void* buffer, size_t size) {
    printf("写入文件: fd=%d, size=%zu\n", fd, size);
    return size;
}

static int qyfs_seek(int fd, off_t offset, int whence) {
    printf("文件定位: fd=%d, offset=%ld, whence=%d\n", fd, offset, whence);
    return 0;
}

static int qyfs_mkdir(const char* path, u32 permissions) {
    printf("创建目录: %s (权限: %o)\n", path, permissions);
    return 0;
}

static int qyfs_rmdir(const char* path) {
    printf("删除目录: %s\n", path);
    return 0;
}

static int qyfs_unlink(const char* path) {
    printf("删除文件: %s\n", path);
    return 0;
}

static int qyfs_rename(const char* old_path, const char* new_path) {
    printf("重命名: %s -> %s\n", old_path, new_path);
    return 0;
}

static int qyfs_readdir(int fd, dir_entry_t* entry) {
    printf("读取目录: fd=%d\n", fd);
    // 模拟返回一些目录项
    static int entry_count = 0;
    if (entry_count == 0) {
        strcpy(entry->name, ".");
        entry->type = FS_TYPE_DIR;
        entry_count++;
        return 1;
    } else if (entry_count == 1) {
        strcpy(entry->name, "..");
        entry->type = FS_TYPE_DIR;
        entry_count++;
        return 1;
    } else if (entry_count == 2) {
        strcpy(entry->name, "test.txt");
        entry->type = FS_TYPE_FILE;
        entry->size = 1024;
        entry_count++;
        return 1;
    }
    return 0; // 没有更多目录项
}

static int qyfs_stat(const char* path, dir_entry_t* stat) {
    printf("获取文件状态: %s\n", path);
    strcpy(stat->name, "test.txt");
    stat->type = FS_TYPE_FILE;
    stat->size = 1024;
    stat->permissions = FS_PERM_READ | FS_PERM_WRITE;
    return 0;
}

// QYFS 操作接口
static fs_operations_t qyfs_ops = {
    .mount = qyfs_mount,
    .umount = qyfs_umount,
    .open = qyfs_open,
    .close = qyfs_close,
    .read = qyfs_read,
    .write = qyfs_write,
    .seek = qyfs_seek,
    .mkdir = qyfs_mkdir,
    .rmdir = qyfs_rmdir,
    .unlink = qyfs_unlink,
    .rename = qyfs_rename,
    .readdir = qyfs_readdir,
    .stat = qyfs_stat
};

// QYFS 文件系统定义
static filesystem_t qyfs = {
    .name = "qyfs",
    .type = FS_TYPE_QYFS,
    .ops = &qyfs_ops
};

// 文件系统初始化
int fs_init(void) {
    if (fs_initialized) {
        return 0;
    }
    
    printf("初始化文件系统...\n");
    
    // 注册 QYFS 文件系统
    fs_register(&qyfs);
    
    // 挂载根文件系统
    fs_mount("/dev/sda1", "qyfs", "/");
    
    fs_initialized = 1;
    printf("文件系统初始化完成\n");
    return 0;
}

void fs_shutdown(void) {
    if (!fs_initialized) {
        return;
    }
    
    printf("关闭文件系统...\n");
    fs_sync();
    fs_initialized = 0;
}

int fs_sync(void) {
    printf("同步文件系统...\n");
    return 0;
}

// 文件系统注册
int fs_register(filesystem_t* fs) {
    if (fs_count >= 16) {
        return -1; // 太多文件系统
    }
    
    registered_fs[fs_count++] = fs;
    printf("注册文件系统: %s\n", fs->name);
    return 0;
}

int fs_unregister(const char* name) {
    for (int i = 0; i < fs_count; i++) {
        if (strcmp(registered_fs[i]->name, name) == 0) {
            // 移动数组元素
            for (int j = i; j < fs_count - 1; j++) {
                registered_fs[j] = registered_fs[j + 1];
            }
            fs_count--;
            printf("注销文件系统: %s\n", name);
            return 0;
        }
    }
    return -1; // 未找到
}

// 挂载/卸载
int fs_mount(const char* device, const char* type, const char* mount_point) {
    for (int i = 0; i < fs_count; i++) {
        if (strcmp(registered_fs[i]->name, type) == 0) {
            return registered_fs[i]->ops->mount(device, mount_point);
        }
    }
    printf("未找到文件系统类型: %s\n", type);
    return -1;
}

int fs_umount(const char* mount_point) {
    // 简化实现，假设只有一个挂载点
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->umount) {
            return registered_fs[i]->ops->umount(mount_point);
        }
    }
    return -1;
}

// 文件操作
int fs_open(const char* path, int flags) {
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->open) {
            return registered_fs[i]->ops->open(path, flags);
        }
    }
    return -1;
}

int fs_close(int fd) {
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->close) {
            return registered_fs[i]->ops->close(fd);
        }
    }
    return -1;
}

ssize_t fs_read(int fd, void* buffer, size_t size) {
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->read) {
            return registered_fs[i]->ops->read(fd, buffer, size);
        }
    }
    return -1;
}

ssize_t fs_write(int fd, const void* buffer, size_t size) {
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->write) {
            return registered_fs[i]->ops->write(fd, buffer, size);
        }
    }
    return -1;
}

int fs_seek(int fd, off_t offset, int whence) {
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->seek) {
            return registered_fs[i]->ops->seek(fd, offset, whence);
        }
    }
    return -1;
}

// 目录操作
int fs_mkdir(const char* path, u32 permissions) {
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->mkdir) {
            return registered_fs[i]->ops->mkdir(path, permissions);
        }
    }
    return -1;
}

int fs_rmdir(const char* path) {
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->rmdir) {
            return registered_fs[i]->ops->rmdir(path);
        }
    }
    return -1;
}

int fs_unlink(const char* path) {
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->unlink) {
            return registered_fs[i]->ops->unlink(path);
        }
    }
    return -1;
}

int fs_rename(const char* old_path, const char* new_path) {
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->rename) {
            return registered_fs[i]->ops->rename(old_path, new_path);
        }
    }
    return -1;
}

int fs_readdir(int fd, dir_entry_t* entry) {
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->readdir) {
            return registered_fs[i]->ops->readdir(fd, entry);
        }
    }
    return -1;
}

int fs_stat(const char* path, dir_entry_t* stat) {
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i]->ops->stat) {
            return registered_fs[i]->ops->stat(path, stat);
        }
    }
    return -1;
}

// 路径处理
int fs_normalize_path(const char* path, char* normalized) {
    // 简单的路径标准化实现
    strcpy(normalized, path);
    return 0;
}

int fs_get_parent(const char* path, char* parent) {
    // 获取父目录
    strcpy(parent, path);
    char* last_slash = strrchr(parent, '/');
    if (last_slash) {
        *last_slash = '\0';
    } else {
        strcpy(parent, "/");
    }
    return 0;
}

int fs_get_basename(const char* path, char* basename) {
    // 获取文件名
    const char* last_slash = strrchr(path, '/');
    if (last_slash) {
        strcpy(basename, last_slash + 1);
    } else {
        strcpy(basename, path);
    }
    return 0;
}