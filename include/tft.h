#pragma once
#include "wholeinclude.h"
#include <array>

#define X_Resolution 480
#define Y_Resolution 320

namespace tft_wrapper
{
    // std::array<std::byte, X_Resolution> pixel_mask_x;
    std::vector<std::vector<char>> pixel_mask(Y_Resolution, std::vector<char>(X_Resolution, '1'));

    inline void init(void *param)
    {
    }

    static void btn_event_cb(lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *btn = lv_event_get_current_target_obj(e);
        if (code == LV_EVENT_CLICKED)
        {
            static uint8_t cnt = 0;
            cnt++;

            /*Get the first child of the button which is the label and change its text*/
            lv_obj_t *label = lv_obj_get_child(btn, 0);
            lv_label_set_text_fmt(label, "Button: %d", cnt);
        }
    }

    void main_menu(void)
    {
        lv_obj_t *btn = lv_button_create(lv_screen_active());       /*Add a button the current screen*/
        lv_obj_set_pos(btn, 10, 10);                                /*Set its position*/
        lv_obj_set_size(btn, 120, 50);                              /*Set its size*/
        lv_obj_add_event(btn, btn_event_cb, LV_EVENT_ALL, NULL);    /*Assign a callback to the button*/

        lv_obj_t *label = lv_label_create(btn); /*Add a label to the button*/
        lv_label_set_text(label, "Button");     /*Set the labels text*/
        lv_obj_center(label);
        
    }

}