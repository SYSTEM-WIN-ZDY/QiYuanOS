#include "apps.h"
#include "../gui/gui.h"
#include "../fs/fs.h"
#include <stdio.h>

// 示例应用程序：简单的文件管理器
static window_t* main_window = NULL;
static control_t* listbox = NULL;
static control_t* open_button = NULL;
static control_t* delete_button = NULL;
static control_t* new_button = NULL;

// 窗口绘制函数
static void file_manager_paint(window_t* win) {
    // 绘制标题栏
    gui_draw_text(win->rect.x + 5, win->rect.y + 5, "文件管理器", (color_t)COLOR_WHITE);
    
    // 绘制状态栏
    rect_t status_rect = {win->rect.x, win->rect.y + win->rect.height - 20, win->rect.width, 20};
    gui_clear_rect(&status_rect, (color_t)COLOR_LIGHT_GRAY);
    gui_draw_text(status_rect.x + 5, status_rect.y + 5, "就绪", (color_t)COLOR_BLACK);
}

// 窗口事件处理函数
static void file_manager_event(window_t* win, event_t* event) {
    switch (event->type) {
        case EVENT_WINDOW_CLOSE:
            window_destroy(win);
            break;
        
        case EVENT_MOUSE_CLICK:
            // 处理鼠标点击
            break;
        
        case EVENT_KEY_DOWN:
            // 处理键盘事件
            break;
    }
}

// 按钮点击处理函数
static void open_button_click(control_t* ctrl) {
    printf("打开文件按钮被点击\n");
    // 这里应该实现文件打开逻辑
}

static void delete_button_click(control_t* ctrl) {
    printf("删除文件按钮被点击\n");
    // 这里应该实现文件删除逻辑
}

static void new_button_click(control_t* ctrl) {
    printf("新建文件按钮被点击\n");
    // 这里应该实现文件创建逻辑
}

// 刷新文件列表
static void refresh_file_list(void) {
    if (!listbox) return;
    
    // 清空列表
    // 这里应该实现清空列表的逻辑
    
    // 读取当前目录
    int fd = fs_open("/", 0);
    if (fd >= 0) {
        dir_entry_t entry;
        while (fs_readdir(fd, &entry) > 0) {
            printf("文件: %s\n", entry.name);
            // 这里应该将文件添加到列表框中
        }
        fs_close(fd);
    }
}

// 启动文件管理器
void start_file_manager(void) {
    printf("启动文件管理器...\n");
    
    // 创建主窗口
    main_window = window_create("文件管理器", 100, 100, 600, 400, 
                               WINDOW_STYLE_BORDER | WINDOW_STYLE_TITLEBAR | 
                               WINDOW_STYLE_CLOSABLE | WINDOW_STYLE_RESIZABLE);
    if (!main_window) {
        printf("无法创建窗口\n");
        return;
    }
    
    // 加载并设置窗口图标
    u8* icon_data;
    int icon_width, icon_height;
    if (gui_load_icon("QiYuan OS.png", &icon_data, &icon_width, &icon_height) == 0) {
        main_window->icon_data = icon_data;
        main_window->icon_width = icon_width;
        main_window->icon_height = icon_height;
    }
    
    main_window->on_paint = file_manager_paint;
    main_window->on_event = file_manager_event;
    
    // 创建文件列表框
    listbox = control_create(main_window, CONTROL_TYPE_LISTBOX, 10, 40, 400, 300);
    if (listbox) {
        control_set_text(listbox, "文件列表");
        control_set_colors(listbox, (color_t)COLOR_BLACK, (color_t)COLOR_WHITE);
    }
    
    // 创建按钮
    open_button = control_create(main_window, CONTROL_TYPE_BUTTON, 420, 40, 80, 30);
    if (open_button) {
        control_set_text(open_button, "打开");
        control_set_colors(open_button, (color_t)COLOR_BLACK, (color_t)COLOR_LIGHT_GRAY);
        open_button->on_click = open_button_click;
    }
    
    delete_button = control_create(main_window, CONTROL_TYPE_BUTTON, 420, 80, 80, 30);
    if (delete_button) {
        control_set_text(delete_button, "删除");
        control_set_colors(delete_button, (color_t)COLOR_BLACK, (color_t)COLOR_LIGHT_GRAY);
        delete_button->on_click = delete_button_click;
    }
    
    new_button = control_create(main_window, CONTROL_TYPE_BUTTON, 420, 120, 80, 30);
    if (new_button) {
        control_set_text(new_button, "新建");
        control_set_colors(new_button, (color_t)COLOR_BLACK, (color_t)COLOR_LIGHT_GRAY);
        new_button->on_click = new_button_click;
    }
    
    // 显示窗口
    window_show(main_window);
    window_focus(main_window);
    
    // 刷新文件列表
    refresh_file_list();
    
    printf("文件管理器启动完成\n");
}

// 示例应用程序：简单的文本编辑器
static window_t* editor_window = NULL;
static control_t* text_area = NULL;
static control_t* save_button = NULL;
static control_t* load_button = NULL;

// 文本编辑器绘制函数
static void text_editor_paint(window_t* win) {
    // 绘制标题栏
    gui_draw_text(win->rect.x + 5, win->rect.y + 5, "文本编辑器", (color_t)COLOR_WHITE);
}

// 文本编辑器事件处理函数
static void text_editor_event(window_t* win, event_t* event) {
    switch (event->type) {
        case EVENT_WINDOW_CLOSE:
            window_destroy(win);
            break;
    }
}

// 保存按钮点击处理
static void save_button_click(control_t* ctrl) {
    printf("保存按钮被点击\n");
    // 这里应该实现文件保存逻辑
}

// 加载按钮点击处理
static void load_button_click(control_t* ctrl) {
    printf("加载按钮被点击\n");
    // 这里应该实现文件加载逻辑
}

// 启动文本编辑器
void start_text_editor(void) {
    printf("启动文本编辑器...\n");
    
    // 创建主窗口
    editor_window = window_create("文本编辑器", 150, 150, 500, 350,
                                 WINDOW_STYLE_BORDER | WINDOW_STYLE_TITLEBAR |
                                 WINDOW_STYLE_CLOSABLE | WINDOW_STYLE_RESIZABLE);
    if (!editor_window) {
        printf("无法创建窗口\n");
        return;
    }
    
    // 加载并设置窗口图标
    u8* icon_data;
    int icon_width, icon_height;
    if (gui_load_icon("QiYuan OS.png", &icon_data, &icon_width, &icon_height) == 0) {
        editor_window->icon_data = icon_data;
        editor_window->icon_width = icon_width;
        editor_window->icon_height = icon_height;
    }
    
    editor_window->on_paint = text_editor_paint;
    editor_window->on_event = text_editor_event;
    
    // 创建文本区域
    text_area = control_create(editor_window, CONTROL_TYPE_TEXTBOX, 10, 40, 460, 250);
    if (text_area) {
        control_set_text(text_area, "欢迎使用 QiYuanOS 文本编辑器！\n\n在这里输入文本...");
        control_set_colors(text_area, (color_t)COLOR_BLACK, (color_t)COLOR_WHITE);
    }
    
    // 创建按钮
    save_button = control_create(editor_window, CONTROL_TYPE_BUTTON, 10, 300, 80, 30);
    if (save_button) {
        control_set_text(save_button, "保存");
        control_set_colors(save_button, (color_t)COLOR_BLACK, (color_t)COLOR_LIGHT_GRAY);
        save_button->on_click = save_button_click;
    }
    
    load_button = control_create(editor_window, CONTROL_TYPE_BUTTON, 100, 300, 80, 30);
    if (load_button) {
        control_set_text(load_button, "加载");
        control_set_colors(load_button, (color_t)COLOR_BLACK, (color_t)COLOR_LIGHT_GRAY);
        load_button->on_click = load_button_click;
    }
    
    // 显示窗口
    window_show(editor_window);
    window_focus(editor_window);
    
    printf("文本编辑器启动完成\n");
}

// 示例应用程序：系统信息查看器
static window_t* info_window = NULL;

// 系统信息绘制函数
static void system_info_paint(window_t* win) {
    // 绘制标题栏
    gui_draw_text(win->rect.x + 5, win->rect.y + 5, "系统信息", (color_t)COLOR_WHITE);
    
    // 绘制系统信息
    int y = win->client_rect.y + 10;
    gui_draw_text(win->client_rect.x + 10, y, "QiYuanOS 系统信息", (color_t)COLOR_BLACK);
    y += 25;
    
    gui_draw_text(win->client_rect.x + 10, y, "版本: 1.0.0", (color_t)COLOR_BLACK);
    y += 20;
    
    gui_draw_text(win->client_rect.x + 10, y, "内核: QiYuanOS Kernel", (color_t)COLOR_BLACK);
    y += 20;
    
    gui_draw_text(win->client_rect.x + 10, y, "文件系统: QYFS", (color_t)COLOR_BLACK);
    y += 20;
    
    gui_draw_text(win->client_rect.x + 10, y, "图形界面: QiYuanGUI", (color_t)COLOR_BLACK);
    y += 20;
    
    gui_draw_text(win->client_rect.x + 10, y, "屏幕分辨率: 1024x768", (color_t)COLOR_BLACK);
    y += 20;
    
    gui_draw_text(win->client_rect.x + 10, y, "内存: 64MB", (color_t)COLOR_BLACK);
    y += 20;
    
    gui_draw_text(win->client_rect.x + 10, y, "构建时间: " __DATE__ " " __TIME__, (color_t)COLOR_BLACK);
}

// 系统信息事件处理函数
static void system_info_event(window_t* win, event_t* event) {
    switch (event->type) {
        case EVENT_WINDOW_CLOSE:
            window_destroy(win);
            break;
    }
}

// 启动系统信息查看器
void start_system_info(void) {
    printf("启动系统信息查看器...\n");
    
    // 创建主窗口
    info_window = window_create("系统信息", 200, 200, 350, 250,
                              WINDOW_STYLE_BORDER | WINDOW_STYLE_TITLEBAR |
                              WINDOW_STYLE_CLOSABLE);
    if (!info_window) {
        printf("无法创建窗口\n");
        return;
    }
    
    // 加载并设置窗口图标
    u8* icon_data;
    int icon_width, icon_height;
    if (gui_load_icon("QiYuan OS.png", &icon_data, &icon_width, &icon_height) == 0) {
        info_window->icon_data = icon_data;
        info_window->icon_width = icon_width;
        info_window->icon_height = icon_height;
    }
    
    info_window->on_paint = system_info_paint;
    info_window->on_event = system_info_event;
    
    // 显示窗口
    window_show(info_window);
    window_focus(info_window);
    
    printf("系统信息查看器启动完成\n");
}

// 桌面应用程序启动器
void launch_desktop_apps(void) {
    printf("启动桌面应用程序...\n");
    
    // 等待GUI系统完全初始化
    for (int i = 0; i < 10; i++) {
        sleep(100);
    }
    
    // 启动系统信息查看器
    start_system_info();
    
    // 等待一下再启动其他应用
    sleep(500);
    
    // 启动文件管理器
    start_file_manager();
    
    // 等待一下再启动文本编辑器
    sleep(500);
    
    // 启动文本编辑器
    start_text_editor();
    
    printf("桌面应用程序启动完成\n");
}