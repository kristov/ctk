#include "../ctk.h"

int main(int argc, char** argv) {
    ctk_ctx_t ctx;
    ctk_widget_t file_menu_items[2];
    ctk_widget_t view_menu_items[2];
    ctk_widget_t help_menu_items[2];
    ctk_widget_t menus[3];

    ctk_widget_t menu_bar;

    ctk_init_menu_item(&file_menu_items[0], 'L', "Load ROM");
    ctk_init_menu_item(&file_menu_items[1], 'Q', "Quit");

    ctk_init_menu_item(&view_menu_items[0], 'R', "ROM view");
    ctk_init_menu_item(&view_menu_items[1], 'M', "Memory view");

    ctk_init_menu_item(&help_menu_items[0], 'L', "Load ROM");
    ctk_init_menu_item(&help_menu_items[1], 0, "About");

    ctk_init_menu(&menus[0], 'F', "File", &file_menu_items[0], 2);
    ctk_init_menu(&menus[1], 'V', "View", &view_menu_items[0], 2);
    ctk_init_menu(&menus[2], 'H', "Help", &help_menu_items[0], 2);
    ctk_init_menu_bar(&menu_bar, &menus[0], 3);

    ctk_init(&ctx, &menu_bar, 1);
    ctk_main_loop(&ctx);
    ctk_end(&ctx);

    return 0;
}
