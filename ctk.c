#include <ncurses.h>
#include <signal.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include "ctk.h"

#define BIT_TEST(a, f)   ((a >> f) & 1)
#define BIT_SET(a, f)    (a |= (1 << f))
#define BIT_UNSET(a, f)  (a &= ~(1 << f))
#define BIT_FLIP(a, f)   (a ^= (1 << f))

uint8_t count = 0;
//mvwprintw(ctx->win, 10 + count++, 10, "%s [%dx%d]", widget->widget.menu_item.label, x, y);

static uint8_t p1_layout_widget(ctk_ctx_t* ctx, ctk_widget_t* widget);

static uint8_t p1_layout_widget_noop(ctk_ctx_t* ctx, ctk_widget_t* widget) {
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        p1_layout_widget(ctx, &widget->children[i]);
    }
    return 1;
}

static uint8_t p1_layout_widget_hbox(ctk_ctx_t* ctx, ctk_widget_t* widget, uint8_t padding) {
    uint16_t min_width = 0;
    uint16_t min_height = 0;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        p1_layout_widget(ctx, &widget->children[i]);
        widget->children[i].x = min_width;
        widget->children[i].width += padding;
        min_width += widget->children[i].width;
        if (widget->children[i].height > min_height) {
            min_height = widget->children[i].height;
        }
    }
    if (widget->width < min_width) {
        widget->width = min_width;
    }
    if (widget->height > min_height) {
        widget->height = min_height;
    }
    return 1;
}

static uint8_t p1_layout_widget_vbox(ctk_ctx_t* ctx, ctk_widget_t* widget) {
    uint16_t min_width = 0;
    uint16_t min_height = 0;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        p1_layout_widget(ctx, &widget->children[i]);
        widget->children[i].y = min_height;
        min_height += widget->children[i].height;
        if (widget->children[i].width > min_width) {
            min_width = widget->children[i].width;
        }
    }
    if (widget->width > min_width) {
        widget->width = min_width;
    }
    if (widget->height < min_height) {
        widget->height = min_height;
    }
    return 1;
}

static uint8_t p1_layout_widget_menu(ctk_ctx_t* ctx, ctk_widget_t* widget) {
    uint16_t min_width = 0;
    uint16_t min_height = 0;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        p1_layout_widget(ctx, &widget->children[i]);
        widget->children[i].y = min_height + 1;
        min_height += widget->children[i].height;
        if (widget->children[i].width > min_width) {
            min_width = widget->children[i].width;
        }
    }
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        widget->children[i].width = min_width;
    }
    return 1;
}

static uint8_t p1_layout_widget_menu_item(ctk_ctx_t* ctx, ctk_widget_t* widget) {
    return 1;
}

static uint8_t p1_layout_widget(ctk_ctx_t* ctx, ctk_widget_t* widget) {
    widget->win = ctx->win;
    switch (widget->type) {
        case CTK_WIDGET_AREA:
            return p1_layout_widget_noop(ctx, widget);
            break;
        case CTK_WIDGET_HBOX:
            return p1_layout_widget_hbox(ctx, widget, 0);
            break;
        case CTK_WIDGET_VBOX:
            return p1_layout_widget_vbox(ctx, widget);
            break;
        case CTK_WIDGET_WINDOW:
            return p1_layout_widget_noop(ctx, widget);
            break;
        case CTK_WIDGET_MENU_BAR:
            return p1_layout_widget_hbox(ctx, widget, 1);
            break;
        case CTK_WIDGET_MENU:
            return p1_layout_widget_menu(ctx, widget);
            break;
        case CTK_WIDGET_VRULE:
            return p1_layout_widget_noop(ctx, widget);
            break;
        case CTK_WIDGET_HRULE:
            return p1_layout_widget_noop(ctx, widget);
            break;
        case CTK_WIDGET_MENU_ITEM:
            return p1_layout_widget_menu_item(ctx, widget);
            break;
        default:
            return p1_layout_widget_vbox(ctx, widget);
            break;
    }
    return 0;
}

static uint8_t p2_expand_widget(ctk_ctx_t* ctx, ctk_widget_t* widget);

static uint8_t p2_expand_widget_noop(ctk_ctx_t* ctx, ctk_widget_t* widget) {
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        p2_expand_widget(ctx, &widget->children[i]);
    }
    return 1;
}

static uint8_t p2_expand_widget_hbox(ctk_ctx_t* ctx, ctk_widget_t* widget) {
    uint16_t width = widget->width;
    uint16_t child_width = 0;
    uint16_t nr_expandable = 0;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        child_width += widget->children[i].width;
        if (BIT_TEST(widget->children[i].flags, CTK_FLAG_EXPAND_X)) {
            nr_expandable++;
        }
    }
    uint16_t diff = width - child_width;
    uint16_t space_per_child = 0;
    if (nr_expandable > 0) {
        space_per_child = diff / nr_expandable;
    }
    uint16_t xmove = 0;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        uint8_t recalc_child = 0;
        widget->children[i].x = xmove;
        if (BIT_TEST(widget->children[i].flags, CTK_FLAG_EXPAND_X)) {
            widget->children[i].width += space_per_child;
            recalc_child = 1;
        }
        if (BIT_TEST(widget->children[i].flags, CTK_FLAG_EXPAND_Y)) {
            widget->children[i].height = widget->height;
            recalc_child = 1;
        }
        if (recalc_child) {
            p2_expand_widget(ctx, &widget->children[i]);
        }
        xmove += widget->children[i].width;
    }

    return 1;
}

static uint8_t p2_expand_widget_vbox(ctk_ctx_t* ctx, ctk_widget_t* widget) {
    uint16_t height = widget->height;
    uint16_t child_height = 0;
    uint16_t nr_expandable = 0;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        child_height += widget->children[i].height;
        if (BIT_TEST(widget->children[i].flags, CTK_FLAG_EXPAND_Y)) {
            nr_expandable++;
        }
    }
    uint16_t diff = height - child_height;
    uint16_t space_per_child = 0;
    if (nr_expandable > 0) {
        space_per_child = diff / nr_expandable;
    }
    uint16_t ymove = 0;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        uint8_t recalc_child = 0;
        widget->children[i].y = ymove;
        if (BIT_TEST(widget->children[i].flags, CTK_FLAG_EXPAND_Y)) {
            widget->children[i].height += space_per_child;
            recalc_child = 1;
        }
        if (BIT_TEST(widget->children[i].flags, CTK_FLAG_EXPAND_X)) {
            widget->children[i].width = widget->width;
            recalc_child = 1;
        }
        if (recalc_child) {
            p2_expand_widget(ctx, &widget->children[i]);
        }
        ymove += widget->children[i].height;
    }

    return 1;
}

static uint8_t p2_expand_widget(ctk_ctx_t* ctx, ctk_widget_t* widget) {
    switch (widget->type) {
        case CTK_WIDGET_HBOX:
            return p2_expand_widget_hbox(ctx, widget);
            break;
        case CTK_WIDGET_VBOX:
            return p2_expand_widget_vbox(ctx, widget);
            break;
        case CTK_WIDGET_WINDOW:
            return p2_expand_widget_vbox(ctx, widget);
            break;
        case CTK_WIDGET_MENU_BAR:
            return p2_expand_widget_hbox(ctx, widget);
            break;
        default:
            return p2_expand_widget_noop(ctx, widget);
            break;
    }
    return 0;
}

static uint8_t draw_widget_vrule(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t x, uint16_t y) {
    wattron(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    for (uint16_t i = 0; i < widget->height; i++) {
        mvwaddch(ctx->win, y + i, x, ' ');
    }
    wattroff(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    return 1;
}

static uint8_t draw_widget_hrule(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t x, uint16_t y) {
    wattron(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    for (uint16_t i = 0; i < widget->width; i++) {
        mvwaddch(ctx->win, y, x + i, ' ');
    }
    wattroff(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    return 1;
}

static uint8_t draw_widget_menu_item(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t x, uint16_t y) {
    wattron(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    uint16_t label_len = strlen(widget->label);
    uint16_t pad_len = widget->width - label_len;
    uint16_t pad_loc = x + label_len;
    mvwprintw(ctx->win, y, x, "%s", widget->label);
    for (uint16_t i = 0; i < pad_len; i++) {
        mvwaddch(ctx->win, y, pad_loc + i, ' ');
    }
    if (widget->hotkey != 0) {
        wattron(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_HOTKEY));
        mvwprintw(ctx->win, y, x, "%c", widget->hotkey);
        wattroff(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_HOTKEY));
    }
    wattroff(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    return 1;
}

static uint8_t draw_widget_menu(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t x, uint16_t y) {
    wattron(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    mvwprintw(ctx->win, y, x, "%s", widget->label);
    wattroff(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    wattron(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_HOTKEY));
    mvwprintw(ctx->win, y, x, "%c", widget->hotkey);
    wattroff(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_HOTKEY));
    if (!BIT_TEST(widget->flags, CTK_FLAG_ACTIVE)) {
        return 1;
    }
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        ctk_draw_widget(ctx, &widget->children[i], x, y);
    }
    return 1;
}

static uint8_t draw_widget_menu_bar(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t x, uint16_t y) {
    wattron(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    for (uint16_t i = 0; i < widget->width; i++) {
        mvwaddch(ctx->win, y, x + i, ' ');
    }
    wattroff(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        ctk_draw_widget(ctx, &widget->children[i], x, y);
    }
    return 1;
}

static uint8_t draw_widget_default(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t x, uint16_t y) {
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        ctk_draw_widget(ctx, &widget->children[i], x, y);
    }
    return 1;
}

uint8_t ctk_draw_widget(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t x, uint16_t y) {
    x += widget->x;
    y += widget->y;
    uint8_t status = 0;
    if (widget->event_callback) {
        ctk_event_t event;
        event.x = widget->x;
        event.y = widget->y;
        event.type = CTK_EVENT_DRAW;
        event.ctx = ctx;
        event.widget = widget;
        status = widget->event_callback(&event, widget->user_data);
    }
    switch (widget->type) {
        case CTK_WIDGET_MENU:
            status = draw_widget_menu(ctx, widget, x, y);
            break;
        case CTK_WIDGET_MENU_ITEM:
            status = draw_widget_menu_item(ctx, widget, x, y);
            break;
        case CTK_WIDGET_MENU_BAR:
            status = draw_widget_menu_bar(ctx, widget, x, y);
            break;
        case CTK_WIDGET_VRULE:
            status = draw_widget_vrule(ctx, widget, x, y);
            break;
        case CTK_WIDGET_HRULE:
            status = draw_widget_hrule(ctx, widget, x, y);
            break;
        default:
            status = draw_widget_default(ctx, widget, x, y);
            break;
    }
    return status;
}

static void add_abs_widget_pos(ctk_widget_t* widget, uint16_t* xp, uint16_t* yp) {
    while (widget) {
        *xp += widget->x;
        *yp += widget->y;
        widget = widget->parent;
    }
}

void ctk_addstr(ctk_widget_t* widget, uint16_t x, uint16_t y, uint8_t brush, char* string) {
    add_abs_widget_pos(widget, &x, &y);
    wattron(widget->win, COLOR_PAIR(brush));
    mvwaddstr(widget->win, y, x, string);
    wattroff(widget->win, COLOR_PAIR(brush));
}

void ctk_printf(ctk_widget_t* widget, uint16_t x, uint16_t y, uint8_t brush, const char *format, ...) {
    add_abs_widget_pos(widget, &x, &y);
    va_list arg;
    va_start(arg, format);
    wmove(widget->win, y, x);
    vwprintw(widget->win, format, arg);
    va_end(arg);
}

void ctk_addch(ctk_widget_t* widget, uint16_t x, uint16_t y, uint8_t brush, char c) {
    add_abs_widget_pos(widget, &x, &y);
    wattron(widget->win, COLOR_PAIR(brush));
    mvwaddch(widget->win, y, x, c);
    wattroff(widget->win, COLOR_PAIR(brush));
}

static void cleanup() {
}

//static void resize_view() {
//    ctk_realize_widget(&ctx->mainwin);
//}

static void refresh_view(ctk_ctx_t* ctx) {
    wclear(ctx->win);
    ctk_draw_widget(ctx, &ctx->mainwin, 0, 0);
    refresh();
    wrefresh(ctx->win);
}

static void zero_widget(ctk_widget_t* widget) {
    memset(widget, 0, sizeof(ctk_widget_t));
}

void ctk_deactivate_all(ctk_widget_t* widget) {
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        BIT_UNSET(widget->children[i].flags, CTK_FLAG_ACTIVE);
    }
}

static uint8_t default_menu_handler(ctk_event_t* event, void* user_data) {
    if (event->type != CTK_EVENT_MOUSE) {
        return 0;
    }
    uint8_t active = BIT_TEST(event->widget->flags, CTK_FLAG_ACTIVE);
    ctk_deactivate_all(event->widget->parent);
    if (!active) {
        BIT_SET(event->widget->flags, CTK_FLAG_ACTIVE);
    }
    event->ctx->redraw = 1;
    return 1;
}

uint8_t ctk_init_menu_item(ctk_widget_t* widget, char hotkey, char* label) {
    zero_widget(widget);
    BIT_UNSET(widget->flags, CTK_FLAG_ACTIVE);
    BIT_SET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->type = CTK_WIDGET_MENU_ITEM;
    widget->width = strlen(label);
    widget->height = 1;
    widget->label = label;
    widget->hotkey = hotkey;
    return 1;
}

uint8_t ctk_init_menu(ctk_widget_t* widget, char hotkey, char* label, ctk_widget_t* menu_items, uint16_t nr_menu_items) {
    zero_widget(widget);
    BIT_SET(widget->flags, CTK_FLAG_VISIBLE);
    BIT_UNSET(widget->flags, CTK_FLAG_ACTIVE);
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->type = CTK_WIDGET_MENU;
    widget->width = strlen(label);
    widget->height = 1;
    widget->label = label;
    widget->hotkey = hotkey;
    widget->children = menu_items;
    widget->nr_children = nr_menu_items;
    widget->event_callback = default_menu_handler;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        widget->children[i].parent = widget;
    }
    return 1;
}

uint8_t ctk_init_menu_bar(ctk_widget_t* widget, ctk_widget_t* menus, uint16_t nr_menus) {
    zero_widget(widget);
    BIT_SET(widget->flags, CTK_FLAG_VISIBLE);
    BIT_SET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->type = CTK_WIDGET_MENU_BAR;
    widget->width = 0;
    widget->height = 1;
    widget->children = menus;
    widget->nr_children = nr_menus;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        widget->children[i].parent = widget;
    }
    return 1;
}

uint8_t ctk_init_window(ctk_widget_t* widget, uint16_t x, uint16_t y, uint16_t width, uint16_t height, ctk_widget_t* children, uint16_t nr_children) {
    zero_widget(widget);
    widget->type = CTK_WIDGET_WINDOW;
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->x = x;
    widget->y = y;
    widget->width = width;
    widget->height = height;
    widget->nr_children = nr_children;
    widget->children = children;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        widget->children[i].parent = widget;
    }
    return 1;
}

uint8_t ctk_init_area(ctk_widget_t* widget, uint16_t width, uint16_t height, uint8_t expand_x, uint8_t expand_y) {
    zero_widget(widget);
    widget->type = CTK_WIDGET_AREA;
    BIT_UNSET(widget->flags, CTK_FLAG_VISIBLE);
    if (expand_x) {
        BIT_SET(widget->flags, CTK_FLAG_EXPAND_X);
    }
    if (expand_y) {
        BIT_SET(widget->flags, CTK_FLAG_EXPAND_Y);
    }
    widget->width = width;
    widget->height = height;
    return 1;
}

uint8_t ctk_init_vrule(ctk_widget_t* widget) {
    zero_widget(widget);
    widget->type = CTK_WIDGET_VRULE;
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_SET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->width = 1;
    widget->height = 1;
    return 1;
}

uint8_t ctk_init_hrule(ctk_widget_t* widget) {
    zero_widget(widget);
    widget->type = CTK_WIDGET_HRULE;
    BIT_SET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->width = 1;
    widget->height = 1;
    return 1;
}

uint8_t ctk_init_hbox(ctk_widget_t* widget, ctk_widget_t* children, uint16_t nr_children) {
    zero_widget(widget);
    widget->type = CTK_WIDGET_HBOX;
    BIT_UNSET(widget->flags, CTK_FLAG_VISIBLE);
    BIT_SET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_SET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->nr_children = nr_children;
    widget->children = children;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        widget->children[i].parent = widget;
    }
    return 1;
}

uint8_t ctk_init_vbox(ctk_widget_t* widget, ctk_widget_t* children, uint16_t nr_children) {
    zero_widget(widget);
    widget->type = CTK_WIDGET_VBOX;
    BIT_UNSET(widget->flags, CTK_FLAG_VISIBLE);
    BIT_SET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_SET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->nr_children = nr_children;
    widget->children = children;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        widget->children[i].parent = widget;
    }
    return 1;
}

static uint8_t handle_event_widget(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t px, uint16_t py, uint16_t x, uint16_t y, ctk_event_type_t type) {
    px += widget->x;
    py += widget->y;
    uint8_t handled = 0;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        handled = handle_event_widget(ctx, &widget->children[i], px, py, x, y, type);
        if (handled) {
            break;
        }
    }
    if (handled) {
        return 1;
    }
    if ((x >= px) && (x <= px + widget->width) && (y >= py) && (y <= py + widget->height)) {
        if (widget->event_callback) {
            ctk_event_t event;
            event.x = px - x;
            event.y = py - y;
            event.type = type;
            event.ctx = ctx;
            event.widget = widget;
            return widget->event_callback(&event, widget->user_data);
        }
    }
    return 0;
}

static void handle_event(ctk_ctx_t* ctx, uint16_t x, uint16_t y, ctk_event_type_t type) {
    handle_event_widget(ctx, &ctx->mainwin, 0, 0, x, y, type);
}

static uint8_t handle_event_key(ctk_ctx_t* ctx, int c) {
    ctk_widget_t* widget = &ctx->mainwin;
    if (widget->event_callback) {
        ctk_event_t event;
        event.x = 0;
        event.y = 0;
        event.key = c;
        event.type = CTK_EVENT_KEY;
        event.ctx = ctx;
        event.widget = widget;
        return widget->event_callback(&event, widget->user_data);
    }
    return 0;
}

void ctk_widget_event_handler(ctk_widget_t* widget, ctk_event_callback_t handler, void* user_data) {
    widget->event_callback = handler;
    widget->user_data = user_data;
}

void ctk_loop_callback(ctk_ctx_t* ctx, ctk_loop_callback_t handler, void* user_data) {
    ctx->loop_callback = handler;
    ctx->user_data = user_data;
}

static void trigger_hotkey(ctk_ctx_t* ctx, ctk_widget_t* widget, char hotkey) {
    if (widget->hotkey == hotkey) {
    }
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        trigger_hotkey(ctx, &widget->children[i], hotkey);
    }
}

uint8_t ctk_init_widgets(ctk_ctx_t* ctx, ctk_widget_t* widgets, uint16_t nr_widgets) {
    int x, y;
    getmaxyx(ctx->win, y, x);
    ctk_init_window(&ctx->mainwin, 0, 0, x, y, widgets, nr_widgets);
    BIT_SET(ctx->mainwin.flags, CTK_FLAG_ACTIVE);
    p1_layout_widget(ctx, &ctx->mainwin);
    p2_expand_widget(ctx, &ctx->mainwin);
    return 1;
}

uint8_t ctk_init_ctx(ctk_ctx_t* ctx, WINDOW* target) {
    ctx->win = target;
    wbkgd(ctx->win, COLOR_PAIR(CTK_COLOR_WINDOW));
    ctx->redraw = 1;
    return 1;
}

static void init_curses() {
    setlocale(LC_ALL, "");
    initscr();
    if (has_colors()) {
        start_color();
        init_pair(CTK_COLOR_WINDOW, COLOR_WHITE, COLOR_BLACK);
        init_pair(CTK_COLOR_MENU_BAR, COLOR_WHITE, COLOR_BLUE);
        init_pair(CTK_COLOR_MENU_HOTKEY, COLOR_RED, COLOR_BLUE);
        init_pair(CTK_COLOR_WARNING, COLOR_YELLOW, COLOR_BLACK);
        init_pair(CTK_COLOR_HIGHLIGHT, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(CTK_COLOR_SELECTED, COLOR_BLACK, COLOR_MAGENTA);
        init_pair(CTK_COLOR_OK, COLOR_GREEN, COLOR_BLACK);
        init_pair(CTK_COLOR_COOL, COLOR_BLUE, COLOR_BLACK);
    }
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    timeout(50);
    curs_set(FALSE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
}

static void handle_winch(int sig){
    signal(SIGWINCH, SIG_IGN);
    endwin();
    //resize_view();
    //refresh_view();
    signal(SIGWINCH, handle_winch);
}

static void handle_int(int sig) {
    cleanup();
    endwin();
    exit(0);
}

static void install_signal_handlers() {
    signal(SIGINT, handle_int);
    signal(SIGWINCH, handle_winch);
}

uint8_t ctk_init_curses() {
    init_curses();
    install_signal_handlers();
    return 1;
}

uint8_t ctk_init(ctk_ctx_t* ctx, ctk_widget_t* widgets, uint16_t nr_widgets) {
    ctk_init_curses();
    ctk_init_ctx(ctx, stdscr);
    ctk_init_widgets(ctx, widgets, nr_widgets);
    return 1;
}

uint8_t ctk_main_loop(ctk_ctx_t* ctx) {
    int c;
    MEVENT event;
    refresh_view(ctx);
    while (1) {
        c = getch();
        switch (c) {
            case ERR:
                break;
            case 6:
                trigger_hotkey(ctx, &ctx->mainwin, 'F');
                break;
            case KEY_MOUSE:
                if (getmouse(&event) == OK) {
                    handle_event(ctx, event.x, event.y, CTK_EVENT_MOUSE);
                }
                break;
            default:
                handle_event_key(ctx, c);
                break;
        }
        if (ctx->loop_callback) {
            ctx->loop_callback(ctx, ctx->user_data);
        }
        if (ctx->redraw) {
            refresh_view(ctx);
            ctx->redraw = 0;
        }
    }
}

//    if (c > 0 && ) {
//    }

void ctk_end(ctk_ctx_t* ctx) {
    cleanup();
    endwin();
    //Gpm_Close();
}
