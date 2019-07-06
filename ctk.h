#ifndef CTK_H
#define CTK_H

#include <ncurses.h>
#include <stdint.h>
#ifdef CTK_GPM
#include <gpm.h>
#endif

#define CTK_COLOR_WINDOW 0x01
#define CTK_COLOR_MENU_BAR 0x02
#define CTK_COLOR_MENU_HOTKEY 0x03
#define CTK_COLOR_WARNING 0x04
#define CTK_COLOR_HIGHLIGHT 0x05
#define CTK_COLOR_SELECTED 0x06
#define CTK_COLOR_OK 0x07
#define CTK_COLOR_COOL 0x08

typedef enum {
    CTK_EVENT_DRAW = 1,
    CTK_EVENT_KEY = 2,
    CTK_EVENT_MOUSE = 3
} ctk_event_type_t;

typedef enum {
    CTK_WIDGET_AREA = 1,
    CTK_WIDGET_HBOX = 2,
    CTK_WIDGET_VBOX = 3,
    CTK_WIDGET_WINDOW = 4,
    CTK_WIDGET_MENU_BAR = 5,
    CTK_WIDGET_MENU = 6,
    CTK_WIDGET_MENU_ITEM = 7,
    CTK_WIDGET_VRULE = 8,
    CTK_WIDGET_HRULE = 9
} ctk_widget_type_t;

typedef enum {
    CTK_FLAG_ENABLED = 1,
    CTK_FLAG_SELECTED = 2,
    CTK_FLAG_VISIBLE = 3,
    CTK_FLAG_EXPAND_X = 4,
    CTK_FLAG_EXPAND_Y = 5,
    CTK_FLAG_ACTIVE = 6
} ctk_flag_t;

typedef struct ctk_widget ctk_widget_t;
typedef struct ctk_event ctk_event_t;
typedef uint8_t (*ctk_event_callback_t)(ctk_event_t* event, void* user_data);

typedef struct ctk_widget {
    WINDOW* win;
    ctk_widget_type_t type;
    uint16_t width;
    uint16_t height;
    uint16_t x;
    uint16_t y;
    char hotkey;
    char* label;
    uint8_t flags;
    uint16_t nr_children;
    ctk_widget_t* children;
    ctk_widget_t* parent;
    ctk_event_callback_t event_callback;
    void* user_data;
} ctk_widget_t;

typedef struct {
    WINDOW* win;
    ctk_widget_t mainwin;
    uint8_t redraw;
} ctk_ctx_t;

typedef struct ctk_event {
    ctk_ctx_t* ctx;
    ctk_widget_t* widget;
    ctk_event_type_t type;
    uint16_t x;
    uint16_t y;
    int key;
} ctk_event_t;

void ctk_addstr(ctk_widget_t* widget, uint16_t x, uint16_t y, uint8_t color, char* string);

void ctk_printf(ctk_widget_t* widget, uint16_t x, uint16_t y, uint8_t brush, const char *format, ...);

uint8_t ctk_init_menu_item(ctk_widget_t* widget, char hotkey, char* label);

uint8_t ctk_init_menu(ctk_widget_t* widget, char hotkey, char* label, ctk_widget_t* menu_items, uint16_t nr_menu_items);

uint8_t ctk_init_menu_bar(ctk_widget_t* widget, ctk_widget_t* menus, uint16_t nr_menus);

uint8_t ctk_init_window(ctk_widget_t* widget, uint16_t x, uint16_t y, uint16_t width, uint16_t height, ctk_widget_t* children, uint16_t nr_children);

uint8_t ctk_init_area(ctk_widget_t* widget, uint16_t width, uint16_t height, uint8_t expand_x, uint8_t expand_y);

uint8_t ctk_init_vrule(ctk_widget_t* widget);

uint8_t ctk_init_hbox(ctk_widget_t* widget, ctk_widget_t* children, uint16_t nr_children);

uint8_t ctk_init_vbox(ctk_widget_t* widget, ctk_widget_t* children, uint16_t nr_children);

uint8_t ctk_realize_widget(ctk_ctx_t* ctx, ctk_widget_t* widget);

void ctk_widget_event_handler(ctk_widget_t* widget, ctk_event_callback_t handler, void* user_data);

uint8_t ctk_draw_widget(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t x, uint16_t y);

uint8_t ctk_init_widgets(ctk_ctx_t* ctx, ctk_widget_t* widgets, uint16_t nr_widgets);

uint8_t ctk_init_ctx(ctk_ctx_t* ctx, WINDOW* target);

uint8_t ctk_init_curses();

uint8_t ctk_init(ctk_ctx_t* ctx, ctk_widget_t* widgets, uint16_t nr_widgets);

uint8_t ctk_main_loop(ctk_ctx_t* ctx);

void ctk_end(ctk_ctx_t* ctx);

#define CTK_KEY_CTRL_A 0x01
#define CTK_KEY_CTRL_B 0x02
#define CTK_KEY_CTRL_C 0x03
#define CTK_KEY_CTRL_D 0x04
#define CTK_KEY_CTRL_E 0x05
#define CTK_KEY_CTRL_F 0x06
#define CTK_KEY_CTRL_G 0x07
#define CTK_KEY_CTRL_H 0x08
#define CTK_KEY_CTRL_I 0x09
#define CTK_KEY_CTRL_J 0x0a
#define CTK_KEY_CTRL_K 0x0b
#define CTK_KEY_CTRL_L 0x0c
#define CTK_KEY_CTRL_M 0x0d
#define CTK_KEY_CTRL_N 0x0e
#define CTK_KEY_CTRL_O 0x0f
#define CTK_KEY_CTRL_P 0x10
#define CTK_KEY_CTRL_Q 0x11
#define CTK_KEY_CTRL_R 0x12
#define CTK_KEY_CTRL_S 0x13
#define CTK_KEY_CTRL_T 0x14
#define CTK_KEY_CTRL_U 0x15
#define CTK_KEY_CTRL_V 0x16
#define CTK_KEY_CTRL_W 0x17
#define CTK_KEY_CTRL_X 0x18
#define CTK_KEY_CTRL_Y 0x19
#define CTK_KEY_CTRL_Z 0x1a
#define CTK_KEY_CTRL_LSB 0x1b
#define CTK_KEY_CTRL_BS 0x1c
#define CTK_KEY_CTRL_RSB 0x1d
#define CTK_KEY_CTRL_HAT 0x1e
#define CTK_KEY_CTRL_US 0x1f

#endif
