#include "../ctk.h"

uint8_t draw_area(ctk_event_t* event, void* user_data) {
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
    ctk_widget_t area;

    ctk_init_area(&area, 5, 5, 0, 1);
    area.event_callback = draw_area;
    area.user_data = "#";

    ctk_init(&ctx, &area, 1);
    ctk_main_loop(&ctx);
    ctk_end(&ctx);

    return 0;
}
