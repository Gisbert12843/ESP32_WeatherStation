#pragma once

#include "lvgl.h"
#include "display_driver.h"

namespace ui
{
#define display_height DISPLAY_VERTICAL_PIXELS
#define display_width DISPLAY_HORIZONTAL_PIXELS

#define statusbar_height display_height * 0.05
#define graphbar_height display_height * 0.25
#define warnings_box_height display_height * 0.5
#define warnings_box_width display_width * 0.40

    inline int x12 = 0;

    void tick_task(void *param)
    {
        std::cout << uxTaskGetStackHighWaterMark2(NULL) << "\n";
        std::cout << esp_get_free_heap_size() << "\n";
        while (1)
        {
            if (x12 == 100)
            {
                x12 = 0;
                std::cout << uxTaskGetStackHighWaterMark2(NULL) << "\n";
                std::cout << esp_get_free_heap_size() << "\n";

            }
            lv_timer_handler();
            x12++;

            vTaskDelay(pdMS_TO_TICKS(20));
        }
        vTaskDelete(NULL);
    }

    void main_menu()
    {
        lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(255, 255, 255), LV_STATE_DEFAULT);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Statusbar
        {
            static lv_obj_t *statusbar = lv_obj_create(lv_scr_act());
            lv_obj_set_size(statusbar, DISPLAY_HORIZONTAL_PIXELS, statusbar_height);
            lv_obj_set_pos(statusbar, 0, 0);

            static lv_style_t statusbar_style;
            lv_style_init(&statusbar_style);

            lv_style_set_pad_all(&statusbar_style, 0);                         // No padding
            lv_style_set_border_width(&statusbar_style, 0);                    // No border
            lv_style_set_bg_color(&statusbar_style, lv_color_make(255, 0, 0)); // Red background
            lv_style_set_outline_width(&statusbar_style, 0);                   // No outline
            lv_style_set_radius(&statusbar_style, 0);                          // No rounded corners, this is the key
            lv_style_set_bg_opa(&statusbar_style, LV_OPA_COVER);               // Fully opaque

            lv_obj_add_style(statusbar, &statusbar_style, 0);
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Graphen
        {

            static lv_obj_t *graphbar = lv_obj_create(lv_scr_act());
            lv_obj_set_size(graphbar, DISPLAY_HORIZONTAL_PIXELS, graphbar_height);
            lv_obj_set_pos(graphbar, 0, DISPLAY_VERTICAL_PIXELS - graphbar_height);

            static lv_style_t graphbar_style;
            lv_style_init(&graphbar_style);

            lv_style_set_pad_all(&graphbar_style, 0);                         // No padding
            lv_style_set_border_width(&graphbar_style, 0);                    // No border
            lv_style_set_bg_color(&graphbar_style, lv_color_make(255, 0, 0)); // Red background
            lv_style_set_outline_width(&graphbar_style, 0);                   // No outline
            lv_style_set_radius(&graphbar_style, 0);                          // No rounded corners, this is the key
            lv_style_set_bg_opa(&graphbar_style, LV_OPA_COVER);               // Fully opaque

            lv_obj_add_style(graphbar, &graphbar_style, 0);
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // tägliche Warnungen
        {

            static lv_obj_t *warning_box = lv_obj_create(lv_scr_act());
            lv_obj_set_size(warning_box, warnings_box_width, warnings_box_height);
            lv_obj_set_pos(warning_box, display_width * 0.55, statusbar_height + ((display_height - (statusbar_height + graphbar_height)) - warnings_box_height) * 0.5);

            static lv_style_t warning_box_style;
            lv_style_init(&warning_box_style);

            lv_style_set_pad_all(&warning_box_style, 0);
            // lv_style_set_pad_ver(&warning_box_style, display_h);
            lv_style_set_border_width(&warning_box_style, 2);                  // No border
            lv_style_set_bg_color(&warning_box_style, lv_color_make(0, 0, 0)); // Red background
            lv_style_set_outline_width(&warning_box_style, 0);                 // No outline
            lv_style_set_radius(&warning_box_style, 0);                        // No rounded corners, this is the key
            lv_style_set_bg_opa(&warning_box_style, LV_OPA_0);                 // Fully opaque

            lv_obj_add_style(warning_box, &warning_box_style, 0);

            ///////////////

            // static lv_obj_t *thunder_box = lv_obj_create(warning_box);
            // lv_obj_set_size(thunder_box, warnings_box_width * 0.2, warnings_box_height * 0.2);

            // static lv_style_t thunder_box_style;
            // lv_style_init(&thunder_box_style);

            // lv_style_set_pad_all(&thunder_box_style, 0);
            // // lv_style_set_pad_ver(&warning_box_style, display_h);
            // lv_style_set_border_width(&thunder_box_style, 2);                  // No border
            // lv_style_set_bg_color(&thunder_box_style, lv_color_make(0, 0, 0)); // Red background
            // lv_style_set_outline_width(&thunder_box_style, 0);                 // No outline
            // lv_style_set_radius(&thunder_box_style, 0);                        // No rounded corners, this is the key
            // lv_style_set_bg_opa(&thunder_box_style, LV_OPA_0);                 // Fully opaque

            // lv_obj_add_style(thunder_box, &thunder_box_style, 0);
        }
    }
}