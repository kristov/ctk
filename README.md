# CTK - Curses ToolKit

Hints of GTK. Features:

* No malloc in the code, handle memory yourself.
* Use the library per ncurses window or attach it to stdscr, you decide.

## Synopsis

    #include "ctk.h"

    int main(int argc, char** argv) {
        ctk_ctx_t ctx;
        ctk_widget_t file_menu_items[2];
        ctk_widget_t help_menu_items[1];
        ctk_widget_t menus[2];

        ctk_widget_t menu_bar;

        ctk_menu_item_init(&file_menu_items[0], 'L', "Load");
        ctk_menu_item_init(&file_menu_items[1], 'Q', "Quit");

        ctk_menu_item_init(&help_menu_items[1], 0, "About");

        ctk_menu_init(&menus[0], 'F', "File", &file_menu_items[0], 2);
        ctk_menu_init(&menus[1], 'H', "Help", &help_menu_items[0], 1);
        ctk_menu_bar_init(&menu_bar, &menus[0], 2);

        ctk_init(ctx, &menu_bar, 1);
        ctk_main_loop(ctx);
        ctk_end(ctx);

        return 0;
    }

## Initialization

    ctk_ctx_t ctx;
    ctk_init(&ctx, &main_widget, 1);
    ctk_main_loop(&ctx);

The `ctx_init()` call can be replaced with the following:

    ctk_ctx_t ctx;
    ctk_init_curses();
    WINDOW* window = newwin(20, 40, 5, 5);
    ctk_init_ctx(&ctx, window);
    ctk_init_widgets(&ctx, &main_widget, 1);
    ctk_main_loop(&ctx);
    ctk_end(ctx);

Meaning you can initialize curses outside ctk and pass whatever window you want. For example you could have only part of your application managed by ctk.
