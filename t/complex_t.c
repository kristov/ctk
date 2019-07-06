#include "../ctk.h"

#define AREA_RL 0
#define AREA_RR 1
#define RIGHT_HBOX 2
#define AREA_RB 3
#define AREA_L 4
#define RIGHT_VBOX 5
#define MAIN_HBOX 6

// +--------------+---------+---------+
// |              |         |         |
// |   AREA_L     | AREA_RL | AREA_RR |
// |              |         |         |
// |              |         |         |
// |              |         |         |
// |              +---------+---------+
// |              |      AREA_RB      |
// +--------------+-------------------+

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

    ctk_init_area(&widgets[AREA_RL], 10, 10, 1, 1);
    ctk_init_area(&widgets[AREA_RR], 10, 10, 1, 1);
    ctk_init_hbox(&widgets[RIGHT_HBOX], &widgets[AREA_RL], 2);
    ctk_init_area(&widgets[AREA_RB], 20, 1, 1, 0);
    ctk_init_vbox(&widgets[RIGHT_VBOX], &widgets[RIGHT_HBOX], 2);
    ctk_init_area(&widgets[AREA_L], 20, 20, 1, 1);
    ctk_init_hbox(&widgets[MAIN_HBOX], &widgets[AREA_L], 2);

    widgets[AREA_L].event_callback = draw_win_l;
    widgets[AREA_L].user_data = "l";

    widgets[AREA_RL].event_callback = draw_win_l;
    widgets[AREA_RL].user_data = "x";

    widgets[AREA_RB].event_callback = draw_win_l;
    widgets[AREA_RB].user_data = ".";

    ctk_init(&ctx, &widgets[MAIN_HBOX], 1);
    ctk_main_loop(&ctx);
    ctk_end(&ctx);

    return 0;
}
