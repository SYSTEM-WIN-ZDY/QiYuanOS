#ifndef FS_H
#define FS_H

#include <stdint.h>
#include "../kernel/kernel.h"

// 文件系统类型
#define FS_TYPE_FAT32    1
#define FS_TYPE_EXT2     2
#define FS_TYPE_QYFS     3  // QiYuanOS 文件系统

// 文件权限
#define FS_PERM_READ    0x01
#define FS_PERM_WRITE   0x02
#define FS_PERM_EXECUTE 0x04

// 文件类型
#define FS_TYPE_FILE     1
#define FS_TYPE_DIR      2
#define FS_TYPE_LINK     3
#define FS_TYPE_DEVICE   4

// 文件系统最大值
#define FS_MAX_NAME_LEN    255
#define FS_MAX_PATH_LEN    4096
#define FS_MAX_FILE_SIZE   (4ULL * 1024 * 1024 * 1024) // 4GB

// 文件描述符结构
typedef struct {
    int fd;
    char name[FS_MAX_NAME_LEN + 1];
    u32 flags;
    u32 permissions;
    u64 size;
    u64 position;
    void* private_data;
} file_descriptor_t;

// 目录项结构
typedef struct {
    char name[FS_MAX_NAME_LEN + 1];
    u32 inode;
    u8 type;
    u32 permissions;
    u64 size;
    u64 create_time;
    u64 modify_time;
    u64 access_time;
} dir_entry_t;

// 文件系统操作接口
typedef struct {
    int (*mount)(const char* device, const char* mount_point);
    int (*umount)(const char* mount_point);
    int (*open)(const char* path, int flags);
    int (*close)(int fd);
    ssize_t (*read)(int fd, void* buffer, size_t size);
    ssize_t (*write)(int fd, const void* buffer, size_t size);
    int (*seek)(int fd, off_t offset, int whence);
    int (*mkdir)(const char* path, u32 permissions);
    int (*rmdir)(const char* path);
    int (*unlink)(const char* path);
    int (*rename)(const char* old_path, const char* new_path);
    int (*readdir)(int fd, dir_entry_t* entry);
    int (*stat)(const char* path, dir_entry_t* stat);
} fs_operations_t;

// 文件系统注册结构
typedef struct {
    char name[32];
    u32 type;
    fs_operations_t* ops;
} filesystem_t;

// 文件系统初始化
int fs_init(void);
void fs_shutdown(void);
int fs_sync(void);

// 文件系统注册
int fs_register(filesystem_t* fs);
int fs_unregister(const char* name);

// 挂载/卸载
int fs_mount(const char* device, const char* type, const char* mount_point);
int fs_umount(const char* mount_point);

// 文件操作
int fs_open(const char* path, int flags);
int fs_close(int fd);
ssize_t fs_read(int fd, void* buffer, size_t size);
ssize_t fs_write(int fd, const void* buffer, size_t size);
int fs_seek(int fd, off_t offset, int whence);

// 目录操作
int fs_mkdir(const char* path, u32 permissions);
int fs_rmdir(const char* path);
int fs_unlink(const char* path);
int fs_rename(const char* old_path, const char* new_path);
int fs_readdir(int fd, dir_entry_t* entry);
int fs_stat(const char* path, dir_entry_t* stat);

// 路径处理
int fs_normalize_path(const char* path, char* normalized);
int fs_get_parent(const char* path, char* parent);
int fs_get_basename(const char* path, char* basename);

#endif // FS_H