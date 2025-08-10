#ifndef GUI_H
#define GUI_H

#include <stdint.h>
#include "../kernel/kernel.h"

// 颜色定义
typedef struct {
    u8 r, g, b, a;
} color_t;

// 预定义颜色
#define COLOR_BLACK     {0, 0, 0, 255}
#define COLOR_WHITE     {255, 255, 255, 255}
#define COLOR_RED       {255, 0, 0, 255}
#define COLOR_GREEN     {0, 255, 0, 255}
#define COLOR_BLUE      {0, 0, 255, 255}
#define COLOR_GRAY      {128, 128, 128, 255}
#define COLOR_LIGHT_GRAY {192, 192, 192, 255}
#define COLOR_DARK_GRAY {64, 64, 64, 255}

// 窗口状态
#define WINDOW_STATE_NORMAL    0
#define WINDOW_STATE_MINIMIZED 1
#define WINDOW_STATE_MAXIMIZED 2
#define WINDOW_STATE_HIDDEN    3

// 窗口样式
#define WINDOW_STYLE_BORDER     0x01
#define WINDOW_STYLE_TITLEBAR   0x02
#define WINDOW_STYLE_RESIZABLE  0x04
#define WINDOW_STYLE_CLOSABLE   0x08
#define WINDOW_STYLE_MINIMIZABLE 0x10
#define WINDOW_STYLE_MAXIMIZABLE 0x20

// 事件类型
#define EVENT_NONE          0
#define EVENT_MOUSE_MOVE    1
#define EVENT_MOUSE_DOWN    2
#define EVENT_MOUSE_UP      3
#define EVENT_MOUSE_CLICK   4
#define EVENT_MOUSE_DBLCLK  5
#define EVENT_KEY_DOWN      6
#define EVENT_KEY_UP        7
#define EVENT_KEY_PRESS     8
#define EVENT_WINDOW_CLOSE  9
#define EVENT_WINDOW_RESIZE 10
#define EVENT_WINDOW_MOVE   11
#define EVENT_WINDOW_FOCUS  12
#define EVENT_WINDOW_BLUR   13

// 鼠标按钮
#define MOUSE_BUTTON_LEFT   1
#define MOUSE_BUTTON_RIGHT  2
#define MOUSE_BUTTON_MIDDLE 3

// 键盘修饰键
#define KEY_MOD_SHIFT       0x01
#define KEY_MOD_CTRL        0x02
#define KEY_MOD_ALT         0x04
#define KEY_MOD_SUPER       0x08

// 点结构
typedef struct {
    int x, y;
} point_t;

// 矩形结构
typedef struct {
    int x, y, width, height;
} rect_t;

// 事件结构
typedef struct {
    u32 type;
    u32 timestamp;
    union {
        struct {
            int x, y;
            int button;
            u32 modifiers;
        } mouse;
        struct {
            int key;
            u32 modifiers;
            char character;
        } keyboard;
        struct {
            int width, height;
        } resize;
        struct {
            int x, y;
        } move;
    } data;
} event_t;

// 窗口结构
typedef struct window {
    char title[256];
    rect_t rect;
    rect_t client_rect;
    u32 state;
    u32 style;
    color_t background_color;
    color_t title_color;
    color_t border_color;
    int focused;
    const u8* icon_data;  // 窗口图标数据
    int icon_width;       // 图标宽度
    int icon_height;      // 图标高度;
    void (*on_paint)(struct window* win);
    void (*on_event)(struct window* win, event_t* event);
    void* user_data;
    struct window* parent;
    struct window* next;
    struct window* children;
} window_t;

// 控件类型
#define CONTROL_TYPE_BUTTON    1
#define CONTROL_TYPE_LABEL     2
#define CONTROL_TYPE_TEXTBOX   3
#define CONTROL_TYPE_LISTBOX   4
#define CONTROL_TYPE_CHECKBOX  5
#define CONTROL_TYPE_RADIO     6
#define CONTROL_TYPE_PROGRESS  7
#define CONTROL_TYPE_SCROLLBAR 8

// 控件结构
typedef struct control {
    u32 type;
    rect_t rect;
    char text[256];
    color_t fg_color;
    color_t bg_color;
    int visible;
    int enabled;
    void (*on_click)(struct control* ctrl);
    void (*on_change)(struct control* ctrl);
    void* user_data;
    struct control* next;
    struct control* parent;
} control_t;

// 桌面结构
typedef struct {
    window_t* windows;
    window_t* focused_window;
    window_t* desktop_window;
    color_t desktop_color;
    rect_t desktop_rect;
    int show_taskbar;
    int show_icons;
} desktop_t;

// GUI 初始化
int gui_init(void);
void gui_shutdown(void);
void gui_update(void);

// 窗口管理
window_t* window_create(const char* title, int x, int y, int width, int height, u32 style);
void window_destroy(window_t* win);
void window_show(window_t* win);
void window_hide(window_t* win);
void window_move(window_t* win, int x, int y);
void window_resize(window_t* win, int width, int height);
void window_focus(window_t* win);
void window_blur(window_t* win);
void window_set_title(window_t* win, const char* title);
void window_set_background(window_t* win, color_t color);
void window_invalidate(window_t* win);

// 控件管理
control_t* control_create(window_t* parent, u32 type, int x, int y, int width, int height);
void control_destroy(control_t* ctrl);
void control_set_text(control_t* ctrl, const char* text);
void control_set_position(control_t* ctrl, int x, int y);
void control_set_size(control_t* ctrl, int width, int height);
void control_set_visible(control_t* ctrl, int visible);
void control_set_enabled(control_t* ctrl, int enabled);
void control_set_colors(control_t* ctrl, color_t fg, color_t bg);

// 绘图函数
void gui_clear_rect(rect_t* rect, color_t color);
void gui_draw_rect(rect_t* rect, color_t color);
void gui_draw_text(int x, int y, const char* text, color_t color);
void gui_draw_line(int x1, int y1, int x2, int y2, color_t color);
void gui_draw_image(int x, int y, int width, int height, const u8* data);

// 事件处理
int gui_poll_event(event_t* event);
void gui_push_event(event_t* event);
void gui_set_event_handler(void (*handler)(event_t* event));

// 桌面管理
desktop_t* desktop_get(void);
void desktop_set_color(color_t color);
void desktop_set_taskbar_visible(int visible);
void desktop_set_icons_visible(int visible);

// 实用函数
int gui_get_screen_width(void);
int gui_get_screen_height(void);
void gui_get_mouse_position(int* x, int* y);
int gui_get_mouse_button_state(int button);
int gui_get_key_state(int key);

// 图标加载函数
int gui_load_icon(const char* path, u8** out_data, int* out_width, int* out_height);
void gui_free_icon(u8* data);

#endif // GUI_H