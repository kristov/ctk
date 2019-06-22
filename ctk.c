#include <ncurses.h>
#include <signal.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include "ctk.h"

#ifdef CTK_GPM
int my_handler(Gpm_Event *event, void *data)
{       printf("Event Type : %d at x=%d y=%d\n", event->type, event->x, event->y);
        return 0;       
}
#endif

#define BIT_TEST(a, f)   ((a >> f) & 1)
#define BIT_SET(a, f)    (a |= (1 << f))
#define BIT_UNSET(a, f)  (a &= ~(1 << f))

uint8_t count = 0;
//mvwprintw(ctx->win, 10 + count++, 10, "%s [%dx%d]", widget->widget.menu_item.label, x, y);

static uint8_t p1_layout_widget(ctk_ctx_t* ctx, ctk_widget_t* widget);

static uint8_t p1_layout_widget_void(ctk_ctx_t* ctx, ctk_widget_t* widget) {
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
        widget->children[i].y = min_height;
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
        case CTK_WIDGET_VOID:
            return p1_layout_widget_void(ctx, widget);
            break;
        case CTK_WIDGET_HBOX:
            return p1_layout_widget_hbox(ctx, widget, 0);
            break;
        case CTK_WIDGET_VBOX:
            return p1_layout_widget_vbox(ctx, widget);
            break;
        case CTK_WIDGET_WINDOW:
            return p1_layout_widget_vbox(ctx, widget);
            break;
        case CTK_WIDGET_MENU_BAR:
            return p1_layout_widget_hbox(ctx, widget, 1);
            break;
        case CTK_WIDGET_MENU:
            return p1_layout_widget_menu(ctx, widget);
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

static uint8_t p2_expand_widget_x(ctk_ctx_t* ctx, ctk_widget_t* widget) {
    uint16_t width = widget->width;
    uint16_t child_width = 0;
    uint16_t nr_expandable = 0;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        child_width += widget->children[i].width;
        if (BIT_TEST(widget->children[i].flags, CTK_FLAG_EXPAND_X)) {
            nr_expandable++;
        }
    }
mvwprintw(ctx->win, 1 + count++, 100, "nr_expandable: %d", nr_expandable);
    if (nr_expandable == 0) {
        return 1;
    }
    if (width <= child_width) {
        return 1;
    }
    uint16_t diff = width - child_width;
    uint16_t space_per_child = diff / nr_expandable;
mvwprintw(ctx->win, 1 + count++, 100, "diff: %d, space: %d", diff, space_per_child);
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        if (BIT_TEST(widget->children[i].flags, CTK_FLAG_EXPAND_X)) {
            widget->children[i].width += space_per_child;
            p2_expand_widget(ctx, &widget->children[i]);
        }
    }

    return 1;
}

static uint8_t p2_expand_widget(ctk_ctx_t* ctx, ctk_widget_t* widget) {
mvwprintw(ctx->win, 1 + count++, 100, "p2_expand_widget: %dx%d [%d]", widget->width, widget->height, widget->type);
    if (BIT_TEST(widget->flags, CTK_FLAG_EXPAND_X)) {
        p2_expand_widget_x(ctx, widget);
    }
    if (BIT_TEST(widget->flags, CTK_FLAG_EXPAND_Y)) {
    }
    return 0;
}

static uint8_t draw_widget_menu_item(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t x, uint16_t y) {
    if (!BIT_TEST(widget->flags, CTK_FLAG_VISIBLE)) {
        return 0;
    }
    x += widget->x;
    y += widget->y;
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
    return 1;
}

static uint8_t draw_widget_menu(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t x, uint16_t y) {
    x += widget->x;
    y += widget->y;
    wattron(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    mvwprintw(ctx->win, y, x, "%s", widget->label);
    wattroff(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_BAR));
    wattron(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_HOTKEY));
    mvwprintw(ctx->win, y, x, "%c", widget->hotkey);
    wattroff(ctx->win, COLOR_PAIR(CTK_COLOR_MENU_HOTKEY));
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        ctk_draw_widget(ctx, &widget->children[i], x, y);
    }
    return 1;
}

static uint8_t draw_widget_menu_bar(ctk_ctx_t* ctx, ctk_widget_t* widget, uint16_t x, uint16_t y) {
    x += widget->x;
    y += widget->y;
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
    switch (widget->type) {
        case CTK_WIDGET_MENU:
            return draw_widget_menu(ctx, widget, x, y);
            break;
        case CTK_WIDGET_MENU_ITEM:
            return draw_widget_menu_item(ctx, widget, x, y);
            break;
        case CTK_WIDGET_MENU_BAR:
            return draw_widget_menu_bar(ctx, widget, x, y);
            break;
        default:
            return draw_widget_default(ctx, widget, x, y);
            break;
    }
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
    timeout(1000);
    curs_set(FALSE);
}

static void cleanup() {
}

//static void resize_view() {
//    ctk_realize_widget(&ctx->mainwin);
//}

static void refresh_view(ctk_ctx_t* ctx) {
    ctk_draw_widget(ctx, &ctx->mainwin, 0, 0);
    refresh();
    wrefresh(ctx->win);
    //wrefresh();
    //wrefresh();
    //wrefresh();
    //wrefresh();
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

static void zero_widget(ctk_widget_t* widget) {
    memset(widget, 0, sizeof(ctk_widget_t));
}

uint8_t ctk_menu_item_init(ctk_widget_t* widget, char hotkey, char* label) {
    zero_widget(widget);
    BIT_UNSET(widget->flags, CTK_FLAG_VISIBLE);
    BIT_SET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->type = CTK_WIDGET_MENU_ITEM;
    widget->width = strlen(label);
    widget->height = 1;
    widget->label = label;
    widget->hotkey = hotkey;
    return 1;
}

uint8_t ctk_menu_init(ctk_widget_t* widget, char hotkey, char* label, ctk_widget_t* menu_items, uint16_t nr_menu_items) {
    zero_widget(widget);
    BIT_SET(widget->flags, CTK_FLAG_VISIBLE);
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->type = CTK_WIDGET_MENU;
    widget->width = strlen(label);
    widget->height = 1;
    widget->label = label;
    widget->hotkey = hotkey;
    widget->children = menu_items;
    widget->nr_children = nr_menu_items;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        widget->children[i].parent = widget;
    }
    return 1;
}

uint8_t ctk_menu_bar_init(ctk_widget_t* widget, ctk_widget_t* menus, uint16_t nr_menus) {
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

uint8_t ctk_window_init(ctk_widget_t* widget, uint16_t width, uint16_t height, ctk_widget_t* children, uint16_t nr_children) {
    zero_widget(widget);
    widget->type = CTK_WIDGET_WINDOW;
    BIT_SET(widget->flags, CTK_FLAG_VISIBLE);
    BIT_SET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_SET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->width = width;
    widget->height = height;
    widget->nr_children = nr_children;
    widget->children = children;
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        widget->children[i].parent = widget;
    }
    return 1;
}

uint8_t ctk_void_init(ctk_widget_t* widget, uint16_t width, uint16_t height) {
    zero_widget(widget);
    widget->type = CTK_WIDGET_VOID;
    BIT_UNSET(widget->flags, CTK_FLAG_VISIBLE);
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_X);
    BIT_UNSET(widget->flags, CTK_FLAG_EXPAND_Y);
    widget->width = width;
    widget->height = height;
    return 1;
}

uint8_t ctk_hbox_init(ctk_widget_t* widget, ctk_widget_t* children, uint16_t nr_children) {
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

static void trigger_hotkey(ctk_ctx_t* ctx, ctk_widget_t* widget, char hotkey) {
    if (widget->hotkey == hotkey) {
        //mvwprintw(ctx->win, 11, 10, "%c", hotkey);
    }
    for (uint16_t i = 0; i < widget->nr_children; i++) {
        trigger_hotkey(ctx, &widget->children[i], hotkey);
    }
}

uint8_t ctk_init_widgets(ctk_ctx_t* ctx, ctk_widget_t* widgets, uint16_t nr_widgets) {
    int x, y;
    getmaxyx(ctx->win, y, x);
    ctk_window_init(&ctx->mainwin, x, y, widgets, nr_widgets);
    p1_layout_widget(ctx, &ctx->mainwin);
    ctx->mainwin.width = x;
    ctx->mainwin.height = y;
    p2_expand_widget(ctx, &ctx->mainwin);
    return 1;
}

uint8_t ctk_init_ctx(ctk_ctx_t* ctx, WINDOW* target) {
    ctx->win = target;
    wbkgd(ctx->win, COLOR_PAIR(CTK_COLOR_WINDOW));
    return 1;
}

#ifdef CTK_GPM
uint8_t ctk_init_gpm(ctk_ctx_t* ctx) {
    int c;
    ctx->gpm.eventMask = ~0;
    ctx->gpm.defaultMask = 0;
    ctx->gpm.minMod = 0;
    ctx->gpm.maxMod = ~0;
        
    if (Gpm_Open(&ctx->gpm, 0) == -1) {
        printf("Cannot connect to mouse server\n");
    }
    gpm_handler = my_handler;
    Gpm_Close();
    return 0;
}
#endif

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
    refresh_view(ctx);
    while (1) {
        c = getch();
        mvwprintw(ctx->win, 10, 10, "%s %d", keyname(c), c);
        if (ERR == c) {
            continue;
        }
        switch (c) {
            case 6:
                trigger_hotkey(ctx, &ctx->mainwin, 'F');
                refresh_view(ctx);
                break;
            default:
                break;
        }
    }
}

//    if (c > 0 && ) {
//    }

void ctk_end(ctk_ctx_t* ctx) {
    cleanup();
    endwin();
}
