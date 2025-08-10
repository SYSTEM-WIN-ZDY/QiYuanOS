#include "gui.h"
#include <string.h>
#include <stdio.h>

// GUI 全局状态
static desktop_t desktop;
static int gui_initialized = 0;
static int screen_width = 1024;
static int screen_height = 768;
static u32* framebuffer = NULL;

// 简单的字体数据 (8x8 像素)
static const u8 font_8x8[95][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 空格
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, // !
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // "
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}, // #
    // ... 更多字符
};

// 颜色转换函数
static u32 color_to_u32(color_t color) {
    return (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}

// GUI 初始化
int gui_init(void) {
    if (gui_initialized) {
        return 0;
    }
    
    printf("初始化图形界面系统...\n");
    
    // 分配帧缓冲区
    framebuffer = kmalloc(screen_width * screen_height * 4);
    if (!framebuffer) {
        printf("无法分配帧缓冲区\n");
        return -1;
    }
    
    // 初始化桌面
    memset(&desktop, 0, sizeof(desktop));
    desktop.desktop_rect.x = 0;
    desktop.desktop_rect.y = 0;
    desktop.desktop_rect.width = screen_width;
    desktop.desktop_rect.height = screen_height;
    desktop.desktop_color = (color_t)COLOR_LIGHT_GRAY;
    desktop.show_taskbar = 1;
    desktop.show_icons = 1;
    
    // 创建桌面窗口
    desktop.desktop_window = window_create("Desktop", 0, 0, screen_width, screen_height, 0);
    desktop.desktop_window->background_color = desktop.desktop_color;
    
    // 清屏
    gui_clear_rect(&desktop.desktop_rect, desktop.desktop_color);
    
    gui_initialized = 1;
    printf("图形界面系统初始化完成\n");
    return 0;
}

void gui_shutdown(void) {
    if (!gui_initialized) {
        return;
    }
    
    printf("关闭图形界面系统...\n");
    
    // 销毁所有窗口
    window_t* win = desktop.windows;
    while (win) {
        window_t* next = win->next;
        window_destroy(win);
        win = next;
    }
    
    // 释放帧缓冲区
    if (framebuffer) {
        kfree(framebuffer);
        framebuffer = NULL;
    }
    
    gui_initialized = 0;
}

void gui_update(void) {
    if (!gui_initialized) {
        return;
    }
    
    // 重绘桌面
    if (desktop.desktop_window) {
        window_invalidate(desktop.desktop_window);
    }
    
    // 重绘所有窗口
    window_t* win = desktop.windows;
    while (win) {
        if (win->state != WINDOW_STATE_HIDDEN) {
            window_invalidate(win);
        }
        win = win->next;
    }
}

// 窗口管理
window_t* window_create(const char* title, int x, int y, int width, int height, u32 style) {
    window_t* win = kmalloc(sizeof(window_t));
    if (!win) {
        return NULL;
    }
    
    memset(win, 0, sizeof(window_t));
    strncpy(win->title, title, sizeof(win->title) - 1);
    win->rect.x = x;
    win->rect.y = y;
    win->rect.width = width;
    win->rect.height = height;
    win->state = WINDOW_STATE_NORMAL;
    win->style = style;
    win->background_color = (color_t)COLOR_WHITE;
    win->title_color = (color_t)COLOR_BLACK;
    win->border_color = (color_t)COLOR_DARK_GRAY;
    
    // 计算客户区域
    win->client_rect.x = win->rect.x + 2;
    win->client_rect.y = win->rect.y + (style & WINDOW_STYLE_TITLEBAR ? 24 : 2);
    win->client_rect.width = win->rect.width - 4;
    win->client_rect.height = win->rect.height - (style & WINDOW_STYLE_TITLEBAR ? 26 : 4);
    
    // 添加到窗口列表
    win->next = desktop.windows;
    desktop.windows = win;
    
    printf("创建窗口: %s (%d, %d, %d, %d)\n", title, x, y, width, height);
    return win;
}

void window_destroy(window_t* win) {
    if (!win) {
        return;
    }
    
    printf("销毁窗口: %s\n", win->title);
    
    // 从窗口列表中移除
    if (desktop.windows == win) {
        desktop.windows = win->next;
    } else {
        window_t* prev = desktop.windows;
        while (prev && prev->next != win) {
            prev = prev->next;
        }
        if (prev) {
            prev->next = win->next;
        }
    }
    
    // 销毁子控件
    control_t* ctrl = win->children;
    while (ctrl) {
        control_t* next = ctrl->next;
        control_destroy(ctrl);
        ctrl = next;
    }
    
    kfree(win);
}

void window_show(window_t* win) {
    if (win) {
        win->state = WINDOW_STATE_NORMAL;
        window_invalidate(win);
    }
}

void window_hide(window_t* win) {
    if (win) {
        win->state = WINDOW_STATE_HIDDEN;
        // 重绘被遮挡的窗口
        gui_update();
    }
}

void window_move(window_t* win, int x, int y) {
    if (win) {
        win->rect.x = x;
        win->rect.y = y;
        win->client_rect.x = x + 2;
        win->client_rect.y = y + (win->style & WINDOW_STYLE_TITLEBAR ? 24 : 2);
        window_invalidate(win);
    }
}

void window_resize(window_t* win, int width, int height) {
    if (win) {
        win->rect.width = width;
        win->rect.height = height;
        win->client_rect.width = width - 4;
        win->client_rect.height = height - (win->style & WINDOW_STYLE_TITLEBAR ? 26 : 4);
        window_invalidate(win);
    }
}

void window_focus(window_t* win) {
    if (win && desktop.focused_window != win) {
        if (desktop.focused_window) {
            desktop.focused_window->focused = 0;
            window_invalidate(desktop.focused_window);
        }
        desktop.focused_window = win;
        win->focused = 1;
        window_invalidate(win);
    }
}

void window_blur(window_t* win) {
    if (win && desktop.focused_window == win) {
        desktop.focused_window = NULL;
        win->focused = 0;
        window_invalidate(win);
    }
}

void window_set_title(window_t* win, const char* title) {
    if (win) {
        strncpy(win->title, title, sizeof(win->title) - 1);
        window_invalidate(win);
    }
}

void window_set_background(window_t* win, color_t color) {
    if (win) {
        win->background_color = color;
        window_invalidate(win);
    }
}

void window_invalidate(window_t* win) {
    if (!win || win->state == WINDOW_STATE_HIDDEN) {
        return;
    }
    
    // 绘制窗口背景
    gui_clear_rect(&win->rect, win->background_color);
    
    // 绘制边框
    if (win->style & WINDOW_STYLE_BORDER) {
        gui_draw_rect(&win->rect, win->border_color);
    }
    
    // 绘制标题栏
    if (win->style & WINDOW_STYLE_TITLEBAR) {
        rect_t title_rect = {win->rect.x, win->rect.y, win->rect.width, 24};
        color_t title_bg = win->focused ? (color_t){70, 130, 180, 255} : (color_t){192, 192, 192, 255};
        gui_clear_rect(&title_rect, title_bg);
        
        // 绘制窗口图标
        if (win->icon_data && win->icon_width > 0 && win->icon_height > 0) {
            gui_draw_image(win->rect.x + 5, win->rect.y + 3, win->icon_width, win->icon_height, win->icon_data);
            gui_draw_text(win->rect.x + 5 + win->icon_width + 5, win->rect.y + 5, win->title, win->title_color);
        } else {
            gui_draw_text(win->rect.x + 5, win->rect.y + 5, win->title, win->title_color);
        }
        
        // 绘制关闭按钮
        rect_t close_rect = {win->rect.x + win->rect.width - 20, win->rect.y + 4, 16, 16};
        gui_clear_rect(&close_rect, (color_t)COLOR_RED);
    }
    
    // 绘制客户区域
    gui_clear_rect(&win->client_rect, (color_t)COLOR_WHITE);
    
    // 绘制子控件
    control_t* ctrl = win->children;
    while (ctrl) {
        if (ctrl->visible) {
            // 绘制控件背景
            rect_t ctrl_rect = {win->client_rect.x + ctrl->rect.x, win->client_rect.y + ctrl->rect.y, 
                               ctrl->rect.width, ctrl->rect.height};
            gui_clear_rect(&ctrl_rect, ctrl->bg_color);
            
            // 绘制控件文本
            if (ctrl->text[0]) {
                gui_draw_text(ctrl_rect.x + 5, ctrl_rect.y + 5, ctrl->text, ctrl->fg_color);
            }
        }
        ctrl = ctrl->next;
    }
    
    // 调用自定义绘制函数
    if (win->on_paint) {
        win->on_paint(win);
    }
}

// 控件管理
control_t* control_create(window_t* parent, u32 type, int x, int y, int width, int height) {
    if (!parent) {
        return NULL;
    }
    
    control_t* ctrl = kmalloc(sizeof(control_t));
    if (!ctrl) {
        return NULL;
    }
    
    memset(ctrl, 0, sizeof(control_t));
    ctrl->type = type;
    ctrl->rect.x = x;
    ctrl->rect.y = y;
    ctrl->rect.width = width;
    ctrl->rect.height = height;
    ctrl->visible = 1;
    ctrl->enabled = 1;
    ctrl->fg_color = (color_t)COLOR_BLACK;
    ctrl->bg_color = (color_t)COLOR_LIGHT_GRAY;
    ctrl->parent = parent;
    
    // 添加到父窗口的控件列表
    ctrl->next = parent->children;
    parent->children = ctrl;
    
    return ctrl;
}

void control_destroy(control_t* ctrl) {
    if (!ctrl) {
        return;
    }
    
    // 从父窗口的控件列表中移除
    if (ctrl->parent && ctrl->parent->children == ctrl) {
        ctrl->parent->children = ctrl->next;
    } else if (ctrl->parent) {
        control_t* prev = ctrl->parent->children;
        while (prev && prev->next != ctrl) {
            prev = prev->next;
        }
        if (prev) {
            prev->next = ctrl->next;
        }
    }
    
    kfree(ctrl);
}

void control_set_text(control_t* ctrl, const char* text) {
    if (ctrl) {
        strncpy(ctrl->text, text, sizeof(ctrl->text) - 1);
        if (ctrl->parent) {
            window_invalidate(ctrl->parent);
        }
    }
}

void control_set_position(control_t* ctrl, int x, int y) {
    if (ctrl) {
        ctrl->rect.x = x;
        ctrl->rect.y = y;
        if (ctrl->parent) {
            window_invalidate(ctrl->parent);
        }
    }
}

void control_set_size(control_t* ctrl, int width, int height) {
    if (ctrl) {
        ctrl->rect.width = width;
        ctrl->rect.height = height;
        if (ctrl->parent) {
            window_invalidate(ctrl->parent);
        }
    }
}

void control_set_visible(control_t* ctrl, int visible) {
    if (ctrl) {
        ctrl->visible = visible;
        if (ctrl->parent) {
            window_invalidate(ctrl->parent);
        }
    }
}

void control_set_enabled(control_t* ctrl, int enabled) {
    if (ctrl) {
        ctrl->enabled = enabled;
        if (ctrl->parent) {
            window_invalidate(ctrl->parent);
        }
    }
}

void control_set_colors(control_t* ctrl, color_t fg, color_t bg) {
    if (ctrl) {
        ctrl->fg_color = fg;
        ctrl->bg_color = bg;
        if (ctrl->parent) {
            window_invalidate(ctrl->parent);
        }
    }
}

// 绘图函数
void gui_clear_rect(rect_t* rect, color_t color) {
    if (!framebuffer || !rect) {
        return;
    }
    
    u32 color_value = color_to_u32(color);
    for (int y = rect->y; y < rect->y + rect->height && y < screen_height; y++) {
        for (int x = rect->x; x < rect->x + rect->width && x < screen_width; x++) {
            if (x >= 0 && y >= 0) {
                framebuffer[y * screen_width + x] = color_value;
            }
        }
    }
}

void gui_draw_rect(rect_t* rect, color_t color) {
    if (!rect) {
        return;
    }
    
    // 绘制上边
    rect_t top = {rect->x, rect->y, rect->width, 1};
    gui_clear_rect(&top, color);
    
    // 绘制下边
    rect_t bottom = {rect->x, rect->y + rect->height - 1, rect->width, 1};
    gui_clear_rect(&bottom, color);
    
    // 绘制左边
    rect_t left = {rect->x, rect->y, 1, rect->height};
    gui_clear_rect(&left, color);
    
    // 绘制右边
    rect_t right = {rect->x + rect->width - 1, rect->y, 1, rect->height};
    gui_clear_rect(&right, color);
}

void gui_draw_text(int x, int y, const char* text, color_t color) {
    if (!framebuffer || !text) {
        return;
    }
    
    u32 color_value = color_to_u32(color);
    
    while (*text) {
        char c = *text++;
        if (c >= 32 && c <= 126) {
            const u8* glyph = font_8x8[c - 32];
            for (int row = 0; row < 8; row++) {
                u8 row_data = glyph[row];
                for (int col = 0; col < 8; col++) {
                    if (row_data & (1 << col)) {
                        int px = x + col;
                        int py = y + row;
                        if (px >= 0 && px < screen_width && py >= 0 && py < screen_height) {
                            framebuffer[py * screen_width + px] = color_value;
                        }
                    }
                }
            }
        }
        x += 8;
    }
}

void gui_draw_line(int x1, int y1, int x2, int y2, color_t color) {
    // 简单的画线算法
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        rect_t pixel = {x1, y1, 1, 1};
        gui_clear_rect(&pixel, color);
        
        if (x1 == x2 && y1 == y2) {
            break;
        }
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void gui_draw_image(int x, int y, int width, int height, const u8* data) {
    if (!framebuffer || !data) {
        return;
    }
    
    for (int row = 0; row < height && y + row < screen_height; row++) {
        for (int col = 0; col < width && x + col < screen_width; col++) {
            if (x + col >= 0 && y + row >= 0) {
                int src_index = (row * width + col) * 4;
                color_t pixel = {data[src_index], data[src_index + 1], data[src_index + 2], data[src_index + 3]};
                framebuffer[(y + row) * screen_width + (x + col)] = color_to_u32(pixel);
            }
        }
    }
}

// 事件处理
int gui_poll_event(event_t* event) {
    // 简化实现，返回无事件
    if (event) {
        event->type = EVENT_NONE;
    }
    return 0;
}

void gui_push_event(event_t* event) {
    // 简化实现，不做处理
}

void gui_set_event_handler(void (*handler)(event_t* event)) {
    // 简化实现，不做处理
}

// 桌面管理
desktop_t* desktop_get(void) {
    return &desktop;
}

void desktop_set_color(color_t color) {
    desktop.desktop_color = color;
    if (desktop.desktop_window) {
        desktop.desktop_window->background_color = color;
        window_invalidate(desktop.desktop_window);
    }
}

void desktop_set_taskbar_visible(int visible) {
    desktop.show_taskbar = visible;
    gui_update();
}

void desktop_set_icons_visible(int visible) {
    desktop.show_icons = visible;
    gui_update();
}

// 图标加载函数
int gui_load_icon(const char* path, u8** out_data, int* out_width, int* out_height) {
    // 简化实现: 这里应该加载实际的图像文件
    // 对于示例，我们创建一个简单的16x16图标
    *out_width = 16;
    *out_height = 16;
    *out_data = kmalloc(*out_width * *out_height * 4);
    if (!*out_data) {
        return -1;
    }
    
    // 填充一些示例颜色 (可以替换为实际图像数据)
    for (int i = 0; i < *out_width * *out_height; i++) {
        (*out_data)[i*4 + 0] = 0;   // R
        (*out_data)[i*4 + 1] = 0;   // G
        (*out_data)[i*4 + 2] = 255; // B
        (*out_data)[i*4 + 3] = 255; // A
    }
    
    return 0;
}

void gui_free_icon(u8* data) {
    if (data) {
        kfree(data);
    }
}

// 实用函数
int gui_get_screen_width(void) {
    return screen_width;
}

int gui_get_screen_height(void) {
    return screen_height;
}

void gui_get_mouse_position(int* x, int* y) {
    if (x) *x = 0;
    if (y) *y = 0;
}

int gui_get_mouse_button_state(int button) {
    return 0;
}

int gui_get_key_state(int key) {
    return 0;
}