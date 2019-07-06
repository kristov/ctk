#include "../ctk.h"

#define AREA_T 0
#define AREA_B 1
#define VBOX 2

// +--------------+
// |              |
// |   AREA_T     |
// |              |
// +--------------+
// |              |
// |   AREA_B     +
// |              |
// +--------------+

uint16_t c = 0;
uint8_t draw_win_l(ctk_event_t* event, void* user_data) {
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
    ctk_widget_t widgets[3];

    ctk_init_area(&widgets[AREA_T], 5, 5, 1, 1);
    ctk_init_area(&widgets[AREA_B], 5, 5, 1, 1);
    ctk_init_vbox(&widgets[VBOX], &widgets[AREA_T], 2);

    widgets[AREA_T].event_callback = draw_win_l;
    widgets[AREA_T].user_data = "T";

    widgets[AREA_B].event_callback = draw_win_l;
    widgets[AREA_B].user_data = "B";

    ctk_init(&ctx, &widgets[VBOX], 1);
    ctk_main_loop(&ctx);
    ctk_end(&ctx);

    return 0;
}
