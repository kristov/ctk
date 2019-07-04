#include "../ctk.h"

int main(int argc, char** argv) {
    ctk_ctx_t ctx;
    ctk_widget_t file_menu_items[1];
    ctk_widget_t menus[1];

    ctk_widget_t menu_bar;

    ctk_menu_item_init(&file_menu_items[0], 'Q', "Quit");
    ctk_menu_init(&menus[0], 'F', "File", &file_menu_items[0], 1);
    ctk_menu_bar_init(&menu_bar, &menus[0], 1);

    ctk_init_curses();
    WINDOW* window = newwin(20, 40, 5, 5);
    ctk_init_ctx(&ctx, window);
    ctk_init_widgets(&ctx, &menu_bar, 1);
    ctk_main_loop(&ctx);
    ctk_end(&ctx);

    return 0;
}
