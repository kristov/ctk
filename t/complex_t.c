#include "../ctk.h"

#define WIN_RL 0
#define WIN_RR 1
#define RIGHT_HBOX 2
#define WIN_RB 3
#define WIN_L 4
#define RIGHT_VBOX 5
#define MAIN_HBOX 6

// +--------------+--------+--------+
// |              |        |        |
// |   WIN_L      | WIN_RL | WIN_RR |
// |              |        |        |
// |              |        |        |
// |              |        |        |
// |              +--------+--------+
// |              |   WIN_RB        |
// +--------------+-----------------+

uint8_t draw_win_l(ctk_event_t* event, void* user_data) {
    if (event->type != CTK_EVENT_DRAW) {
        return 1;
    }
    char* str = (char*)user_data;
    ctk_widget_t* widget = event->widget;
    for (uint16_t y = 0; y < widget->width; y++) {
        for (uint16_t x = 0; x < widget->height; x++) {
            ctk_addstr(widget, x, y, 1, str);
        }
    }
    return 1;
}

int main(int argc, char** argv) {
    ctk_ctx_t ctx;
    ctk_widget_t widgets[7];

    ctk_void_init(&widgets[WIN_RL], 10, 20);
    ctk_window_init(&widgets[WIN_RR], NULL, 0);
    ctk_hbox_init(&widgets[RIGHT_HBOX], &widgets[WIN_RL], 2);
    ctk_void_init(&widgets[WIN_RB], 20, 1);
    ctk_vbox_init(&widgets[RIGHT_VBOX], &widgets[RIGHT_HBOX], 2);
    ctk_void_init(&widgets[WIN_L], 20, 20);
    ctk_hbox_init(&widgets[MAIN_HBOX], &widgets[WIN_L], 2);

    widgets[WIN_L].event_callback = draw_win_l;
    widgets[WIN_L].user_data = "l";

    widgets[WIN_RL].event_callback = draw_win_l;
    widgets[WIN_RL].user_data = "x";

    widgets[WIN_RB].event_callback = draw_win_l;
    widgets[WIN_RB].user_data = ".";

    ctk_init(&ctx, &widgets[MAIN_HBOX], 1);
    ctk_main_loop(&ctx);
    ctk_end(&ctx);

    return 0;
}
