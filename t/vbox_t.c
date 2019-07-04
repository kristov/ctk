#include "../ctk.h"

#define WIN_T 0
#define WIN_B 1
#define VBOX 2

// +--------------+
// |              |
// |   WIN_T      |
// |              |
// +--------------+
// |              |
// |   WIN_B      +
// |              |
// +--------------+

uint16_t c = 0;
uint8_t draw_win_l(ctk_event_t* event, void* user_data) {
    if (event->type != CTK_EVENT_DRAW) {
        return 1;
    }
    char* str = (char*)user_data;
    ctk_widget_t* widget = event->widget;
//ctk_printf(widget, 0, c++, 1, "%s [%dx%d] at [%dx%d]", str, widget->width, widget->height, widget->x, widget->y);
    for (uint16_t y = 0; y < widget->width; y++) {
        for (uint16_t x = 0; x < widget->height; x++) {
            ctk_addstr(widget, x, y, 1, str);
        }
    }
    return 1;
}

int main(int argc, char** argv) {
    ctk_ctx_t ctx;
    ctk_widget_t widgets[3];

    ctk_window_init(&widgets[WIN_T], NULL, 0);
    ctk_window_init(&widgets[WIN_B], NULL, 0);
    ctk_vbox_init(&widgets[VBOX], &widgets[WIN_T], 2);

    widgets[WIN_T].event_callback = draw_win_l;
    widgets[WIN_T].user_data = "T";

    widgets[WIN_B].event_callback = draw_win_l;
    widgets[WIN_B].user_data = "B";

    ctk_init(&ctx, &widgets[VBOX], 1);
    ctk_main_loop(&ctx);
    ctk_end(&ctx);

    return 0;
}
