#include "../ctk.h"

int main(int argc, char** argv) {
    ctk_ctx_t ctx;
    ctk_widget_t hbox;
    ctk_widget_t menu;
    ctk_widget_t widgets[3];

    ctk_init_menu(&menu, 'T', "There should be 10 spaces on either side of this menu", NULL, 0);
    ctk_init_area(&widgets[0], 10, 10, 0, 0);
    ctk_init_menu_bar(&widgets[1], &menu, 1);
    ctk_init_area(&widgets[2], 10, 10, 0, 0);

    ctk_init_hbox(&hbox, &widgets[0], 3);

    ctk_init(&ctx, &hbox, 1);
    ctk_main_loop(&ctx);
    ctk_end(&ctx);

    return 0;
}
