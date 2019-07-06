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
    for (uint16_t y = 0; y < widget->height; y++) {
        for (uint16_t x = 0; x < widget->width; x++) {
            ctk_addstr(widget, x, y, 1, str);
        }
    }
    return 1;
}

int main(int argc, char** argv) {
    ctk_ctx_t ctx;
    ctk_widget_t top_right_hbox_contents[2];
    ctk_widget_t top_right_hbox_and_rb[2];

    ctk_init_area(&top_right_hbox_contents[0], 10, 10, 1, 1);
    ctk_init_area(&top_right_hbox_contents[1], 10, 10, 1, 1);
    ctk_init_hbox(&top_right_hbox_and_rb[0], &top_right_hbox_contents[0], 2);
    ctk_init_area(&top_right_hbox_and_rb[1], 10, 1, 1, 0);

    ctk_widget_t left_area_and_right_vbox[2];
    ctk_init_area(&left_area_and_right_vbox[0], 30, 10, 0, 1);
    ctk_init_vbox(&left_area_and_right_vbox[1], &top_right_hbox_and_rb[0], 2);

    ctk_widget_t main_hbox;
    ctk_init_hbox(&main_hbox, &left_area_and_right_vbox[0], 2);

    top_right_hbox_contents[0].event_callback = draw_win_l;
    top_right_hbox_contents[0].user_data = "l";

    top_right_hbox_contents[1].event_callback = draw_win_l;
    top_right_hbox_contents[1].user_data = "r";

    top_right_hbox_and_rb[1].event_callback = draw_win_l;
    top_right_hbox_and_rb[1].user_data = "b";

    left_area_and_right_vbox[0].event_callback = draw_win_l;
    left_area_and_right_vbox[0].user_data = ".";

    ctk_init(&ctx, &main_hbox, 1);
    ctk_main_loop(&ctx);
    ctk_end(&ctx);

    return 0;
}
