#include "../ctk.h"

uint16_t c = 0;
uint8_t fill_area(ctk_event_t* event, void* user_data) {
    if (event->type != CTK_EVENT_DRAW) {
        return 1;
    }
    char* str = (char*)user_data;
    ctk_widget_t* widget = event->widget;
    for (uint16_t y = 0; y < widget->height; y++) {
        for (uint16_t x = 0; x < widget->width; x++) {
            ctk_addstr(widget, x, y, 1, str);
        }
    }
    return 1;
}

int main(int argc, char** argv) {
    ctk_ctx_t ctx;
    ctk_widget_t hbox;
    ctk_widget_t widgets[3];

    ctk_init_area(&widgets[0], 10, 10, 0, 1);
    ctk_init_area(&widgets[1], 10, 10, 1, 1);
    ctk_init_area(&widgets[2], 10, 10, 0, 1);

    widgets[0].event_callback = fill_area;
    widgets[0].user_data = "L";

    widgets[1].event_callback = fill_area;
    widgets[1].user_data = "M";

    widgets[2].event_callback = fill_area;
    widgets[2].user_data = "R";

    ctk_init_hbox(&hbox, &widgets[0], 3);

    ctk_init(&ctx, &hbox, 1);

    ctx.mainwin.event_callback = fill_area;
    ctx.mainwin.user_data = ".";

    ctk_main_loop(&ctx);
    ctk_end(&ctx);

    return 0;
}
